// =============================================================================
// Step Buddy 3.0 | Physical student device (micro:bit + Elecfreaks WuKong)
// MakeCode micro:bit -> JavaScript (Static TypeScript). No third-party extensions needed.
//
// Designed as a classroom support tool, NOT a detector of attention/ADHD.
// Student: A = finished; B = help / press B again to cancel; A+B = break.
// During a movement-break offer: A = start, B = later.
// During a break: A = return early; A+B also ends the break.
//
// Hardware wiring (updated):
// ESP8266 D5 (RX) <- WuKong 14S (TX) | ESP8266 D6 (TX) -> WuKong 15S (RX) | GND common
// External Buzzer -> WuKong 13S / 13V / 13G (P13)
// Sonar:bit       -> WuKong 8S / 8V / 8G (P8, seat presence / AWAY state)
// Pointer Servo   -> WuKong S0S / S0V / S0G (S0, I2C 0x10)
// =============================================================================

const POINTER_START = 0
const POINTER_STEP = 30
const TOKENS_PER_REWARD = 5
const HINT_MS = 5000
const BRIGHT_NORMAL = 155
const BRIGHT_LOW = 35
const SOUND_ON = true
// All sound uses the external buzzer connected to WuKong 13S (P13).

const LOW_SENSORY_DEFAULT = true
const TIMER_DEFAULT = false // optional, some learners find a countdown stressful

// Sonar:bit seat-presence settings (WuKong 8S / P8).
const SONAR_ENABLED = true
const SONAR_SAMPLE_MS = 500
const AWAY_DIST_CM = 40
const AWAY_CONFIRM_MS = 3000
const RETURN_CONFIRM_MS = 1500

let tplNames: string[] = ["Homework", "Class", "Reading"]
let tplSteps: string[][] = [
    ["Read one question", "Find key words", "Try first answer", "Do next answer", "Check one answer", "Pack up"],
    ["Hear today's goal", "Watch one example", "Try one item", "Do two more", "Share an answer", "Tidy up"],
    ["Pick a page", "Read one page", "Find key idea", "Tell or write", "Check together"]
]
let tplMins: number[][] = [[2, 2, 4, 5, 3, 2], [3, 5, 4, 7, 3, 2], [2, 5, 3, 4, 3]]

// Keep the order identical to the teacher dashboard HINT_LABELS.
let hintIcons = [
    IconNames.Target,    // Start with one small part
    IconNames.Confused,  // Read the instructions again
    IconNames.Tortoise,  // Take your time
    IconNames.Yes,       // Try one example
    IconNames.Happy,     // You are making progress
    IconNames.Heart      // I'm coming to help
]

// Settings adjustable live by teacher, in seconds.
let moveIntervalSec = 900     // offers, never compulsory (0 disables)
let moveDurationSec = 60
let restDurationSec = 180
let timerView = TIMER_DEFAULT
let lowSensory = LOW_SENSORY_DEFAULT
let teacherLed = false

// Session state.
let tpl = 0
let step = 0
let stepBudgetSec = 180
let finished = false
let paused = false
let away = false
let sonarSeenValid = false
let awaySince = 0
let backSince = 0
let lastSonarAt = 0
let waitingTeacher = false
let helpActive = false
let helpAcknowledged = false
let resting = false
let restLeft = 0
let restStartedMs = 180000
let moving = false
let moveLeft = 0
let moveOffer = false
let moveOfferUntil = 0
let workSinceMove = 0
let workMs = 0 // elapsed task time, NOT measured attention
let stepMs = 0
let tokens = 0
let completed: number[] = []
let hintIdx = 0
let hintUntil = 0
let teacherAckUntil = 0
let localAckUntil = 0
let rewardUntil = 0
let tokenBarUntil = 0
let lastDoneAt = -10000
let stepsDone = 0
let hintsSent = 0
let helpCount = 0
let restCount = 0
let moveCount = 0
let overtimeSent = false
let pressA = false
let pressB = false
let pressAB = false
let cmds: string[] = []

function nSteps(): number { return tplSteps[tpl].length }
function budgetMs(): number { return stepBudgetSec * 1000 }
function sendEvent(name: string, detail: string) {
    serial.writeLine("E|" + name + "|" + (step + 1) + "|" + detail)
}
function beep(note: number) {
    if (SOUND_ON && !lowSensory) music.playTone(note, music.beat(BeatFraction.Sixteenth))
}
function setServo(angle: number) {
    let buf = pins.createBuffer(4)
    buf[0] = 0x03 // WuKong S0 on board I2C 0x10 (as in existing prototype)
    buf[1] = Math.max(0, Math.min(180, angle))
    buf[2] = 0
    buf[3] = 0
    pins.i2cWriteBuffer(0x10, buf)
}
function movePointer() {
    let s = Math.min(step, nSteps() - 1)
    setServo(POINTER_START + s * POINTER_STEP)
}
function readSonarCm(): number {
    pins.digitalWritePin(DigitalPin.P8, 0)
    control.waitMicros(2)
    pins.digitalWritePin(DigitalPin.P8, 1)
    control.waitMicros(10)
    pins.digitalWritePin(DigitalPin.P8, 0)
    let duration = pins.pulseIn(DigitalPin.P8, PulseValue.High, 25000)
    if (duration <= 0) return -1
    return Math.idiv(duration * 34, 2000)
}
function updateSeatPresence(now: number) {
    if (!SONAR_ENABLED) return
    if (paused || finished || resting || moving) {
        awaySince = 0
        backSince = 0
        return
    }
    let dist = readSonarCm()
    if (dist > 0 && dist <= AWAY_DIST_CM) {
        sonarSeenValid = true
        awaySince = 0
        if (away) {
            if (backSince == 0) backSince = now
            else if (now - backSince >= RETURN_CONFIRM_MS) {
                away = false
                backSince = 0
                sendEvent("away_end", "" + dist)
            }
        }
    } else if (sonarSeenValid) {
        backSince = 0
        if (!away) {
            if (awaySince == 0) awaySince = now
            else if (now - awaySince >= AWAY_CONFIRM_MS) {
                away = true
                awaySince = 0
                sendEvent("away_start", dist > 0 ? "" + dist : "out_of_range")
            }
        }
    }
}
function defaultStepBudget() {
    stepBudgetSec = tplMins[tpl][Math.min(step, nSteps() - 1)] * 60
}
function newSession() {
    step = 0
    finished = false
    paused = false
    away = false
    awaySince = 0
    backSince = 0
    waitingTeacher = false
    helpActive = false
    helpAcknowledged = false
    resting = false
    restLeft = 0
    restStartedMs = restDurationSec * 1000
    moving = false
    moveLeft = 0
    moveOffer = false
    moveOfferUntil = 0
    workSinceMove = 0
    workMs = 0
    stepMs = 0
    tokens = 0
    completed = []
    for (let i = 0; i < nSteps(); i++) completed.push(0)
    hintUntil = 0
    teacherAckUntil = 0
    localAckUntil = 0
    rewardUntil = 0
    tokenBarUntil = 0
    lastDoneAt = -10000
    stepsDone = 0
    hintsSent = 0
    helpCount = 0
    restCount = 0
    moveCount = 0
    overtimeSent = false
    defaultStepBudget()
}

function completeStep(byTeacher: boolean) {
    if (paused || finished || resting || moving) return
    let now = input.runningTime()
    away = false
    awaySince = 0
    backSince = 0
    waitingTeacher = false
    helpActive = false
    helpAcknowledged = false
    stepMs = 0
    overtimeSent = false
    lastDoneAt = now
    // Revisiting a previously completed step does not create extra rewards.
    if (completed[step] == 0) {
        completed[step] = 1
        stepsDone += 1
        tokens += 1
        if (tokens % TOKENS_PER_REWARD == 0) {
            rewardUntil = now + 2200
            if (!lowSensory) music.startMelody(music.builtInMelody(Melodies.PowerUp), MelodyOptions.Once)
        } else {
            tokenBarUntil = now + 1200
            beep(Note.C5)
        }
    }
    sendEvent("step_done", byTeacher ? "teacher" : "student")
    step += 1
    if (step >= nSteps()) {
        finished = true
        sendEvent("finished", "")
    } else {
        defaultStepBudget()
    }
    movePointer()
}
function studentDone() {
    if (resting) { endRest("student"); return }
    if (moving) { endMove("student"); return }
    if (paused || finished) return
    if (input.runningTime() - lastDoneAt < 1500) return
    if (teacherLed) {
        if (!waitingTeacher) {
            waitingTeacher = true
            sendEvent("done_wait", "")
        }
    } else completeStep(false)
}
function requestHelp() {
    if (finished) return
    localAckUntil = input.runningTime() + 1000
    if (!helpActive) {
        helpActive = true
        helpAcknowledged = false
        helpCount += 1
        sendEvent("help", "")
    }
}
// Press physical B again to withdraw a help request.
function cancelHelp() {
    if (!helpActive) return
    helpActive = false
    helpAcknowledged = false
    localAckUntil = input.runningTime() + 800
    sendEvent("help_cancel", "student")
}
function acknowledgeHelp() {
    if (!helpActive) return
    helpAcknowledged = true
    teacherAckUntil = input.runningTime() + 2400
    sendEvent("help_ack", "teacher")
}
function resolveHelp() {
    if (!helpActive) return
    helpActive = false
    helpAcknowledged = false
    sendEvent("help_resolved", "teacher")
}
function startRest(who: string) {
    if (paused || finished || resting) return
    if (moving) endMove("rest")
    away = false
    awaySince = 0
    backSince = 0
    moveOffer = false
    moveOfferUntil = 0
    resting = true
    workSinceMove = 0 // voluntary rest also resets scheduled movement prompt
    restStartedMs = restDurationSec * 1000
    restLeft = restStartedMs
    restCount += 1
    sendEvent("rest_start", who)
}
function endRest(why: string) {
    if (!resting) return
    resting = false
    restLeft = 0
    sendEvent("rest_end", why)
}
function offerMove(why: string) {
    if (moveIntervalSec == 0 || moveOffer || moving || resting || paused || finished || away) return
    moveOffer = true
    moveOfferUntil = input.runningTime() + 20000 // offer expires gently if ignored
    sendEvent("move_offer", why)
    beep(Note.E5)
}
function startMove(who: string) {
    if (paused || finished || resting || moving) return
    away = false
    awaySince = 0
    backSince = 0
    moving = true
    moveOffer = false
    moveOfferUntil = 0
    moveLeft = moveDurationSec * 1000
    moveCount += 1
    sendEvent("move_start", who)
}
function endMove(why: string) {
    if (!moving) return
    moving = false
    moveLeft = 0
    workSinceMove = 0
    sendEvent("move_end", why)
}
function moveLater(why: string) {
    if (!moveOffer) return
    moveOffer = false
    moveOfferUntil = 0
    workSinceMove = Math.max(0, moveIntervalSec * 1000 - 300000)
    sendEvent("move_later", why)
}

// Reject malformed numeric commands. For example STEPSEC:300, MOVEINT:900.
function numericCommand(c: string, prefix: string, lo: number, hi: number): number {
    if (c.indexOf(prefix) != 0) return -1
    let raw = c.substr(prefix.length)
    if (raw.length == 0 || raw.length > 5) return -1
    for (let i = 0; i < raw.length; i++) {
        let k = raw.charCodeAt(i)
        if (k < 48 || k > 57) return -1
    }
    let v = parseInt(raw)
    return v >= lo && v <= hi ? v : -1
}
function handleCommand(c: string) {
    let value = -1
    if (c == "NEXT") completeStep(true)
    else if (c == "BACK") {
        if (step > 0 && !resting && !moving) {
            step -= 1
            finished = false
            stepMs = 0
            overtimeSent = false
            waitingTeacher = false
            defaultStepBudget()
            movePointer()
            sendEvent("step_back", "")
        }
    } else if (c == "PAUSE") {
        paused = !paused
        sendEvent(paused ? "pause" : "resume", "")
    } else if (c == "MODE") {
        teacherLed = !teacherLed
        waitingTeacher = false
        sendEvent("mode", teacherLed ? "teacher" : "student")
    } else if (c == "LOW") {
        lowSensory = !lowSensory
        sendEvent("low_sensory", lowSensory ? "on" : "off")
    } else if (c == "TIMER") {
        timerView = !timerView
        sendEvent("step_timer", timerView ? "on" : "off")
    } else if (c == "ACK") acknowledgeHelp()
    else if (c == "HELPDONE") resolveHelp()
    else if (c == "RESTEND") endRest("teacher")
    else if (c == "MOVE") {
        if (moving) endMove("teacher")
        else startMove("teacher")
    } else if (c == "PLUS120") {
        if (!finished) {
            stepBudgetSec = Math.min(3600, stepBudgetSec + 120)
            overtimeSent = false
            sendEvent("time_extended", "120")
        }
    } else if (c == "NEW") {
        newSession()
        movePointer()
        sendEvent("new_session", "")
    } else if (c.indexOf("TPL") == 0) {
        value = numericCommand(c, "TPL", 0, tplNames.length - 1)
        if (value >= 0) {
            tpl = value
            newSession()
            movePointer()
            sendEvent("template", tplNames[tpl])
        }
    } else if (c.indexOf("HINT") == 0) {
        value = numericCommand(c, "HINT", 0, hintIcons.length - 1)
        if (value >= 0) {
            hintIdx = value
            hintUntil = input.runningTime() + HINT_MS
            hintsSent += 1
            beep(Note.E5)
            sendEvent("hint", "" + value)
        }
    } else if (c.indexOf("STEPSEC:") == 0) {
        value = numericCommand(c, "STEPSEC:", 60, 3600)
        if (value >= 0 && !finished) {
            stepBudgetSec = value
            overtimeSent = false
            sendEvent("step_budget", "" + value)
        }
    } else if (c.indexOf("MOVEINT:") == 0) {
        value = numericCommand(c, "MOVEINT:", 0, 3600)
        if (value >= 0) {
            moveIntervalSec = value
            if (value == 0) { moveOffer = false; moveOfferUntil = 0 }
            workSinceMove = 0
            sendEvent("move_interval", "" + value)
        }
    } else if (c.indexOf("RESTSEC:") == 0) {
        value = numericCommand(c, "RESTSEC:", 30, 900)
        if (value >= 0) {
            restDurationSec = value
            sendEvent("rest_duration", "" + value)
        }
    }
}

// LED output: gentle, predictable, no blinking 'failure' state.
const pauseImg = images.createImage(`
. # . # .
. # . # .
. # . # .
. # . # .
. # . # .`)
const awayImg = images.createImage(`
. . . . .
. # # # .
. # . # .
. # # # .
. . . . .`)
const waitImg = images.createImage(`
# # # # #
. # . # .
. . # . .
. # . # .
# # # # #`)
const moveImg = images.createImage(`
. . # . .
. # # # .
. . # . .
. # . # .
. # . # .`)
let lastView = ""
let lastLow = false
function currentView(now: number): string {
    if (now < hintUntil) return "hint" + hintIdx
    if (now < teacherAckUntil) return "teacherack"
    if (now < localAckUntil) return "sent"
    if (paused) return "pause"
    if (resting) return restLeft > 0 ? "rest" + Math.ceil(restLeft * 25 / restStartedMs) : "restover"
    if (moving) return "move"
    if (now < rewardUntil) return "reward"
    if (now < tokenBarUntil) return "tokens" + (tokens % TOKENS_PER_REWARD)
    if (finished) return "finished"
    if (away) return "away"
    if (moveOffer) return "offer"
    if (waitingTeacher) return "wait"
    // Calm alternation: the help symbol is visible even when the teacher's
    // optional mobile dashboard is unavailable. Never flash rapidly.
    if (helpActive && Math.idiv(now, 2500) % 2 == 0) return "needhelp"
    if (!timerView) return "step" + (step + 1)
    let phase = Math.idiv(now, 1000) % 7
    if (phase == 0) return "step" + (step + 1)
    if (stepMs >= budgetMs()) return "timeend" // soft, constant dot
    return "timer" + Math.ceil((budgetMs() - stepMs) * 25 / budgetMs())
}
function draw(view: string) {
    led.setBrightness(lowSensory ? BRIGHT_LOW : BRIGHT_NORMAL)
    basic.clearScreen()
    if (view == "pause") pauseImg.plotImage(0)
    else if (view == "away") awayImg.plotImage(0)
    else if (view.indexOf("rest") == 0 && view != "restover") {
        let n = parseInt(view.substr(4))
        for (let i = 0; i < n; i++) led.plot(i % 5, Math.idiv(i, 5))
    } else if (view == "restover") basic.showString("A")
    else if (view.indexOf("hint") == 0) basic.showIcon(hintIcons[parseInt(view.substr(4))], 0)
    else if (view == "teacherack") basic.showIcon(IconNames.Yes, 0)
    else if (view == "sent") basic.showIcon(IconNames.SmallDiamond, 0)
    else if (view == "reward") basic.showIcon(IconNames.Happy, 0)
    else if (view.indexOf("tokens") == 0) {
        let cols = parseInt(view.substr(6))
        for (let x = 0; x < cols; x++) for (let y = 0; y < 5; y++) led.plot(x, y)
    } else if (view == "finished") basic.showIcon(IconNames.Heart, 0)
    else if (view == "move" || view == "offer") moveImg.plotImage(0)
    else if (view == "wait") waitImg.plotImage(0)
    else if (view == "needhelp") basic.showIcon(IconNames.Confused, 0)
    else if (view == "timeend") led.plot(2, 2)
    else if (view.indexOf("timer") == 0) {
        let n = parseInt(view.substr(5))
        for (let i = 0; i < n; i++) led.plot(i % 5, 4 - Math.idiv(i, 5))
    } else if (view.indexOf("step") == 0) basic.showNumber(parseInt(view.substr(4)), 0)
}

// Fixed-position serial protocol. The ESP8266 parses these fields in the same order.
// S|templateId|template|step|total|current|next|mode|state|tokens|help|
//   acknowledged|restLeft|hints|helpCount|restCount|moveCount|workSec|stepSec|
//   low|stepsDone|timer|moveLeft|expectedSec|moveIntervalSec|restDurationSec
function sendStatus() {
    let state = "RUN"
    if (finished) state = "DONE"
    else if (paused) state = "PAUSE"
    else if (resting) state = restLeft > 0 ? "REST" : "RESTOVER"
    else if (moving) state = "MOVE"
    else if (away) state = "AWAY"
    else if (moveOffer) state = "OFFER"
    else if (waitingTeacher) state = "WAIT"
    else if (timerView && stepMs >= budgetMs()) state = "TIME"
    let cur = finished ? "-" : tplSteps[tpl][step]
    let next = step + 1 < nSteps() ? tplSteps[tpl][step + 1] : "-"
    serial.writeLine("S|" + tpl + "|" + tplNames[tpl] + "|" + Math.min(step + 1, nSteps()) + "|" + nSteps() +
        "|" + cur + "|" + next + "|" + (teacherLed ? "T" : "S") + "|" + state +
        "|" + tokens + "|" + (helpActive ? 1 : 0) + "|" + (helpAcknowledged ? 1 : 0) +
        "|" + Math.idiv(Math.max(0, restLeft), 1000) + "|" + hintsSent + "|" + helpCount +
        "|" + restCount + "|" + moveCount + "|" + Math.idiv(workMs, 1000) +
        "|" + Math.idiv(stepMs, 1000) + "|" + (lowSensory ? 1 : 0) + "|" + stepsDone +
        "|" + (timerView ? 1 : 0) + "|" + Math.idiv(Math.max(0, moveLeft), 1000) +
        "|" + stepBudgetSec + "|" + moveIntervalSec + "|" + restDurationSec)
}

serial.redirect(SerialPin.P14, SerialPin.P15, BaudRate.BaudRate9600)
serial.setRxBufferSize(96)
pins.digitalWritePin(DigitalPin.P13, 0)
pins.analogSetPitchPin(AnalogPin.P13)
music.setBuiltInSpeakerEnabled(false)
serial.onDataReceived(serial.delimiters(Delimiters.NewLine), function () {
    let line = serial.readUntil(serial.delimiters(Delimiters.NewLine))
    if (line.length > 0 && line.charAt(line.length - 1) == "\r") line = line.substr(0, line.length - 1)
    if (line.length > 0 && line.length <= 32 && cmds.length < 12) cmds.push(line)
})
input.onButtonPressed(Button.A, function () { pressA = true })
input.onButtonPressed(Button.B, function () { pressB = true })
input.onButtonPressed(Button.AB, function () { pressAB = true })
basic.pause(300)
newSession()
movePointer()
sendEvent("power_on", "")

let last = input.runningTime()
let lastStatus = 0
basic.forever(function () {
    let now = input.runningTime()
    let dt = Math.max(0, Math.min(2000, now - last))
    last = now

    if (now - lastSonarAt >= SONAR_SAMPLE_MS) {
        updateSeatPresence(now)
        lastSonarAt = now
    }

    // Work clock is NOT a measured focus score. It excludes rest, movement,
    // away periods, and paused or finished sessions.
    if (!paused && !finished) {
        if (resting) {
            if (restLeft > 0) {
                restLeft -= dt
                if (restLeft <= 0) { restLeft = 0; sendEvent("rest_over", ""); beep(Note.G4) }
            }
        } else if (moving) {
            moveLeft -= dt
            if (moveLeft <= 0) { endMove("time"); beep(Note.C5) }
        } else if (!waitingTeacher && !helpActive && !away) {
            // Time waiting for teacher support or away from the seat is NOT
            // counted as work; a help request cannot cause an overtime judgement.
            workMs += dt
            stepMs += dt
            if (moveIntervalSec > 0) {
                workSinceMove += dt
                if (workSinceMove >= moveIntervalSec * 1000) offerMove("scheduled")
            }
            if (timerView && !overtimeSent && stepMs >= budgetMs()) {
                overtimeSent = true
                sendEvent("time_guide_reached", "")
            }
        }
    }

    // Any physical button press confirms the student is back at the device.
    if (pressAB || pressA || pressB) {
        if (away) {
            away = false
            awaySince = 0
            backSince = 0
            sendEvent("away_end", "button")
        }
    }

    // A+B has precedence over both separate button events.
    if (pressAB) {
        pressAB = false
        pressA = false
        pressB = false
        if (resting) endRest("student")
        else startRest("student")
    }
    if (pressA) {
        pressA = false
        if (moveOffer) startMove("student")
        else studentDone()
    }
    if (pressB) {
        pressB = false
        if (moveOffer) moveLater("student")
        else if (helpActive) cancelHelp()
        else requestHelp()
    }
    while (cmds.length > 0) handleCommand(cmds.shift())
    if (moveOffer && now >= moveOfferUntil) moveLater("timeout")

    let view = currentView(now)
    if (view != lastView || lowSensory != lastLow) {
        draw(view)
        lastView = view
        lastLow = lowSensory
    }
    if (now - lastStatus >= 1000) { sendStatus(); lastStatus = now }
    basic.pause(30)
})

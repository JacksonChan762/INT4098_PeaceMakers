# Step Buddy 3.0 — Classroom support for students with ADHD

Step Buddy breaks classroom activities into small, clear steps. Students use a micro:bit to mark a step complete, ask for help, or take a break. Teachers respond through a local dashboard on a phone or computer. The student uses physical task cards and buttons, so the activity can run without a student tablet.

The goal is to help students answer “What do I do now?”, “What comes next?”, and “How do I ask for help?” Step Buddy is a classroom prototype. Its effects on ADHD symptoms and academic performance have not been tested. It supports classroom routines alongside the student's individual support plan.

For a component-by-component explanation and a complete classroom activity, see the [ADHD teaching guide](ADHD_TEACHING_GUIDE.md).

## Teaching approach

The CDC describes clear tasks, timely positive feedback, help with organisation, and supports adapted to the individual student. Evidence on which classroom accommodations work best is limited, so use the student's responses and work to judge which features help. [CDC — ADHD in the Classroom](https://www.cdc.gov/adhd/treatment/classroom.html)

NICE gives shorter work periods, movement breaks, and written instructions alongside spoken instructions as examples of environmental adjustments. These ideas inform Step Buddy's task cards and break controls. [NICE NG87 — Environmental modifications](https://www.nice.org.uk/guidance/ng87/chapter/Recommendations)

| Classroom need | Step Buddy feature | How to use it |
|---|---|---|
| Getting started when several instructions are given at once | Step number, servo pointer, and task cards | Put one action on each card. Ask the student to point to or say the first action. |
| Knowing where they are and what comes next | Button A and the teacher's progress display | Agree on what counts as complete and preview the next step. |
| Asking for help without raising a hand | Button B and teacher acknowledgement | Practise the process and respond in person. The LED may be visible to classmates. |
| Knowing how to continue after feedback | Icon hints and completion feedback | Teach the meaning of each icon and give specific spoken feedback, such as “You circled two key words.” |
| Taking a break and returning to work | Buttons A+B and optional movement break offers | Agree on where to take a break and how to resume. Keep breaks available regardless of points earned. |
| Managing sound, brightness, or countdowns | Quiet mode and an optional visual timer | Start with quiet mode on and the timer off. Adjust with the student. Treat time as a guide. |
| Checking whether a step is complete | Teacher confirmation mode | The student presses A, and the teacher checks the work and selects **Complete step and continue**. Use independent mode when it suits the student. |

The device adds one point each time a step is completed for the first time and gives a celebration cue every five points. Use the student's work to check understanding. Keep points private and leave them unchanged when a student asks for help or takes a break.

## Example reading activity

1. Agree on a goal, such as “Read one page and find one main idea.”
2. In **Personalised support → Advanced settings**, choose **Reading (5 steps)** and select **Switch template**. Switching templates resets progress.
3. Prepare five cards in order: choose a page, read it, find one main idea, tell or write the idea, and check it together. Match each card to its pointer position.
4. Practise A, B, and A+B once. Let the student know they can also ask for help or a break aloud. Choose independent or teacher confirmation mode with the student.
5. Point to the current card and give one short instruction. The student presses A when ready to move on. If they press B, select **Acknowledge request**, offer help such as reading the first sentence together, and then select **Mark as resolved**.
6. If the student takes a break, point back to the same card when they return. Let them say, point to, or write the main idea. Check their response and ask which card or prompt helped most.

Over a few similar sessions, observe how the student starts, asks for help, and demonstrates understanding. Change one setting at a time so the student can tell you what helped. Use paper cards or spoken prompts if the device adds distraction.

## Student and teacher controls

| Control | Action |
|---|---|
| **A** | Marks the step complete. In teacher confirmation mode it submits the step for review. During a break it lets the student return early. |
| **B** | Requests help. Press again to cancel the request. |
| **A+B** | Starts or ends a student-requested rest break. |
| **A / B when a movement break is offered** | A starts the break and B defers it. |
| **Previous step** | Goes back one step. Completed steps and points are retained, so revisiting a step earns no extra point. |
| **Personalised support → Advanced settings → Restart** | Resets progress, points, and counters for the session. |
| **Session activity log → Clear session log** | Clears the temporary ESP8266 event log separately. |

The default is independent mode. A student-requested break lasts 3 minutes, and a movement break is offered after approximately 15 minutes of accumulated task time. The movement break lasts 60 seconds. Teachers can adjust the rest break length, offer interval, and suggested step time on the dashboard. Changing the 60-second movement break duration requires editing the micro:bit program.

## WuKong buzzer

The WuKong's onboard buzzer uses P0 for optional step feedback and certain break prompts, such as a movement break offer. Turn on the board's buzzer switch, then select **Turn quiet mode off** under **Personalised support** to hear the cues. Quiet mode starts on. It mutes cues and dims the LED. Preview the cues with the student before enabling sound during a lesson.

[ElecFreaks WuKong documentation — onboard buzzer and switch](https://wiki.elecfreaks.com/en/microbit/expansion-board/wukong/)

## Wiring and setup

Disconnect power before wiring. The micro:bit and WuKong form the student device. The ESP8266 hosts the teacher dashboard.

| Connection | Destination |
|---|---|
| ESP8266 D5 (RX) | WuKong P14 signal (TX) |
| ESP8266 D6 (TX) | WuKong P15 signal (RX) |
| ESP8266 GND | WuKong GND, shared ground |
| WuKong onboard buzzer | P0, already wired on the board. Turn on its physical switch for sound. |
| Pointer servo | WuKong S0. Match the servo's signal, power, and ground leads. |

1. Open the [micro:bit program](microbit/step_buddy_v3_microbit.ts) in the MakeCode JavaScript editor and download it to the micro:bit.
2. In the `ESP8266/src` folder, copy [config.example.h](ESP8266/src/config.example.h) to `config.h`. Set two different passwords there. Use 8–63 characters for `AP_PASS` (Wi-Fi) and at least 8 characters for `WEB_PASS` (teacher dashboard). Git ignores `config.h` so your passwords stay local.
3. Open the `ESP8266` folder in PlatformIO, build, and upload. If you edit the dashboard HTML, run `python3 tools/embed_pages.py` from the `ESP8266` folder before building.
4. Connect the teacher's device to the `StepBuddy-01` Wi-Fi network with your Wi-Fi password. Open the [teacher dashboard](http://192.168.4.1/teacher) and sign in as `teacher` with your dashboard password. The network provides local access only.
5. Confirm the micro:bit is online in the dashboard. Test step completion, help, breaks, hints, and the optional buzzer.

The dashboard offers Homework (6 steps), Class (6 steps), and Reading (5 steps). Step names and counts live in the micro:bit program. To change them, update the program, printed cards, and pointer labels together.

## Records and testing

Step Buddy operates over local Wi-Fi and has no fields for student names or diagnoses. The ESP8266 holds a temporary event log in memory. The teacher can download it as a CSV. Power loss clears the on-device copy, while downloaded files remain. Keep exported records private.

Completion, help, and break counts describe device events. The task timer excludes breaks, pauses, and time spent waiting for help or teacher confirmation. Use student work and observation to assess learning.

The ESP8266 build passed. The MakeCode build, physical setup, and classroom use still need verification. Paper cards and spoken help requests provide a fallback during testing.

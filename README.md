# Step Buddy 3.0

Step Buddy is a classroom prototype for students with ADHD. A micro:bit, an LED display and a servo pointer guide the student through one printed task card at a time. The ESP8266 hosts an offline dashboard so the teacher can check progress, respond to help requests and offer breaks.

The `seat-reminder` branch adds an optional desk cue using an ElecFreaks Sonar:bit. The teacher can use the cue to check in with the student and point them back to the current task card. Step Buddy cannot measure a student's attention. Use the student's work and feedback to judge whether the classroom setup is helping. See the [ADHD teaching guide](ADHD_TEACHING_GUIDE.md) for a lesson example.

## Hardware and wiring

You need a micro:bit, a WuKong breakout board, an ESP8266 NodeMCU, a pointer servo, jumper wires and printed task cards. A three-wire ElecFreaks Sonar:bit is needed only for the desk reminder. Disconnect power before wiring.

| Connection | Destination |
|---|---|
| ESP8266 D5 (RX) | WuKong P14 signal (TX) |
| ESP8266 D6 (TX) | WuKong P15 signal (RX) |
| ESP8266 GND | WuKong GND |
| Pointer servo | WuKong S0. Match the signal, voltage and ground markings. |
| Sonar:bit, optional | WuKong P8. Match S, V and G to the three wires. |
| WuKong onboard buzzer | Already connected to P0. Use the physical buzzer switch for sound. |

The P8 connection is for the single-signal Sonar:bit. Use the pin markings and the board manufacturer's wiring diagrams when connecting it. [ElecFreaks Sonar:bit](https://wiki.elecfreaks.com/en/microbit/sensor/octopus-sensors/sensor/octopus_ef04089/) · [ElecFreaks WuKong](https://wiki.elecfreaks.com/en/microbit/expansion-board/wukong/)

## Install and connect

1. Paste the [micro:bit program](microbit/step_buddy_v3_microbit.ts) into the MakeCode JavaScript editor and download the program to the micro:bit. Set `SONAR_ENABLED` to `false` before downloading if you want to turn off the desk reminder.
2. Copy [ESP8266/src/config.example.h](ESP8266/src/config.example.h) to `ESP8266/src/config.h`. Set different passwords for `AP_PASS` (8–63 characters) and `WEB_PASS` (at least 8 characters). Git ignores the resulting `config.h`.
3. Open the `ESP8266` folder in PlatformIO and build and upload the `nodemcuv2` environment.
4. Connect a phone or computer to the `StepBuddy-01` Wi-Fi network using your Wi-Fi password. Open [the teacher dashboard](http://192.168.4.1/teacher) and sign in as `teacher` with your dashboard password. The network works offline.
5. Check that the dashboard reports the micro:bit online. Try **Previous step**, **Restart**, and the help and break controls before class.

## Classroom controls

| Control | What it does |
|---|---|
| **A** on the micro:bit | Marks the step complete. In teacher confirmation mode, the teacher reviews the work and selects **Complete step and continue**. |
| **B** | Requests help. Press again to cancel. The teacher can select **Acknowledge request** and then **Mark as resolved**. |
| **A+B** | Starts or ends a rest break. A alone ends an active break early. |
| **A / B during a movement break offer** | A accepts the offer and B defers it. |
| **Personalised support → Advanced settings** | Switches task templates, adjusts timing and selects independent or teacher confirmation mode. **Restart** returns to step 1 and clears progress. |

The three task templates are Homework (6 steps), Class (6 steps) and Reading (5 steps). Prepare one card per step and align the cards with the pointer. Start with the first card, demonstrate A, B and A+B, and agree on how the student can ask for help or take a break. When the student returns from a break, point to the same card and give one short instruction.

## Desk reminder

1. Agree with the student on what the cue means. Aim the Sonar:bit at the student's usual seated position. Test the placement before class. The current near threshold is **40 cm**, set by `AWAY_DIST_CM` in the micro:bit program.
2. During task time, the firmware waits for a valid near reading before it can show an alert. If valid readings then stay farther than the threshold for **3 seconds**, the task timer pauses and the teacher dashboard shows **Possible seat absence**.
3. The dashboard opens one dismissible pop-up per alert. Quiet mode starts on. For a short audible cue, turn on the WuKong buzzer switch and select **Turn quiet mode off**. Audio cues are at least **60 seconds** apart. The same setting enables the usual step and break sounds.
4. A valid near reading for **1.5 seconds** clears the alert. A student button press also clears it and performs the usual button action. Dismissing the dashboard pop-up leaves the status visible while the alert is active.
5. Missing and out-of-range echoes clear the distance alert and require a fresh near reading. The reminder is suspended during breaks and break offers, when paused, and while the student waits for help or teacher confirmation.

The sensor measures a reflecting surface. Check how the reading behaves when the student sits, turns, leaves and returns. If the empty seat produces no echo, the reminder will stay quiet. Use teacher judgment and give the student a way to ask for a break at any time.

## Verify both boards

Use a flat target to test the distance logic before trying it with a student. Start within range, move the target beyond the threshold for 3 seconds, then move it back. Confirm the dashboard opens once, the timer resumes after return, and sound respects quiet mode. Try removing the target completely to verify that a missing echo produces no alert. Check help, breaks and teacher confirmation while trying the same movements.

The activity log stays on the ESP8266 until the device restarts or the teacher clears it. A downloaded CSV is retained on the teacher's device. Treat event counts and elapsed time as records of device activity. Keep exports private.

## Update the dashboard

Edit [ESP8266/web/teacher.html](ESP8266/web/teacher.html). Run `python3 tools/embed_pages.py` from the `ESP8266` folder and commit the updated `src/pages.h` with the HTML. Rebuild and upload the ESP8266 firmware to serve the new page.

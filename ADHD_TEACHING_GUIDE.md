# Step Buddy teaching guide for students with ADHD

## 1. What Step Buddy helps students do

Step Buddy gives students a way to see the current task, mark progress, ask for help, and take a break. Physical task cards, micro:bit buttons, an LED display, a servo pointer, and optional sound guide the student. The ESP8266 dashboard lets the teacher see requests and respond.

The teaching goal is for the student to answer three questions:

1. What is my next small step?
2. How can I ask for help if I get stuck?
3. How can I take a break and pick up where I left off?

Some students with ADHD benefit from extra help starting a task, organising steps, or switching activities. Ask the student which cues make those actions easier. The CDC describes clear tasks, timely positive feedback, organisational skills, and support adapted to the individual. Evidence varies by accommodation. Assess this prototype through student feedback, work, and teacher observation. Its effects on ADHD symptoms, attention, and grades have not been established. [CDC — ADHD in the Classroom](https://www.cdc.gov/adhd/treatment/classroom.html)

## 2. What each component contributes

| Component | What it does | Why it may help | How to use it |
|---|---|---|---|
| micro:bit | Reads buttons, tracks steps, and shows prompts | Gives the student a predictable way to act | Practise A to complete, B to ask for help, and A+B to take a break. Keep the meanings consistent. |
| LED display | Shows step numbers and icons for help and feedback | Makes the current state visible | Teach the icons before use and pair them with task cards. |
| Physical task cards | Put the current action in writing or pictures | Give the student a reference without having to recall a string of spoken directions | Put one main action on each card, such as “Move the cardboard closer.” |
| Servo and pointer | Turn to the card for the current step | Make progress and transitions visible | Align the pointer with the six card positions. At each transition, point out the next card. |
| WuKong onboard buzzer | Gives optional task and break feedback from P0 | Adds a cue for students who find sound helpful | Let the student hear it before the lesson. Keep quiet mode on if they prefer the visual cues. |
| WuKong board | Powers the student device and connects the servo and buzzer | Brings the student's controls and feedback into one device | Secure the device and wires. Give the student room to operate the buttons and work with lesson materials. |
| ESP8266 | Links the micro:bit to the local teacher dashboard | Makes help requests and progress visible to the teacher | Check the connection before class, then follow up on requests in person. |
| Teacher's phone or computer | Shows progress, requests, and teacher controls | Lets the teacher send a hint, allow more time, or check a completed step | Use the dashboard alongside the student's response and work. |

A lesson can use the whole system while giving the student one cue at a time. For example, show one card, let them move an object and read the ruler, and then prompt them to press A. Select sound, brightness, and timing based on what the student finds helpful.

## 3. Teaching routines

### Work through one step at a time

Give one clear goal and let the student press **A** when they finish. The press records their indication that they are done. Check understanding through what they say, show, or write.

In teacher confirmation mode, the student presses A and waits for review. Check their work, then select **Complete step and continue**. Switch to independent mode when the student is ready to manage transitions.

### Respond to help requests

When the student presses **B**, select **Acknowledge request**, give a specific prompt, and select **Mark as resolved** when the support is complete. For example, say “Put the cardboard next to the 20 cm mark and we'll read the ruler together.” The help icon can be visible to other people, so keep spoken requests available too.

### Offer breaks and make it easy to return

**A+B** starts or ends a student-requested rest break. During that break, **A** can end it early. When a movement break is offered, **A** accepts it and **B** defers it.

Agree on the break location and how the student can return. Point them back to the same card when they resume. A requested break is available regardless of points earned. The current defaults are 3 minutes for a requested break and about 15 minutes of accumulated task time between movement break offers. Adjust them with the student.

### Give specific feedback

The device awards a point for the first completion of each step and gives a celebration cue every five points. Pair it with feedback that names a learning action, such as “You compared the two numbers and noticed that the reading went down when the cardboard moved closer to zero.” Keep points private and available independently of whether the student asks for help, makes a mistake, or takes a break.

### Adjust sound, light, and time

Quiet mode starts on and the visual timer starts off. Preview sound with the student before using it. Leave the timer off if it adds pressure, or use **Add 2 minutes** to allow more time. NICE gives shorter work periods, movement breaks, and written instructions alongside spoken instructions as examples of environmental adjustments. [NICE NG87 — Environmental modifications](https://www.nice.org.uk/guidance/ng87/chapter/Recommendations)

## 4. Example lesson using the project — Near and far

The student compares two distance readings and explains, by speaking, pointing, or writing, that a distance from the zero mark gets smaller as an object moves towards zero and larger as it moves away.

### Prepare the activity

- Prepare a flat cardboard target or box, a ruler, a pencil, and a recording sheet. Keep the ruler fixed and use its zero mark as the reference point.
- Select **Class (6 steps)**, then **Switch template**. Select **Switch to teacher confirmation** if the dashboard is in independent mode.
- Place the six task cards in order beside the pointer. Check that the teacher dashboard shows the current step.
- If the student wants sound, turn on the WuKong buzzer switch and select **Turn quiet mode off** after previewing the feedback.

### Six-step lesson

| Class template step | Student's task card | Teacher action | Features used |
|---|---|---|---|
| 1. Hear today's goal | Point to “near” and “far.” | Show two examples and check that the student can identify each. | Task card, pointer, step number, teacher confirmation |
| 2. Watch one example | Watch the cardboard move closer. | Point to the zero mark and show how to read the ruler at the edge of the cardboard in two positions. | Ruler, task card, teacher dashboard |
| 3. Try one item | Place the cardboard about 20 cm away. | Let the student place it, then point to the ruler mark or record the number. Respond if they press B. | Ruler, help button, teacher acknowledgement, LED hint |
| 4. Do two more | Try about 10 cm, then 30 cm. | Give one position at a time. Offer a break between trials if useful, then return to the same card. | Ruler, recording sheet, A+B, pointer |
| 5. Share an answer | Show which number is larger. | Accept an oral, pointed, or written response. Name what the student did to compare the readings, then confirm the step. | A, teacher confirmation, LED feedback and optional buzzer |
| 6. Tidy up | Put away the cardboard and recording sheet. | Preview the end of the lesson and ask which prompt helped most. | Final task card, pointer, completion icon, dashboard |

Use 10, 20, and 30 cm as example positions. Give the student time to compare the readings. Teachers confirm progress based on the student's work.

For example, at step 3 the teacher can say, “Look at the third card and put the cardboard here.” When the ruler shows about 20 cm, ask, “Would you like to say the number or write it down?” The student presses A when ready. After the teacher confirms, the pointer moves to card 4 and the LED shows the next step. If the student gets stuck, they can press B and receive one demonstration. They can request a break with A+B and resume from the same card.

## 5. Find out what helps the student

Compare a few similar tasks with similar support. Collect simple observations, student feedback, and samples of work.

| What to observe | How to check |
|---|---|
| Understanding the current task | Ask the student to say or point to the next action. |
| Getting started | Note whether they start with the card alone or need another explanation, a demonstration, or help getting started. |
| Learning the measurement concept | Ask them to compare two readings and explain which position was nearer. |
| Getting help | Check whether the teacher saw the request and offered suitable help. A rise in requests can reflect that a student is more comfortable asking. |
| Resuming after a break | Observe whether the current card helps them pick up where they left off. |
| Comfort with the setup | Ask “What helped?” and “Was anything too loud or too complicated?” |

The temporary ESP8266 CSV can help reconstruct the sequence of device events. Use student responses and work for evidence of understanding. Elapsed task time and the number of help requests or breaks describe the session. Keep those records private and avoid using them to rank students.

## 6. Project status

The code includes three task templates, physical buttons, LED prompts, the servo pointer, optional sound, teacher confirmation, help requests, breaks, and temporary event exports. Teachers prepare task cards, set the pace, respond to requests, and check learning.

The ESP8266 build passed. This version still needs MakeCode compilation, upload to both boards, and a physical test of the servo and buzzer.

## 7. Project description

Step Buddy is a classroom prototype for students with ADHD. It uses task cards, micro:bit buttons, an LED display, and a servo pointer to show what to do now and give students ways to ask for help or take a break. Teachers respond through an ESP8266 dashboard. The onboard WuKong buzzer gives optional feedback alongside the LED prompts. Teachers assess the design through the student's work, feedback, and classroom observations.

## References and project files

- [CDC — ADHD in the Classroom](https://www.cdc.gov/adhd/treatment/classroom.html)
- [NICE NG87 — ADHD recommendations](https://www.nice.org.uk/guidance/ng87/chapter/Recommendations)
- [README](README.md) for wiring, upload, login, and reset instructions.
- [micro:bit program](microbit/step_buddy_v3_microbit.ts) for device behaviour.
- [Teacher dashboard](ESP8266/web/teacher.html) for controls and status messages.

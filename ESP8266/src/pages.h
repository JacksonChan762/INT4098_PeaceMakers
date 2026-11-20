#pragma once
#include <Arduino.h>

const char TEACHER_PAGE[] PROGMEM = R"STBHTML(<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#f0f4f8"><title>Step Buddy · Teacher dashboard</title><style>
:root{color-scheme:light;--bg:#f1f5f8;--ink:#17283c;--muted:#536779;--line:#dce5ee;--card:#fff;--navy:#233e71;--sage:#0b786a;--warn:#fff2db}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--ink);font:16px/1.5 -apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif}main{max-width:980px;margin:auto;padding:16px 14px 42px}
header{display:flex;justify-content:space-between;align-items:center;gap:12px;margin:8px 0 18px}h1{font-size:28px;margin:0}h2{font-size:19px;margin:0 0 11px}h3{font-size:16px;margin:12px 0 7px}
.sub{color:var(--muted);font-size:14px}.eyebrow{color:var(--navy);font-weight:800;font-size:12px;letter-spacing:1px}.tag{border-radius:99px;border:1px solid var(--line);background:#fff;padding:8px 10px;font-size:13px;font-weight:700}.tag.bad{color:#9b3330;background:#ffe9e5}
.card{background:var(--card);border:1px solid var(--line);border-radius:18px;padding:19px;margin-bottom:13px;box-shadow:0 4px 20px #152c4410}.layout{display:grid;grid-template-columns:1.12fr 1fr;gap:13px}.layout>.card{min-width:0}
.kicker{color:var(--sage);font-weight:750;font-size:13px}.current{font-size:26px;line-height:1.25;font-weight:820;margin:8px 0;letter-spacing:-.4px}.progress{height:8px;background:#e7edef;border-radius:999px;overflow:hidden;margin:14px 0}.progress>div{height:100%;background:var(--sage);width:0%}
.alert{padding:17px;border-radius:14px;background:#eef5f5;margin-bottom:12px}.alert.need{background:var(--warn)}.alert strong{display:block;font-size:20px}.alert p{margin:5px 0 0}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:9px}button{min-height:48px;padding:10px 12px;border-radius:12px;border:1px solid var(--line);font:inherit;font-weight:720;color:var(--ink);background:#fff;cursor:pointer;touch-action:manipulation}
button:focus-visible,input:focus-visible,textarea:focus-visible,select:focus-visible{outline:3px solid #a0c9c5;outline-offset:2px}button:disabled{opacity:.46;cursor:default}button.primary{background:var(--navy);border-color:var(--navy);color:#fff}button.good{background:var(--sage);border-color:var(--sage);color:#fff}button.soft{background:#ebf3f3;color:#095c56}
.field{display:block;font-weight:720;font-size:14px;margin:9px 0 5px}input,textarea,select{width:100%;border:1px solid #bdcdd9;border-radius:10px;background:#fff;font:inherit;padding:11px;color:var(--ink)}textarea{min-height:70px;resize:vertical}select{min-height:46px}
.form-actions{display:flex;gap:8px;flex-wrap:wrap;margin:9px 0}.chips{display:flex;gap:8px;flex-wrap:wrap;margin:8px 0 12px}.chips button{min-height:40px;font-size:14px;background:#f2f6fa}
details{border-top:1px solid var(--line);margin-top:14px;padding-top:13px}summary{cursor:pointer;font-weight:750}details>p{color:var(--muted);font-size:14px}
.stats{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}.stat{border:1px solid var(--line);border-radius:12px;padding:11px}.stat b{font-size:24px;display:block}
.small{font-size:13px;color:var(--muted)}#toast:empty{display:none}#toast{position:sticky;bottom:8px;background:white;padding:10px;border:1px solid var(--line);border-radius:10px;min-height:20px;color:var(--navy);font-size:14px}footer{color:var(--muted);font-size:13px;margin-top:22px}
.modal-backdrop{position:fixed;inset:0;background:rgba(23,40,60,.55);display:none;align-items:center;justify-content:center;padding:16px;z-index:999}
.modal-backdrop.show{display:flex}
.modal{background:#fff;border:2px solid #d97706;border-radius:18px;max-width:420px;width:100%;padding:22px;box-shadow:0 12px 32px rgba(0,0,0,.22)}
.modal h3{margin:0 0 8px;font-size:21px;color:#92400e}
.modal p{margin:0 0 16px;color:var(--ink);font-size:15px}
.modal button{width:100%;background:var(--navy);border-color:var(--navy);color:#fff}
@media(max-width:730px){.layout{grid-template-columns:1fr}.stats{grid-template-columns:repeat(2,minmax(0,1fr))}.card{padding:16px}h1{font-size:23px}header{align-items:flex-start}}
@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation:none!important;transition:none!important}}
</style></head><body><main>
<header><div><div class="eyebrow">STEP BUDDY 3.0 · TEACHER</div><h1>Teacher dashboard</h1><div class="sub">Guide students through one step at a time.</div></div><span class="tag bad" id="online">Connecting…</span></header>
<section class="layout"><div class="card"><div class="kicker" id="stepCounter">Waiting for device</div><div class="current" id="title">Connect the micro:bit</div><div id="instruction" class="sub">—</div>
<div class="progress"><div id="bar"></div></div><div id="stepInfo" class="sub">—</div><div class="sub" id="nextInfo">—</div>
<div class="form-actions"><button class="good" id="nextBtn" data-cmd="NEXT">✓ Complete step and continue</button><button id="backBtn" data-cmd="BACK">Previous step</button><button id="pauseBtn" data-cmd="PAUSE">Pause</button><button data-cmd="PLUS120">Add 2 minutes</button></div></div>
<div class="card"><h2>Live support</h2><div id="support" class="alert" role="status" aria-live="polite"><strong id="supportTitle">No help requests</strong><p id="supportText">Student help requests appear here.</p></div>
<div class="grid"><button class="primary" id="ack" data-cmd="ACK">Acknowledge request</button><button class="good" id="resolved" data-cmd="HELPDONE">Mark as resolved</button></div>
<h3>micro:bit icon hints</h3><p class="small">Choose a hint to send its icon to the student’s micro:bit. Explain the hint aloud.</p><div class="grid" id="hints"></div></div></section>
<section><div class="card"><h2>Personalised support</h2><div class="grid"><button id="restBtn" data-cmd="RESTEND">End rest break</button><button id="moveBtn" data-cmd="MOVE">Offer movement break</button><button id="quietBtn" data-cmd="LOW">Toggle quiet mode</button><button id="timerBtn" data-cmd="TIMER">Toggle visual timer</button></div>
<details><summary>Advanced settings</summary>
<label class="field" for="budget">Suggested time for this step</label><select id="budget"><option value="180">3 minutes</option><option value="300">5 minutes</option><option value="600">10 minutes</option><option value="900">15 minutes</option><option value="1200">20 minutes</option><option value="1800">30 minutes</option></select><button data-setting="STEPSEC:" data-from="budget">Apply step time</button>
<label class="field" for="interval">Time between movement break offers</label><select id="interval"><option value="0">No automatic offers</option><option value="600">10 minutes</option><option value="900">15 minutes</option><option value="1200">20 minutes</option><option value="1800">30 minutes</option></select><button data-setting="MOVEINT:" data-from="interval">Apply interval</button>
<label class="field" for="restDuration">Length of student-requested breaks</label><select id="restDuration"><option value="60">1 minute</option><option value="120">2 minutes</option><option value="180">3 minutes</option><option value="300">5 minutes</option><option value="600">10 minutes</option></select><button data-setting="RESTSEC:" data-from="restDuration">Apply break length</button>
<label class="field" for="template">Task template (switching resets progress)</label><select id="template"><option value="0">Homework (6 steps)</option><option value="1">Class (6 steps)</option><option value="2">Reading (5 steps)</option></select><button id="setTemplate">Switch template</button>
<div class="grid"><button id="modeBtn" data-cmd="MODE">Switch completion mode</button><button data-cmd="NEW" data-confirm="Restart this session and clear its progress?">Restart</button></div></details></div></section>
<section class="card"><h2>Connection and classroom controls</h2>
<p>On the micro:bit, press A to mark a step complete, B to ask for help (press again to cancel), or A+B to take a break.</p>
<p>When offered a movement break, press A to start or B to defer. During a break, press A to return early.</p>
<p class="sub">Connect to StepBuddy-01 Wi-Fi, open <b>http://192.168.4.1/teacher</b> and sign in. This local hotspot works offline.</p></section>
<section class="card"><details><summary>Session activity log</summary><div class="stats" id="stats"></div>
<p class="sub">Counts and times describe session activity. They cannot measure attention or assess ADHD. Avoid using them to punish or rank students.</p><p><a href="/api/teacher/log.csv">Download session CSV</a> <button id="clearLog">Clear session log</button></p></details></section>
<p id="toast" role="status"></p><footer>Step Buddy collects no names or medical records and generates no distraction scores. Check the device status after sending a command. Keep the ESP8266 HTTP hotspot off the public internet.</footer>
<div class="modal-backdrop" id="awayModal" role="alertdialog" aria-modal="true" aria-labelledby="awayModalTitle">
<div class="modal"><h3 id="awayModalTitle">⚠️ Student Left Seat</h3><p id="awayModalText">The student is away from their seat and needs attention. The task timer has been paused automatically.</p><button type="button" id="closeAwayModal">OK, got it</button></div>
</div>
</main><script>
const $=id=>document.getElementById(id);const HINTS=['Start with one small step','Read the question again','Take your time','Look at an example','You’re making progress','I’ll come and help'];
HINTS.forEach((s,i)=>{const b=document.createElement('button');b.textContent=s;b.dataset.cmd='HINT'+i;$('hints').appendChild(b)});
const stats=[['stepsDone','Steps completed'],['helpCount','Private help requests'],['restCount','Rest breaks'],['moveCount','Movement breaks'],['hints','Icon hints'],['workSec','Time on task']];
stats.forEach(([k,label])=>{const card=document.createElement('div');card.className='stat';const cap=document.createElement('span');cap.className='small';cap.textContent=label;const val=document.createElement('b');val.id='stat_'+k;val.textContent='—';card.append(cap,val);$('stats').appendChild(card)});
let latest=null,busy=false,wasAway=false;
$('closeAwayModal').onclick=()=>$('awayModal').classList.remove('show');
function note(s){$('toast').textContent=s}function clock(n){n=+n||0;return Math.floor(n/60)+':'+String(n%60|0).padStart(2,'0')}
function form(body){return {method:'POST',credentials:'same-origin',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(body)}}
async function submit(url,body){
 if(busy){note('Please wait for the previous action to finish.');return}
 busy=true;
 try{
  note('Sending…');
  const r=await fetch(url,form(body));
  if(r.status===504)throw Error('No confirmation from the micro:bit. Check the current step, then check the wire from ESP8266 D6 to micro:bit P15 and the shared ground.');
  if(r.status===409)throw Error('The previous step is unavailable right now.');
  if(!r.ok)throw Error('HTTP '+r.status);
  if(r.status===200 && body.c==='NEW')note('The micro:bit restarted the session at step 1.');
  else if(r.status===200 && body.c==='BACK')note('The micro:bit returned to the previous step.');
  else note('Sent. Check the latest device status.');
  setTimeout(refresh,300)
 }catch(e){note('Action failed. '+e.message)}finally{busy=false}
}
function sendCmd(c){if(!latest||!latest.online){note('The micro:bit is offline. Reconnect it to use this control.');return}submit('/api/teacher/cmd',{c})}
document.addEventListener('click',e=>{const b=e.target.closest('button[data-cmd],button[data-setting]');if(!b||b.disabled)return;
 if(b.dataset.confirm&&!confirm(b.dataset.confirm))return;sendCmd(b.dataset.cmd||(b.dataset.setting+$(b.dataset.from).value))});
$('setTemplate').onclick=()=>{if(confirm('Switch templates and reset progress?'))sendCmd('TPL'+$('template').value)};
$('clearLog').onclick=()=>{if(confirm('Clear the session event log?'))submit('/api/teacher/clear-log',{})};
function render(d){latest=d;$('online').textContent=d.online?'● micro:bit online':'● micro:bit offline';$('online').classList.toggle('bad',!d.online);
 if(!d.online){wasAway=false;$('awayModal').classList.remove('show');$('title').textContent='Waiting for the micro:bit';$('instruction').textContent='Check the power and the P14 and P15 serial connections.';document.querySelectorAll('button[data-cmd],button[data-setting],#setTemplate').forEach(b=>b.disabled=true);return}
 const isAway=d.state==='AWAY';
 if(isAway&&!wasAway){$('awayModal').classList.add('show');note('Alert: Student left their seat.');}
 else if(!isAway&&wasAway){$('awayModal').classList.remove('show');}
 wasAway=isAway;
 document.querySelectorAll('button[data-cmd],button[data-setting],#setTemplate').forEach(b=>b.disabled=false);
 $('stepCounter').textContent='Step '+d.step+' / '+d.total+' · '+d.tpl;
 $('title').textContent=d.state==='DONE'?'All steps complete':d.current;
 $('instruction').textContent='Use the task card to guide the student through this step.';
 $('bar').style.width=Math.round(100*(Number(d.stepsDone)||0)/(Number(d.total)||1))+'%';
 $('stepInfo').textContent='Steps completed: '+d.stepsDone+' · Elapsed: '+clock(d.stepSec)+' · Suggested: '+clock(d.expectedSec)+' · '+(d.mode==='T'?'Teacher confirmation':'Independent mode');
 $('nextInfo').textContent='Next step: '+(d.nextTitle||'None remaining');
 const needsHelp=d.help==='1',ack=d.ack==='1';let heading='No help requests',text='Offer specific encouragement or a hint if needed.';
 if(needsHelp){heading=ack?'✓ Student notified that help is on the way':'🙋 Student needs help';text='The student pressed B on the micro:bit. '+(ack?'Help the student, then select “Mark as resolved”.':'Select “Acknowledge request” to let the student know you saw it.')}
 else if(d.state==='WAIT'){heading='✓ Student marked the step complete';text='Check the work, then select “Complete step and continue”.'}
 else if(d.state==='AWAY'){heading='🪑 Student is away from seat';text='The student left their seat and needs attention. The task timer is paused until they return.'}
 else if(d.state==='REST'||d.state==='RESTOVER'){heading='☁️ Student is taking a break';text='Agree with the student on when to return.'}
 else if(d.state==='OFFER'){heading='🚶 Movement break offered';text='The student can start now or choose later.'}
 else if(d.state==='TIME'){heading='Suggested time reached';text='Offer more time or simplify the instructions. Time alone cannot indicate attention.'}
 else if(d.state==='PAUSE'){heading='Session paused';text='Resume to continue from this step.'}
 else if(d.state==='DONE'){heading='🎉 All steps complete';text='Praise something specific and review the session together.'}
 $('supportTitle').textContent=heading;$('supportText').textContent=text;$('support').classList.toggle('need',needsHelp||d.state==='WAIT'||d.state==='AWAY');
 $('ack').disabled=!needsHelp||ack;$('resolved').disabled=!needsHelp;
 $('nextBtn').disabled=['DONE','REST','RESTOVER','MOVE','PAUSE','AWAY'].includes(d.state);
 $('backBtn').disabled=(Number(d.step)<=1 && d.state!=='DONE') || ['REST','RESTOVER','MOVE'].includes(d.state);
 $('pauseBtn').textContent=d.state==='PAUSE'?'Resume':'Pause';
 $('restBtn').disabled=!['REST','RESTOVER'].includes(d.state);
 $('moveBtn').textContent=d.state==='MOVE'?'End movement break':'Offer movement break';$('moveBtn').disabled=['DONE','PAUSE','AWAY','REST','RESTOVER'].includes(d.state);
 $('quietBtn').textContent=d.low==='1'?'Turn quiet mode off':'Turn quiet mode on';
 $('timerBtn').textContent=d.timer==='1'?'Turn visual timer off':'Turn visual timer on';
 $('modeBtn').textContent=d.mode==='T'?'Switch to independent mode':'Switch to teacher confirmation';
 if(document.activeElement!==$('template'))$('template').value=d.tplId;
 for(const [id,k] of [['budget','expectedSec'],['interval','moveIntervalSec'],['restDuration','restDurationSec']]){if(document.activeElement!==$(id))$(id).value=d[k]}
 stats.forEach(([k])=>{$('stat_'+k).textContent=k==='workSec'?clock(d[k]):d[k]})}
async function refresh(){try{const r=await fetch('/api/teacher/status',{cache:'no-store',credentials:'same-origin'});if(!r.ok)throw Error('HTTP '+r.status);render(await r.json())}
 catch(e){wasAway=false;$('awayModal').classList.remove('show');$('online').textContent='● Dashboard disconnected';$('online').classList.add('bad');note('Could not refresh. '+e.message);latest=null;document.querySelectorAll('button[data-cmd],button[data-setting],#setTemplate').forEach(b=>b.disabled=true)}}
refresh();setInterval(refresh,1200);
</script></body></html>
)STBHTML";

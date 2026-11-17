#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include "pages.h"

// =============================================================================
// STEP BUDDY 3.0 | ESP8266 local classroom server
// Teacher dashboard connects to the physical micro:bit via this device.
// Teacher web app has separate HTTP Basic authentication.
// No internet, tracking, diagnosis, camera, student name, or cloud dependency.
//
// Wiring (same as the original v2 prototype):
//   micro:bit P14 TX -> ESP8266 D5 RX
//   micro:bit P15 RX <- ESP8266 D6 TX
//   Both boards must have common GND. UART = 9600 baud.
// Set AP_PASS and WEB_PASS in your local src/config.h before building.
// This standalone WPA2 + HTTP prototype is NOT suitable for WAN/public hosting.
// =============================================================================

#include "config.h"

const char* AP_NAME = "StepBuddy-01";
const char* WEB_USER = "teacher";

static_assert(sizeof(AP_PASS) >= 9 && sizeof(AP_PASS) <= 64,
              "Set an 8–63 character Wi-Fi password in src/config.h");
static_assert(sizeof(WEB_PASS) >= 9,
              "Set a teacher dashboard password of at least 8 characters in src/config.h");

const int FIELD_COUNT = 26;
const int LOG_MAX = 90;

SoftwareSerial mb(D5, D6); // software RX, TX
ESP8266WebServer server(80);
String fields[FIELD_COUNT];
int nFields = 0;
unsigned long lastStatusAt = 0;
String logLines[LOG_MAX];
uint8_t logStart = 0, logCount = 0;
unsigned long alertSeq = 0;
String rxLine;
const char* expectedEvent = nullptr;
void readMicrobit();

void addLog(const String& event, const String& step = "", const String& detail = "") {
  int idx = (logStart + logCount) % LOG_MAX;
  if (logCount < LOG_MAX) ++logCount;
  else logStart = (logStart + 1) % LOG_MAX;
  logLines[idx] = String(millis()/1000) + "," + event + "," + step + "," + detail;
}
int split(const String& in, char separator, String* output, int maxParts) {
  int n = 0, from = 0;
  while (n < maxParts) {
    int at = in.indexOf(separator, from);
    if (at == -1) { output[n++] = in.substring(from); break; }
    output[n++] = in.substring(from, at);
    from = at + 1;
  }
  return n;
}
void handleLine(const String& line) {
  if (line.startsWith("S|")) {
    String next[FIELD_COUNT];
    // Older firmware appends extra fields that the dashboard can ignore.
    int count = split(line, '|', next, FIELD_COUNT);
    if (count == FIELD_COUNT) {
      for (int i = 0; i < FIELD_COUNT; ++i) fields[i] = next[i];
      nFields = FIELD_COUNT;
      lastStatusAt = millis();
    }
  } else if (line.startsWith("E|")) {
    String p[4];
    int count = split(line, '|', p, 4);
    if (count < 2) return;
    String eventName = p[1];
    String step = count > 2 ? p[2] : "";
    String detail = count > 3 ? p[3] : "";
    if (expectedEvent && eventName == expectedEvent) expectedEvent = nullptr;
    addLog(eventName, step, detail);
    if (eventName == "help" || eventName == "done_wait") ++alertSeq;
  }
}
String f(int i) { return (i >= 0 && i < nFields) ? fields[i] : String(""); }
bool online() { return nFields == FIELD_COUNT && lastStatusAt > 0 && (millis() - lastStatusAt < 4000); }

String jsonEsc(const String& in) {
  String out;
  out.reserve(in.length() + 8);
  for (unsigned int i = 0; i < in.length(); ++i) {
    char c = in[i];
    if (c == '\\' || c == '"') { out += '\\'; out += c; }
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else if (c == '\t') out += "\\t";
    else if ((unsigned char)c >= 32) out += c;
  }
  return out;
}
void setSafetyHeaders() {
  server.sendHeader("Cache-Control", "no-store, max-age=0");
  server.sendHeader("X-Content-Type-Options", "nosniff");
  server.sendHeader("Referrer-Policy", "no-referrer");
  server.sendHeader("Content-Security-Policy", "default-src 'self'; style-src 'self' 'unsafe-inline'; script-src 'self' 'unsafe-inline'; connect-src 'self'; img-src 'self' data:; object-src 'none'; base-uri 'none'");
}
bool teacherAuth() {
  if (server.authenticate(WEB_USER, WEB_PASS)) return true;
  server.requestAuthentication(BASIC_AUTH, "Step Buddy teacher");
  return false;
}
bool checkOrigin() {
  const String origin = server.header("Origin");
  if (origin.length() && origin != "http://192.168.4.1") {
    setSafetyHeaders(); server.send(403, "text/plain", "Origin not allowed"); return false;
  }
  return true;
}

// No remote assets or cloud; teacher page is in flash memory.
void teacherPage() {
  if (!teacherAuth()) return;
  setSafetyHeaders(); server.send_P(200, "text/html; charset=utf-8", TEACHER_PAGE);
}
String buildStatus() {
  String result;
  result.reserve(2300);
  result = "{\"online\":";
  result += online() ? "true" : "false";
  result += ",\"tplId\":\"" + f(1) + "\",\"tpl\":\"" + jsonEsc(f(2)) + "\"";
  result += ",\"step\":\"" + f(3) + "\",\"total\":\"" + f(4) + "\"";
  result += ",\"mode\":\"" + f(7) + "\",\"state\":\"" + f(8) + "\"";
  result += ",\"tokens\":\"" + f(9) + "\",\"help\":\"" + f(10) + "\",\"ack\":\"" + f(11) + "\"";
  result += ",\"restLeft\":\"" + f(12) + "\",\"hints\":\"" + f(13) + "\"";
  result += ",\"helpCount\":\"" + f(14) + "\",\"restCount\":\"" + f(15) + "\"";
  result += ",\"moveCount\":\"" + f(16) + "\",\"workSec\":\"" + f(17) + "\"";
  result += ",\"stepSec\":\"" + f(18) + "\",\"low\":\"" + f(19) + "\"";
  result += ",\"stepsDone\":\"" + f(20) + "\",\"timer\":\"" + f(21) + "\"";
  result += ",\"moveLeft\":\"" + f(22) + "\",\"expectedSec\":\"" + f(23) + "\"";
  result += ",\"moveIntervalSec\":\"" + f(24) + "\",\"restDurationSec\":\"" + f(25) + "\"";
  result += ",\"current\":\"" + jsonEsc(f(5)) + "\"";
  result += ",\"nextTitle\":\"" + jsonEsc(f(8) == "DONE" || f(6) == "-" ? String("") : f(6)) + "\"";
  result += ",\"alertSeq\":" + String(alertSeq);
  result += "}";
  return result;
}
void teacherStatus() {
  if (!teacherAuth()) return;
  setSafetyHeaders(); server.send(200, "application/json; charset=utf-8", buildStatus());
}
bool digitsSuffix(const String& c, const String& prefix, int lo, int hi) {
  if (!c.startsWith(prefix)) return false;
  String num = c.substring(prefix.length());
  if (num.length() < 1 || num.length() > 5) return false;
  for (unsigned int i=0; i<num.length(); ++i) if (num[i] < '0' || num[i] > '9') return false;
  return num.toInt() >= lo && num.toInt() <= hi;
}
bool validTeacherCommand(const String& c) {
  return c == "NEXT" || c == "BACK" || c == "PAUSE" || c == "MODE" ||
    c == "LOW" || c == "TIMER" || c == "ACK" || c == "HELPDONE" ||
    c == "RESTEND" || c == "MOVE" || c == "PLUS120" || c == "NEW" ||
    digitsSuffix(c, "TPL", 0, 2) || digitsSuffix(c, "HINT", 0, 5) ||
    digitsSuffix(c, "STEPSEC:", 60, 3600) || digitsSuffix(c, "MOVEINT:", 0, 3600) ||
    digitsSuffix(c, "RESTSEC:", 30, 900);
}
void sendMicrobit(const String& c) {
  mb.print(c);
  mb.print('\n');
}
void teacherCommand() {
  if (!teacherAuth() || !checkOrigin()) return;
  String c = server.arg("c");
  if (!validTeacherCommand(c)) { server.send(400, "text/plain", "Invalid teacher command"); return; }
  if (!online()) { server.send(503, "text/plain", "micro:bit is offline"); return; }
  readMicrobit();
  if (c == "NEW") expectedEvent = "new_session";
  if (c == "BACK") {
    if ((f(3).toInt() <= 1 && f(8) != "DONE") || f(8) == "REST" || f(8) == "RESTOVER" || f(8) == "MOVE") {
      server.send(409, "text/plain", "Cannot go back in current state");
      return;
    }
    expectedEvent = "step_back";
  }
  sendMicrobit(c);
  addLog("teacher_cmd", "", c);
  if (expectedEvent) {
    const unsigned long started = millis();
    while (expectedEvent && millis() - started < 2500) {
      readMicrobit();
      delay(1);
    }
    bool confirmed = !expectedEvent;
    expectedEvent = nullptr;
    setSafetyHeaders();
    server.send(confirmed ? 200 : 504, "text/plain", confirmed ? "Confirmed" : "No micro:bit confirmation");
    return;
  }
  setSafetyHeaders(); server.send(202, "text/plain", "Accepted by ESP; verify on micro:bit status");
}

void teacherLog() {
  if (!teacherAuth()) return;
  String csv = "device_uptime_s,event,step,detail\n";
  csv.reserve(4500);
  for (int i=0; i<logCount; ++i) csv += logLines[(logStart+i)%LOG_MAX] + "\n";
  setSafetyHeaders();
  server.sendHeader("Content-Disposition", "attachment; filename=stepbuddy3-events.csv");
  server.send(200, "text/csv; charset=utf-8", csv);
}
void teacherClearLog() {
  if (!teacherAuth() || !checkOrigin()) return;
  for (int i=0; i<LOG_MAX; ++i) logLines[i] = "";
  logCount = 0; logStart = 0;
  setSafetyHeaders(); server.send(200, "text/plain", "Log cleared");
}

void setup() {
  Serial.begin(115200);
  mb.begin(9600, SWSERIAL_8N1, D5, D6, false, 512, 512);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME, AP_PASS);
  server.collectHeaders("Origin");

  server.on("/", HTTP_GET, []() { setSafetyHeaders(); server.sendHeader("Location", "/teacher"); server.send(302, "text/plain", "Go to /teacher"); });
  server.on("/teacher", HTTP_GET, teacherPage);
  server.on("/api/teacher/status", HTTP_GET, teacherStatus);
  server.on("/api/teacher/cmd", HTTP_POST, teacherCommand);
  server.on("/api/teacher/log.csv", HTTP_GET, teacherLog);
  server.on("/api/teacher/clear-log", HTTP_POST, teacherClearLog);
  server.onNotFound([]() { setSafetyHeaders(); server.send(404, "text/plain", "Not found"); });
  server.begin();

  Serial.println("Step Buddy 3.0 ready");
  Serial.print("Teacher: http://"); Serial.print(WiFi.softAPIP()); Serial.println("/teacher");
}

void readMicrobit() {
  while (mb.available()) {
    char c = (char)mb.read();
    if (c == '\n') {
      rxLine.trim();
      if (rxLine.length() && rxLine.length() < 440) handleLine(rxLine);
      rxLine = "";
    } else if (rxLine.length() < 440) rxLine += c;
    else rxLine = ""; // Drop oversized frame; resync on next LF.
  }
}

void loop() {
  server.handleClient();
  readMicrobit();
}

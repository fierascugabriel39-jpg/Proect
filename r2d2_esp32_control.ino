/*
 * R2-D2 MOTION CONTROL v3.0 — Hibrid Claude+Gemini
 * ==================================================
 * Hardware confirmat 2026-06-07:
 *   Driver:  2x IBT-2 (BTS7960) — R_EN/L_EN pe 5V hardware
 *   ESP32:   LAFVIN WROOM-32E
 *   Protocol: UDP WiFi, port 4210
 *
 * Ce aduce fiecare:
 *   Claude  → LEDC 20kHz, WiFi reconectare, status JSON, buffer 255
 *   Gemini  → rotire continuă fără delay fix, timeout 1500ms, cmd v:150
 */

#include <WiFi.h>
#include <WiFiUdp.h>

// ── REȚEA ──────────────────────────────────────────────
const char* SSID     = "NUME_HOTSPOT";
const char* PASS     = "PAROLA";
const int   UDP_PORT = 4210;

// ── MOTOR A — IBT-2 #1 — CONFIRMAT FIZIC ──────────────
#define RPWM_A 25
#define LPWM_A 26
#define CH_RA  0
#define CH_LA  1

// ── MOTOR B — IBT-2 #2 — CONFIRMAT FIZIC 2026-06-12 ───
#define RPWM_B 32
#define LPWM_B 33
#define CH_RB  2
#define CH_LB  3

// ── PWM ────────────────────────────────────────────────
#define PWM_FREQ  20000   // 20kHz — inaudibil, optim IBT-2
#define PWM_BITS  8       // 0-255

// ── VITEZĂ ─────────────────────────────────────────────
int viteza      = 200;   // viteză mișcare liniară
int vitezaViraj = 180;   // viteză rotire (separat — mai precis)

// ── SAFETY ─────────────────────────────────────────────
const unsigned long TIMEOUT_MS = 1500;  // stop dacă tace 1.5s
unsigned long ultimaComanda    = 0;
bool          motorsRunning    = false;

// ── OBIECTE ────────────────────────────────────────────
WiFiUDP udp;
char    buf[255];

// ══════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[R2-D2 v3.0 Hibrid] Pornire...");

  initMotors();
  opreste();
  connectWiFi();

  udp.begin(UDP_PORT);
  Serial.printf("[UDP] Activ pe port %d\n", UDP_PORT);
  Serial.println("[R2-D2] Gata.\n");
}

// ══════════════════════════════════════════════════════
void loop() {
  // Reconectare automată WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Pierdut — reconectare...");
    opreste();
    connectWiFi();
  }

  // Safety timeout — stop automat la 1.5s fără comandă
  if (motorsRunning && millis() - ultimaComanda > TIMEOUT_MS) {
    Serial.println("[SAFETY] Timeout 1.5s — stop automat.");
    opreste();
  }

  // Citire UDP
  int sz = udp.parsePacket();
  if (!sz) return;

  int len = udp.read(buf, sizeof(buf) - 1);
  if (len <= 0) return;
  buf[len] = '\0';

  String cmd = String(buf);
  cmd.trim();
  cmd.toLowerCase();

  Serial.println("[CMD] " + cmd);
  ultimaComanda = millis();
  executa(cmd);
}

// ══════════════════════════════════════════════════════
void executa(const String& cmd) {
  if      (cmd == "fata"   || cmd == "inainte") inainte();
  else if (cmd == "inapoi" || cmd == "spate")   inapoi();
  else if (cmd == "left"   || cmd == "stanga")  stanga();   // rotire continuă
  else if (cmd == "right"  || cmd == "dreapta") dreapta();  // rotire continuă
  else if (cmd == "stop"   || cmd == "opreste") opreste();
  else if (cmd == "status")                     sendStatus();
  else if (cmd.startsWith("v:")) {              // ex: v:150 → setează viteza
    int val = cmd.substring(2).toInt();
    if (val >= 0 && val <= 255) {
      viteza = val;
      vitezaViraj = constrain(val - 20, 0, 255);
      Serial.printf("[CONFIG] Viteza → %d, Viraj → %d\n", viteza, vitezaViraj);
    }
  }
  else Serial.println("[WARN] Necunoscut: " + cmd);
}

// ══════════════════════════════════════════════════════
// MIȘCARE
// IBT-2: RPWM=spd + LPWM=0 → înainte | RPWM=0 + LPWM=spd → înapoi
// ══════════════════════════════════════════════════════
void inainte() {
  motor(CH_RA, CH_LA, viteza, true);
  motor(CH_RB, CH_LB, viteza, true);
  motorsRunning = true;
  Serial.printf("[MOT] Înainte @ %d\n", viteza);
}

void inapoi() {
  motor(CH_RA, CH_LA, viteza, false);
  motor(CH_RB, CH_LB, viteza, false);
  motorsRunning = true;
  Serial.printf("[MOT] Înapoi @ %d\n", viteza);
}

void stanga() {
  // Tracțiune tanc — rotire continuă, oprire la "stop"
  motor(CH_RA, CH_LA, vitezaViraj, false);  // A înapoi
  motor(CH_RB, CH_LB, vitezaViraj, true);   // B înainte
  motorsRunning = true;
  Serial.printf("[MOT] Stânga @ %d (continuu)\n", vitezaViraj);
}

void dreapta() {
  // Tracțiune tanc — rotire continuă, oprire la "stop"
  motor(CH_RA, CH_LA, vitezaViraj, true);   // A înainte
  motor(CH_RB, CH_LB, vitezaViraj, false);  // B înapoi
  motorsRunning = true;
  Serial.printf("[MOT] Dreapta @ %d (continuu)\n", vitezaViraj);
}

void opreste() {
  ledcWrite(CH_RA, 0); ledcWrite(CH_LA, 0);
  ledcWrite(CH_RB, 0); ledcWrite(CH_LB, 0);
  motorsRunning = false;
  Serial.println("[MOT] STOP");
}

// ══════════════════════════════════════════════════════
// UTILITĂȚI
// ══════════════════════════════════════════════════════
void motor(int chR, int chL, int spd, bool inainte) {
  ledcWrite(chR, inainte ? spd : 0);
  ledcWrite(chL, inainte ? 0 : spd);
}

void initMotors() {
  ledcSetup(CH_RA, PWM_FREQ, PWM_BITS); ledcAttachPin(RPWM_A, CH_RA);
  ledcSetup(CH_LA, PWM_FREQ, PWM_BITS); ledcAttachPin(LPWM_A, CH_LA);
  ledcSetup(CH_RB, PWM_FREQ, PWM_BITS); ledcAttachPin(RPWM_B, CH_RB);
  ledcSetup(CH_LB, PWM_FREQ, PWM_BITS); ledcAttachPin(LPWM_B, CH_LB);
  Serial.println("[MOTOR] LEDC 20kHz OK");
}

void connectWiFi() {
  WiFi.begin(SSID, PASS);
  Serial.print("[WiFi] Conectare");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500); Serial.print("."); tries++;
  }
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("\n[WiFi] IP: " + WiFi.localIP().toString());
  else
    Serial.println("\n[WiFi] EROARE — reîncerc la loop.");
}

void sendStatus() {
  String r = "{\"ip\":\"" + WiFi.localIP().toString() +
             "\",\"viteza\":" + viteza +
             ",\"viraj\":" + vitezaViraj +
             ",\"uptime\":" + millis()/1000 +
             ",\"pwm_hz\":20000,\"driver\":\"IBT2x2\"}";
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(r);
  udp.endPacket();
  Serial.println("[STATUS] " + r);
}

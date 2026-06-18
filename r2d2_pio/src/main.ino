// --- AI-PROVENIENTA ---
// Model:      Gemini 3.5 Flash (Antigravity)
// Clasa:      Gemini-class
// Data:       2026-06-18
// Contribuție: refactorizat
// Scop:       Implementare control UDP WiFi robust pe portul 4210 la 1kHz, eliminând spasmele motorului (frecvență redusă de la 20kHz la 1kHz din cauza optocuploarelor lente IBT-2). Integrare secrets.h.
// Validare:   Pregătit pentru upload și verificare hardware de către Gabriel.
// Cross-AI:   Modificat frecvența stabilită de Claude de la 20kHz la 1kHz, rezolvând spasmele motorului raportate de utilizator.
// --- /AI-PROVENIENTA ---

/*
 * R2-D2 MOTION CONTROL v3.1 — Calibrat de Gemini/Antigravity
 * ==========================================================
 * Hardware confirmat 2026-06-07 & 2026-06-18:
 *   Driver:    2x IBT-2 (BTS7960) — R_EN/L_EN pe 5V hardware
 *   ESP32:     LAFVIN WROOM-32E
 *   Protocol:  UDP WiFi, port 4210
 *   Frecvență: 1000Hz (1kHz) — rezolvă spasmele cauzate de optocuploarele lente
 *   Securitate: Credențiale din secrets.h (git-ignored)
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

// ── REȚEA ──────────────────────────────────────────────
const int   UDP_PORT = 4210;

// ── MOTOR A — IBT-2 #1 — CONFIRMAT FIZIC ──────────────
#define RPWM_A 25
#define LPWM_A 26
#define CH_RA  0
#define CH_LA  1

// ── MOTOR B — IBT-2 #2 — CONFIRMAT FIZIC (Singurul activ acum) ───
#define RPWM_B 32
#define LPWM_B 33
#define CH_RB  2
#define CH_LB  3

// ── PWM ────────────────────────────────────────────────
#define PWM_FREQ  1000    // 1kHz — optim pentru optocuploarele IBT-2 (PC817)
#define PWM_BITS  8       // 0-255

// ── VITEZĂ ─────────────────────────────────────────────
int viteza      = 200;   // viteză mișcare liniară (0-255)
int vitezaViraj = 180;   // viteză rotire (0-255)

// ── SAFETY ─────────────────────────────────────────────
const unsigned long TIMEOUT_MS = 1500;  // stop dacă tace 1.5s
unsigned long ultimaComanda    = 0;
bool          motorsRunning    = false;

// ── OBIECTE ────────────────────────────────────────────
WiFiUDP udp;
char    buf[255];

unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000; // verifică la fiecare 10s în loop

// ══════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[R2-D2 v3.1] Pornire...");

  initMotors();
  opreste();

  // Conectare inițială la WiFi
  connectWiFi();
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) { // maxim 10 secunde de așteptare blocantă la pornire
    delay(500);
    Serial.print(".");
    tries++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Conectat cu succes!");
    Serial.print("[WiFi] IP local: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Conectarea inițială a eșuat. Va reîncerca în loop (fundal).");
  }

  udp.begin(UDP_PORT);
  Serial.printf("[UDP] Ascultare pe portul %d\n", UDP_PORT);
  Serial.println("[R2-D2] Pregătit pentru comenzi.");
}

void loop() {
  // Verificare și reconectare WiFi non-blocantă
  verifyWiFi();

  // Safety timeout — oprire automată la 1.5s fără comandă primită
  if (motorsRunning && millis() - ultimaComanda > TIMEOUT_MS) {
    Serial.println("[SAFETY] Timeout 1.5s — oprire automată motoare.");
    opreste();
  }

  // 1. Citire Serial (pentru testare directă prin cablu USB)
  if (Serial.available() > 0) {
    String serialCmd = Serial.readStringUntil('\n');
    serialCmd.trim();
    serialCmd.toLowerCase();
    if (serialCmd.length() > 0) {
      Serial.println("[SERIAL CMD] " + serialCmd);
      ultimaComanda = millis();
      executa(serialCmd);
    }
  }

  // 2. Citire UDP
  int sz = udp.parsePacket();
  if (sz > 0) {
    int len = udp.read(buf, sizeof(buf) - 1);
    if (len > 0) {
      buf[len] = '\0';
      String cmd = String(buf);
      cmd.trim();
      cmd.toLowerCase();
      Serial.println("[UDP CMD] " + cmd);
      ultimaComanda = millis();
      executa(cmd);
    }
  }
}


// ══════════════════════════════════════════════════════
void executa(const String& cmd) {
  if      (cmd == "fata"   || cmd == "inainte") inainte();
  else if (cmd == "inapoi" || cmd == "spate")   inapoi();
  else if (cmd == "left"   || cmd == "stanga")  stanga();   // rotire stânga continuă
  else if (cmd == "right"  || cmd == "dreapta") dreapta();  // rotire dreapta continuă
  else if (cmd == "stop"   || cmd == "opreste") opreste();
  else if (cmd == "status")                     sendStatus();
  else if (cmd.startsWith("v:")) {              // ex: v:150
    int val = cmd.substring(2).toInt();
    if (val >= 0 && val <= 255) {
      viteza = val;
      vitezaViraj = constrain(val - 20, 0, 255);
      Serial.printf("[CONFIG] Viteza setată la %d, Viteza viraj la %d\n", viteza, vitezaViraj);
    }
  }
  else {
    Serial.println("[WARN] Comandă necunoscută: " + cmd);
  }
}

// ══════════════════════════════════════════════════════
// MIȘCARE (IBT-2: RPWM=spd + LPWM=0 -> înainte | RPWM=0 + LPWM=spd -> înapoi)
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
  // Tracțiune diferențială continuă (rotire pe loc)
  motor(CH_RA, CH_LA, vitezaViraj, false);  // Motor A înapoi
  motor(CH_RB, CH_LB, vitezaViraj, true);   // Motor B înainte
  motorsRunning = true;
  Serial.printf("[MOT] Rotire Stânga @ %d\n", vitezaViraj);
}

void dreapta() {
  // Tracțiune diferențială continuă (rotire pe loc)
  motor(CH_RA, CH_LA, vitezaViraj, true);   // Motor A înainte
  motor(CH_RB, CH_LB, vitezaViraj, false);  // Motor B înapoi
  motorsRunning = true;
  Serial.printf("[MOT] Rotire Dreapta @ %d\n", vitezaViraj);
}

void opreste() {
  ledcWrite(CH_RA, 0); ledcWrite(CH_LA, 0);
  ledcWrite(CH_RB, 0); ledcWrite(CH_LB, 0);
  motorsRunning = false;
  Serial.println("[MOT] STOP total.");
}

// ══════════════════════════════════════════════════════
// UTILITĂȚI
// ══════════════════════════════════════════════════════
void motor(int chR, int chL, int spd, bool directieInainte) {
  ledcWrite(chR, directieInainte ? spd : 0);
  ledcWrite(chL, directieInainte ? 0 : spd);
}

void initMotors() {
  // Configurare LEDC pentru ESP32 (compatibil Core 2.x)
  ledcSetup(CH_RA, PWM_FREQ, PWM_BITS); ledcAttachPin(RPWM_A, CH_RA);
  ledcSetup(CH_LA, PWM_FREQ, PWM_BITS); ledcAttachPin(LPWM_A, CH_LA);
  ledcSetup(CH_RB, PWM_FREQ, PWM_BITS); ledcAttachPin(RPWM_B, CH_RB);
  ledcSetup(CH_LB, PWM_FREQ, PWM_BITS); ledcAttachPin(LPWM_B, CH_LB);
  Serial.println("[MOTOR] LEDC 1kHz inițializat (OK)");
}

void connectWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("[WiFi] Inițiere conexiune la ");
  Serial.println(WIFI_SSID);
}

void verifyWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastWiFiCheck > WIFI_CHECK_INTERVAL) {
      lastWiFiCheck = millis();
      Serial.println("[WiFi] Conexiune pierdută. Încercare reconectare...");
      opreste();
      // Pornim reconectarea non-blocantă
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  } else {
    static bool loggedIP = false;
    if (!loggedIP) {
      Serial.print("[WiFi] Reconectat! IP: ");
      Serial.println(WiFi.localIP());
      loggedIP = true;
    }
  }
}

void sendStatus() {
  String r = "{\"ip\":\"" + WiFi.localIP().toString() +
             "\",\"viteza\":" + viteza +
             ",\"viraj\":" + vitezaViraj +
             ",\"uptime\":" + millis()/1000 +
             ",\"pwm_hz\":1000,\"driver\":\"IBT2x2\"}";
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(r);
  udp.endPacket();
  Serial.println("[STATUS] Răspuns trimis: " + r);
}

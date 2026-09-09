# STARE PROIECT — R2-D2 + Antigravity

> Document de context. Ține-l deschis când reiei lucrul.
> Ultima actualizare: 2026-09-09 (Claude Code)
> Ultimul commit de cod real: `86cdcd1` — 18 iunie 2026, "Faza 1 robot R2D2".

---

## 0. SITUAȚIA CURENTĂ — citește asta prima dată

*Confirmat de Gabriel, 9 septembrie 2026.*

**Gabriel este în Germania, la muncă. Robotul este acasă în România.
Calculatorul lui NU este cu el. Are doar telefonul.**

Consecința e mai largă decât lipsa hardware-ului: **Gabriel nu poate rula nimic** —
nici comenzi, nici scripturi, nici compilare, nici upload. Nu-i cere să execute nimic
și nu-i da comenzi „de copiat". Nu are unde să le pună.

### Cum se lucrează în perioada asta — „podul"

    Gabriel (telefon)  ←→  Claude Code (calculator în cloud)  ←→  GitHub
       citește, decide,        scrie codul, comite, urcă        păstrează tot
       aprobă                                                    până acasă

- **Claude face toată munca de execuție.** Scrie, modifică, comite, urcă pe GitHub.
- **Gabriel citește pe telefon și decide.** Explicații scurte, fără comenzi de rulat.
- **GitHub e locul de întâlnire.** Tot ce se produce acum îl așteaptă acolo, gata
  de folosit când ajunge la calculator, în România.

### Ce se poate face REAL în perioada asta

| Se poate | De ce |
|---|---|
| Capul R2-D2 3D (`r2d2_head.scad`) | design pur, Claude îl scrie integral |
| Documentație și ordine în proiect | text, zero execuție |
| Cod nou pentru robot (Faza 2) | se poate scrie și comite; testarea așteaptă |

| NU se poate | De ce |
|---|---|
| Orice test fizic pe robot | robotul e în România |
| Upload pe placă, monitor serial | nu are placa și nici calculator |
| Antigravity / VoiceCommander, în cea mai mare parte | codul principal (`bus.py`, `orchestrator.py`) **nu e în acest repo**, stă doar pe calculatorul de acasă. Din proiect se văd doar scripturile mici care îl apelează. |

### Necunoscut încă
- când are loc întoarcerea în România;
- dacă motoarele au encodere (blochează PID-ul din Faza 2);
- ce ecran de 7" și ce telefon Huawei anume (dimensiuni reale pentru capul 3D);
- dacă există imprimantă 3D și ce dimensiune de pat are.

> **Notă pentru orice asistent AI care citește acest fișier:** nu ai memorie între
> sesiuni. Tot ce știi despre situația reală a lui Gabriel este scris aici. Nu-i cere
> să ruleze comenzi și nu presupune că are calculatorul sau robotul la îndemână —
> întreabă întâi dacă secțiunea asta pare veche.

---

## 1. Unde ești acum (pe scurt)

**Faza 1 e TERMINATĂ și funcțională.** Robotul se mișcă, motorul merge fin, comenzile
se trimit prin WiFi. Codul e comis și urcat pe GitHub. Nu ai nimic nefinalizat în git.

Ce a rămas de făcut: **Faza 2** (accelerație lină / PID) — vezi secțiunea 6.
> **Dar citește întâi secțiunea 0** — testele fizice nu sunt posibile momentan.

Starea git: branch-ul de lucru, `main` și GitHub sunt toate la același commit. Nimic
necomis, nimic nepusat.

---

## 2. Hardware confirmat fizic

| Componentă | Detaliu |
|---|---|
| Placă | ESP32 LAFVIN WROOM-32E |
| Driver motoare | 2x IBT-2 (BTS7960), cu optocuploare PC817 |
| R_EN / L_EN | legate hardware la 5V (nu se controlează din cod) |
| Motor A | RPWM = GPIO **25**, LPWM = GPIO **26** |
| Motor B | RPWM = GPIO **32**, LPWM = GPIO **33** |
| Port USB (Linux) | `/dev/ttyACM0` |
| Comunicație | UDP, port **4210** |
| Baud serial | 115200 |

**Lecția cea mai importantă a Fazei 1:** PWM-ul a fost coborât de la 20kHz la **1kHz**.
Optocuploarele PC817 din IBT-2 sunt lente și nu apucau să comute complet la 20kHz —
de aici spasmele motorului. La 1kHz merge fin. **Nu urca frecvența înapoi.**

---

## 3. Fișierele care contează

    r2d2_pio/                        ← proiectul PlatformIO (upload din terminal)
      platformio.ini                 ← config placă + port USB
      src/main.ino                   ← CODUL PRINCIPAL
      src/secrets.h                  ← NU e în git (parola WiFi). Trebuie recreat!

    r2d2_esp32_control/
      r2d2_esp32_control.ino         ← COPIE IDENTICĂ, pentru Arduino IDE

    r2d2_udp_test.py                 ← trimite comenzi de pe laptop către robot
    r2d2_head.scad                   ← capul R2-D2 pentru printare 3D (OpenSCAD)

⚠️ **Cele două `.ino` sunt identice acum.** Dacă modifici unul, copiază-l și în celălalt,
altfel ajungi să testezi o versiune și să comiți alta.

Restul fișierelor (`test_kimi.py`, `olx_kimi_sync.py`, `start_antigravity.sh`,
`fix_mic.sh`, `test_gemini.py`, `test_piramida.py`, `config_olx.json`) țin de proiectul
separat **Antigravity / VoiceCommander**, nu de robot.

---

## 4. PAS CU PAS: cum repornești lucrul la robot

> ⚠️ **Necesită robotul fizic — vezi secțiunea 0.** Pașii de mai jos sunt pentru
> momentul în care ai placa în față. Nu sunt de executat de la distanță.

### Pasul 0 — Ia codul la zi
    cd ~/Proect
    git checkout main
    git pull origin main

### Pasul 1 — Recreează `secrets.h` (OBLIGATORIU, altfel nu compilează)
Fișierul e intenționat exclus din git, ca să nu-ți ajungă parola WiFi pe GitHub.
După fiecare clonare nouă a repo-ului trebuie recreat manual:

    cat > r2d2_pio/src/secrets.h <<'FIN'
    #define WIFI_SSID "numele_retelei_tale"
    #define WIFI_PASS "parola_ta"
    FIN

Apoi copiază-l și lângă sketch-ul de Arduino IDE:

    cp r2d2_pio/src/secrets.h r2d2_esp32_control/secrets.h

> Robotul și laptopul trebuie să fie pe **aceeași rețea WiFi**, și trebuie să fie o
> rețea de **2.4 GHz** — ESP32 nu vede rețelele de 5 GHz.

### Pasul 2 — Conectează ESP32 prin USB și verifică portul
    ls /dev/ttyACM* /dev/ttyUSB*

Dacă apare `/dev/ttyUSB0` în loc de `/dev/ttyACM0`, schimbă cele două linii de port
din `r2d2_pio/platformio.ini`.

Dacă primești "permission denied", o dată pentru totdeauna:

    sudo usermod -a -G dialout $USER

(apoi deloghează-te și loghează-te înapoi)

### Pasul 3 — Compilează și urcă pe placă
    cd ~/Proect/r2d2_pio
    pio run --target upload

Dacă vrei doar să verifici că se compilează, fără să urci: `pio run`

### Pasul 4 — Deschide monitorul serial și află IP-ul robotului
    pio device monitor

Apasă butonul **EN/RST** de pe ESP32. Ar trebui să vezi:

    [R2-D2 v3.1] Pornire...
    [MOTOR] LEDC 1kHz inițializat (OK)
    [WiFi] Conectat cu succes!
    [WiFi] IP local: 192.168.x.xxx      ← NOTEAZĂ ACEST IP
    [UDP] Ascultare pe portul 4210
    [R2-D2] Pregătit pentru comenzi.

Ieși din monitor cu `Ctrl+C`.

### Pasul 5 — Testează întâi prin cablu (fără WiFi)
Cât timp ești în monitorul serial, poți scrie direct comenzi și apăsa Enter:

    stop
    v:120
    fata
    stop

Începe cu `v:120` (viteză mică). Ridică robotul de pe podea sau pune-l pe un suport,
ca să nu-ți fugă de pe masă.

### Pasul 6 — Testează prin WiFi
Pune IP-ul notat la Pasul 4 în `r2d2_udp_test.py`, linia `UDP_IP = "..."`. Apoi:

    cd ~/Proect
    python3 r2d2_udp_test.py status      # verifică legătura
    python3 r2d2_udp_test.py fata
    python3 r2d2_udp_test.py stop

---

## 5. Comenzile pe care le înțelege robotul

| Comandă | Efect |
|---|---|
| `fata` / `inainte` | ambele motoare înainte |
| `inapoi` / `spate` | ambele motoare înapoi |
| `stanga` / `left` | rotire pe loc, stânga |
| `dreapta` / `right` | rotire pe loc, dreapta |
| `stop` / `opreste` | oprire totală |
| `v:150` | setează viteza (0–255); viteza de viraj devine automat viteză − 20 |
| `status` | răspunde prin UDP cu IP, viteză, uptime |

**Protecții deja active în cod:**
- dacă nu primește nicio comandă 1.5 secunde → oprește singur motoarele;
- dacă pică WiFi-ul → oprește motoarele și reîncearcă conexiunea la fiecare 10s.

---

## 6. Ce urmează

Codul actual e marcat explicit ca **experiment funcțional, nu formulă finală**.
Lista e împărțită după ce se poate face acum și ce așteaptă robotul.

### 6a. Se poate face ACUM, de la distanță

| # | Ce | Fișier | Stare |
|---|---|---|---|
| 1 | Capul R2-D2 pentru ecran 7" | `r2d2_head.scad` | desenat, nevalidat |
| 2 | Antigravity — doar fișierele din repo | `test_kimi.py`, `olx_kimi_sync.py`, `start_antigravity.sh` | limitat: codul principal e pe calculatorul de acasă, nu aici |
| 3 | Documentație și ordine în proiect | acest fișier | în lucru |

**Despre capul 3D — ce lipsește din `r2d2_head.scad`:**
- nu are găuri de aerisire (telefonul și ecranul se încălzesc într-o carcasă închisă);
- nu are spate detașabil — carcasa e o singură bucată, deci odată montat telefonul
  nu-l mai poți scoate;
- gâtul e un simplu cilindru lipit, fără mod de prindere de corpul robotului;
- gaura de cabluri e una singură, 30x20mm, în centrul spatelui;
- dimensiunile ecranului (165x100x12mm) sunt notate ca **estimate**, nu măsurate;
- nu s-a verificat dacă piesa încape pe patul imprimantei 3D (≈173x118mm în plan).

### 6b. Așteaptă robotul fizic (întoarcerea în România)

1. **Verificare Motor A** — în comentarii scrie că doar Motor B era activ la ultimul
   test. De verificat fizic dacă ambele merg. **Primul lucru de făcut la întoarcere.**
2. **Accelerație lină (ramping)** — acum motorul sare direct de la 0 la viteza setată.
   Se rezolvă cu o creștere treptată a PWM-ului către o valoare-țintă.
3. **Control PID** — merge doar cu encodere pe motoare. Nu se știe dacă există
   encodere; dacă nu, ăsta e primul lucru de cumpărat pentru Faza 2.

## 7. Reguli de lucru (ca să nu mai pierzi contextul)

- **Un singur lucru odată**, și îl comiți când merge. Mesajul de commit spune
  *ce s-a schimbat și de ce* — commit-ul `86cdcd1` e modelul bun.
- **Nu urca niciodată `secrets.h`** — e deja blocat în `.gitignore`, lasă-l așa.
- Când modifici codul robotului, **modifică ambele copii** ale sketch-ului (secțiunea 3).
- Când reiei lucrul după o pauză: citește acest fișier, apoi `git log --oneline -5`.
- Actualizează **secțiunea 0** ori de câte ori se schimbă situația reală (locație,
  acces la hardware, ce calculator ai). E secțiunea de care depinde tot restul.
- Actualizează secțiunile 1 și 6 după fiecare sesiune de lucru.

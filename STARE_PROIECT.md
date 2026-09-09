# STARE PROIECT — R2-D2 + Antigravity

> Document de context. Ține-l deschis când reiei lucrul.
> Ultima actualizare: 2026-09-09 (Claude Code)
> Ultimul commit de cod real: `86cdcd1` — 18 iunie 2026, "Faza 1 robot R2D2".

---

## 0. SITUAȚIA CURENTĂ — citește asta prima dată

*Confirmat de Gabriel, 9 septembrie 2026.*

**Gabriel este în Germania, la muncă. Robotul și calculatorul sunt acasă,
în România. Are cu el doar telefonul.**

### Calendar — perioada asta se termină în 2 zile

| Data | Ce se întâmplă |
|---|---|
| **9 sept.** (azi) | Germania. Doar telefon. Se lucrează prin „pod" (mai jos). |
| **10 sept., seara** | Pleacă spre casă. |
| **10 sept., noaptea** | Odihnă. Nimic de lucru. |
| **11 sept., ziua** | **Activ pe calculatorul de acasă.** Robotul devine accesibil. |

Din 11 septembrie situația de mai jos NU mai este valabilă — se poate lucra normal,
cu execuție și teste fizice. **Vezi secțiunea 4b: planul primei zile acasă.**

Consecința asupra robotului: **niciun test fizic** — placa e în România.

**ATENȚIE, corectat 9 sept. 2026:** o versiune anterioară a acestui document spunea
că „Gabriel nu poate rula nimic". **Afirmație greșită**, scrisă fără verificare.
Vezi secțiunea 0b — există un pod de sesiuni, iar pe telefon rulează Claude Code CLI
cu shell real.

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
| Cod nou pentru robot (Faza 2) | se poate scrie, dar **nu merită acum** — pe 11 se scrie direct cu testare pe placă, ceea ce e mult mai eficient |

| NU se poate | De ce |
|---|---|
| Orice test fizic pe robot | robotul e în România |
| Upload pe placă, monitor serial | nu are placa și nici calculator |
| Antigravity / VoiceCommander, în cea mai mare parte | codul principal (`bus.py`, `orchestrator.py`) **nu e în acest repo**, stă doar pe calculatorul de acasă. Din proiect se văd doar scripturile mici care îl apelează. |

### Stare: STAND BY (din 9 sept. 2026, seara)

Nu se începe nicio lucrare nouă până pe 11 septembrie. Capul 3D a rămas nedemarat —
se face acasă, unde poate fi și văzut. Nimic în lucru, nimic neterminat.

### Unelte disponibile pe calculatorul de acasă

Menționate de Gabriel, de confirmat la fața locului pe 11 sept.:
- **adb** — telefonul se conectează prin USB la calculator;
- **scrcpy** — oglindirea ecranului telefonului pe calculator;
- unealtă de blocare a secretelor la commit, configurată global (nume neidentificat —
  probabil git-secrets, gitleaks sau detect-secrets; nu a lăsat urme în acest repo);
- PlatformIO, folosit în iunie pentru upload headless pe ESP32.

### Necunoscut încă
- dacă motoarele au encodere (blochează PID-ul din Faza 2);
- ce ecran de 7" și ce telefon Huawei anume (dimensiuni reale pentru capul 3D);
- dacă există imprimantă 3D și ce dimensiune de pat are.

> **Notă pentru orice asistent AI care citește acest fișier:** nu ai memorie între
> sesiuni. Tot ce știi despre situația reală a lui Gabriel este scris aici. Nu-i cere
> să ruleze comenzi și nu presupune că are calculatorul sau robotul la îndemână —
> întreabă întâi dacă secțiunea asta pare veche.

---

## 0b. „PODUL" — sesiuni legate între ele

Gabriel a construit un sistem de sesiuni Claude Code legate, ca să poată lucra și de
pe telefon. **Constatat prin verificare directă, 9 sept. 2026** (nu presupus):

### Ce s-a confirmat — testat în ambele direcții, 9 sept. 2026

**Telefon → cloud: FUNCȚIONEAZĂ.** Sesiunea de pe telefon (Redmi, Termux/proot,
`session_01X1PdY51wTvvP9GzGQtfReP`, podită prin Remote Control) a trimis un mesaj
către această sesiune cloud și **a ajuns**. De acolo, sesiunea cloud se vede în
`ListAgents` și unealta `SendMessage` e disponibilă.

**Cloud → telefon: NU FUNCȚIONEAZĂ.** Motiv aflat din răspunsul serverului, nu
presupus:

> *„this cloud session cannot message other sessions yet — its credential is
> accepted for its own work but not for delivering to another session"*

Adică sesiunile cloud au o legitimație care le permite să-și facă treaba proprie,
dar nu și să livreze mesaje altor sesiuni. Nu e o setare de pornit — e o limitare
a platformei, la data asta.

**Ipoteza anterioară a fost greșită.** Se presupusese că lipsește Remote Control
activat în sesiunea emitentă. Nu asta era: `ListAgents` din cloud raportează „niciun
agent accesibil" chiar și când sesiunea de pe telefon e activă și vede cloud-ul.
Cauza e legitimația, nu configurarea.

### Ce înseamnă practic

Podul e **cu sens unic**: telefonul poate împinge stări, întrebări și rezultate către
o sesiune cloud; sesiunea cloud nu poate iniția în sens invers.

Modul de lucru care rezultă:
- **telefonul conduce**, cloud-ul răspunde în transcriptul lui;
- Gabriel citește răspunsul cloud-ului în transcript, nu primit înapoi pe telefon;
- **GitHub rămâne canalul comun** pentru orice trebuie să supraviețuiască: ce scrie
  cloud-ul acolo, telefonul citește, și invers.

### Unelte de pe telefon (din datele sesiunilor)

Termux, Wireless Debug, adb — telefonul are shell real, nu doar ecran.

---

## 0c. FIRUL — cum se leagă lucrurile între ele

Restul documentului spune *ce este*. Secțiunea asta spune *de ce a ajuns așa* și
*ce hotărăște mai departe*. Fiecare lanț se citește: cauză → efect → ce obligă în viitor.

### 1. De ce 1kHz, și de ce nu se mai urcă niciodată

Driverele IBT-2 au optocuploare PC817 **→** optocuploarele comută lent **→** la 20kHz
nu apucau să comute complet **→** motorul avea spasme **→** coborât la 1kHz **→** merge fin.

**Mai departe:** accelerația lină din Faza 2 se scrie **peste** 1kHz, nu în locul lui.
Dacă cineva „optimizează" cândva frecvența în sus, spasmele revin — și va părea o
problemă de algoritm, deși e aceeași problemă veche de hardware. *Simptomul va minți.*

### 2. De ce Motor A blochează Faza 2

UDP nu garantează livrarea **→** o comandă se poate pierde **→** inclusiv un „stop"
**→** de aceea există timeout la 1.5s și watchdog pe WiFi, care opresc singure motoarele.

**Mai departe:** orice comandă nouă adăugată în Faza 2 **trebuie** să reîmprospăteze
`ultimaComanda`. Dacă accelerația lină e scrisă ca o acțiune de durată care nu
actualizează ceasul ăla, robotul se va opri singur în mijlocul mișcării — și va părea
că accelerația e stricată, deși protecția își face treaba corect. *Al doilea simptom
care va minți.*

### 3. De ce ordinea „Motor A întâi, PID după" nu e negociabilă

În comentarii scrie că la ultimul test doar Motor B era activ **→** nu se știe dacă e
cablaj, driver sau alimentare **→** cauza e nelămurită de trei luni.

**Mai departe:** PID-ul citește encodere și corectează diferența dintre motoare. Pornit
peste un motor care nu se învârte, va încerca la nesfârșit să compenseze ceva ce nu se
poate compensa. Vei da vina pe reglaj și vei pierde zile. *De asta verificarea Motor A
e pasul C din planul zilei de 11, înaintea oricărui cod nou.*

### 4. De ce `secrets.h` se verifică, nu se recreează orbește

Repo-ul e public **→** credențialele au fost scoase din sursă în iunie **→** puse în
`secrets.h` **→** exclus din git.

**Mai departe:** fiind exclus, nu vine cu descărcarea proiectului — dar **nici nu se
șterge** la actualizare. Pe calculatorul de acasă există din iunie. Deci pasul corect
e „verifică dacă e acolo", nu „creează-l". Recreat orbește, îl suprascrii și poți
pierde parola bună.

### 5. De ce cele două `.ino` sunt o capcană

În iunie sketch-ul a fost dublat: unul pentru Arduino IDE, unul pentru PlatformIO
**→** sunt copii identice **→** niciun mecanism nu le ține sincronizate.

**Mai departe:** modifici unul, uiți celălalt, urci pe placă versiunea veche și comiți
versiunea nouă. Testul și codul salvat ajung să nu mai fie același lucru. *Al treilea
simptom care va minți.* Merită unificate cândva; până atunci, se modifică amândouă.

### 6. De ce podul cu sens unic schimbă cum lucrăm

Legitimația unei sesiuni cloud nu permite livrare către altă sesiune **→** telefon →
cloud merge, cloud → telefon nu **→** podul e cu sens unic.

**Mai departe:** telefonul inițiază, cloud-ul răspunde în transcript. Iar ce trebuie
să supraviețuiască trecerii dintre sesiuni **nu se transmite prin mesaje — se scrie
în GitHub.** Documentul ăsta e canalul, nu conversația.

### 7. De ce o sesiune nu e un loc de păstrare

Verificat pe 9 sept. 2026, pe starea reală a sesiunilor contului:

Sesiunile de pod rulează pe telefon **→** telefonul adoarme, Termux e oprit, rețeaua
pică **→** patru din cinci sesiuni erau oprite cu aceeași eroare, `computer_unreachable`
**→** fiecare a înghețat exact unde era, în mijlocul firului.

Consecințele s-au și văzut, nu sunt teoretice:
- o sesiune din 4 sept. terminase o cercetare și rezultatul a rămas necitit 5 zile;
- o sesiune din 5 sept. aștepta o instrucțiune și s-a deconectat cu ea în brațe;
- o sesiune din 24 mai a lucrat pe o ramură care **nu a ajuns niciodată pe GitHub** —
  verificat, ramura nu există. Munca ei nu mai poate fi recuperată.

**Mai departe:** o sesiune e un loc de **trecere**, nu de păstrare. Un rezultat care
trăiește doar într-un transcript e, practic, pierdut — nu pentru că-l șterge cineva,
ci pentru că nimeni nu-l mai scoate de acolo.

Regula care rezultă: **ce merită păstrat se scrie în GitHub în aceeași sesiune în care
s-a aflat.** Nu „la sfârșit", nu „când am timp" — atunci, pe loc. Fiindcă nu se știe
dacă va mai fi un sfârșit al acelei sesiuni.

### 8. Seiful de note — trei copii care se despart

Verificat pe 9 sept. 2026, la cererea sesiunii de pe telefon:

Seiful Obsidian (~153 note) există în **trei locuri, cu trei vârste diferite**:

| Unde | Vechime | Sincronizare |
|---|---|---|
| Calculatorul de acasă | originalul | — |
| Telefon (`/sdcard/Antigraviti`) | copie din **2 august** | **niciun sens** |
| Un depozit privat pe GitHub | ultima urcare **16 iunie** | oprită |

**Din cloud nu se vede deloc pe cale directă** — verificat: nu există `/sdcard`,
nici vreun dosar cu numele ăsta, nici vreo notă de-a lui Gabriel pe mașina cloud.
Copia de pe GitHub ar fi teoretic accesibilă, dar atașarea depozitului a fost
**blocată de clasificatorul de permisiuni** pe 9 sept. Deci azi: nu.

**Mai departe:** copia de pe GitHub e cu ~7 săptămâni mai veche decât cea de pe
telefon. Dacă cineva o citește crezând că e la zi, va lucra pe informație expirată —
*al cincilea simptom care va minți*, și cel mai greu de prins, fiindcă notele arată
la fel indiferent de vârstă.

Ce trebuie hotărât acasă, pe 11: **care copie e cea adevărată** și dacă se
restabilește o sincronizare. Până atunci, orice notă folosită se ia de pe telefon,
nu de pe GitHub.

### 9. De ce documentul ăsta există

Gabriel a pierdut firul contextului **→** nu pentru că lipsea informația, era toată în
proiect **→** ci pentru că nimic nu lega commit-ul din iunie de decizia de azi.

**Mai departe:** un document care doar enumeră stări repetă exact problema. De aceea
secțiunea asta. **Când adaugi ceva nou aici, adaugă și lanțul lui** — ce l-a cauzat și
ce obligă mai departe. O stare fără fir e o fotografie; firul e ce se poate folosi.

**Și un termen de expirare, de tratat ca atare:** secțiunea 0 descrie situația de
9 sept. 2026 și **devine falsă pe 11 sept.**, când Gabriel ajunge acasă. Nimic nu o
actualizează singură. Prima sarcină a primei sesiuni de pe 11 este să rescrie
secțiunea 0 — altfel documentul făcut ca să prevină contextul pierdut ajunge chiar el
să mintă. *Al patrulea simptom care va minți, și cel mai ușor de evitat.*

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

## 4b. PLAN PENTRU PRIMA ZI ACASĂ — 11 septembrie

Ordinea e gândită ca fiecare pas să fie scurt și să confirme ceva înainte de următorul.
Nu sări peste pași; dacă unul eșuează, oprește-te acolo și spune ce ai văzut.

### A. Pregătire (5 minute)

1. **Ia proiectul la zi:** `git pull origin main`
   Vei primi documentul ăsta plus tot ce s-a lucrat cât ai fost plecat.

2. **Verifică dacă `secrets.h` există deja:**
   `ls r2d2_pio/src/secrets.h`
   Pe calculatorul de acasă **probabil există deja** — a fost creat acolo în iunie și
   nu se șterge la `git pull` (e ignorat de git, nu urmărit). Dacă apare, treci mai
   departe. Dacă scrie „No such file", recreează-l după secțiunea 4, pasul 1.

3. **Bagă ESP32 în USB** și verifică portul: `ls /dev/ttyACM* /dev/ttyUSB*`

### B. Confirmă că tot ce mergea în iunie merge și acum (10 minute)

4. **Urcă codul existent, neschimbat:** `cd r2d2_pio && pio run --target upload`
   Scopul NU e o funcție nouă — e să confirmi că lanțul întreg (calculator → cablu →
   placă) e viu după 3 luni de pauză. Dacă asta merge, orice altceva e ușor.

5. **Monitor serial:** `pio device monitor`, apoi apasă EN/RST. Trebuie să vezi
   `[MOTOR] LEDC 1kHz inițializat (OK)` și un IP. **Notează IP-ul.**

### C. Răspunde la întrebarea rămasă din iunie (10 minute)

6. **Motor A merge?** În monitorul serial, cu robotul ridicat de pe podea:
   `v:120` apoi `fata` apoi `stop`.
   Uită-te la **ambele** motoare. În comentariile codului scrie că la ultimul test
   doar Motor B era activ — acum se lămurește. Spune-mi ce ai văzut:
   - ambele se învârt → perfect, mergem la Faza 2;
   - doar unul → verificăm cablajul pe GPIO 25/26 și driverul IBT-2 #1;
   - niciunul → problemă de alimentare, nu de cod.

### PRIORITATE, hotărâtă de Gabriel pe 9 sept. 2026

**Ziua de 11 începe cu OLX. Robotul vine după.**

Pașii A–D de mai sus rămân scriși și valabili oricând se ajunge la robot — nu se
pierd, doar așteaptă. Vezi secțiunea 4c pentru OLX.

### D. De aici încolo, împreună

7. Cu Motor A lămurit, se scrie **accelerația lină** — și se testează pe loc, pas cu
   pas. Ăsta e motivul pentru care nu s-a scris în avans: scrisă cu placa în față,
   iese bine din prima; scrisă orbește, ar fi trebuit rescrisă oricum.

---

## 4c. OLX — prioritatea zilei de 11

> **Vezi `SINTEZA_OLX.md`** — sinteza motorului OLX existent: trei straturi, ce e
> fapt și ce e ipoteză, ce lipsește ca să producă bani, și lista de verificat acasă
> pe 10–11 sept. Raport din a doua mână, transmis de sesiunea de pe telefon pe
> 9 sept.; se verifică la sursă acasă.
>
> **Atenție:** motorul OLX real trăiește în `~/Documents/Proect/olx_agent/` pe
> calculatorul de acasă — **nu în acest depozit.** Fișierele OLX de aici
> (`config_olx.json`, `test_gemini.py`, `olx_kimi_sync.py`) sunt altceva, mai mic.

**Stare: pregătire în curs. Ce înseamnă concret munca pe OLX nu e încă stabilit** —
Gabriel precizează. Sesiunea de pe telefon a raportat pe 9 sept. că pe 11 începe
„real și permanent, nu exercițiu", iar activitatea anterioară pe Kleinanzeigen
(Germania) a fost **exercițiu, încheiat** — subiectul nu se redeschide.

### Ce există deja în proiect (inventariat 9 sept. 2026)

| Fișier | Ce face | Stare |
|---|---|---|
| `config_olx.json` | instrucțiunea pentru generarea anunțurilor de **vânzare**: titlu optimizat, detalii pe liniuțe, îndemn la acțiune | funcțional |
| `test_gemini.py` | clientul care chiar generează anunțul, citind `config_olx.json` | funcțional |
| `olx_kimi_sync.py` | căutare de oferte pentru **cumpărare** + evaluare calitate/preț | **nefuncțional aici** |

Deci codul acoperă două direcții diferite: **vânzare** (anunțuri) și **cumpărare**
(căutare de oferte). Care dintre ele e munca de pe 11 — nestabilit.

### Trei lucruri de reparat înainte de folosirea reală

1. **`config_olx.json` are pe primul loc un model inexistent**, scris intenționat așa:
   `gemini-MODEL-STRICT-INEXISTENT`. A fost o probă a mecanismului de rezervă. La
   folosire reală, fiecare anunț va eșua întâi pe el și va arde o încercare degeaba.
   **Se scoate înainte de 11.**
2. **Celelalte două modele din listă nu au fost verificate** că mai există și mai
   răspund. Se verifică înainte, nu în timpul lucrului — altfel primul anunț real
   eșuează și pare că e stricat programul.
3. **`olx_kimi_sync.py` importă dintr-o cale care există doar pe calculatorul de
   acasă** (`/home/gabriel/Documents/VoiceCommander`). Nu poate fi nici măcar testat
   din cloud. Merge doar acasă.

**Mai departe:** primele două se pot rezolva din cloud, înainte de 11, dacă Gabriel
confirmă direcția. Al treilea se rezolvă doar acasă.

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
### Date personale — repo-ul este PUBLIC

**Verificat 9 sept. 2026: `fierascugabriel39-jpg/Proect` este public.** Oricine, de
oriunde, poate citi tot ce se scrie aici, inclusiv istoricul complet al modificărilor.

**Nu se scriu în proiect:** adrese de domiciliu, localități unde locuiește cineva,
numere de telefon, date de identitate, nume complete ale persoanelor apropiate.
Nimic din toate astea nu ajută la construirea robotului — deci nu au ce căuta aici.

Se pot scrie fără probleme: cod, scheme, pinout, decizii tehnice, calendar de lucru,
prenumele proprietarului proiectului.

**Cheile de acces și parolele** (WiFi, API) stau exclusiv în fișiere ignorate de git —
`secrets.h`, variabile de mediu. Niciodată direct în cod. Scanat 9 sept. 2026: istoricul
e curat, `secrets.h` nu a fost comis niciodată.

**Atenție:** ce ajunge o dată într-un repo public rămâne public. Ștergerea ulterioară
nu ajută — rămâne în istoric, în cópii și în arhive. Singura protecție reală e să nu
ajungă acolo de la început.

> **Pentru asistenții AI:** dacă utilizatorul îți spune în conversație unde locuiește
> sau alte date personale, folosește-le ca să înțelegi contextul, dar **nu le scrie în
> fișiere**. Notează doar ce e relevant tehnic.

### Automatizări active

- `.github/workflows/scanare-secrete.yml` — caută chei/parole scăpate în cod.
  Verificat funcțional pe 9 sept. 2026 (rulare reușită, „no leaks found"). Rulează în
  două moduri: **la fiecare modificare urcată** verifică doar commit-urile noi, iar
  **lunea dimineața** (și la pornire manuală) verifică întreg istoricul.
  Prinde formate de chei, **nu** date personale.
- De activat manual din browser (Settings → Code security → Secret scanning →
  Push protection): blochează trimiterea unei chei încă dinainte să ajungă pe GitHub.

### Regulă: nu se afirmă limite fără test

Impusă de Gabriel pe 9 sept. 2026, după trei afirmații greșite făcute într-o
singură zi de asistentul AI:
1. „din browser nu vezi nimic" — fals, podul de sesiuni funcționează;
2. „Gabriel nu poate rula nimic" — fals, are Claude Code CLI pe telefon, cu shell;
3. „probabil lipsește Remote Control activat" — fals, cauza era legitimația
   sesiunii cloud.

Toate trei au fost rostite cu aceeași siguranță ca lucrurile verificate. Gabriel
nu avea cum să le deosebească — și a trebuit să le corecteze el, deși structurile
astea sunt exact partea care ar trebui să fie treaba asistentului.

**Regula, pentru orice asistent AI care lucrează la acest proiect:**

- „Nu se poate", „nu merge", „nu ai cum" — **nu se spun fără un test rulat.**
  La fel și „merge sigur". Un mecanism nefolosit încă nu e un mecanism imposibil.
- Dacă testul nu s-a făcut, se spune **„nu am verificat"** — nu „nu se poate".
- Fiecare afirmație despre ce e posibil poartă eticheta ei: **confirmat prin test**,
  **infirmat prin test**, sau **ipoteză**. Fără amestec.
- Când testul cade, se caută **motivul dat de sistem**, nu unul inventat. Mesajul
  de eroare e dovadă; presupunerea nu e.
- Utilizatorul cunoaște uneltele pe care și le-a construit. Când spune că ceva
  merge, aia e informație — se verifică, nu se contrazice din reflex.

### Rutina de lucru

- Actualizează **secțiunea 0** ori de câte ori se schimbă situația reală (locație,
  acces la hardware, ce calculator ai). E secțiunea de care depinde tot restul.
- Actualizează secțiunile 1 și 6 după fiecare sesiune de lucru.

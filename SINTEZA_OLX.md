# MOTORUL OLX — sinteză

> **Proveniență, citește întâi:** informația de mai jos **nu a fost verificată de
> sesiunea cloud care scrie acest fișier.** A fost citită pe 9 sept. 2026 din seiful
> Obsidian de pe telefon, de sesiunea de pe telefon, și transmisă încoace. Este un
> **raport din a doua mână**, consemnat ca să nu se piardă odată cu sesiunea.
> Se verifică la sursă, acasă, pe 10–11 sept.
>
> **Avertismentul lui Gabriel, 12 aug. 2026:** *„tu ai impresia că cunoști totul
> despre motor, dar nu cunoști totul. Totul este pe PC."*
>
> Seiful e o copie-instantaneu din 2 august și **nu conține codul** — folderul
> `olx_agent/` nu a fost copiat niciodată. Deci ce urmează e **harta urmelor din
> note, nu inventarul proiectului.** Ce lipsește din seif nu înseamnă că nu există.

---

## 1. Ce este, de fapt

Nu sunt trei aplicații. E **una singură, în trei straturi de vârstă** — un motor
cerere ↔ ofertă, construit în iunie–iulie 2026.

Principiul lui Gabriel: **„Marfa e variabilă, motorul e constantă."**

## 2. Cele trei straturi

**V1 — schița.** Scraping → filtrare → notificare. `monitor.py`, `scorer.py`,
`database.py`, `phone_extractor.py`, `caller.py`.
⛔ **Twilio abandonat** — contactul merge prin telefon (ADB + Google Messages),
gratis. Confirmat de Gabriel că mesajul a ajuns.

**V2 — motorul universal, inima proiectului** (1–2 iulie). Repo `~/Documents/Proect`,
folder `olx_agent/`.

**Nu e un scraper — e un registru de oportunități.** Fiecare intrare poartă un steag:
`OFFER` (marfă) sau `DEMAND` (client care caută).

| Componentă | Rol |
|---|---|
| `monitor.py` / `monitor_http.py` | daemon systemd la 5 minute |
| surse | 7 URL-uri; 5 categorii live: telefon 292 · placă video 158 · laptop 66 · consolă 51 · robotică 1 |
| `market_prices` | preț de referință auto-calibrat pe **mediana live** per produs |
| `scorer.py` + `risk_flags` | scor de deal + avertisment de țeapă (display iPhone la 28% din piață → semnalat „blocat/furat") |
| `classifier.detect_side` | „Caut/Cumpăr" → DEMAND · „Vând" → OFFER |
| `opportunity.py` | registru idempotent (356 oferte rămân 356 la re-sincronizare) |
| `matcher.py` | potrivește cerere ↔ ofertă, comision 8% |
| `alerter.py` | Discord webhook la scor ≥ 50 |
| `report.py` + `emailer.py` | digest pe email |
| `publisher.py` | publicare cu **3 porți de siguranță**: fără token/preț/poze → `PENDING_APPROVAL`. Nu a trimis nimic niciodată. |
| `copywriter.py` | scrie anunțul din stoc (titlu 64 car., descriere 1524 — în limitele OLX) |
| `seller.py` | linia de comandă: status / sync / generate / publish |

**V3 — extinderea B2B** (5 iulie). Al doilea robinet: nu doar marfă, ci și **clienți**.
`b2b_lead_scraper.py` → `baza_date_clienti_b2b.csv`, 14 adrese business.
Sursa: DuckDuckGo — **Google e mort la scraping HTTP, măsurat, nu presupus.**
Volum la acea dată: 589 oferte reale în registru.

**Nu face parte din motor:** `OLX_Agent_Colab.md` („Agent de Marketing OLX", 2032
rânduri, făcut de Gemini). Înăuntru sunt GPU rental, Bunker-Shield, ReverseAuction,
HiveMind — alt proiect. Singura parte utilă e generatorul de text (`untitled1.py`),
echivalentul lui `copywriter.py`.

## 3. Linia care contează — fapt vs. ipoteză

**FAPT** — rulat, cu dovadă în note:
scanarea live pe 5 categorii · calibrarea pe mediană · scoring + semnalare de risc pe
cazuri reale · idempotența registrului · matcher pe oferte reale · daemon systemd la
5 minute · publisher care **refuză** să publice fără condiții · ruta de contact prin
telefon.

**IPOTEZĂ** — netestat:
publicarea reală pe OLX prin API (**fără token, deci niciodată încercată**) · scriptul
de contact automat (ruta validată manual, scriptul nu e scris) · **toate cifrele de
profit** (2250 lei arbitraj, 344 lei mediere = simulare pe gol).

**ZERO:** vânzări reale, lei încasați, clienți.

## 4. Ce lipsește ca să producă bani

**Niciunul nu e problemă de cod.**

| # | Ce | Cine |
|---|---|---|
| 1 | token API OLX (`developer.olx.ro`) — are timp de aprobare | Gabriel |
| 2 | App Password Gmail, pentru digest pe telefon — 3 minute | Gabriel |
| 3 | sursă de cereri reale pentru mediere — **decizie**, nu muncă | Gabriel |
| 4 | preț + 2–3 poze pentru marfa proprie în `stock.json` | Gabriel |
| 5 | maparea la schema OLX + timer de digest + `sms_send.py` fiabil | de lucru, ore |
| 6 | legarea celor două direcții (marfă găsită → clienți găsiți) | de lucru, ore |

**Blocajul nu e tehnic: un token, o parolă de aplicație și o decizie.**

## 5. Onest, despre valoare

Astăzi valorează **munca din el, nu profitul promis** — nu a produs niciun leu.
**Prima vânzare reală valorează mai mult decât încă 100 de ore de cod.**

*(Estimările în bani din actul original nu sunt reproduse aici: acest depozit este
public. Sunt în seif.)*

## 6. DE VERIFICAT ACASĂ, 10–11 SEPTEMBRIE

Din `DE_FACUT_LA_INTOARCERE_20260910.md`. **În ordinea asta.**

**a) CODUL MAI E PE DISC?** — primul lucru, înaintea oricărei alte munci.
Semnalat pe 12 aug.: `olx_agent/` era **untracked** în git, iar un raport Gemini
constata că `monitor.py`, `phone_extractor.py` și `caller.py` **nu erau pe disc**.
Dacă lipsesc, se caută în copiile de siguranță **înainte de orice altceva**.

**b) CÂT A LUCRAT MOTORUL** — din jurnalul systemd, **nu din memorie**.
Se știe sigur: serviciul `olx-monitor` la 5 minute, pornit la boot, activ pe 31 iulie,
oprit odată cu PC-ul în jur de 2 august.
⚠️ Cifrele „288 scanări/zi · 52 anunțuri/scanare" sunt **de proiectare, nu măsurate.**
Nu se repetă ca fapt.

**c) SEIFUL SE DUCE ACASĂ PE CABLU** — primul lucru la sosire.
Nu se sincronizează în niciun sens. Tot ce s-a lucrat trei luni e **numai pe telefon**.

---

*Consemnat pe 9 sept. 2026. Sursă: 39 de note din seif menționează OLX; actul
principal este `05_PROECTE/OLX_Motor_Cautare_Comparatie_Si_Cost_20260812.md` (20 KB).*

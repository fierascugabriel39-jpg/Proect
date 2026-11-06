# Pentru Claude Code de pe PC — ancheta căderii din oct.–nov. 2026

Scris de sesiunea din cloud, 6 nov. 2026. Gabriel a lipit acest text. Ce urmează e
datoria ta: cercetezi pe PC, cu uneltele tale. Nu-l întrebi pe Gabriel ce nu merge.

Întrebarea lui Gabriel: **a șters agentul local Hermes fișierele de care depindea Claude
Code?** Hermes i-a scris într-un mesaj, aproximativ: „eu sunt aici pe PC-ul tău, sunt
agentul tău local, Claude Code e în nori”. Gabriel se teme că i-a dat lui Hermes drept
să ruleze comenzi și că Hermes a șters ceva.

---

## A. Ce s-a stabilit deja (nu reface, folosește)

| # | Fapt | Etichetă | Sursa |
|---|---|---|---|
| 1 | `which -a claude` nu întorcea nimic; `timeout 60 claude ...` → `No such file or directory` | am măsurat | rulat de Gabriel în terminal, 6 nov. |
| 2 | `claude` era doar funcția bash din `~/.bashrc`, rândurile 162–188 (pornește în tmux, sesiunea `claude`). Fără binar, tmux afișa doar `[exited]` | am măsurat | `grep -n claude ~/.bashrc` |
| 3 | `node` și `npm` → `Command not found` | am măsurat | terminal, 6 nov. |
| 4 | `~/.local/bin/claude` → `No such file or directory` | am măsurat | `ls -la` |
| 5 | `~/.local/share/claude/versions/` conținea doar `2.1.145` (20 mai 21:37) și `2.1.146` (21 mai 05:12) | am măsurat | `ls -la` |
| 6 | `~/.local/bin` apare de 4 ori în PATH (duplicate) | am măsurat | `echo $PATH` |
| 7 | La `claude update`: `Warning: Running native installation but config install method is 'global'`, apoi `Config updated ... native`, `Successfully updated from 2.1.146 to version 2.1.319` | am măsurat | terminal, 6 nov. |
| 8 | Rețeaua merge: api.anthropic.com → 404, claude.ai → 403 (deci serverele răspund); disc: 30G liberi (87% ocupat) | am măsurat | `curl`, `df -h ~` |
| 9 | Ultima conversație reală prin Remote Control de pe PC: 14 sept. 2026. Cereri de reconectare (`initialize`) spre sesiunea arhivată până pe **4 oct. 2026**; după aceea nimic | am măsurat | evidența sesiunilor, citită din cloud |
| 10 | Reparația făcută: `ln -sf ~/.local/share/claude/versions/2.1.146 ~/.local/bin/claude`, apoi `claude update` → 2.1.319 | am măsurat | terminal, 6 nov. |
| 11 | Comentariul din `.bashrc` (rândul 174) pomenește „PATH-ul corect (nvm/node/claude)” | am măsurat | `grep` |

**Deducție (nu e fapt):** înainte, Claude Code rula dintr-o instalare **npm globală**, sub Node
adus prin **nvm** (rândurile 7 și 11). Node/nvm a dispărut sau nu se mai încarcă, așa că a
dispărut și `claude`. Copia nativă din mai nu avea scurtătura `~/.local/bin/claude`.
Fereastra probabilă a căderii: **după 4 oct. 2026**.

**Nu știm încă:** cine sau ce a scos Node/nvm, când anume, și dacă Hermes are vreo legătură.

**Ipoteze concurente. Toate sunt la fel de neverificate; nu favoriza H1 doar pentru că e întrebarea:**
- H1. Hermes a rulat o comandă care a șters/dezinstalat Node, nvm sau `claude`.
- H2. O sesiune Claude Code a făcut-o. În `~/.claude/settings.json` exista regula `Bash` la `permissions.allow`, adică orice comandă fără întrebare. Gabriel a fost întrebat pe 6 nov. dacă o scoate.
- H3. Curățenie de sistem: `apt autoremove`/`purge`, actualizare de distribuție, `nvm uninstall`, ștergere manuală.
- H4. O actualizare automată a lui Claude Code s-a oprit la jumătate.
- H5. Node/nvm e încă pe disc, dar nu se mai încarcă (de exemplu, s-a schimbat `.bashrc`/`.profile`).

---

## B. Delimitare

### Faci. Un pas pe rând, cu raport după fiecare. Totul doar citește.

1. **Mai există nvm/Node pe disc? (testează H5)**
   ```bash
   ls -la --time-style=full-iso ~/.nvm ~/.nvm/versions/node 2>&1
   grep -nE "nvm|NVM_DIR|node" ~/.bashrc ~/.profile ~/.bash_profile 2>/dev/null
   ls -la --time-style=full-iso ~/.bashrc ~/.profile
   ```
2. **Datele exacte ale schimbărilor**
   ```bash
   ls -la --time-style=full-iso ~/.local/bin ~/.local/share/claude ~/.local/share/claude/versions
   stat ~/.claude/settings.json ~/.claude.json 2>&1 | grep -E "File|Modify|Change"
   ```
3. **apt/snap au scos Node? (testează H3)**
   ```bash
   zgrep -hE "Start-Date|Commandline|nodejs|npm" /var/log/apt/history.log* 2>/dev/null | grep -B1 -A1 -iE "node|npm|autoremove|purge"
   grep -hiE "nodejs|npm" /var/log/dpkg.log* 2>/dev/null | grep -iE "remove|purge"
   snap list 2>/dev/null | grep -i node
   ```
4. **Istoricul de comenzi al shell-ului**
   ```bash
   grep -nE "rm .*(nvm|node|\.local/bin|claude)|nvm (uninstall|deactivate)|npm (uninstall|rm) |apt(-get)? (remove|purge|autoremove)" ~/.bash_history
   ```
5. **Hermes: unde stă, ce drepturi are, ce a rulat (testează H1)**
   - Găsește-l: iconița „Hermes local” de pe desktop (`grep -l -i hermes ~/Desktop/*.desktop ~/.local/share/applications/*.desktop`, apoi `Exec=` din fișier) și `HERMES_FISA_BUTOANE.txt` de pe desktop.
   - Află ce unealtă de comenzi are și dacă rulează comenzi **fără confirmare** (configurația, lista de comenzi permise).
   - Caută în jurnalele și istoricul lui, pe intervalul **4 oct.–6 nov. 2026**, după: `rm`, `nvm`, `npm`, `node`, `apt`, `.local/bin`, `claude`. Citează rândurile brute, cu data.
   - Rulează ca serviciu sau la pornire? `systemctl --user list-units | grep -i hermes`, `crontab -l`, `ls ~/.config/autostart`.
6. **Jurnalele Claude Code (testează H2 și H4)**: caută în `~/.claude/projects/*/*.jsonl` comenzi Bash pe același interval, cu aceleași cuvinte ca la pasul 5.

### NU faci
- Nu ștergi, nu oprești și nu modifici Hermes, configurația lui sau `~/.claude/settings.json`.
- Nu instalezi nimic (nici Node/nvm) fără un „da” în chat.
- Nu declari vinovat pe nimeni fără un rând de jurnal cu dată și comandă. „Hermes a scris că e pe PC” nu e dovadă.
- Pentru o lipsă („nu există jurnal”, „nu a rulat”) ceri a doua sursă independentă, înainte să tragi concluzia.
- Nu scrii în seif și nu faci commit fără „da”. Fără `git add -A`; la cerere: `git status` complet, apoi doar fișierele aprobate.
- Nu extinzi ancheta în alte subiecte.

---

## C. Formatul raportului (în română, scurt)

Pentru fiecare pas:
- **am măsurat**: comanda și rezultatul brut (rândurile relevante, cu date);
- **rămâne ipoteză**: ce nu s-a putut confirma;
- la final, pentru H1–H5, câte un verdict: **confirmat** / **infirmat** / **nedeterminat**, cu rândul de dovadă.

Dacă găsești că Hermes poate rula comenzi fără confirmare, **spune asta separat și
clar**, chiar dacă nu el a șters fișierele. E un risc de sine stătător. Propune o variantă
de limitare, dar **nu o aplici** fără „da”.

Nu încheia cu „de unde continuăm?”.

---

## D. Context (judecată, nu măsurătoare)

- Mesajul lui Hermes („eu sunt aici pe PC, Claude Code e în nori”) descrie doar unde
  rulează fiecare. Nu arată că a șters ceva.
- Regula `Bash` din setările Claude Code dădea aceeași putere oricărei sesiuni Claude Code.
  H2 are deci aceeași greutate ca H1 până la dovezi.

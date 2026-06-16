#!/bin/bash

# SCRIPT MASTER DE PORNIRE ANTIGRAVITY v5.0

echo "--- Pornire Sistem ANTIGRAVITY v5.0 ---"

# 1. Încărcare Variabile de Mediu
source ~/.bashrc
# Dacă există un fișier de secrete separat, îl încărcăm
if [ -f ~/.secrets/kimi_key.env ]; then
    source ~/.secrets/kimi_key.env
fi

# 2. Curățare Message Bus
echo "[1/4] Curățare Message Bus vechi..."
mkdir -p /tmp/antigravity/bus
rm -f /tmp/antigravity/bus/*.json

# 3. Pornire TTS Worker în Background
echo "[2/4] Pornire TTS Synchronized Worker..."
cd ~/Documents/VoiceCommander
source venv/bin/activate
python3 tts_sync.py > /tmp/antigravity_tts.log 2>&1 &
TTS_PID=$!
echo "TTS Worker pornit cu PID: $TTS_PID"

# 4. Verificare Orchestrator și Kimi
echo "[3/4] Verificare Orchestrator..."
python3 -c "from orchestrator import get_orchestrator; o = get_orchestrator(); print('Orchestrator OK')"

# 5. Afișare Status LIVE
echo "[4/4] Sistemul este ONLINE."
echo "--------------------------------------"
echo "Comenzi active: Kimi, Gemini, Robot, Ollama"
echo "Log-uri TTS disponibile în /tmp/antigravity_tts.log"
echo "--------------------------------------"

# Menținem scriptul activ pentru a monitoriza dacă vrei
# Sau poți închide aici și sistemul rulează în background
echo "Apasă [CTRL+C] pentru a opri monitorizarea (TTS va rămâne în background)."
wait $TTS_PID

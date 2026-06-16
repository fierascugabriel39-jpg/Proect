import sys
import os

# Adăugăm calea către VoiceCommander pentru a importa orchestratorul și bus-ul
sys.path.append("/home/gabriel/Documents/VoiceCommander")

import bus
from orchestrator import get_orchestrator

def scan_olx_and_analyze(search_term):
    print(f"[Sync] Încep scanarea OLX pentru: {search_term}")
    orch = get_orchestrator()
    
    # 1. Gemini caută oferte (simulăm căutarea prin delegare)
    print("[Sync] Pas 1: Gemini caută oferte...")
    search_prompt = f"gemini caută pe OLX și web cele mai bune oferte pentru {search_term}. Extrage specificațiile tehnice."
    offers_raw = orch.handle_voice(search_prompt)
    
    # 2. Kimi analizează specificațiile
    print("[Sync] Pas 2: Kimi analizează raportul calitate/preț...")
    analysis_prompt = (
        f"Kimi, analizează aceste oferte pentru {search_term}: \n\n {offers_raw} \n\n "
        "Verdictul trebuie să includă: compatibilitate ESP32 (dacă e cazul), "
        "raport calitate/preț și recomandarea finală de cumpărare."
    )
    verdict = orch.delegate_kimi(analysis_prompt)
    
    # 3. Publicăm rezultatul în bus pentru TTS
    final_response = f"Analiză OLX finalizată pentru {search_term}. Verdictul Kimi: {verdict}"
    bus.post("tts_queue", "olx_sync", {"text": final_response, "priority": "high"})
    
    return verdict

if __name__ == "__main__":
    if len(sys.argv) > 1:
        term = " ".join(sys.argv[1:])
    else:
        term = "motor stepper NEMA 17"
    
    print(scan_olx_and_analyze(term))

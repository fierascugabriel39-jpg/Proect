import urllib.request
import urllib.error
import json
import sys
import argparse
import time
import os

CONFIG_PATH = "config_olx.json"

def get_api_key(key_path="/home/gabriel/.secrets/gemini_key.env"):
    api_key = None
    try:
        if not os.path.exists(key_path):
            return os.environ.get("GEMINI_API_KEY")
        with open(key_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'): continue
                if 'export ' in line: line = line.replace('export ', '')
                if '=' in line:
                    key, val = line.split('=', 1)
                    api_key = val.strip().strip('"').strip("'")
                    break
            if not api_key:
                 f.seek(0)
                 api_key = f.read().strip()
        return api_key
    except Exception as e:
        print(f"Eroare la obținerea cheii: {e}", file=sys.stderr)
        return None

def load_config(config_path=CONFIG_PATH):
    try:
        with open(config_path, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Avertisment: Nu am putut încărca {config_path}. Folosim setări implicite. ({e})")
        return None

def ask_gemini_with_resilience(prompt, api_key, audio_path=None):
    config = load_config()
    system_instruction = config.get("system_instruction") if config else None
    models = config.get("models_priority", ["gemini-1.5-flash"]) if config else ["gemini-1.5-flash"]
    retry_cfg = config.get("retry_config", {"max_retries": 3, "initial_delay": 1, "backoff_factor": 2}) if config else {}
    
    max_retries = retry_cfg.get("max_retries", 3)
    initial_delay = retry_cfg.get("initial_delay", 1)
    backoff_factor = retry_cfg.get("backoff_factor", 2)

    for model_name in models:
        print(f"> Încercăm modelul: {model_name}...")
        
        url = f"https://generativelanguage.googleapis.com/v1beta/models/{model_name}:generateContent?key={api_key}"
        
        parts = [{"text": prompt}]
        if audio_path:
            import base64
            import mimetypes
            try:
                with open(audio_path, "rb") as f:
                    audio_data = f.read()
                b64_str = base64.b64encode(audio_data).decode('utf-8')
                mime_type, _ = mimetypes.guess_type(audio_path)
                if not mime_type: mime_type = "audio/webm"
                parts.append({"inlineData": {"mimeType": mime_type, "data": b64_str}})
            except Exception as e:
                return f"Eroare la procesarea fișierului audio: {e}"

        payload = {
            "contents": [{"parts": parts}],
            "generationConfig": {"temperature": 0.4, "maxOutputTokens": 1500}
        }
        if system_instruction:
            payload["systemInstruction"] = {"parts": [{"text": system_instruction}]}

        data = json.dumps(payload).encode('utf-8')
        headers = {'Content-Type': 'application/json'}

        # Bucla de Retry pentru modelul curent
        for attempt in range(max_retries):
            try:
                req = urllib.request.Request(url, data=data, headers=headers, method='POST')
                with urllib.request.urlopen(req) as response:
                    result = json.loads(response.read().decode('utf-8'))
                    return result['candidates'][0]['content']['parts'][0]['text'].strip()
            
            except urllib.error.HTTPError as e:
                err_body = e.read().decode('utf-8')
                if e.code in [503, 429]: # Service Unavailable sau Rate Limit
                    wait_time = initial_delay * (backoff_factor ** attempt)
                    print(f"  [!] Server ocupat (Cod {e.code}). Reîncercăm în {wait_time}s... (Încercarea {attempt + 1}/{max_retries})")
                    time.sleep(wait_time)
                else:
                    print(f"  [!] Eroare HTTP {e.code}: {err_body}")
                    break # Trecem la următorul model dacă eroarea nu e de tip "busy"
            except Exception as e:
                print(f"  [!] Eroare neașteptată: {str(e)}")
                break

        print(f"  [X] Modelul {model_name} a eșuat după {max_retries} încercări.")
    
    return "Eroare: Toate modelele din listă au eșuat sau sunt indisponibile."

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Expert Vânzări OLX (cu Smart-Bridge Resilience).")
    parser.add_argument("prompt", type=str, nargs='?', default="Vand telefon vechi, merge bine", help="Descrierea produsului.")
    parser.add_argument("--audio", type=str, default=None, help="Calea către fișierul audio.")
    
    args = parser.parse_args()

    key = get_api_key()
    if not key:
        print("Eroare: Nu am putut găsi cheia API.", file=sys.stderr)
        sys.exit(1)

    print(f"> Analizăm baza de la tine: '{args.prompt}'")
    print("-" * 50 + "\n")
    
    raspuns = ask_gemini_with_resilience(
        prompt=args.prompt,
        api_key=key,
        audio_path=args.audio
    )
    
    print("\n" + "=" * 50)
    print(" REZULTAT GENERAT:")
    print("=" * 50 + "\n")
    print(raspuns)

import os
import sys
from openai import OpenAI

# Încercăm să luăm cheia din variabila de mediu
api_key = os.getenv("KIMI_API_KEY")

if not api_key or api_key == "sk-your-key":
    print("\n❌ EROARE: Cheia KIMI_API_KEY nu este setată sau este invalidă.")
    print("Te rog să rulezi: export KIMI_API_KEY='cheia_ta_reala'")
    sys.exit(1)

# Inițializăm clientul Kimi (Moonshot AI)
client = OpenAI(
    api_key=api_key,
    base_url="https://api.moonshot.ai/v1",
)

def test_kimi():
    print("📡 Comunic cu Kimi (Moonshot AI)...")
    try:
        completion = client.chat.completions.create(
            model="kimi-k2.6",
            messages=[
                {"role": "system", "content": "Ești Kimi, agentul de context masiv al sistemului Antigravity."},
                {"role": "user", "content": "Salut Kimi! Confirmă că ești activ și pregătit pentru analize tehnice."}
            ],
            temperature=0.3,
        )
        print("\n✅ RĂSPUNS KIMI:")
        print("-" * 30)
        print(completion.choices[0].message.content)
        print("-" * 30)
    except Exception as e:
        print(f"\n❌ EROARE API: {e}")

if __name__ == "__main__":
    test_kimi()

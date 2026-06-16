import urllib.request
import json

# Definim comanda pentru a crea o piramidă (care în Blender este un con cu 4 colțuri/vertices)
url = "http://127.0.0.1:8000/mcp/invoke/create_mesh_object"
payload = {
    "arguments": {
        "name": "Piramida_Test",
        "primitive_type": "cone",
        "vertices": 4,
        "size": 2.0,
        "location": [0, 0, 1]
    }
}

data = json.dumps(payload).encode('utf-8')
req = urllib.request.Request(url, data=data, headers={'Content-Type': 'application/json'})

print("Trimitem comanda către Blender MCP pe portul 8000...")
try:
    with urllib.request.urlopen(req) as response:
        result = json.loads(response.read().decode('utf-8'))
        print("\n✅ Succes! Răspuns de la Blender:")
        print(json.dumps(result, indent=2, ensure_ascii=False))
except Exception as e:
    print(f"\n❌ Eroare: {e}")

import socket
import sys

# === CONFIGURARE ===
UDP_IP = "192.168.1.100"  # ← Înlocuiește cu IP-ul real al ESP32
UDP_PORT = 4210

def trimite_comanda(cmd):
    print(f"[UDP] Trimit: {cmd} către {UDP_IP}:{UDP_PORT}")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(cmd.encode(), (UDP_IP, UDP_PORT))

if __name__ == "__main__":
    if len(sys.argv) > 1:
        comanda = " ".join(sys.argv[1:])
    else:
        comanda = "status"
    
    trimite_comanda(comanda)

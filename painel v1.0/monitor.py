import psutil
import socket
import time
import GPUtil
import requests
import sys

# --- CONFIGURAÇÕES DO MONITOR (EDITAR IP) ---
ESP32_IP = "192.168.XX.XX" # Substitua pelo IP que aparecer na tela do ESP32
PORTA = 4210
LHM_URL = "http://localhost:8085/data.json"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def buscar_valor_pelo_nome(data, nomes_alvo):
    if isinstance(data, dict):
        texto = data.get('Text', '')
        valor = data.get('Value', '')
        if any(nome in texto for nome in nomes_alvo) and "°C" in valor:
            try:
                return int(float(valor.replace(" °C", "").replace(",", ".")))
            except:
                return None
        for child in data.get('Children', []):
            res = buscar_valor_pelo_nome(child, nomes_alvo)
            if res is not None: return res
    elif isinstance(data, list):
        for item in data:
            res = buscar_valor_pelo_nome(item, nomes_alvo)
            if res is not None: return res
    return None

def obter_dados():
    cpu_uso = int(psutil.cpu_percent(interval=None))
    cpu_temp = 0
    try:
        response = requests.get(LHM_URL, timeout=0.5)
        if response.status_code == 200:
            json_data = response.json()
            nomes_cpu = ["Core (Tctl/Tdie)", "CCD1 (Tdie)", "CPU Package"]
            res = buscar_valor_pelo_nome(json_data, nomes_cpu)
            if res is not None: cpu_temp = res
    except: cpu_temp = 0

    gpu_uso = 0
    gpu_temp = 0
    try:
        gpus = GPUtil.getGPUs()
        if gpus:
            gpu_uso = int(gpus[0].load * 100)
            gpu_temp = int(gpus[0].temperature)
    except: pass

    return f"{cpu_temp},{gpu_temp},{cpu_uso},{gpu_uso}"

print("========================================")
print("     PAINEL MONITOR - VERSÃO 1.0        ")
print("========================================")

while True:
    try:
        pacote = obter_dados()
        sock.sendto(pacote.encode(), (ESP32_IP, PORTA))
        sys.stdout.write(f"\r[DADOS ENVIADOS] {pacote}      ")
        sys.stdout.flush()
    except Exception as e:
        print(f"\nErro: {e}")
    time.sleep(1)
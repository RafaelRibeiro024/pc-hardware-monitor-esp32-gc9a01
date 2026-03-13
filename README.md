# pc-hardware-monitor-esp32-gc9a01
Monitor de desempenho do sistema com ESP32 e GC9A01 | Real-time PC hardware monitor via UDP using Python 3.14


# PC Monitor Display - ESP32 & Python 3.14

Este projeto é um painel de monitoramento de hardware em tempo real de alto desempenho. Utiliza um script **Python 3.14** para coletar métricas do Windows e enviá-las via protocolo **UDP** (rede local) para um **ESP32**, que renderiza as informações em um display circular **GC9A01**.

## Funcionalidades
- **Monitoramento Térmico:** Temperatura da CPU e GPU em tempo real.
- **Carga do Sistema:** Porcentagem de uso de processamento e vídeo.
- **Visual Gamer:** Borda animada com giro de arco-íris (RGB) em degradê.
- **Interface Intuitiva:** Barras de progresso dinâmicas que acompanham o uso dos componentes.

##  Hardware e Conexão
- **Microcontrolador:** ESP32 (Wroom ou similar).
- **Display:** LCD Circular 1.28" (Driver GC9A01).
- **Montagem Física:** Foram utilizados **9 cabos** para a conexão completa.
- **Nota de Pinagem:** O pino **BLK** (Backlight) do display **não foi utilizado** nesta montagem, simplificando a fiação e mantendo o brilho constante.

##  Software & Dependências

### Python
- **Versão:** 3.14
- **Bibliotecas:** `psutil`, `GPUtil`, `requests`.
- **Instalação:** `pip install psutil gputil requests`

### Arduino IDE (C++)
- **Bibliotecas:** `Adafruit_GFX`, `Adafruit_GC9A01A`, `WiFi`, `WiFiUdp`.

###  LibreHardwareMonitor (LHM)
O projeto depende do LHM para extrair dados térmicos precisos do processador.
1. **Instalação:** Baixe a versão mais recente do [LibreHardwareMonitor](https://librehardwaremonitor.github.io/).
2. **Privilégios:** Deve ser executado sempre como **Administrador** (obrigatório para acessar os sensores da placa-mãe).
3. **Servidor de Rede:** É necessário ativar o servidor web para que os dados fiquem acessíveis ao script Python. Vá em:
   - `Options` -> `Remote Web Server` -> `Run`.

##  Configuração e Uso

1. **No Firmware (C++):**
   - No arquivo `.ino`, altere as variáveis `ssid` e `password` para os dados da sua rede Wi-Fi.
   - Faça o upload para o ESP32 e anote o IP que aparecerá na tela após a conexão.

2. **No Script (Python):**
   - No arquivo `monitor.py`, ajuste a variável `ESP32_IP` com o endereço obtido no passo anterior.
   - Certifique-se de que o LibreHardwareMonitor está aberto (como Admin) e com o servidor rodando.

3. **Execução:**
   - Execute o script Python: `python monitor.py`.
   - O painel começará a receber e exibir os dados instantaneamente.

---
*Projeto desenvolvido como parte de estudos em sistemas embarcados e integração Python/C++.*

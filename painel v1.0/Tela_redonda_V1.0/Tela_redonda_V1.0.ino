#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// --- CONFIGURAÇÕES DE REDE (EDITAR ANTES DE SUBIR) ---
const char* ssid = "SEU-WIFI"; 
const char* password = "SUA-SENHA";
WiFiUDP udp;
unsigned int localUdpPort = 4210; 
char incomingPacket[255];

#define TFT_CS     5
#define TFT_DC     2
#define TFT_RST    4
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

// --- CORES ESTÁTICAS DA V1.0 ---
#define COR_ROXO      0x781F 
#define COR_MAGENTA   0xF81F 
#define COR_ROSA      0xF812 
#define COR_LARANJA   0xFC08 
#define COR_AMARELO   0xFD00 
#define COR_PRETO     0x0000
#define COR_CINZA     0x7BEF 
#define COR_BRANCO    0xFFFF

// Definição de cores fixas para os textos
#define COR_TXT_CPU   COR_AMARELO 
#define COR_TXT_GPU   COR_MAGENTA 

int anguloInicial = 0;
int velocidadeGiro = 8; 

// Funções de mistura e degradê (Mantidas)
uint16_t misturarCores(uint16_t c1, uint16_t c2, float p) {
  uint8_t r = ((c1 >> 11) & 0x1F) + (((c2 >> 11) & 0x1F) - ((c1 >> 11) & 0x1F)) * p;
  uint8_t g = ((c1 >> 5) & 0x3F) + (((c2 >> 5) & 0x3F) - ((c1 >> 5) & 0x3F)) * p;
  uint8_t b = (c1 & 0x1F) + ((c2 & 0x1F) - (c1 & 0x1F)) * p;
  return (r << 11) | (g << 5) | b;
}

uint16_t obterCorDegradeCompleto(int angulo) {
  angulo = angulo % 360;
  if (angulo < 90) return misturarCores(COR_AMARELO, COR_LARANJA, angulo / 90.0);
  else if (angulo < 180) return misturarCores(COR_LARANJA, COR_ROSA, (angulo - 90) / 90.0);
  else if (angulo < 270) return misturarCores(COR_ROSA, COR_MAGENTA, (angulo - 180) / 90.0);
  else return misturarCores(COR_MAGENTA, COR_AMARELO, (angulo - 270) / 90.0);
}

void desenharBordaGiroPerfeito() {
  int centroX = 120, centroY = 120, espessura = 10;
  float raioBase = 119;
  for (float i = 0; i < 360; i += 0.8) { 
    int anguloCor = (int(i) + anguloInicial) % 360;
    uint16_t cor = obterCorDegradeCompleto(anguloCor);
    float rad = i * 0.0174533; 
    float cosRad = cos(rad), sinRad = sin(rad);
    for (int e = 0; e < espessura; e++) {
      tft.drawPixel(centroX + cosRad * (raioBase - e), centroY + sinRad * (raioBase - e), cor);
    }
  }
}

void desenharBarraProgresso(int x, int y, int largura, int altura, String valor, uint16_t cor) {
  int pct = valor.toInt();
  if (pct > 100) pct = 100;
  int larguraPreenchida = (largura * pct) / 100;
  tft.drawRect(x, y, largura, altura, COR_CINZA);
  tft.fillRect(x + 1, y + 1, larguraPreenchida, altura - 2, cor);
  tft.fillRect(x + 1 + larguraPreenchida, y + 1, largura - larguraPreenchida - 2, altura - 2, COR_PRETO);
}

void desenharBloco(int x, int y, String label, String valor, uint16_t cor, bool isTemp) {
  tft.fillRect(x, y + 12, 60, 25, COR_PRETO);
  tft.setTextSize(1);
  tft.setTextColor(COR_CINZA, COR_PRETO);
  tft.setCursor(x, y); tft.print(label);
  tft.setTextSize(2);
  tft.setTextColor(cor, COR_PRETO);
  tft.setCursor(x, y + 12); tft.print(valor);
  
  int offsetSymbol = (valor.length() * 12) + 4; 
  tft.setTextSize(1);
  if(isTemp) {
    tft.setCursor(x + offsetSymbol, y + 10); tft.print("o"); 
    tft.setCursor(x + offsetSymbol + 6, y + 12); tft.print("C");
  } else {
    tft.setCursor(x + offsetSymbol, y + 16); tft.print("%");
    desenharBarraProgresso(x, y + 32, 50, 5, valor, cor);
  }
}

void setup() {
  tft.begin();
  tft.setRotation(2); 
  tft.fillScreen(COR_PRETO);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  udp.begin(localUdpPort);
  tft.fillScreen(COR_PRETO);
}

void loop() {
  desenharBordaGiroPerfeito();
  anguloInicial = (anguloInicial + velocidadeGiro) % 360; 

  tft.setTextSize(2); 
  tft.setTextColor(COR_BRANCO);
  tft.setCursor(65, 45); tft.print("MONITOR-PC"); 
  tft.drawFastHLine(60, 68, 120, COR_ROXO);

  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
      String data = String(incomingPacket);
      int v1 = data.indexOf(',');
      int v2 = data.indexOf(',', v1 + 1);
      int v3 = data.indexOf(',', v2 + 1);

      if (v3 != -1) {
        // Ordem: cpu_temp, gpu_temp, cpu_uso, gpu_uso
        desenharBloco(40, 85,  "CPU TEMP", data.substring(0, v1), COR_TXT_CPU, true);
        desenharBloco(130, 85, "CPU USO",  data.substring(v2+1, v3), COR_TXT_CPU, false);
        desenharBloco(40, 145, "GPU TEMP", data.substring(v1+1, v2), COR_TXT_GPU, true);
        desenharBloco(130, 145, "GPU USO",  data.substring(v3+1), COR_TXT_GPU, false);
      }
    }
  }
}
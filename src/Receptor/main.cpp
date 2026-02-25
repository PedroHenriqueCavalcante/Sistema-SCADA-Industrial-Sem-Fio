#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include "ComunicacaoWireless.h"

// Impede conflitos de macro com o Arduino Cloud no ESP32
#ifdef CD
#undef CD
#endif

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "Telas.h"
#include "Armazenamento.h"

// --- CONFIGURAÇÕES DO WI-FI ---
const char SSID[] = "Pedro";   
const char PASS[] = "senha123";   

// --- CONFIGURAÇÕES DO ARDUINO CLOUD ---
const char DEVICE_LOGIN_NAME[]  = "9b729cba-2ed7-4949-b8cb-a75527f918f1";
const char DEVICE_KEY[]         = "2JvzBjP4@mEfTzTgHuOQpe?Rp";

// --- MAPEAMENTO DE PINOS (WEMOS D1 R32) ---
#define CE_PIN 4  
#define CSN_PIN 5 
#define SD_CS_PIN 26 // Pino exclusivo para o Chip Select do SD

// Instancia a máquina de RECEPÇÃO usando as configurações cravadas no .h
RadioReceptor receptor(CE_PIN, CSN_PIN);

// Instâncias dos periféricos periféricos
GerenciadorDeTelas telas;
//GerenciadorDeArmazenamento sdCard(SD_CS_PIN);

// Variáveis espelhadas no Arduino Cloud
float temp_interna;
float temp_externa;
float umidade;
float cooler_rpm;
float luz_ambiente;
float luz_interna;

// Handler do Cloud (Assume o Wi-Fi)
WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(temp_interna, Permission::Read);
  ArduinoCloud.addProperty(temp_externa, Permission::Read);
  ArduinoCloud.addProperty(umidade, Permission::Read);
  ArduinoCloud.addProperty(cooler_rpm, Permission::Read);
  ArduinoCloud.addProperty(luz_ambiente, Permission::Read);
  ArduinoCloud.addProperty(luz_interna, Permission::Read);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\n=== INICIANDO RECEPTOR SCADA LITEME ===");

  // 1. BLINDAGEM DO BARRAMENTO SPI
  SPI.begin(18, 19, 23); 

  // 2. Inicializa Periféricos I2C e SPI
  telas.iniciar();
  //sdCard.iniciar();

  // 3. Inicializa Arduino Cloud
  Serial.println("Conectando ao Arduino Cloud e Wi-Fi...");
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // 4. Inicializa o Rádio através da Classe
  Serial.print("Iniciando NRF24L01... ");
  if (!receptor.begin()) {
    Serial.println("ERRO CRÍTICO: Radio nao encontrado! Verifique jumpers e SPI.");
  } else {
    Serial.println("Radio OK! Ouvindo o ar...");
  }
}

void loop() {
  // Mantém a máquina de estados da Nuvem viva
  ArduinoCloud.update(); 
  
  // Pergunta à nossa classe se algum pacote bateu na antena
  if (receptor.available()) { 
    PacoteDados pacote;
    
    // Passa a struct vazia para a classe preencher
    receptor.receber(pacote);

    String id = String(pacote.idSensor);
    float valor = pacote.valor;

    Serial.print("Recebido [" + id + "]: " + String(valor));

    // Despacha o dado para a tela OLED/LCD e pro Cartão SD
    telas.atualizar(id, valor);
    //sdCard.salvarDado(id, valor);
    
    // Roteador de variáveis para o Dashboard da Nuvem
    if (id == "Temp_Interna") {
        temp_interna = valor;
        Serial.println(" -> Cloud OK");
    }
    else if (id == "Temp_Externa") {
        temp_externa = valor;
        Serial.println(" -> Cloud OK");
    }
    else if (id == "Umidade_Sala") {
        umidade = valor;
        Serial.println(" -> Cloud OK");
    }
    else if (id == "Cooler") {
        cooler_rpm = valor;
        Serial.println(" -> Cloud OK");
    }
    else if (id == "Luz_Ambiente") {
        luz_ambiente = valor;
        Serial.println(" -> Cloud OK");
    }
    else if (id == "Luz_Interna") {
        luz_interna = valor;
        Serial.println(" -> Cloud OK");
    }
    else {
        Serial.println(" -> ID Desconhecido (Descartado)");
    }
  }
}
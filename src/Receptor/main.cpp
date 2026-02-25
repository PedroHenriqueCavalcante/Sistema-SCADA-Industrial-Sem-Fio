#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include "ComunicacaoWireless.h"

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

// --- MAPEAMENTO DE PINOS ---
#define CE_PIN 4  
#define CSN_PIN 5 
#define SD_CS_PIN 26 

RadioReceptor receptor(CE_PIN, CSN_PIN);
GerenciadorDeTelas telas;
GerenciadorDeArmazenamento sdCard(SD_CS_PIN);
RTC_DS3231 rtc; // Instância do Módulo de Relógio

// Variáveis espelhadas no Arduino Cloud
float temp_interna;
float temp_externa;
float umidade;
float cooler_rpm;
float luz_ambiente;
float luz_interna;
String timestamp_nuvem; // NOVA VARIÁVEL PARA O CLOUD!

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
  ArduinoCloud.addProperty(timestamp_nuvem, Permission::Read); // String da Data/Hora
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\n=== INICIANDO RECEPTOR E DATALOGGER LITEME ===");

  SPI.begin(18, 19, 23); // Via Expressa
  Wire.begin();          // Inicializa o I2C para a Tela e o RTC

  telas.iniciar();
  sdCard.iniciar();

  // Inicializa o RTC
  Serial.print("Iniciando Módulo RTC... ");
  if (!rtc.begin()) {
    Serial.println("FALHA! RTC nao encontrado no barramento I2C.");
  } else {
    Serial.println("OK!");
    // Se o RTC perdeu a energia da bateria moeda, ele ajusta com a hora da compilação:
    if (rtc.lostPower()) {
      Serial.println("RTC perdeu energia, ajustando a hora...");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  Serial.println("Conectando ao Arduino Cloud e Wi-Fi...");
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  
  Serial.print("Iniciando NRF24L01... ");
  if (!receptor.begin()) {
    Serial.println("ERRO CRITICO: Radio nao encontrado!");
  } else {
    Serial.println("Radio OK! Ouvindo o ar...");
  }
}

void loop() {
  ArduinoCloud.update(); 
  
  if (receptor.available()) { 
    PacoteDados pacote;
    receptor.receber(pacote);

    String id = String(pacote.idSensor);
    float valor = pacote.valor;

    // --- CRIAÇÃO DO TIMESTAMP ---
    DateTime agora = rtc.now();
    char bufferData[22];
    // Formata no padrão BR legível: DD/MM/YYYY HH:MM:SS
    sprintf(bufferData, "%02d/%02d/%04d %02d:%02d:%02d", 
            agora.day(), agora.month(), agora.year(), 
            agora.hour(), agora.minute(), agora.second());
    
    String strDataHora = String(bufferData);

    Serial.print("[" + strDataHora + "] " + id + ": " + String(valor));

    // Atualiza Nuvem e Periféricos locais
    timestamp_nuvem = strDataHora; 
    telas.atualizar(id, valor);
    sdCard.salvarDado(strDataHora, id, valor); 
    
    // Roteador de variáveis Cloud
    if (id == "Temp_Interna") temp_interna = valor;
    else if (id == "Temp_Externa") temp_externa = valor;
    else if (id == "Umidade_Sala") umidade = valor;
    else if (id == "Cooler") cooler_rpm = valor;
    else if (id == "Luz_Ambiente") luz_ambiente = valor;
    else if (id == "Luz_Interna") luz_interna = valor;

    Serial.println(" -> Processado");
  }
}
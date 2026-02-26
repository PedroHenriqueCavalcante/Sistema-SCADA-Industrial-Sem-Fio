#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include "ComunicacaoWireless.h"

#ifdef CD //Isso aqui é pra evitar conflito com a definição de CD entre as bibliotecas. Tinha dado erro antes
#undef CD
#endif

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "Telas.h"
#include "Armazenamento.h"

//SSID e senha do hotspot do celular de Pedro pra compartilhar na hora da apresentação
const char SSID[] = "Pedro";   
const char PASS[] = "senha123";   

//Configurações do WEMOS D1 R32 ESP32 que foi cadastrado no Arduino Cloud. NÃO MUDAR ISSO DAQUI!!!!
const char DEVICE_LOGIN_NAME[]  = "9b729cba-2ed7-4949-b8cb-a75527f918f1";
const char DEVICE_KEY[]         = "2JvzBjP4@mEfTzTgHuOQpe?Rp";

#define CE_PIN 4  
#define CSN_PIN 5 
#define SD_CS_PIN 26 

RadioReceptor receptor(CE_PIN, CSN_PIN);
GerenciadorDeTelas telas;
GerenciadorDeArmazenamento sdCard(SD_CS_PIN);
RTC_DS3231 rtc;

//Variáveis espelhadas no Arduino Cloud. Tem que ser IDÊNTICO ao Arduino Cloud, se não, não funciona
float temp_interna;
float temp_externa;
float umidade;
float cooler_rpm;
float luz_ambiente;
float luz_interna;
String timestamp_nuvem;

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS); //Isso aqui é pra conectar ao Wi-Fi

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(temp_interna, Permission::Read);
  ArduinoCloud.addProperty(temp_externa, Permission::Read);
  ArduinoCloud.addProperty(umidade, Permission::Read);
  ArduinoCloud.addProperty(cooler_rpm, Permission::Read);
  ArduinoCloud.addProperty(luz_ambiente, Permission::Read);
  ArduinoCloud.addProperty(luz_interna, Permission::Read);
  ArduinoCloud.addProperty(timestamp_nuvem, Permission::Read);
}

void setup() {
  
  Serial.begin(115200);
  delay(1500);
  Serial.println("Iniciando o receptor do sistema SCADA");

  SPI.begin(18, 19, 23);
  Wire.begin();

  telas.iniciar();
  sdCard.iniciar();

  Serial.print("Iniciando Módulo RTC... ");

  if (!rtc.begin()) {
    Serial.println("FALHA! RTC nao encontrado no barramento I2C.");
  } 
  
  else {
    Serial.println("OK!");
    
    if (rtc.lostPower()) { //Se a gente der o azar de o RTC perder a energia da bateria, ele ajusta com a hora da compilação:
      Serial.println("RTC perdeu energia, ajustando a hora");
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
  } 
  
  else {
    Serial.println("Radio OK! Rádio ouvindo!");
  }
}

void loop() {

  ArduinoCloud.update(); 
  
  if (receptor.available()) { 
    PacoteDados pacote;
    receptor.receber(pacote);

    String id = String(pacote.idSensor);
    float valor = pacote.valor;

    DateTime agora = rtc.now(); //Criação do timestamp
    char bufferData[22];
    
    sprintf(bufferData, "%02d/%02d/%04d %02d:%02d:%02d", //Formata no padrão BR legível: DD/MM/YYYY HH:MM:SS
            agora.day(), agora.month(), agora.year(), 
            agora.hour(), agora.minute(), agora.second());
    
    String strDataHora = String(bufferData);

    Serial.print("[" + strDataHora + "] " + id + ": " + String(valor));

    //Atualiza Nuvem e periféricos
    timestamp_nuvem = strDataHora; 
    telas.atualizar(id, valor);
    sdCard.salvarDado(strDataHora, id, valor); 
    
    //Roteador de variáveis Cloud
    if (id == "Temp_Interna") temp_interna = valor;
    else if (id == "Temp_Externa") temp_externa = valor;
    else if (id == "Umidade_Sala") umidade = valor;
    else if (id == "Cooler") cooler_rpm = valor;
    else if (id == "Luz_Ambiente") luz_ambiente = valor;
    else if (id == "Luz_Interna") luz_interna = valor;
  }
}
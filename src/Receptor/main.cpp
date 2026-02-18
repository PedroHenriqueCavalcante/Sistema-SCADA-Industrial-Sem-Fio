#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "ComunicacaoWireless.h"
#ifdef CD
#undef CD
#endif
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "Telas.h"


//CONFIGURAÇÕES DO WI-FI
const char SSID[]     = "NOME_DO_SEU_WIFI";     // <--- MUDE AQUI
const char PASS[]     = "SENHA_DO_SEU_WIFI";     // <--- MUDE AQUI

//Configurações do Arduino Cloud
const char DEVICE_LOGIN_NAME[]  = "COPIE_DO_SITE_DEVICE_ID";
const char DEVICE_KEY[]         = "COPIE_DO_SITE_SECRET_KEY";


//Configurações do rádio [CE no D2 (GPIO4), CSN no D8 (GPIO15)]
#define CE_PIN 2  
#define CSN_PIN 15

RF24 radio(CE_PIN, CSN_PIN);
const uint64_t enderecoPipe = 0x1234567890LL;

GerenciadorDeTelas telas;

//O nome tem que estar igual no Arduino Cloud
float temp_interna;
float temp_externa;
float umidade;
float cooler_rpm;
float luz_ambiente;
float luz_interna;

//Função necessária para conectar (Callback)
WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

//Função de configuração das variáveis (boilerplate do Arduino Cloud)
void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  // Mapeando as variáveis do código para a nuvem
  ArduinoCloud.addProperty(temp_interna, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(temp_externa, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(umidade, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cooler_rpm, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(luz_ambiente, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(luz_interna, READ, ON_CHANGE, NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\nIniciando o Receptor SCADA");

  telas.iniciar();

  // A. Inicia Arduino Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  Serial.print("Iniciando Radio... ");
  if (!radio.begin()) {
    Serial.println("ERRO: Radio nao encontrado!");
    while(1); // Trava se não achar o rádio
  }
  
  radio.setChannel(5);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, enderecoPipe);
  radio.startListening();
  
  Serial.println("Radio OK! Aguardando dados...");
}

void loop() {

  ArduinoCloud.update(); //Mantém a conexão com a nuvem viva

  
  if (radio.available()) { //Verifica se chegou dado pelo Rádio
    PacoteDados pacote;
    radio.read(&pacote, sizeof(PacoteDados));

    String id = String(pacote.idSensor);//Converte o char array para String para facilitar comparação
    float valor = pacote.valor;

    Serial.print("Recebido [" + id + "]: " + String(valor));

    telas.atualizarLCD(id, valor);
    telas.atualizarOLED(id, valor);
    
    if (id == "Temp_Interna") {
        temp_interna = valor;
        Serial.println(" -> Atualizado Cloud: Temp Interna");
    }
    else if (id == "Temp_Externa") {
        temp_externa = valor;
        Serial.println(" -> Atualizado Cloud: Temp Externa");
    }
    else if (id == "Umidade_Sala") {
        umidade = valor;
        Serial.println(" -> Atualizado Cloud: Umidade");
    }
    else if (id == "Cooler") {
        cooler_rpm = valor;
        Serial.println(" -> Atualizado Cloud: RPM");
    }
    else if (id == "Luz_Ambiente") {
        luz_ambiente = valor;
        Serial.println(" -> Atualizado Cloud: Luz Amb");
    }
    else if (id == "Luz_Interna") {
        luz_interna = valor;
        Serial.println(" -> Atualizado Cloud: Luz Int");
    }
    else {
        Serial.println(" -> ID Desconhecido!");
    }
  }
}
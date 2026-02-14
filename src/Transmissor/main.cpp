#include <Arduino.h>
#include "ClassesSCADA.h"
#include "ComunicacaoWireless.h"
#include "Seguranca.h"

// Pinos dos Sensores
#define PINO_ONE_WIRE  4  // Fio de dados dos DS18B20
#define PINO_DHT       2  // Sensor DHT
#define PINO_OPTICO    7  // Sensor Óptico (TCRT5000)
#define PINO_LDR_1     A0 // LDR 1
#define PINO_LDR_2     A1 // LDR 2

// Pinos do Rádio NRF24L01 (NO ARDUINO)
#define PINO_CE  9
#define PINO_CSN 10

#define TIPO_DHT DHT22 

Watchdog dog; 

OneWire oneWire(PINO_ONE_WIRE);
DallasTemperature sensors(&oneWire);
DHT dht(PINO_DHT, TIPO_DHT);

// Instância do Rádio
RadioTransmissor meuRadio(PINO_CE, PINO_CSN);

// Endereços dos sensores DS18B20 (Lembrar de colocar os que Eugênio enviar aqui)
DeviceAddress enderecoSensorTemp1 = { 0x28, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
DeviceAddress enderecoSensorTemp2 = { 0x28, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11 };

// Array de ponteiros para a classe mãe. São 6 sensores
Sensores* listaSensores[6]; 

void setup() {
  Serial.begin(9600);
  dog.iniciar();
  Serial.println("INICIANDO SISTEMA SCADA E O SISTEMA DE SEGURANÇA");

  // Inicializa Hardware dos Sensores
  sensors.begin();
  dht.begin();

  // Inicializa o Rádio
  Serial.print("Iniciando Radio... ");
  meuRadio.begin();
  Serial.println("OK!");

  // INSTANCIAÇÃO (Polimorfismo)

  //Sensor Óptico
  listaSensores[0] = new TCRT5000("Cooler", "Externo", PINO_OPTICO);

  //Sensor DHT
  listaSensores[1] = new SensorDHT("Umidade_Sala", "Externo", &dht);

  //Sensor Temperatura DS18B20 #1
  listaSensores[2] = new DS18B20("Temp_Interna", "Interno", &sensors, enderecoSensorTemp1);

  //Sensor Temperatura DS18B20 #2
  listaSensores[3] = new DS18B20("Temp_Externa", "Externo", &sensors, enderecoSensorTemp2);

  //Sensor LDR #1
  listaSensores[4] = new LDR("Luz_Ambiente", "Externo", PINO_LDR_1);

  //Sensor LDR #2
  listaSensores[5] = new LDR("Luz_Interna", "Interno", PINO_LDR_2);
}

void loop() {
  Serial.println("\nNova leitura iniciando e sistema de segurança alimentado");
  dog.alimentar();

  for(int i = 0; i < 6; i++) {
    
    listaSensores[i]->ler(); // 1. Manda cada sensor ler

    // 2. Imprime o relatório no Serial (Para Debug)
    Serial.println(listaSensores[i]->getRelatorio());
    
    PacoteDados pacote;

    // A. Converte o ID (String) para char array (necessário para o rádio)
    // O '15' é o tamanho máximo que definimos na struct no arquivo .h
    listaSensores[i]->getId().toCharArray(pacote.idSensor, 15);

    // B. Pega o valor numérico (float)
    pacote.valor = listaSensores[i]->getValor();

    // C. Envia
    Serial.print("-> Enviando via Radio... ");
    if (meuRadio.enviar(pacote)) {
        Serial.println("Sucesso no envio");
    } else {
        Serial.println("Falha no envio (Sem resposta).");
    }

    // Pequeno delay para não congestionar o rádio entre um sensor e outro
    delay(250); 
  }

  // Espera 2 segundos para a próxima rodada completa
  delay(2000);
}
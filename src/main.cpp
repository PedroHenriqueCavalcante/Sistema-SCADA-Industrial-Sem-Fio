#include <Arduino.h>
#include "ClassesSCADA.h"

// 1. Setup dos objetos globais das bibliotecas
OneWire oneWire(4); // Pino do DS18B20
DallasTemperature sensors(&oneWire);
DHT dht(2, DHT22); // Pino 2, Tipo DHT22

// Endereços (exemplo fictício, use o scanner para achar os seus)
DeviceAddress end1 = { 0x28, 0xFF, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 };

// 2. Criação da Lista Polimórfica (Array Fixo)
Sensores* listaSensores[4]; 

void setup() {
  Serial.begin(9600);
  sensors.begin();
  dht.begin();

  // 3. Instanciando os objetos na lista
  listaSensores[0] = new DS18B20("Temp Motor", "Bloco A", &sensors, end1);
  listaSensores[1] = new LDR("Luz Sala", "Teto", A0); // Pino A0
  listaSensores[2] = new TCRT5000("RPM Eixo", "Esteira", 7); // Pino 7
  listaSensores[3] = new SensorDHT("Clima", "Painel", &dht);
}

void loop() {
  // 4. Loop Polimórfico
  for(int i = 0; i < 4; i++) {
    listaSensores[i]->ler(); // Cada um lê do seu jeito
    Serial.println(listaSensores[i]->getRelatorio());
    
    // Aqui você mandaria via Rádio NRF24...
  }
  delay(2000);
}
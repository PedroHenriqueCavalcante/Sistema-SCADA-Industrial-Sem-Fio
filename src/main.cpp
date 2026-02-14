#include <Arduino.h>
#include "ClassesSCADA.h"

// Pinos dos Sensores
#define PINO_ONE_WIRE  4  // Fio de dados dos DS18B20
#define PINO_DHT       2  // Sensor DHT
#define PINO_OPTICO    7  // Sensor Óptico (TCRT5000)
#define PINO_LDR_1     A0 // LDR 1
#define PINO_LDR_2     A1 // LDR 2

#define TIPO_DHT DHT22 

OneWire oneWire(PINO_ONE_WIRE);
DallasTemperature sensors(&oneWire);
DHT dht(PINO_DHT, TIPO_DHT);

// Endereços dos sensores DS18B20. MUDAR DEPOIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!<<<<<-----------
DeviceAddress enderecoSensorTemp1 = { 0x28, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
DeviceAddress enderecoSensorTemp2 = { 0x28, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11 };


// Array de ponteiros para a classe mãe. Tamanho 6 pois são 6 sensores.
Sensores* listaSensores[6]; 

void setup() {
  Serial.begin(9600);
  Serial.println("INICIANDO SISTEMA SCADA");

  sensors.begin();
  dht.begin();

  // 1. Sensor Óptico (RPM)
  listaSensores[0] = new TCRT5000("Cooler", "Externo", PINO_OPTICO);

  // 2. Sensor DHT (Temperatura e Umidade)
  listaSensores[1] = new SensorDHT("Umidade_Sala", "Externo", &dht);

  // 3. Sensor Temperatura DS18B20 #1
  listaSensores[2] = new DS18B20("Temp_Interna", "Interno", &sensors, enderecoSensorTemp1);

  // 4. Sensor Temperatura DS18B20 #2
  listaSensores[3] = new DS18B20("Temp_Externa", "Externo", &sensors, enderecoSensorTemp2);

  // 5. Sensor LDR #1
  listaSensores[4] = new LDR("Luz_Ambiente", "Externo", PINO_LDR_1);

  // 6. Sensor LDR #2
  listaSensores[5] = new LDR("Luz_Interna", "Interno", PINO_LDR_2);
}

void loop() {
  Serial.println("\n--- Nova leitura iniciando ---");

  // Loop Polimórfico: Trata todos os sensores igual, não importa o tipo
  for(int i = 0; i < 6; i++) {
    
    
    listaSensores[i]->ler();// 1. Manda ler todos os sensores. Cada um usa sua própria lógica definida no header (ClassesSCADA.h)

    // 2. Imprime o relatório
    // Nota: O DHT vai imprimir apenas a temperatura aqui, a menos que você tenha sobrescrito o getRelatorio() na classe dele para mostrar umidade.
    Serial.println(listaSensores[i]->getRelatorio());
    
    // Pequeno delay para não travar o serial
    delay(100);
  }

  // Espera 2 segundos para a próxima rodada
  delay(2000);
}
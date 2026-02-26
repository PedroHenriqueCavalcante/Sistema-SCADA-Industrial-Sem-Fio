#include <Arduino.h>
#include "ClassesSCADA.h"
#include "ComunicacaoWireless.h"
#include "Seguranca.h"

#define PINO_OPTICO    2  //TCRT5000
#define PINO_DHT       3  //Sensor DHT
#define PINO_ONE_WIRE  5  //Fio de dados dos DS18B20
#define PINO_LDR_1     A0 //LDR 1
#define PINO_LDR_2     A1 //LDR 2

//Pinos do Rádio NRF24L01
#define PINO_CE  9
#define PINO_CSN 10

#define TIPO_DHT DHT22 

Watchdog dog; 

OneWire oneWire(PINO_ONE_WIRE);
DallasTemperature sensors(&oneWire);
DHT dht(PINO_DHT, TIPO_DHT);

// Instância do Rádio
RadioTransmissor meuRadio(PINO_CE, PINO_CSN);

//Endereços dos sensores DS18B20 (os dois estão conectados ao mesmo tempo na porta 5)
DeviceAddress enderecoSensorTemp1 = {0x28, 0x81, 0x40, 0x49, 0xF6, 0xF0, 0x3C, 0x81}; //A pova d'água. Vai ser o externo
DeviceAddress enderecoSensorTemp2 = {0x28, 0x95, 0x35, 0xE8, 0x0F, 0x00, 0x00, 0x18}; //Parece um transistor. Sensor interno

//Array de ponteiros (Polimorfismo)
Sensores* listaSensores[6]; 

void setup() {
  Serial.begin(9600); 
  dog.iniciar();
  Serial.println("Iniciando o transmissor do sistema SCADA");

  sensors.begin();
  dht.begin();

  meuRadio.begin();
  Serial.println("OK! Rádio em UDP.");

  listaSensores[0] = new TCRT5000("Cooler", "Externo", PINO_OPTICO);
  listaSensores[1] = new SensorDHT("Umidade_Sala", "Externo", &dht);
  listaSensores[2] = new DS18B20("Temp_Interna", "Interno", &sensors, enderecoSensorTemp1);
  listaSensores[3] = new DS18B20("Temp_Externa", "Externo", &sensors, enderecoSensorTemp2);
  listaSensores[4] = new LDR("Luz_Ambiente", "Externo", PINO_LDR_1);
  listaSensores[5] = new LDR("Luz_Interna", "Interno", PINO_LDR_2);
}

void loop() {

  static unsigned long tempoAtual = 0;

  if (millis()-tempoAtual >= 2000) {

    Serial.println("\nNova leitura iniciando...");
    dog.alimentar();

    for(int i = 0; i < 6; i++) {
      
      listaSensores[i]->ler(); 
      Serial.println(listaSensores[i]->getRelatorio()); //Debug no monitor serial
      
      PacoteDados pacote = {0}; //Zera a struct para não enviar lixo de memória da leitura anterior

      listaSensores[i]->getId().toCharArray(pacote.idSensor, 16); //Copia o Nome do Sensor (Respeitando o limite de 16 caracteres da struct)
      
      pacote.valor = listaSensores[i]->getValor(); //Copia o Valor Numérico

      //O envio é feito com o bloco completo (Manda os 24 bytes de uma vez só pelo rádio)
      Serial.print("Atirando pacote struct no ar. ");
      meuRadio.enviar(pacote);
      Serial.println("Enviado.");

      //Delay para a antena do rádio não colapsar kk
      delay(250); 
    }

    tempoAtual = millis(); //Alimenta a variável que guarda o tempo da última leitura
  }
}
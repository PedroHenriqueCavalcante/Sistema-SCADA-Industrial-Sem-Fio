#ifndef COMUNICACAO_WIRELESS_H
#define COMUNICACAO_WIRELESS_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


struct PacoteDados { //Scruct que vai ser enviada para o ESP8266
    char idSensor[15]; // Nome do sensor (ex: "Temp_Interna")
    float valor;       // O valor lido (ex: 45.5)
    char unidade[5];   // Unidade (ex: "C", "RPM")
};

class RadioTransmissor {
    private:
        RF24 radio; // Objeto da biblioteca oficial
        const uint64_t enderecoPipe = 0x1234567890LL; // Endereço de envio

    public:

        RadioTransmissor(int pinCE, int pinCSN);
        void begin();
        bool enviar(PacoteDados dados);
};

RadioTransmissor::RadioTransmissor(int pinCE, int pinCSN):radio(pinCE, pinCSN) { // Construtor: Inicializa o objeto RF24 com os pinos CE e CSN

}

void RadioTransmissor::begin() {

    radio.begin(); 
    radio.setChannel(5); 
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    
    radio.openWritingPipe(enderecoPipe); // Abre o tubo para escrita (Tx)
    
    radio.stopListening(); // O transmissor não ouve, ele só fala
}

bool RadioTransmissor::enviar(PacoteDados dados) {
            
    return radio.write(&dados, sizeof(PacoteDados)); // Retorna 'true' se o envio foi recebido com sucesso (ACK)

}

#endif
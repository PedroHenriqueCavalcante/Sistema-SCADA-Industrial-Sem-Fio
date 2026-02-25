#ifndef COMUNICACAO_WIRELESS_H
#define COMUNICACAO_WIRELESS_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// =========================================================
// 1. O CONTRATO (Estrutura compartilhada por ambas as placas)
// =========================================================
struct PacoteDados { 
    char idSensor[16]; 
    float valor;       
    char unidade[4];   
};

// =========================================================
// 2. MÁQUINA DO TRANSMISSOR (Para o Arduino Uno)
// =========================================================
class RadioTransmissor {
    private:
        RF24 radio; 
        const uint64_t enderecoPipe = 0x1234567890LL; 

    public:
        RadioTransmissor(int pinCE, int pinCSN) : radio(pinCE, pinCSN) {}

        void begin() {
            radio.begin(); 
            radio.setAutoAck(false); 
            radio.setChannel(115); 
            radio.setDataRate(RF24_250KBPS);
            radio.setPALevel(RF24_PA_LOW); 
            radio.openWritingPipe(enderecoPipe); 
            radio.stopListening(); 
        }

        bool enviar(PacoteDados dados) {
            return radio.write(&dados, sizeof(PacoteDados)); 
        }
};

// =========================================================
// 3. MÁQUINA DO RECEPTOR (Para o ESP32)
// =========================================================
class RadioReceptor {
    private:
        RF24 radio; 
        const uint64_t enderecoPipe = 0x1234567890LL; 

    public:
        RadioReceptor(int pinCE, int pinCSN) : radio(pinCE, pinCSN) {}

        bool begin() {
            if (!radio.begin()) return false;
            
            radio.setAutoAck(false); 
            radio.setChannel(115); 
            radio.setDataRate(RF24_250KBPS);
            radio.setPALevel(RF24_PA_LOW); 
            radio.openReadingPipe(1, enderecoPipe); 
            radio.startListening(); 
            return true;
        }

        bool available() {
            return radio.available();
        }

        void receber(PacoteDados &dados) {
            radio.read(&dados, sizeof(PacoteDados));
        }
};

#endif
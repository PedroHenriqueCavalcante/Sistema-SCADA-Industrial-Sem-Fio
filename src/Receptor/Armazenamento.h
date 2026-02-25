#ifndef ARMAZENAMENTO_H
#define ARMAZENAMENTO_H

#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

class GerenciadorDeArmazenamento {
    private:
        uint8_t pinoCS;
        const char* nomeArquivo;
        bool cartaoPronto;
        SPIClass spiSD; // Criando uma via SPI exclusiva para o SD!

    public:
        GerenciadorDeArmazenamento(uint8_t pino);
        void iniciar();
        void salvarDado(String dataHora, String idSensor, float valor);
};

GerenciadorDeArmazenamento::GerenciadorDeArmazenamento(uint8_t pino) : spiSD(HSPI) {
    this->pinoCS = pino;
    this->nomeArquivo = "/datalogger.csv"; 
    this->cartaoPronto = false;
}

void GerenciadorDeArmazenamento::iniciar() {
    // Inicia a via secundária (HSPI) nos pinos: SCK=14, MISO=12, MOSI=13, SS=26
    spiSD.begin(14, 12, 13, pinoCS);

    Serial.print(F("Iniciando Cartao SD (Via Isolada)... "));
    
    // Passamos a nossa via spiSD exclusiva para a biblioteca do SD
    if (!SD.begin(pinoCS, spiSD)) {
        Serial.println(F("FALHA! Verifique fios do HSPI."));
        cartaoPronto = false;
        return;
    }
    Serial.println(F("OK!"));
    cartaoPronto = true;

    if (!SD.exists(nomeArquivo)) {
        File arquivo = SD.open(nomeArquivo, FILE_WRITE);
        if (arquivo) {
            arquivo.println("DATA_HORA;ID_SENSOR;VALOR");
            arquivo.close();
        }
    }
}

void GerenciadorDeArmazenamento::salvarDado(String dataHora, String idSensor, float valor) {
    if (!cartaoPronto) return; 

    File arquivo = SD.open(nomeArquivo, FILE_APPEND);
    
    if (arquivo) {
        arquivo.print(dataHora);
        arquivo.print(";");
        arquivo.print(idSensor);
        arquivo.print(";");
        arquivo.println(valor); 
        arquivo.close();
    } else {
        Serial.print(F(" -> Erro SD"));
    }
}

#endif
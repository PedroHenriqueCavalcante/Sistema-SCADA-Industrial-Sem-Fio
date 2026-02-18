#ifndef ARMAZENAMENTO_H
#define ARMAZENAMENTO_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class GerenciadorDeArmazenamento {
    private:
        uint8_t pinoCS;
        const char* nomeArquivo;

    public:
        
        GerenciadorDeArmazenamento(uint8_t pino);
        void iniciar();
        void salvarDado(String idSensor, float valor);
};

GerenciadorDeArmazenamento::GerenciadorDeArmazenamento(uint8_t pino) {
    this->pinoCS = pino;
    this->nomeArquivo = "/datalogger.csv"; 
}

void GerenciadorDeArmazenamento::iniciar() {

    Serial.print(F("Iniciando Cartao SD... "));
    
    if (!SD.begin(pinoCS)) {
        Serial.println(F("FALHA! Verifique o cartao."));
        return;
    }
    Serial.println(F("OK!"));

    if (!SD.exists(nomeArquivo)) {
        File arquivo = SD.open(nomeArquivo, FILE_WRITE);
        if (arquivo) {
            arquivo.println("ID_SENSOR;VALOR;TEMPO_MS");
            arquivo.close();
        }
    }
}

void GerenciadorDeArmazenamento::salvarDado(String idSensor, float valor) {
    File arquivo = SD.open(nomeArquivo, FILE_WRITE);
    
    if (arquivo) {
        arquivo.print(idSensor);
        arquivo.print(";");
        arquivo.print(valor);
        arquivo.print(";");
        arquivo.println(millis()); // Salva o tempo (ms) desde que ligou
        arquivo.close();
        Serial.println(F(" -> Salvo no SD."));
    } else {
        Serial.println(F(" -> Erro ao abrir arquivo para escrita."));
    }
}

#endif
#ifndef SEGURANCA_H
#define SEGURANCA_H

#include <Arduino.h>
#include <avr/wdt.h> // Biblioteca nativa do Watchdog para Arduino UNO

class Watchdog {
    public:
        void iniciar();
        void alimentar();
        void forcarReinicializacao();
};


void Watchdog::iniciar() { //Inicia o Cão de Guarda
    wdt_enable(WDTO_8S); 
}

void Watchdog::alimentar() { //Reinicia a contagem dos 8 segundos
    wdt_reset();
}

void Watchdog::forcarReinicializacao() { //Força um reset via software da placa
    wdt_enable(WDTO_15MS); // Define tempo mínimo de 15ms para reinicialização
    while(true) {} // Trava propositalmente até reiniciar
}

#endif
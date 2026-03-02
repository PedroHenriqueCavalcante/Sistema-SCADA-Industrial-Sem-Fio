#ifndef CLASSES_SCADA_H
#define CLASSES_SCADA_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

class Sensores {
    protected:
        String id; //Nome do sensor
        String localizacao; //Onde ele está localizado
        float leitura; //Valor da leitura dele (temperatura, umidade, RPM, etc.)
        String unidade; //Unidade de medida da leitura (C, %, RPM, etc.)

    public:
        Sensores(String _id, String _loc, String _uni) {
            id = _id;
            localizacao = _loc;
            unidade = _uni;
            leitura = 0.0;
        }

        virtual void ler(){
            //Em branco apenas para ser sobrescrito pelas subclasses
        }

        //Método virtual: Pode ser sobrescrito (pelo DHT, por exemplo) para que a gente use polimorfismo
        virtual String getRelatorio() {
            return "[" + id + "] " + localizacao + ": " + String(leitura) + " " + unidade;
        }

        float getValor() { 
            return leitura; 
        }

        String getId() { 
            return id; 
        }
};

//---------------------------------------------DS18B20---------------------------------------------------------------------------------------------------

class DS18B20:public Sensores {
    private:
        DallasTemperature* _dallas; //Ponteiro para o controlador
        DeviceAddress _endereco;    //Endereço específico deste sensor

    public:

        DS18B20(String _id, String _loc, DallasTemperature* dallas, DeviceAddress enderecoFisico);

        void ler() override;
};

//Método construtor da classe
DS18B20::DS18B20(String _id, String _loc, DallasTemperature* dallas, DeviceAddress enderecoFisico):Sensores(_id, _loc, "C") {

    _dallas = dallas;
    memcpy(_endereco, enderecoFisico, 8); //Copia o endereço recebido para a variável interna
}

//Método override de leitura
void DS18B20::ler() {
    _dallas->requestTemperaturesByAddress(_endereco);
    float tempC = _dallas->getTempC(_endereco);

    if(tempC == DEVICE_DISCONNECTED_C) { //Verifica se houve erro de leitura (-127)
        Serial.println("Erro: Sensor " + id + " desconectado!");
    } 
    
    else {
        leitura = tempC;
    }
}

//---------------------------------------------LDR-------------------------------------------------------------------------------------------------------

class LDR:public Sensores {
    private:
        int pinoAnalogico;

    public:
        LDR(String _id, String _loc, int _pino);

        void ler() override;
};

//Método construtor da classe
LDR::LDR(String _id, String _loc, int _pino):Sensores(_id, _loc, "%") {
    pinoAnalogico = _pino;
    pinMode(pinoAnalogico, INPUT);
}

//Método override de leitura
void LDR::ler() {

    int valorBruto = analogRead(pinoAnalogico);
    // Converte 0-1023 (analógico) para 0-100% (porcentagem)
    // LDR: Quanto mais luz, menor a resistência (depende do circuito)
    // Assumindo pull-down:
    leitura = map(valorBruto, 300, 1023, 0, 100);
    
    // Se o valor mapeado for negativo, trava em 0
    if (leitura < 0) {
        leitura = 0;
    }
    // Opcional: se passar de 100%, trava em 100
    if (leitura > 100) {
        leitura = 100;
    }
}

//---------------------------------------------DHT--------------------------------------------------------------------------------------------------------

class SensorDHT:public Sensores {
    private:
        DHT* _dht; //Ponteiro para o objeto da biblioteca
        float umidade; 

    public:
        
        SensorDHT(String _id, String _loc, DHT* dhtObj); //Recebe o objeto DHT já configurado no main
        void ler() override;
        
        String getRelatorio() override; 
};

//Método construtor da classe
SensorDHT::SensorDHT(String _id, String _loc, DHT* dhtObj):Sensores(_id, _loc, "%") {
    _dht = dhtObj;
    umidade = 0.0;
}

void SensorDHT::ler() {

    float t = _dht->readTemperature();
    float h = _dht->readHumidity();
    
    if (isnan(t) || isnan(h)) { 
        Serial.println("Falha ao ler DHT!");
    } 
    
    else {
        leitura = h; 
        umidade = t; 
    }
}

String SensorDHT::getRelatorio() { 
    return "[" + id + "] " + localizacao + ": " + String(umidade) + "C / " + String(leitura) + "%";
}

#endif
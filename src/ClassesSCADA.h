#ifndef CLASSES_SCADA_H
#define CLASSES_SCADA_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

class Sensores {
    protected:
        String id;
        String localizacao;
        float leitura; 
        String unidade;

    public:
        Sensores(String _id, String _loc, String _uni) {
            id = _id;
            localizacao = _loc;
            unidade = _uni;
            leitura = 0.0;
        }

        virtual void ler(){

        }

        // Método virtual: Pode ser sobrescrito (pelo DHT, por exemplo)
        virtual String getRelatorio() {
            return "[" + id + "] " + localizacao + ": " + String(leitura) + " " + unidade;
        }

        float getValor() { return leitura; }
        String getId() { return id; }
};


class DS18B20 : public Sensores {
    private:
        DallasTemperature* _dallas; // Ponteiro para o controlador
        DeviceAddress _endereco;    // Endereço específico deste sensor

    public:
        // Construtor recebe o controlador e o endereço físico
        DS18B20(String _id, String _loc, DallasTemperature* dallas, DeviceAddress enderecoFisico) 
          : Sensores(_id, _loc, "C") {
            _dallas = dallas;
            // Copia o endereço recebido para a variável interna
            memcpy(_endereco, enderecoFisico, 8);
        }

        void ler() override {
            // Pede para o sensor ler a temperatura
            _dallas->requestTemperaturesByAddress(_endereco);
            float tempC = _dallas->getTempC(_endereco);

            // Verifica se houve erro de leitura (-127)
            if(tempC == DEVICE_DISCONNECTED_C) { 
                Serial.println("Erro: Sensor " + id + " desconectado!");
                // Mantém a última leitura válida ou zera (opcional)
            } else {
                leitura = tempC;
            }
        }
};

// ==========================================
// 3. SENSOR DE LUMINOSIDADE (LDR)
// ==========================================
class LDR : public Sensores {
    private:
        int pinoAnalogico;

    public:
        LDR(String _id, String _loc, int _pino) 
          : Sensores(_id, _loc, "%") {
            pinoAnalogico = _pino;
            pinMode(pinoAnalogico, INPUT);
        }

        void ler() override {
            int valorBruto = analogRead(pinoAnalogico);
            // Converte 0-1023 (analógico) para 0-100% (porcentagem)
            // LDR: Quanto mais luz, menor a resistência (depende do circuito)
            // Assumindo pull-down:
            leitura = map(valorBruto, 0, 1023, 0, 100);
        }
};


class TCRT5000 : public Sensores {
    private:
        int pinoDigital;
        
    public:
        TCRT5000(String _id, String _loc, int _pino) 
          : Sensores(_id, _loc, "RPM") {
            pinoDigital = _pino;
            pinMode(pinoDigital, INPUT);
        }

        void ler() override {
            // Lógica Simplificada para TP (sem interrupção):
            // Lê o pulso atual. pulseIn espera o pino ir de LOW para HIGH.
            // Timeout de 100ms para não travar o Arduino
            unsigned long duracao = pulseIn(pinoDigital, HIGH, 100000); 
            
            if (duracao > 0) {
                // Conversão básica de tempo de pulso para RPM (aproximada)
                // RPM = 60 segundos / tempo de uma volta (em segundos)
                // Exemplo genérico (ajustar conforme sua roda/hélice)
                leitura = 60000000.0 / (duracao * 2); // Multiplicador fictício
            } else {
                leitura = 0; // Motor parado
            }
        }
};


class SensorDHT:public Sensores {
    private:
        DHT* _dht; // Ponteiro para o objeto da biblioteca
        float umidade; 

    public:
        // Recebe o objeto DHT já configurado no main
        SensorDHT(String _id, String _loc, DHT* dhtObj) 
          : Sensores(_id, _loc, "C") {
            _dht = dhtObj;
            umidade = 0.0;
        }

        void ler() override {
            // O DHT lê devagar, ideal é não chamar toda hora
            float t = _dht->readTemperature();
            float h = _dht->readHumidity();

            // Verifica se é NaN (Not a Number = erro de leitura)
            if (isnan(t) || isnan(h)) {
                Serial.println("Falha ao ler DHT!");
            } else {
                leitura = t; // Salva temp na variável da mãe
                umidade = h; // Salva umidade na variável própria
            }
        }

        // Sobrescreve o relatório para mostrar os dois valores
        String getRelatorio() override {
             return "[" + id + "] " + localizacao + ": " + String(leitura) + "C / " + String(umidade) + "%";
        }
};

#endif
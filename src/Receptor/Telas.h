#ifndef TELAS_H
#define TELAS_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configurações do OLED
#define LARGURA_OLED 128
#define ALTURA_OLED 64
#define ENDERECO_OLED 0x3C // Geralmente é 0x3C

// Configurações do LCD
#define ENDERECO_LCD 0x27 // Pode ser 0x27 ou 0x3F (use i2c scanner se não der imagem)
#define COLUNAS_LCD 16
#define LINHAS_LCD 2

class GerenciadorDeTelas {
    private:
        LiquidCrystal_I2C lcd;
        Adafruit_SSD1306 oled;

    public:

        GerenciadorDeTelas();
        void iniciar();
        void atualizarLCD(String idSensor, float valor);
        void atualizarOLED(String id, float valor);
};

GerenciadorDeTelas::GerenciadorDeTelas():lcd(ENDERECO_LCD, COLUNAS_LCD, LINHAS_LCD), oled(LARGURA_OLED, ALTURA_OLED, &Wire, -1) {
       
}

void GerenciadorDeTelas::iniciar() {
    // Inicia LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando...");

    // Inicia OLED
    if(!oled.begin(SSD1306_SWITCHCAPVCC, ENDERECO_OLED)) {
        Serial.println(F("Falha ao iniciar OLED"));
    }
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(1);
    oled.setCursor(0,0);
    oled.println("Gateway SCADA");
    oled.println("Aguardando...");
    oled.display();
}

void GerenciadorDeTelas::atualizarLCD(String idSensor, float valor) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Recebido:");
    
    lcd.setCursor(0, 1);
    // Formata: "Temp_Sala: 25.5"
    lcd.print(idSensor.substring(0, 10)); // Corta nome se for longo
    lcd.print(":");
    lcd.print(valor, 1);
}

void GerenciadorDeTelas::atualizarOLED(String id, float valor) {
    oled.clearDisplay();
    
    // Cabeçalho
    oled.setTextSize(1);
    oled.setCursor(0,0);
    oled.println("--- MONITOR ---");
    
    // Dado Principal Grande
    oled.setTextSize(2);
    oled.setCursor(0, 20);
    oled.println(valor);
    
    // Nome do Sensor embaixo
    oled.setTextSize(1);
    oled.setCursor(0, 45);
    oled.println(id);
    
    oled.display();
}


#endif
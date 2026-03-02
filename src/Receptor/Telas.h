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
#define ENDERECO_OLED 0x3C //Geralmente é 0x3C

// Configurações do LCD
#define LARGURA_OLED 128
#define ALTURA_OLED 64
#define ENDERECO_OLED 0x3C
#define ENDERECO_LCD 0x27 //Confirmar se é 0x27 ou 0x3F
#define COLUNAS_LCD 16
#define LINHAS_LCD 2

class GerenciadorDeTelas {
    private:
        LiquidCrystal_I2C lcd;
        Adafruit_SSD1306 oled;
        
        float temperaturaAmbiente = 0.0;
        float temperaturaExterna = 0.0; // Vamos usar Temp_Externa como motor p/ teste
        int umidade = 0;
        int luz1 = 0, luz2 = 0;

        // --- VARIÁVEIS DO GRÁFICO ---
        int grafico[128]; // Array para guardar o histórico

    public:

        GerenciadorDeTelas();
        void iniciar();
        void atualizar(String idSensor, float valor); // Método principal que recebe do rádio e distribui

    private:
        // Métodos internos de desenho
        void desenharLCD();
        void desenharOLED();
        void atualizarGrafico(float novoValor);
};

GerenciadorDeTelas::GerenciadorDeTelas():lcd(ENDERECO_LCD, COLUNAS_LCD, LINHAS_LCD), oled(LARGURA_OLED, ALTURA_OLED, &Wire, -1) {
    // Preenche o gráfico com linha reta inicial (meio da tela)
    for(int i=0; i<128; i++) grafico[i] = 32; 
}

void GerenciadorDeTelas::iniciar() {

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Sistema SCADA");
    lcd.setCursor(0, 1);
    lcd.print("Inicializando...");

    // Inicia OLED
    if(!oled.begin(SSD1306_SWITCHCAPVCC, ENDERECO_OLED)) {
        Serial.println(F("Falha OLED"));
    }
    oled.clearDisplay();
    oled.display();
}

void GerenciadorDeTelas::atualizar(String id, float valor) {
    
    if (id == "Temp_Externa") {
        temperaturaExterna = valor;
        atualizarGrafico(temperaturaExterna); // Só anda o gráfico quando chega a temperatura externa
    }
    else if (id == "Umidade_Sala") umidade = (int)valor;
    else if (id == "Temp_Interna") temperaturaAmbiente = valor; //Atualiza a memória interna baseada no ID que chegou
    else if (id == "Luz_Ambiente") luz1 = (int)valor;
    else if (id == "Luz_Interna") luz2 = (int)valor;

    // 2. Redesenha as telas com os dados atualizados
    desenharLCD();
    desenharOLED();
}

void GerenciadorDeTelas::atualizarGrafico(float valorTemp) {
    
    for(int i=0; i<127; i++) { // Move todos os pontos para a esquerda (efeito scroll)
        grafico[i] = grafico[i+1];
    }

    // Mapeia a temperatura para a altura do gráfico (pixels)
    // Diminuímos a escala (ex: 20 a 35°C) para que pequenas mudanças fiquem mais visíveis
    int ponto = map((int)(valorTemp * 10), 200, 350, 63, 16);
    
    // Limita para não sair da tela
    if(ponto < 16) ponto = 16; 
    if(ponto > 63) ponto = 63;

    // Adiciona o ponto novo no final
    grafico[127] = ponto;
}

void GerenciadorDeTelas::desenharLCD() {
    // Layout baseado no seu código:
    // Linha 0: Data/Hora (Como não temos RTC ainda, vou por um título ou tempo ligado)
    // Linha 1: Amb:25.5C U:60%

    lcd.setCursor(0, 0);
    lcd.print("SCADA Monitor   ");

    lcd.setCursor(0, 1);
    lcd.print("Int:"); 
    lcd.print(temperaturaAmbiente, 1);
    
    lcd.print(" U:");
    lcd.print(umidade);
    lcd.print("% ");
}

void GerenciadorDeTelas::desenharOLED() {
    oled.clearDisplay();
    
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    
    //Parte de cima
    oled.setCursor(0,0);

    oled.print("L1:"); oled.print(luz1);
    oled.print(" L2:"); oled.print(luz2); 
    
    oled.setCursor(0, 10); //Um pouco mais pra baixo
    oled.print("Externa: "); oled.print(temperaturaExterna, 1); oled.print(" C");

    //Parte do gráfico que fica mais para baixo
    //Desenha as linhas conectando os pontos do array
    for(int i=0; i<127; i++) {
        // Linha principal do gráfico (desenhada duas vezes para ficar um pouco mais grossa, mas sem preencher a tela toda)
        oled.drawLine(i, grafico[i], i+1, grafico[i+1], SSD1306_WHITE);
        oled.drawLine(i, grafico[i]+1, i+1, grafico[i+1]+1, SSD1306_WHITE);
    }
    
    oled.display();
}

#endif
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
#define LARGURA_OLED 128
#define ALTURA_OLED 64
#define ENDERECO_OLED 0x3C
#define ENDERECO_LCD 0x27 // Confirme se é 0x27 ou 0x3F
#define COLUNAS_LCD 16
#define LINHAS_LCD 2

class GerenciadorDeTelas {
    private:
        LiquidCrystal_I2C lcd;
        Adafruit_SSD1306 oled;
        
        float tAmbiente = 0.0;
        float tMotor = 0.0; // Vamos usar Temp_Externa como motor p/ teste
        int humidade = 0;
        int luz1 = 0, luz2 = 0, luz3 = 0;

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

GerenciadorDeTelas::GerenciadorDeTelas() 
    : lcd(ENDERECO_LCD, COLUNAS_LCD, LINHAS_LCD), 
      oled(LARGURA_OLED, ALTURA_OLED, &Wire, -1) 
{
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
    
    if (id == "Temp_Interna") tAmbiente = valor; //Atualiza a memória interna baseada no ID que chegou
    else if (id == "Umidade_Sala") humidade = (int)valor;
    else if (id == "Temp_Externa") { // Vamos usar essa como Temperatura do Motor
        tMotor = valor;
        atualizarGrafico(tMotor); // Só anda o gráfico quando chega temp do motor
    }
    else if (id == "Luz_Ambiente") luz1 = (int)valor;
    // Adicione outros IFs se tiver sensores separados para luz2 e luz3

    // 2. Redesenha as telas com os dados atualizados
    desenharLCD();
    desenharOLED();
}

void GerenciadorDeTelas::atualizarGrafico(float valorTemp) {
    
    for(int i=0; i<127; i++) { // Move todos os pontos para a esquerda (efeito scroll)
        grafico[i] = grafico[i+1];
    }

    // Mapeia a temperatura para a altura do gráfico (pixels)
    // Exemplo: 20°C = baixo (63), 60°C = alto (16)
    // Ajuste os valores 20 e 60 conforme a temperatura real do seu motor
    int ponto = map((int)valorTemp, 20, 60, 63, 16);
    
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
    lcd.print("SCADA Monitor   "); // Espaços para limpar linha

    lcd.setCursor(0, 1);
    lcd.print("Amb:"); 
    lcd.print(tAmbiente, 1); // 1 casa decimal
    
    lcd.print(" U:"); 
    lcd.print(humidade); 
    lcd.print("% "); // Espaço extra pra garantir limpeza
}

void GerenciadorDeTelas::desenharOLED() {
    oled.clearDisplay();
    
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    
    // --- PARTE DE CIMA (TEXTO) ---
    oled.setCursor(0,0);
    // Simula as 3 luzes (ou repete a mesma se só tiver um sensor)
    oled.print("L1:"); oled.print(luz1);
    oled.print(" L2:"); oled.print(luz2); 
    
    oled.setCursor(0, 10); // Um pouco mais pra baixo
    oled.print("Motor: "); oled.print(tMotor, 1); oled.print(" C");

    // --- PARTE DE BAIXO (GRÁFICO) ---
    // Desenha as linhas conectando os pontos do array
    for(int i=0; i<127; i++) {
        oled.drawLine(i, grafico[i], i+1, grafico[i+1], SSD1306_WHITE);
    }
    
    oled.display();
}

#endif
# Sistema SCADA Industrial Sem Fio

O roteiro que deve ser seguido para o nosso projeto está neste READ.ME. Podemos ajustar a qualquer momento.

## Objetivo

O objetivo do nosso sistema é monitorar remotamente o funcionamento de um motor industrial, garantindo integridade de dados e detecção automática de falhas de comunicação (Fail-Safe).

---

## Como vai funcionar?

Teremos um Arduino UNO R3 e um ESP32, que se comunicarão via nRF24L01 (comunicação de rádio sem fio).

O arduino lerá vários sensores que estarão dentro e fora de uma caixa (talvez de isopor) e enviará para o ESP32.
Daí, o ESP32 lerá os dados recebidos sem fio em formato de struct, os tratará, se for necessário, e enviará para o Arduino CLoud, que é um sistema de dashboard interativo que fica atualizando os dados na tela em tempo real. 

## Do que precisamos?

Pedro Henrique tem tudo, mas o que pensamos até agora é:

- 1 Arduino UNO R3
- 1 ESP32
- 2 módulos nRF24L01
- 1 display OLED 0,96"
- 1 LCD 16x2
- 2 sensores de temperatura DS18B20
- 1 sensor óptico
- 2 fotosensores LDR
- 1 buzzer passivo 5V
- 1 módulo para cartão microSD
- 1 cartão microSD
- LEDs
- Resistores
- Jumpers

## Como devemos pensar a lógica de programação?

Estamos na disciplina de Técnicas de Programação, em que aprendemos sobre:

- Classes
- Objetos
- Herança
- Polimorfismo
- UML

Portanto, precisamos adicionar esses 4 primeiros no nosso código e a UML no nossso relatório que será enviado para Fabrício.

## Quem ficará responsável pelo o que?

Para o código, quem tem experiência maior deve ficar na parte do desenvolvimento grosso e revisão do código dos demais. Todos vão participar da parte de código, mas eles serão revisado Por Pedro Henrique e Pedro Eugênio.

OBS1.: Quem não quiser participar da parte do código e quiser ficar apenas com a parte do relatório e da montagem da caixa, avise no grupo. 

OBS2.: Logicamente, a gente vai ter que produzir o relarório, então, todos devem colaborar, mas quem quiser ficar só com essa parte, deve se debruçar mais por ele (principalmente na parte de formatação de acordo com as regras ABNT e revisão do texto).

## Porque nosso projeto é bom?

- Confiabilidade: ele terá um sistema de Watchdog de comunicação. Se o transmissor falhar, o receptor sabe e avisa via LED, em vez de mostrar dados velhos no dashboard
- Eficiência: O datalogger é inteligente. Ele economiza memória em operação normal, mas aumenta a resolução de gravação durante crises
- Organização: O código usa uma Máquina de Estados, tornando o sistema previsível e fácil de dar manutenção

## Onde estudar mais para conseguir ajudar no código?

Uma sugestão de playlist é: https://youtube.com/playlist?list=PLx4x_zx8csUjczg1qPHavU1vw1IkBcm40&si=aiOoUn1asAsF_0ll. 

# InteliHorta

Regando uma horta de forma inteligente.

## Instalando ESP8266 no Arduino IDE

1) Abra a opção de preferências na Arduino IDE: File > Preferences
2) Insira http://arduino.esp8266.com/stable/package_esp8266com_index.json no campo “Additional Board Manager URLs”, e, logo após, clique no botão “OK”.
3) Vá para o gerenciador de placas: Tools > Board > Boards Manager…
4) Procure por ESP8266 e instale “esp8266”

## Adicionado as bibliotecas

Na opção de gerenciamento de bibliotecas, instale as seguintes versões:
1) ThingSpeak - Version 1.5.0
2) OneWire - Version 2.3.4
3) DallasTemperature - Version 3.8.0

## Definindo as chaves

1) Faça uma cópia do arquivo keys.h_template para keys.h
2) Defina as configurações do WiFi e do ThingSpeak no arquivo keys.h

## Testando

Faça upload do seu código para o NodeMCU, e vá ao ThingSpeak para verificar se ele está recebendo novos dados.

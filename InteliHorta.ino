/*
 * Código utilizado no projeto InteliHorta
 * 
 * Desenvolvedores:
 *    Antonio Deusany de Carvalho Junior
 *    Janaina da Silva Fortirer
 *    Celso
 *    Kessia
 *    Leticia
 *    Luan
 *    Silvia
 */

#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // v 1.5.0
#include "keys.h"
#include <OneWire.h> // v 2.3.4 ou 2.3.5
#include <DallasTemperature.h> // v 3.8.0

// WiFi
WiFiClient client;
uint8_t MAC[6] = {0xc8,0x2a,0x14,0x4e,0xc4,0x86}; // Endereço de MAC para a placa

// Temperatura (Sensor DS18B20)
#define ONE_WIRE_BUS D4 // Fio para comunicação com o Arduino (Normalmente o fio amarelo)
OneWire oneWire(ONE_WIRE_BUS); // Configura a biblioteca para se comunicar pelo porta selecionada
DallasTemperature sensors(&oneWire); // Configura a biblioteca para medir a temperatura

// Umidade do solo (Sensor soil moisture)
#define SM_PIN A0 // Fio para comunicação com o Arduino (Normalmente o fio amarelo)

// Bomba d'água
#define BOMBA_PIN D5
float umidadeCritica = 50; 
float umidadeBaixa = 60;
float TEMPIDEAL = 30; 
int tempoDeRegarMax = 20000; // Tempo máximo para regar em milissegundos
int tempoDeRegarMin = 10000; // Tempo máximo para regar em milissegundos

// Tempo sem coletar dados (em minutos)
int minutosDeDescanso = 1; 

void setup(void)
{
  Serial.begin(9600);

  // WiFi
  wifi_set_macaddr(STATION_IF, MAC); // Define novo endereço de MAC
  conectarWiFi(); // Configura o Wifi

  // ThingSpeak
  ThingSpeak.begin(client); // Inicia o cliente IoT

  // DS18B20
  sensors.begin(); // Inicia a biblioteca que mede a temperatura
  pinMode(ONE_WIRE_BUS, INPUT); // Define a porta do sensor como entrada

  // Bomba d'água
  pinMode (BOMBA_PIN, OUTPUT);
  digitalWrite(BOMBA_PIN, HIGH);
  
  delay(1000); // Aguarda um tempo antes de iniciar a coleta de dados
}

void loop(void)
{
  // Verifica a conexão WiFi
  while (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  
  // Temperatura
  requisitarDadosDeDS18B20();
  float ds18b20T = sensors.getTempCByIndex(0);
  Serial.print("Temperatura do solo:\t");
  Serial.print(ds18b20T, 1);
  Serial.println("C");
  
  // Umidade do Solo
  float soilMoisture = analogRead(SM_PIN);
  float smMapped = map(soilMoisture,750,250,0,100); // Mapeia os valores para uma faixa em %
  Serial.print("Umidade do solo:\t");
  Serial.print(smMapped, 1);
  Serial.println("%");
  
  // Controle da da bomba d'água 
  int coloqueiAgua = 0;

  if (smMapped <= umidadeBaixa && ds18b20T > TEMPIDEAL)  {         
              Serial.println("Colocando mais agua.");
              digitalWrite(BOMBA_PIN, LOW);  // Ligar  a bomba
              delay (tempoDeRegarMax);
              digitalWrite(BOMBA_PIN, HIGH);  // Desligar a bomba
              coloqueiAgua = 1;
              Serial.println("Coloquei muita água.");
    } else 
  if (smMapped <= umidadeCritica)  {        
           Serial.println("Colocando água."); 
              digitalWrite(BOMBA_PIN, LOW);  // Ligar  a bomba
              delay (tempoDeRegarMin);
              digitalWrite(BOMBA_PIN, HIGH);  // Desligar a bomba
              coloqueiAgua = 1;
              Serial.println("Coloquei água.");
    } else {
              Serial.println("Nao coloquei água.");
    }
  
  // Define os valores dos campos a serem enviados para a Nuvem
  ThingSpeak.setField(1, ds18b20T);
  ThingSpeak.setField(2, smMapped);
  ThingSpeak.setField(3, coloqueiAgua);

  // Envia os dados para o ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Envia dados e retorna informação sobre o envio
  if(x == 200){
    Serial.println("Canal atualizado com sucesso.");
  }
  else{
    Serial.println("Problema ao atualizar o canal. Código do erro no HTTP: " + String(x));
  }

  Serial.println("\nAgora vou dormir!\n");
//  delay(minutosDeDescanso * 60e6);
  ESP.deepSleep(minutosDeDescanso * 60e6); // Método para desligar o ESP (tempo em microssegundos)
}

// Método para (re)conectar à rede WiFi
void conectarWiFi() {
  Serial.print("Conectando-se à rede: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
}

// Método para requisitar dados do sensor de temperatura
void requisitarDadosDeDS18B20()
{
  sensors.requestTemperatures();
  
  // print data
  int numDev = sensors.getDeviceCount();
  for (int i = 0; i < numDev; i++) {
    Serial.print("Dispositivo ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(sensors.getTempCByIndex(i)); 
  }
}

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

// WiFi
WiFiClient client;

// Umidade do solo (Sensor soil moisture)
#define SM_PIN A0 // Porta analógica usada para comunicação (Onde se pluga o fio amarelo no NodeMCU)

// Bomba d'água
#define BOMBA_PIN D5
float umidadeBaixa = 60; // É preciso regar
float umidadeCritica = 50; // É preciso regar muito
int tempoDeRegarMin = 10000; // Tempo máximo para regar em milissegundos
int tempoDeRegarMax = 20000; // Tempo máximo para regar em milissegundos

// Tempo sem coletar dados (em minutos)
int minutosDeDescanso = 1; 

void setup(void)
{
  Serial.begin(9600);

  // WiFi
  //wifi_set_macaddr(STATION_IF, MAC); // Define novo endereço de MAC daplaca de rede se for preciso
  conectarWiFi(); // Configura o Wifi

  // ThingSpeak
  ThingSpeak.begin(client); // Inicia o cliente IoT

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
  
  // Umidade do Solo
  float soilMoisture = analogRead(SM_PIN);
  float smMapped = map(soilMoisture,750,250,0,100); // Mapeia os valores para uma faixa entre 0 e 100%
  Serial.print("Umidade do solo:\t");
  Serial.print(smMapped, 1);
  Serial.println("%");
  
  // Controle da da bomba d'água 
  int coloqueiAgua = 0;

  if (smMapped <= umidadeCritica)  {        
           Serial.println("Colocando água."); 
              digitalWrite(BOMBA_PIN, LOW);  // Ligar  a bomba
              delay (tempoDeRegarMax);
              digitalWrite(BOMBA_PIN, HIGH);  // Desligar a bomba
              coloqueiAgua = 1;
              Serial.println("Coloquei muita água.");
  } else
  if (smMapped <= umidadeBaixa)  {         
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
  ThingSpeak.setField(1, smMapped);
  ThingSpeak.setField(2, coloqueiAgua);

  // Envia os dados para o ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Envia dados e retorna informação sobre o envio
  if(x == 200){
    Serial.println("Canal atualizado com sucesso.");
  }
  else{
    Serial.println("Problema ao atualizar o canal. Código do erro no HTTP: " + String(x));
  }

  Serial.println("\nAgora vou dormir!\n");
  delay(minutosDeDescanso * 60e6);
  //ESP.deepSleep(minutosDeDescanso * 60e6); // Método para desligar o ESP (tempo em microssegundos)
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

//#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>
#include "keys.h"

// WiFi
WiFiClient client;

// ThingSpeak
const char* server = "api.thingspeak.com";

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS D4
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// DHT22 settings
dht DHT;
#define DHT22_PIN D5

// SOIL MOISTURE
#define SM_PIN A0

void setup(void)
{
  // Start serial port
  Serial.begin(9600);
  
  // Start WiFi
  wifiConnection();
  
  // Start up sensors' library
  sensors.begin();

  // Setup DS18B20  
  pinMode(ONE_WIRE_BUS, INPUT);
  
  // Setup DHT22
  pinMode(DHT22_PIN, INPUT);
  
  delay(1000);
}

void loop(void)
{
  // Verify connection
  while (WiFi.status() != WL_CONNECTED) {
    wifiConnection();
    delay(500);
    Serial.println(".");
  }

  // Get Data

  // DHT22
  requestDHT22Data();
  float dht22T = DHT.temperature;
  float dht22H = DHT.humidity;
  
  // DS18B20
  requestDS18B20Data();
  float ds18b20T = sensors.getTempCByIndex(0);

  // Soil Moisture
  float soilMoisture = analogRead(SM_PIN);
  float smMapped = map(soilMoisture,750,250,0,100);

  // Send Data
  if (client.connect(server,80)) {
    String postStr = apiKey;
           postStr +="&amp;field1=";
           postStr += String(dht22T);
           postStr +="&amp;field2=";
           postStr += String(dht22H);
           postStr +="&amp;field3=";
           postStr += String(ds18b20T);
           postStr +="&amp;field4=";
           postStr += String(smMapped);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);

     Serial.print("Post message: ");
     Serial.println(postStr);
  }
  client.stop();  
  
  delay(30000);
}

// Request temperature for every DS18B20 device available
void requestDS18B20Data()
{
  sensors.requestTemperatures();
  
  // print data
  int numDev = sensors.getDeviceCount();
  for (int i = 0; i < numDev; i++) {
    Serial.print("Device ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(sensors.getTempCByIndex(i)); 
  }
}

// Print device id and address for every DS18B20 device available
void printDevicesInfo()
{
  int numDev = sensors.getDeviceCount();
  DeviceAddress DS18B20[numDev];
  Serial.println("DS18B20 devices available:");
  for (int i = 0; i < numDev; i++) {
    sensors.getAddress(DS18B20[i], i);
    Serial.print("Dev ");
    Serial.print(i);
    Serial.print(" id: ");
    printAddress(DS18B20[i]);
    delay(500);
  }
}

// Function to print a device address for every DS18B20
void printAddress(DeviceAddress deviceAddress)
{
 for (uint8_t i = 0; i < 8; i++)
 {
   // zero pad the address if necessary
   if (deviceAddress[i] < 16) Serial.print("0");
   Serial.print(deviceAddress[i], HEX);
   if (i != 7) Serial.print(":");
 }
 Serial.println();
}

// Function to request DHT22 status and values
void requestDHT22Data() {
  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();

  Serial.print("DHT22,\t");
  
//  stat.total++;
  switch (chk)
  {
  case DHTLIB_OK:
//      stat.ok++;
      Serial.print("OK,\t");
      break;
  case DHTLIB_ERROR_CHECKSUM:
//      stat.crc_error++;
      Serial.print("Checksum error,\t");
      break;
  case DHTLIB_ERROR_TIMEOUT:
//      stat.time_out++;
      Serial.print("Time out error,\t");
      break;
  default:
//      stat.unknown++;
      Serial.print("Unknown error,\t");
      break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.print(DHT.temperature, 1);
  Serial.print(",\t");
  Serial.print(stop - start);
  Serial.println();
}

// Method to (re)connect to WiFi network
void wifiConnection() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

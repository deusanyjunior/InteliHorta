#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable
#include "keys.h"

// WiFi
WiFiClient client;

// DHT22
dht DHT;
#define DHT22_PIN D5

// SOIL MOISTURE
#define SM_PIN A0

// DS18B20
#define ONE_WIRE_BUS D4 // Data wire is plugged into pin 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

void setup(void)
{
  Serial.begin(9600);
  
  // WiFi
  wifiConnection(); // Setup Wifi
    
  // DHT22
  pinMode(DHT22_PIN, INPUT); // Set DHT22 as input

  // DS18B20
  sensors.begin(); // Start up sensors' library 
  pinMode(ONE_WIRE_BUS, INPUT); // Set DS18B20 as input

  // ThingSpeak
  ThingSpeak.begin(client); // Start the client
  
  delay(1000); // Wait the setup to finish
}

void loop(void)
{
  // Verify connection
  while (WiFi.status() != WL_CONNECTED) {
    wifiConnection();
  }

  // DHT22
  requestDHT22Data();
  float dht22T = DHT.temperature;
  float dht22H = DHT.humidity;
  
  // Soil Moisture
  float soilMoisture = analogRead(SM_PIN);
  float smMapped = map(soilMoisture,750,250,0,100); // Map values from sensor

  // DS18B20
  requestDS18B20Data();
  float ds18b20T = sensors.getTempCByIndex(0);

  // Set the fields with the values
  ThingSpeak.setField(1, dht22T);
  ThingSpeak.setField(2, dht22H);
  ThingSpeak.setField(3, ds18b20T);
  ThingSpeak.setField(4, smMapped);

  // Send data to ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(30000);
}

// Method to (re)connect to WiFi network
void wifiConnection() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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

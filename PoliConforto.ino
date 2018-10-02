#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS D4
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


dht DHT;
#define DHT22_PIN D3

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  pinMode(D4, INPUT);

  // Start up the library
  sensors.begin();
  delay(1000);
  
  // Print Devices
  printDevicesInfo();

  Serial.println("dht22");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)\tTime (us)");
}
 
void loop(void)
{
  Serial.println("Requesting DS18b20 temperatures..");
  sensors.requestTemperatures();
  
  printTemperatures();

  // READ DATA
  Serial.print("DHT22, \t");

  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();

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
  
  delay(1000);
}

// Print temperature for every DS18B20 device available
void printTemperatures()
{
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

// Function to print a device address
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



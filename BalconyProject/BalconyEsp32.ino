#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "DHT.h"
#define DHTPIN 25
//our sensor is DHT11 type
#define DHTTYPE DHT11
//create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);
const int moistSensor = 33;
const int relayPin    = 27;

const char *ssid     = "HAUSE";
const char *password = "kommune277";

int validMoisture = 0;
const int highestDryReading = 2047;
const int lowestWetReading = 540;
int sensorResult;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup(){
  Serial.begin(115200);
  dht.begin();
  pinMode(relayPin, OUTPUT);

  WiFi.begin(ssid, password);
  timeClient.begin();

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print ( "." );
  }
  printWifiStatus();
}

void loop() {
  timeClient.update();
  String time = timeClient.getFormattedTime();
  int hour   = time.substring(0, 2).toInt();
  int minute = time.substring(4, 6).toInt();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // moisture sensor only reads 10 bits GPIO of ESP32 reads 12 bits:
  int moisture = analogRead(moistSensor) - 2048;
  // Only accept a moisture sensor value with a difference of 10
  // to prevent the sensor to jump from dry to wet to wet to dry etc...
  if (abs(validMoisture - moisture) > 10) {
    validMoisture = moisture;
  }
  sensorResult = map(validMoisture, lowestWetReading, highestDryReading, 0, 4);

  // Turns pump on for 2 minutes if the clock shows 07:00 or 19:00 otherwise
  // turns the pump off
  if((hour == 7 || hour == 19) && (minute <= 1)) {
    turnPumpOn();
  } else {
    turnPumpOff();
  }

  Serial.println(time);
  Serial.print("minute: ");
  Serial.println(minute);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
  Serial.print("Moisture ");
  Serial.print(validMoisture);
  printMoisture();

  delay(5000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("\nSSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printMoisture() {
  String wordForMoisture;
  switch (sensorResult) {
    case 0:
      wordForMoisture = "Wet";
      break;
    case 1:
      wordForMoisture = "Damp";
      break;
    case 2:
      wordForMoisture = "Moist";
      break;
    case 3:
      wordForMoisture = "Dry";
      break;
    case 4:
      wordForMoisture = "Bone Dry";
      break;
  }
  Serial.println(wordForMoisture);
}

void turnPumpOn() {
  digitalWrite(relayPin, HIGH);
}

void turnPumpOff() {
  digitalWrite(relayPin, LOW);
}


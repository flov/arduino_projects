/*********************************************************************
  This is my implementation of my water irrigation system on my balcony.
  When the earth becomes too dry it triggers the pump which
  is in a water bucket. The pump pumps water into the pipes which waters
  the soil. Soil is measured with a Capacity Soil Moisture Sensor v1.2

*********************************************************************/

// Temperature and humidity library
#include "DHT.h"
// SD card reading functions:
#include <SPI.h>
#include <SD.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

File myFile;

// const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int pumpPin     = 7;   // <-- pin for the pump
const int moistSensor = A0;   // <-- pin for the moisture sensor
const int dhtPin      = 8;    // <-- pin for the DHT11 temp sensor
const int sdChipPin   = 10;   // <-- Adafruit has pin 10, don't use 10 as another pin
#define DHTTYPE DHT11

// Initialize DHT sensor.
DHT dht(dhtPin, DHTTYPE);

int moisture = 0;
int validMoisture = 0;
int sensorResult;        // scaled sensor data [0..3] = [wet, damp, moist, dry]
unsigned long msPassed;
unsigned long msTurnOnPump;
DateTime now;

const int highestDryReading = 808;
const int lowestWetReading = 400;

void setup() {
  Serial.begin(9200);
  dht.begin();
  initializeSDCardReader();
  pinMode(pumpPin, OUTPUT);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
}

void loop() {
  moisture = analogRead(moistSensor);  // read the input pin
  myFile = SD.open("hause.txt", FILE_WRITE);

  // Only accept a moisture sensor value with a difference of 10
  // to prevent the sensor to jump from dry to wet to wet to dry etc...
  if (abs(validMoisture - moisture) > 10) {
    validMoisture = moisture;
  }

  // scale analog input to a smaller range for wet to dry
  sensorResult = map(validMoisture, lowestWetReading, highestDryReading, 0, 4);
  printDateTime();
  printTemperature();
  printMoisture();

  // Turns pump on for 5 minutes if the clock shows 07:00 or 19:00 otherwise
  // turns the pump off
  now = rtc.now();
  if ((now.hour() == 7 && now.minute() <= 5) || (now.hour() == 19 && now.minute() <= 0)) {
    turnPumpOn();
  } else {
    turnPumpOff();
  }

  delay(30000);
  if (myFile) {
    myFile.print("\n");
    myFile.close();
  }
}

void turnPumpOn() {
  digitalWrite(pumpPin, HIGH);
  if (myFile) {
    myFile.print("1");
  }
}

void turnPumpOff() {
  if (myFile) {
    myFile.print("0");
  }
  digitalWrite(pumpPin, LOW);
}

void printTemperature() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  if(myFile) {
    myFile.print(temp);
    myFile.print(",");
    myFile.print(humidity);
    myFile.print(",");
  }
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

  if(myFile) {
    myFile.print(validMoisture);
    myFile.print(",");
    myFile.print(sensorResult);
    myFile.print(",");
    myFile.print(wordForMoisture);
    myFile.print(",");
  }
}

void initializeSDCardReader()
{
  Serial.print("Initializing SD card...");
  if (!SD.begin(sdChipPin)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");
  }
}

void printDateTime() {
  now = rtc.now();
  String dataString = "";
  int runningSince = millis() / 1000;

  dataString += now.day();
  dataString += '/';
  dataString += now.month();
  dataString += '/';
  dataString += now.year();
  dataString += ' ';
  dataString += now.hour();
  dataString += ':';
  dataString += now.minute();
  dataString += ':';
  dataString += now.second();

  if (myFile) {
    myFile.print(dataString);
    myFile.print(",");
    myFile.print(runningSince);
    myFile.print(",");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  Serial.print(dataString);
  Serial.print(" running since: ");
  Serial.print(runningSince);
  Serial.println("s");
}

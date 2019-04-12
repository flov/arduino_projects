/*********************************************************************
  This is my implementation of my water irrigation system on my balcony.
  When the earth becomes too dry it triggers the pump which
  is in a water bucket. The pump pumps water into the pipes which waters
  the soil. Soil is measured with a Capacity Soil Moisture Sensor v1.2

*********************************************************************/

#include <LiquidCrystal.h>
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

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int pumpPin     = 13;   // <-- pin for the pump
const int moistSensor = A0;   // <-- pin for the moisture sensor
const int dhtPin      = 8;    // <-- pin for the DHT11 temp sensor
const int sdChipPin   = 10;   // <-- Adafruit has pin 10, don't use 10 as another pin
#define DHTTYPE DHT11

// initialize the LCD with the numbers of the interface pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Initialize DHT sensor.
DHT dht(dhtPin, DHTTYPE);

int moisture = 0;
int validMoisture = 0;
int sensorResult;        // scaled sensor data [0..3] = [wet, damp, moist, dry]
unsigned long msPassed;
unsigned long msTurnOnPump;

const int highestDryReading = 808;
const int lowestWetReading = 400;

void setup() {
  Serial.begin(9200);
  lcd.begin(16, 2);
  dht.begin();
  initializeSDCardReader();
  // create a new character
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
  lcd.clear();

  printDateTime();
  printTemperature();
  printMoisture();

  lcd.setCursor(0,0);
  lcd.print("Soil: ");
  switch (sensorResult) {
    case 0:
      lcd.print("Wet");
      turnPumpOn();
      break;
    case 1:
      lcd.print("Damp");
      turnPumpOn();
      break;
    case 2:
      lcd.print("Moist");
      turnPumpOn();
      break;
    case 3:
      lcd.print("Dry");
      turnPumpOff();
      break;
    case 4:
      lcd.print("Bone Dry");
      turnPumpOff();
      break;
  }

  countDown(10000);
  Serial.println();
  myFile.print("\n");
  myFile.close();
}

void turnPumpOn() {
  digitalWrite(pumpPin, HIGH);
  if (myFile) {
    myFile.print("1");
    Serial.println("Logging Pump On");
  }
  lcd.setCursor(8,1);
  lcd.print("PumpOn");
}

void turnPumpOff() {
  if (myFile) {
    myFile.print("0");
    Serial.println("Logging Pump Off");
  }
  digitalWrite(pumpPin, LOW);
}

void printTemperature() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  if(myFile) {
    Serial.println("Saving temp and humiditiy");
    myFile.print(temp);
    myFile.print(",");
    myFile.print(humidity);
    myFile.print(",");
  }
  lcd.setCursor(0,1);
  lcd.print(round(temp));
  lcd.print("C ");
  lcd.print(round(humidity));
  lcd.print("%");
}

void printMoisture() {
  lcd.setCursor(12,0);
  lcd.print(validMoisture);
  Serial.print("Moisture: ");
  Serial.println(validMoisture);
  if(myFile) {
    Serial.println("Saving moisture");
    myFile.print(validMoisture);
    myFile.print(",");
  }
}

void initializeSDCardReader()
{
  Serial.print("Initializing SD card...");
  if (!SD.begin(sdChipPin)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

// Counts down the time until the next reading and displays it on LCD
void countDown(int msToCount) {
  int counter;
  unsigned long timer = millis();
  msPassed = 0;
  while(msPassed <= msToCount){
    msPassed = millis() - timer;
    counter = (msToCount / 1000) - (msPassed / 1000);
    lcd.setCursor(14,1);
    if (counter < 10) {
      lcd.print(" ");
      lcd.print(counter);
    } else {
      lcd.print(counter);
    }
    delay(1000);
  }
}

void printDateTime() {
  DateTime now = rtc.now();
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
    Serial.println("Saving timestamp");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  Serial.println(dataString);
  Serial.println(runningSince);
}

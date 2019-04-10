/*********************************************************************
  This is my implementation of my water irrigation system on my balcony.
  When the earth becomes too dry it triggers the pump which
  is in a water bucket. The pump pumps water into the pipes which waters
  the soil. Soil is measured with a Capacity Soil Moisture Sensor v1.2

*********************************************************************/

#include <LiquidCrystal.h>
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
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

// make some custom characters:
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

void setup() {
  Serial.begin(9200);
  lcd.begin(16, 2);
  dht.begin();
  initializeSDCardReader();
  // create a new character
  lcd.createChar(0, heart);
  pinMode(pumpPin, OUTPUT);
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

  printTimestamp();
  printTemperature();
  printMoisture();

  lcd.setCursor(0,0);
  lcd.print("Soil: ");
  switch (sensorResult) {
    case 0:
      lcd.print("Wet");
      turnPumpOff();
      break;
    case 1:
      lcd.print("Damp");
      turnPumpOff();
      break;
    case 2:
      lcd.print("Moist");
      turnPumpOff();
      break;
    case 3:
      lcd.print("Dry");
      turnPumpOn();
      break;
    case 4:
      lcd.print("Bone Dry");
      turnPumpOn();
      break;
  }

  countDown(10000);
  Serial.println();
  myFile.print("\n");
  myFile.close();
}

void turnPumpOn() {
  digitalWrite(pumpPin, HIGH);
  lcd.setCursor(8,1);
  lcd.print("PumpOn");
}

void turnPumpOff() {
  digitalWrite(pumpPin, LOW);
  // printILoveHause();
}

void printTimestamp() {
  unsigned long ms = millis();
  if (myFile) {
    myFile.print(ms);
    myFile.print(",");
    Serial.println("Saving timestamp");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
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

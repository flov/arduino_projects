/*********************************************************************
  This is my implementation of automatically hosing the plants on 
  the balcony when the earth becomes too dry.
  Soil is measured with a Capacity Soil Moisture Sensor v1.2

*********************************************************************/

#include <LiquidCrystal.h>
#include "DHT.h"

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int pumpPin     = 13;   // <-- change this to the pin for the pump
const int moistSensor = A0;   // <-- change this to the pin for the moisture sensor
const int dhtPin      = 8;    // <-- change this to the pin for the DHT11 temp sensor
#define DHTTYPE DHT11

// initialize the LCD with the numbers of the interface pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Initialize DHT sensor.
DHT dht(dhtPin, DHTTYPE);

int moisture = 0;
int validMoisture = 0;
int sensorResult;        // scaled sensor data [0..3] = [wet, damp, moist, dry]
int countdown;
unsigned long msPassed;
unsigned long msTurnOnPump;
unsigned long timer;

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
  // create a new character
  lcd.createChar(0, heart);
  pinMode(pumpPin, OUTPUT);
}

void loop() {
  moisture = analogRead(moistSensor);  // read the input pin

  // Only accept a moisture sensor value with a difference of 10
  // to prevent the sensor to jump from dry to wet to wet to dry etc...
  if (abs(validMoisture - moisture) > 10) {
    validMoisture = moisture;
  }

  // scale analog input to a smaller range for wet to dry
  sensorResult = map(validMoisture, lowestWetReading, highestDryReading, 0, 4);
  Serial.println(validMoisture);
  lcd.clear();
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

  delay(1000);
}

void turnPumpOn() {
  digitalWrite(pumpPin, HIGH);
  lcd.setCursor(8,1);
  lcd.print("PumpOn");
  printTemperature();
  timer = millis();
  msPassed = 0;
  // Turn pump on for msTurnOnPump ms
  msTurnOnPump = 5000;
  while(msPassed <= msTurnOnPump){
    msPassed = millis() - timer;
    countdown = (msTurnOnPump / 1000) - (msPassed / 1000);
    lcd.setCursor(14,1);
    if (countdown < 10) {
      lcd.print(" ");
      lcd.print(countdown);
    } else {
      lcd.print(countdown);
    }
    printMoistValue();
    delay(1000);
  }
}

void turnPumpOff() {
  digitalWrite(pumpPin, LOW);
  printTemperature();
  // printILoveHause();
  printMoistValue();
  delay(5000);
}

void printTemperature() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  lcd.setCursor(0,1);
  lcd.print(round(temp));
  lcd.print("C ");
  lcd.print(round(humidity));
  lcd.print("%");
}

void printILoveHause() {
  lcd.setCursor(0,1);
  lcd.print("   I ");
  lcd.write(byte(0)); // when calling lcd.write() '0' must be cast as a byte
  lcd.print(" Hause!");
}

void printMoistValue() {
  lcd.setCursor(12,0);
  lcd.print(validMoisture);
}

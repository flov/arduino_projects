/*********************************************************************
  This is my implementation of automatically hosing the plants on 
  the balcony when the earth becomes too dry.
  Soil is measured with a Capacity Soil Moisture Sensor v1.2

*********************************************************************/

#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int pumpPin  = 13;       // <-- change this to the pin for the pump
int moistSensor = A0;    // <-- change this to the pin for the moisture sensor

int moisture = 0;
int validMoisture = 0;
int sensorResult;        // scaled sensor data [0..3] = [wet, damp, moist, dry]
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
  // create a new character
  lcd.createChar(0, heart);
  pinMode(pumpPin, OUTPUT);
}

void loop() {
  moisture = analogRead(moistSensor);  // read the input pin

  // we want to go in steps of 10 to prevent sensor to jump from
  // dry to wet to wet to dry etc...
  if (abs(validMoisture - moisture) > 10) {
    validMoisture = moisture;
  }

  // scale analog input to a smaller range for wet to dry
  sensorResult = map(validMoisture, lowestWetReading, highestDryReading, 0, 4);
  Serial.print ("Scaled Sensor Reading 0-4: ");
  Serial.println (sensorResult);
  Serial.println ();

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
  lcd.setCursor(0,1);
  // Print a message to the lcd.
  lcd.print("Watering...");
}

void turnPumpOff() {
  digitalWrite(pumpPin, LOW);
  lcd.setCursor(0,1);
  // Print a message to the lcd.
  lcd.print("   I ");
  lcd.write(byte(0)); // when calling lcd.write() '0' must be cast as a byte
  lcd.print(" Hause!");
}

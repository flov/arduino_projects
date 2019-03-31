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

void setup() {
  Serial.begin(9200);
  lcd.begin(16, 2);
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
      break;
    case 1:
      lcd.print("Damp");
      break;
    case 2:
      lcd.print("Moist");
      break;
    case 3:
      lcd.print("Dry");
      break;
    case 4:
      lcd.print("Bone Dry");
      break;
  }

  lcd.setCursor(0,1);
  lcd.print(I);
  delay(1000);
}


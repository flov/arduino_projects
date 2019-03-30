int pumpPin  = 13;
int moistPin = A0;
int moisture = 0;
int validMoisture = 0;

void setup() {
  Serial.begin(9200);
  pinMode(pumpPin, OUTPUT);
}

void loop() {
  moisture = analogRead(moistPin);  // read the input pin

  // we want to go in steps of 10 to prevent sensor to jump from
  // dry to wet to wet to dry etc...
  if (abs(validMoisture - moisture) > 10) {
    validMoisture = moisture;
  }

  Serial.println(validMoisture);
  if (validMoisture > 700){
    Serial.println("dry soil");
    digitalWrite(pumpPin, HIGH);
    delay(10000);
  } else {
    Serial.println("moist soil");
    digitalWrite(pumpPin, LOW);
    delay(10000);
  }
}

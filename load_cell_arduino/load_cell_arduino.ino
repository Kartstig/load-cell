/*
  Load Cell Circuit
  (http://www.amazon.com/Electronic-Balance-Weighing-Sensor-0-5Kg/dp/B006W2IDUO/ref=sr_1_3?ie=UTF8&qid=1420998526&sr=8-3&keywords=load+cell)
  0-5kg Sensor
  1.0946mV/V
*/


const int inputPin = A0;

void setup() {
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
}

void loop() {
  Serial.print("Sensor ADC: ");
  Serial.print(getADC(inputPin));
  Serial.print("\t");
  Serial.print("Sensor Voltage: ");
  Serial.print(getVoltage(inputPin));
  Serial.print("\n");
  delay(100);
}

int getADC(int pin) {
  return analogRead(pin);
}

float getVoltage(int pin) {
  return getADC(pin)*5/1024;
}

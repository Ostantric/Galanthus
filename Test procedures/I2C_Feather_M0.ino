#include <Wire.h>
int led = 13;
void setup() {
   pinMode(led, OUTPUT); 
  Wire.begin(0x19); // join i2c bus (address optional for master)
  
}

byte x = 0;

void loop() {
   digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  Wire.beginTransmission(0x09); // transmit to device #8
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  x++;
  delay(250);
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(250);
}

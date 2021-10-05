#include <Arduino.h>
#include <ssd1306.h>
SSD1306   display(0x3c, 4, 5, GEOMETRY_128_32);

int light = 0;
const int pulseA = 13;
const int pulseB = 12;
const int pushSW = 2;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

IRAM_ATTR void handleRotary() {
  int MSB = digitalRead(pulseA);
  int LSB = digitalRead(pulseB);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded;
  if(encoderValue > 255){
    encoderValue = 255;
  }
  else if(encoderValue < 0){
    encoderValue = 0;
  }
}

IRAM_ATTR void buttonClicked() {
  Serial.println("pushed");
}

void setup(){
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(15,OUTPUT);
  pinMode(pushSW, INPUT_PULLUP);
  pinMode(pulseA, INPUT_PULLUP);
  pinMode(pulseB, INPUT_PULLUP);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  attachInterrupt(pulseA, handleRotary, CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 10, "hello World");
  display.display();
}

void relay(){
  if (light / 4 < encoderValue) {
    digitalWrite(15,HIGH);
  }
  else
    digitalWrite(15,LOW);
}

void display1() {
  light = analogRead(A0);
  Serial.print("Lightness : "); Serial.println(light);
  Serial.println("Rotary Value : "); Serial.print(encoderValue); 

  display.resetDisplay();
  char buf[15];
  sprintf(buf, "%d, %d ", light / 4, encoderValue);
  display.drawString(10,10, buf); 

  display.display();
}

void loop() {
  relay();
  display1();
  delay(1000);
}
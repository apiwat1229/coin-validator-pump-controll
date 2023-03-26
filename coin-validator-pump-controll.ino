#include <Arduino.h>
#include <TM1637Display.h>
#include <SimpleRotary.h>
#include <EEPROM.h>
#include "OneButton.h"

#define CLK 7
#define DIO 3

TM1637Display display(CLK, DIO);
// Pin Up, Pin Down, Button Pin
SimpleRotary rotary(4, 5, 6);

OneButton button1(A3, true);
OneButton button2(A4, true);
OneButton button3(A5, true);

byte rotate, rotary_button, time_counter, value;
byte current_Time;

int led_State = LOW;

unsigned long previousMillis_1 = 0, previousMillis_2 = 0;

const byte led1_Pin = A0;
const byte led2_Pin = A1;
const byte led3_Pin = A2;

const byte relay1_Pin = 8;
const byte relay2_Pin = 9;
const byte relay3_Pin = 10;

int button1_State = 0;
int button2_State = 0;
int button3_State = 0;

int sensor = 2;
int stat = 0;

int i = 0;
int c = 0;

void setup() {
  Serial.begin(9600);

  rotary.setTrigger(HIGH);
  rotary.setDebounceDelay(2);
  rotary.setErrorDelay(200);

  display.setBrightness(15);

  value = EEPROM.read(0);
  current_Time = value;

  pinMode(led1_Pin, OUTPUT);
  pinMode(led2_Pin, OUTPUT);
  pinMode(led3_Pin, OUTPUT);

  display.showNumberDec(current_Time);
  button1.attachClick(click1);
  button2.attachClick(click2);
  button3.attachClick(click3);

  pinMode(sensor, INPUT);
  attachInterrupt(0, doCounter, FALLING);
}

boolean isCounter = false;
int count = 0;

void loop() {
  while (count == 0) {
    Coin();
    display.showNumberDec(00);
    
  }

  button1.tick();
  button2.tick();
  button3.tick();

  get_rotaryStat();
  blink_led();

  if (button1_State == 1) {
    value = EEPROM.read(0);
    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      current_Time--;
      digitalWrite(led1_Pin, HIGH);
      digitalWrite(led2_Pin, LOW);
      digitalWrite(led3_Pin, LOW);

      digitalWrite(relay1_Pin, HIGH);
      digitalWrite(relay2_Pin, LOW);
      digitalWrite(relay3_Pin, LOW);
      delay(1000);
    }
    current_Time = value;
    button1_State = 0;
    display.showNumberDec(current_Time);
    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;

  } else if (button2_State == 2) {
    value = EEPROM.read(0);
    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      current_Time--;
      digitalWrite(led1_Pin, LOW);
      digitalWrite(led2_Pin, HIGH);
      digitalWrite(led3_Pin, LOW);

      digitalWrite(relay1_Pin, LOW);
      digitalWrite(relay2_Pin, HIGH);
      digitalWrite(relay3_Pin, LOW);

      delay(1000);
    }
    current_Time = value;
    button2_State = 0;
    display.showNumberDec(current_Time);
    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;

  } else if (button3_State == 3) {
    value = EEPROM.read(0);
    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      current_Time--;
      digitalWrite(led1_Pin, LOW);
      digitalWrite(led2_Pin, LOW);
      digitalWrite(led3_Pin, HIGH);

      digitalWrite(relay1_Pin, LOW);
      digitalWrite(relay2_Pin, LOW);
      digitalWrite(relay3_Pin, HIGH);
      delay(1000);
    }
    current_Time = value;
    button2_State = 0;
    display.showNumberDec(current_Time);
    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;
  }
}

void Coin() {
  if (isCounter == true) {
    isCounter = false;
    delay(500);
  }
  if (count > 1) {
      i = i + 1;
      Serial.print("จำนวนเหรียญ : ");
      Serial.println(i);
      c = i * 10;
      Serial.print("จำนวนเงิน : ");
      Serial.print(c);
      Serial.println(" บาท");
      delay(1000);
      count = 0;
    }
}


void doCounter() {
  isCounter = true;
  count++;
}

void click1() {
  button1_State = 1;
  Serial.println("Button 1 stat = " + String(button1_State));
}  // click1
void click2() {
  button2_State = 2;
  Serial.println("Button 2 stat = " + String(button2_State));
}  // click1
void click3() {
  button3_State = 3;
  Serial.println("Button 3 stat = " + String(button3_State));
}  // click1

void blink_led() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_1 >= 500) {
    previousMillis_1 = currentMillis;

    if (led_State == LOW) {
      led_State = HIGH;
    } else {
      led_State = LOW;
    }
    digitalWrite(led1_Pin, led_State);
    digitalWrite(led2_Pin, led_State);
    digitalWrite(led3_Pin, led_State);
  }
}

void get_rotaryStat() {
  rotate = rotary.rotate();
  rotary_button = rotary.pushType(1000);

  if (rotate == 1) {
    time_counter++;
    Serial.println(time_counter);
    display.showNumberDec(time_counter);
  }
  if (rotate == 2) {
    time_counter--;
    Serial.println(time_counter);
    display.showNumberDec(time_counter);
  }
  if (rotary_button == 2) {
    current_Time = time_counter;
    Serial.println(time_counter);
    display.showNumberDec(time_counter);
    EEPROM.write(0, time_counter);
    display.setSegments("----");
    delay(2000);
    display.clear();
    display.showNumberDec(current_Time);
  }
}
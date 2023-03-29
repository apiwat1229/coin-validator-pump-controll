#include <TM1637Display.h>
#include <SimpleRotary.h>
#include <EEPROM.h>

const int CLK = 7;
const int DIO = 3;

const int coinSensorPin = 2;
const int button1 = A3;
const int button2 = A4;
const int button3 = A5;
const int buzzerPin = 13;

TM1637Display display(CLK, DIO);
SimpleRotary rotary(4, 5, 6);

int i = 0;
int one_coin = 0;
int five_coin = 0;
int ten_coin = 0;

int total_ten_coin = 0;
int total_five_coin = 0;
int total_one_coin = 0;
int total_all_coin = 0;

byte rotate, rotary_button, time_counter, value;
byte current_Time;

const byte led1_Pin = A0;
const byte led2_Pin = A1;
const byte led3_Pin = A2;

const byte relay1_Pin = 8;
const byte relay2_Pin = 9;
const byte relay3_Pin = 10;

int button1_State = 0;
int button2_State = 0;
int button3_State = 0;
int led_State = LOW;

unsigned long previousMillis_1 = 0, previousMillis_2 = 0;

boolean isCounter = false;
int count = 0;

void doCounter() {
  isCounter = true;
  count++;
}

void coin_Check_Status() {
  if (isCounter == true) {
    isCounter = false;
    //Serial.println(count);
    delay(500);
  }
  if (i != 1000 && count != 0) {
    i = i + 1;
  }
  if (i == 1000) {
    i = 0;
    if (count == 10) {
      ten_coin = ten_coin + 1;
      total_ten_coin = ten_coin * 10;  // มูลค่าเหรียญ
      buzzer(ten_coin);
    }
    if (count == 5) {
      five_coin = five_coin + 1;
      total_five_coin = five_coin * 5;
    }
    if (count == 1) {
      one_coin = one_coin + 1;
      total_one_coin = one_coin * 1;
    }

    total_all_coin = total_one_coin + total_five_coin + total_ten_coin;

    current_Time = value * ten_coin;  ////////.  <<<<<< เพิ่มเวลาตามทำจำนวนเงิน ทดสอยเฉพาะปุ่ม 1

    Serial.print("    รวมชนิดเหรียญ 10 บาท: ");
    Serial.print(ten_coin);
    Serial.println(" เหรียญ");
    Serial.print("    รวมชนิดเหรียญ  5 บาท: ");
    Serial.print(five_coin);
    Serial.println(" เหรียญ");
    Serial.print("    รวมชนิดเหรียญ  1 บาท: ");
    Serial.print(one_coin);
    Serial.println(" เหรียญ");
    Serial.println();

    Serial.print("    ยอดเงินรวมทั้งหมด: ");
    Serial.print(total_all_coin);
    Serial.println(" บาท");
    Serial.println();

    // count = 0;
  }
}

void blink_led() {
  if (millis() - previousMillis_1 >= 250) {
    previousMillis_1 = millis();
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
    display.showNumberDec(current_Time);
    buzzer(2);
  }
}

void get_ButtonStat() {
  if (millis() - previousMillis_2 >= 100) {
    previousMillis_2 = millis();
    button1_State = digitalRead(button1);
    button2_State = digitalRead(button2);
    button3_State = digitalRead(button3);
    // Serial.println("Button 1 : " + String(button1_State) + " Button 2 : " + String(button2_State) + " Button 3 : " + String(button3_State));
  }
}

void buzzer(int x) {
  for (int a = 0; a <= x; a++) {
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  value = EEPROM.read(0);
  current_Time = value;
  rotary.setTrigger(HIGH);
  rotary.setDebounceDelay(2);
  rotary.setErrorDelay(200);

  display.setBrightness(15);

  pinMode(led1_Pin, OUTPUT);
  pinMode(led2_Pin, OUTPUT);
  pinMode(led3_Pin, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(led1_Pin, HIGH);
  digitalWrite(led2_Pin, HIGH);
  digitalWrite(led3_Pin, HIGH);

  digitalWrite(buzzerPin, HIGH);

  pinMode(coinSensorPin, INPUT);
  attachInterrupt(0, doCounter, FALLING);
  display.clear();
}

void loop() {
  while (count == 0) {
    coin_Check_Status();
    get_ButtonStat();
    display.showNumberDec(00);
    Serial.println("Waiting Coin");
    delay(50);
  }
  // Serial.println(" !!! Systems Ready !!! ");
 
  blink_led();
  get_ButtonStat();
  get_rotaryStat();

  if (button1_State == 0) {
    value = EEPROM.read(0);
    current_Time = value;
    buzzer(3);

    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      Serial.println(current_Time);
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

    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;
  } else if (button2_State == 0) {
    value = EEPROM.read(0);
    current_Time = value;
    // value = value * ten_coin;  ////////.  <<<<<< เพิ่มเวลาตามทำจำนวนเงิน ทดสอยเฉพาะปุ่ม 1
    buzzer(3);

    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      Serial.println(current_Time);
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

    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;
  } else if (button3_State == 0) {
    value = EEPROM.read(0);
    current_Time = value;
    // value = value * ten_coin;  ////////.  <<<<<< เพิ่มเวลาตามทำจำนวนเงิน ทดสอยเฉพาะปุ่ม 1
    buzzer(3);
    display.clear();
    for (int i = value; i >= 0; i--) {
      display.showNumberDec(current_Time);
      Serial.println(current_Time);
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

    digitalWrite(led1_Pin, LOW);
    digitalWrite(led2_Pin, LOW);
    digitalWrite(led3_Pin, LOW);

    digitalWrite(relay1_Pin, LOW);
    digitalWrite(relay2_Pin, LOW);
    digitalWrite(relay3_Pin, LOW);
    count = 0;
  }
}

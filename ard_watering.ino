#include <Wire.h> 
#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//--------------------------------------------

const int ap_sensor1 = A0;
const int ap_sensor2 = A1;
const int ap_pot = A3;
const int dp_pump1 = 2;
const int dp_pump2 = 7;
const int dpButton1 = 4;
const int dpButton2 = 5;
const unsigned long repeatLimit = (long)1000 * 60 * 30;
const unsigned long screenActiveTime = (long)1000 * 60;
const long pumpRunTime = (long)1000 * 10;
const int loopDelay = 2000;
const int row = 10;
const int col = 6;

int timesPumped1 = 0;
int timesPumped2 = 0;
unsigned long lastRun1 = 0;
unsigned long lastRun2 = 0;
unsigned long lastScreen = 0;
int value1UpperLimit = 600;
int value2UpperLimit = 600;
int selectedSensor = 1;
char formatBuffer[20];

//--------------------------------------------

void setup() {
  pinMode(dp_pump1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(dpButton1, INPUT);
  pinMode(dpButton2, INPUT);

  // High = relay blocks, Low = relay opens
  digitalWrite(dp_pump1, HIGH);

  Serial.begin(9600);
  Serial.println("Hi!");

  EEPROM.get(0, value1UpperLimit);
  EEPROM.get(2, value2UpperLimit);

  u8g2.begin();  
  
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);


  // Damit nicht im nullten Tick gearbeitet wird.
  delay(200);
}

//--------------------------------------------

void loop() {

  // put your main code here, to run repeatedly:
  int val1 = analogRead(ap_sensor1);
  int val2 = analogRead(ap_sensor2);
  int potVal = map(analogRead(ap_pot),0, 1023, 200, 600);
  bool button1Pressed = digitalRead(dpButton1);
  bool button2Pressed = digitalRead(dpButton2);
  Serial.print("Gelesener Wert:");
  Serial.println(val1);
  Serial.println(val2);
  Serial.println(selectedSensor);
  Serial.println(button1Pressed);
  Serial.println(button2Pressed);
  
  if (button2Pressed) {

    lastScreen = millis();
    
    if (selectedSensor == 1){
      selectedSensor = 2;
    } else {
      selectedSensor = 1;
    }
  }

  if(button1Pressed) {
    if (selectedSensor == 1){
      value1UpperLimit = potVal;
    } else {
      value2UpperLimit = potVal;
    }
    EEPROM.put(0, value1UpperLimit);
    EEPROM.put(2, value2UpperLimit);
  }

  // ------------------------ LED --------
  bool screenActive = (millis() - lastScreen) <= screenActiveTime;
  
  if (screenActive) {
    u8g2.firstPage();  
    do {
      // Selected Sensor & Potval
      if (selectedSensor == 1){
        u8g2.drawStr(col*0, row*0, ">");
        u8g2.drawStr(col*10, row*0, format(potVal));
      } else {
        u8g2.drawStr(col*0, row*1, ">");
        u8g2.drawStr(col*10, row*1, format(potVal));
      }
    
      // Values & Limits
      u8g2.drawStr(col*1, row*0, format(val1));
      u8g2.drawStr(col*5, row*0, format(value1UpperLimit));
      u8g2.drawStr(col*15, row*0, format(timesPumped1));
    
      u8g2.drawStr(col*1, row*1, format(val2));
      u8g2.drawStr(col*5, row*1, format(value2UpperLimit));
      u8g2.drawStr(col*15, row*0, format(timesPumped1));
    } while( u8g2.nextPage() );
  } else {
    u8g2.clearDisplay();
  }

  calcPump(val1, value1UpperLimit, lastRun1, timesPumped1, dp_pump1);
  //calcPump(val2, value2UpperLimit, lastRun2, timesPumped2, dp_pump2);

  delay (loopDelay);
}

void calcPump(int val, int limit, unsigned long &lastRun, int &timesPumped, int pin){
  if (val > limit) {
    if (((lastRun == 0) or ((millis() - lastRun) > repeatLimit))) {
      timesPumped++;
      
      lastRun = millis();
      
      Serial.println("On");

      digitalWrite(pin, LOW);
      delay(pumpRunTime);
      digitalWrite(pin, HIGH);
      
      Serial.println("Off");
    }
  }
}

char * format(int val){
  itoa(val, formatBuffer, 10);
  return formatBuffer;
}

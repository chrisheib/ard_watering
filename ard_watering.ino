#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 

//--------------------------------------------

const int ap_sensor1 = A0;
const int ap_sensor2 = A1;
const int ap_pot = A3;
const int dp_pump1 = 2;
const int dp_pump2 = 7;
const int dpButton1 = 4;
const int dpButton2 = 5;
const unsigned long repeatLimit = (long)1000 * 60 * 30;
const long pumpRunTime = (long)1000 * 10;
const int loopDelay = 2000;

int timesPumped1 = 0;
int timesPumped2 = 0;
unsigned long lastRun1 = 0;
unsigned long lastRun2 = 0;
int value1UpperLimit = 360;
int value2UpperLimit = 360;
int selectedSensor = 1;

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
  //Serial.print(repeatLimit);

  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.noBacklight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 

  

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
  
  if (button2Pressed) {
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
  }

  // ------------------------ LCD --------

  lcd.clear();

  lcd.setCursor(8,selectedSensor);
  lcd.print("x");
  
  lcd.setCursor(0, 0);
  lcd.print(val1);
  lcd.setCursor(1, 0);
  lcd.print(val2);

  lcd.setCursor(12, 1);
  lcd.print(potVal); 

  lcd.setCursor(4, 0);
  lcd.print(value1UpperLimit);
  lcd.setCursor(4, 1);
  lcd.print(value2UpperLimit); 
  
  lcd.setCursor(10, 0);
  lcd.print(timesPumped1); 
  lcd.setCursor(10, 1);
  lcd.print(timesPumped2); 

  calcPump(val1, value1UpperLimit, lastRun1, timesPumped1, dp_pump1);
  calcPump(val2, value2UpperLimit, lastRun2, timesPumped2, dp_pump2);

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

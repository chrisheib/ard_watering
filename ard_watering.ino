#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 

//--------------------------------------------

const int ap_sensor1 = A0;
const int ap_pot = A3;
const int dp_pump1 = 2;
const int dpButton = 4;
const unsigned long repeatLimit = (long)1000 * 60 * 30;
const long pumpRunTime = (long)1000 * 10;
const int loopDelay = 2000;

int val = 0;
int timesPumped = 0;
unsigned long lastRun = 0;
int potVal = 0;
int valueUpperLimit = 360;
bool buttonPressed = false;

//--------------------------------------------

void setup() {
  pinMode(dp_pump1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(dpButton, INPUT);

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
  val = analogRead(ap_sensor1);
  potVal = map(analogRead(ap_pot),0, 1023, 200, 600);
  buttonPressed = digitalRead(dpButton);
  Serial.print("Gelesener Wert:");
  Serial.println(val);

  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(val); 
  lcd.setCursor(0, 1);
  lcd.print(potVal); 
  
  lcd.setCursor(15, 0);
  lcd.print(timesPumped); 

  if(buttonPressed) {
    lcd.setCursor(5, 1);
    lcd.print("pressed!");
    valueUpperLimit = potVal;
  }

  lcd.setCursor(5, 0);
  lcd.print(valueUpperLimit); 

  if (val > valueUpperLimit) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (((lastRun == 0) or ((millis() - lastRun) > repeatLimit))) {
      timesPumped++;
      
      lastRun = millis();
      
      Serial.println("On");

      digitalWrite(dp_pump1, LOW);
      delay(pumpRunTime);
      digitalWrite(dp_pump1, HIGH);
      
      Serial.println("Off");
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay (loopDelay);
}

#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 

//--------------------------------------------

const int ap_sensor1 = A0;
const int ap_pot = A3;
const int dp_pump1 = 2;
const unsigned long repeatLimit = (long)1000 * 60 * 30;
const int valueUpperLimit = 340;
const long pumpRunTime = (long)1000 * 10;
const int loopDelay = 250;

int val = 0;
int timesPumped = 0;
unsigned long lastRun = 0;
bool pumpRunning = false;
int potVal = 0;

//--------------------------------------------

void setup() {
  pinMode(dp_pump1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // High = relay blocks, Low = relay opens
  digitalWrite(dp_pump1, HIGH);

  Serial.begin(9600);
  Serial.println("Hi!");
  //Serial.print(repeatLimit);

  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergru ndbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 

  

  // Damit nicht im nullten Tick gearbeitet wird.
  delay(200);
}

//--------------------------------------------

void loop() {
  
  // put your main code here, to run repeatedly:
  val = analogRead(ap_sensor1);
  potVal = analogRead(ap_pot);
  Serial.print("Gelesener Wert:");
  Serial.println(val);

  lcd.clear();
  
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print(val); 
  lcd.setCursor(0, 1);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print(potVal); 
  
  lcd.setCursor(4, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print(timesPumped); 

  if (val > valueUpperLimit) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (!pumpRunning and ((lastRun == 0) or ((millis() - lastRun) > repeatLimit))) {
      pumpRunning = true;
      digitalWrite(dp_pump1, LOW);
      timesPumped++;
      // +1, sonst wird der erste Tick bei Millis = 0 ausgeführt und es folgt direkt ein zweiter
      //lastRun = max(millis(), 1);
      lastRun = millis();
      Serial.println("On");
      //Serial.println(lastRun);
      //Serial.println(millis());
      //Serial.println(millis() - lastRun);

    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (pumpRunning and (millis() - lastRun > pumpRunTime)) {
    digitalWrite(dp_pump1, HIGH);
    pumpRunning = false;
    Serial.println("Off");
  }

  delay (loopDelay);
}

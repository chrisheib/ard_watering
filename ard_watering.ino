const int ap_sensor1 = A0;
const int dp_pump1 = 2;
const unsigned long repeatLimit = (long)1000 * 60 * 30;
const int valueUpperLimit = 400;
const int pumpRunTime = (long)1000 * 10;
const int loopDelay = 250;
 
int val = 0;
unsigned long lastRun = 0;
bool pumpRunning = false;

//--------------------------------------------

void setup() {
  pinMode(dp_pump1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // High = relais blocks, Low = relais opens
  digitalWrite(dp_pump1, LOW);

  Serial.begin(9600);
  Serial.println("Hi!");
}

//--------------------------------------------

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(ap_sensor1);
  Serial.print("Gelesener Wert:");
  Serial.println(val);

  if (val > valueUpperLimit){
    digitalWrite(LED_BUILTIN, HIGH);
    if (!pumpRunning and ((lastRun == 0) or (millis() - lastRun > repeatLimit))) {
      pumpRunning = true;
      digitalWrite(dp_pump1, LOW);
      lastRun = millis();
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  if (pumpRunning and (millis() - lastRun > pumpRunTime)) {
    digitalWrite(dp_pump1, HIGH);
    pumpRunning = false;
  }

  delay (loopDelay);
}

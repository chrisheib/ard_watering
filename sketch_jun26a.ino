int ap_sensor1 = A0;
int dp_pump1 = 2;
int val = 0;
unsigned long startTime = 0;
bool pumpRunning = false;
void setup() {
  pinMode(dp_pump1, OUTPUT);
  digitalWrite(dp_pump1, LOW);
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hi!");
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(ap_sensor1);
  Serial.println(val);

  if (!pumpRunning and val > 400){
    pumpRunning = true;
    digitalWrite(dp_pump1, LOW);
    startTime = millis();
  }
  if (pumpRunning and (millis() - startTime > 2500)) {
    digitalWrite(dp_pump1, HIGH);
    pumpRunning = false;
  }

  delay (100);
}

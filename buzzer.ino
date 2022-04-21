#define PIN_BUZZER D3
void setup() {
  pinMode(PIN_BUZZER,OUTPUT);

}

void loop() {
  digitalWrite(PIN_BUZZER,HIGH);
  delay(10000);
  digitalWrite(PIN_BUZZER,LOW);
  delay(5000);
}

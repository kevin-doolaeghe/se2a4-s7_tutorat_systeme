int i;

void setup() {
  for (i = 0; i < 12; i++)
  pinMode(i, OUTPUT);
}

void loop() {
  for (i = 0; i < 12; i++) {
    digitalWrite(i, HIGH);
    delay(1000);
    digitalWrite(i, LOW);
  }
}

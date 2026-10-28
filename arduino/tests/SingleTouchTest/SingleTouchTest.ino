const int touchPin1 = 2;
const int touchPin2 = 3;

void setup() {
  Serial.begin(115200);

  pinMode(touchPin1, INPUT);
  pinMode(touchPin2, INPUT);
}

void loop() {
  int touch1 = digitalRead(touchPin1);
  int touch2 = digitalRead(touchPin2);

  if (touch1 == HIGH) {
    Serial.println("TOUCH 1");
  }

  if (touch2 == HIGH) {
    Serial.println("TOUCH 2");
  }

  if (touch1 == LOW && touch2 == LOW) {
    Serial.println("NOT TOUCHED");
  }

  delay(200);
}

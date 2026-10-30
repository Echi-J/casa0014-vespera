const int sensorCount = 6;
const int touchPins[sensorCount] = {2, 3, 4, 5, 6, 7};

// Store the previous state of each sensor
bool previousState[sensorCount] = {
  false, false, false, false, false, false
};

void setup() {
  Serial.begin(115200);

  // Configure all six touch sensors as inputs
  for (int i = 0; i < sensorCount; i++) {
    pinMode(touchPins[i], INPUT);
  }

  Serial.println("Six Touch Sensors Ready");
}

void loop() {

  for (int i = 0; i < sensorCount; i++) {

    // Read the current touch sensor state
    bool touched = digitalRead(touchPins[i]) == HIGH;

    // Detect changes in touch state
    if (touched != previousState[i]) {

      if (touched) {
        Serial.print("TOUCH ");
      } else {
        Serial.print("RELEASE ");
      }

      Serial.println(i + 1);

      // Update the previous sensor state
      previousState[i] = touched;
    }
  }

  delay(10);
}

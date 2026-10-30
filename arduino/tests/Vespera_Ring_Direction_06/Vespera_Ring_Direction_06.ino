const int sensorCount = 6;
const int touchPins[sensorCount] = {2, 3, 4, 5, 6, 7};

// Store the last accepted sensor
int lastSensor = -1;

// Store the previous single active sensor
int previousSingleSensor = -1;
int previousActiveCount = 0;

// Track gesture timing
unsigned long lastTouchTime = 0;
const unsigned long timeout = 2000;

void setup() {
  Serial.begin(115200);

  // Configure all six sensor pins
  for (int i = 0; i < sensorCount; i++) {
    pinMode(touchPins[i], INPUT);
  }

  Serial.println("Six-Sensor Ring Direction Ready");
}

void loop() {
  int activeCount = 0;
  int currentSensor = -1;

  // Read all sensors
  for (int i = 0; i < sensorCount; i++) {
    if (digitalRead(touchPins[i]) == HIGH) {
      activeCount++;
      currentSensor = i;
    }
  }

  // Ignore ambiguous overlapping touches
  if (activeCount > 1) {
    if (previousActiveCount <= 1) {
      Serial.println("OVERLAP -> IGNORED");
    }
  }

  // Process a touch when exactly one sensor is active
  if (activeCount == 1 &&
      currentSensor != previousSingleSensor) {

    unsigned long now = millis();

    Serial.print("TOUCH ");
    Serial.print(currentSensor + 1);

    if (lastSensor == -1 ||
        now - lastTouchTime > timeout) {

      Serial.println(" -> START");

    } else if (currentSensor == lastSensor) {

      Serial.println(" -> SAME SENSOR");

    } else {

      // Calculate clockwise and counterclockwise steps
      int clockwiseStep =
        (currentSensor - lastSensor + sensorCount)
        % sensorCount;

      int counterclockwiseStep =
        (lastSensor - currentSensor + sensorCount)
        % sensorCount;

      if (clockwiseStep == 1) {
        Serial.println(" -> CLOCKWISE");
      } else if (counterclockwiseStep == 1) {
        Serial.println(" -> COUNTERCLOCKWISE");
      } else {
        Serial.println(" -> JUMP");
      }
    }

    // Save the latest accepted touch
    lastSensor = currentSensor;
    lastTouchTime = now;
  }

  // Track state changes, including overlap transitions
  previousSingleSensor =
    (activeCount == 1) ? currentSensor : -1;

  previousActiveCount = activeCount;

  delay(10);
}

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <math.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>

// Wi-Fi and MQTT configuration
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* ssid1         = SECRET_SSID1;
const char* password1     = SECRET_PASS1;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Vespera configuration
String lightId = "28";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId = "";

const int num_leds = 72;
const int payload_size = num_leds * 3;
byte RGBpayload[payload_size];

// Six capacitive touch sensors
const int sensorCount = 6;
const int touchPins[sensorCount] = {2, 3, 4, 5, 6, 7};

// Touch gesture tracking
int previousSingleSensor = -1;
int lastSensor = -1;

unsigned long lastTouchTime = 0;
const unsigned long gestureTimeout = 2000;

// Smooth hue transition
float currentHue = 0.0;
float targetHue = 0.0;

bool hueInitialized = false;
bool colourNeedsUpdate = false;

unsigned long lastFadeTime = 0;
const unsigned long fadeInterval =40;

// Larger values produce faster transitions
const float smoothingFactor = 0.2;

void setup() {
  Serial.begin(115200);

  Serial.println("Starting Vespera Smooth Hue Ring...");

  // Configure the onboard RGB status LED
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  LedRed();

  // Configure all touch sensors
  for (int i = 0; i < sensorCount; i++) {
    pinMode(touchPins[i], INPUT);
  }

  // Generate a unique MQTT client ID
  byte mac[6];
  WiFi.macAddress(mac);

  char macStr[18];
  snprintf(
    macStr,
    sizeof(macStr),
    "%02X%02X%02X%02X%02X%02X",
    mac[5], mac[4], mac[3],
    mac[2], mac[1], mac[0]
  );

  clientId = "MKR1010_Sender_" + String(macStr);

  Serial.print("Publishing to: ");
  Serial.println(mqtt_topic);

  // Connect to Wi-Fi
  startWifi();

  // Configure MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);

  Serial.println("Smooth Hue Ring Ready!");
}

void loop() {

  // Maintain Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
    colourNeedsUpdate = true;
  }

  mqttClient.loop();

  int activeCount = 0;
  int currentSensor = -1;

  // Read all six touch sensors
  for (int i = 0; i < sensorCount; i++) {
    if (digitalRead(touchPins[i]) == HIGH) {
      activeCount++;
      currentSensor = i;
    }
  }

  // Accept only a single active touch sensor
  if (activeCount == 1 &&
      currentSensor != previousSingleSensor) {

    unsigned long now = millis();

    Serial.print("TOUCH ");
    Serial.print(currentSensor + 1);

    // Detect gesture direction
    if (lastSensor == -1 ||
        now - lastTouchTime > gestureTimeout) {

      Serial.println(" -> START");

    } else if (currentSensor == lastSensor) {

      Serial.println(" -> SAME SENSOR");

    } else {

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

    // Each sensor represents 60 degrees of hue
    float sensorHue = currentSensor * 60.0;

    if (!hueInitialized) {

      // Set the initial colour immediately
      currentHue = sensorHue;
      targetHue = sensorHue;
      hueInitialized = true;

    } else {

      // Find the shortest circular hue transition
      float difference = sensorHue - targetHue;

      while (difference > 180.0) {
        difference -= 360.0;
      }

      while (difference < -180.0) {
        difference += 360.0;
      }

      targetHue += difference;
    }

    // Request a new colour update
    colourNeedsUpdate = true;

    lastSensor = currentSensor;
    lastTouchTime = now;
  }

  // Reset touch tracking during release or overlap
  previousSingleSensor =
    (activeCount == 1) ? currentSensor : -1;

  // Update the colour without blocking touch detection
  updateSmoothHue();

  delay(10);
}

// Gradually move the current hue towards the target hue
void updateSmoothHue() {

  if (!hueInitialized || !mqttClient.connected()) {
    return;
  }

  unsigned long now = millis();

  if (now - lastFadeTime < fadeInterval) {
    return;
  }

  lastFadeTime = now;

  float difference = targetHue - currentHue;
  bool settled = fabsf(difference) < 0.5;

  if (settled) {
    currentHue = targetHue;
  } else {
    currentHue += difference * smoothingFactor;
  }

  // Stop publishing after the final colour is reached
  if (!colourNeedsUpdate && settled) {
    return;
  }

  // Wrap hue into the range 0-360 degrees
  float hue = fmodf(currentHue, 360.0);

  if (hue < 0.0) {
    hue += 360.0;
  }

  // Convert hue into RGB
  int r, g, b;
  hueToRGB(hue, r, g, b);

  // Send the interpolated colour to Lumi / Vespera
  if (send_colour_to_all(r, g, b)) {

    Serial.print("Published Hue: ");
    Serial.println(hue, 1);

    colourNeedsUpdate = !settled;

  } else {
    colourNeedsUpdate = true;
  }
}


void hueToRGB(float hue, int &r, int &g, int &b) {

  // Soft Morandi-inspired RGB palette
const int palette[6][3] = {
  {199, 123, 153},  // Dusty Rose
  {217, 155, 97},   // Warm Apricot
  {124, 166, 109},  // Sage Green
  {83, 165, 162},   // Muted Teal
  {103, 136, 195},  // Soft Periwinkle
  {162, 112, 188}   // Dusty Lavender
};


  // Find the two neighbouring colours
  int index = (int)(hue / 60.0) % 6;
  int nextIndex = (index + 1) % 6;

  // Calculate the position between two colours
  float blend = (hue - index * 60.0) / 60.0;

  // Interpolate the RGB channels smoothly
  r = palette[index][0] +
      (palette[nextIndex][0] - palette[index][0]) * blend;

  g = palette[index][1] +
      (palette[nextIndex][1] - palette[index][1]) * blend;

  b = palette[index][2] +
      (palette[nextIndex][2] - palette[index][2]) * blend;
}


// Publish the same RGB colour to all 72 LEDs
bool send_colour_to_all(int r, int g, int b) {

  if (!mqttClient.connected()) {
    return false;
  }

  for (int pixel = 0; pixel < num_leds; pixel++) {

    RGBpayload[pixel * 3 + 0] =
      (byte)constrain(r, 0, 255);

    RGBpayload[pixel * 3 + 1] =
      (byte)constrain(g, 0, 255);

    RGBpayload[pixel * 3 + 2] =
      (byte)constrain(b, 0, 255);
  }

  return mqttClient.publish(
    mqtt_topic.c_str(),
    RGBpayload,
    payload_size
  );
}

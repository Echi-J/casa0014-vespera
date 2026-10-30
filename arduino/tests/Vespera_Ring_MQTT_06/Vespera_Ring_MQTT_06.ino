#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>

// Wi-Fi and MQTT credentials
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* ssid1         = SECRET_SSID1;
const char* password1     = SECRET_PASS1;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

// Create Wi-Fi and MQTT client objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Vespera light ID and MQTT topic
String lightId = "28";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId = "";

// Vespera RGB payload: 72 LEDs, 3 bytes per LED
const int num_leds = 72;
const int payload_size = num_leds * 3;
byte RGBpayload[payload_size];

// Six touch sensors connected to D2-D7
const int sensorCount = 6;
const int touchPins[sensorCount] = {2, 3, 4, 5, 6, 7};

// Six colours arranged clockwise around the ring
const byte ringColours[6][3] = {
  {255, 0, 0},      // Sensor 1: Red
  {255, 255, 0},    // Sensor 2: Yellow
  {0, 255, 0},      // Sensor 3: Green
  {0, 255, 255},    // Sensor 4: Cyan
  {0, 0, 255},      // Sensor 5: Blue
  {255, 0, 255}     // Sensor 6: Magenta
};

// Track the previous touch position
int previousSingleSensor = -1;
int lastSensor = -1;

// Track gesture timing
unsigned long lastTouchTime = 0;
const unsigned long gestureTimeout = 2000;

void setup() {
  Serial.begin(115200);

  Serial.println("Starting Vespera Touch Ring...");

  // Configure the onboard RGB status LED
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  LedRed();

  // Configure all touch sensors as inputs
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

  Serial.print("Device Client ID: ");
  Serial.println(clientId);

  Serial.print("Publishing to topic: ");
  Serial.println(mqtt_topic);

  // Connect to Wi-Fi
  startWifi();

  // Configure the MQTT connection
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);

  Serial.println("Touch Ring Ready!");
}

void loop() {

  // Maintain Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
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

  // Process only one active sensor at a time
  if (activeCount == 1 &&
      currentSensor != previousSingleSensor) {

    unsigned long now = millis();

    Serial.print("TOUCH ");
    Serial.print(currentSensor + 1);

    // Identify the start of a new gesture
    if (lastSensor == -1 ||
        now - lastTouchTime > gestureTimeout) {

      Serial.println(" -> START");

    } else if (currentSensor == lastSensor) {

      Serial.println(" -> SAME SENSOR");

    } else {

      // Calculate the direction between sensors
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

    // Select the colour associated with this sensor
    int r = ringColours[currentSensor][0];
    int g = ringColours[currentSensor][1];
    int b = ringColours[currentSensor][2];

    // Publish the selected colour to Lumi / Vespera
    send_colour_to_all(r, g, b);

    Serial.print("Selected RGB: ");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.println(b);

    lastSensor = currentSensor;
    lastTouchTime = now;
  }

  // Ignore ambiguous states with multiple active sensors
  previousSingleSensor =
    (activeCount == 1) ? currentSensor : -1;

  delay(10);
}

// Set all 72 LEDs to the same RGB colour
void send_colour_to_all(int r, int g, int b) {

  if (mqttClient.connected()) {

    for (int pixel = 0; pixel < num_leds; pixel++) {

      RGBpayload[pixel * 3 + 0] =
        (byte)constrain(r, 0, 255);

      RGBpayload[pixel * 3 + 1] =
        (byte)constrain(g, 0, 255);

      RGBpayload[pixel * 3 + 2] =
        (byte)constrain(b, 0, 255);
    }

    bool published = mqttClient.publish(
      mqtt_topic.c_str(),
      RGBpayload,
      payload_size
    );

    if (published) {
      Serial.println("MQTT publish successful.");
    } else {
      Serial.println("MQTT publish failed.");
    }

  } else {
    Serial.println("MQTT not connected.");
  }
}

// Turn all 72 LEDs off
void send_all_off() {
  send_colour_to_all(0, 0, 0);
}

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

//Firmware version
#define FIRMWARE_VERSION   "0.3.1"

//WiFi config
#define WLAN_SSID          "HomePi"
#define WLAN_PASS          "p@ssw0rd"

//MQTT Server config
#define MQTT_SERVER        "192.168.1.10"

//Setup WiFi/MQTT Client
WiFiClient wlan;
MQTTClient client;

/*
 * DEVICE MODES
 * 0 - As conroller board (All outputs)
 * 1 - As power outlet (One/Many relay outputs)
 * 2 - As led controller (One/Two rgb led outputs)
 */
int device_type = 0;

const int chipID = ESP.getChipId();
char chipID_char[32];
char device_name[64] = "ESP8266_";
char device_topic[64] = "devices/";
char device_topic_state[64] = "devices/";
char device_topic_set[64] = "devices/";
char devices_topic[64] = "devices";

unsigned long previousMillisFunction = 0;
const long functionInterval = 1000;

const int pin[] = {0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16};
const int pinCount = sizeof(pin) / sizeof(pin[0]);
int pinStates[16];

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(10);

  initSetup();
  connectWifi();
  connectMQTT();
  deviceSpecificSetup();
}


void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if(!client.connected()) {
    connectMQTT();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFunction >= functionInterval) {
    loopFunctions();
    previousMillisFunction = millis();
  }
}

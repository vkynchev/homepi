#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

//Firmware version
#define FIRMWARE_VERSION   "0.2.2"

//WiFi config
#define WLAN_SSID          "HomePi"
#define WLAN_PASS          "p@ssw0rd"

//MQTT Server config
#define MQTT_SERVER        "192.168.1.10"


//Setup MQTT Client
WiFiClient wlan;
MQTTClient client;



const int chipID = ESP.getChipId();
char chipID_char[32];
char device_name[64] = "ESP8266_";
char device_topic[64] = "devices/";
char device_topic_state[64] = "devices/";
char device_topic_set[64] = "devices/";
char devices_topic[64] = "devices";

unsigned long previousMillisState = 0;
const long stateInterval = 1000;


const int ledPin = 2;
const int pin[] = {0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16};
const int pinCount = sizeof(pin) / sizeof(pin[0]);
int pinStates[16];

bool busyPinState = false;


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(10);

  //Force all pins as OUTPUTS for now and pull them LOW
  for (int i = 0; i < pinCount; i++)
  {
    pinMode(pin[i], OUTPUT);
    analogWrite(pin[i], 0);

    //Init the pinStates array
    pinStates[pin[i]-1] = 0;
  }

  //Convert chipID to char
  sprintf(chipID_char, "%d", chipID);
  strcat(device_name, chipID_char);           // ESP8266_XXXXXXXX
  strcat(device_topic, chipID_char);          // devices/XXXXXXXX
  strcat(device_topic_state, chipID_char);    // devices/XXXXXXXX
  strcat(device_topic_set, chipID_char);    // devices/XXXXXXXX
  strcat(device_topic_state, "/state");    // devices/XXXXXXXX/state
  strcat(device_topic_set, "/set");    // devices/XXXXXXXX/set

  Serial.println(device_topic_state);
  Serial.println(device_topic_set);

  //Print device info
  Serial.println();
  Serial.println();
  Serial.println("------------------INFO---------------------");
  Serial.print("Firmware version: ");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("ChipID: ");
  Serial.println(chipID);
  Serial.print("Device Name: ");
  Serial.println(device_name);
  Serial.print("Device Topic: ");
  Serial.println(device_topic);
  Serial.println("-------------------------------------------");

  //Start connecting to the WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  
  //Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
  Serial.println();
  digitalWrite(ledPin, LOW);
  
  //Tell the IP Address
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Connect to MQTT Server
  client.begin(MQTT_SERVER, wlan);


  //Turn off the internal led after setup complete
  delay(1500);
  digitalWrite(ledPin, HIGH);
  pinStates[1] = 1023;
}



void sendPinState() {
  if(!busyPinState) {
    //Begin sending
    busyPinState = true;
    
    //Generate json and send it to main devices topic
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& states = root.createNestedArray("pinStates");
    for (int i = 0; i < pinCount; i++)
    {
      JsonArray& data = states.createNestedArray();
      data.add(pin[i]);
      data.add(pinStates[pin[i]-1]);
    }
  
    char JSONBuffer[root.measureLength() + 1];
    root.printTo(JSONBuffer, sizeof(JSONBuffer));
    
    MQTTMessage message;
    message.topic = device_topic_state;
    message.payload = JSONBuffer;
    message.length = sizeof(JSONBuffer) - 1;
    message.retained = true;
    if(client.publish(&message)) {
      Serial.println("Sent pins state! With length of ");
      Serial.print(sizeof(JSONBuffer));
      Serial.println(" bytes");
    } else {
      Serial.println("The pins state was not sent! :(");
    }

    //Finish sending
      previousMillisState = millis();
      busyPinState = false;
  }
}



void connectMQTT() {
  Serial.println("Connecting to MQTT Server...");
  while (!client.connect(device_name, "try", "try")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");

  
  
  client.subscribe(device_topic_set);
  Serial.println("Subscribed to my own /set topic!");
  // client.unsubscribe("/example");


  
  //Generate json and send it to main devices topic
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = chipID;
  root["firmware_version"] = FIRMWARE_VERSION;
  root["name"] = device_name;
  root["topic"] = device_topic;

  char JSONBuffer[root.measureLength() + 1];
  root.printTo(JSONBuffer, sizeof(JSONBuffer));
  
  MQTTMessage message;
  message.topic = devices_topic;
  message.payload = JSONBuffer;
  message.length = sizeof(JSONBuffer) - 1;
  message.retained = false;
  
  
  if(client.publish(&message)) {
    Serial.print("Published device info to '");
    Serial.print(devices_topic);
    Serial.print("' topic! With length of ");
    Serial.print(sizeof(JSONBuffer) - 1);
    Serial.println(" bytes");
  } else {
    Serial.println("The device info was not sent! :(");
  }
  
}



void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  /*
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
  */
  
  if(topic == device_topic_set) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(bytes);
    int pin = root["pin"];
    int value = root["value"];
  
    pinMode(pin, OUTPUT);
    analogWrite(pin, value);

    //Update the value in pinStates array
    pinStates[pin-1] = value;

    //sendPinState();
  } 
}



void loop() {

  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if(!client.connected()) {
    connectMQTT();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisState >= stateInterval) {
    sendPinState();
  }
}


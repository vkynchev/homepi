void initSetup() {
  EEPROM.begin(1024);
  if(EEPROM.read(0) != 0) {
    EEPROM.write(0, 0); //Is new device -> false
    EEPROM.write(1, 0); // Set device type
    EEPROM.commit();
  }
  device_type = EEPROM.read(1);

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
  Serial.print("Device Type: ");
  Serial.println(device_type);
  Serial.print("Device Name: ");
  Serial.println(device_name);
  Serial.print("Device Topic: ");
  Serial.println(device_topic);
  Serial.println("-------------------------------------------");
}

void connectWifi() {
  //Start connecting to the WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  //Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println();

  //Tell the IP Address
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Connect to MQTT Server
  client.begin(MQTT_SERVER, wlan);
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
  root["type"] = device_type;
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

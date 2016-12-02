void deviceSpecificSetup() {

  switch (device_type) {
    case 0:
      //Force all pins as OUTPUTS for now and pull them LOW
      for (int i = 0; i < sizeof(outputPins)/sizeof(outputPins[0]); i++)
      {
        pinMode(outputPins[i], OUTPUT);
        // Internal led check
        if(outputPins[i] == 2) {
          analogWrite(outputPins[i], 1023);
          pinStates[outputPins[i]-1] = 1023;
        } else {
          analogWrite(outputPins[i], 0);
          pinStates[outputPins[i]-1] = 0;
        }
      }
      break;
    case 1:
      //Force relay pins as OUTPUTS and pull them LOW
      for (int i = 0; i < sizeof(relayPins)/sizeof(relayPins[0]); i++)
      {
        pinMode(relayPins[i], OUTPUT);
        analogWrite(relayPins[i], 0);
        pinStates[relayPins[i]-1] = 0;
      }
      break;
    case 2:
      //Force all pins as OUTPUTS for now and pull them LOW
      for (int i = 0; i < sizeof(ledPins)/sizeof(ledPins[0]); i++)
      {
        pinMode(ledPins[i], OUTPUT);
        analogWrite(ledPins[i], 0);
        pinStates[ledPins[i]-1] = 0;
      }
      break;
  }

}

void loopFunctions() {

  switch (device_type) {
    case 0:
      sendPinState();
      break;
    case 1:
        //
        break;
    case 2:
      sendPinState();
      break;
  }

}

void sendPinState() {
  //Generate json and send it to main devices topic
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& states = root.createNestedArray("pinStates");
  switch (device_type) {
    case 0:
      for (int i = 0; i < sizeof(outputPins)/sizeof(outputPins[0]); i++)
      {
        JsonArray& data = states.createNestedArray();
        data.add(outputPins[i]);
        data.add(pinStates[outputPins[i]-1]);
      }
      break;
    case 1:
      for (int i = 0; i < sizeof(relayPins)/sizeof(relayPins[0]); i++)
      {
        JsonArray& data = states.createNestedArray();
        data.add(relayPins[i]);
        data.add(pinStates[relayPins[i]-1]);
      }
      break;
    case 2:
      for (int i = 0; i < sizeof(ledPins)/sizeof(ledPins[0]); i++)
      {
        JsonArray& data = states.createNestedArray();
        data.add(ledPins[i]);
        data.add(pinStates[ledPins[i]-1]);
      }
      break;
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
}


void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  /*
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println();
  */
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(bytes);
  String device_command = root["command"];

  if(topic == device_topic_set) {
    if(device_command == "set_type") {
      int type = root["type"];
      device_type = type;
      EEPROM.write(3, device_type); // Set device type
      EEPROM.commit();
      ESP.restart();
    }
  }

  switch (device_type) {
    case 0:
      if(topic == device_topic_set) {
        if(device_command == "set_pin") {
          int pin = root["pin"];
          int value = root["value"];

          pinMode(pin, OUTPUT);
          analogWrite(pin, value);

          //Update the value in pinStates array
          pinStates[pin-1] = value;
        }
      }
      break;
    case 1:
      if(topic == device_topic_set) {
        if(device_command == "set_pin") {
          int pin = root["pin"];
          int value = root["value"];

          pinMode(pin, OUTPUT);
          analogWrite(pin, value);

          //Update the value in pinStates array
          pinStates[pin-1] = value;

          sendPinState();
        }
      }
      break;
      case 2:
        if(topic == device_topic_set) {
          if(device_command == "set_pin") {
            int pin = root["pin"];
            int value = root["value"];

            pinMode(pin, OUTPUT);
            analogWrite(pin, value);

            //Update the value in pinStates array
            pinStates[pin-1] = value;
          }
        }
        break;
  }
}

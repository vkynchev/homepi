void loopFunctions() {

  switch (device_type) {
    case 0:
      sendPinState();
      break;
  }

}

void deviceSpecificSetup() {

  switch (device_type) {
    case 0:
      //Force all pins as OUTPUTS for now and pull them LOW
      for (int i = 0; i < pinCount; i++)
      {
        pinMode(pin[i], OUTPUT);

        // Internal led check
        if(pin[i] == 2) {
          analogWrite(pin[i], 1023);
          pinStates[pin[i]-1] = 1023;
        } else {
          analogWrite(pin[i], 0);
          pinStates[pin[i]-1] = 0;
        }


        //Init the pinStates array

      }
      break;
  }

}

void sendPinState() {
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
}


void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  /*
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
  */

  switch (device_type) {
    case 0:
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
      break;
  }
}

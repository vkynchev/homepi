void relayHardwarePinPressed(){

    for (int i = 0; i < sizeof(relayHardwarePins)/sizeof(relayHardwarePins[0]); i++)
    {
      if(!digitalRead(relayHardwarePins[i])){
        int pin = relayPins[i];
        int value = 1023;
        if(digitalRead(relayPins[i])) {
          value = 0;
        }

        pinMode(pin, OUTPUT);
        analogWrite(pin, value);

        //Update the value in pinStates array
        pinStates[pin-1] = value;
      }
    }

}

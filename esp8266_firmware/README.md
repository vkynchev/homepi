# HomePi - ESP8266 Firmware
Home Automation System with Raspberry Pi as MQTT Broker

## How-to

### Install the libraries

Install the libraries in the ``/libraries``` directory.
Copy them to the Arduino IDE libraries folder or use the Arduino library manager.

### Compile and flash

You will need to install the [Arduino core for ESP8266](https://github.com/esp8266/Arduino).

#### Installing with Boards Manager

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install Arduino 1.6.8 from the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

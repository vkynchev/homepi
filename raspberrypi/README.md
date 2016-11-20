# HomePi - Raspberry Pi
Home Automation System with Raspberry Pi as MQTT Broker

## Installation

### Mosquitto broker
Unfortunately, the Raspberry Pi normal “apt-get” archives do not contain the latest version of the Mosquitto software.  If you don’t install the latest version of the broker, you will get odd errors (because of version compatibility errors) and it will not work.  So, the first thing is to open a terminal window (or log in using ssh) to your Raspberry Pi and do the following:

```
sudo wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
sudo apt-key add mosquitto-repo.gpg.key
cd /etc/apt/sources.list.d/
sudo wget http://repo.mosquitto.org/debian/mosquitto-wheezy.list
sudo apt-get update
sudo apt-get install mosquitto
```

Next we can install the Mosquitto broker proper.

```
sudo apt-get install mosquitto
```

As is the case with most packages from Debian, the broker is immediately started. Now you have MQTT Broker running on the raspberry pi.

### Assign static IP address to the Raspberry Pi
Next you will need to set static IP.
Note: These instructions are for Raspbian Jessie.

Open terminal and type
```
sudo nano /etc/dhcpcd.conf
```
to start editing the `dhcpcd.conf` file

Scroll all the way to the bottom of the file and add one, or both of the following snippets. Depending on whether you want to set a static IP address for a wired connection or a wireless connection eth0 = wired, wlan0 = wireless.

You’ll need to edit the numbers in the snippet so they match your network configuration.
```
interface eth0

static ip_address=192.168.1.10/24
static routers=192.168.1.1
static domain_name_servers=192.168.1.1

interface wlan0

static ip_address=192.168.1.10/24
static routers=192.168.1.1
static domain_name_servers=192.168.1.1
```

`interface` _> This defines which network interface you are setting the configuration for. <_
`static ip_address` -> This is the IP address that you want to set your device to. (Make sure you leave the /24 at the end)
`static routers` -> This is the IP address of your gateway (probably the IP address or your router)
`static domain_name_servers` -> This is the IP address of your DNS (probably the IP address of your router). You can add multiple IP addresses here separated with a single space.


To exit the editor, press ctrl+x
To save your changes press the letter “Y” then hit enter
Now all you need to do is reboot, and everything should be set!

Type `reboot` and press enter.

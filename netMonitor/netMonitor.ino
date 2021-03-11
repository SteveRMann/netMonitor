#define SKETCH_NAME "netMonitor.ino"
#define SKETCH_VERSION "Version 5.1 3/10/2021"
#define hostPrefix "NetMonitor-"

/*
   This pings essential IP addresses, and if no response is received, it lights a red LED.
   If all pings are good, it lights a green LED

   PCB built on a Wemos D1 Mini (ESP8266)

   Version 3.0 9/4/2019
     Working with three-color LEDs
   Version 3.1 10/3/2019
     Removed local router from the ping list, added plex server.
     (If the router is offline, no other pings would work anyway).
     Added Eagle folder to the git
   Version 4.0 2/17/2020
     Added OTA.
   Version 5.0 8/25/2020
     Added mqtt to publish ping errors to be logged.
   Version 5.1 3/10/2021
     Publish the failing IP.

*/



//-------------------------
#include <ESP8266Ping.h>
#include <Wire.h>               // Wire.h is the library for communicating with I2C devices
#include <ArduinoOTA.h>
//#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  \\ WiFi credentials
#include <Kaywinnet.h>          // WiFi credentials

#define DEBUG true  //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial
#define DBUG                    // If defined, show more debug statements

#define ledPin 14               // Pin D5 on NodeMcu or Wemos D1 Mini (Blue LED)

int myBits = 0;
int hostNum  = 0;

//-------------------------
//mqtt
#include <ESP8266WiFi.h>        // Connect (and reconnect) an ESP8266 to the a WiFi network.
#include <PubSubClient.h>       // connect to a MQTT broker and publish/subscribe messages in topics.
// Declare an object of class WiFiClient, which allows to establish a connection to a specific IP and port
// Declare an object of class PubSubClient, which receives as input of the constructor the previously defined WiFiClient.
// The constructor MUST be unique on the network. I use the last two bytes of the MAC
WiFiClient netmClientEF;
PubSubClient client(netmClientEF);

#define NODENAME "netMonitor"                             // Give this node a name
const char *cmndTopic = NODENAME "/cmnd";                 // Incoming commands, payload is a command.
const char *statusTopic = NODENAME "/status";
const char *connectName =  NODENAME "EF";                  // Must be unique on the network
const char *mqttServer = MQTT_SERVER;                     // Local broker defined in Kaywinnet.h
const int mqttPort = 1883;




//-------------------------
// For OTA
char macBuffer[24];       // Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = hostPrefix;
char hostName[24];        // Holds hostNamePrefix + the last three bytes of the MAC address.

// The Library ESP8266Ping only works with IP's, but....
// You can use
// WiFi.hostByName(domainName,resultIP);
// Reference: https://www.arduino.cc/en/Reference/WiFiNINAhostByName



// IP's to check.
#include "ipList.h"
const int webIpCount = (sizeof(ipx) / sizeof(ipx[0])) - 3;
//const int ipCount = 4;
//const int webIpCount = 9;
int pingDelay = 10000;       //Ping all IP's then wait (in ms) before pinging again.



//-------------------------
//for LED status
#include <Ticker.h>
Ticker blueTicker;                //Ticker object for the WiFi Connecting LED
Ticker greenTicker;               //Ticker object for the Ping Activity LED



// ****************************  Function to blink LED on pin D5 ****************************
void blueTick() {
  //toggle state
  int state = digitalRead(ledPin);            // get the current state of GPIO14 pin
  digitalWrite(ledPin, !state);               // set pin to the opposite state
}



// ****************************  Function to blink LED[i] ****************************
void greenTick() {
  // Toggle the LED
  bitWrite(myBits, hostNum , !bitRead(myBits, hostNum ));
  wSend(myBits);
  delay(40);
}




// **************************** Function to send a byte to the LED array. ****************************
void wSend (byte wData) {
  //Send the data to the MCP23008
  Wire.beginTransmission(0x20);   //Start talking to the slave device.
  Wire.write(0x09);               //Select the GPIO pins.
  Wire.write(wData);              //Writing a 1 to the pin turns it on, while writing a 0 turns it off.
  Wire.endTransmission();         //ends communication with the device
}






// **************************** Function to print an 8-bit binary number. ****************************
// Usage: printBinaryByte(0x97);
// Prints: "10010111"
void printBinaryByte(byte value) {
  for (byte mask = 0x80; mask; mask >>= 1)
  {
    Serial.print((mask & value) ? '1' : '0');
  }
}

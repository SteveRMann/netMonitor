#define SKETCH __FILE__
#define VERSION "2.30"           // Four characters
#define hostPrefix "NETMON"      // Six characters max

/*
   A fork of NetMonitor
   This program pings essential IP addresses, and if no response is received, it lights a red LED.
   If all pings are good, it lights a green LED

   PCB built on a Wemos D1 Mini (ESP8266)

   Version 2.00 11/25/2023   Removed ticker
   Version 2.30 12/14/2023   Removed OTA, using <ESPping.h>

*/



//-------------------------
//#include <ESP8266Ping.h>        // Includes ESP8266WiFi.h
#include <ESPping.h>
#include <Wire.h>               // Wire.h is the library for communicating with I2C devices
#include <Kaywinnet.h>          // WiFi credentials

#include "hosts.h"              //List of hostnames and IP addresses to ping
int ipCount;                    //Number of local IP addresses to ping.
int numberOfHosts;              //Number of hosts in the hosts[] array.


// setup_wifi vars
char macBuffer[24];       // Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = hostPrefix;
char hostName[24];        // Holds hostNamePrefix + the last three bytes of the MAC address.

const char WIFISSID[] = MY_SSID;      //Select the network credentials
const char WIFIPASS[] = MY_PASSWORD;


#include <dlay.h>
dlay  aTimer;                   // Allocate a global timer.


#define DEBUG true              // Set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial
#define DBUG                    // If defined, show more debug statements

#define blueLedPin D4           // GPIO2, BuiltIn LED.
#define LEDON 0
#define LEDOFF 1

int myBits = 0;                 //Contains the LED bit pattern: BRRRRGGGG
int ledNum  = 0;



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


// **************************** Function to checksum a string. ****************************
byte stringChecksum(char *s)
{
  byte c = 0;
  while (*s != '\0')
    c ^= *s++;
  return c;
}


// ********************** Functions to turn on LEDs **********************
void red(int lednum) {
  bitWrite(myBits, lednum + 4, 1);
}
void green(int lednum) {
  bitWrite(myBits, lednum , 1);
}
void yellow(int lednum) {
  bitSet(myBits, lednum);
  bitSet(myBits, lednum + 4);
}


void printBinary(byte inByte)
{
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(inByte, b));
  }
}

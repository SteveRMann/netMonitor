#define SKETCH_NAME "netMonitor.ino"
#define SKETCH_VERSION "Version 3.1 10/3/2019"
#define hostPrefix "NetMonitor-"

/*
   This pings essential IP addresses, and if no response is received, it lights a red LED.
   If all pings are good, it lights a green LED

   built on a NodeMCU (ESP8266)

   Version 3.0 9/4/2019
     Working with three-color LEDs
   Version 3.1 10/3/2019
     Removed local router from the ping list, added plex server.
     (If the router is offline, no other pings would work anyway).
     Added Eagle folder to the git
   Version 4.0 2/17/2020
     Added OTA.

*/

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Wire.h>               // Wire.h is the library for communicating with I2C devices
#include <ArduinoOTA.h>
#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  \\ WiFi credentials


#define DEBUG true  //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial
#define DBUG                    // If defined, show more debug statements

//#define ESP01                   // Just select one
#define NODEMCU

#ifdef NODEMCU
#define ledPin 14               // Pin D5 on NodeMcu
#endif
#ifdef ESP01
#define ledPin 1                // GPIO1 is the Tx pin on the ESP-01
#endif



int myBits = 0;
int hostNum  = 0;


// For OTA
char macBuffer[24];       // Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = hostPrefix;
char hostName[24];        // Holds hostNamePrefix + the last three bytes of the MAC address.


// IP's to check.
const char* ipx[] = {
  "192.168.1.111",      // Plex Server
  "192.168.1.124",      // MQTT Broker
  "192.168.1.60",       // Rosie (I.E. Home Assistant)
  // Web IPs, ping one at random per loop
  "172.217.11.14",      // Google.com
  "176.32.103.205",     // amazon.com
  "172.217.6.206",      // youtube.com
  "98.137.246.7",       // yahoo.com
  "173.223.17.209",     // sears.com
  "83.100.177.234",     // element14.com
  "140.82.113.3",       // github.com
  "31.13.71.36",        // facebook.com
  "104.25.24.31",       // home-assistant.io
  "204.141.42.155"      // Zoho.com
};

const int webIpCount = (sizeof(ipx) / sizeof(ipx[0]))-3;
//const int ipCount = 4;
//const int webIpCount = 9;
int pingDelay = 10000;       //Ping all IP's then wait (in ms) before pinging again.



//for LED status
#include <Ticker.h>
Ticker blueTicker;                //Ticker object for the WiFi Connecting LED
Ticker greenTicker;               //Ticker object for the Ping Activity LED



// ****************************  Function to blink LED on pin D5 ****************************
void blueTick() {
  //toggle state
  int state = digitalRead(ledPin);            // get the current state of GPI14 pin
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



//**************************** SETUP ****************************
void setup() {

  beginSerial();

  pinMode(ledPin, OUTPUT);                  // set the blue LED pin on GPIO14 (D5) as output

  //Set up the MCP23008 I/O expander
  Wire.begin();                   //Creates a Wire object with the default SDA and SCL pins.
  Wire.beginTransmission(0x20);   //Begins talking to the slave device.
  Wire.write(0x00);               //Selects the IODIR register.  The IODIR is the register of all the 8 pins.
  Wire.write(0x00);               //This sets all I/O pins to outputs.
  Wire.endTransmission();         //Stops talking to device.


  // Flash all LEDs
  Serial.println(F("Flash all LEDs"));
  wSend (B00000000);            //Set all pins low
  delay(400);
  wSend(B11111111);             // Set all output  pins high
  delay(400);
  wSend (0);                    //Walk the LEDS
  delay(400);
  for (int i = 0; i < 8; i++) {
    bitWrite(myBits, i, 1);
    wSend(myBits);
    delay(40);
  }
  for (int i = 7; i > 0; i--) {
    bitWrite(myBits, i, 0);
    wSend(myBits);
    delay(40);
  }
  myBits = B00000000;                       // All LEDs off
  wSend(myBits);


  // Connect to a WiFi network
  blueTicker.attach(0.1, blueTick);         // start blueTick() with a fast blink while we connect
  //connectWiFi();
  setup_wifi();
  blueTicker.detach();                      // Stop blueTick()
  digitalWrite(ledPin, 0);                  // Make sure the blue LED is off.

  start_OTA();

  myBits = B11110000;                       // Reset with all red LEDs on
  wSend(myBits);


Serial.print(F("webIpCount= "));
Serial.println(webIpCount);
}




//************************************** LOOP *******************************
void loop() {
  ArduinoOTA.handle();

  char pingIP[20];                            // IP that we're going to ping.

  //For each IP in the list
  for (int i = 0; i < 4; i++) {               // for four LEDS
    hostNum = i;

    if (i == 3) i = i + random(0, webIpCount);  // Last LED. Select an external IP at random.
    strcpy(pingIP, ipx[i]);

    Serial.println();
    Serial.print(F("IP "));
    Serial.print(i);
    Serial.print(F(",  Host "));
    Serial.println(hostNum);
    //Serial.print(F("pingIP= "));
    //Serial.println(pingIP);

    Serial.print("Pinging ");
    Serial.print(pingIP);

    greenTicker.attach(0.1, greenTick);         // Start greenTick() while we ping

    yield();                                     // Reset the WDT on the ESP8266
    if (Ping.ping(pingIP)) {                    // Ping
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 1);             // Turn on the green LED
      bitWrite(myBits, hostNum + 4, 0);         // Turn off the red LED
      wSend(myBits);


      //Serial.println();
      //Serial.println(F("Success!!"));
      //Serial.print(F("LED bits: "));
      //printBinaryByte(myBits);
      //Serial.println();


    } else {
      // FAIL
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 0);             // Turn off the green LED
      bitWrite(myBits, hostNum + 4, 1);         // Turn on the red LED
      wSend(myBits);
      pingDelay = 10;                           //Retry immediately

      Serial.println();
      Serial.println(F("Error"));
      Serial.print(F("LED bits: "));
      printBinaryByte(myBits);
      Serial.println();
    }
  }  //end For

  Serial.println();
  Serial.println(F("----------------------"));
  delay(pingDelay);                           // Don't want to flood the net with pings.
  pingDelay = 10000;                          // Set delay back to default.
}

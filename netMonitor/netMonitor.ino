#define SketchName "netMonitor.ino"
#define sketchVersion "Version 3.0 9/4/2019"

/*
   This pings essential IP addresses, and if no response is received, it lights a red LED.
   If all pings are good, it lights a green LED

   built on a NodeMCU (ESP8266)

   Version 3.0 9/4/2019
     Working with three-color LEDs
     
*/

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Wire.h>               // Wire.h is the library for communicating with I2C devices


#define DEBUG true  //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial

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


// IP's to check.
const char* ipx[] = {
  "192.168.1.1",       // Router
  "192.168.1.124",     // MQTT Broker
  "192.168.1.128",     // Rosie (I.E. Home Assistant)
  "172.217.11.14",     // Google.com
  "176.32.103.205",    // amazon.com
  "172.217.6.206",     // youtube.com
  "98.137.246.7"       // yahoo.com
};

//"192.168.1.111",     // Plex server
//"amazon.com",        // We will pick one of these external IPs at random for the 4th IP
//"youtube.com",       // This changes the external IP with each scan to minimize ping activity.
//"yahoo.com"

const int ipCount = 4;
const int webIpCount = 4;
const int pingDelay = 1000;       //Ping all IP's then wait (in ms) before pinging again.



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






#ifndef ESP01
// **************************** Function to print an 8-bit binary number. ****************************
// Usage: printBinaryByte(0x97);
// Prints: "10010111"
void printBinaryByte(byte value) {
  for (byte mask = 0x80; mask; mask >>= 1)
  {
    Serial.print((mask & value) ? '1' : '0');
  }
}
#endif



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
  connectWiFi();
  blueTicker.detach();                      // Stop blueTick()
  digitalWrite(ledPin, 0);                  // Make sure the blue LED is off.


  myBits = B11110000;                       // Reset with all red LEDs on
  wSend(myBits);

}




//************************************** LOOP *******************************
void loop() {
  char pingIP[20];                            // IP that we're going to ping.

  //For each IP in the list
  for (int i = 0; i < ipCount; i++) {
    hostNum = i;


    if (i == ipCount - 1) i = i + random(0, webIpCount);  // Last IP. Select an external IP at random.
    strcpy(pingIP, ipx[i]);

    Serial.print(F("DBG I="));
    Serial.println(i);
    Serial.print(F("DBG hostNum="));
    Serial.println(hostNum);
    Serial.print(F("DBG pingIP="));
    Serial.println(pingIP);


#ifndef ESP01
    Serial.print("Pinging host ");
    Serial.print(i);
    Serial.print(", ");
    //    Serial.print(ipx[i]);
    Serial.print(pingIP);
#endif

    greenTicker.attach(0.1, greenTick);         // Start greenTick() while we ping

    yield();                                     // Reset the WDT on the ESP8266
    if (Ping.ping(pingIP)) {                    // Ping
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 1);             // Turn on the green LED
      bitWrite(myBits, hostNum + 4, 0);         // Turn off the red LED
      wSend(myBits);

#ifndef ESP01
      Serial.println();
      Serial.println(F("Success!!"));
      Serial.print(F("LED bits: "));
      printBinaryByte(myBits);
      Serial.println();
#endif

    } else {
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 0);             // Turn off the green LED
      bitWrite(myBits, hostNum + 4, 1);         // Turn on the red LED
      wSend(myBits);

#ifndef ESP01
      Serial.println();
      Serial.println(F("Error"));
      Serial.print(F("LED bits: "));
      printBinaryByte(myBits);
      Serial.println();
#endif

    }
  }  //end For

  Serial.println(F("Before Delay"));
  delay(pingDelay);                           // Don't want to flood the net with pings.
  Serial.println(F("After Delay"));

}

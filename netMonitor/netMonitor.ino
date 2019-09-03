#define SketchName "netMonitor.ino"
#define sketchVersion "Version 2.0 9/2/2019"

/*
   This pings essential IP addresses, and if no response is received, it lights a red LED.
   If all pings are good, it lights a green LED

   built on a NodeMCU (ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Wire.h>               // Wire.h is the library for communicating with I2C devices


#define DEBUG true  //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial


int myBits = 0;
int hostNum  = 0;


// IP's to check.
const char* ipx[] = {
  "google.com",
  "192.168.1.1",      // Router
  "192.168.1.124",    // MQTT Broker
  "192.168.1.128"     // Rosie
};
const int ipCount = 4;
const int pingDelay = 5000;       //Ping all IP's then wait (in ms) before pinging again.



//for LED status
#include <Ticker.h>
Ticker blueTicker;                //Ticker object for the WiFi Connecting LED
Ticker greenTicker;               //Ticker object for the Ping Activity LED



// ****************************  Function to blink LED on pin D5 ****************************
void blueTick()
{
  //toggle state
  int state = digitalRead(14);            // get the current state of GPI14 pin
  digitalWrite(14, !state);               // set pin to the opposite state
}



// ****************************  Function to blink LED[i] ****************************
void greenTick()
{
  // Toggle the LED
  bitWrite(myBits, hostNum , !bitRead(myBits, hostNum ));
  wSend(myBits);
  delay(40);
}




// **************************** Function to send a byte to the LED array. ****************************
void wSend (byte wData)
{
  //Send the data to the MCP23008
  Wire.beginTransmission(0x20);   //Start talking to the slave device.
  Wire.write(0x09);               //Select the GPIO pins.
  Wire.write(wData);              //Writing a 1 to the pin turns it on, while writing a 0 turns it off.
  Wire.endTransmission();         //ends communication with the device
}







// **************************** Function to print an 8-bit binary number. ****************************
// Usage: printBinaryByte(0x97);
// Prints: "10010111"
void printBinaryByte(byte value)
{
  for (byte mask = 0x80; mask; mask >>= 1)
  {
    Serial.print((mask & value) ? '1' : '0');
  }
}



//**************************** SETUP ****************************
void setup() {

  beginSerial();

  pinMode(14, OUTPUT);                      // set the blue LED pin on GPIO14 (D5) as output

  // We start by connecting to a WiFi network
  blueTicker.attach(0.1, blueTick);         // start blueTick() with a fast blink while we connect
  connectWiFi();
  blueTicker.detach();                      // Stop blueTick()
  digitalWrite(14, 1);                      // Make sure the blue LED is on.


  //Set up the MCP23008 I/O expander
  Wire.begin();                   //Creates a Wire object with the default SDA and SCL pins.
  Wire.beginTransmission(0x20);   //Begins talking to the slave device.
  Wire.write(0x00);               //Selects the IODIR register.  The IODIR is the register of all the 8 pins.
  Wire.write(0x00);               //This sets all I/O pins to outputs.
  Wire.endTransmission();         //Stops talking to device.

  // Flash all LEDs
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
  myBits = B11110000;          // Reset with all red LEDs on
  wSend(myBits);
}


//************************************** LOOP *******************************
void loop() {

  //For each IP in the list
  for (int i = 0; i < ipCount; i++) {
    hostNum = i;
    Serial.print("Pinging host ");
    Serial.print(i);
    Serial.print(", ");
    Serial.print(ipx[i]);
    Serial.print(", ");

    greenTicker.attach(0.1, greenTick);         //start greenTick() while we ping

    if (Ping.ping(ipx[i])) {                    //Ping
      greenTicker.detach();                     //Stop greenTick()
      Serial.println("Success!!");
      printBinaryByte(myBits);
      Serial.println();
      bitWrite(myBits, i, 1);                   //Turn on the green LED
      bitWrite(myBits, i + 4, 0);               //Turn off the red LED
      printBinaryByte(myBits);
      Serial.println();
      wSend(myBits);

    } else {
      greenTicker.detach();                     //Stop greenTick()
      Serial.println("Error");
      printBinaryByte(myBits);
      Serial.println();
      bitWrite(myBits, i, 0);                   //Turn off the green LED
      bitWrite(myBits, i + 4, 1);               //Turn on the red LED
      printBinaryByte(myBits);
      Serial.println();
      wSend(myBits);

    }

  }

  delay(pingDelay);                           // Don't want to flood the net with pings.
  Serial.println();
  Serial.println();

}

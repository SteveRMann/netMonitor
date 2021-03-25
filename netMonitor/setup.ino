//**************************** SETUP ****************************
void setup() {
  beginSerial();
  setup_wifi();                   // MUST be before setupMqtt()
  start_OTA();
  setup_mqtt();                   // Generate the topics

  // Call the setServer method on the PubSubClient object
  client.setServer(mqttServer, mqttPort);
  mqttConnect();



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


  blueTicker.attach(0.1, blueTick);         // start blueTick() with a fast blink while we connect
  //setup_wifi();
  blueTicker.detach();                      // Stop blueTick()
  digitalWrite(ledPin, 0);                  // Make sure the blue LED is off.

  //start_OTA();

  myBits = B11110000;                       // Reset with all red LEDs on
  wSend(myBits);


  Serial.print(F("webIpCount= "));
  Serial.println(webIpCount);



  aTimer.setTime(10000);                   // Time between IP scans
  aTimer.start();
  ipscan();                                //First scan- no delays
}

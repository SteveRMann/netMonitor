//**************************** SETUP ****************************
void setup() {
  beginSerial();

  setup_wifi();
  start_OTA();

  pinMode(blueLedPin, OUTPUT);    // set the blue LED pin on GPIO14 (D5) as output

  //Set up the MCP23008 I/O expander
  Wire.begin();                   // Creates a Wire object with the default SDA and SCL pins.
  Wire.beginTransmission(0x20);   // Begins talking to the slave device.
  Wire.write(0x00);               // Selects the IODIR register.  The IODIR is the register of all the 8 pins.
  Wire.write(0x00);               // This sets all I/O pins to outputs.
  Wire.endTransmission();         // Stops talking to device.

  const int ledOnTime = 500;      // LED On Time during startup

  // Flash all LEDs
  Serial.println(F("Flash all LEDs"));

  //Off
  wSend (B00000000);              // Set all pins low (off)
  digitalWrite(blueLedPin, 0);    // Turn the blue LEDs OFF
  delay(ledOnTime);

  //Red
  wSend(B11110000);               // All Red
  delay(ledOnTime);
  wSend(B00000000);               // All off

  //Green
  wSend(B00001111);               // All Green
  delay(ledOnTime);

  //Blue
  wSend(B00000000);               // All off
  digitalWrite(blueLedPin, 1);    // Turn the blue LEDs ON
  delay(ledOnTime);
  digitalWrite(blueLedPin, 0);    // Turn the blue LEDs OFF

  wSend(B11111111);               // All Yellow
  delay(ledOnTime);
  wSend (0);                      // All off again
  delay(ledOnTime);

  // Walk the LEDS
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


  //Cycle through all LED colors.
  wSend(0);
  delay(ledOnTime);
  for (int i = 0; i < 4; i++) {
    red(i);
    wSend(myBits);
    delay(ledOnTime);
  }
  myBits = B00000000;
  wSend(myBits);
  for (int i = 0; i < 4; i++) {
    green(i);
    wSend(myBits);
    delay(ledOnTime);
  }
  myBits = B00000000;
  wSend(myBits);
  for (int i = 0; i < 4; i++) {
    yellow(i);
    wSend(myBits);
    delay(ledOnTime);
  }
  myBits = B00000000;
  wSend(myBits);

  digitalWrite(blueLedPin, LEDOFF);           // Make sure the blue LED is off.

  //myBits = B11110000;                       // Reset with all red LEDs on
  //wSend(myBits);

  // Calculate the number of IP addresses in the array
  ipCount = sizeof(ipAddressArray) / sizeof(ipAddressArray[0]);
  Serial.print("Number of IP addrtesses: ");
  Serial.println(ipCount);

  // Calculate the number of hosts in the array
  numberOfHosts = sizeof(hosts) / sizeof(hosts[0]);
  Serial.print("Number of hosts: ");
  Serial.println(numberOfHosts);
  Serial.println(F("----------------"));


  aTimer.setTime(10000);                   // Time between IP scans
  aTimer.start();
  Serial.println(F("End of setup()"));
}

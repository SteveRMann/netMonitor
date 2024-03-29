//************************************** LOOP *******************************
void loop() {
  ArduinoOTA.handle();
  if (aTimer.ding()) {                          // If the timer has expired..
    // Serial.println(F("aTimer ding"));
    ipscan();
  }
}


void ipscan() {
  char pingIP[20];                            // IP that we're going to ping.

  //For each IP in the list
  for (int i = 0; i < 4; i++) {               // for four LEDS
    ledNum = i;

    if (i == 3) i = i + random(0, webIpCount);  // Last LED. Select an external IP at random.
    strcpy(pingIP, ipx[i]);

/*
    Serial.println();
    Serial.print(F("IP "));
    Serial.print(i);
    Serial.print(F(",  Led "));
    Serial.println(ledNum);

    Serial.print("Pinging ");
    Serial.print(pingIP);
*/ 
    greenTicker.attach(0.1, greenTick);         // Start greenTick() while we ping

    yield();                                    // Reset the WDT on the ESP8266

    if (Ping.ping(pingIP)) {                    // Ping
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, ledNum, 1);             // Turn on the green LED
      bitWrite(myBits, ledNum + 4, 0);         // Turn off the red LED
      wSend(myBits);

    } else {
      // FAIL
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, ledNum, 0);             // Turn off the green LED
      bitWrite(myBits, ledNum + 4, 1);         // Turn on the red LED
      wSend(myBits);
      pingDelay = 10;                           //Retry immediately

      Serial.println();
      Serial.print(F("Error: "));
      Serial.println(pingIP);
      Serial.print(F("LED bits: "));
      printBinaryByte(myBits);
      Serial.println();
      // if (i == 3) client.publish(statusTopic, pingIP);
      ///client.publish(statusTopic, pingIP);
    }
  }
//  Serial.println();
//  Serial.println(F("----------------------"));
  aTimer.start();                 // Restart the timer.

}

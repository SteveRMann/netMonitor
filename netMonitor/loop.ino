//************************************** LOOP *******************************
void loop() {
  ArduinoOTA.handle();

  mqttValidate();                   // Make sure we're still connected ot the MQTT broker.

  if (aTimer.ding()) {                          // If the timer has expired..
    ipscan();
  }
}



void ipscan() {
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

    Serial.print("Pinging ");
    Serial.print(pingIP);

    greenTicker.attach(0.1, greenTick);         // Start greenTick() while we ping

    yield();                                    // Reset the WDT on the ESP8266

    mqttValidate();                             // Make sure we stay connected to the mqtt broker

    if (Ping.ping(pingIP)) {                    // Ping
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 1);             // Turn on the green LED
      bitWrite(myBits, hostNum + 4, 0);         // Turn off the red LED
      wSend(myBits);

    } else {
      // FAIL
      greenTicker.detach();                     // Stop greenTick()
      bitWrite(myBits, hostNum, 0);             // Turn off the green LED
      bitWrite(myBits, hostNum + 4, 1);         // Turn on the red LED
      wSend(myBits);
      pingDelay = 10;                           //Retry immediately

      Serial.println();
      Serial.print(F("Error: "));
      Serial.println(pingIP);
      Serial.print(F("LED bits: "));
      printBinaryByte(myBits);
      Serial.println();
      // if (i == 3) client.publish(statusTopic, pingIP);
      client.publish(statusTopic, pingIP);
    }
  }
  Serial.println();
  Serial.println(F("----------------------"));
  aTimer.start();                 // Restart the timer.

}

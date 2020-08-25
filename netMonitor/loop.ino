//************************************** LOOP *******************************
void loop() {
  ArduinoOTA.handle();

  //Make sure we stay connected to the mqtt broker
  if (!client.connected()) {
    mqttConnect();
  }
  if (!client.loop()) {
    client.connect(connectName);
  }


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

    yield();                                     // Reset the WDT on the ESP8266
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
      client.publish(statusTopic, pingIP);
    }
  }  //end For

  Serial.println();
  Serial.println(F("----------------------"));
  delay(pingDelay);                           // Don't want to flood the net with pings.
  pingDelay = 10000;                          // Set delay back to default.
}

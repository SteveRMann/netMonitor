//************************************** LOOP *******************************
void loop() {
  ArduinoOTA.handle();
  if (aTimer.ding()) {                          //If the timer has expired..
    Serial.println(F("aTimer dinged"));
    ipscan();                                   //Ping the three local IP addresses
    aTimer.start();                             //Restart the timer.
  }
}


void ipscan() {
  //For each IP in the list. Because the library doesn't know .local hosts
  for (int ipNum = 0; ipNum < ipCount; ipNum++) {
    yellow(ipNum);
    wSend(myBits);
    yield();                                    //Reset the WDT on the ESP8266

    Serial.print(F("Pinging.. "));
    Serial.print(ipAddressArray[ipNum]);
    if (Ping.ping(ipAddressArray[ipNum])) {
      Serial.println(F(" OK"));
      bitWrite(myBits, ipNum, 1);               //Turn on the green LED
      bitWrite(myBits, ipNum + 4, 0);           //Turn off the red LED
      wSend(myBits);

    } else {
      Serial.println(F(" FAIL"));
      bitWrite(myBits, ipNum, 0);             // Turn off the green LED
      bitWrite(myBits, ipNum + 4, 1);         // Turn on the red LED
      wSend(myBits);

      Serial.println();
      Serial.print(F("Error: "));
      Serial.println(ipAddressArray[ipNum]);
      Serial.println();
    }

  }
}

void hostscan() {
  //For each host in the list
  for (int hostNum = 0; hostNum < numberOfHosts; hostNum++) {
    Serial.print(F("Pinging host["));
    Serial.print(hostNum);
    Serial.print(F("]= "));
    Serial.println(hosts[hostNum]);
    Serial.print(F("Pinging "));
    Serial.println(hosts[hostNum]);
    Serial.println((Ping.ping(hosts[hostNum])) ? "OK" : "fail");
  }
}

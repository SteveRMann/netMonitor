// setup_wifi
// ============================= Connect the ESP to the router =============================
// Connect to WiFi network so we can reach the MQTT broker and publish messages to topics.

/*
  Make sure you include at the start of the sketch:
  #define SKETCH_NAME "netMonitor"

  #include "ESP8266WiFi.h"        // Not needed if also using the Arduino OTA Library...
  #include <Kaywinnet.h>          // WiFi credentials

  char macBuffer[24];             // Holds the last three digits of the MAC, in hex.
  char hostName[24];              // Holds nodeName + the last three bytes of the MAC address.

*/

void setup_wifi() {
#ifndef Kaywinnet
#include <Kaywinnet.h>
#endif
  byte mac[6];                    // The MAC address of your Wifi

  Serial.println(F("\n"));
  Serial.print(F("Connecting to "));
  Serial.println(MY_SSID);
  dbugs("Connecting to ", MY_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.enableInsecureWEP();
  WiFi.begin(MY_SSID, MY_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status()); Serial.print(F(" "));
  }
  Serial.println(F("\nWiFi connected, "));
  Serial.print(F("MAC Address: "));
  Serial.println(WiFi.macAddress());
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());


  // Get the last three numbers of the mac address.
  // "4C:11:AE:0D:83:86" becomes "0D8386" in macBuffer.
  WiFi.macAddress(mac);
  snprintf(macBuffer, sizeof(macBuffer), "%02X%02X%02X", mac[3], mac[4], mac[5]);

  // Build hostName from prefix + last three bytes of the MAC address.
  strcpy(hostName, nodeName);
  strcat(hostName, "-");
  strcat(hostName, macBuffer);
  WiFi.hostname(hostName);
  dbugs("hostName= ", hostName);


  /* Some experiments
    int value = atoi(mac5);
    Serial.print("------mac5= ");
    Serial.print(value);
    Serial.println("----------");

    int numbr = stringChecksum(macBuffer);
    Serial.print("------Checksum= ");
    Serial.print(numbr);
    Serial.println("----------");

    char mac5h[3];
    sprintf(mac5h, "%x", value);
    Serial.print("------mac5h= ");
    Serial.print(mac5h);
    Serial.println("----------");
  */

}

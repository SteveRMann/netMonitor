//#define MQTT

// ================================== connectWiFi() ==================================
void connectWiFi() {
#ifndef Kaywinnet
#include "D:\River Documents\Arduino\libraries\kaywinnet.h"
#endif

  char cTemp[60];                             // Memory of local variables is freed when the function exits (unless declared "static")
  strcpy(cTemp, my_ssid);                     // Fixed the "not used" compile warnings.
  strcpy(cTemp, my_password);
  strcpy(cTemp, my_broker);
  strcpy(cTemp, mqtt_server);

  WiFi.enableInsecureWEP();
  WiFi.begin(my_ssid, my_password);
#ifndef ESP01
  Serial.print("Connecting to ");
  Serial.print(my_ssid);
  Serial.println(" ...");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
#ifndef ESP01
    Serial.print(WiFi.status()); Serial.print(' ');
#endif
  }

  long rssi = WiFi.RSSI();
#ifndef ESP01
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Strength (RSSI):");
  Serial.println(rssi);
#endif

#ifdef WiFiDebug
  // Optional information:
  long rssi = WiFi.RSSI();
#ifndef ESP01
  Serial.print(F("RSSI=       "));
  Serial.println(rssi);
  Serial.print(F("LocalIP=    "));
  Serial.println(WiFi.localIP());
  Serial.print(F("subnetMask= "));
  Serial.println(WiFi.subnetMask());
  Serial.print(F("gatewayIP=  "));
  Serial.println(WiFi.gatewayIP());
  Serial.print(F("dnsIP=      "));
  Serial.println(WiFi.dnsIP());
  Serial.println();
#endif
#endif

}


#ifdef MQTT
// ================================== mqttConnect() =================================
// Connect or Reconnect to MQTT broker
void mqttConnect() {
  // Loop until we're Connected
  while (!client.connected()) {
#ifndef ESP01
    Serial.print(F("Attempting MQTT connection..."));
#endif

    // Attempt to connect
    if (client.connect(connectName)) {
#ifndef ESP01
      Serial.println(F("connected"));
#endif

      client.setCallback(callback);

      //Subscribe or resubscribe to a topic
      client.subscribe(incomingTopic);
      client.subscribe(cmndTopic);
      client.subscribe(timeTopic);
      client.subscribe(temperatureTopic);
#ifndef ESP01
      Serial.print(F("Subscribing to "));
      Serial.println(incomingTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(cmndTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(timeTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(temperatureTopic);
      Serial.println();
#endif

    } else {
#ifndef ESP01
      Serial.print(F("failed, rc="));
      Serial.print(String(client.state()));
      Serial.println(F(" try again in 5 seconds"));
#endif
      delay(5000);
    }
  }
}




// ==================================  mqtt callback ==================================
// This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
// The MQTT payload is the filename of the message to play when the phone is picked up.  The payload is case-sensitive.
//
void callback(String topic, byte * payload, unsigned int length) {
  char message[length + 1];

  // copy contents of payload to message
  // convert the payload from a byte array to a char array
  memcpy(message, payload, length);
  message[length] = '\0';                 // add NULL terminator to message

  // Sometimes in the MQTT tool, I accdentally hit "Enter" on my keyboard.
  // This removes it.
  for (i = 0; i == strlen(message); i++) {
    if (message[i] == 10) {
      message[i] = '\0';
      break;
    }
  }


#ifdef DEBUGM
  Serial.println();
  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.print(topic);
  Serial.println(F("."));

  Serial.print("message: ");
  Serial.println(message);
  Serial.print(F("Length= "));
  Serial.print(strlen(message));
  Serial.println();

  // If the message terminates in a line-feed, make it the terminating null char.
  int j = strlen(message) - 1;
  if (message[j] == 10) message[j] = '\0';
#endif

  // --------- Incoming Call ---------
  if (topic == incomingTopic) {               // Incoming call
#ifdef DEBUGM
    Serial.println();
    Serial.print(F("Incoming call: "));
    Serial.println(message);
#endif
    strcpy(incomingMessage, message);
    // Add the file suffix, .wav, if one is not present
    if (!strchr(incomingMessage, 46)) strcat(incomingMessage, ".wav");
    ringFlag = true;                          // Start ringing the phone
  }



  // --------- Command ---------
  if (topic == cmndTopic) {                    // Process incoming commands (V8.0.5)
    //Serial.print(F("received message on cmdTopic: '"));
    //Serial.print(message);
    //Serial.println(F("'"));

    if (!strcmp(message, "hangup")) {          // if message=="hangup", then strcmp returns a zero (false).
      //Terminate the incoming call
      Serial.println(F("hanging up"));
      Serial.println();
      ringFlag = false;
      ringOFF();                               // De-energizes both bell coils

      //shieldRawLow = 1024;                         // Temp- reset the raw reads
      //shieldRawHigh = 0;

    }

    // if message=="badfile", then a bad file is sent to the Uno.
    // This raises the badFile pin on the UNO.
    if (!strcmp(message, "badfile")) {
      sendWav("badFile");
    }

    if (!strcmp(message, "reset")) {          // if message=="reset", then strcmp returns a zero (false).
      ringFlag = false;
      ringOFF();                              // De-energizes both bell coils
      shieldRawLow = 1024;                    // reset the raw reads
      shieldRawHigh = 0;
      shieldError = false;
      filePlaying = false;
    }
  }



  // ----- Time -----
  if (topic == timeTopic) {                    // Process incoming commands (V8.0.5)
    strcpy(popcornTime, message);      // Save the time in case popcorn needs it later.
  }

  // ----- temperatureTopic -----
  if (topic == temperatureTopic) {
    strcpy(wxTemperature, message);      // Save the temperature for future use.

  }


}           //callback
#endif

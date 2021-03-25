
/* ================================== mqttConnect() =================================
  Include at the top of the main ino file:

  //--------------- mqtt declarations ---------------
  #include <ESP8266WiFi.h>        // Connect (and reconnect) an ESP8266 to the a WiFi network.
  #include <PubSubClient.h>       // connect to a MQTT broker and publish/subscribe messages in topics.
  // Declare an object of class WiFiClient, which allows to establish a connection to a specific IP and port
  // Declare an object of class PubSubClient, which receives as input of the constructor the previously defined WiFiClient.
  // The constructor MUST be unique on the network. (Does it?)
  WiFiClient xyzzy;
  PubSubClient client(xyzzy);

  // Declare strings for the topics. Topics will be created in setup_mqtt().
  char statusTopic[20];
  char cmndTopic[20];                           // Incoming commands, payload is a command.
  // Other topics as needed

  const char *mqttServer = MQTT_SERVER;         // Local broker defined in Kaywinnet.h
  const int mqttPort = 1883;

  char nodeName[] = SKETCH_NAME;  // Give this node a name

  const char *mqttServer = MQTT_SERVER;         // Local broker defined in Kaywinnet.h
  const int mqttPort = 1883;
  //-------------------------------------------------



  // --------------- Example setup: ---------------
  void setup() {
    beginSerial();
    setup_wifi();                   // MUST be before setupMqtt()
    start_OTA();                    // Ifusing OTA
    setup_mqtt();                   // Generate the topics

    // Call the setServer method on the PubSubClient object
    client.setServer(mqttServer, mqttPort);
    mqttConnect();
  //-------------------------------------------------



  //----------
  //IN LOOP()
  mqttValidate();         //Make sure we stay connected to the mqtt broker

*/



// ==================================  setup_mqtt ==================================
// Create topic names
void setup_mqtt() {
  //MUST follow setupWiFi()
  strcpy(cmndTopic, nodeName);
  strcat(cmndTopic, "/cmnd");             // Incoming commands, payload is a command.
  strcpy(statusTopic, nodeName);
  strcat(statusTopic, "/stat");

  //Topics
  dbugs("cmndTopic= ", cmndTopic);
  dbugs("statusTopic= ", statusTopic);
  
}


// ==================================  mqttValidate ==================================
void mqttValidate() {
  //Make sure we stay connected to the mqtt broker
  if (!client.connected()) {
    mqttConnect();
  }
  if (!client.loop()) {
    client.connect(hostName);
  }
}

// ==================================  mqttConnect ==================================
void mqttConnect() {
  while (!client.connected()) {
    Serial.print(F("MQTT connecting..."));
    if (client.connect(hostName)) {
      Serial.println(F("connected"));

      //Subscriptions:
      client.subscribe(cmndTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(cmndTopic);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("- trying again in 5-seconds."));
      delay(5000);
    }
  }
}



// ==================================  mqtt callback ==================================
// This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
// The MQTT payload is the filename of the message to play when the phone is picked up.  The payload is case-sensitive.
//
void callback(String topic, byte * message, unsigned int length) {

  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.println(topic);


  // Convert the character array to a string
  String messageString;
  for (unsigned int i = 0; i < length; i++) {
    messageString += (char)message[i];
  }
  messageString.trim();
  messageString.toUpperCase();          //Make the string upper-case


  Serial.print("messageString: ");
  Serial.print(messageString);
  Serial.println();



  if (topic == cmndTopic) {
    //Handle the command
  }

} //callback

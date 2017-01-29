#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "OneWire.h"
#include "DallasTemperature.h"

/************************* 1Wire Setup *********************************/
#define ONE_WIRE_BUS  13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp = 0;

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "42"
#define WLAN_PASS       "raspunsul"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.1.108"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "mariusene"
#define AIO_KEY         "1234"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//const char PHOTOCELL_FEED[] PROGMEM = "/esp8266/feeds/photocell";
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, PHOTOCELL_FEED, MQTT_QOS_1);
const char TEMP_FEED[] PROGMEM = "hass/bath/temp";
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, TEMP_FEED, MQTT_QOS_1);

const char LED_FEED[] PROGMEM = "hass/led/state";
Adafruit_MQTT_Publish ledFeed = Adafruit_MQTT_Publish(&mqtt, LED_FEED, MQTT_QOS_1);


// Setup a feed called 'onoff' for subscribing to changes.
const char ONOFF_FEED[] PROGMEM = "hass/led/command";
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, ONOFF_FEED, MQTT_QOS_1);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  // Set GPIO#0 as output
  pinMode(0, OUTPUT);
  
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      char * receivedSubscription = (char *)onoffbutton.lastread;
      Serial.print(F("Got: "));
      Serial.println(receivedSubscription);

      if(0 == strncmp("on",receivedSubscription,2))
      {
        digitalWrite(0, LOW);
        if (! ledFeed.publish("on")) {
          Serial.println(F("Failed LED ON publish"));
        } else {
          Serial.println(F("OK LED ON publish!"));
        }
      }
      else if(0 == strncmp("off",receivedSubscription,3))
      {
        digitalWrite(0, HIGH);
        if (! ledFeed.publish("off")) {
          Serial.println(F("Failed LED OFF publish"));
        } else {
          Serial.println(F("OK LED OFF publish!"));
        }
      } 
      else
      {
        Serial.print("Error, received: ");
        Serial.println(receivedSubscription);
      } 
    }
  }

  /* Temperature read */
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(temp);
  
  // Now we can publish stuff!
  Serial.print(F("\nSending temp val "));
  Serial.print(temp);
  Serial.print("...");
  if (! tempFeed.publish(temp)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  
  /*ESP.deepSleep(30000000);*/    // Sleep for 30 seconds
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

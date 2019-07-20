#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//
//const char* ssid = "sairam 123";
//const char* password = "1234567j";

const char* ssid = "HACKATHON";
const char* password = "12345678";
//Setting up the wifi using personal hotspot
char* topic = "gadhala12345";
//char * server = "ws://iot.eclipse.org/mqtt";
char* server = "iot.eclipse.org";
//char* server = "broker.mqtt-dashboard.com";
float RSSI=10;
int ID=1;


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

//PubSubClient client(server, 8000 , callback, wifiClient);

WiFiClient wifiClient;
//WiFiClient.setmode(WiFiClient.STATION);
PubSubClient client(server, 1883 , callback, wifiClient);
//client.setServer(server, 8000);
//client.setCallback(callback);

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
//End of setting up wifi
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN BUILTIN_LED // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
   Serial.begin(115200);
  //Establishing Connection using ssid and password
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

//  WiFi.begin(ssid, password);
  WiFi.begin(ssid, NULL);
  //WiFi.setMode(WiFi.STATION);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //end of establishing connections

  // Generate client name based on MAC address and last 8 bits of microsecond RSSI
  String clientName;
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  //Connecting to the MQTT Server :
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);

    if (client.publish(topic, "hello from NODEMCU")) {
      Serial.println("Client Published !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  // end of connecting to MQTT server

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

//    RSSI = (RSSI+1)%25;
    String payload=String(ID)+":"+String(RSSI);
    if (client.connected()) {
      Serial.print("Sending payload: ");
      Serial.println(payload);

      if (client.publish(topic, (char*) payload.c_str())) {
        Serial.println("Publish ok");
      }
      else {
        Serial.println("Publish failed");
      }
    }
    delay(1000);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}

/* The Code is developed by Alok Shaw (alokom78@gmaail.com) and is free to use and modify. Watch the full explanation for the same on 
 *  
*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>4  
//const char* ssid = "ard";
//const char* password = "hibrohowru";

const char* ssid = "B-Block Innovation";
const char* password = "sairam@123";

//const char* ssid = "B-Block Innovation";
//const char* password = "sairam@123";

static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps; // The TinyGPS++ object
//WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget                                             

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device;

BlynkTimer timer;

float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS                                   // Corresponding Password


//Setting up the wifi using personal hotspot
char* topic = "gadhala12345";
//char * server = "ws://iot.eclipse.org/mqtt";
char* server = "iot.eclipse.org";
//char* server = "broker.mqtt-dashboard.com";
int ID=1;
float latitude=-1.0,longitude=-1.0;
long seqCounter=0;

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
//wifiClient.setmode(
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


void setup()
{
  Serial.begin(115200);
  Serial.println();
  ss.begin(GPSBaud);
  //timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
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

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
}

void loop()
{
 
    if(ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read())){
        displayInfo();
      }
      else{
        Serial.println("Error !!");
      }
  }
  timer.run();
  timer.setInterval(5000L, checkGPS);
}

void displayInfo()
{ 
    String payload;
    if (client.connected()) {
      if (gps.location.isValid() ) 
      {
        latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
        longitude = (gps.location.lng()); 
        
        Serial.print("LAT:  ");
        Serial.println(latitude, 6);  // float to x decimal places
        Serial.print("LONG: ");
        Serial.println(longitude, 6);  
        payload=String(ID)+":"+String(latitude,6)+":"+String(longitude,6);
      }
      else{
        payload=String(ID)+":"+String(latitude,6)+":"+String(longitude,6);
      }
      payload+=":"+String(seqCounter);
      if (client.publish(topic, (char*) payload.c_str())) {
        Serial.print("Sending payload: ");
        Serial.println(payload);
        Serial.println("Publish ok");
        seqCounter++;
      }
      else {
        Serial.println("Publish failed");
      }
    }
    else{
      Serial.print("Client not connected !!! ");
      abort();
    }
    delay(250);
}

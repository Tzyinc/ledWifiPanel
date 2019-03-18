#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>
#include <ArduinoJson.h>


// HARDWARE CONNECTION
const int ONBOARD_LED = 2;
const int DOUT = 6;
const int NUM_LEDS = 256;
CRGB leds[NUM_LEDS];

// Internet settings
const char* DEFAULT_SSID = "";
const char* DEFAULT_PW = "";
DynamicJsonDocument doc(2048);

// state constants
const int INIT = 0;
const int CONNECTED = 1;

int red = 0;
int redDir = 1;
int green = 1;
int greenDir = 1;
int blue = 0;
int blueDir = 1;
const int CAP = 10;

char packetBuffer[255]; //buffer to hold incoming packet
int networkState = INIT;

void setup() {
  pinMode(ONBOARD_LED, OUTPUT);
  FastLED.addLeds<WS2812, DOUT, GRB>(leds, NUM_LEDS);

  Serial.begin(115200);
  while(!Serial);
}

void loop() {
  if (networkState == INIT) {
    initSequence();
  } else if (networkState == CONNECTED) {
    controlSequence();
  }
}

void initSequence() {
  connect();
}

// TODO: exit condition
void connect() {
  WiFi.begin(DEFAULT_SSID, DEFAULT_PW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(ONBOARD_LED, true);
    delay(250);
    digitalWrite(ONBOARD_LED, false);
    Serial.print(".");
    //TODO: some exit condition (timeout?)
  }
  networkState = CONNECTED;
}

void controlSequence() {
  delay(500);
  digitalWrite(ONBOARD_LED, false);
//  Serial.print(red);
//  Serial.print(green);
//  Serial.print(blue);
//  Serial.println("");
  for (int curr = 0; curr < NUM_LEDS; curr += 1) {
    leds[curr] = CRGB(0, 0, 0);
  }
  FastLED.show();
  fetchHttp();
}

void fetchHttp() {
   if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin("http://jsonplaceholder.typicode.com/users/1");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload

      // deserialise json objecto for use
      JsonObject root = doc.as<JsonObject>();
      deserializeJson(doc, payload);
    
      // Fetch values.
      //
      // Most of the time, you can rely on the implicit casts.
      // In other case, you can do root["time"].as<long>();
      const char* sensor = root["username"];
      Serial.println(sensor);
 
    }
 
    http.end();   //Close connection
 
  }
}



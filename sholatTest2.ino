#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "NTPClient.h"
#include "WiFiUdp.h"

#include <ArduinoJson.h>

//const char* ssid = "KUN_ANTA";
//const char* password = "P@$$w0rd.!";

const char* ssid = "real";
const char* password = "P@ssw0rd.";

const unsigned long utcOffsetInSeconds = 25200;

const char* SERVER_NAME = "http://api.aladhan.com/v1/timingsByCity?city=Jakarta&country=Indonesia";
//const char* RESOURCE_NAME = "/v1/timingsByCity?city=Jakarta&country=Indonesia";

//const char* SERVER_NAME = "arduinojson.org";
//const char* RESOURCE_NAME = "/example.json";

unsigned long previousAthan = 0;
const unsigned long PERIOD_ATHAN = 5000;

  
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void setSholat() {
  // Send an HTTP POST request depending on PERIOD_CLOCK
  if (millis() - previousAthan >= PERIOD_ATHAN) {
    previousAthan = millis();
    
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      
      WiFiClient client;
      client.setTimeout(10000);
      HTTPClient http;
    
      // Your IP address with path or Domain name with URL path 
      http.begin(client, SERVER_NAME);
      
      // Send HTTP POST request
      int httpResponseCode = http.GET();
      
      String payload = "{}"; 
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
      Serial.print("payload :");
      Serial.println(payload);

      
      StaticJsonDocument<2048> doc;
      
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      
      int code = doc["code"]; // 200
      const char* status = doc["status"]; // "OK"
      
      JsonObject data = doc["data"];
      
      JsonObject data_timings = data["timings"];
      const char* data_timings_Fajr = data_timings["Fajr"]; // "04:38"
      const char* data_timings_Sunrise = data_timings["Sunrise"]; // "05:57"
      const char* data_timings_Dhuhr = data_timings["Dhuhr"]; // "12:07"
      const char* data_timings_Asr = data_timings["Asr"]; // "15:21"
      const char* data_timings_Sunset = data_timings["Sunset"]; // "18:16"
      const char* data_timings_Maghrib = data_timings["Maghrib"]; // "18:16"
      const char* data_timings_Isha = data_timings["Isha"]; // "19:28"
      const char* data_timings_Imsak = data_timings["Imsak"]; // "04:28"
      const char* data_timings_Midnight = data_timings["Midnight"]; // "00:07"

      Serial.print("Fajr : ");
      Serial.println(data_timings_Fajr);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

void loop() {
  setSholat();
}

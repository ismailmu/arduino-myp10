#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "NTPClient.h"
#include "WiFiUdp.h"

#include <ArduinoJson.h>

const char* ssid = "KUN_ANTA";
const char* password = "P@$$w0rd.!";

//const char* ssid = "real";
//const char* password = "P@ssw0rd.";

const unsigned long utcOffsetInSeconds = 25200;

const char* SERVER_NAME = "http://api.aladhan.com/v1/timingsByCity?city=Jakarta&country=Indonesia";

unsigned long previousClock = 0;
unsigned long previousAthan = 0;

const unsigned long PERIOD_CLOCK = 1000;
const unsigned long PERIOD_ATHAN = 1000 * 3600; //1 jam

unsigned int hh_imsak = 0;
unsigned int mm_imsak = 0;

unsigned int hh_fajar = 0;
unsigned int mm_fajar = 0;

unsigned int hh_zhuhur = 0;
unsigned int mm_zhuhur = 0;

unsigned int hh_ashar = 0;
unsigned int mm_ashar = 0;

unsigned int hh_magrib = 0;
unsigned int mm_magrib = 0;

unsigned int hh_isya = 0;
unsigned int mm_isya = 0;

unsigned int dd = 0; 
unsigned int hh = 0; 
unsigned int mm = 0;
unsigned int ss = 0;

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

void checkAthan() {
  //if imsak
  //if fajar
  //if shubuh
  //if zhuhur
  //if ashar
  //if magrib
  //if isya
  if ( (hh == hh_isya) && (mm = mm_isya) && (ss == 0) ) {
    Serial.println("Saatnya sholat isya");
  }
}

void setAthan() {
  
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
      }else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return;
      }
      
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
      String data_timings_Fajr = data_timings["Fajr"]; // "04:38"
      //String data_timings_Sunrise = data_timings["Sunrise"]; // "05:57"
      String data_timings_Dhuhr = data_timings["Dhuhr"]; // "12:07"
      String data_timings_Asr = data_timings["Asr"]; // "15:21"
      String data_timings_Sunset = data_timings["Sunset"]; // "18:16"
      String data_timings_Maghrib = data_timings["Maghrib"]; // "18:16"
      String data_timings_Isha = data_timings["Isha"]; // "19:28"
      String data_timings_Imsak = data_timings["Imsak"]; // "04:28"
      //String data_timings_Midnight = data_timings["Midnight"]; // "00:07"

      hh_fajar = data_timings_Fajr.substring(0,2).toInt();
      mm_fajar = data_timings_Fajr.substring(3).toInt();

      hh_zhuhur = data_timings_Dhuhr.substring(0,2).toInt();
      mm_zhuhur = data_timings_Dhuhr.substring(3).toInt();

      hh_ashar = data_timings_Asr.substring(0,2).toInt();
      mm_ashar = data_timings_Asr.substring(3).toInt();

      hh_magrib = data_timings_Maghrib.substring(0,2).toInt();
      mm_magrib = data_timings_Maghrib.substring(3).toInt();

      hh_isya = data_timings_Isha.substring(0,2).toInt();
      mm_isya = data_timings_Isha.substring(3).toInt();

      hh_imsak = data_timings_Imsak.substring(0,2).toInt();
      mm_imsak = data_timings_Imsak.substring(3).toInt();

      //pantek untuk testing
      hh_isya = 20;
      mm_isya = 47;

      Serial.print("data_timings_Imsak : ");
      Serial.println(data_timings_Imsak);
      Serial.print("data_timings_Fajr : ");
      Serial.println(data_timings_Fajr);
      Serial.print("data_timings_Dhuhr : ");
      Serial.println(data_timings_Dhuhr);
      Serial.print("data_timings_Asr : ");
      Serial.println(data_timings_Asr);
      Serial.print("data_timings_Maghrib : ");
      Serial.println(data_timings_Maghrib);
      Serial.print("data_timings_Isha : ");
      Serial.println(data_timings_Isha);
      //Serial.println(hh_fajar);
      //Serial.println(mm_fajar);

      checkAthan();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

void setTime() {
  if (millis() - previousClock >= PERIOD_CLOCK) {
    previousClock = millis();
  
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);
    timeClient.update();
    dd = timeClient.getDay();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    
    checkAthan();
  }
}

void showPrayerTime() {
  
}

void loop() {
  setTime();
  setAthan();
  showPrayerTime();
}

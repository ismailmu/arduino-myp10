/*
   Contoh penggunaan library DMDESP

   email : bonny@grobak.net - www.grobak.net - www.elektronmart.com
*/

#include "NTPClient.h"
#include "WiFiUdp.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ArduinoJson.h>

#include <DMDESP.h>
#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/EMSans5x6.h>
#include <fonts/EMSans6x8.h>


//SETUP DMD
#define DISPLAYS_WIDE 2 // Kolom Panel
#define DISPLAYS_HIGH 1 // Baris Panel
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)

const unsigned long utcOffsetInSeconds = 25200;

const unsigned int POS_COL_1 = 0;
const unsigned int POS_COL_2 = 8;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

const char *ssid     = "KUN_ANTA";
const char *password = "P@$$w0rd.!";

const char *host = "http://arduinojson.org/example.json"; //"https://api.aladhan.com/v1/timingsByCity?city=Jakarta&country=indonesia";

String sepClock = ":";

unsigned long hh, mm = 0;
const unsigned long PERIOD_CLOCK = 1000;
unsigned long previousClock = 0;

void getAthan() {
  /*
  HTTPClient http;    //Declare object of class HTTPClient
  
  Serial.print("Request Link:");
  Serial.println(host);
  
  http.begin(host);     //Specify request destination
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server
  
  Serial.print("Response Code:"); //200 is OK
  Serial.println(httpCode);   //Print HTTP return code
  
  Serial.print("Returned data from Server:");
  Serial.println(payload);    //Print request response payload
  
  if(httpCode == 200)
  {
    // Allocate JsonBuffer
    // Use arduinojson.org/assistant to compute the capacity.
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
    DynamicJsonDocument doc(capacity);
    
    // Parse JSON object
    // Parse JSON object
    DeserializationError error = deserializeJson(doc, http);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      http.end();
      return;
    }
  
    // Decode JSON/Extract values
    Serial.println(F("Response:"));
    Serial.println(doc["sensor"].as<char*>());
    Serial.println(doc["time"].as<char*>());
    Serial.println(doc["data"][0].as<char*>());
    Serial.println(doc["data"][1].as<char*>());
  }
  else
  {
    Serial.println("Error in response");
  }
  
  http.end();  //Close connection
  
  delay(5000);  //GET Data at every 5 seconds
  */
}

void showClock() {
  Disp.setFont(EMSans5x6);
  unsigned long currenctClock = millis();
  timeClient.update();

  if (currenctClock - previousClock >= PERIOD_CLOCK) {
    previousClock = currenctClock;
    String dt;
    int dy = timeClient.getDay();
    switch(dy) {
      case 0:
        dt = "Ahad";
        break;
      case 1:
        dt = "Senin";
        break;
      case 2:
        dt = "Selasa";
        break;
      case 3:
        dt = "Rabu";
        break;
      case 4:
        dt = "Kamis";
        break;
      case 5:
        dt = "Jumat";
        break;
      default:
        dt = "Sabtu";
        break;
    }
    Disp.drawText(Disp.width() - dt.length() - 20, POS_COL_2, dt);

    hh = timeClient.getHours();
    String hhStr;

    if (hh < 10) {
      hhStr = "0";
    }
    hhStr += String(hh);
    Disp.drawText(35, POS_COL_1, hhStr);
    if (sepClock.equalsIgnoreCase(":")) {
      sepClock = "";
    } else {
      sepClock = ":";
    }
    Disp.drawText(47, POS_COL_1, sepClock);

    mm = timeClient.getMinutes();
    String mmStr;
    if (mm < 10) {
      mmStr = "0";
    }
    mmStr += String(mm);

    Disp.drawText(50, POS_COL_1, String(mmStr));
  }
}

void showSscrollingText(int y, uint8_t kecepatan, char *teks[]) {

  static uint32_t pM;
  static uint32_t x;
  int width = Disp.width();
  Disp.setFont(EMSans6x8);
  int fullScroll = Disp.textWidth(teks[0]) + width;
  if ((millis() - pM) > kecepatan) {
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      return;
    }
    Disp.drawText(width - x, y, teks[0]);
  }

}

void setup() {

  Serial.begin(115200);

  // DMDESP Setup
  Disp.start(); // Jalankan library DMDESP
  Disp.setBrightness(20); // Tingkat kecerahan

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 1000 );
    char *teks[] = {"Connecting ..."};
    showSscrollingText(POS_COL_2, 150, teks);
  }
  Disp.clear();

  timeClient.begin();
  getAthan();
}

void loop() {
  Disp.loop(); // Jalankan Disp loop untuk refresh LED
  showClock();
}

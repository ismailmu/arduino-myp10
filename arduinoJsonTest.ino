#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

const char* ssid     = "KUN_ANTA";
const char* password = "P@$$w0rd.!";

//const char* SERVER = "api.openweathermap.org";
//const char* SERVER = "https://api.pray.zone";
const char* SERVER = "api.aladhan.com";

//const char* RESOURCE = "/data/2.5/weather?q=Jakarta&appid=bd7743211677cc27dfe30cf7460aa32e";
//const char* RESOURCE = "/v2/times/today.json?city=jakarta";
const char* RESOURCE = "/v1/calendarByCity?city=Jakarta&country=Indonesia&month=01&year=2022";

const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server

WiFiClient client;

bool connect() {
  Serial.print("Connect to ");
  Serial.println(SERVER);
  
  bool ok = client.connect(SERVER, 80);
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool sendRequest() {
  Serial.print("GET ");
  Serial.println(RESOURCE);

  client.print("GET ");
  client.print(RESOURCE);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(SERVER);
  client.println("Connection: close");
  client.println();

  return true;
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
  /*char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    //client.stop();
    return false;
  }
  */
  
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
    //client.stop();
  }
  return ok;
}

bool readReponseContent() {
  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //DynamicJsonDocument doc(1024);
  DynamicJsonDocument doc(49152);
  
  char ch = static_cast<char>(client.read());
  Serial.println(ch);

  /*
  DeserializationError error = deserializeJson(doc, ch);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  JsonObject results = doc["results"];
  JsonObject results_datetime_0_times = results["datetime"][0]["times"];
  
  Serial.print("data : ");
  const char* imsak = results_datetime_0_times["Imsak"];
  const char* sunrise = results_datetime_0_times["Sunrise"];
  Serial.println(imsak);
  Serial.println(sunrise);
  */
  // Here were copy the strings we're interested in using to your struct data
  //strcpy(clientData->fajr, doc["data"]["timings"]["Fajr"]);
  // It's not mandatory to make a copy, you could just use the pointers
  // Since, they are pointing inside the "content" buffer, so you need to make
  // sure it's still in memory when you read the string

  return true;
}

// Print the data extracted from the JSON
void printclientData(const struct clientData* clientData) {
  //Serial.print("Fajr = ");
  //Serial.println(clientData->fajr);
}

// Close the connection with the HTTP server
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}

void wait() {
  Serial.println("Wait 60 seconds");
  delay(60000 * 1);
}

void getPrayerTime() {
  if(connect()) {
    if(sendRequest() && skipResponseHeaders()) {
      readReponseContent();
    }
  }
  disconnect();
  wait();
}
void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  getPrayerTime();

}

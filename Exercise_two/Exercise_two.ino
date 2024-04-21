#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>
#include "DHT.h"

const char* ssid = "Vux";
const char* password = "opal18032001";
const char* serverUrl = "http://192.168.7.144:8080/api/lightconbuom";


#define DHTPIN D5
#define led1 D6
#define led2 D7
#define led3 D8
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter(0x23);

WiFiClient client;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  float lightIntensity = lightMeter.readLightLevel();

  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["light"] = lightIntensity;
  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode == 200) {
    String response = http.getString();
    DynamicJsonDocument json(200);
    deserializeJson(json, response);

    int numLights = json["data"]["light"];

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    Serial.println(response);
  } else {
    Serial.print("HTTP POST request failed, error code: ");
  }

  http.end();
  delay(5000);
}

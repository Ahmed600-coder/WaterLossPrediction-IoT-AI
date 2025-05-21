#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

volatile int pulseCount = 0;

void IRAM_ATTR countPulse() {
  pulseCount++;
}

const char* ssid = "bzeouich";       // Nom du réseau Wi-Fi
const char* password = "Aa123456789"; 
const char* serverUrl = "http://192.168.218.214:5000/receive";

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP); // وصّل السلك الأصفر لـ D2
  attachInterrupt(digitalPinToInterrupt(D2), countPulse, FALLING);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté !");
}

void loop() {
  float flowRate = pulseCount / 7.5;
  Serial.print("Débit: ");
  Serial.print(flowRate);
  Serial.println(" L/min");



  if (WiFi.status() == WL_CONNECTED) {
     // Lit jusqu'au saut de ligne

    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverUrl); 
    http.addHeader("Content-Type", "application/json");
    // Créez le payload JSON
    StaticJsonDocument<200> doc;
    doc["sensorB"] = "flow B";
    doc["valueB"] = String(flowRate);
    doc["timestampB"] = millis();
    
    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println("Réponse : " + response);
    } else {
      Serial.println("Erreur HTTP : " + String(httpCode));
    }
    http.end();
  } 
  pulseCount = 0; 
 delay(1000);
}
#include "wifi.h"
#include <Energia.h>
#include <WiFi.h>

void connectWifi(char* ssid, char* password) {	
  Serial.print("Connecting to WIFI network ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println(" connected!");
  Serial.print("Waiting for an IP address ");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println(" received!");
}
#include <Wire.h>
#include <WiFi.h>
#include "accelerometer.h"
#include "wifi.h"
#include "http.h"

#define WLAN_PUB_SSID "YourWifiSsid"
#define WLAN_KEY_CODE "YourWifiPasscode"

void setup() {
  Serial.begin(9600);
  Wire.begin();
  connectWifi(WLAN_PUB_SSID, WLAN_KEY_CODE);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  AccData acc = readAccelerometer();
  Serial.println(acc.z);

  if (acc.z > 26) {
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(RED_LED, HIGH);  
  }

  httpGetRequest("httpbin.org", "/bytes/4");
  delay(30000);
}

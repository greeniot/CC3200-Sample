#include <Wire.h>
#include <WiFi.h>
#include "accelerometer.h"
#include "wifi.h"
#include "http.h"
#include "time.h"

#define WLAN_PUB_SSID "YourWifiSsid"
#define WLAN_KEY_CODE "YourWifiPasscode"

void setup() {
  Serial.begin(9600);
  Wire.begin();
  connectWifi(WLAN_PUB_SSID, WLAN_KEY_CODE);
  pinMode(RED_LED, OUTPUT);
  setCurrentTime();
}

void loop() {
  AccData acc = readAccelerometer();
  Serial.println(acc.z);

  if (acc.z > 26) {
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(RED_LED, HIGH);  
  }

  httpsGetRequest("www.random.org", "/integers/?num=1&min=1&max=100&col=1&base=10&format=plain&rnd=new", "/cert/root.pem");
  delay(30000);
}

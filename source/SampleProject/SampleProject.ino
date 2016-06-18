#include <Wire.h>
#include <WiFi.h>
#include "accelerometer.h"
#include "wifi.h"
#include "http.h"
#include "time.h"

#define WLAN_PUB_SSID "YourWifiSsid"
#define WLAN_KEY_CODE "YourWifiPasscode"

#define RNG_HOST "www.random.org"
#define RNG_PATH "/integers/?num=1&min=1&max=100&col=1&base=10&format=plain&rnd=new"
#define RNG_CERT "/cert/root.pem"

void setup() {
  Serial.begin(9600);
  Wire.begin();
  connectWifi(WLAN_PUB_SSID, WLAN_KEY_CODE);
  pinMode(RED_LED, OUTPUT);
  setCurrentTime();
}

int extractNumber(char* response) {
  int length = strlen(response);
  int lastNewLine = length - 2;

  while (lastNewLine > 0 && response[lastNewLine--] == '\n');

  while (lastNewLine > 0) {
    if (response[lastNewLine - 1] == '\n') {
      break;
    }

    --lastNewLine;
  }

  return atoi(response + lastNewLine);
}

void loop() {
  char receive_msg_buffer[1024];
  httpsGetRequest(RNG_HOST, RNG_PATH, RNG_CERT, receive_msg_buffer);
  int number = extractNumber(receive_msg_buffer);

  for (int i = 0; i < number; ++i) {
    digitalWrite(RED_LED, HIGH);
    delay(400);
    digitalWrite(RED_LED, LOW);
    delay(200);
  }

  delay(1800);
}

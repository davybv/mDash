// Minimal Arduino sketch for mdash.net
//
//  - Install mDash library:
//    - Select "Sketch" &rarr; "Include Library" &rarr; "Manage Libraries"
//    - In the search field, type "mDash" and press Enter
//    - Click on "Install" to install the library
//  - Select "Tools" → "Board" → "ESP32 Dev Module"
//  - Select "Tools" → "Partitioning Scheme" → "Minimal SPIFFS"
//  - Select "Tools" → "Port" → your serial port
//  - Click on "Upload" button to build and flash the firmware

#define MDASH_APP_NAME "myApp"
#include <mDash.h>

#include <WiFi.h>

#define WIFI_NETWORK "MyWifiNetworkName"
#define WIFI_PASSWORD "MyWifiPassword"
#define DEVICE_PASSWORD "mDashDeviceToken"

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  mDashStart(DEVICE_PASSWORD);
}

void loop() {
  delay(100);
}

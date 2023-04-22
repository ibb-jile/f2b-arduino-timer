#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <WiFiUdp.h>

HTTPUpdateServer httpUpdater;
WiFiUDP Udp;

const char* ssid = "HOUSENKA";
const char* password = "medvidekPu";
const char* ssidAP = "F2B";
const char* passwordAP = "f2bpasswd";
const int udpPort = 8888;

void startServer() {
  server.enableCORS(true);
  httpUpdater.setup(&server);
  server.begin();
}

void setupWifi() {
  delay(10000);
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  byte counter = 0;
  /*while (WiFi.status() != WL_CONNECTED && counter < 20) {
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Connection failed");
      WiFi.begin(ssid, password);
    }
    delay(500);
    Serial.print(".");
    delay(500);
    counter++;
  }*/

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot connect to " + String(ssid));
    Serial.println("Configuring access point... SSID: " + String(ssidAP) + ", Password: " + String(passwordAP));
    while (!WiFi.softAP(ssidAP, passwordAP)) {
      Serial.print(".");
    }

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  } else {
    Serial.println("");
    Serial.print("Pico W is connected to WiFi network ");
    Serial.println(WiFi.SSID());

    Serial.print("Assigned IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void shareOnUdpPort(String data) {
  int str_len = data.length() + 1;
  char char_array[str_len];

  // Copy it over
  data.toCharArray(char_array, str_len);
  IPAddress ip;
  ip.fromString("<192.168.42.255");
  Udp.beginPacketMulticast(ip, 8888, WiFi.localIP());
  Udp.write(char_array);
  Udp.endPacket();
}

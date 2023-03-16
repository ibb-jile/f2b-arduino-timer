#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
HTTPUpdateServer httpUpdater;

const char* ssid = "F2B";
const char* password = "f2bpasswd";

void startServer() {
  server.enableCORS(true);
  httpUpdater.setup(&server);
  server.begin();
}

void setupWifi() {
  delay(10000);
  Serial.println("Configuring access point...");
  while(!WiFi.softAP(ssid, password)) {
    Serial.print(".");
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void shareOnUdpPort(String data) {
  int str_len = data.length() + 1;
  char char_array[str_len];

  // Copy it over
  data.toCharArray(char_array, str_len);
  IPAddress ip;
  ip.fromString("192.168.42.17");
  Udp.beginPacketMulticast(ip, 8888, WiFi.localIP());
  Udp.write(char_array);
  Udp.endPacket();
}

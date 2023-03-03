#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiUdp.h>

WiFiUDP Udp;

//const char* ssid = "HOUSENKA";
//const char* password = "medvidekPu";
const char* ssid = "F2B";
const char* password = "f2bpasswd";

void startServer() {
  server.enableCORS(true);
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

  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  /*while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Connection failed");
      WiFi.begin(ssid, password);
    }
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Pico W is connected to WiFi network ");
  Serial.println(WiFi.SSID());

  Serial.print("Assigned IP Address: ");
  Serial.println(WiFi.localIP());*/
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
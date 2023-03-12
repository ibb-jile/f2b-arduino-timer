#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
HTTPUpdateServer httpUpdater;
WebServer server(80);

const char* ssid = "F2B";
const char* password = "f2bpasswd";

void startServer() {
  server.on("/", handleRoot);
  server.on("/flight", downloadFlight);
  server.on("/set", HTTP_PUT, setData);
  server.on("/get", getData);
  server.on("/timer/start", startTimerRequest);
  server.on("/timer/stop", stopTimerRequest);
  server.on("/motor/stop", stopMotorRequest);
  server.on("/motor/start", startMotorRequest);
  server.on("/speed/up", speedUpRequest);
  server.on("/speed/down", speedDownRequest);
  server.onNotFound(handleNotFound);
  server.enableCORS(true);
  httpUpdater.setup(&server);
  server.begin();
}

void setupWifi() {
  delay(10000);
  Serial.println("Configuring access point...");
  while (!WiFi.softAP(ssid, password)) {
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


void getData() {
  StaticJsonDocument<500> doc;

  doc["preset"]["speed"] = getPresetSpeed();
  doc["preset"]["timeSec"] = presetTimeSec;
  doc["preset"]["delaySec"] = presetDelaySec;
  doc["preset"]["autostart"] = autostart;
  doc["preset"]["optimizer"]["allowSpeedOptimizer"] = allowSpeedOptimizer;
  doc["preset"]["optimizer"]["maxSpeedUp"] = maxSpeedUp;
  doc["preset"]["optimizer"]["halfSpeedUp"] = halfSpeedUp;
  doc["preset"]["optimizer"]["littleSpeedUp"] = littleSpeedUp;
  doc["preset"]["optimizer"]["speedChangeStep"] = speedChangeStep;
  doc["preset"]["optimizer"]["optimalMinG"] = optimalMinG;

  doc["current"]["speed"] = getCurrentSpeed();
  doc["current"]["state"] = state;
  doc["current"]["finishAt"] = timerFinishAt;
  doc["current"]["rpm"] = getRpm();
  doc["current"]["time"] = millis();
  doc["current"]["fsStatus"] = getFsInfo();
  String output;
  serializeJson(doc, output);

  server.send(200, "application/json", output);
}

void setData() {
  String json = server.arg("plain");
  Serial.println("JSON:" + json);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  setPresetSpeed(byte(doc["preset"]["speed"]));
  presetTimeSec = int(doc["preset"]["timeSec"]);
  presetDelaySec = byte(doc["preset"]["delaySec"]);
  autostart = byte(doc["preset"]["autostart"]);
  allowSpeedOptimizer = byte(doc["preset"]["optimizer"]["allowSpeedOptimizer"]);
  maxSpeedUp = byte(doc["preset"]["optimizer"]["maxSpeedUp"]);
  halfSpeedUp = byte(doc["preset"]["optimizer"]["halfSpeedUp"]);
  littleSpeedUp = byte(doc["preset"]["optimizer"]["littleSpeedUp"]);
  speedChangeStep = byte(doc["preset"]["optimizer"]["speedChangeStep"]);
  optimalMinG = byte(doc["preset"]["optimizer"]["optimalMinG"]);

  storePresets();

  server.send(200, "application/json", "");
}

void downloadFlight() {
  File file = LittleFS.open(logName, "r");
  server.streamFile(file, "application/octet-stream");
  file.close();
}
void handleRoot() {
  Serial.println("client connected 5");
  server.send(200, "text/html", pageBody);
}
void startTimerRequest() {
  startTimer();
  server.send(200, "application/json", "");
}
void stopTimerRequest() {
  stopTimer();
  server.send(200, "application/json", "");
}
void stopMotorRequest() {
  stopMotor();
  server.send(200, "application/json", "");
}
void startMotorRequest() {
  startMotor();
  server.send(200, "application/json", "");
}
void speedUpRequest() {
  speedUp();
  server.send(200, "application/json", "");
}
void speedDownRequest() {
  speedDown();
  server.send(200, "application/json", "");
}

void handleNotFound() {
  server.send(404, "text/plain", "unknown URL");
}

void handleServerClients() {
  server.handleClient();
}

#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>

WebServer server(80);

const char * ssid = "#1@";
const char * pass = "unxh4995";
const char * host = "esp32";

static bool hasSD = false;
File uploadFile;

void index0() {
  File html = SD.open("/index.htm");
  String htmlPage = "";
  while(html.available()) {
    int x = html.read();
    htmlPage += x;
    Serial.write(x);
  }
  html.close();
  server.send(200, "text/html", htmlPage);
  html.rewindDirectory();
  return;
}

void index1() {
  File html = SD.open("/index1.htm");
  String htmlPage = "";
  while(html.available()) {
    int x = html.read();
    htmlPage += x;
    Serial.write(x);
  }
  html.close();
  server.send(200, "text/html", htmlPage);
  html.rewindDirectory();
  return;
}


void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(200);  
  }
  Serial.print("WiFi Connected to ");
  Serial.println(WiFi.localIP());
  if (SD.begin(5)) {
    Serial.println("SD Card Connected");
  } else {
    Serial.println("SD Card NOT Connected");
  }
  server.on("/0", HTTP_GET, index0);
  server.on("/1", HTTP_GET, index1);
  server.begin();
}

void loop() {
  server.handleClient();
}

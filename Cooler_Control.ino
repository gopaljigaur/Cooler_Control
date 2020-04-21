#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define rel 2

int wlan_on = 1;
int stat = 1;
int no_need = 0;
long rcv;
bool ota_flag = false;
uint16_t time_elapsed = 0;

const char* ssid = "Cooler server";
const char* pass = "Ljgad#94912";
const char* ssid_loc = "JioFi_101EC45";
const char* pass_loc = "6xqwoy3x4w";

HTTPClient http;
ESP8266WebServer server(80);

void handleRoot();
void handleONRequest();
void handleOFFRequest();
void handleTimer(long);
void handleSetTimer();
void writeFile(long);
long readFile();
void cancel();

void setup() {
Serial.begin(115200);
  SPIFFS.begin();
  //Set pin state to value from previous session
  pinMode(rel, OUTPUT);
  rcv = readFile();
  Serial.print("Status from last session : ");
  Serial.println(stat);
  if (stat==0)
    digitalWrite(rel, LOW);
  else
    digitalWrite(rel,HIGH);

  //check for program mode
  File f1 = SPIFFS.open("/boot.txt", "r");
  while (f1.available()) {
    Serial.print("Startup boot mode : ");
    String bootMode = f1.readString();
    Serial.println(bootMode);
    if(bootMode == "program"){
      ota_flag = true;
    }
    else{
      ota_flag=false;
    }
  }
  f1.close();
  if(ota_flag){
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_loc, pass_loc);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }
  else{
  //Start WIFI
  Serial.println('\n');
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, pass);
  WiFi.begin(ssid_loc, pass_loc);
  Serial.println("Connecting ...");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println('\n');
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());               // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  }
  Serial.print("AP mode ");

  IPAddress ipadr = WiFi.softAPIP();
  Serial.print("Hotspot IP: ");
  Serial.println(ipadr);
  Serial.println("");
  
  
  //Associate requests to functions
  server.on("/", handleRoot);
  server.on("/on", handleONRequest);
  server.on("/off", handleOFFRequest);
  server.on("/setTimer", handleSetTimer);
  server.on("/cancel", cancel);
  server.on("/status", sendStatus);
  server.on("/restart", restartModule);

  server.begin();
  Serial.println("HTTP server started on port 80");
  //Set timer if time left from previous session was>0
  
  if (rcv > 0) {
    Serial.print("Time left from previous session in secs : ");
    Serial.println(rcv);
    handleTimer(rcv);
  }
  }
}

void loop() {
  if(ota_flag){
    while(time_elapsed<30000)
    {
      ArduinoOTA.handle();
      time_elapsed=millis();
      delay(10);  
    }
    ota_flag=false;
    File fw = SPIFFS.open("/boot.txt", "w");
  if (!fw) {
    Serial.println("Unable to open File");
    }
  else
  {
    fw.print("normal");
    Serial.print("wrote : normal");
  }
  fw.close();
    Serial.println("OTA Timeout, reverting to normal mode...");
    ESP.restart();
  }
  
  server.handleClient();
    
}

void restartModule(){
  server.send(200, "text/html", "Restarting...");
  Serial.println("Rebooting...");
  String bootMode = server.arg("boot");
  File fw = SPIFFS.open("/boot.txt", "w");
  if (!fw) {
    Serial.println("Unable to open File");
    }
  else
  {
    if((bootMode!="normal")&&(bootMode!="program"))
      bootMode="normal";
    fw.print(bootMode);
    Serial.print("wrote : ");
    Serial.print(bootMode);
  }
  fw.close();
  delay(1000);
  ESP.restart();
}

void handleRoot() {
  server.send(200, "text/html", "Server up and running");
  Serial.println("handleRoot() was called");
}

void handleONRequest() {
  server.send(200, "text/html", "This returns ON status");
  digitalWrite(rel, HIGH);
  Serial.println("handleONRequest() was called");
  stat = 1; no_need = 1; rcv = 0;
  writeFile(rcv);
}

void handleOFFRequest() {
  server.send(200, "text/html", "This returns OFF status");
  digitalWrite(rel, LOW);
  Serial.println("handleOFFRequest() was called");
  stat = 0; no_need = 1; rcv = 0;
  writeFile(rcv);
}

void handleSetTimer() {
  Serial.println("handleSetTimer() was called");
  no_need = 0;
  String message = "";
  String tme = server.arg("secs");
  if (tme == "") {
    message = "Time left argument not found (secs) ";
  }
  else {
    message = "Time Left in secs = ";
    message += tme;
  }

  server.send(200, "text/html", message);
  long current_millis = millis();
  long time_left = (atol(tme.c_str()));
  time_left = time_left * 1000;
  time_left = time_left + current_millis;
  handleTimer(time_left / 1000);
}

void cancel() {
  server.send(200, "text/plain", "Timer has been cancelled");
  Serial.println("cancel() was called");
  no_need = 1; rcv = 0;
  writeFile(0);
}

void sendStatus() {
  String send_status = String(rcv) + " " + String(stat) + " ";
  server.send(200, "text/plain", send_status);
}

long readFile() {
  String left, pin_status;
  long rcvd;
  File f = SPIFFS.open("/timer.txt", "r");
  while (f.available()) {
    left = f.readString();
    int ind = left.indexOf(" ");
    int len = left.length();
    pin_status = left.substring(ind + 1, len );
    left = left.substring(0, ind);
  }
  f.close();
  rcvd = atol(left.c_str());
  stat = pin_status.toInt();
  return rcvd;
}

void writeFile(long left) {
  Serial.print("Writefile was called with value in secs ");
  Serial.println(left);
  File fw = SPIFFS.open("/timer.txt", "w");
  if (!fw) {
    Serial.println("Unable to open File");
  }
  else
  {
    fw.print(left);
    fw.print(" ");
    fw.print(stat);
    Serial.print("wrote : ");
    Serial.print(left);
    Serial.print(" ");
    Serial.println(stat);
  }
  fw.close();
}

void handleTimer(long secs) {
  Serial.print("Timer set with secs ");
  Serial.println(secs);
  long millisec = secs * 1000;
  long write_to_file = secs - (millis() / 1000);
  rcv = write_to_file;
  writeFile(write_to_file);
  long t = 0;
  no_need = 0;
  long i = millis();
  millisec += 1000;
  while (millis() < millisec)
  {
    server.handleClient();

    if (no_need == 1)
      break;
    if (t == 10000)
    { i = millis();
      writeFile((millisec - i) / 1000);
      t = 0;
      rcv = (millisec - i) / 1000;
    }
    t = millis() - i;
  }
  if (no_need == 0) {
    if (stat == 1)
      handleOFFRequest();
    else
      handleONRequest();
  }
}

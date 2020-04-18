#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>

#define rel 2

int wlan_on = 1;
int stat = 1;
int no_need = 0;
long rcv;
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
void handleGenericArgs();
void handleSpecificArg();
void writeFile(long);
long readData();
void cancel();

void setup() {
  Serial.begin(115200);
  delay(10);
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

  pinMode(rel, OUTPUT);

  Serial.println("");

  server.on("/", handleRoot);
  server.on("/on", handleONRequest);
  server.on("/off", handleOFFRequest);
  server.on("/genericArgs", handleGenericArgs); //Associate the handler function to the path
  server.on("/specificArgs", handleSpecificArg);   //Associate the handler function to the path
  server.on("/cancel", cancel);
  server.on("/status", sendStatus);
  SPIFFS.begin();

  server.begin();
  Serial.println("HTTP server started on port 80");

  rcv = readData();
  Serial.print("Status from last session : ");
  Serial.println(stat);
  if (stat==0)
    digitalWrite(rel, LOW);
  else
    digitalWrite(rel,HIGH);
  if (rcv > 0) {
    Serial.print("Time left from previous session in secs : ");
    Serial.println(rcv);
    //if (stat == 0)
      //digitalWrite(rel, LOW);
    //else
      //digitalWrite(rel, HIGH);
    handleTimer(rcv);
  }
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(897, "text/html", "Server up and running");
  Serial.println("handleRoot() was called");
}

void handleONRequest() {
  server.send(897, "text/html", "This returns ON status");
  digitalWrite(rel, HIGH);
  Serial.println("handleONRequest() was called");
  stat = 1; no_need = 1; rcv = 0;
  writeFile(rcv);
}

void handleOFFRequest() {
  server.send(897, "text/html", "This returns OFF status");
  digitalWrite(rel, LOW);
  Serial.println("handleOFFRequest() was called");
  stat = 0; no_need = 1; rcv = 0;
  writeFile(rcv);
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

void handleGenericArgs() {

  String message = "Number of args received:";
  message += server.args();
  message += "\n";

  for (int i = 0; i < server.args(); i++) {
    message += "Arg no." + (String)i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
  }

  server.send(897, "text/html", message);
}

void handleSpecificArg() {
  Serial.println("handleSpecificArgs() was called");
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

  server.send(897, "text/html", message);          //Returns the HTTP response
  long current_millis = millis();
  long time_left = (atol(tme.c_str()));
  time_left = time_left * 1000;
  time_left = time_left + current_millis;
  handleTimer(time_left / 1000);
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

long readData() {
  String left, pin_status;
  long rcvd;
  File f = SPIFFS.open("/timer.txt", "r");
  while (f.available()) {
    left = f.readString();
    int ind = left.indexOf(" ");
    int len = left.length();
    pin_status = left.substring(ind + 1, len );
    left = left.substring(0, ind);
    //Serial.println("left "+left);
    //Serial.println("pin_status "+pin_status);
  }
  f.close();
  rcvd = atol(left.c_str());
  stat = pin_status.toInt();
  return rcvd;
}

void cancel() {
  server.send(897, "text/plain", "Timer has been cancelled");
  Serial.println("cancel() was called");
  no_need = 1; rcv = 0;
  writeFile(0);
}

void sendStatus() {
  String send_status = String(rcv) + " " + String(stat) + " ";
  server.send(897, "text/plain", send_status);
}

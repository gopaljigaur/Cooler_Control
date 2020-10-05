#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#define rel 0
#define leed 2

int wlan_on = 1;
int stat = 1;
int no_need = 0;
long rcv;
bool ota_flag = false;
unsigned long time_elapsed = 0;
unsigned long last_print = 0;
unsigned long last_print2 = 0;

const String ssid_loc = "Cooler server";
const String pass_loc = "Ljgad#94912";
const char *host = "https://gopalji.ml/.netlify/functions/alternate";

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
// Set your Static IP address
IPAddress local_IP(192, 168, 43, 246);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void handleRoot();
void handleONRequest();
void handleOFFRequest();
void handleTimer(long);
void handleSetTimer();
void writeFile(long);
long readFile();
void cancel();
void changeWIFI();
void beginScan();
void addHotspots();
void getNetworkStatus();

void setup() {
Serial.begin(115200);
  SPIFFS.begin();
  pinMode(rel, OUTPUT);
  pinMode(leed, OUTPUT);
  rcv = readFile();
  //Start WIFI
  Serial.println('\n');
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_AP_STA);
  addHotspots();
 
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println('\n');
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  }
  else{
    WiFi.softAP(ssid_loc, pass_loc);
    Serial.print("AP mode ");
    IPAddress ipadr = WiFi.softAPIP();
    Serial.print("Hotspot IP: ");
    Serial.println(ipadr);
  }
  Serial.println("");

  Serial.print("Status from last session : ");
  Serial.println(stat);
  if (stat==0){
    digitalWrite(rel, LOW);
    digitalWrite(leed, HIGH);
  }
  else{
    digitalWrite(rel,HIGH);
    digitalWrite(leed, LOW);
  }

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
  //---------------------------------------------ota code do not touch-------------------------------------------------------------
  if(ota_flag){
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
  }

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
    //----------------------------------------end of ota code-----------------------------------------
  else{
  
  //Associate requests to functions
  server.on("/", handleRoot);
  server.on("/on", handleONRequest);
  server.on("/off", handleOFFRequest);
  server.on("/setTimer", handleSetTimer);
  server.on("/cancel", cancel);
  server.on("/status", sendStatus);
  server.on("/restart", restartModule);
  server.on("/changeWIFI", changeWIFI);
  server.on("/beginScan", beginScan);
  server.on("/networkStatus",getNetworkStatus);
  
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
    while(time_elapsed<100000)
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
  // serial print status update every 1 minute
  time_elapsed=millis();

  if(int(time_elapsed/1000)%60==0 && (long(time_elapsed/1000) - last_print)>1){
    if(WiFi.status()==WL_CONNECTED){
      Serial.print(">> Connected to : ");
      Serial.println(WiFi.SSID());
      Serial.print(">> IP Address : ");
      Serial.println(WiFi.localIP());
    }
    else{
      Serial.print(">> AP mode SSID : ");
      Serial.println(ssid_loc);
      Serial.print(">> IP Address : ");
      Serial.println(WiFi.softAPIP());
    }
    Serial.println();
    last_print = long(time_elapsed/1000);
  }
    if(int(time_elapsed/1000)%900==0 && (long(time_elapsed/1000) - last_print2)>1){
    if(wifiMulti.run()==WL_CONNECTED){
      if(stat==1){  //ON
        digitalWrite(leed,HIGH); 
      }
      else{
        digitalWrite(leed,LOW);
      }
      std::unique_ptr<WiFiClientSecure>client(new WiFiClientSecure);
      client->setInsecure();
      HTTPClient https;
      String payload = WiFi.SSID();
      if(https.begin(*client, host)){
        https.addHeader("Content-Type", "text/plain");
        Serial.println("Connecting to gopalji.ml");
        int httpCode = https.POST(payload);
        if(httpCode>0){
          Serial.println("Request Succeeded");
        }else{
          Serial.println("Request Failed");
        }
        Serial.print("Code : ");
        Serial.println(httpCode);
        https.end();
        }else{
          Serial.println("Unable to Connect to gopalji.ml");
        }
      if(stat==1){  //ON
        digitalWrite(leed,LOW); 
      }
      else{
        digitalWrite(leed,HIGH);
      }
    }
    Serial.println();
    last_print2 = long(time_elapsed/1000);
  }
  wifiMulti.run();
  server.handleClient();
}

void restartModule(){
  server.send(200, "text/html", "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>Restarting...</body></html>");
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
  server.send(200, "text/html", "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/><style>button{background-color:#e7e7e7;color:#000;border:none;padding:15px 32px;cursor:pointer;text-align:center;width:100%;font-size:16px;border-radius:5px;transition:background-color .2s ease-in}button:hover{background-color:#ccc;transition:background-color .2s ease-in}#form,form{display:inline-block;margin-left:10px;margin-right:10px;text-align:left;padding:20px}input[type=text]{margin-bottom:5px;line-height:3;border:solid 1px #000;border-radius:5px;outline:0;box-shadow:none;padding-left:10px;padding-right:10px;font-size:18px;transition:box-shadow .2s ease-in}input[type=text]:focus{box-shadow:0 0 1px 1px #000;transition:box-shadow .2s ease-in}</style><script>function httpGetAsync(e){var n=new XMLHttpRequest;n.open('GET',e,!0);n.send(null);}</script></head><body><h2>Server Up and Running</h2> <br><div style='text-align: center;'><div style='display: inline-block;'><div id='form'> <button onclick='httpGetAsync(\"/on\");'>On</button></div><div id='form'> <button onclick='httpGetAsync(\"/off\");'>Off</button></div><form action='/beginScan'> <button type='submit'>Begin Scan</button></form><form action='/setTimer'> <input type='text' name='secs' placeholder='Enter time in seconds'><br> <button type='submit'>Set Timer</button></form><form action='/cancelTimer'> <button type='submit'>Cancel Timer</button></form><form action='/getStatus'> <button type='submit'>Check Timer Status</button></form><form action='/restart'> <button type='submit' name='mode' value='normal'>Restart Normal</button></form><form action='/restart'> <button type='submit' name='mode' value='program'>Restart Program</button></form><form action='/beginScan'> <button type='submit'>Scan for Networks</button></form><form action='/networkStatus'> <button type='submit'>Check network status</button></form></div></div> <br></body></html>");
  Serial.println("handleRoot() was called");
}

void handleONRequest() {
  server.send(200, "text/html", "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>The device is now ON</body></html>");
  digitalWrite(rel, HIGH);
  digitalWrite(leed, LOW);
  Serial.println("handleONRequest() was called");
  stat = 1; no_need = 1; rcv = 0;
  writeFile(rcv);
}

void handleOFFRequest() {
  server.send(200, "text/html", "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>The device is now OFF</body></html>");
  digitalWrite(rel, LOW);
  digitalWrite(leed, HIGH);
  Serial.println("handleOFFRequest() was called");
  stat = 0; no_need = 1; rcv = 0;
  writeFile(rcv);
}

void handleSetTimer() {
  Serial.println("handleSetTimer() was called");
  no_need = 0;
  String message = "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>";
  String tme = server.arg("secs");
  if (tme == "") {
    message = message+ "Time left argument not found (secs) ";
  }
  else {
    message += "Time Left in secs = ";
    message += tme;
  }
  message += "</body></html>";
  server.send(200, "text/html", message);
  long current_millis = millis();
  long time_left = (atol(tme.c_str()));
  time_left = time_left * 1000;
  time_left = time_left + current_millis;
  handleTimer(time_left / 1000);
}

void cancel() {
  server.send(200, "text/html", "<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>Timer has been cancelled</body></html>");
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

void addHotspots(){
  DynamicJsonDocument doc(2000);
  String wifi_ssid="";
  File f = SPIFFS.open("/wifi_ssid.txt", "r");
  while (f.available()) {
    wifi_ssid = f.readString();
  }
  f.close();
  deserializeJson(doc,wifi_ssid);
  JsonArray data = doc["data"];
  for(int i=0;i<data.size();i++){
    wifiMulti.addAP(data[i]["ssid"],data[i]["pass"]);
  }
  wifiMulti.run();
}

void changeWIFI(){
  server.send(200,"text/HTML","<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>Adding new WiFi. Server will restart.</body></html>");
  String new_ssid = server.arg("ssid");
  String new_pass = server.arg("pass");
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  delay(200);
  WiFi.begin(new_ssid, new_pass);
  if(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("Connection Failed! Reverting to previous WIFI settings...");
    WiFi.disconnect();
    }
  else{
    DynamicJsonDocument doc(2000);
    String wifi_ssid="";
    File f = SPIFFS.open("/wifi_ssid.txt", "r");
    while (f.available()) {
      wifi_ssid = f.readString();
    }
    f.close();
    deserializeJson(doc,wifi_ssid);
    JsonArray data = doc["data"];
    bool exist=false;
    int i=0;
    for(;i<data.size();i++){
      if(data[i]["ssid"]==new_ssid)
        {
          exist = true;
          break;
        }
    }
    if(!exist){
      JsonObject data_new = data.createNestedObject();
      data_new["ssid"]=new_ssid;
      data_new["pass"]=new_pass;
    }
    else{
      data[i]["pass"]=new_pass;
    }
    
    File fw_s = SPIFFS.open("/wifi_ssid.txt", "w");
    if (!fw_s) {
      Serial.println("Unable to open File");
    }
    serializeJson(doc,fw_s);
    fw_s.close();
    }
    ESP.restart();
}
void beginScan(){
  String current_ssid= WiFi.SSID();
  String enct = "";
  Serial.println("SSID scan started ...");
  int networksFound = WiFi.scanNetworks();
  String wlist="<html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no\"><style>button{background-color: #e7e7e7; color: black;border:none;padding: 15px 32px;cursor:pointer;text-align: center;width:100%;font-size:16px;border-radius:5px;transition: background-color 0.2s ease-in;}button:hover{background-color: #cccccc;transition:background-color 0.2s ease-in;}#forma{display:block;text-align:center}#form{display:inline-block;margin-left:auto;margin-right:auto;text-align:left;}#networks{display:inline-block;text-align:center}#networks0{display:block;text-align:center;}</style><script>function httpGetAsync(e){var n=new XMLHttpRequest;var dat='ssid='+encodeURIComponent(document.getElementById('ssid').value)+'&pass='+encodeURIComponent(document.getElementById('pass').value);n.open('POST',e,!0);n.setRequestHeader('Content-Type','application/x-www-form-urlencoded');n.send(dat);}</script></head><body><h3>Scanned Networks</h3><div id=\"networks0\"><div id=\"networks\">";
  if(networksFound<=0){
    Serial.println("No networks found");
    wlist = wlist+ "No networks found";
  }
  else{
  Serial.printf("%d network(s) found\n", networksFound);
  }
  for (int i = 0; i < networksFound; i++)
  {
    if(WiFi.SSID(i)==current_ssid){
    wlist=wlist+"Current Network : <br>";
    }
    switch(WiFi.encryptionType(i)){
      case 2:{
        enct="WPA";
        break;
      }
      case 4:{
        enct="WPA";
        break;
      }
      case 5:{
        enct="WEP";
        break;
      }
      case 7:{
        enct="open";
        break;
      }
      default:{
        enct="unknown";
      }
    }
    wlist=wlist+"<button onclick=\"document.getElementById('form').style.display='';document.getElementById('ssid').value='"+WiFi.SSID(i)+"'\">"+WiFi.SSID(i)+" ("+enct+") (Strength : "+WiFi.RSSI(i)+" dB)</button><br><br>";
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
  }
wlist=wlist+"</div></div><br><div id='forma'><div id='form' style='display:none;'><label for='ssid'>Network Name:</label><br><input type='text' id='ssid' name='ssid' value='' readonly/><br><label for='pass'>Password:</label><br><input type='password' id='pass' name='pass'/><br><br><button name='submit' onclick=\"httpGetAsync('/changeWIFI');\"/>Submit</button><br><br>Please close this page after clicking submit</div></div></body></html>";
  server.send(200,"text/html",wlist);
}

void getNetworkStatus(){
  Serial.println("Get network Status called");
  if(WiFi.status()==WL_CONNECTED){
    Serial.println("ssid : "+WiFi.SSID());
    Serial.println("ip : "+WiFi.localIP().toString());
    server.send(200,"text/html","<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>SSID: "+WiFi.SSID()+", IP: "+WiFi.localIP().toString()+"</body></html>");
  }
  else{
    Serial.println("ssid : "+ssid_loc);
    Serial.println("ip : "+WiFi.softAPIP().toString());
    server.send(200,"text/html","<html lang='en'><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'/></head><body>SSID: "+ssid_loc+", IP: "+WiFi.softAPIP().toString()+"</body></html>");
  }
}

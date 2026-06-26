#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "AdafruitIO_WiFi.h"

//Wifi & Adafruit _IO

#define WIFI_SSID     "WIFI-PASSWORD"
#define WIFI_PASS     "WIFI-PASS"
#define IO_USERNAME   "IO-USERNAME"
#define IO_KEY        "IO-KEY"
AdafruitIO_WiFi io(IO_USERNAME,IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *object1Feed = io.feed("object1");
AdafruitIO_Feed *object2Feed = io.feed("object2");
AdafruitIO_Feed *object3Feed = io.feed("object3");
AdafruitIO_Feed *object4Feed = io.feed("object4");

//Pins & Relay

const int object1Pin = 26;
const int object2Pin = 27;
const int object3Pin = 25;
const int object4Pin = 32;
#define RELAY_ON LOW
#define RELAY_OFF HIGH

//Web server & modes

WebServer server(80) ;
Preferences preferences;
String mode = "AP";

// Webpage

const char HTML [] PROGMEM = R"rawliteral(
<!DOCTYPE   html><html><head><title>ESP32  Control</title>
<meta name = "viewport" content="width=device-width, initial-scale=1">
<style>
  body{text-align:center;font-family:sans-serif;background-color:#f4f4f9;}
  button{padding:14px;margin:8px;font-size:18px;border-radius:8px;border:none;background-color:#007bff;color:white;cursor:pointer;}
  button:active{background-color:#0056b3;}
  #status{margin-top:20px;font-weight:bold;color:#333;}
</style>
<script>
  function sendCmd (url)  {
    document.getElementById('status').innerText = 'Sending command...';
    fetch(url)
      .then(response => response.text())
      .then(text =>  {  document.getElementById( 'status').innerText = 'Status: ' + text; })
      .catch(err =>  {  document.getElementById('status').innerText = 'Error connecting to ESP32'; });

  }
</script>
</head><body>
<h2>ESP32 Relay Control</h2>
<p id="status">Status: Ready</p>
<hr>
<h3>Object 1</h3>
<button onclick="sendCmd('/object1/on')">Object1 ON</button>
<button onclick="sendCmd('/object1/off')">Object1 OFF</button>
<h3>Object 2</h3>
<button onclick="sendCmd('/object2/on')">Object2 ON</button>
<button onclick="sendCmd('/object2/off')">Object2 OFF</button>
<h3>Object 3</h3>
<button onclick="sendCmd('/object3/on')">Object3 ON</button>
<button onclick="sendCmd('/object3/off')">Object3 OFF</button>
<h3>Object 4</h3>
<button onclick="sendCmd('/object4/on')">Object4 ON</button>
<button onclick="sendCmd('/object4/off')">Object4 OFF</button>
<hr>
<h3>System Mode</h3>
<button onclick="sendCmd('/switchMode')" style="background-color:#dc3545; ">Switch to %MODE%</button>
</body></html>
)rawliteral";

String processor(const String& var)  {
  return (mode == "AP") ? "Station" : "AP";
}


//  Web Handlers

void handleRoot()  {
  String page = HTML ; 
  page.replace("%MODE%" , processor(""));
  server.send(200, "text/html", page);
}

void handleSwitchMode()  {
  mode = (mode == "AP") ? "SM" : "AP";
  preferences.putString("mode", mode);
  Serial.println("Switch requested: mode changed to " + mode);
  server.send(200, "text/plain", "Switching mode to " + mode);
  delay(1000);
  ESP.restart();
}

void handleObject1On()  { digitalWrite(Object1Pin, Relay_ON);  Serial.println("Object 1 ON");  server.send(200, "text/plain", "Object 1 ON"); }
void handleObject1Off()  { digitalWrite(Object1Pin, Relay_OFF);  Serial.println("Object 1 OFF");  server.send(200, "text/plain", "Object 1 OFF"); }

void handleObject2On()  { digitalWrite(Object2Pin, Relay_ON);  Serial.println("Object 2 ON");  server.send(200, "text/plain", "Object 2 ON"); }
void handleObject2Off()  { digitalWrite(Object2Pin, Relay_OFF);  Serial.println("Object 2 OFF");  server.send(200, "text/plain", "Object 2 OFF"); }

void handleObject3On()  { digitalWrite(Object3Pin, Relay_ON);  Serial.println("Object 3 ON");  server.send(200, "text/plain", "Object 3 ON"); }
void handleObject3Off()  { digitalWrite(Object3Pin, Relay_OFF);  Serial.println("Object 3 OFF");  server.send(200, "text/plain", "Object 3 OFF"); }

void handleObject4On()  { digitalWrite(Object4Pin, Relay_ON);  Serial.println("Object 4 ON");  server.send(200, "text/plain", "Object 4 ON"); }
void handleObject4Off()  { digitalWrite(Object4Pin, Relay_OFF);  Serial.println("Object 4 OFF");  server.send(200, "text/plain", "Object 4 OFF"); }

// Web Routes

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/object1/on", handleObject1On);
  server.on("/object1/off", handleObject1Off);
  server.on("/object2/on", handleObject2On);
  server.on("/object2/off", handleObject2Off);
  server.on("/object3/on", handleObject3On);
  server.on("/object3/off", handleObject3Off);
  server.on("/object4/on", handleObject4On);
  server.on("/object4/off", handleObject4Off);
  server.on("/switchMode", handleSwitchMode);
  server.begin();
  Serial.println("Web server started");
}


//  Setup

void setup()  {
  Serial.begin(115200);
  delay(1000);

  //Initialize Pins

  pinMode(object1Pin, OUTPUT);
  pinMode(object2Pin, OUTPUT);
  pinMode(object3Pin, OUTPUT);
  pinMode(object4Pin, OUTPUT);

  digitalWrite(object1Pin, RELAY_OFF);
  digitalWrite(object2Pin, RELAY_OFF);
  digitalWrite(object3Pin, RELAY_OFF);
  digitalWrite(object4Pin, RELAY_OFF);

  preferences.begin("esp32-mode", false);
  mode = preferences.getString("mode", "AP");

  if (mode == "AP") {
    WiFi.softAP("ESP32-CONTROL", "12345678");
    Serial.println("Mode: Access Point (AP)");
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
  } else {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("Mode: Station (SM)");
    Serial.print("connecting to WiFi");

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)  {
      delay(500); Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi Connected");
      Serial.print("Station IP: "); Serial.println(WiFi.localIP());

      // ADAFRUIT IO Connection logics

      io.connect();
      Serial.print("connecting to ADAFRUIT IO");

      unsigned long aio_start = millis();
      while(io.status() < AIO_CONNECTED && millis() - aio _start <10000) {
        delay(500);
        Serial.print(".");
      }
      
      if (io.status() < AIO_CONNECTED) {
        Serial.println("\n Failed to connect to Adafruit IO! (Moving On...)");
      } else {
      Serial.println("\nAdafruit IO Connected!");
      }

      // dashboard commands

      object1Feed->onMessage([](AdafruitIO_Data *data) {
        String cmd = data->toString();
        Serial.println("AdrafruitIO Object 1 Command: " + cmd);
        digitalWrite(object1Pin, (cmd == "ON") ? RELAY_ON : RELAY_OFF);
      });

      object2Feed->onMessage([](AdafruitIO_Data *data) {
        String cmd = data->toString();
        Serial.println("AdrafruitIO Object 2 Command: " + cmd);
        digitalWrite(object2Pin, (cmd == "ON") ? RELAY_ON : RELAY_OFF);
      });

      object3Feed->onMessage([](AdafruitIO_Data *data) {
        String cmd = data->toString();
        Serial.println("AdrafruitIO Object 3 Command: " + cmd);
        digitalWrite(object3Pin, (cmd == "ON") ? RELAY_ON : RELAY_OFF);
      });

      object4Feed->onMessage([](AdafruitIO_Data *data) {
        String cmd = data->toString();
        Serial.println("AdrafruitIO Object 4 Command: " + cmd);
        digitalWrite(object4Pin, (cmd == "ON") ? RELAY_ON : RELAY_OFF);
      });

    }  else {
      Serial.println("\nFailed to connect to WiFi");
      Serial.println("Starting fallback AP. (Will try WiFi again on next reboot done)");
      WiFi.softAP("ESP32-CONTROL", "12345678");
      Serial.print("Fallback AP IP: "); Serial.println(WiFi.softAPIP());
    }
  }
  
  setupWebServer();
}

// Loop

void loop() {
  server.handleClient();
  if (mode == "SM" && WiFi.status() == WL_CONNECTED) {
    io.run(); // only run io when connected to wifi
  }
}
      



  

    
  


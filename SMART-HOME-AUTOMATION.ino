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
AdafruitIO_Feed *bulb = io.feed("bulb");

//Pins & Relay

const int object1 = 26;
const int object2 = 27;
const int object3 = 25;
const int object4 = 32;
#define RELAY_ON_LOW
#define RELAY_OFF HIGH

//Web server & modes

Webserver server(80) ;
Preferences preferences;
String mode = "AP";

// Webpage

const char HTML [] PROGMEM = R"rawliteral(
<!DOCTYPE   html><html><head><title>ESP32  Control</title>
<meta name = "viewport" content="width=device-width, intial-scale=1">
<style>
  body{text-align:centre;font-family:sans-serif;background-color:#f4f4f9;}
  button{padding:14px;margin:8px;font-size:18px;border-radius:8px;border:none;background-color:#007bff;color:white;cursor:pointer;}
  button;active{background-color:#0056b3;}
  #status{margin-top:20px;font-weight:bold;color:#333;}
</style>
<script>
  function sendDmd (url)  {
    document.getElementById('status').innerText = 'Sending command...';}
    fetch(url)
      .then(response => response.text())
      .then(text =>  {  document.getElementById( 'status').innerText = 'status: ' + text; })
      .catch(err =>  {  document.getElemntById('status').innerText = 'Error connecting to ESP32'; });

  }
</script>
</head><body>
<h2>ESP32 Relay Control</h2>
<p id="status">Status: Ready</p>
<hr>
<h3>Object 1</h3>
<button oneclick="sendCmd('/object1/on')">Object1 ON</button>
<button oneclick="sendCmd('/object1/off')">Object1 OFF</button>
<h3>Object 2</h3>
<button oneclick="sendCmd('/object2/on')">Object2 ON</button>
<button oneclick="sendCmd('/object2/off')">Object2 OFF</button>
<h3>Object 3</h3>
<button oneclick="sendCmd('/object3/on')">Object3 ON</button>
<button oneclick="sendCmd('/object3/off')">Object3 OFF</button>
<h3>Object 4</h3>
<button oneclick="sendCmd('/object4/on')">Object4 ON</button>
<button oneclick="sendCmd('/object4/off')">Object4 OFF</button>
<hr>
<h3>System Mode</h3>
<button oneclick="sendCmd('/switchMode')" style="background-color:#dc3545; ">Switch to %MODE%</button>
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
void handleObject1On()  { digitalWrite(Object1Pin, Relay_OFF);  Serial.println("Object 1 OFF");  server.send(200, "text/plain", "Object 1 OFF"); }

void handleObject2On()  { digitalWrite(Object2Pin, Relay_ON);  Serial.println("Object 2 ON");  server.send(200, "text/plain", "Object 2 ON"); }
void handleObject2On()  { digitalWrite(Object2Pin, Relay_OFF);  Serial.println("Object 2 OFF");  server.send(200, "text/plain", "Object 2 OFF"); }

void handleObject3On()  { digitalWrite(Object3Pin, Relay_ON);  Serial.println("Object 3 ON");  server.send(200, "text/plain", "Object 3 ON"); }
void handleObject3On()  { digitalWrite(Object3Pin, Relay_OFF);  Serial.println("Object 3 OFF");  server.send(200, "text/plain", "Object 3 OFF"); }

void handleObject4On()  { digitalWrite(Object4Pin, Relay_ON);  Serial.println("Object 4 ON");  server.send(200, "text/plain", "Object 4 ON"); }
void handleObject4On()  { digitalWrite(Object4Pin, Relay_OFF);  Serial.println("Object 4 OFF");  server.send(200, "text/plain", "Object 4 OFF"); }



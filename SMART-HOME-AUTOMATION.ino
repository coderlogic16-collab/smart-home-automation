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

const int motorpin = 26;
const int bulbpin  = 27;
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
<h3>Motor</h3>
<button oneclick="sendCmd('/motor/on')">Motor ON</button>
<button oneclick="sendCmd('/motor/off')">Motor OFF</button>
<h3>Bulb</h3>
<button oneclick="sendCmd('/bulb/on')">Bulb ON</button>
<button oneclick="sendCmd('/bulb/off')">Bulb OFF</button>
<h3>Object3</h3>
<button oneclick="sendCmd('/object3/on')">Object3 ON</button>
<button oneclick="sendCmd('/object3/off')">Object3 OFF</button>
<hr>
<h3>System Mode</h3>
<button oneclick="sendCmd('/switchMode')" style="background-color:#dc3545; ">Switch to %MODE%</button>
</body></html>
)rawliteral";

String processor(const String& var)  {
  return (mode == "AP") ? "Station" : "AP";
}




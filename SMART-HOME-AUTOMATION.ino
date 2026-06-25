#include <WiFi.h>
#include <Webserver.h>
#include <Preference.h>
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


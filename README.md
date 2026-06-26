                                      Smart Home Automation — ESP32 Relay Controller

💡 Why This Project?
Most smart home solutions lock you into a closed ecosystem, require monthly subscriptions, or depend entirely on cloud servers. This project puts you in full control:

Pain Point	Our Solution
1) Expensive smart switches	₹500 total hardware cost
2) Locked ecosystems	Fully open source, modify anything
3) Cloud dependency	Works offline in AP mode
4) Needs a specific app	Any browser on any device
5) No voice control	Google Assistant via IFTTT


✨ Features:
Feature	Description
1) 4-Channel Relay Control	Independently switch 4 loads — lights, fans, motors, anything
2) Dual-Mode WiFi	AP Mode creates its own hotspot · Station Mode joins your home WiFi
3) Built-in Web Dashboard	Responsive control panel accessible from any browser — no app required
4) Adafruit IO Cloud Sync	Monitor and control relays from anywhere in the world
5) Google Assistant Voice	"Hey Google, turn on Object 2" via IFTTT + Adafruit IO
6) Persistent Mode Memory	Remembers your AP/Station choice across reboots using ESP32 flash
7) Auto Fallback	If WiFi fails in Station mode, automatically starts AP hotspot
8) Boot State Sync	On every boot, relays sync to last known cloud state
10) Mobile Responsive	Dashboard works perfectly on phones, tablets, and desktops


🏗️ System Architecture
The project is built on three layers — Hardware, Firmware, and Cloud — all working together seamlessly.

<img width="1254" height="1254" alt="image" src="https://github.com/user-attachments/assets/edd10c63-6a99-4c7e-93d6-cd48bb67373d" />


🧠 How It Works — System Mechanism

This section explains the complete internal mechanism of the project — how every component connects, communicates, and operates from the moment you plug in the ESP32 to the moment a relay switches.


1. Power-On & Initialization
When the ESP32 is powered on, the firmware executes a precise startup sequence:
Key Design Decisions:
Safe state on boot — all relays start OFF to prevent unintended switching
Flash memory persistence — the AP/STA mode choice survives reboots, so users don't reconfigure every time
Graceful fallback — if WiFi credentials are wrong or the router is down, the ESP32 doesn't get stuck — it automatically starts AP mode so you can still control it locally


2. Dual-Mode WiFi — How the Two Modes Work
The ESP32 operates in one of two WiFi modes. The user can switch between them from the web dashboard, and the choice is remembered permanently.
Aspect	             AP Mode	                                             Station Mode
WiFi	         ESP32 creates hotspot	                                 ESP32 joins your router
IP Address	    Always 192.168.4.1	                               Dynamic (check Serial Monitor)
Internet	           ❌ None	                                                ✅ Full
Web Dashboard	       ✅ Works	                                                ✅ Works
Cloud Control	       ❌	                                                    ✅ Adafruit IO
Voice Control	       ❌	                                                    ✅ Google Assistant
Users	Devices     connected to ESP32 hotspot only	                        Anyone on same WiFi
Best For	           Testing, portable use, no router	                 Daily smart home use


How Mode Switching Works:

User clicks the red "Switch to Station/AP" button on the dashboard
ESP32 saves the new mode to non-volatile flash memory using the Preferences library
ESP32 sends an HTTP response confirming the switch
ESP32 reboots itself with ESP.restart()
On reboot, it reads the saved mode and starts in the new configuration


3. Web Server — How the Dashboard Works
The ESP32 runs a full HTTP web server on port 80. The entire HTML page (including CSS and JavaScript) is stored in flash memory using PROGMEM, so it uses zero RAM.

How a Button Click Flows:

┌─────────┐    HTTP GET           ┌─────────┐    digitalWrite()    ┌────────┐
│ Browser │ ──────────────────▶  │  ESP32  │ ──────────────────▶  │ Relay  │
│         │    /object1/on        │  Web    │    GPIO 26 → LOW     │ Module │
│         │                       │  Server │                      │        │
│         │  ◀──────────────────  │         │                      │        │
│         │    "Object 1 ON"      │         │                      │        │
└─────────┘    (plain text)       └─────────┘                      └────────┘


Step by step:

User taps "Object 1 ON" button on the web page
JavaScript sendCmd() function fires fetch('/object1/on')
Browser sends an HTTP GET request to the ESP32's IP
ESP32's WebServer matches the URL to handleObject1On() handler
Handler calls digitalWrite(object1Pin, RELAY_ON) → relay activates
Handler sends back "Object 1 ON" as plain text response
JavaScript updates the status display on the page
If any error occurs, the .catch() block shows an error message


Why PROGMEM? The ESP32 has limited RAM (~320KB). By storing the HTML in flash memory (PROGMEM), the page uses 0 bytes of RAM until it's actually being served. This is critical for embedded systems.

4. Cloud Control — The Adafruit IO Pipeline
When in Station mode, the ESP32 maintains a persistent MQTT connection to Adafruit IO. MQTT is a lightweight messaging protocol designed for IoT — it uses minimal bandwidth and keeps a live connection so messages arrive instantly.

Key Mechanism Details:

Subscription model — ESP32 subscribes to 4 feeds (object1 through object4). Whenever any feed receives new data, the ESP32 is notified immediately
Lambda callbacks — each feed has an onMessage() callback (C++ lambda) that extracts the command string and drives the appropriate GPIO pin
Boot sync — on every startup, feed->get() fetches the last published value from each feed, ensuring relays match the dashboard state even after a power outage
io.run() loop — called every iteration of loop(), this processes incoming MQTT packets, sends keepalive pings, and handles reconnections5. Voice Control — The Complete Chain


Voice control uses a 4-service chain: Google Assistant → IFTTT → Adafruit IO → ESP32. The entire trip takes about 2–5 seconds.

Why this architecture?

No public IP needed — the ESP32 doesn't need to be accessible from the internet. It connects outward to Adafruit IO via MQTT, and Adafruit IO pushes commands to it
No port forwarding — works behind any NAT/router
Reliable — if the ESP32 briefly disconnects, MQTT will deliver queued messages on reconnection
Scalable — adding voice control to more objects only requires new IFTTT applets, no code changes


7. Safety & Reliability Mechanisms
Mechanism                                                     	Purpose
All relays OFF on boot	                              Prevents accidental activation during power glitches
10-second WiFi timeout	                              Doesn't hang forever if router is unreachable
10-second AIO timeout	                                Cloud failure doesn't block local web control
Fallback AP mode	                                    If WiFi fails, ESP32 is never bricked — you can always connect via hotspot
Persistent mode storage	                              Mode choice survives reboots — no reconfiguration needed
Connection-guarded io.run()	                          Only processes cloud messages when actually connected, preventing crashes
server.sendHeader("Connection","close")              	Ensures HTTP response sends completely before reboot during mode switch

🔩 Hardware Requirements
Component	Qty	Approx Cost	Where to Buy
ESP32 Dev Board (30-pin)	1	
4-Channel Relay Module (5V, Active LOW)	1	
Male-to-Female Jumper Wires	6+	
Micro-USB / Type-C Cable	1	
5V USB Power Supply	1	

🔌 Wiring Guide
ESP32 → Relay Module

    ESP32 DevKit                         4-Channel Relay Module
  ┌─────────────────┐                 ┌──────────────────────────┐
  │                 │                 │                          │
  │  ┌──┐          │                 │   ┌──────┐              │
  │  │  │  GPIO 26 ├─── 🟡 ─────────┤── │ IN1  │  Object 1    │
  │  │  │          │                 │   └──────┘              │
  │  │  │  GPIO 27 ├─── 🟢 ─────────┤── │ IN2  │  Object 2    │
  │  │  │          │                 │   └──────┘              │
  │  │U │  GPIO 25 ├─── 🔵 ─────────┤── │ IN3  │  Object 3    │
  │  │S │          │                 │   └──────┘              │
  │  │B │  GPIO 32 ├─── 🟠 ─────────┤── │ IN4  │  Object 4    │
  │  │  │          │                 │   └──────┘              │
  │  │  │     GND  ├─── ⚫ ─────────┤── │ GND  │              │
  │  │  │          │                 │   └──────┘              │
  │  │  │  VIN(5V) ├─── 🔴 ─────────┤── │ VCC  │              │
  │  └──┘          │                 │   └──────┘              │
  │                 │                 │                          │
  └─────────────────┘                 └──────────────────────────┘


	Pin Reference Table
ESP32 Pin	Relay Pin	Controls	Wire Colour
GPIO 26	IN1	Object 1	🟡 Yellow
GPIO 27	IN2	Object 2	🟢 Green
GPIO 25	IN3	Object 3	🔵 Blue
GPIO 32	IN4	Object 4	🟠 Orange
GND	GND	Common Ground	⚫ Black
VIN (5V)	VCC	Relay Power	🔴 Red



Relay → Appliance (Each Channel)

                         Relay Output Terminal
                        ┌──────────────────────┐
  Live Wire  ───────────┤  COM (Common)        │
  (from mains 220V)     │                      │
                        │  NO (Normally Open) ──┼──▶ To Appliance
                        │                      │
                        │  NC (Not used)       │
                        └──────────────────────┘

  Neutral Wire ─────────────────────────────────────▶ To Appliance (direct)


	NOTE

COM = mains Live wire in
NO = to appliance (relay ON = circuit closed = appliance ON)
NC = not used here (NC means "always on until relay activates")
Active-LOW is the default. If your relay is Active-HIGH, swap in code:
cpp

#define RELAY_ON  HIGH
#define RELAY_OFF LOW



Quick Start
For those who want to get running fast:


1. Wire ESP32 → Relay Module (6 jumper wires — see table above)
2. Install Arduino IDE + ESP32 board package + Adafruit IO library
3. Open .ino file → update WiFi & Adafruit IO credentials
4. Upload to ESP32 → Open Serial Monitor (115200 baud)
5. Connect phone to "ESP32-CONTROL" WiFi → Open 192.168.4.1
6. 🎉 Control your relays!

💻 Software Setup — Arduino IDE
Step 1 — Install Arduino IDE
Download from arduino.cc/en/software (version 2.x)
Install and open
Step 2 — Add ESP32 Board Support
File → Preferences → paste in Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Tools → Board → Boards Manager → search esp32 → install "esp32 by Espressif Systems"
Step 3 — Install Libraries
Sketch → Include Library → Manage Libraries → search and install:

Library	Author
Adafruit IO Arduino	Adafruit
Adafruit MQTT Library	Adafruit
ArduinoHttpClient	Arduino
TIP

Install Adafruit IO Arduino first — when prompted "Install all dependencies?", click Yes to auto-install the other two.

Step 4 — Board Settings
Setting	Value
Board	ESP32 Dev Module
Upload Speed	115200
Flash Frequency	80MHz
Partition Scheme	Default 4MB with spiffs
Port	Your ESP32's COM port
Can't find COM port? Plug in ESP32 → open Device Manager → Ports (COM & LPT) → look for CP210x or CH340. If missing, install the CP2102 driver or CH340 driver.



Upload Code to ESP32:


Open the .ino file in Arduino IDE
Update your credentials (see next section)
Connect ESP32 via USB
Select correct Board + Port in Tools menu
Click → Upload button
Wait for "Done uploading"
Open Serial Monitor → set baud to 115200
You'll see:

Stored mode: AP
Mode: Access Point (AP)
AP IP: 192.168.4.1
Web server started.
Connect phone to WiFi ESP32-CONTROL (pass: 12345678)
Open browser → go to 192.168.4.1 → ✅ Dashboard loads!

 WiFi Credentials & Modes
What to Change in Code
cpp

#define WIFI_SSID   "Your-WiFi-Name"        // ← home WiFi name (case-sensitive)
#define WIFI_PASS   "Your-WiFi-Password"     // ← home WiFi password
#define IO_USERNAME "Your-Adafruit-Username"  // ← from io.adafruit.com
#define IO_KEY      "aio_xxxxxxxxxxxx"        // ← from io.adafruit.com → 🔑 My Key
When Are Credentials Needed?
Mode	WiFi Creds?	Adafruit IO Creds?
AP Mode (default)	❌ No	❌ No
Station Mode	✅ Yes	✅ Yes
You can test everything in AP mode first without setting up any accounts. Credentials only matter when switching to Station mode.


 Adafruit IO Setup:
Adafruit IO is a free IoT cloud platform that connects your ESP32 to the internet.

Step 1 — Create Account
Go to io.adafruit.com → Get Started for Free

Step 2 — Get Your Key
Click the 🔑 yellow key icon (top-right) → copy Username and Active Key

Step 3 — Create 4 Feeds
Feeds → + New Feed → create each one:

Feed Name	Key (must match exactly)	Controls
Object 1	object1	Relay 1
Object 2	object2	Relay 2
Object 3	object3	Relay 3
Object 4	object4	Relay 4
Step 4 — Create Dashboard (Optional but Recommended)
Dashboards → + New Dashboard → name: ESP32 Control
Open it → ⚙️ → Create New Block → Toggle
Connect to object1 → ON value: ON, OFF value: OFF
Repeat for all 4 feeds


IFTTT + Google Voice Setup:
IFTTT connects Google Assistant to Adafruit IO. Voice command → IFTTT → publishes to feed → ESP32 receives it.

💰 IFTTT Pricing — Read First
Plan	Applets	Cost	Enough For
Free	2 applets	$0	1 object (ON + OFF) ✅
Pro	20 applets	$3.49/mo	All 4 objects
Pro+	Unlimited	$14.99/mo	Complex automations
IMPORTANT

1 object needs 2 applets (one for ON, one for OFF). Free tier = voice control for 1 object only. For 2+ objects, you need IFTTT Pro ($3.49/month).

Step 1 — Create IFTTT Account
Go to ifttt.com → sign up (use your Google account for easy linking)

Step 2 — Connect Google Assistant
Explore → search "Google Assistant" → Connect → sign in with same Google account as your phone/Google Home

Step 3 — Connect Adafruit
Search "Adafruit" → Connect → sign in with your Adafruit IO credentials

Step 4 — Create "Turn ON" Applet
Create → If This → Google Assistant → "Say a simple phrase"

ield	Value
What do you want to say?	Turn on object 2
Another way (optional)	Switch on object 2
Another way (optional)	Object 2 on
Assistant response	Turning on object 2
Then That → Adafruit → "Send data to Adafruit IO"

Field	Value
Feed name	object2
Data to save	ON
Continue → Finish

Step 5 — Create "Turn OFF" Applet
Same steps, but:

Phrase: Turn off object 2
Data: OFF
Step 6 — Test It
ESP32 in Station mode, connected to WiFi
Serial Monitor shows Adafruit IO Connected!
Say: "Hey Google, turn on object 2"
Serial Monitor shows: AIO Object 2: ON
Relay clicks ✅
Using a Google Home Speaker
If you have a Google Nest / Home Mini speaker:

Install Google Home app (Android / iOS)
Sign in with the same Google account as IFTTT
Set up the speaker through the app
Voice commands work on the speaker automatically — no extra setup
NOTE

A Google Home speaker is optional. "Hey Google" on your Android phone works the same way.



Troubleshooting
Problem	                                    Cause	                                                    Fix
No COM port in IDE	                  Missing USB driver	                                 Install CP2102 or CH340 driver
Upload fails	                        Wrong board/port	                                   Tools → Board → ESP32 Dev Module, correct COM port
Page won't load on 192.168.4.1	      Phone on mobile data	                               Turn off mobile data, connect to ESP32-CONTROL WiFi only
Relay clicks but appliance stays off	Wrong terminal	                                     Use COM + NO terminals, not NC
WiFi won't connect	                  Wrong credentials	                                   WIFI_SSID and WIFI_PASS are case-sensitive
Adafruit IO won't connect	            Wrong key	                                           Verify at io.adafruit.com → 🔑 icon
Voice doesn't trigger                 relay	Feed name mismatch	                           Must be exactly object2 (lowercase, no spaces) in IFTTT
Relay logic inverted	                Active-HIGH module	                                 Swap RELAY_ON/RELAY_OFF values

Roadmap
 4-channel relay control via web interface
 Dual-mode WiFi (AP + Station with fallback)
 Adafruit IO cloud integration with boot sync
 Google Assistant voice control via IFTTT
 OTA (Over-The-Air) firmware updates
 Physical button support alongside web/voice
 Scheduling & timer functionality
 MQTT broker support (Home Assistant compatible)
 Relay state persistence across reboots
 Alexa voice support
🤝 Contributing
Contributions are welcome!

Fork this repository
Create a branch: git checkout -b feature/amazing-feature
Commit: git commit -m 'Add amazing feature'
Push: git push origin feature/amazing-feature
Open a Pull Request
📄 License
This project is licensed under the MIT License — see 
LICENSE
 for details.

⭐ If this project helped you, give it a star! ⭐

Built with 🔧 ESP32 · ☁️ Adafruit IO · 🎙️ Google Assistant · ❤️ Open Source




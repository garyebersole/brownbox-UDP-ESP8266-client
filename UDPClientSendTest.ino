//
//  COMPONENT:  UDPClientSendTest
//  VERSION:    1.0
//  DATE:       2016-12-10
//  AUTHOR:     Gary Ebersole
//  DESCRIPTION:
//    A simple test to connect an ESP8266 client to an access point with a static IP and send a UDP packet
//    to a listener running on a host on that network (probably the router or the same host).
//    
//  REFERENCES:
//    https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/station-class.md#config
//    The ESP8266 version of the Wifi lib...the config method is different than
//    the Arduino version...use the ESP8266 instructions
//
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>

// Raspberry Pi 3 router (static IP)
const char* SSID = "Your-AP";
const char* PASSPHRASE = "your_passphrase";
IPAddress componentStaticIp(172, 24, 1, 20);  // in /etc/hosts file on object
IPAddress accessPointIp(172, 24, 1, 1);
IPAddress accessPointGateway(172, 24, 1, 1);
IPAddress accessPointSubnetMask(255,255,255,0);

// Connection loop...tweak to see if it makes a difference
const int CONNECTION_MAX_WAIT_TRIES = 100;
const int CONNECTION_WAIT_DELAY_MS = 50;
int CONNECTION_WAIT_TIMES = 0;

// UDP Listener
WiFiUDP Udp;
char udpPacket[50];

// Elapsed time
int awake_ms;
int connection_ms;
int message_sent_ms;

void setup() {
    Serial.begin(115200);
  Serial.println();
  Serial.println("=========================");
  awake_ms = millis();
  Serial.printf("%i\n", awake_ms);
  // Connect to WiFi access point after setting static IP configuration
  if (WiFi.config(componentStaticIp, accessPointGateway, accessPointSubnetMask)) {
    Serial.println("Static IP configuration set");
    WiFi.begin(SSID, PASSPHRASE);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      if (CONNECTION_WAIT_TIMES++ < CONNECTION_MAX_WAIT_TRIES) {
        delay(CONNECTION_WAIT_DELAY_MS);
        Serial.print(".");
      } else {
        Serial.println("Failed to connect...resetting");
        delay(1000);
        ESP.reset();
      }
    }
    // Connected to AP
    connection_ms = millis();
    Serial.println();
    Serial.printf("Connected at %i ms\n", connection_ms);
    sprintf(udpPacket, "Awake: %ims / Connected: %ims", awake_ms, connection_ms);
    Udp.beginPacket("172.24.1.1", 41234);  // Send this to the listener on the Pi
    Udp.write(udpPacket);
    Udp.endPacket();
    Serial.printf("Sent message at %i ms\n", millis());
  } else {
    Serial.println("Failed to configure static IP");
  }
  //delay(100);  // allow sufficient time for packet to be fired before reset
  //ESP.reset();
}

void loop() {}


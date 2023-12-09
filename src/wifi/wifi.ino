/*
* Copyright (C) 2022 by Roberto Calvo-Palomino
*
*
*  This programa is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with RTL-Spec.  If not, see <http://www.gnu.org/licenses/>.
*
*   Authors: Roberto Calvo-Palomino <roberto.calvo [at] urjc [dot] es>
*/

// Subject: Sistemas Empotrados y de Tiempo Real
// Universidad Rey Juan Carlos, Spain

#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "network"
#define WLAN_PASS       "password"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "193.147.53.2" // (garceta.tsc.urjc.es)
#define AIO_SERVERPORT  21883
#define AIO_USERNAME    "/SETR/2023/"
#define AIO_KEY         "key"

// Define specific pins for Serial2.
#define RXD2 33
#define TXD2 4

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish publisher = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "Forocoches");

void initWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {

  Serial.begin(9600);

  // Serial port to communicate with Arduino UNO
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  initWiFi();
  
}

String sendBuff;

void loop() {
  MQTT_connect();
  // We always check if there is data in the serial buffer (max: 64 bytes)

  if (Serial2.available()) {

    char c = Serial2.read();
    sendBuff += c;
    
    if (c == '}')  {            
      Serial.print("Received data in serial port from Arduino: ");
      Serial.println(sendBuff);
      publisher.publish(c);

      sendBuff = "";
    } 


  }

}

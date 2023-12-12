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
#include "Messages.hpp"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "wifieif"
#define WLAN_PASS       "Goox0sie_WZCGGh25680000"

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
  Serial2.print("{Start}");
}

unsigned long get_time(char time[MAX_MSG_LENGTH]) {
  unsigned long curr_num = 0;
  char *ptr = time;
  ptr += 1; // Remove 2 spaces and msg id

  for (ptr; *ptr != '|'; ptr++) {
    if (*ptr >= '0' && *ptr <= '9') {
      curr_num = 10 * curr_num + *ptr - '0';
    }
  }

  return curr_num;
}

int get_dst(char distance[MAX_MSG_LENGTH]) {
  int curr_num = 0;
  char *ptr = distance;
  ptr += 1; // Remove 2 spaces and msg id

  for (ptr; *ptr != '|'; ptr++) {
    if (*ptr >= '0' && *ptr <= '9') {
      curr_num = 10 * curr_num + *ptr - '0';
    }
  }

  return curr_num;
}

void setup() {

  Serial.begin(9600);

  // Serial port to communicate with Arduino UNO
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // initWiFi();
  Serial2.print("{Start}");
  
}

String sendBuff;
char to_send[MAX_MSG_LENGTH];
char msg_buffer[MAX_MSG_LENGTH];

void loop() {
  // MQTT_connect();
  // We always check if there is data in the serial buffer (max: 64 bytes)

  if (Serial2.available()) {

    char c = Serial2.read();
    sendBuff += c;
    
    if (c == '|')  {            
      strcpy(msg_buffer, sendBuff.c_str());

      switch (*msg_buffer) { // Remove 2 initial spaces
        case '0':
          sprintf(to_send, MSG_BASE, TEAM_STR, ID_STR, "START_LAP");
          break;
        case '1':
          sprintf(to_send, MSG_TIME, TEAM_STR, ID_STR, "END_LAP", get_time(msg_buffer));
          break;
        case '2':
          sprintf(to_send, MSG_DIST, TEAM_STR, ID_STR, "OBSTACLE_DETECTED", get_dst(msg_buffer));
          break;
        case '3':
          sprintf(to_send, MSG_BASE, TEAM_STR, ID_STR, "LINE_LOST");
          break;
        case '4':
          sprintf(to_send, MSG_TIME, TEAM_STR, ID_STR, "PING", get_time(msg_buffer));
          break;
        case '5':
          sprintf(to_send, MSG_BASE, TEAM_STR, ID_STR, "INIT_LINE_SEARCH");
          break;
        case '6':
          sprintf(to_send, MSG_BASE, TEAM_STR, ID_STR, "STOP_LINE_SEARCH");
          break;
        case '7':
          sprintf(to_send, MSG_BASE, TEAM_STR, ID_STR, "LINE_FOUND");
          break;
        case '8':
          sprintf(to_send, MSG_VAL, TEAM_STR, ID_STR, "VISIBLE_LINE", 1.0);
          break;
      }
      //publish.publish(to_send);
      Serial.println(to_send);
      // Serial.println(sendBuff);

      sendBuff = "";
    } 


  }

}

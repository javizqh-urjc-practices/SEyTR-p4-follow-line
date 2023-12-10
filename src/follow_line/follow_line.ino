#include <Arduino_FreeRTOS.h>
#include "Messages.hpp"

#define PERIODIC_IDLE 100

Messages *send_msg = new Messages("A", "B");

static void idleTask(void * arg) {
  while (1) {
    send_msg->send_message();
    xTaskDelayUntil( &xLastWakeTime, (PERIODIC_IDLE / portTICK_PERIODO_MS))
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  send_msg->wait_connection();
  xTaskCreate(idleTask, "IdleTask", 100, NULL, 0, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}

#include <Arduino.h>
#include "Messages.hpp"
#include <string.h>

#define MAX_MSG_LENGTH 128

#define MSG_BASE "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\"}"
#define MSG_TIME "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"time\": %ld}"
#define MSG_DIST "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"distance\": %d}"
#define MSG_VAL "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"value\": %.2f}"

Messages::Messages(char _team_name[MAX_TEAM_STR_SIZE],
                   char _team_id[MAX_ID_STR_SIZE]) {
  strcpy(team_name,_team_name);
  strcpy(team_id,_team_id);
  queue_size = 0;
}

void Messages::add_message(msg_type type) {
  struct msg to_send = {type};
  queue[queue_size] = to_send;
  queue_size++;
}

void Messages::add_message(msg_type type, int distance) {
  struct msg to_send = {type};
  to_send.dst = distance; 
  queue[queue_size] = to_send;
  queue_size++;
}

void Messages::add_message(msg_type type, float val) {
  struct msg to_send = {type};
  to_send.value = val; 
  queue[queue_size] = to_send;
  queue_size++;
}

void Messages::send_message() {
  char to_send[MAX_MSG_LENGTH];

  if (millis() - last_ping > 4000) {
    // Send ping
    last_ping = millis();
    sprintf(to_send, MSG_TIME, team_name, team_id, "PING", millis() - init_time);
    Serial.println(to_send);
    return;
  }

  if (queue_size == 0) return; // No messages to send

  switch (queue[0].type) {
    case START_LAP:
      init_time = millis();
      last_ping = -4000; // So it triggers in 0
      sprintf(to_send, MSG_BASE, team_name, team_id, "START_LAP");
      break;
    case END_LAP:
      sprintf(to_send, MSG_TIME, team_name, team_id, "END_LAP", millis() - init_time);
      break;
    case OBSTACLE_DETECTED:
      sprintf(to_send, MSG_DIST, team_name, team_id, "OBSTACLE_DETECTED", queue[0].dst);
      break;
    case LINE_LOST:
      sprintf(to_send, MSG_BASE, team_name, team_id, "LINE_LOST");
      break;
    case INIT_LINE_SEARCH:
      sprintf(to_send, MSG_BASE, team_name, team_id, "INIT_LINE_SEARCH");
      break;
    case STOP_LINE_SEARCH:
      sprintf(to_send, MSG_BASE, team_name, team_id, "STOP_LINE_SEARCH");
      break;
    case LINE_FOUND:
      sprintf(to_send, MSG_BASE, team_name, team_id, "LINE_FOUND");
      break;
    case VISIBLE_LINE:
      sprintf(to_send, MSG_VAL, team_name, team_id, "VISIBLE_LINE", queue[0].value);
      break;
  }

  Serial.println(to_send);

  // Shift everything
  // TODO: Create a link list so this is O(1) instead of O(n)
  for (int i = queue_size - 1; i >= 0; i--) {
    queue[i + 1] = queue[i];
  }
  queue_size--;
}

void wait_connection() {
  while(1) {
      if (Serial.available()) {
        char c = Serial.read();
        if (c == '}')  { // Wait for message        
          break;
        } 

      }
    }
}

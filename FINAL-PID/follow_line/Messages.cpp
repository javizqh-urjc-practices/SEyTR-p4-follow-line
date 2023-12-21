#include <Arduino.h>
#include "Messages.hpp"
#include <string.h>

#define MAX_MSG_LENGTH 128

// Cannot be less than 80 columns. So does not pass 
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

void Messages::wait_connection() {
  char c;
  while(1) {
      if (Serial.available()) {
        c = Serial.read();
        if (c == '|')  { // Wait for message        
          break;
        } 

      }
    }
}

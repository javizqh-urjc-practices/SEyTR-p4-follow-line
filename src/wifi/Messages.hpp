#define TEAM_STR "Forocoches"
#define ID_STR "3"
#define MAX_MSG_LENGTH 128


#define MSG_BASE "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\"}"
#define MSG_TIME "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"time\": %ld}"
#define MSG_DIST "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"distance\": %d}"
#define MSG_VAL "{\"team_name\": \"%s\", \"id\": \"%s\", \"action\": \"%s\", \"value\": %s"

typedef enum {
    START_LAP = 0,
    END_LAP,
    OBSTACLE_DETECTED,
    LINE_LOST,
    PING,
    INIT_LINE_SEARCH,
    STOP_LINE_SEARCH,
    LINE_FOUND,
    VISIBLE_LINE
} msg_type;
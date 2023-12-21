#define MAX_QUEUED_MSG 64
#define MAX_TEAM_STR_SIZE 64
#define MAX_ID_STR_SIZE 4

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

typedef struct msg {
    msg_type type;
    union {
        int dst;
        float value;
    };
} msg;

class Messages {
public:
    Messages(char team_name[MAX_TEAM_STR_SIZE], char team_id[MAX_ID_STR_SIZE]);
    void wait_connection();

private:
    // Queue of messages
    msg queue[MAX_QUEUED_MSG];
    int queue_size;
    // Timer for the ping and for time since start
    unsigned long init_time;
    long last_ping;
    // Team name and id
    char team_name[MAX_TEAM_STR_SIZE];
    char team_id[MAX_ID_STR_SIZE];
};
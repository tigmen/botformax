typedef int64_t bigint;
typedef bigint user_id;
typedef bigint chat_id;
typedef bigint Date;

typedef struct {
	user_id user_id;
	char is_bot;
	char* first_name;
	char* username;
} User;

typedef struct {
	chat_id chat_id;
	char* first_name;
	char* username;
	char* type;
} Chat;

typedef struct {
	bigint message_id;
	User* from;
	Chat* chat;
	Date date;
	char* text;
} Message;

typedef struct {
	bigint update_id;
	Message* message;
} Update;

struct update_node {
	Update* data;
	struct update_node* next;
};

struct update_queue {
	struct update_node* head;
	struct update_node* tail;
};

Chat* chat_parse(struct json_object* json);
User* user_parse(struct json_object* json);
Message* message_parse(struct json_object* json, int* offset);
Update* update_parse(struct json_object* json);

void update_queue_init(struct update_queue**);
struct update_queue* update_queue_delete(struct update_queue*);
void update_queue_push(struct update_queue*, Update*);
Update* update_queue_pop(struct update_queue*);
char update_queue_is_empty(struct update_queue*);

void copy_str(char** to, const char* from);


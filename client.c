#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <string.h>
#include "trace/trace.h"
#include "net.h"

#define URL "https://api.telegram.org/bot7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w"

#define sym(x) #x

typedef int64_t bigint;
typedef bigint user_id;
typedef bigint chat_id;
typedef bigint Date;

typedef struct {
	user_id user_id;
	char is_bot;
	char * first_name;
	char * username;
} User;

typedef struct {
	chat_id chat_id;
	char * first_name;
	char * username;
	char * type;
} Chat ;

typedef struct {
	bigint message_id;
	User * from;
	Chat * chat;
	Date date;
	char * text;
} Message;

typedef struct {
	bigint update_id;
	Message * message;
} Update;

struct update_node {
	Update * data;
	struct update_node * next;
};

struct update_queue {
	struct update_node * head;
	struct update_node * tail;
};


void update_queue_init(struct update_queue **);
struct update_queue * update_queue_delete(struct update_queue *);
void update_queue_push(struct update_queue *, Update *);
Update * update_queue_pop(struct update_queue *);
char update_queue_is_empty(struct update_queue *);

void copy_str(char ** to, const char * from) {
	*to = malloc(strlen(from)) + 1;
	*to = strcpy(*to, from);	
}

#define GETUPDATES "/getUpdates"
#define json_get(type, root, key) (json_object_get_##type(json_object_object_get(root, key)))
struct update_queue * get_updates(char * url) {
	char * temp = malloc(strlen(url) + strlen(GETUPDATES) + 1);
        temp = strcpy(temp, url);
	temp = strcat(temp, GETUPDATES);
	trace(DEBUG, "GET request to %s", temp);

	struct json_object * json = request_get(temp);

	free(temp);

	struct update_queue * result;
	update_queue_init(&result);

	struct json_object * updates = json_object_object_get(json, "result");
	int n = json_object_array_length(updates);
	for(int i = 0; i < n; i++) {
		struct json_object * json_temp = json_object_array_get_idx(updates, i);
		trace(DEBUG,"\t%d %s", i, json_object_to_json_string(json_temp));
		trace(DEBUG,"i: %d is empty: %s", i ,update_queue_is_empty(result) ? "Y" : "n");

		Update * update = malloc(sizeof(Update));
		update->update_id = json_get(int, json_temp, "update_id");

		struct json_object * json_temp_message = json_object_object_get(json_temp, "message");

		Message * message = malloc(sizeof(Message));
		message->message_id = json_get(int, json_temp_message, "message_id");

		struct json_object * json_temp_message_user = json_object_object_get(json_temp, "from");

		User * user = malloc(sizeof(User));
		user->user_id = json_get(int, json_temp_message_user, "user_id");
		user->first_name = (char *)json_get(string, json_temp_message_user, "first_name");
		user->username = (char *)json_get(string, json_temp_message_user, "username");
		message->from = user;

		struct json_object * json_temp_message_chat = json_object_object_get(json_temp, "chat");

		Chat * chat = malloc(sizeof(Chat));
		chat->chat_id = json_get(int, json_temp_message_chat, "chat_id");
		chat->first_name = (char *)json_get(string, json_temp_message_chat, "first_name");
		chat->username = (char *)json_get(string, json_temp_message_chat, "username");
		chat->type = (char *)json_get(string, json_temp_message_chat, "type");

		message->date = json_get(int, json_temp_message, "date");
		message->text = (char *)json_get(string, json_temp_message, "text");


		update->message = message;

		update_queue_push(result, update);
	}
	json_object_put(json);

	return result;
}

int main(void) {
	trace_init("log", INFO);

	CURLcode code = curl_global_init(CURL_GLOBAL_SSL);
	if (code) {
		trace(ERROR, "%s %d: Error initializating curl_global %s",
		      __FILE__, __LINE__, strerror(errno));
		exit(0);
	}

	struct update_queue * queue = get_updates(URL);
	Update * update;
	while(update = update_queue_pop(queue)) {
	printf("%s\n", update->message->text);
	}

	curl_global_cleanup();
}

void update_queue_init(struct update_queue ** queue) {
	*queue = malloc(sizeof(struct update_queue));
	(*queue)->tail = NULL;
	(*queue)->head = NULL;
}

struct update_queue * update_queue_delete(struct update_queue * queue) {
	if(queue !=  NULL) {
		while(!update_queue_is_empty(queue)) {
			update_queue_pop(queue);
		}
	}
	return NULL;
}

void update_queue_push(struct update_queue * queue, Update * data) {
	struct update_node * new = malloc(sizeof(struct update_node));
	new->next = NULL;
	new->data = data;

	if(update_queue_is_empty(queue)) {
		queue->tail = new;
		queue->head = new;
	} else {
		queue->tail->next = new;
		queue->tail = new;
	}
}

Update * update_queue_pop(struct update_queue * queue) {
	Update * data; 
	if(!update_queue_is_empty(queue)) {
	struct update_node * node = queue->head;
	queue->head = node->next;
	
	data = node->data;

	free(node);

	} else {
		trace(WARNING,"%s %d: try to pop empty queue", __FILE__, __LINE__);
		data = NULL;
	}
	return data;
}

inline char update_queue_is_empty(struct update_queue * queue) {
	return queue->head == NULL;
}

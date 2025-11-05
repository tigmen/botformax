#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include "botapi.h"
#include "trace/trace.h"

#define json_get(type, root, key) \
	(json_object_get_##type(json_object_object_get(root, key)))

void copy_str(char** to, const char* from) {
	*to = malloc(strlen(from)) + 1;
	*to = strcpy(*to, from);
}

Chat* chat_parse(struct json_object* json) {
	Chat* chat = malloc(sizeof(Chat));
	chat->chat_id = json_get(int, json, "id");
	chat->first_name = (char*)json_get(string, json, "first_name");
	chat->username = (char*)json_get(string, json, "username");
	chat->type = (char*)json_get(string, json, "type");
	return chat;
}

User* user_parse(struct json_object* json) {
	User* user = malloc(sizeof(User));
	user->user_id = json_get(int, json, "id");
	user->first_name = (char*)json_get(string, json, "first_name");
	user->username = (char*)json_get(string, json, "username");
	return user;
}

Message* message_parse(struct json_object* json, int* offset) {
	Message* message = malloc(sizeof(Message));
	message->message_id = json_get(int, json, "message_id");

	struct json_object* json_user = json_object_object_get(json, "from");

	User* user = user_parse(json_user);
	message->from = user;

	struct json_object* json_chat = json_object_object_get(json, "chat");

	Chat* chat = chat_parse(json_chat);
	message->chat = chat;

	message->date = json_get(int, json, "date");
	message->text = (char*)json_get(string, json, "text");

	return message;
}
Update* update_parse(struct json_object* json) {
	Update* update = malloc(sizeof(Update));
	update->update_id = json_get(int, json, "update_id");

	struct json_object* json_message =
	    json_object_object_get(json, "message");

	Message* message = message_parse(json_message, (int*)0);
	update->message = message;

	return update;
}

void update_queue_init(struct update_queue** queue) {
	*queue = malloc(sizeof(struct update_queue));
	(*queue)->tail = NULL;
	(*queue)->head = NULL;
}

struct update_queue* update_queue_delete(struct update_queue* queue) {
	if (queue != NULL) {
		while (!update_queue_is_empty(queue)) {
			update_queue_pop(queue);
		}
	}
	return NULL;
}

void update_queue_push(struct update_queue* queue, Update* data) {
	struct update_node* new = malloc(sizeof(struct update_node));
	new->next = NULL;
	new->data = data;

	if (update_queue_is_empty(queue)) {
		queue->tail = new;
		queue->head = new;
	} else {
		queue->tail->next = new;
		queue->tail = new;
	}
}

Update* update_queue_pop(struct update_queue* queue) {
	Update* data;
	if (!update_queue_is_empty(queue)) {
		struct update_node* node = queue->head;
		queue->head = node->next;

		data = node->data;

		free(node);

	} else {
		trace(WARNING, "%s %d: try to pop empty queue", __FILE__,
		      __LINE__);
		data = NULL;
	}
	return data;
}

inline char update_queue_is_empty(struct update_queue* queue) {
	return queue->head == NULL;
}

#include <curl/curl.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include "botapi.h"
#include "net.h"
#include "trace/trace.h"

#define URL                         \
	"https://api.telegram.org/" \
	"bot7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w"

#define SENDMESSAGE "/sendMessage"
void send_message(char* url, Chat* chat, char* text) {
	char* temp = malloc(strlen(url) + strlen(SENDMESSAGE) + 1);
	temp = strcpy(temp, url);
	temp = strcat(temp, SENDMESSAGE);
	trace(DEBUG, "GET request to %s", temp);

	struct json_object* param = json_object_new_object();
	if (!param) {
		trace(ERROR, "%s %d: json not created", __FILE__, __LINE__);
		exit(0);
	}

	struct json_object* json;
	json_object_object_add(param, "chat_id",
	                       json_object_new_int(chat->chat_id));
	json_object_object_add(param, "text", json_object_new_string(text));
	json = request_get(temp, json_object_to_json_string(param));
	json_object_put(param);

	free(temp);
}

#define GETUPDATES "/getUpdates"
struct update_queue* get_updates(char* url) {
	static int offset = 0;
	char* temp = malloc(strlen(url) + strlen(GETUPDATES) + 1);
	temp = strcpy(temp, url);
	temp = strcat(temp, GETUPDATES);
	trace(DEBUG, "GET request to %s", temp);

	struct json_object* param = json_object_new_object();
	if (!param) {
		trace(ERROR, "%s %d: json not created", __FILE__, __LINE__);
		exit(0);
	}

	struct json_object* json;
	if (offset > 0) {
		json_object_object_add(param, "offset",
		                       json_object_new_int(offset));
		json = request_get(temp, json_object_to_json_string(param));
		json_object_put(param);
	} else {
		json = request_get(temp, "");
	}

	free(temp);

	struct update_queue* result;
	update_queue_init(&result);

	struct json_object* updates = json_object_object_get(json, "result");
	int n = json_object_array_length(updates);
	for (int i = 0; i < n; i++) {
		struct json_object* json_temp =
		    json_object_array_get_idx(updates, i);
		trace(DEBUG, "\t%d %s", i,
		      json_object_to_json_string(json_temp));
		trace(DEBUG, "i: %d is empty: %s", i,
		      update_queue_is_empty(result) ? "Y" : "n");

		Update* update = update_parse(json_temp);

		send_message(url, update->message->chat, update->message->text);

		offset = update->update_id + 1;
		update_queue_push(result, update);
	}
	printf("%s\n", json_object_to_json_string(json));
	json_object_put(json);

	return result;
}

int main(void) {
	trace_init("log", DEBUG);

	CURLcode code = curl_global_init(CURL_GLOBAL_SSL);
	if (code) {
		trace(ERROR, "%s %d: Error initializating curl_global %s",
		      __FILE__, __LINE__, strerror(errno));
		exit(0);
	}

	while (1) {
		struct update_queue* queue = get_updates(URL);
		Update* update;
		while (update = update_queue_pop(queue)) {
			printf("%s\n", update->message->text);
		}
	}

	curl_global_cleanup();
}

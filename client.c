#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <string.h>
#include "trace/trace.h"

#define url "https://api.telegram.org/bot7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w/getMe"

static size_t write_callback(char *ptr, size_t size, size_t realsize, void * userdata);

int main(void) {
	trace_init("log", DEBUG);

	CURLcode code = curl_global_init(CURL_GLOBAL_SSL);
	if (code) {
		trace(ERROR, "%s %d: Error initializating curl_global %s",
		      __FILE__, __LINE__, strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tInit global curl");
	}

	CURL* curl = curl_easy_init();
	if (curl == NULL) {
		trace(ERROR, "%s %d: Error initializating curl_easy", __FILE__,
		      __LINE__);
		exit(0);
	} else {
		trace(DEBUG, "\tInit easy curl");
	}

	code = curl_easy_setopt(curl, CURLOPT_URL, url);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tSet opt success");
	}

	code = curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tSet opt success");
	}

	code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tSet opt success");
	}
	struct json_object * data;
	code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &data);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tSet opt success");
	}

	code = curl_easy_perform(curl);
	if (code) {
		trace(ERROR, "%s %d: Error perform curl: %s", __FILE__,
		      __LINE__, strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tPerform success");
	}

	if(json_object_get_type(data) == json_type_null) {
		trace(WARNING, "%s %d: NULL json", __FILE__, __LINE__);
	}

	trace(INFO, "\033[1;33mResponce\033[0m\n%s", json_object_to_json_string(data));

	json_object_put(data);

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

static size_t write_callback(char *ptr, size_t size, size_t realsize, void * userdata) {
	struct json_object ** p_userdata = (struct json_object **)userdata;
	*p_userdata = json_tokener_parse(ptr);
	trace(DEBUG, "\033[1;33mHTTP->JSON:\033[0m\n%s \n\t-> %s", ptr, json_object_to_json_string(*p_userdata));
	return realsize;
}

#include "net.h"

static size_t write_callback(char* ptr,
                             size_t size,
                             size_t realsize,
                             void* userdata) {
	struct json_object** p_userdata = (struct json_object**)userdata;
	*p_userdata = json_tokener_parse(ptr);
	trace(DEBUG, "\033[1;33mHTTP->JSON:\033[0m\n%s \n\t-> %s", ptr,
	      json_object_to_json_string(*p_userdata));
	return realsize;
}
void setopt(CURL* curl, CURLoption option, void* param) {
	CURLcode code = curl_easy_setopt(curl, option, param);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	}
}

struct json_object* request_get(char* url, const char* param) {
	CURLcode code = curl_global_init(CURL_GLOBAL_SSL);
	if (code) {
		trace(ERROR, "%s %d: Error initializating curl_global %s",
		      __FILE__, __LINE__, strerror(errno));
		exit(0);
	}

	CURL* curl = curl_easy_init();
	if (curl == NULL) {
		trace(ERROR, "%s %d: Error initializating curl_easy", __FILE__,
		      __LINE__);
		exit(0);
	}

	setopt(curl, CURLOPT_URL, url);

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	setopt(curl, CURLOPT_HTTPHEADER, headers);

	setopt(curl, CURLOPT_POSTFIELDS, (void*)param);

	setopt(curl, CURLOPT_USE_SSL, (void*)CURLUSESSL_ALL);

	setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	struct json_object* data;
	setopt(curl, CURLOPT_WRITEDATA, (void*)&data);

	code = curl_easy_perform(curl);
	if (code) {
		trace(ERROR, "%s %d: Error perform curl: %s", __FILE__,
		      __LINE__, strerror(errno));
		exit(0);
	}

	if (json_object_get_type(data) == json_type_null) {
		trace(WARNING, "%s %d: NULL json", __FILE__, __LINE__);
	}

	trace(INFO, "\033[1;33mResponce\033[0m\n%s",
	      json_object_to_json_string(data));

	curl_easy_cleanup(curl);

	return data;
}

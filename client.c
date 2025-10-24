#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "trace/trace.h"

#define url "https://api.telegram.org"

struct memory {
	char* response;
	size_t size;
};

static size_t cb(char* data, size_t size, size_t nmemb, void* clientp) {
	size_t realsize = nmemb;
	struct memory* mem = (struct memory*)clientp;

	char* ptr = realloc(mem->response, mem->size + realsize + 1);
	if (!ptr)
		return 0; /* out of memory */

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}

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

	code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	if (code) {
		trace(ERROR, "%s %d: Error set opt %s", __FILE__, __LINE__,
		      strerror(errno));
		exit(0);
	} else {
		trace(DEBUG, "\tSet opt success");
	}
	struct memory chunk = {0};
	code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
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

	trace(INFO, "\033[1;33mResponce\033[0m\n%s", chunk.response);


	curl_global_cleanup();
}

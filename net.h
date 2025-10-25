#pragma once

#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <string.h>
#include "trace/trace.h"

static size_t write_callback(char *ptr, size_t size, size_t realsize, void * userdata);
struct json_object * request_get(char * url);

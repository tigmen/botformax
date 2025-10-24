#include <stdlib.h>
#include "trace.h"

char* trace_path;
enum TRACE_LEVEL current_trace_level;

void trace_init(char* path, enum TRACE_LEVEL level) {
	trace_path = path;
	FILE* trace_file = fopen(trace_path, "w");
	fclose(trace_file);
	current_trace_level = level;
}

void trace(enum TRACE_LEVEL trace_level, char* fmt, ...) {
	if (trace_level <= current_trace_level) {
		va_list p_arg;
		va_start(p_arg, fmt);

		FILE* trace_file = fopen(trace_path, "a");
		if (trace_file == NULL) {
			exit(1);
		}

		switch (trace_level) {
			case ERROR:
				fprintf(trace_file,
				        "%s %s ====ERROR==== ", __DATE__,
				        __TIME__);
				break;

			case WARNING:
				fprintf(trace_file, "%s %s ---WARNING---- ",
				        __DATE__, __TIME__);
				break;

			case INFO:
				fprintf(trace_file, "%s %s <INFO>\n", __DATE__,
				        __TIME__);
				break;

			case DEBUG:
				fprintf(trace_file,
				        "%s %s ::::DEBUG:::: ", __DATE__,
				        __TIME__);
				break;

			default:
				break;
		}

		vfprintf(trace_file, fmt, p_arg);
		fprintf(trace_file, "\n");

		fclose(trace_file);

		va_end(p_arg);
	}
}

enum TRACE_LEVEL set_trace_level(enum TRACE_LEVEL level) {
	enum TRACE_LEVEL last_level = current_trace_level;
	current_trace_level = level;

	return last_level;
}

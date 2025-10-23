#include "log.h"
#include <stdlib.h>

char * log_path;
enum LOG_LEVEL current_log_level; 

void log_init(char * path, enum LOG_LEVEL level) {
	log_path = path;
        FILE * log_file = fopen(log_path, "w");
	fclose(log_file);
	current_log_level = level;
}

void log(enum LOG_LEVEL log_level, char * fmt, ...) {
    if(log_level <= current_log_level) {
        va_list p_arg;
        va_start(p_arg, fmt);

        FILE * log_file = fopen(log_path, "a");
        if(log_file == NULL) {
            exit(1);
        }

        switch (log_level)
        {
        case ERROR:
            fprintf(log_file, "%s %s ====ERROR==== ", __DATE__, __TIME__);
            break;
        
        case WARNING:
            fprintf(log_file, "%s %s ---WARNING---- ", __DATE__, __TIME__);
            break;
        
        case INFO:
            fprintf(log_file, "%s %s <INFO>\n", __DATE__, __TIME__);
            break;
        
        case DEBUG:
            fprintf(log_file, "%s %s ::::DEBUG:::: ", __DATE__, __TIME__);
            break;
        
        default:
            break;
        }

        vfprintf(log_file, fmt, p_arg);
        fprintf(log_file, "\n");

        fclose(log_file);

        va_end(p_arg);
    }
}

enum LOG_LEVEL set_log_level(enum LOG_LEVEL level) {
    enum LOG_LEVEL last_level = current_log_level;
    current_log_level = level;

    return last_level;
}

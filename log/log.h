#pragma once

#include <stdio.h>
#include <stdarg.h>

enum LOG_LEVEL  {
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

extern enum LOG_LEVEL current_log_level;
extern char * log_path;

void log_init(char *, enum LOG_LEVEL);
void log(enum LOG_LEVEL, char *, ...);
enum LOG_LEVEL set_log_level(enum LOG_LEVEL);

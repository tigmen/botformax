#pragma once

#include <stdarg.h>
#include <stdio.h>

enum TRACE_LEVEL { ERROR, WARNING, INFO, DEBUG };

extern enum TRACE_LEVEL current_trace_level;
extern char* trace_path;

void trace_init(char*, enum TRACE_LEVEL);
void trace(enum TRACE_LEVEL, char*, ...);
enum TRACE_LEVEL set_trace_level(enum TRACE_LEVEL);

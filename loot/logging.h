#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdio.h>

#define LOG_ERROR(message) log_error(message, __FILE__, __LINE__)
static inline void log_error(const char* message, const char* file, const int line) {
    fprintf(stderr, "%s:%d - %s\n", file, line, message);
}

#endif // _LOGGING_H

#ifndef PLUM_H
#define PLUM_H

#define PLUM_NRM  "\x1B[0m"
#define PLUM_RED  "\x1B[31m"
#define PLUM_GRN  "\x1B[32m"
#define PLUM_YEL  "\x1B[33m"
#define PLUM_BLU  "\x1B[34m"
#define PLUM_MAG  "\x1B[35m"
#define PLUM_CYN  "\x1B[36m"
#define PLUM_WHT  "\x1B[37m"
#define PLUM_GRA  "\x1B[90m"

static char plum_enable_logger = 0;

typedef enum PlumLogLevel {
    PLUM_TRACE,
    PLUM_INFO,
    PLUM_DEBUG,
    PLUM_WARNING,
    PLUM_ERROR,
    PLUM_EXPERIMENTAL
} PlumLogLevel;

const char* plum_unix_log_level_color(PlumLogLevel logLevel);
const char* plum_stringify_log_level(PlumLogLevel logLevel);

void plum_log(PlumLogLevel logLevel, const char *format, ...);

#define PLUM_LOG(logLevel, ...) plum_log(logLevel, __VA_ARGS__)

#ifdef PLUM_IMPLEMENTATION

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h> 

const char* plum_unix_log_level_color(PlumLogLevel logLevel) {
    switch (logLevel) {
        case PLUM_TRACE:         { return PLUM_GRA; }
        case PLUM_INFO:          { return PLUM_GRN; }
        case PLUM_DEBUG:         { return PLUM_CYN; }
        case PLUM_WARNING:       { return PLUM_YEL; }
        case PLUM_ERROR:         { return PLUM_RED; }
        case PLUM_EXPERIMENTAL:  { return PLUM_MAG; }
        default:                 { return PLUM_WHT; }
    }

    return NULL;
}

const char* plum_stringify_log_level(PlumLogLevel logLevel) {
    switch (logLevel) {
        case PLUM_TRACE:         { return "TRC"; }
        case PLUM_INFO:          { return "INF"; }
        case PLUM_DEBUG:         { return "DBG"; }
        case PLUM_WARNING:       { return "WAR"; }
        case PLUM_ERROR:         { return "ERR"; }
        case PLUM_EXPERIMENTAL:  { return "EXP"; }
        default:                 { return "NIL"; }
    }

    return NULL;
}

void plum_get_time_str(char* time_str) {
    time_t t ;
    struct tm *tmp ;
    time( &t );
     
    tmp = localtime( &t );
     
    strftime(time_str, 50, "%H:%M:%S", tmp);
}

void plum_log(PlumLogLevel logLevel, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char time_str[50];
    plum_get_time_str(time_str);

    printf("\x1B[90m[%s %s%s\x1B[90m] ", time_str, plum_unix_log_level_color(logLevel), plum_stringify_log_level(logLevel));
    
    printf("\033[0m");
    vprintf(format, args);

    printf("\n");
    fflush(stdout);

    va_end(args);
}

#endif

#endif
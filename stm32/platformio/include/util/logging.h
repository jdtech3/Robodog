#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#include "util/extern_handles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum log_level {
    DEBUG,
    OK,
    INFO,
    WARN,
    ERR
} log_level_t;

void log_(log_level_t level, const char* context, const char* msg, bool print_newline, ...);

#ifdef ENABLE_DEBUG
    #define LOG_DEBUG(context, msg, ...) log_(DEBUG, context, msg, true, ## __VA_ARGS__)
#else
    #define LOG_DEBUG(context, msg, ...) do {} while(0);    // no op
#endif
#define LOG_OK(context, msg, ...) log_(OK, context, msg, true, ## __VA_ARGS__)
#define LOG_INFO(context, msg, ...) log_(INFO, context, msg, true, ## __VA_ARGS__)
#define LOG_WARN(context, msg, ...) log_(WARN, context, msg, true, ## __VA_ARGS__)
#define LOG_ERR(context, msg, ...) log_(ERR, context, msg, true, ## __VA_ARGS__)

#ifdef __cplusplus
}
#endif

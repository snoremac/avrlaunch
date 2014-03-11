#ifndef AVRLAUNCH_LOG_H
#define AVRLAUNCH_LOG_H

#include <stdio.h>
#include <avr/pgmspace.h>

#include "avrlaunch/hal/hal_gpio.h"

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ERROR
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
	#define LOG_DEBUG(format, ...) LOG("DEBUG", format, __VA_ARGS__)
#else
	#define LOG_DEBUG(format, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
	#define LOG_INFO(format, ...) LOG("INFO", format, __VA_ARGS__)
#else
	#define LOG_INFO(format, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
	#define LOG_WARN(format, ...) LOG("WARN", format, __VA_ARGS__)
#else
	#define LOG_WARN(format, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
	#define LOG_ERROR(format, ...) LOG("ERROR", format, __VA_ARGS__)
#else
	#define LOG_ERROR(format, ...)
#endif
    
void LOG(const char* level, const char* format, ...);

void log_error_malloc(void);

void set_system_warning(void);

void clear_system_warning(void);

void set_system_error(void);

void clear_system_error(void);

#endif
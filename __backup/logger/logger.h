#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#if CONFIG_ARDUINO_ISR_IRAM
#define ARDUINO_ISR_ATTR IRAM_ATTR
#define ARDUINO_ISR_FLAG ESP_INTR_FLAG_IRAM
#else
#define ARDUINO_ISR_ATTR
#define ARDUINO_ISR_FLAG (0)
#endif
const char * pathToFileName(const char * path);

#define LOGGER_BUFFLEN 256

#define LOGGER_LOG_FORMAT(format) "[%s:%u] %s():\t" format "\r\n", pathToFileName(__FILE__), __LINE__, __FUNCTION__
#define LOGGER_LOG_FORMAT_INLINE(format) "[%s:%u] %s():\t" format, pathToFileName(__FILE__), __LINE__, __FUNCTION__

#define logln(format, ...) logger_log_formatted_string(LOGGER_LOG_FORMAT(format), ##__VA_ARGS__)

#define log_inline_begin() logger_log_formatted_string(LOGGER_LOG_FORMAT_INLINE(""))
#define log_inline(format, ...) logger_log_formatted_string(format, ##__VA_ARGS__)
#define log_inline_end() logger_log_formatted_string("\r\n")

void logger_init();
void logger_log_formatted_string(const char *format, ...);

#endif

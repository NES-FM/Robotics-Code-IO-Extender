#include "logger.h"

// char _bt_buf[LOGGER_BUFFLEN];
// uint _bt_buf_idx = 0;
char _logger_ser_buf[LOGGER_BUFFLEN];
unsigned int _logger_ser_buf_idx = 0;

void logger_init()
{
    Serial.println("\r\n");
    logln("Starting up IO Expansion Board...");
}

void logger_log_formatted_string(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char str[256];
    vsprintf(str, format, args);

    Serial.print(str); // INTENDED
    va_end(args);
}

const char * ARDUINO_ISR_ATTR pathToFileName(const char * path)
{
    size_t i = 0;
    size_t pos = 0;
    char * p = (char *)path;
    while(*p){
        i++;
        if(*p == '/' || *p == '\\'){
            pos = i;
        }
        p++;
    }
    return path+pos;
}

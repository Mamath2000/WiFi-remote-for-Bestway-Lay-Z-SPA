#include <Arduino.h>
#include <cstdarg>
// #include <iostream>
#include <LittleFS.h>
#include "bwc_debug.h"

void log2file(const char* s)
{
    LittleFS.begin();
    File file = LittleFS.open(F("/debugoutput.txt"), "a");
    if (!file) {
        // Serial.println(F("Failed to save states.txt"));
        return;
    }

    file.printf(s);
    file.close();
}

static LogSinkFn _logSink = nullptr;

void setLogSink(LogSinkFn fn)
{
    _logSink = fn;
}

void bwcLog(LogLevel lvl, const char* tag, const char* fmt, ...)
{
    if(_logSink == nullptr) return;
    char msg[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    _logSink(tag, lvl, msg);
}

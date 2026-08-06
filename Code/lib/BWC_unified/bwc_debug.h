#ifndef BWC_DEBUG_H
#define BWC_DEBUG_H

#define BWC_DEBUG_OUTPUT_OFF 0
#define BWC_DEBUG_OUTPUT_SERIAL 1
#define BWC_DEBUG_OUTPUT_FILE 2

#if BWC_DEBUGGING == BWC_DEBUG_OUTPUT_OFF 
    #define BWC_LOG_P(s, ...) 
    #define BWC_LOG(s, ...) 
#elif BWC_DEBUGGING == BWC_DEBUG_OUTPUT_SERIAL
    #define BWC_LOG_P(pstr_string, ...) Serial.printf_P(pstr_string, __VA_ARGS__)
    #define BWC_LOG(s, ...) Serial.printf(s, __VA_ARGS__)
#elif BWC_DEBUGGING == BWC_DEBUG_OUTPUT_FILE
    #define BWC_LOG_P(pstr_string, ...) {char s[128]; sprintf_P(s, pstr_string, __VA_ARGS__); Serial.print(s); log2file(s); }
#else
    #define BWC_LOG_P(s, ...) 
    #define BWC_LOG(s, ...) 
#endif

#ifndef BWC_SPRINKLE_YIELDS
    #define BWC_YIELD
#else
    #define BWC_YIELD optimistic_yield(100)
#endif

void log2file(const char* s);

/* Transport-agnostic log sink: lib code (bwc/cio/dsp) calls bwcLog() without
   knowing or caring who's listening. main.cpp registers a sink that forwards
   to MQTT once connected - keeps MQTT/PubSubClient out of the lib/ tree. */
enum LogLevel : uint8_t { LOGLVL_ERROR = 0, LOGLVL_INFO = 1, LOGLVL_DEBUG = 2 };
typedef void (*LogSinkFn)(const char* tag, LogLevel lvl, const char* msg);

void setLogSink(LogSinkFn fn);
void bwcLog(LogLevel lvl, const char* tag, const char* fmt, ...);

#endif //guard
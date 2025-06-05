#ifndef LOG_HTTP_H
#define LOG_HTTP_H
#include <Arduino.h>
#include <queue>

void enqueueLog(const String& message);  
void processLogQueue();                   

void sendLogHttp(const char* message);  
void sendFormattedLog(const char* format, ...);


#endif
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <raylib.h>

// Global colors
extern Color Green;
extern Color Dark_Green;
extern Color Light_Green;
extern Color Yellow;

// DevLog buffering & global variables
extern std::vector<std::string> logHistory;
extern std::mutex logMutex;
extern bool isConsoleVisible;

void CustomLogCallback(int logLevel, const char* text, va_list args);

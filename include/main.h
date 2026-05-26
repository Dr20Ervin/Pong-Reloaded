#pragma once
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <mutex>
#include <cstdarg>
#include <raylib.h>


// Global colors
Color Green = Color{ 38, 185, 154, 255 };
Color Dark_Green = Color{ 20, 160, 133, 255 };
Color Light_Green = Color{ 129, 204, 184, 255 };
Color Yellow = Color{ 243, 213, 91, 255 };

// DevLog buffering & function prototype
std::vector<std::string> logHistory;
std::mutex logMutex;

void CustomLogCallback(int logLevel, const char* text, va_list args);

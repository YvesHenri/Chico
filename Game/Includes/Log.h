#pragma once

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <mutex>

class Log
{
public:
    static void filter(int level);
    static void print(const Log& log, const char* filePath, int fileLine, const char* message, ...);

public:
    static const Log INFO;
    static const Log DEBUG;
    static const Log TRACE;
    static const Log WARN;
    static const Log ERR;

private:
    static int currentLevel;
    static const HANDLE CONSOLE;

private:
    Log(int level, int color, const char* name) {
        this->level = level;
        this->color = color;
        this->name = name;
    }

private:
    int level;
    int color;
    const char* name;
};

#define LOG(level) Log::filter(level)
#define INFO(message, ...) Log::print(Log::INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define DEBUG(message, ...) Log::print(Log::DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define TRACE(message, ...) Log::print(Log::TRACE, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define WARN(message, ...) Log::print(Log::WARN, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define ERR(message, ...) Log::print(Log::ERR, __FILE__, __LINE__, message, ##__VA_ARGS__)
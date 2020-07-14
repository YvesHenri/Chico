#include "../Includes/Log.h"

int Log::currentLevel = 0;
const Log Log::INFO = Log(0, 14, "INFO");
const Log Log::DEBUG = Log(1, 10, "DEBUG");
const Log Log::TRACE = Log(2, 3, "TRACE");
const Log Log::WARN = Log(3, 6, "WARN");
const Log Log::ERR = Log(4, 12, "ERR");
const HANDLE Log::CONSOLE = GetStdHandle(STD_OUTPUT_HANDLE);

void Log::filter(int level) {
    Log::currentLevel = level;
}

void Log::print(const Log& log, const char* filePath, int fileLine, const char* message, ...) {
    std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);

    if (currentLevel <= log.level) {
        std::tm tm;
        std::time_t time = std::time(0);
        std::va_list args;
        std::string buffer(18, '\0');
        std::string fileName(strrchr(filePath, '\\') ? strrchr(filePath, '\\') + 1 : filePath);

        localtime_s(&tm, &time);
        std::strftime(&buffer[0], buffer.size(), "%d/%m/%y %H:%M:%S", &tm);
        SetConsoleTextAttribute(CONSOLE, log.color);
        printf("[%s %s %s:%d] ", buffer.c_str(), log.name, fileName.c_str(), fileLine);
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
        printf("\n");
        SetConsoleTextAttribute(CONSOLE, 7);
    }
}
#ifndef UTILS_LOGGER_IMPL
#define UTILS_LOGGER_IMPL

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <mutex>

#define _FILENAME_ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

namespace logger {

    namespace colors {
        constexpr auto BLUE = 1;
        constexpr auto GREEN = 2;
        constexpr auto LIGHT_BLUE = 3;
        constexpr auto RED = 4;
        constexpr auto PURPLE = 5;
        constexpr auto HOT_YELLOW = 6;
        constexpr auto DEFAULT = 7;
        constexpr auto GRAY = 8;
        constexpr auto MID_BLUE = 9;
        constexpr auto LIGHT_GREEN = 10;
        constexpr auto CYAN = 11;
        constexpr auto LIGHT_RED = 12;
        constexpr auto LIGHT_PURPLE = 13;
        constexpr auto LIGHT_YELLOW = 14;
        constexpr auto WHITE = 15;
    }

    namespace levels {

        namespace info {
            auto color = colors::LIGHT_YELLOW;
            constexpr auto LEVEL = 0;
            constexpr auto NAME = "INFO";
        }
        namespace debug {
            auto color = colors::LIGHT_GREEN;
            constexpr auto LEVEL = 1;
            constexpr auto NAME = "DEBUG";
        }
        namespace trace {
            auto color = colors::LIGHT_BLUE;
            constexpr auto LEVEL = 2;
            constexpr auto NAME = "TRACE";
        }
        namespace warn {
            auto color = colors::HOT_YELLOW;
            constexpr auto LEVEL = 3;
            constexpr auto NAME = "WARN";
        }
        namespace err {
            auto color = colors::LIGHT_RED;
            constexpr auto LEVEL = 4;
            constexpr auto NAME = "ERROR";
        }

        auto current = info::LEVEL;
    }
}

#define LOG_LEVEL(level) logger::levels::current = level; // #level
#define LOG(color, level, prefix, condition, message, ...) \
    if (condition && logger::levels::current <= level) { \
        std::mutex mutex; \
        std::lock_guard<std::mutex> guard(mutex); \
        std::tm tm; \
        std::time_t time = std::time(0); \
        std::string buffer(18, '\0'); \
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); \
        localtime_s(&tm, &time); \
        std::strftime(&buffer[0], buffer.size(), "%d/%m/%y %H:%M:%S", &tm); \
        SetConsoleTextAttribute(console, color); \
        printf("[%s %5s @ %s:%d] ", buffer.c_str(), prefix, _FILENAME_, __LINE__); \
        printf(message, ##__VA_ARGS__); \
        printf("\n"); \
        SetConsoleTextAttribute(console, logger::colors::DEFAULT); \
    }

#define INFO(message, ...) LOG(logger::levels::info::color, logger::levels::info::LEVEL, logger::levels::info::NAME, true, message, ##__VA_ARGS__)
#define DEBUG(message, ...) LOG(logger::levels::debug::color, logger::levels::debug::LEVEL, logger::levels::debug::NAME, true, message, ##__VA_ARGS__)
#define TRACE(message, ...) LOG(logger::levels::trace::color, logger::levels::trace::LEVEL, logger::levels::trace::NAME, true, message, ##__VA_ARGS__)
#define WARN(message, ...) LOG(logger::levels::warn::color, logger::levels::warn::LEVEL, logger::levels::warn::NAME, true, message, ##__VA_ARGS__)
#define ERR(message, ...) LOG(logger::levels::err::color, logger::levels::err::LEVEL, logger::levels::err::NAME, true, message, ##__VA_ARGS__)

#define INFO_IF(condition, message, ...) LOG(logger::levels::info::color, logger::levels::info::LEVEL, logger::levels::info::NAME, condition, message, ##__VA_ARGS__)
#define DEBUG_IF(condition, message, ...) LOG(logger::levels::debug::color, logger::levels::debug::LEVEL, logger::levels::debug::NAME, condition, message, ##__VA_ARGS__)
#define TRACE_IF(condition, message, ...) LOG(logger::levels::trace::color, logger::levels::trace::LEVEL, logger::levels::trace::NAME, condition, message, ##__VA_ARGS__)
#define WARN_IF(condition, message, ...) LOG(logger::levels::warn::color, logger::levels::warn::LEVEL, logger::levels::warn::NAME, condition, message, ##__VA_ARGS__)
#define ERR_IF(condition, message, ...) LOG(logger::levels::err::color, logger::levels::err::LEVEL, logger::levels::err::NAME, condition, message, ##__VA_ARGS__)

#endif
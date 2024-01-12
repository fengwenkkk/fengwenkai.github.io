/**
 * @file            color_print.hpp
 * @brief           颜色输出print的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-06-03 13:23:34
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_COLOR_PRINT_HPP_
#define GOS_COLOR_PRINT_HPP_

//#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace gos
{
enum log_color
{
    LOG_COLOR_ORIGIN,
    LOG_COLOR_WHITE,
    LOG_COLOR_BLUE,
    LOG_COLOR_GREEN,
    LOG_COLOR_YELLOW,
    LOG_COLOR_RED,
};

#ifdef _WIN32
enum window_color
{
    //ORIGIN_COLOR = FOREGROUND_INTENSITY,                                                       ///< 原色
    ORIGIN_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    WHITE_COLOR  = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,  ///< 白色
    BLUE_COLOR   = FOREGROUND_INTENSITY | FOREGROUND_BLUE,                                       ///< 蓝色
    GREEN_COLOR  = FOREGROUND_INTENSITY | FOREGROUND_GREEN,                                     ///< 绿色
    YELLOW_COLOR = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,                   ///< 黄色
    RED_COLOR    = /*FOREGROUND_INTENSITY | */FOREGROUND_RED,                                         ///< 红色
};

inline WORD GetWindowColor(const log_color& color)
{
    switch (static_cast<int>(color))
    {
        case static_cast<int>(LOG_COLOR_ORIGIN):
            return static_cast<WORD>(ORIGIN_COLOR);
        case static_cast<int>(LOG_COLOR_WHITE):
            return static_cast<WORD>(WHITE_COLOR);
        case static_cast<int>(LOG_COLOR_BLUE):
            return static_cast<WORD>(BLUE_COLOR);
        case static_cast<int>(LOG_COLOR_GREEN):
            return static_cast<WORD>(GREEN_COLOR);
        case static_cast<int>(LOG_COLOR_YELLOW):
            return static_cast<WORD>(YELLOW_COLOR);
        case static_cast<int>(LOG_COLOR_RED):
            return static_cast<WORD>(RED_COLOR);
        default:
            return static_cast<WORD>(ORIGIN_COLOR);
    }
}
#else
inline const char* GetLinuxColor(const log_color& color)
{
    switch (static_cast<int>(color))
    {
        case static_cast<int>(LOG_COLOR_ORIGIN):
            return "\e[0m";
        case static_cast<int>(LOG_COLOR_WHITE):
            return "\e[1;37m";
        case static_cast<int>(LOG_COLOR_BLUE):
            return "\e[1;34m";
        case static_cast<int>(LOG_COLOR_GREEN):
            return "\e[1;32m";
        case static_cast<int>(LOG_COLOR_YELLOW):
            return "\e[1;33m";
        case static_cast<int>(LOG_COLOR_RED):
            return "\e[1;31m";
        default:
            return "\e[0m";
    }
}
#endif

/// 颜色输出函数
inline void printf_color(const gos::log_color& Color, const char* szLog)
{
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowColor(Color));
    printf("%s", szLog);
#ifdef USE_WHITE_COLOR_IN_LOG_TEXT
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowColor(LOG_COLOR_WHITE));
#else
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowColor(LOG_COLOR_ORIGIN));
#endif
#else
    printf("%s%s%s", GetLinuxColor(Color), szLog, GetLinuxColor(LOG_COLOR_ORIGIN));
#endif
}

}  // namespace gos

#endif  ///< GOS_COLOR_PRINT_HPP_

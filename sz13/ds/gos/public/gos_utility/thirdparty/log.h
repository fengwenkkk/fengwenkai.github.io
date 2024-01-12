/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef GOS_THIRDPARTY_LOG_H
#define GOS_THIRDPARTY_LOG_H

#include <stdio.h>
#include <stdarg.h>
#ifdef _BCB_
#include <stdbool.h>
#endif
#include <time.h>
#include <iostream>

#define MAX_CALLBACKS 32

typedef struct
{
    va_list ap;
    const char *fmt;
    const char *file;
    struct tm *time;
    void *udata;
    int line;
    int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum
{
    LOG_TRACE_THIRDPARTY,
    LOG_DEBUG_THIRDPARTY,
    LOG_INFO_THIRDPARTY,
    LOG_WARN_THIRDPARTY,
    LOG_ERROR_THIRDPARTY,
    LOG_FATAL_THIRDPARTY
};

#define LogTrace(...) log_log(LOG_TRACE_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)
#define LogDebug(...) log_log(LOG_DEBUG_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)
#define LogInfo(...) log_log(LOG_INFO_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)
#define LogWarn(...) log_log(LOG_WARN_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)
#define LogError(...) log_log(LOG_ERROR_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)
#define LogFatal(...) log_log(LOG_FATAL_THIRDPARTY, __FILE__, __LINE__, __VA_ARGS__)

/// const char *log_level_string(int level);
/// void log_set_lock(log_LockFn fn, void *udata);
/// void log_set_level(int level);
/// void log_set_quiet(bool enable);
/// int log_add_callback(log_LogFn fn, void *udata, int level);
/// int log_add_fp(FILE *fp, int level);
/// int log_add_simple_fun(FILE *fp, int level);
///
/// void log_log(int level, const char *file, int line, const char *fmt, ...);

typedef struct
{
    log_LogFn fn;
    void *udata;
    int level;
} LogFuncCallback;

struct LoggerInfo
{
    static LoggerInfo &GetInstance()
    {
        static LoggerInfo *pInstance = NULL;
        if (pInstance == NULL)
        {
            pInstance = new LoggerInfo();
        }
        return *pInstance;
    }
    void *udata;
    log_LockFn lock;
    int level;
    bool quiet;
    LogFuncCallback callbacks[MAX_CALLBACKS];
};

static const char *level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

inline static void stdout_callback(log_Event *ev)
{
    char buf[16] = {0};
    // buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
    fprintf((FILE *)ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buf, level_colors[ev->level],
            level_strings[ev->level], ev->file, ev->line);
#else
    fprintf((FILE *)ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file, ev->line);
#endif
    vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
    fprintf((FILE *)ev->udata, "\n");
    fflush((FILE *)ev->udata);
}

inline static void file_callback(log_Event *ev)
{
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf((FILE *)ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file, ev->line);
    vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
    fprintf((FILE *)ev->udata, "\n");
    fflush((FILE *)ev->udata);
}

inline static void simple_file_callback(log_Event *ev)
{
    /// fprintf((FILE *)ev->udata, "%-5s %s:%d: ", level_strings[ev->level], ev->file, ev->line);
    fprintf((FILE *)ev->udata, "\n");
    vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
    fflush((FILE *)ev->udata);
    fprintf((FILE *)ev->udata, "\n");
}

inline static void lock(void)
{
    if (LoggerInfo::GetInstance().lock)
    {
        LoggerInfo::GetInstance().lock(true, LoggerInfo::GetInstance().udata);
    }
}

inline static void unlock(void)
{
    if (LoggerInfo::GetInstance().lock)
    {
        LoggerInfo::GetInstance().lock(false, LoggerInfo::GetInstance().udata);
    }
}

inline const char *log_level_string(int level)
{
    return level_strings[level];
}

inline void log_set_lock(log_LockFn fn, void *udata)
{
    LoggerInfo::GetInstance().lock = fn;
    LoggerInfo::GetInstance().udata = udata;
}

inline void log_set_level(int level)
{
    LoggerInfo::GetInstance().level = level;
}

inline void log_set_quiet(bool enable)
{
    LoggerInfo::GetInstance().quiet = enable;
}

inline int log_add_callback(log_LogFn fn, void *udata, int level)
{
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (!LoggerInfo::GetInstance().callbacks[i].fn)
        {
            LogFuncCallback st;
            st.fn = fn;
            st.udata = udata;
            st.level = level;
            LoggerInfo::GetInstance().callbacks[i] = st;
            /// LoggerInfo::GetInstance().callbacks[i] = (LogFuncCallback){fn, udata, level};
            return 0;
        }
    }
    return -1;
}

inline int log_add_fp(FILE *fp, int level)
{
    return log_add_callback(file_callback, fp, level);
}

inline int log_add_simple_fun(FILE *fp, int level)
{
    return log_add_callback(simple_file_callback, fp, level);
}

inline static void init_event(log_Event *ev, void *udata)
{
    if (!ev->time)
    {
        time_t t = time(NULL);
        ev->time = localtime(&t);
    }
    ev->udata = udata;
}

inline void log_log(int level, const char *file, int line, const char *fmt, ...)
{
    /// log_Event ev = {
    ///     .fmt = fmt,
    ///     .file = file,
    ///     .line = line,
    ///     .level = level,
    /// };
    static bool IsInit = false;
    if (!IsInit)
    {
        IsInit = true;
        char file_name[32];
        sprintf(file_name, "./log/debug.log");
        FILE *fp = fopen(file_name, "a+t");
        if (fp)
        {
            log_set_quiet(true);
            log_add_simple_fun(fp, LOG_TRACE_THIRDPARTY);
        }
    }
    log_Event ev;
    ev.fmt = fmt, ev.file = file, ev.line = line, ev.level = level;

    lock();

    if (!LoggerInfo::GetInstance().quiet && level >= LoggerInfo::GetInstance().level)
    {
        init_event(&ev, stderr);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && LoggerInfo::GetInstance().callbacks[i].fn; i++)
    {
        LogFuncCallback *cb = &LoggerInfo::GetInstance().callbacks[i];
        if (level >= cb->level)
        {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }

    unlock();
}

#endif

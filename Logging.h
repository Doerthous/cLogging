/*******************************************************************************

  Copyright © 2020

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the “Software”), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  Author: doerthous <doerthous@gmail.com>

*******************************************************************************/

#ifndef _LOGGER_H_
#define _LOGGER_H_

# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/**
  * Logging record
  */
typedef struct log_record
{
    struct log_record *next;
    FILE *direction;
    int level;
    const char *level_flag;
    const char *flieline;
    int64_t time;
    struct tm *datetime;
    const char *module;
    void *color_begin;
    void *color_end;
    const char *seperator;
    int message_size;
    int message_len;
    char message;
} log_record_t;
# ifndef LOGGING_LOG_RECORD_SIZE
#  define LOGGING_LOG_RECORD_SIZE 1024
# endif

/******************************************************************************/
// Logging Level
/******************************************************************************/
# define LOGGING_DEBUG_LEVEL 4
# define LOGGING_INFO_LEVEL 3
# define LOGGING_WARN_LEVEL 2
# define LOGGING_ERROR_LEVEL 1
# ifndef LOGGING_LOG_LEVEL
#  define LOGGING_LOG_LEVEL LOGGING_DEBUG_LEVEL
# endif

/******************************************************************************/
// Logging Format
/******************************************************************************/
# ifndef LOGGING_LOG_LEVELFLAG
#  undef LOGGING_DEBUG_FLAG
#  undef LOGGING_INFO_FLAG
#  undef LOGGING_WARN_FLAG
#  undef LOGGING_ERROR_FLAG
#  define LOGGING_DEBUG_FLAG
#  define LOGGING_INFO_FLAG
#  define LOGGING_WARN_FLAG
#  define LOGGING_ERROR_FLAG
#  define LOGGING_GET_LOG_LEVEL(log_record, level, level_flag) (0)
#  define LOGGING_LEVEL_FMT
#  define LOGGING_LEVEL_VAL(log_record)
# else
#  ifndef LOGGING_DEBUG_FLAG
#   define LOGGING_DEBUG_FLAG "[D]"
#  endif
#  ifndef LOGGING_INFO_FLAG
#   define LOGGING_INFO_FLAG "[I]"
#  endif
#  ifndef LOGGING_WARN_FLAG
#   define LOGGING_WARN_FLAG "[W]"
#  endif
#  ifndef LOGGING_ERROR_FLAG
#   define LOGGING_ERROR_FLAG "[E]"
#  endif
   static inline void *LOGGING_GET_LOG_LEVEL(log_record_t *log_record,
    int level, const char *level_flag)
    {
        log_record->level = level;
        log_record->level_flag = level_flag;
        return log_record;
    }
#  define LOGGING_LEVEL_FMT "%s"
#  define LOGGING_LEVEL_VAL(log_record) , (log_record)->level_flag
# endif

# ifndef LOGGING_LOG_DIRECTION
#  define LOGGING_DIRECTION stdout
# else
#  define LOGGING_DIRECTION LOGGING_LOG_DIRECTION
# endif

# ifndef LOGGING_LOG_MODULE
#  define LOGGING_MODULE_FMT
#  define LOGGING_MODULE_VAL(log_record)
#  define LOGGING_GET_LOG_MODULE(log_record) (0)
# else
#  define LOGGING_MODULE_FMT "%s"
#  define LOGGING_MODULE_VAL(log_record) , (log_record)->module
   static inline void *LOGGING_GET_LOG_MODULE(log_record_t *log_record)
    {
        log_record->module = LOGGING_LOG_MODULE;
        return log_record;
    }
# endif

# ifndef LOGGING_LOG_FILELINE
#  define LOGGING_FILELINE_FMT
#  define LOGGING_FILELINE_VAL(log_record)
#  define LOGGING_GET_LOG_FILELINE(log_record) (0)
# else
#  define __STR(x) #x
#  define STR(x) __STR(x)
#  define LOGGING_FILELINE_FMT "%s"
#  define LOGGING_FILELINE_VAL(log_record) , (log_record)->flieline
#  define LOGGING_GET_LOG_FILELINE(log_record) \
    ( \
        /* Here we have to use macro function to get the __LINE__ */ \
        (log_record)->flieline = __FILE__ "(" STR(__LINE__) ")", log_record \
    )
# endif

# ifndef LOGGING_LOG_TIME
#  define LOGGING_GET_LOG_TIME(log_record) (0)
#  define LOGGING_TIME_FMT
#  define LOGGING_TIME_VAL(log_record)
# else
#  define LOGGING_TIME_FMT "[%.6lf]"
#  define LOGGING_TIME_VAL(log_record) , (log_record)->time/1e6
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/time.h>
    static inline void *LOGGING_GET_LOG_TIME(log_record_t *log_record)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        log_record->time = (int64_t)tv.tv_sec * 100000 + tv.tv_usec;
        return log_record;
    }
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
    static inline void *LOGGING_GET_LOG_TIME(log_record_t *log_record)
    {
        FILETIME ft; LARGE_INTEGER li;
        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        log_record->time = (li.QuadPart - 116444736000000000UL)/10;
        return log_record;
    }
#  endif
# endif

# if defined(LOGGING_LOG_DIRECTION) && (LOGGING_LOG_MAX_SIZE > 0)
#  if defined(__linux) || defined(__CYGWIN__)
#   include <unistd.h>
#   define LOGGING_FILE_TRUNCATE(file, size) ftruncate(fileno(file), size)
#  elif defined(_WIN32) || defined(_WIN64)
#   include <io.h>
#   define LOGGING_FILE_TRUNCATE(file, size) _chsize(_fileno(file), size)
#  endif
    #define LOGGING_LOG_ROLLBACK(log_file) do \
    { \
        if (log_file != stdout) { \
            long org = ftell(log_file); \
            fseek(log_file, 0L, SEEK_END); \
            long pos = ftell(log_file); \
            if (pos > LOGGING_LOG_MAX_SIZE) { \
                LOGGING_FILE_TRUNCATE(log_file, 0); \
                fseek(log_file, 0L, SEEK_SET); \
            } \
            else { \
                fseek(log_file, org, SEEK_SET); \
            } \
        } \
    } while (0)
# else
#  define LOGGING_LOG_ROLLBACK(log_file)
# endif

# ifndef LOGGING_LOG_DATETIME
#  define LOGGING_GET_LOG_DATETIME(log_record) (0)
#  define LOGGING_DATETIME_FMT
#  define LOGGING_DATETIME_VAL(log_record)
# else
#  include <time.h>
   static inline void *LOGGING_GET_LOG_DATETIME(log_record_t *log_record)
    {
        time_t tm; time(&tm);
        log_record->datetime = localtime(&tm);
        return log_record;
    }
#  define LOGGING_DATETIME_FMT "[%04d-%02d-%02d %02d:%02d:%02d]"
#  define LOGGING_DATETIME_VAL(log_record) \
    , (log_record)->datetime->tm_year+1900 \
    , (log_record)->datetime->tm_mon \
    , (log_record)->datetime->tm_mday \
    , (log_record)->datetime->tm_hour \
    , (log_record)->datetime->tm_min \
    , (log_record)->datetime->tm_sec
# endif

# if !defined(LOGGING_LOG_COLOR)
#  define LOGGING_DEBUG_COLOR
#  define LOGGING_INFO_COLOR
#  define LOGGING_WARN_COLOR
#  define LOGGING_ERROR_COLOR
#  define LOGGING_GET_LOG_COLOR(log_record, color) (0)
#  define LOGGING_COLOR_BEGIN(log_record)
#  define LOGGING_COLOR_END(log_record)
# else
#  if defined(LOGGING_LOG_DIRECTION)
#   error Color feature only support with stdout
#  endif
#  if defined(__linux) || defined(__CYGWIN__)
#   define LOGGING_DEBUG_COLOR ((void *)"\033[37m")
#   define LOGGING_INFO_COLOR ((void *)"\033[0m")
#   define LOGGING_WARN_COLOR ((void *)"\033[1m\033[33m")
#   define LOGGING_ERROR_COLOR ((void *)"\033[1m\033[31m")
#   define LOGGING_CLEAR_COLOR ((void *)"\033[0m")
#   define LOGGING_COLOR_SET(color) do \
    { \
        fprintf(LOGGING_DIRECTION, "%s", (const char *)color); \
    } while (0)
#  elif defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#   define LOGGING_DEBUG_COLOR ((void *)FOREGROUND_INTENSITY)
#   define LOGGING_INFO_COLOR  ((void *)0x07)
#   define LOGGING_WARN_COLOR  ((void *)0x07)
#   define LOGGING_ERROR_COLOR ((void *)(FOREGROUND_INTENSITY | FOREGROUND_RED))
#   define LOGGING_CLEAR_COLOR ((void *)0x07)
#   define LOGGING_COLOR_SET(color) do \
    { \
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); \
        SetConsoleTextAttribute(handle, (int)color); \
    } while (0)
#  else
#   error Not support platform
#  endif
   static inline void *LOGGING_GET_LOG_COLOR(log_record_t *log_record,
    void *color)
    {
        log_record->color_begin = (void *)color;
        log_record->color_end = (void *)LOGGING_CLEAR_COLOR;
        return log_record;
    }
#  define LOGGING_COLOR_BEGIN(log_record) \
    LOGGING_COLOR_SET((log_record)->color_begin)
#  define LOGGING_COLOR_END(log_record) \
    LOGGING_COLOR_SET((log_record)->color_end)
# endif

# if defined(LOGGING_LOG_LEVELFLAG) || defined(LOGGING_LOG_FILELINE) \
    || defined(LOGGING_LOG_TIME) || defined(LOGGING_LOG_DATETIME) \
    || defined(LOGGING_LOG_MODULE)
#  define FORMAT_SPACE " "
#  define FORMAT_COLON ":" FORMAT_SPACE
#  define LOGGING_LOG_SEPERATOR_FMT "%s"
#  define LOGGING_LOG_SEPERATOR_VAL(log_record) , (log_record)->seperator
   static inline void LOGGING_BUILD_FORMAT(log_record_t *log_record)
    {
        char *format;
        int format_size, format_len, would_written;

        format = &(log_record->message) + log_record->message_len+1;
        format_size = log_record->message_size - log_record->message_len-1;
        would_written = format_len = 0;

        #ifdef LOGGING_LOG_LEVELFLAG
            would_written = snprintf(format, format_size,
                LOGGING_LEVEL_FMT
                LOGGING_LEVEL_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #ifdef LOGGING_LOG_TIME
            would_written = snprintf(format, format_size,
                " " LOGGING_TIME_FMT + !format_len
                LOGGING_TIME_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #ifdef LOGGING_LOG_DATETIME
            would_written = snprintf(format, format_size,
                " " LOGGING_DATETIME_FMT + !format_len
                LOGGING_DATETIME_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #ifdef LOGGING_LOG_MODULE
            would_written = snprintf(format, format_size,
                " " LOGGING_MODULE_FMT + !format_len
                LOGGING_MODULE_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #ifdef LOGGING_LOG_FILELINE
            would_written = snprintf(format, format_size,
                " " LOGGING_FILELINE_FMT + !format_len
                LOGGING_FILELINE_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        would_written = snprintf(format, format_size,
            LOGGING_LOG_SEPERATOR_FMT
            LOGGING_LOG_SEPERATOR_VAL(log_record));
        // assert(would_written < format_size)
        format_len += would_written;
        format_size -= would_written;
        format += would_written;
    }
# else
#  define FORMAT_SPACE ""
#  define FORMAT_COLON "" /*FORMAT_SPACE*/
#  define LOGGING_LOG_SEPERATOR_FMT
#  define LOGGING_LOG_SEPERATOR_VAL(log_record)
#  define LOGGING_BUILD_FORMAT(log_record)
# endif

/******************************************************************************/
// Logging Record Function
/******************************************************************************/
# define LOGGING_LOG_RECORD_WRITE(log_record) do \
{ \
    LOGGING_BUILD_FORMAT(log_record); \
    LOGGING_COLOR_BEGIN(log_record); \
    fprintf((log_record)->direction, "%s%s", \
        &((log_record)->message) + (log_record)->message_len+1, \
        &((log_record)->message)); \
    LOGGING_COLOR_END(log_record); \
    LOGGING_LOG_ROLLBACK((log_record)->direction); \
} while (0)
# if defined(__linux) || defined(__MINGW32__) || defined(__CYGWIN__)
#  define LOGGING_LOG_RECORD_INIT(memory, size, LEVEL) \
    ({ \
        memset(memory, 0, size); \
        ((log_record_t *)memory)->direction = LOGGING_DIRECTION; \
        ((log_record_t *)memory)->message_size = size - sizeof(log_record_t); \
        LOGGING_GET_LOG_LEVEL((log_record_t *)memory, \
            LOGGING_##LEVEL##_LEVEL, LOGGING_##LEVEL##_FLAG); \
        LOGGING_GET_LOG_FILELINE((log_record_t *)memory); \
        LOGGING_GET_LOG_MODULE((log_record_t *)memory); \
        LOGGING_GET_LOG_COLOR((log_record_t *)memory, \
            LOGGING_##LEVEL##_COLOR); \
        LOGGING_GET_LOG_TIME((log_record_t *)memory); \
        LOGGING_GET_LOG_DATETIME((log_record_t *)memory); \
        (log_record_t *)memory; \
    })
# elif defined(_WIN32) || defined(_WIN64)
#  define LOGGING_LOG_RECORD_INIT(memory, size, LEVEL) \
    ( \
        (memset(memory, 0, size)), \
        (((log_record_t *)memory)->direction = LOGGING_DIRECTION), \
        (((log_record_t *)memory)->message_size = size - sizeof(log_record_t)),\
        (LOGGING_GET_LOG_LEVEL((log_record_t *)memory, \
            LOGGING_##LEVEL##_LEVEL, LOGGING_##LEVEL##_FLAG)), \
        (LOGGING_GET_LOG_FILELINE((log_record_t *)memory)), \
        (LOGGING_GET_LOG_MODULE((log_record_t *)memory)), \
        (LOGGING_GET_LOG_COLOR((log_record_t *)memory, \
            LOGGING_##LEVEL##_COLOR)), \
        (LOGGING_GET_LOG_TIME((log_record_t *)memory)), \
        (LOGGING_GET_LOG_DATETIME((log_record_t *)memory)), \
        ((log_record_t *)memory) \
    )
# endif

/******************************************************************************/
// Logging Locking
/******************************************************************************/
# ifdef LOGGING_LOG_LOCKING
#  ifndef LOGGING_LOCK
#   error You need to define LOGGING_LOCK()
#  endif
#  ifndef LOGGING_UNLOCK
#   error You need to define LOGGING_UNLOCK()
#  endif
# else
#  define LOGGING_LOCK()
#  define LOGGING_UNLOCK()
# endif

/******************************************************************************/
// Logging Threading
/******************************************************************************/
# ifdef LOGGING_LOG_THREAD
#  ifndef LOGGING_LOG_RECORD_LIST
#   error You need to define a log_record_t as LOGGING_LOG_RECORD_LIST
#  else
    extern log_record_t LOGGING_LOG_RECORD_LIST;
#  endif
#  ifndef LOGGING_LOG_LOCKING
#   error You need to enable LOGGING_LOG_LOCKING
#  endif
#  define LOGGING_WRITE_RECORD(logger) do \
    { \
        LOGGING_LOCK(); \
        logger->next = LOGGING_LOG_RECORD_LIST.next; \
        LOGGING_LOG_RECORD_LIST.next = logger; \
        LOGGING_UNLOCK(); \
    } while (0)
#  define LOGGING_MALLOC(ptr, size) ptr = (log_record_t*)malloc(size);
#  define LOGGING_FREE(ptr) free(ptr)
#  define LOGGING_THREAD_LOOP(log_record_list) do \
    { \
        LOGGING_LOCK(); \
        log_record_t **log_record = &((log_record_list)->next); \
        while (*log_record) { \
            while ((*log_record)) { \
                if (!(*log_record)->next) { \
                    LOGGING_LOG_RECORD_WRITE(*log_record); \
                    LOGGING_FREE(*log_record); \
                    *log_record = NULL; \
                    break; \
                } \
                log_record = &((*log_record)->next); \
            } \
        } \
        LOGGING_UNLOCK(); \
    } while (0)
# else
#  define LOGGING_WRITE_RECORD(log_record) do \
    { \
        LOGGING_LOCK(); \
        LOGGING_LOG_RECORD_WRITE(log_record); \
        LOGGING_UNLOCK(); \
    } while (0)
#  define LOGGING_MALLOC(ptr, size) char mem[size]; ptr = (log_record_t*)mem
#  define LOGGING_FREE(ptr)
#  define LOGGING_THREAD_LOOP(dummy)
# endif


/******************************************************************************/
// Basic Interfaces
/******************************************************************************/
#include <stdarg.h>
static inline void LOG_LEVEL(log_record_t *logger, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    /* TODO: if message_len > message_size, than error */
    logger->message_len = vsnprintf(
        (&logger->message), logger->message_size, fmt, args);
    va_end(args);
    logger->seperator = FORMAT_COLON;
    LOGGING_WRITE_RECORD(logger);
}
# define PRE_LOG(LEVEL, fmt, ...) do \
{ \
    log_record_t *logger; \
    LOGGING_MALLOC(logger, LOGGING_LOG_RECORD_SIZE); \
    LOGGING_LOG_RECORD_INIT(logger, LOGGING_LOG_RECORD_SIZE, LEVEL); \
    LOG_LEVEL(logger, fmt "\n", ##__VA_ARGS__); \
} while (0)
# if LOGGING_LOG_LEVEL >= LOGGING_DEBUG_LEVEL
#  undef DEBUG /* TODO: such undef may cause problem */
#  define LOG_DEBUG(fmt, ...) PRE_LOG(DEBUG, fmt, ##__VA_ARGS__)
# else
#  define LOG_DEBUG(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_INFO_LEVEL
#  undef INFO
#  define LOG_INFO(fmt, ...) PRE_LOG(INFO, fmt, ##__VA_ARGS__)
# else
#  define LOG_INFO(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_WARN_LEVEL
#  undef WARN
#  define LOG_WARN(fmt, ...) PRE_LOG(WARN, fmt, ##__VA_ARGS__)
# else
#  define LOG_WARN(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_ERROR_LEVEL
#  undef ERROR
#  define LOG_ERROR(fmt, ...) PRE_LOG(ERROR, fmt, ##__VA_ARGS__)
# else
#  define LOG_ERROR(fmt, ...)
# endif

/******************************************************************************/
// Extend Interfaces
/******************************************************************************/
# if LOGGING_LOG_LEVEL >= LOGGING_DEBUG_LEVEL
#  define LOG_BUFFER(msg, buff, cnt) do \
    { \
        log_record_t *logger; \
        LOGGING_MALLOC(logger, LOGGING_LOG_RECORD_SIZE); \
        logger = LOGGING_LOG_RECORD_INIT(logger, \
            LOGGING_LOG_RECORD_SIZE, DEBUG); \
        /* TODO: if message_len > message_size, then error */ \
        logger->message_len += snprintf( \
            (&logger->message)+logger->message_len, \
            logger->message_size-logger->message_len, \
            "%s", msg); \
        for (int i = 0; i < cnt; ++i) { \
            logger->message_len += snprintf( \
                (&logger->message)+logger->message_len, \
                logger->message_size-logger->message_len, \
                " %02X%s"+!i, buff[i], ((i+1)==cnt) ? "\n" : ""); \
        } \
        logger->seperator = FORMAT_SPACE; \
        LOGGING_WRITE_RECORD(logger); \
    } while (0)

#  define LOG_IF(expr, fmt, ...) if (expr) LOG_DEBUG(fmt, ##__VA_ARGS__)
#  define LOG_IF_CHANGED(var, fmt, ...) do \
    { \
        static char mem[sizeof(var)]; \
        if (memcmp(mem, &var, sizeof(mem)) != 0) { \
            memcpy(mem, &var, sizeof(mem)); \
            LOG_DEBUG(fmt, ##__VA_ARGS__); \
        } \
    } while (0)
#  define LOG_ELSE(fmt, ...) else LOG_DEBUG(fmt, ##__VA_ARGS__)
# else
#  define LOG_BUFFER(...)
#  define LOG_IF(...)
#  define LOG_IF_CHANGED(...)
#  define LOG_ELSE(...)
# endif

#endif // _LOGGER_H_

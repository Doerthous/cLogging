/*******************************************************************************

  Copyright © 2019~2023

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
// Logging
#ifndef LOGGING_H_
# define LOGGING_H_

# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stddef.h>

/******************************************************************************/
// Logging Direction
/******************************************************************************/
typedef struct log_direction
{
    struct log_direction *next;
    void *dir;
    void (*write)(void *dir, const void *data, size_t size);
} log_direction_t;

# ifndef LOGGING_LOG_DIRECTION
#  define LOGGING_DIRECTION stdout
# else
#  define LOGGING_DIRECTION LOGGING_LOG_DIRECTION
# endif
# ifndef LOGGING_DIR_WRITE
   static inline void logging_dir_fwrite(void *dir,
                                         const void *data, size_t size)
   {
       fwrite(data, 1, size, (FILE *)dir);
   }
#  define LOGGING_DIR_WRITE logging_dir_fwrite
# endif
#  define LOGGING_GET_LOG_DIRECTION_EX(r,D,w,n) do \
    { \
        (r)->d.dir = D; (r)->d.write = w; (r)->d.next = n; \
    } while (0)
# ifndef LOGGING_LOG_DIRECTION_LIST
#  define LOGGING_LOG_DIRECTION_LIST NULL
# endif
# define LOGGING_GET_LOG_DIRECTION(r) LOGGING_GET_LOG_DIRECTION_EX(r, \
            LOGGING_DIRECTION, LOGGING_DIR_WRITE, LOGGING_LOG_DIRECTION_LIST)

/******************************************************************************/
// Logging Record
/******************************************************************************/
typedef struct log_record
{
    struct log_record *next;
    struct log_direction d;
    int level;
    const char *level_flag;
    const char *flieline;
    const char *function;
    int64_t time;
    struct tm *datetime;
    const char *module;
    const char *seperator;
    int64_t pid;
    int tid;
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
/// Level Flag
# ifndef LOGGING_LOG_LEVELFLAG
#  undef LOGGING_DEBUG_FLAG
#  undef LOGGING_INFO_FLAG
#  undef LOGGING_WARN_FLAG
#  undef LOGGING_ERROR_FLAG
#  define LOGGING_DEBUG_FLAG
#  define LOGGING_INFO_FLAG
#  define LOGGING_WARN_FLAG
#  define LOGGING_ERROR_FLAG
#  define LOGGING_GET_LOG_LEVELFLAG(log_record)
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
   #define LOGGING_GET_LOG_LEVELFLAG(log_record) do \
    { \
        const char *lvl_flag[] = { \
            "", LOGGING_DEBUG_FLAG, LOGGING_INFO_FLAG, \
            LOGGING_WARN_FLAG, LOGGING_ERROR_FLAG, \
        }; \
        (log_record)->level_flag = lvl_flag[(log_record)->level]; \
    } while (0)
#  define LOGGING_LEVEL_FMT "%s"
#  define LOGGING_LEVEL_VAL(log_record) , (log_record)->level_flag
# endif

/// Module
# ifndef LOGGING_LOG_MODULE
#  define LOGGING_MODULE_FMT
#  define LOGGING_MODULE_VAL(log_record)
#  define LOGGING_GET_LOG_MODULE(log_record)
# else
#  define LOGGING_MODULE_FMT "%s"
#  define LOGGING_MODULE_VAL(log_record) , (log_record)->module
#  define LOGGING_GET_LOG_MODULE(log_record) do \
    { \
        (log_record)->module = LOGGING_LOG_MODULE; \
    } while (0)
# endif

/// File & Line
# ifndef LOGGING_LOG_FILELINE
#  define LOGGING_FILELINE_FMT
#  define LOGGING_FILELINE_VAL(log_record)
#  define LOGGING_GET_LOG_FILELINE(log_record)
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

/// Time
# ifndef LOGGING_LOG_TIME
#  define LOGGING_GET_LOG_TIME(log_record)
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

/// Datetime
# ifndef LOGGING_LOG_DATETIME
#  define LOGGING_GET_LOG_DATETIME(log_record)
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
    , (log_record)->datetime->tm_mon+1 \
    , (log_record)->datetime->tm_mday \
    , (log_record)->datetime->tm_hour \
    , (log_record)->datetime->tm_min \
    , (log_record)->datetime->tm_sec
# endif

/// Function
# ifndef LOGGING_LOG_FUNCTION
#  define LOGGING_FUNCTION_FMT "%s"
#  define LOGGING_FUNCTION_VAL(log_record)
#  define LOGGING_GET_LOG_FUNCTION(log_record)
# else
#  define LOGGING_FUNCTION_FMT "%s"
#  define LOGGING_FUNCTION_VAL(log_record) , (log_record)->function
#  define LOGGING_GET_LOG_FUNCTION(log_record) \
    ( \
        (log_record)->function = __FUNCTION__, log_record \
    )
# endif

/// Process ID
# ifndef LOGGING_LOG_PROCID
#  define LOGGING_PROCID_FMT
#  define LOGGING_PROCID_VAL(r)
#  define LOGGING_GET_LOG_PROCID(r)
# else
#  define LOGGING_PROCID_FMT "pid(%d)"
#  define LOGGING_PROCID_VAL(r) , (r)->pid
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/types.h>
#   include <unistd.h>
#   define LOGGING_GET_LOG_PROCID(r) do \
     { (r)->pid = (int64_t)getpid(); } while (0)
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   define LOGGING_GET_LOG_PROCID(r) do \
     { (r)->pid = (int64_t)GetCurrentProcessId(); } while (0)
#  endif
# endif

/// Thread ID
# ifndef LOGGING_LOG_THRDID
#  define LOGGING_THRDID_FMT
#  define LOGGING_THRDID_VAL(r)
#  define LOGGING_GET_LOG_THRDID(r)
# else
   // "Not support thread id now"
#  define LOGGING_THRDID_FMT
#  define LOGGING_THRDID_VAL(r)
#  define LOGGING_GET_LOG_THRDID(r)
# endif

/******************************************************************************/
// Logging Color
/******************************************************************************/
# if !defined(LOGGING_LOG_COLOR)
#  define LOGGING_DEBUG_COLOR
#  define LOGGING_INFO_COLOR
#  define LOGGING_WARN_COLOR
#  define LOGGING_ERROR_COLOR
# else
#  if defined(LOGGING_LOG_DIRECTION)
#   error Color feature only support with stdout
#  endif
#  if defined(__linux) || defined(__CYGWIN__)
#   define LOGGING_DEBUG_COLOR ((void *)"\033[37m")
#   define LOGGING_INFO_COLOR ((void *)"\033[0m")
#   define LOGGING_WARN_COLOR ((void *)"\033[1;33;40m")
#   define LOGGING_ERROR_COLOR ((void *)"\033[1;31;40m")
#   define LOGGING_CLEAR_COLOR ((void *)"\033[0m")
#   define LOGGING_COLOR_SET(color) do \
    { \
        fprintf(stdout, "%s", (const char *)color); \
    } while (0)
#  elif defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#   define LOGGING_DEBUG_COLOR ((void *)FOREGROUND_INTENSITY)
#   define LOGGING_INFO_COLOR  ((void *)0x07)
#   define LOGGING_WARN_COLOR  ((void *)(FOREGROUND_INTENSITY | FOREGROUND_RED \
                                         | FOREGROUND_GREEN))
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
# endif

/******************************************************************************/
// Logging File Truncate Support
/******************************************************************************/
# if defined(LOGGING_LOG_DIRECTION) && (LOGGING_LOG_MAX_SIZE > 0)
#  if defined(__linux) || defined(__CYGWIN__)
#   include <unistd.h>
#   define LOGGING_FILE_TRUNCATE(file, size) ftruncate(fileno(file), size)
#  elif defined(_WIN32) || defined(_WIN64)
#   include <io.h>
#   define LOGGING_FILE_TRUNCATE(file, size) _chsize(_fileno(file), size)
#  endif
#  ifndef LOGGING_LOG_ROLLBACK
    #define LOGGING_LOG_ROLLBACK(_log_file) do \
    { \
        FILE *log_file = (FILE *)_log_file; \
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
#  endif
# else
#  define LOGGING_LOG_ROLLBACK(log_file)
# endif

/******************************************************************************/
// Logging Format Builder
/******************************************************************************/
# if defined(LOGGING_LOG_LEVELFLAG) || defined(LOGGING_LOG_FILELINE) \
    || defined(LOGGING_LOG_TIME) || defined(LOGGING_LOG_DATETIME) \
    || defined(LOGGING_LOG_MODULE) || defined(LOGGING_LOG_FUNCTION) \
    || defined(LOGGING_LOG_PROCID) || defined(LOGGING_LOG_THRDID)
#  define FORMAT_SPACE " "
#  define FORMAT_COLON ":" FORMAT_SPACE
#  define LOGGING_LOG_SEPERATOR_FMT "%s"
#  define LOGGING_LOG_SEPERATOR_VAL(log_record) , (log_record)->seperator
   static inline void LOGGING_BUILD_FORMAT(log_record_t *log_record)
    {
        char *format;
        int format_size, format_len, would_written;

        format = &(log_record->message);
        format_size = log_record->message_size;
        would_written = format_len = 0;

        #ifdef LOGGING_LOG_LEVELFLAG
            would_written = snprintf(format, format_size,
                " " LOGGING_LEVEL_FMT + !format_len
                LOGGING_LEVEL_VAL(log_record));
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

        #ifdef LOGGING_LOG_TIME
            would_written = snprintf(format, format_size,
                " " LOGGING_TIME_FMT + !format_len
                LOGGING_TIME_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #ifdef LOGGING_LOG_PROCID
            would_written = snprintf(format, format_size,
                " " LOGGING_PROCID_FMT + !format_len
                LOGGING_PROCID_VAL(log_record));
            // assert(would_written < format_size)
            format_len += would_written;
            format_size -= would_written;
            format += would_written;
        #endif

        #if defined(LOGGING_LOG_THRDID) && 0
            would_written = snprintf(format, format_size,
                " " LOGGING_THRDID_FMT + !format_len
                LOGGING_THRDID_VAL(log_record));
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

        #ifdef LOGGING_LOG_FUNCTION
            would_written = snprintf(format, format_size,
                " " LOGGING_FUNCTION_FMT + !format_len
                LOGGING_FUNCTION_VAL(log_record));
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

        log_record->message_len = format_len;
    }
# else
#  define FORMAT_SPACE ""
#  define FORMAT_COLON "" /*FORMAT_SPACE*/
#  define LOGGING_LOG_SEPERATOR_FMT
#  define LOGGING_LOG_SEPERATOR_VAL(log_record)
#  define LOGGING_BUILD_FORMAT(log_record)
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
        LOGGING_UNLOCK(); \
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
// Logging Record
/******************************************************************************/
static inline void LOGGING_LOG_RECORD_WRITE(struct log_record *r)
{
    char *msg = &(r->message);
    size_t msg_len = strlen(msg);

    // dir head, color output and file truncate feature are only support on
    // this dir.
# if defined(LOGGING_LOG_COLOR)
    void *color[] = {
        LOGGING_CLEAR_COLOR,
        LOGGING_ERROR_COLOR,
        LOGGING_WARN_COLOR,
        LOGGING_INFO_COLOR,
        LOGGING_DEBUG_COLOR,
    };
    LOGGING_COLOR_SET(color[r->level]);
# endif
    r->d.write(r->d.dir, msg, msg_len);
# if defined(LOGGING_LOG_COLOR)
    LOGGING_COLOR_SET(color[0]);
# endif
    LOGGING_LOG_ROLLBACK(r->d.dir);

    // other dirs
    struct log_direction *dir = r->d.next;
    while (dir) {
        dir->write(dir->dir, msg, msg_len);
        dir = dir->next;
    }
}

/******************************************************************************/
// Functional Interfaces
/******************************************************************************/
#include <stdarg.h>
static inline void LOG_LEVEL(log_record_t *logger, const char *fmt, ...)
{
    va_list args;
    logger->seperator = FORMAT_COLON;
    LOGGING_BUILD_FORMAT(logger);
    va_start(args, fmt);
    /* TODO: if message_len > message_size, than error */
    logger->message_len = vsnprintf(
        &(logger->message)+logger->message_len,
        logger->message_size-logger->message_len, fmt, args);
    va_end(args);

    LOGGING_WRITE_RECORD(logger);
}
# define LOGGING_GET_LOG_INFO(r) do \
{ \
    LOGGING_GET_LOG_DIRECTION(r); \
    LOGGING_GET_LOG_LEVELFLAG(r); \
    LOGGING_GET_LOG_FILELINE(r); \
    LOGGING_GET_LOG_MODULE(r); \
    LOGGING_GET_LOG_TIME(r); \
    LOGGING_GET_LOG_DATETIME(r); \
    LOGGING_GET_LOG_FUNCTION(r); \
    LOGGING_GET_LOG_PROCID(r); \
    LOGGING_GET_LOG_THRDID(r); \
} while (0)
# define LOGGING_DEFAULT_LOG(LEVEL, fmt, ...) do \
{ \
    log_record_t *r; \
    LOGGING_MALLOC(r, LOGGING_LOG_RECORD_SIZE); \
    memset(r, 0, LOGGING_LOG_RECORD_SIZE); \
    r->message_size = LOGGING_LOG_RECORD_SIZE - sizeof(log_record_t); \
    r->level = LEVEL; \
    LOGGING_GET_LOG_INFO(r); \
    LOG_LEVEL(r, fmt "\n", ##__VA_ARGS__); \
} while (0)

/******************************************************************************/
// Dynamic Logging Level
/******************************************************************************/
# ifdef LOGGING_CONF_DYNAMIC_LOG_LEVEL
#  ifdef LOGGING_LOG_MODULE
#   define DEFINED_LOGGING_LOG_MODULE() 1
#  else
#   define DEFINED_LOGGING_LOG_MODULE() 0
#  endif
#  define PRE_LOG(level, fmt, ...) do \
{ \
    char *log_level_str = NULL; \
    int log_level; \
    if (DEFINED_LOGGING_LOG_MODULE()) { \
        log_level_str = getenv(LOGGING_LOG_MODULE "_LOGGING_LOG_LEVEL"); \
        if (log_level_str != NULL) { \
            log_level = atoi(log_level_str); \
            if (log_level < level) break; \
        } \
    } \
    log_level_str = getenv("LOGGING_LOG_LEVEL"); \
    if (log_level_str != NULL) { \
        log_level = atoi(log_level_str); \
        if (log_level < level) break; \
    } \
    LOGGING_DEFAULT_LOG(level, fmt, ##__VA_ARGS__); \
} while (0)
# else
#  define PRE_LOG LOGGING_DEFAULT_LOG
# endif

/******************************************************************************/
// Basic Interfaces
/******************************************************************************/
# if LOGGING_LOG_LEVEL >= LOGGING_DEBUG_LEVEL
#  define LOG_DEBUG(fmt, ...) PRE_LOG(LOGGING_DEBUG_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_DEBUG(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_INFO_LEVEL
#  define LOG_INFO(fmt, ...) PRE_LOG(LOGGING_INFO_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_INFO(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_WARN_LEVEL
#  define LOG_WARN(fmt, ...) PRE_LOG(LOGGING_WARN_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_WARN(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_ERROR_LEVEL
#  define LOG_ERROR(fmt, ...) PRE_LOG(LOGGING_ERROR_LEVEL, fmt, ##__VA_ARGS__)
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
        memset(logger, 0, LOGGING_LOG_RECORD_SIZE); \
        logger->message_size = LOGGING_LOG_RECORD_SIZE-sizeof(log_record_t); \
        logger->level = LOGGING_DEBUG_LEVEL; \
        LOGGING_GET_LOG_INFO(logger); \
        logger->seperator = FORMAT_SPACE; \
        LOGGING_BUILD_FORMAT(logger); \
        /* TODO: if message_len > message_size, then error */ \
        logger->message_len += snprintf( \
            &(logger->message)+logger->message_len,  \
            logger->message_size-logger->message_len, \
            "%s", msg); \
        for (int i = 0; i < cnt; ++i) { \
            logger->message_len += snprintf( \
                &(logger->message)+logger->message_len,   \
                logger->message_size-logger->message_len, \
                " %02X%s"+!i, (uint8_t)(buff[i]), ((i+1)==cnt) ? "\n" : ""); \
        } \
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
#  define LOG_DEBUG_VAR(type, name, init) type name = init
# else
#  define LOG_BUFFER(...)
#  define LOG_IF(...)
#  define LOG_IF_CHANGED(...)
#  define LOG_ELSE(...)
#  define LOG_DEBUG_VAR(type, name, init)
# endif

#else

# define LOG_DEBUG(fmt, ...)
# define LOG_INFO(fmt, ...)
# define LOG_WARN(fmt, ...)
# define LOG_ERROR(fmt, ...)
# define LOG_BUFFER(...)
# define LOG_IF(...)
# define LOG_IF_CHANGED(...)
# define LOG_ELSE(...)
# define LOG_DEBUG_VAR(type, name, init)

#endif // LOGGING_H_

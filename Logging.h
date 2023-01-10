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
# include <stdarg.h>

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
# define LOGGING_GET_LOG_DIRECTION_EX(r,D,w,n) \
  ( \
      (r)->d.dir = D, (r)->d.write = w, (r)->d.next = n, (r) \
  )
# ifndef LOGGING_LOG_DIRECTION_LIST
#  define LOGGING_GET_LOG_DIRECTION(r) LOGGING_GET_LOG_DIRECTION_EX(r, \
           LOGGING_DIRECTION, LOGGING_DIR_WRITE, NULL)
# else
#  define LOGGING_GET_LOG_DIRECTION(r) LOGGING_GET_LOG_DIRECTION_EX(r, \
           LOGGING_DIRECTION, LOGGING_DIR_WRITE, LOGGING_LOG_DIRECTION_LIST)
# endif

/******************************************************************************/
// Logging Record
/******************************************************************************/
typedef struct log_record
{
    #ifdef LOGGING_LOG_THREAD
    struct log_record *next;
    struct log_record *prev;
    #endif

    struct log_direction d;
    int level;
    const char *seperator;

    #ifdef LOGGING_LOG_LEVELFLAG
    const char *level_flag;
    #endif
    #ifdef LOGGING_LOG_FILELINE
    const char *fileline;
    #endif
    #ifdef LOGGING_LOG_FUNCTION
    const char *function;
    #endif
    #ifdef LOGGING_LOG_TIME
    int64_t time;
    #endif
    #ifdef LOGGING_LOG_DATETIME
    struct tm *datetime;
    #endif
    #ifdef LOGGING_LOG_MODULE
    const char *module;
    #endif
    #ifdef LOGGING_LOG_PROCID
    int64_t pid;
    #endif
    #ifdef LOGGING_LOG_THRDID
    int tid;
    #endif

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
#  define LOGGING_GET_LOG_LEVELFLAG(r) (r)
#  define LOGGING_LEVEL_FMT
#  define LOGGING_LEVEL_VAL(r)
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
   static inline log_record_t *LOGGING_GET_LOG_LEVELFLAG(log_record_t *r)
   {
       const char *lvl_flag[] = {
           "", LOGGING_DEBUG_FLAG, LOGGING_INFO_FLAG,
           LOGGING_WARN_FLAG, LOGGING_ERROR_FLAG,
       };
       r->level_flag = lvl_flag[r->level];
       return r;
   }
#  define LOGGING_LEVEL_FMT "%s"
#  define LOGGING_LEVEL_VAL(log_record) , (log_record)->level_flag
# endif

/// Module
# ifndef LOGGING_LOG_MODULE
#  define LOGGING_MODULE_FMT
#  define LOGGING_MODULE_VAL(r)
#  define LOGGING_GET_LOG_MODULE(r) (r)
# else
#  define LOGGING_MODULE_FMT "%s"
#  define LOGGING_MODULE_VAL(r) , (r)->module
#  define LOGGING_GET_LOG_MODULE(r) ((r)->module = LOGGING_LOG_MODULE, (r))
# endif

/// File & Line
# ifndef LOGGING_LOG_FILELINE
#  define LOGGING_FILELINE_FMT
#  define LOGGING_FILELINE_VAL(r)
#  define LOGGING_GET_LOG_FILELINE(r) (r)
# else
#  define __STR(x) #x
#  define STR(x) __STR(x)
#  define LOGGING_FILELINE_FMT "%s"
#  define LOGGING_FILELINE_VAL(r) , (r)->fileline
#  define LOGGING_GET_LOG_FILELINE(r) \
   ( \
       /* Here we have to use macro function to get the __LINE__ */ \
       (r)->fileline = __FILE__ "(" STR(__LINE__) ")", (r) \
   )
# endif

/// Time
# ifndef LOGGING_LOG_TIME
#  define LOGGING_GET_LOG_TIME(r) (r)
#  define LOGGING_TIME_FMT
#  define LOGGING_TIME_VAL(r)
# else
#  define LOGGING_TIME_FMT "[%.6lf]"
#  define LOGGING_TIME_VAL(r) , (r)->time/1e6
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/time.h>
    static inline void *LOGGING_GET_LOG_TIME(log_record_t *r)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        r->time = (int64_t)tv.tv_sec * 100000 + tv.tv_usec;
        return r;
    }
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
    static inline void *LOGGING_GET_LOG_TIME(log_record_t *r)
    {
        FILETIME ft; LARGE_INTEGER li;
        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        r->time = (li.QuadPart - 116444736000000000UL)/10;
        return r;
    }
#  endif
# endif

/// Datetime
# ifndef LOGGING_LOG_DATETIME
#  define LOGGING_GET_LOG_DATETIME(r) (r)
#  define LOGGING_DATETIME_FMT
#  define LOGGING_DATETIME_VAL(r)
# else
#  include <time.h>
   static inline void *LOGGING_GET_LOG_DATETIME(log_record_t *r)
   {
       time_t tm; time(&tm);
       r->datetime = localtime(&tm);
       return r;
   }
#  define LOGGING_DATETIME_FMT "[%04d-%02d-%02d %02d:%02d:%02d]"
#  define LOGGING_DATETIME_VAL(r) \
    , (r)->datetime->tm_year+1900 \
    , (r)->datetime->tm_mon+1 \
    , (r)->datetime->tm_mday \
    , (r)->datetime->tm_hour \
    , (r)->datetime->tm_min \
    , (r)->datetime->tm_sec
# endif

/// Function
# ifndef LOGGING_LOG_FUNCTION
#  define LOGGING_FUNCTION_FMT "%s"
#  define LOGGING_FUNCTION_VAL(r)
#  define LOGGING_GET_LOG_FUNCTION(r) (r)
# else
#  define LOGGING_FUNCTION_FMT "%s"
#  define LOGGING_FUNCTION_VAL(r) , (r)->function
#  define LOGGING_GET_LOG_FUNCTION(r) ((r)->function = __FUNCTION__, (r))
# endif

/// Process ID
# ifndef LOGGING_LOG_PROCID
#  define LOGGING_PROCID_FMT
#  define LOGGING_PROCID_VAL(r)
#  define LOGGING_GET_LOG_PROCID(r) (r)
# else
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/types.h>
#   include <unistd.h>
#   define LOGGING_GETPID getpid
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   define LOGGING_GETPID GetCurrentProcessId
#  endif
#  define LOGGING_PROCID_FMT "pid(%d)"
#  define LOGGING_PROCID_VAL(r) , (r)->pid
#  define LOGGING_GET_LOG_PROCID(r) ((r)->pid = (int64_t)LOGGING_GETPID(), (r))
# endif

/// Thread ID
# ifndef LOGGING_LOG_THRDID
#  define LOGGING_THRDID_FMT
#  define LOGGING_THRDID_VAL(r)
#  define LOGGING_GET_LOG_THRDID(r) (r)
# else
   // "Not support thread id now"
#  define LOGGING_THRDID_FMT
#  define LOGGING_THRDID_VAL(r)
#  define LOGGING_GET_LOG_THRDID(r) (r)
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
#   define LOGGING_LOG_ROLLBACK(_log_file) do \
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
#  define LOGGING_LOG_SEPERATOR_VAL(r) , (r)->seperator
   static inline void LOGGING_BUILD_FORMAT(log_record_t *log_record)
   {
       char *format;
       int format_size, format_len, would_written;

       format = &(log_record->message);
       format_size = log_record->message_size;
       would_written = format_len = 0;

       const char *fnames[] = { "",
           "LVFG", "DTTM", "TIME", "PCID",
           "TRID", "MODU", "FLLN", "FUNC",
       };
       const char *fname = "LVFG DTTM TIME PCID TRID MODU FLLN FUNC";
       int fi;

       enum {
           LOGGING_FMT_LEVELFLAG = 1,
           LOGGING_FMT_DATETIME,
           LOGGING_FMT_TIME,
           LOGGING_FMT_PROCID,
           LOGGING_FMT_THRDID,
           LOGGING_FMT_MODULE,
           LOGGING_FMT_FILELINE,
           LOGGING_FMT_FUNCTION,
       };

       #ifdef LOGGING_CONF_DYNAMIC_LOG_FORMAT
       fnames[0] = getenv("LOGGING_LOG_FORMAT");
       fname = fnames[0] == NULL ? fname : fnames[0];
       # ifdef LOGGING_LOG_MODULE
       fnames[0] = getenv(LOGGING_LOG_MODULE "_LOGGING_LOG_FORMAT");
       fname = fnames[0] == NULL ? fname : fnames[0];
       # endif
       #endif

       while (fname) {
           for (fi = LOGGING_FMT_LEVELFLAG; fi <= LOGGING_FMT_FUNCTION; ++fi) {
               if (*(uint32_t *)fnames[fi] == *(uint32_t *)fname) {
                   break;
               }
           }
           fname = strpbrk(fname, " ");
           fname = fname != NULL ? fname+1 : fname;

           switch (fi) {
               #ifdef LOGGING_LOG_LEVELFLAG
               case LOGGING_FMT_LEVELFLAG: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_LEVEL_FMT + !format_len
                       LOGGING_LEVEL_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_DATETIME
               case LOGGING_FMT_DATETIME: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_DATETIME_FMT + !format_len
                       LOGGING_DATETIME_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_TIME
               case LOGGING_FMT_TIME: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_TIME_FMT + !format_len
                       LOGGING_TIME_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_PROCID
               case LOGGING_FMT_PROCID: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_PROCID_FMT + !format_len
                       LOGGING_PROCID_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #if defined(LOGGING_LOG_THRDID) && 0
               case LOGGING_FMT_THRDID: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_THRDID_FMT + !format_len
                       LOGGING_THRDID_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_MODULE
               case LOGGING_FMT_MODULE: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_MODULE_FMT + !format_len
                       LOGGING_MODULE_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_FILELINE
               case LOGGING_FMT_FILELINE: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_FILELINE_FMT + !format_len
                       LOGGING_FILELINE_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif

               #ifdef LOGGING_LOG_FUNCTION
               case LOGGING_FMT_FUNCTION: {
                   would_written = snprintf(format, format_size,
                       " " LOGGING_FUNCTION_FMT + !format_len
                       LOGGING_FUNCTION_VAL(log_record));
                   // assert(would_written < format_size)
                   format_len += would_written;
                   format_size -= would_written;
                   format += would_written;
               } break;
               #endif
               default: break;
           }
       }

       if (format_len > 0) {
           would_written = snprintf(format, format_size,
               LOGGING_LOG_SEPERATOR_FMT
               LOGGING_LOG_SEPERATOR_VAL(log_record));
           // assert(would_written < format_size)
           format_len += would_written;
           format_size -= would_written;
           format += would_written;
       }

       log_record->message_len = format_len;
   }
# else
#  define FORMAT_SPACE ""
#  define FORMAT_COLON "" /*FORMAT_SPACE*/
#  define LOGGING_LOG_SEPERATOR_FMT
#  define LOGGING_LOG_SEPERATOR_VAL(r)
#  define LOGGING_BUILD_FORMAT(r)
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
#   error You need to define a log_record_t * as LOGGING_LOG_RECORD_LIST
#  else
    extern log_record_t *LOGGING_LOG_RECORD_LIST;
#  endif
#  ifndef LOGGING_LOG_LOCKING
#   error You need to enable LOGGING_LOG_LOCKING
#  endif
#  define LOGGING_WRITE_RECORD(r) do \
   { \
       LOGGING_LOCK(); \
       if (LOGGING_LOG_RECORD_LIST == NULL) { \
           (r)->next = (r); \
           (r)->prev = (r); \
       } \
       else { \
           (r)->next = LOGGING_LOG_RECORD_LIST; \
           LOGGING_LOG_RECORD_LIST->prev->next = (r); \
           (r)->prev = LOGGING_LOG_RECORD_LIST->prev; \
           LOGGING_LOG_RECORD_LIST->prev = (r); \
       } \
       LOGGING_LOG_RECORD_LIST = (r); \
       LOGGING_UNLOCK(); \
   } while (0)
#  define LOGGING_MALLOC(ptr, size) ptr = (log_record_t*)malloc(size);
#  define LOGGING_FREE(ptr) free(ptr)
#  define LOGGING_THREAD_LOOP(record_list) do \
   { \
       log_record *tail_record; \
       LOGGING_LOCK(); \
       if ((record_list) == NULL) { \
           LOGGING_UNLOCK(); \
           break; \
       } \
       tail_record = (record_list)->prev; \
       tail_record->prev->next = (record_list); \
       (record_list)->prev = tail_record->prev; \
       tail_record->next = NULL; \
       tail_record->prev = NULL; \
       if ((record_list)->next == NULL && (record_list)->prev == NULL) { \
           (record_list) = NULL; \
       } \
       LOGGING_UNLOCK(); \
       LOGGING_LOG_RECORD_WRITE(tail_record); \
       LOGGING_FREE(tail_record); \
   } while (0)
# else
#  define LOGGING_WRITE_RECORD(log_record) do \
   { \
       LOGGING_LOCK(); \
       LOGGING_LOG_RECORD_WRITE(log_record); \
       LOGGING_UNLOCK(); \
       LOGGING_FREE(log_record); \
   } while (0)
#  define LOGGING_MALLOC(ptr, size) char mem[size]; ptr = (log_record_t*)mem
#  define LOGGING_FREE(ptr)
#  define LOGGING_THREAD_LOOP(dummy)
# endif

/******************************************************************************/
// Logging Record Functions
/******************************************************************************/
/// init
# define LOGGING_LOG_RECORD_INIT(MEM, SIZE, LEVEL, SEP) \
( \
    memset(MEM, 0, SIZE), \
    ((log_record_t *)(MEM))->message_size = (SIZE)-sizeof(log_record_t), \
    ((log_record_t *)(MEM))->level = (LEVEL), \
    ((log_record_t *)(MEM))->seperator = (SEP), \
    LOGGING_GET_LOG_DIRECTION((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_LEVELFLAG((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_FILELINE((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_MODULE((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_TIME((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_DATETIME((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_FUNCTION((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_PROCID((log_record_t *)(MEM)), \
    LOGGING_GET_LOG_THRDID((log_record_t *)(MEM)), \
    ((log_record_t *)(MEM)) \
)

/// write
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

# ifdef LOGGING_LOG_DIRECTION_LIST
    // other dirs
    struct log_direction *dir = r->d.next;
    while (dir) {
        dir->write(dir->dir, msg, msg_len);
        dir = dir->next;
    }
# endif
}

/// format
static inline void LOGGING_LOG_RECORD_FMT(log_record_t *r, const char *fmt, ...)
{
    va_list args;

    LOGGING_BUILD_FORMAT(r);
    va_start(args, fmt);
    /* TODO: if message_len > message_size, than error */
    r->message_len = vsnprintf(&(r->message)+(r->message_len),
        (r->message_size)-(r->message_len), fmt, args);
    va_end(args);

    LOGGING_WRITE_RECORD(r);
}

/******************************************************************************/
// Dynamic Logging Format
/******************************************************************************/
/* #define LOGGING_CONF_DYNAMIC_LOG_FORMAT to enable this feature */

/******************************************************************************/
// Dynamic Logging Level
/******************************************************************************/
# ifdef LOGGING_CONF_DYNAMIC_LOG_LEVEL
#  ifndef LOGGING_LOG_MODULE
#   define LOGGING_DYNAMIC_LOG_LEVEL_CHECK_M(l)
#  else
#   define LOGGING_DYNAMIC_LOG_LEVEL_CHECK_M(l) \
    { \
        const char *s = LOGGING_LOG_MODULE "_LOGGING_LOG_LEVEL"; \
        if (((s = getenv(s)) != NULL) && (atoi(s) < l)) break; \
    }
#  endif
#  define LOGGING_DYNAMIC_LOG_LEVEL_CHECK(l) \
   { \
       LOGGING_DYNAMIC_LOG_LEVEL_CHECK_M(l); \
       const char *s = "LOGGING_LOG_LEVEL"; \
       if (((s = getenv(s)) != NULL) && (atoi(s) < l)) break;\
   }
# else
#  define LOGGING_DYNAMIC_LOG_LEVEL_CHECK(l)
# endif

// Macro Entry
# define LOG_LEVEL(level, fmt, ...) do \
{ \
    LOGGING_DYNAMIC_LOG_LEVEL_CHECK(level); \
    log_record_t *r; \
    LOGGING_MALLOC(r, LOGGING_LOG_RECORD_SIZE); \
    r = LOGGING_LOG_RECORD_INIT(r, LOGGING_LOG_RECORD_SIZE, \
            level, FORMAT_COLON); \
    LOGGING_LOG_RECORD_FMT(r, fmt "\n", ##__VA_ARGS__); \
} while (0)

/******************************************************************************/
// Basic Interfaces
/******************************************************************************/
# if LOGGING_LOG_LEVEL >= LOGGING_DEBUG_LEVEL
#  define LOG_DEBUG(fmt, ...) LOG_LEVEL(LOGGING_DEBUG_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_DEBUG(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_INFO_LEVEL
#  define LOG_INFO(fmt, ...) LOG_LEVEL(LOGGING_INFO_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_INFO(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_WARN_LEVEL
#  define LOG_WARN(fmt, ...) LOG_LEVEL(LOGGING_WARN_LEVEL, fmt, ##__VA_ARGS__)
# else
#  define LOG_WARN(fmt, ...)
# endif
# if LOGGING_LOG_LEVEL >= LOGGING_ERROR_LEVEL
#  define LOG_ERROR(fmt, ...) LOG_LEVEL(LOGGING_ERROR_LEVEL, fmt, ##__VA_ARGS__)
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
       logger = LOGGING_LOG_RECORD_INIT(logger, LOGGING_LOG_RECORD_SIZE, \
                    LOGGING_DEBUG_LEVEL, FORMAT_SPACE); \
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

#else // #define LOGGING_H_ to disable all interfaces

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

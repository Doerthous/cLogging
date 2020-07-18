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

# include <stdio.h>

/*

## Log Format

[level] [time] [datetime] module [file line]: message

## Usage

```C

// test.c:

#define LOGGING_LOG_LEVEL LOGGING_LOG_DEBUG_LEVEL
#define LOGGING_DEBUG_FLAG "Debug"
#define LOGGING_LOG_MODULE "ModuleA"
#define LOGGING_LOG_FILELINE
#define LOGGING_LOG_TIME
#define LOGGING_LOG_DATETIME
#define LOGGING_LOG_MAX_SIZE 1024
#define LOGGING_LOG_DIRECTION log_file
#include "Logging.h"

int main()
{
    FILE *log_file;

    log_file = fopen("log.txt", "w");
    int b[] = {1,2,3,4,5,6,7,8,9,10};
    LOG_DEBUG("%d", 1);
    LOG_BUFFER("buff: ", b, 10);
    LOG_BUFFER("buff 2: ", b, 10);
    LOG_ERROR("error x %d", 1);
    LOG_ERROR("error ss");
    return 0;
}


// log.txt:

Debug [1595003128.576167] [2020-06-18 00:25:28] ModuleA main.c(20): 1
Debug [1595003128.576592] [2020-06-18 00:25:28] ModuleA main.c(21) buff: 01 02
Debug [1595003128.576673] [2020-06-18 00:25:28] ModuleA main.c(22) buff 2: 01
[E] [1595003128.576740] [2020-06-18 00:25:28] ModuleA main.c(23): error x 1
[E] [1595003128.576804] [2020-06-18 00:25:28] ModuleA main.c(24): error ss

## TODO

- thread-safety

- multi-direction output

```

*/

# define LOGGING_LOG_DEBUG_LEVEL 4
# define LOGGING_LOG_INFO_LEVEL 3
# define LOGGING_LOG_WARN_LEVEL 2
# define LOGGING_LOG_ERROR_LEVEL 1
# ifndef LOGGING_LOG_LEVEL
#  define LOGGING_LOG_LEVEL LOGGING_LOG_DEBUG_LEVEL
#  ifndef LOGGING_DEBUG_FLAG
#   define LOGGING_DEBUG_FLAG ""
#  endif
#  ifndef LOGGING_INFO_FLAG
#   define LOGGING_INFO_FLAG ""
#  endif
#  ifndef LOGGING_WARN_FLAG
#   define LOGGING_WARN_FLAG ""
#  endif
#  ifndef LOGGING_ERROR_FLAG
#   define LOGGING_ERROR_FLAG ""
#  endif
#  define FORMAT_SPACE ""
#  define FORMAT_COLON "" FORMAT_SPACE
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
#  define FORMAT_SPACE " "
#  define FORMAT_COLON ":" FORMAT_SPACE
# endif

# ifndef LOGGING_LOG_DIRECTION
#  define LOGGING_LOG_DIRECTION stdout
# endif

# ifndef LOGGING_LOG_MODULE
#  define __LOGGING_LOG_MODULE ""
# else
#  define __LOGGING_LOG_MODULE " " LOGGING_LOG_MODULE ""
# endif

# ifndef LOGGING_LOG_FILELINE
#  define __LOGGING_LOG_FILELINE ""
# else
#  define __STR(x) #x
#  define STR(x) __STR(x)
#  define __LOGGING_LOG_FILELINE " " __FILE__ "(" STR(__LINE__) ")"
# endif

# ifndef LOGGING_LOG_TIME
#  define LOGGING_GET_LOG_TIME()
#  define LOGGING_LOG_TIME_FMT
#  define LOGGING_LOG_TIME_VAL
# else
#  if defined(__linux)
#   include <sys/time.h>
#   define LOGGING_GET_LOG_TIME() struct timeval tv; gettimeofday(&tv, NULL);
#   define LOGGING_LOG_TIME_FMT " [%ld.%ld]"
#   define LOGGING_LOG_TIME_VAL , tv.tv_sec, tv.tv_usec
#  elif defined(WIN32)
#   include <windows.h>
#   define LOGGING_GET_LOG_TIME() \
        FILETIME ft; LARGE_INTEGER li; int64_t tt = 0; \
        GetSystemTimeAsFileTime(&ft); \
        li.LowPart = ft.dwLowDateTime; \
        li.HighPart = ft.dwHighDateTime; \
        tt = (li.QuadPart - 116444736000000000UL)/10;
#   define LOGGING_LOG_TIME_FMT " [%.6lf]"
#   define LOGGING_LOG_TIME_VAL , tt/1e6
#  endif
# endif

# if (LOGGING_LOG_MAX_SIZE > 0)
#  if defined(__linux)
#   include <unistd.h>
#   define LOGGING_FILE_TRUNCATE(file, size) ftruncate(fileno(file), size)
#  elif defined(WIN32)
#   include <io.h>
#   define LOGGING_FILE_TRUNCATE(file, size) _chsize(_fileno(file), size)
#  endif
    static void LOGGING_LOG_ROLLBACK(FILE *log_file)
    {
        if (log_file != stdout) {
            long org = ftell(log_file);
            fseek(log_file, 0L, SEEK_END);
            long pos = ftell(log_file);
            if (pos > LOGGING_LOG_MAX_SIZE) {
                LOGGING_FILE_TRUNCATE(log_file, 0);
                fseek(log_file, 0L, SEEK_SET);
            }
            else {
                fseek(log_file, org, SEEK_SET);
            }
        }
    }
# else
#  define LOGGING_LOG_ROLLBACK(log_file)
# endif

# ifndef LOGGING_LOG_DATETIME
#  define LOGGING_GET_LOG_DATETIME()
#  define LOGGING_LOG_DATATIME_FMT
#  define LOGGING_LOG_DATATIME_VAL
# else
#  include <time.h>
#  define LOGGING_GET_LOG_DATETIME() \
    time_t tm; time(&tm); struct tm *dt = localtime(&tm)
#  define LOGGING_LOG_DATATIME_FMT " [%04d-%02d-%02d %02d:%02d:%02d]"
#  define LOGGING_LOG_DATATIME_VAL \
    , dt->tm_year+1900, dt->tm_mon, dt->tm_mday \
    , dt->tm_hour, dt->tm_min, dt->tm_sec
# endif

# define LOG_LEVEL(lvl, fmt, ...) do \
{\
    LOGGING_GET_LOG_TIME(); \
    LOGGING_GET_LOG_DATETIME(); \
    fprintf(LOGGING_LOG_DIRECTION, lvl \
        LOGGING_LOG_TIME_FMT LOGGING_LOG_DATATIME_FMT \
        __LOGGING_LOG_MODULE __LOGGING_LOG_FILELINE FORMAT_COLON \
        LOGGING_LOG_TIME_VAL LOGGING_LOG_DATATIME_VAL); \
    fprintf(LOGGING_LOG_DIRECTION, fmt "\n", ##__VA_ARGS__); \
    LOGGING_LOG_ROLLBACK(LOGGING_LOG_DIRECTION); \
} while (0)
# define LOG_BUFFER(msg, buff, cnt) \
do { \
    LOGGING_GET_LOG_TIME(); \
    LOGGING_GET_LOG_DATETIME(); \
    fprintf(LOGGING_LOG_DIRECTION, LOGGING_DEBUG_FLAG \
        LOGGING_LOG_TIME_FMT LOGGING_LOG_DATATIME_FMT \
        __LOGGING_LOG_MODULE __LOGGING_LOG_FILELINE FORMAT_SPACE \
        msg LOGGING_LOG_TIME_VAL LOGGING_LOG_DATATIME_VAL); \
    for (int i = 0; i < cnt; ++i) { \
        fprintf(LOGGING_LOG_DIRECTION, "%02X ", buff[i]); \
    } \
    fprintf(LOGGING_LOG_DIRECTION, "\n"); \
    LOGGING_LOG_ROLLBACK(LOGGING_LOG_DIRECTION); \
} while (0)

/**
  * Logging Level Control
  */
# if LOGGING_LOG_LEVEL >= LOGGING_LOG_DEBUG_LEVEL
#  define LOG_DEBUG(fmt, ...) LOG_LEVEL(LOGGING_DEBUG_FLAG, fmt, ##__VA_ARGS__)
# else
#  define LOG_DEBUG(fmt, ...)
#  undef LOG_BUFFER
#  define LOG_BUFFER(msg, buff, cnt)
# endif

# if LOGGING_LOG_LEVEL >= LOGGING_LOG_INFO_LEVEL
#  define LOG_INFO(fmt, ...) LOG_LEVEL(LOGGING_INFO_FLAG, fmt, ##__VA_ARGS__)
# else
#  define LOG_INFO(fmt, ...)
# endif

# if LOGGING_LOG_LEVEL >= LOGGING_LOG_WARN_LEVEL
#  define LOG_WARN(fmt, ...) LOG_LEVEL(LOGGING_WARN_FLAG, fmt, ##__VA_ARGS__)
# else
#  define LOG_WARN(fmt, ...)
# endif

# if LOGGING_LOG_LEVEL >= LOGGING_LOG_ERROR_LEVEL
#  define LOG_ERROR(fmt, ...) LOG_LEVEL(LOGGING_ERROR_FLAG, fmt, ##__VA_ARGS__)
# else
#  define LOG_ERROR(fmt, ...)
# endif

#endif // _LOGGER_H_

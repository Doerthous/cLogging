/*******************************************************************************

  copyright © 2019~2023

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
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOGGING_H_
# define LOGGING_H_

# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stddef.h>
# include <stdarg.h>

/// Evil Mode
# if defined(LOGGING_AS_HEADER)
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    extern SIGNATURE;
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    extern SIGNATURE;
# elif defined(LOGGING_AS_SOURCE)
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    SIGNATURE __VA_ARGS__
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    extern SIGNATURE;
# else
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    static inline SIGNATURE __VA_ARGS__
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    static inline SIGNATURE;
# endif
# if defined(LOGGING_AS_HEADER) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_EVIL_MODE
# endif

/// Debug Utils
 //# define LOGGING_CONF_DEBUG
# ifdef LOGGING_CONF_DEBUG
#  define LOGGING_PRINTF(...) do \
   { printf(__VA_ARGS__); fflush(stdout); } while (0)
#  define LOGGING_DEBUG_BLOCK(...) __VA_ARGS__
#  ifdef LOGGING_EVIL_MODE
#   define LOGGING_EVIL_DEBUG_BLOCK LOGGING_DEBUG_BLOCK
#  else
#   define LOGGING_EVIL_DEBUG_BLOCK(...)
#  endif
# else
#  define LOGGING_PRINTF(...)
#  define LOGGING_DEBUG_BLOCK(...)
#  define LOGGING_EVIL_DEBUG_BLOCK(...)
# endif

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
   LOGGING_FUNC_DEF(
   void logging_dir_fwrite(void *dir, const void *data, size_t size),
   {
       fwrite(data, 1, size, (FILE *)dir);
   }
   )
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
struct log_format;
typedef struct log_record
{
    #if defined(LOGGING_LOG_THREAD)
    struct log_record *next;
    struct log_record *prev;
    #endif

    struct log_direction d;
    int level;
    const char *seperator;

    struct log_format *fmt;

    size_t mem_size;
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
/// Definition
//// Struct
typedef int (*log_formatter_t)(void *val, char *msg, int msg_len, int first);
typedef struct log_format
{
#if !defined(LOGGING_EVIL_MODE)

# if defined(LOGGING_LOG_LEVELFLAG)
    const char * levelflag;
# endif
# if defined(LOGGING_LOG_FILELINE)
    const char * fileline;
# endif
# if defined(LOGGING_LOG_FUNCTION)
    const char * function;
# endif
# if defined(LOGGING_LOG_TIME)
    int64_t time;
# endif
# if defined(LOGGING_LOG_DATETIME)
    struct tm * datetime;
# endif
# if defined(LOGGING_LOG_MODULE)
    const char * module;
# endif
# if defined(LOGGING_LOG_PROCID)
    int64_t pid;
# endif
# if defined(LOGGING_LOG_THRDID)
    int tid;
# endif
    int count;

# else
    struct log_format *next;
    const char *name;
    log_formatter_t format;
    /* variable info data */
# endif
} log_format_t;
//// EVIL_FILED
LOGGING_FUNC_DCL(void *LOGGING_LOG_RECORD_MALLOC(log_record_t *r, size_t size));
# ifdef LOGGING_EVIL_MODE
   log_format_t *LOGGING_LOG_RECORD_FORMAT_ALLOC(log_record_t *r, size_t s,
       const char *n, log_formatter_t f);
#  define LOGGING_FMT_FIELD_ADD(r, t, n, f) \
   ((t *)(LOGGING_LOG_RECORD_FORMAT_ALLOC(r, sizeof(t), n, f)+1))
#  define LOGGING_FMT_FILED_VAL(f, t) (*(t *)(((log_format_t *)(f))+1))
# endif
//// Format Template
///// LOGGING_FMT_FMT_FILED
# define LOGGING_FMT_FMT_TEMPLATE_DECLARE(NAME) \
  LOGGING_FUNC_DCL(int NAME(void *, char *, int, int))
# define LOGGING_FMT_FMT_TEMPLATE(NAME, FMT, VAL) \
  LOGGING_FUNC_DEF( \
  int NAME(void *r, char *msg, int msg_len, int first), \
  { \
      LOGGING_PRINTF("r: %p, fmt: %s\n", r, FMT); \
      int would_written = snprintf(msg, msg_len, " " FMT + !!(first), VAL(r)); \
      LOGGING_PRINTF("ww: %d\n", would_written); \
      if (would_written > msg_len) { \
          return 0; \
      } \
      return would_written; \
  } \
  )
///// LOGGING_FMT_SET_FILED
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_FMT_SET_TEMPLATE(FIELD, SFIELD, TYPE, NAME, FMTER) \
   LOGGING_FUNC_DEF( \
   void LOGGING_FMT_SET_##FIELD(log_record_t *r, TYPE v), \
   { \
       TYPE *m; \
       m = LOGGING_FMT_FIELD_ADD(r, TYPE, NAME, FMTER); \
       if (m != NULL) *m = v; \
   } \
   )
# else
#  define LOGGING_FMT_SET_TEMPLATE(FIELD, SFIELD, TYPE, NAME, FMTER) \
   LOGGING_FUNC_DEF( \
   void LOGGING_FMT_SET_##FIELD(log_record_t *r, TYPE v), \
   { \
       log_formatter_t *fmter; \
       r->fmt->SFIELD = v; \
       fmter = (log_formatter_t *)LOGGING_LOG_RECORD_MALLOC(r, \
                                     sizeof(log_formatter_t)); \
       if (fmter != NULL) { \
           *fmter = FMTER; \
           (r)->fmt->count += 1; \
       } \
   } \
   )
# endif

///// LOGGING_FMT_GET_FILED
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_FMT_GET_TEMPLATE(FIELD, SFIELD, TYPE) \
   LOGGING_FUNC_DEF( \
   TYPE LOGGING_FMT_GET_##FIELD(void *f), \
   { \
       return LOGGING_FMT_FILED_VAL((log_format_t *)f, TYPE); \
   } \
   )
# else
#  define LOGGING_FMT_GET_TEMPLATE(FIELD, SFIELD, TYPE) \
   LOGGING_FUNC_DEF( \
   TYPE LOGGING_FMT_GET_##FIELD(void *f), \
   { \
       return ((log_format_t *)f)->SFIELD; \
   } \
   )
# endif

///// LOGGING_FMT_FIELD_DEF
# define LOGGING_FMT_FIELD_DEF(FIELD, SFIELD, TYPE, NAME) \
  LOGGING_FMT_FMT_TEMPLATE_DECLARE(LOGGING_FMT_FMT_##FIELD) \
  LOGGING_FMT_SET_TEMPLATE(FIELD, SFIELD, TYPE, NAME, \
      LOGGING_FMT_FMT_##FIELD) \
  LOGGING_FMT_GET_TEMPLATE(FIELD, SFIELD, TYPE) \
  LOGGING_FMT_FMT_TEMPLATE(LOGGING_FMT_FMT_##FIELD, \
      LOGGING_##FIELD##_FMT, LOGGING_##FIELD##_VAL)

/// Level Flag
# if defined(LOGGING_LOG_LEVELFLAG) || defined(LOGGING_AS_SOURCE)
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
#  define LOGGING_LEVELFLAG_FMT "%s"
#  define LOGGING_LEVELFLAG_VAL LOGGING_FMT_GET_LEVELFLAG
   LOGGING_FMT_FIELD_DEF(LEVELFLAG, levelflag, const char *, "LVFG")
   #define LOGGING_GET_LOG_LEVELFLAG(r) do \
   { \
       const char *lvl_flag[] = { \
           "", LOGGING_DEBUG_FLAG, LOGGING_INFO_FLAG, \
           LOGGING_WARN_FLAG, LOGGING_ERROR_FLAG, \
       }; \
       LOGGING_FMT_SET_LEVELFLAG(r, lvl_flag[r->level]); \
   } while (0)
# else
#  define LOGGING_DEBUG_FLAG
#  define LOGGING_INFO_FLAG
#  define LOGGING_WARN_FLAG
#  define LOGGING_ERROR_FLAG
#  define LOGGING_GET_LOG_LEVELFLAG(r) (r)
# endif

/// Module
# if defined(LOGGING_LOG_MODULE) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_MODULE_FMT "%s"
#  define LOGGING_MODULE_VAL LOGGING_FMT_GET_MODULE
   LOGGING_FMT_FIELD_DEF(MODULE, module, const char *, "MODU")
#  define LOGGING_GET_LOG_MODULE(r) do \
   { \
       LOGGING_FMT_SET_MODULE(r, LOGGING_LOG_MODULE); \
   } while (0)
# else
#  define LOGGING_GET_LOG_MODULE(r) (r)
# endif

/// File & Line
# if defined(LOGGING_LOG_FILELINE) || defined(LOGGING_AS_SOURCE)
#  define __STR(x) #x
#  define STR(x) __STR(x)
#  define LOGGING_FILELINE_FMT "%s"
#  define LOGGING_FILELINE_VAL LOGGING_FMT_GET_FILELINE
   LOGGING_FMT_FIELD_DEF(FILELINE, fileline, const char *, "FLLN")
#  define LOGGING_GET_LOG_FILELINE(r) do \
   { \
       /* Here we have to use macro function to get the __LINE__ */ \
       LOGGING_FMT_SET_FILELINE(r, __FILE__ "(" STR(__LINE__) ")"); \
   } while (0)
# else
#  define LOGGING_GET_LOG_FILELINE(r) (r)
# endif

/// Time
# if defined(LOGGING_LOG_TIME) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_TIME_FMT "[%.6lf]"
#  define LOGGING_TIME_VAL(f) LOGGING_FMT_GET_TIME(f)/1e6
   LOGGING_FMT_FIELD_DEF(TIME, time, int64_t, "TIME")
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/time.h>
    LOGGING_FUNC_DEF(
    void *LOGGING_GET_LOG_TIME(log_record_t *r),
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        LOGGING_FMT_SET_TIME(r, (int64_t)tv.tv_sec * 100000 + tv.tv_usec);
        return r;
    }
    )
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
    LOGGING_FUNC_DEF(
    void *LOGGING_GET_LOG_TIME(log_record_t *r),
    {
        FILETIME ft; LARGE_INTEGER li;
        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        LOGGING_FMT_SET_TIME(r, (li.QuadPart - 116444736000000000UL)/10);
        return r;
    }
    )
#  endif
# else
#  define LOGGING_GET_LOG_TIME(r) (r)
# endif

/// Datetime
# if defined(LOGGING_LOG_DATETIME) || defined(LOGGING_AS_SOURCE)
#  include <time.h>
#  define LOGGING_DATETIME_FMT "[%04d-%02d-%02d %02d:%02d:%02d]"
#  define LOGGING_DATETIME_VAL(f) LOGGING_FMT_GET_DATETIME(f)->tm_year+1900 \
   , LOGGING_FMT_GET_DATETIME(f)->tm_mon+1 \
   , LOGGING_FMT_GET_DATETIME(f)->tm_mday \
   , LOGGING_FMT_GET_DATETIME(f)->tm_hour \
   , LOGGING_FMT_GET_DATETIME(f)->tm_min \
   , LOGGING_FMT_GET_DATETIME(f)->tm_sec
   LOGGING_FMT_FIELD_DEF(DATETIME, datetime, struct tm *, "DTTM")
   LOGGING_FUNC_DEF(
   void *LOGGING_GET_LOG_DATETIME(log_record_t *r),
   {
       time_t tm; time(&tm);
       LOGGING_FMT_SET_DATETIME(r, localtime(&tm));
       return r;
   }
   )
# else
#  define LOGGING_GET_LOG_DATETIME(r) (r)
# endif

/// Function
# if defined(LOGGING_LOG_FUNCTION) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_FUNCTION_FMT "%s"
#  define LOGGING_FUNCTION_VAL LOGGING_FMT_GET_FUNCTION
   LOGGING_FMT_FIELD_DEF(FUNCTION, function, const char *, "FUNC")
#  define LOGGING_GET_LOG_FUNCTION(r) LOGGING_FMT_SET_FUNCTION(r, __FUNCTION__)
# else
#  define LOGGING_GET_LOG_FUNCTION(r) (r)
# endif

/// Process ID
# if defined(LOGGING_LOG_PROCID) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_PROCID_FMT "pid(%d)"
#  define LOGGING_PROCID_VAL LOGGING_FMT_GET_PROCID
   LOGGING_FMT_FIELD_DEF(PROCID, pid, int64_t, "PCID")
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/types.h>
#   include <unistd.h>
#   define LOGGING_GETPID() (int64_t)getpid()
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   define LOGGING_GETPID() (int64_t)GetCurrentProcessId()
#  endif
#  define LOGGING_GET_LOG_PROCID(r) LOGGING_FMT_SET_PROCID(r, LOGGING_GETPID())
# else
#  define LOGGING_GET_LOG_PROCID(r) (r)
# endif

/// Thread ID
# ifndef LOGGING_LOG_THRDID
#  define LOGGING_THRDID_FMT
#  define LOGGING_GET_LOG_THRDID(r) (r)
# else
   // "Not support thread id now"
#  define LOGGING_THRDID_FMT
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
    LOGGING_FUNC_DEF(
    void LOGGING_LOG_ROLLBACK(void *_log_file),
    {
        FILE *log_file = (FILE *)_log_file;
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
    )
#  endif
# else
#  define LOGGING_LOG_ROLLBACK(log_file)
# endif

/******************************************************************************/
// Dynamic Logging Format
/******************************************************************************/
/// Format Config String
# ifdef LOGGING_LOG_MODULE
#  define LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr) do \
   { \
       *(pstr) = getenv(LOGGING_LOG_MODULE "_LOGGING_LOG_FORMAT"); \
       LOGGING_PRINTF("%s module format conf str: %p\n", \
                      LOGGING_LOG_MODULE, *(pstr)); \
   } while (0)

# else
#  define LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr)
# endif
# define LOGGING_GET_FORMAT_CONF_STR(pstr) do \
  { \
      LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr); \
      if (*(pstr) != NULL) break; \
      *(pstr) = getenv("LOGGING_LOG_FORMAT"); \
      LOGGING_PRINTF("format conf str: %p\n", *(pstr)); \
  } while (0)

/// Static Format Config
# define LOGGING_INIT_FORMAT_STATIC(RECORD) do \
  { \
      LOGGING_GET_LOG_LEVELFLAG(RECORD); \
      LOGGING_GET_LOG_DATETIME(RECORD); \
      LOGGING_GET_LOG_TIME(RECORD); \
      LOGGING_GET_LOG_PROCID(RECORD); \
      LOGGING_GET_LOG_THRDID(RECORD); \
      LOGGING_GET_LOG_FILELINE(RECORD); \
      LOGGING_GET_LOG_MODULE(RECORD); \
      LOGGING_GET_LOG_FUNCTION(RECORD); \
  } while (0);

/// Dynamic Format Config
# define LOGGING_INIT_FORMAT_DYNAMIC(RECORD) do \
  { \
      const char *fname = NULL; \
      LOGGING_GET_FORMAT_CONF_STR(&fname); \
      if (fname == NULL) { \
          LOGGING_INIT_FORMAT_STATIC(RECORD); \
          break; \
      } \
      LOGGING_PRINTF("FORMAT_CONF_STR: %s\n", fname); \
      while (fname && strlen(fname) >= 4) { \
          do { \
              if (*(int32_t *)fname == *(uint32_t *)"LVFG") { \
                  LOGGING_GET_LOG_LEVELFLAG(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"DTTM") { \
                  LOGGING_GET_LOG_DATETIME(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"TIME") { \
                  LOGGING_GET_LOG_TIME(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"PCID") { \
                  LOGGING_GET_LOG_PROCID(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"TRID") { \
                  LOGGING_GET_LOG_THRDID(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"MODU") { \
                  LOGGING_GET_LOG_MODULE(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"FLLN") { \
                  LOGGING_GET_LOG_FILELINE(RECORD); break; \
              } \
              if (*(int32_t *)fname == *(uint32_t *)"FUNC") { \
                  LOGGING_GET_LOG_FUNCTION(RECORD); break; \
              } \
          } while (0); \
          fname = strpbrk(fname, " "); \
          fname = fname != NULL ? fname+1 : fname; \
      } \
  } while (0)

/// Interface
# ifdef LOGGING_CONF_DYNAMIC_LOG_FORMAT
#  define LOGGING_INIT_FORMAT LOGGING_INIT_FORMAT_DYNAMIC
# else
#  define LOGGING_INIT_FORMAT LOGGING_INIT_FORMAT_STATIC
# endif

/******************************************************************************/
// Logging Format Builder
/******************************************************************************/
# if defined(LOGGING_LOG_LEVELFLAG) || defined(LOGGING_LOG_FILELINE) \
    || defined(LOGGING_LOG_TIME) || defined(LOGGING_LOG_DATETIME) \
    || defined(LOGGING_LOG_MODULE) || defined(LOGGING_LOG_FUNCTION) \
    || defined(LOGGING_LOG_PROCID) || defined(LOGGING_LOG_THRDID) \
    || defined(LOGGING_EVIL_MODE)
/// Get Formatter
#  ifdef LOGGING_EVIL_MODE
    LOGGING_FUNC_DEF(
    void LOGGING_GET_FORMATTERS(log_record_t *r,
                                log_formatter_t fis[], void *fds[], int *fil),
    {
        log_format_t *fmt = (r)->fmt;
        int i = 0;
        while (fmt) {
            LOGGING_PRINTF("format %p ", fmt);
            LOGGING_PRINTF("add %s\n", fmt->name);
            fis[i] = fmt->format; fds[i] = (void *)fmt; i++;
            fmt = fmt->next;
        }
        *(fil) = i;
    }
    )
#  else
    LOGGING_FUNC_DEF(
    void LOGGING_GET_FORMATTERS(log_record_t *r,
                                log_formatter_t fis[], void *fds[], int *fil),
    {
        log_format_t *fmt = r->fmt;
        log_formatter_t *fmters = (log_formatter_t *)fmt;
        for (int i = 0; i < fmt->count; ++i) {
            fis[i] = fmters[-1-i]; fds[i] = (void *)r->fmt;
            LOGGING_PRINTF("fmter: %p\n", fis[i]);
        }
        *(fil) = fmt->count;
    }
    )
#  endif
/// Build Format
#  define FORMAT_SPACE " "
#  define FORMAT_COLON ":" FORMAT_SPACE
#  define LOGGING_LOG_SEPERATOR_FMT "%s"
#  define LOGGING_LOG_SEPERATOR_VAL(r) , ((log_record_t *)(r))->seperator
   LOGGING_FUNC_DEF(
   void LOGGING_BUILD_FORMAT(log_record_t *r),
   {
       LOGGING_EVIL_DEBUG_BLOCK(
           log_format_t *f = r->fmt;
           LOGGING_PRINTF("formats: ");
           while (f) { LOGGING_PRINTF("%s ", f->name); f = f->next; }
           LOGGING_PRINTF("\n");
       )

       log_formatter_t fis[10];
       void *fds [10];
       int fil = 0;
       LOGGING_PRINTF("parser format conf\n");
       LOGGING_GET_FORMATTERS(r, fis, fds, &fil);
       LOGGING_PRINTF("format count: %d\n", fil);

       LOGGING_PRINTF("build format\n");
       int would_written;
       for (int i = 0; i < fil; ++i) {
           if (fis[i]) {
               would_written = fis[i](fds[i],
                   (&(r->message))+(r->message_len),
                   (r->message_size)-(r->message_len),
                   !r->message_len);
               r->message_len += would_written;
           }
       }

       if (r->message_len > 0) {
           would_written = snprintf((&(r->message))+(r->message_len),
               (r->message_size)-(r->message_len),
               LOGGING_LOG_SEPERATOR_FMT LOGGING_LOG_SEPERATOR_VAL(r));
           // assert(would_written < format_size)
           r->message_len += would_written;
       }

       r->message_size = r->mem_size; // free space taken by formats
       LOGGING_PRINTF("format built\n");
   }
   )
# else
#  define FORMAT_SPACE ""
#  define FORMAT_COLON "" /*FORMAT_SPACE*/
#  define LOGGING_LOG_SEPERATOR_FMT
#  define LOGGING_LOG_SEPERATOR_VAL(r)
#  define LOGGING_BUILD_FORMAT(r) LOGGING_PRINTF("not build format\n")
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
/// send record to process thread
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
/// process thread body
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
/// directly write
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
/// malloc
LOGGING_FUNC_DEF(
void *LOGGING_LOG_RECORD_MALLOC(log_record_t *r, size_t size),
{
    uint8_t *msg_end = (uint8_t *)(&(r->message)+(r->message_size));
    LOGGING_PRINTF("msg end: %p\n", msg_end);

    if (r->message_size < size) {
        return NULL;
    }
    r->message_size -= size;

    return msg_end-size;
}
)
/// init
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_LOG_RECORD_FMT_INIT(r) ((r)->fmt = NULL, (r))
# else
#  define LOGGING_LOG_RECORD_FMT_INIT(r) \
   ( \
       (r)->fmt = (log_format_t *) \
           LOGGING_LOG_RECORD_MALLOC(r, sizeof(log_format_t)), (r) \
   )
# endif
# define LOGGING_LOG_RECORD_INIT(RECORD, SIZE, LEVEL, SEP) \
  ( \
      memset(RECORD, 0, SIZE), \
      (RECORD)->mem_size = (SIZE)-sizeof(log_record_t), \
      (RECORD)->message_size = (SIZE)-sizeof(log_record_t), \
      (RECORD)->level = (LEVEL), \
      (RECORD)->seperator = (SEP), \
      LOGGING_LOG_RECORD_FMT_INIT(RECORD), /* alloc space for formats */ \
      LOGGING_GET_LOG_DIRECTION(RECORD), \
      (RECORD) \
  )

/// write
# ifdef LOGGING_LOG_DIRECTION_LIST
#  define LOGGING_OTHER_DIR_ITER(r, m, l) do \
   {
       struct log_direction *dir = (r)->d.next;
       while (dir) {
           dir->write(dir->dir, m, l);
           dir = dir->next;
       }
   }
# else
#  define LOGGING_OTHER_DIR_ITER(r)
# endif
# ifdef LOGGING_LOG_COLOR
#  define LOGGING_WRITE_WITH_COLOR(r, m, l) do \
   { \
       void *color[] = { \
           LOGGING_CLEAR_COLOR, \
           LOGGING_ERROR_COLOR, \
           LOGGING_WARN_COLOR, \
           LOGGING_INFO_COLOR, \
           LOGGING_DEBUG_COLOR, \
       }; \
       LOGGING_COLOR_SET(color[r->level]); \
       (r)->d.write(r->d.dir, m, l); \
       LOGGING_COLOR_SET(color[0]); \
   } while (0)
# else
#  define LOGGING_WRITE_WITH_COLOR(r, m, l) (r)->d.write(r->d.dir, m, l)
# endif
LOGGING_FUNC_DEF(
void LOGGING_LOG_RECORD_WRITE(struct log_record *r),
{
    char *msg = &(r->message);
    size_t msg_len = strlen(msg);

    LOGGING_WRITE_WITH_COLOR(r, msg, msg_len);
    LOGGING_LOG_ROLLBACK(r->d.dir);
    LOGGING_OTHER_DIR_ITER(r)
}
)

/// format
LOGGING_FUNC_DEF(
void LOGGING_LOG_RECORD_FMT(log_record_t *r, const char *fmt, ...),
{
    LOGGING_BUILD_FORMAT(r);

    va_list args;
    va_start(args, fmt);
    /* TODO: if message_len > message_size, than error */
    r->message_len = vsnprintf(&(r->message)+(r->message_len),
        (r->message_size)-(r->message_len), fmt, args);
    va_end(args);

    LOGGING_WRITE_RECORD(r);
}
)

/// malloc format
# if defined(LOGGING_EVIL_MODE)
LOGGING_FUNC_DEF(
log_format_t *LOGGING_LOG_RECORD_FORMAT_ALLOC(log_record_t *r, size_t s,
    const char *n, log_formatter_t f),
{
    log_format_t *fmt, **tail;
    LOGGING_PRINTF("alloc %d+%d, name: %s, fmt: %p\n",
                   sizeof(log_format_t), s, n, f);

    fmt = (log_format_t *)LOGGING_LOG_RECORD_MALLOC(r, sizeof(log_format_t)+s);
    if (fmt != NULL) {
        fmt->next = NULL;
        fmt->name = n;
        fmt->format = f;
        tail = &(r->fmt);
        while (*tail) {
            tail = &((*tail)->next);
        }
        *tail = fmt;
    }

    return fmt;
}
)
# endif

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
    LOGGING_INIT_FORMAT(r); \
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
       LOGGING_INIT_FORMAT(logger); \
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

// All Interfaces (invalid mode)
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

#ifdef __cplusplus
}
#endif

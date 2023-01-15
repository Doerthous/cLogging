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
# if defined(LOGGING_AS_SOURCE)
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    SIGNATURE __VA_ARGS__
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    extern SIGNATURE;
# elif defined(LOGGING_AS_HEADER)
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    extern SIGNATURE;
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    extern SIGNATURE;
# else
#  define LOGGING_FUNC_DEF(SIGNATURE, ...) \
    static inline SIGNATURE __VA_ARGS__
#  define LOGGING_FUNC_DCL(SIGNATURE) \
    static inline SIGNATURE;
# endif

/// Utils
# define LOGGING__STR(x) #x
# define LOGGING_STR(x) LOGGING__STR(x)

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
// Logging Configuration
/******************************************************************************/
# if defined(LOGGING_AS_HEADER) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_EVIL_MODE
# endif
# if (defined(LOGGING_LOG_DIRECTION) && (LOGGING_LOG_MAX_SIZE > 0))
#  define LOGGING_FEAT_FILE_TRUNCATE
# endif
# if defined(LOGGING_LOG_LEVELFLAG) || defined(LOGGING_LOG_FILELINE) \
    || defined(LOGGING_LOG_TIME) || defined(LOGGING_LOG_DATETIME) \
    || defined(LOGGING_LOG_MODULE) || defined(LOGGING_LOG_FUNCTION) \
    || defined(LOGGING_LOG_PROCID) || defined(LOGGING_LOG_THRDID) \
    || defined(LOGGING_EVIL_MODE)
#  define LOGGING_FEAT_WITH_FORMAT
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
# define LOGGING_INIT_DIRECTION(r, l) do \
  { \
      LOGGING_GET_LOG_DIRECTION(r); \
  } while (0)

/******************************************************************************/
// Logging Record
/******************************************************************************/
struct log_record_format;
typedef struct log_record
{
    #if defined(LOGGING_LOG_THREAD) || defined(LOGGING_EVIL_MODE)
    struct log_record *next;
    struct log_record *prev;
    #endif

    struct log_direction d;
    int level;
    const char *seperator;

    struct log_record_format *fmt;

    int mem_size;
    int message_size;
    int message_len;
    char message;
} log_record_t;
# ifndef LOGGING_LOG_RECORD_SIZE
#  define LOGGING_LOG_RECORD_SIZE 1024
# endif

/******************************************************************************/
// Logging Logger
/******************************************************************************/
struct log_logger;
typedef int (*log_format_init_t)(struct log_record *, struct log_logger *);
struct log_logger_format
{
    const char *name;
    log_format_init_t init;
};
# ifndef LOGGING_LOG_LOGGER_FORMAT_COUNT
#  define LOGGING_LOG_LOGGER_FORMAT_COUNT 32
# endif
typedef struct log_logger
{
    const char *name; // LOGGING_LOG_MODULE
    int level;
    const char *levelflag;
    const char *fileline;
    const char *flie;
    int line;
    const char *function;
    size_t format_count;
    const char *format_conf;
    struct log_logger_format formats[LOGGING_LOG_LOGGER_FORMAT_COUNT];
} log_logger_t;

/******************************************************************************/
// Logging Format
/******************************************************************************/
/// Definition
//// Struct
typedef int (*log_format_format_t)(void *val, char *msg, int mlen, int first);
typedef struct log_format_data
{
    void *data;
    log_format_format_t format;
} log_format_data_t;
typedef struct log_record_format
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
    struct log_record_format *next;
    const char *name;
    log_format_format_t format;
    /* variable info data */
# endif
} log_format_t;
//// EVIL_FILED
LOGGING_FUNC_DCL(void *LOGGING_RECORD_MALLOC(log_record_t *r, size_t size));
# ifdef LOGGING_EVIL_MODE
   log_format_t *LOGGING_RECORD_ADD_FORMAT(log_record_t *r, size_t s,
       const char *n, log_format_format_t f);
#  define LOGGING_FMT_FIELD_ADD(r, t, n, f) \
   ((t *)(LOGGING_RECORD_ADD_FORMAT(r, sizeof(t), n, f)+1))
#  define LOGGING_FMT_FILED_VAL(f, t) (*(t *)(((log_format_t *)(f))+1))
# endif
//// Format Template
///// LOGGING_FORMAT_FORMAT
# define LOGGING_FORMAT_FORMAT(FIELD, TYPE, FMT, ...) \
  LOGGING_FUNC_DEF( \
  int LOGGING_FORMAT_FORMAT_##FIELD(void *r, char *m, int mlen, int f), \
  { \
      TYPE v = LOGGING_FORMAT_GET_##FIELD(r); \
      LOGGING_PRINTF("r: %p, fmt: %s\n", r, FMT); \
      int would_written = snprintf(m, mlen, " " FMT + !!(f), ##__VA_ARGS__); \
      LOGGING_PRINTF("ww: %d\n", would_written); \
      if (would_written > mlen) { \
          return 0; \
      } \
      return would_written; \
  } \
  )
///// LOGGING_FORMAT_SET
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_FORMAT_SET(FIELD, SFIELD, TYPE, NAME) \
   LOGGING_FUNC_DEF( \
   void LOGGING_FORMAT_SET_##FIELD(log_record_t *r, TYPE v), \
   { \
       TYPE *m; \
       m = LOGGING_FMT_FIELD_ADD(r, TYPE, NAME, \
                                 LOGGING_FORMAT_FORMAT_##FIELD); \
       if (m != NULL) *m = v; \
   } \
   )
# else
#  define LOGGING_FORMAT_SET(FIELD, SFIELD, TYPE, NAME) \
   LOGGING_FUNC_DEF( \
   void LOGGING_FORMAT_SET_##FIELD(log_record_t *r, TYPE v), \
   { \
       log_format_format_t *fmter; \
       r->fmt->SFIELD = v; \
       fmter = (log_format_format_t *)LOGGING_RECORD_MALLOC(r, \
                                     sizeof(log_format_format_t)); \
       if (fmter != NULL) { \
           *fmter = LOGGING_FORMAT_FORMAT_##FIELD; \
           (r)->fmt->count += 1; \
       } \
   } \
   )
# endif

///// LOGGING_FORMAT_GET
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_FORMAR_GET(FIELD, SFIELD, TYPE) \
   LOGGING_FUNC_DEF( \
   TYPE LOGGING_FORMAT_GET_##FIELD(void *f), \
   { \
       return LOGGING_FMT_FILED_VAL((log_format_t *)f, TYPE); \
   } \
   )
# else
#  define LOGGING_FORMAR_GET(FIELD, SFIELD, TYPE) \
   LOGGING_FUNC_DEF( \
   TYPE LOGGING_FORMAT_GET_##FIELD(void *f), \
   { \
       return ((log_format_t *)f)->SFIELD; \
   } \
   )
# endif


///// LOGGING_FORMAT_INIT
# define LOGGING_FORMAT_INIT(FIELD, COLLECT) \
  LOGGING_FUNC_DEF( \
  int LOGGING_FORMAT_INIT_##FIELD(log_record_t *r, log_logger_t *l), \
  { \
      LOGGING_FORMAT_SET_##FIELD(r, COLLECT); \
      return 1; /* TODO */ \
  } \
  )

///// LOGGING_FORMAT_REGISTER
# define LOGGING_FMT_DEF(FIELD, SFIELD, NAME, TYPE, COLLECT, FMT, ...) \
  LOGGING_FORMAR_GET(FIELD, SFIELD, TYPE) \
  LOGGING_FORMAT_FORMAT(FIELD, TYPE, FMT, ##__VA_ARGS__) \
  LOGGING_FORMAT_SET(FIELD, SFIELD, TYPE, NAME) \
  LOGGING_FORMAT_INIT(FIELD, COLLECT)
# ifdef LOGGING_EVIL_MODE
#  define LOGGING_FORMAT_REGISTER(FIELD, NAME, TYPE, COLLECT, FMT, ...) \
    LOGGING_FMT_DEF(FIELD, _, NAME, TYPE, COLLECT, FMT, ##__VA_ARGS__)
# else
#  define LOGGING_FORMAT_REGISTER(...)
# endif

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
#  define LOGGING_LEVELFLAG_VAL(r) r
   LOGGING_FMT_DEF(LEVELFLAG, levelflag, "LVFG", const char *, l->levelflag,
                   "%s", v)
#  define LOGGING_LEVELFLAG_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "LVFG", LOGGING_FORMAT_INIT_LEVELFLAG)
# else
#  define LOGGING_DEBUG_FLAG
#  define LOGGING_INFO_FLAG
#  define LOGGING_WARN_FLAG
#  define LOGGING_ERROR_FLAG
#  define LOGGING_LEVELFLAG_BUILTIN(l)
# endif

/// Module
# if defined(LOGGING_LOG_MODULE) || defined(LOGGING_AS_SOURCE)
#  define LOGGING_MODULE_VAL(r) r
   LOGGING_FMT_DEF(MODULE, module, "MODU", const char *, l->name, "%s", v)
#  define LOGGING_MODULE_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "MODU", LOGGING_FORMAT_INIT_MODULE)
# else
#  define LOGGING_MODULE_BUILTIN(l)
# endif

/// File & Line
# if defined(LOGGING_LOG_FILELINE) || defined(LOGGING_AS_SOURCE)
#  define __STR(x) #x
#  define STR(x) __STR(x)
#  define LOGGING_FILELINE_VAL(r) r
   LOGGING_FMT_DEF(FILELINE, fileline, "FLLN", const char *, l->fileline,
                   "%s", v)
#  define LOGGING_FILELINE_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "FLLN", LOGGING_FORMAT_INIT_FILELINE)
# else
#  define LOGGING_FILELINE_BUILTIN(l)
# endif

/// Time
# if defined(LOGGING_LOG_TIME) || defined(LOGGING_AS_SOURCE)
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/time.h>
    LOGGING_FUNC_DEF(
    int64_t LOGGING_TIME(),
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (int64_t)tv.tv_sec * 100000 + tv.tv_usec;
    }
    )
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
    LOGGING_FUNC_DEF(
    int64_t LOGGING_TIME(),
    {
        FILETIME ft; LARGE_INTEGER li;
        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        return (li.QuadPart - 116444736000000000UL)/10;
    }
    )
#  endif
   LOGGING_FMT_DEF(TIME, time, "TIME", int64_t, LOGGING_TIME(),
                   "[%.6lf]", v/1e6)
#  define LOGGING_TIME_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "TIME", LOGGING_FORMAT_INIT_TIME)
# else
#  define LOGGING_TIME_BUILTIN(l)
# endif

/// Datetime
# if defined(LOGGING_LOG_DATETIME) || defined(LOGGING_AS_SOURCE)
#  include <time.h>
   LOGGING_FUNC_DEF(
   struct tm *LOGGING_DATETIME(),
   {
       time_t tm; time(&tm);
       return localtime(&tm);
   }
   )
   LOGGING_FMT_DEF(DATETIME, datetime, "DTTM", struct tm *, LOGGING_DATETIME(),
                   "[%04d-%02d-%02d %02d:%02d:%02d]",
                   v->tm_year+1900, v->tm_mon+1, v->tm_mday,
                   v->tm_hour, v->tm_min, v->tm_sec)
#  define LOGGING_DATETIME_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "DTTM", LOGGING_FORMAT_INIT_DATETIME)
# else
#  define LOGGING_DATETIME_BUILTIN(l)
# endif

/// Function
# if defined(LOGGING_LOG_FUNCTION) || defined(LOGGING_AS_SOURCE)
   LOGGING_FMT_DEF(FUNCTION, function, "FUNC", const char *, l->function,
                   "%s", v)
#  define LOGGING_FUNCTION_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "FUNC", LOGGING_FORMAT_INIT_FUNCTION)
# else
#  define LOGGING_FUNCTION_BUILTIN(l)
# endif

/// Process ID
# if defined(LOGGING_LOG_PROCID) || defined(LOGGING_AS_SOURCE)
#  if defined(__linux) || defined(__CYGWIN__)
#   include <sys/types.h>
#   include <unistd.h>
#   define LOGGING_GETPID() (int64_t)getpid()
#  elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   define LOGGING_GETPID() (int64_t)GetCurrentProcessId()
#  endif
#  define LOGGING_PROCID_VAL(d) d
   LOGGING_FMT_DEF(PROCID, pid, "PCID", int64_t, LOGGING_GETPID(), "pid(%d)")
#  define LOGGING_PROCID_BUILTIN(l) \
   LOGGING_LOGGER_ADD_FORMAT(l, "PCID", LOGGING_FORMAT_INIT_PROCID)
# else
#  define LOGGING_PROCID_BUILTIN(l)
# endif

/// Thread ID
# ifndef LOGGING_LOG_THRDID
#  define LOGGING_THRDID_FMT
# else
   // "Not support thread id now"
#  define LOGGING_THRDID_FMT
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
        SetConsoleTextAttribute(handle, (int)(uint64_t)(color)); \
    } while (0)
#  else
#   error Not support platform
#  endif
# endif

/******************************************************************************/
// Logging File Truncate Support
/******************************************************************************/
# if defined(LOGGING_FEAT_FILE_TRUNCATE)
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
// Dynamic Logging Format
/******************************************************************************/
/// Format Config String
# ifndef LOGGING_CONF_DYNAMIC_LOG_FORMAT
#  define LOGGING_GET_FORMAT_CONF_STR(x)
# else
#  ifdef LOGGING_LOG_MODULE
#   define LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr) do \
    { \
        *(pstr) = getenv(LOGGING_LOG_MODULE "_LOGGING_LOG_FORMAT"); \
        LOGGING_PRINTF("%s module format conf str: %p\n", \
                       LOGGING_LOG_MODULE, *(pstr)); \
    } while (0)

#  else
#   define LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr)
#  endif
#  define LOGGING_GET_FORMAT_CONF_STR(pstr) do \
   { \
       LOGGING_GET_MODULE_FORMAT_CONF_STR(pstr); \
       if (*(pstr) != NULL) break; \
       *(pstr) = getenv("LOGGING_LOG_FORMAT"); \
       LOGGING_PRINTF("format conf str: %p\n", *(pstr)); \
   } while (0)
# endif

/// Static Format Config
LOGGING_FUNC_DEF(
void LOGGING_INIT_FORMAT_STATIC(struct log_record *r, struct log_logger *l),
{
    LOGGING_PRINTF("logger format count: %d\n", l->format_count);
    for (int i = 0; i < l->format_count; ++i) {
        LOGGING_PRINTF("%s format init\n", l->formats[i].name);
        l->formats[i].init(r, l);
    }
}
)
/// Dynamic Format Config
LOGGING_FUNC_DCL(log_format_init_t
    LOGGING_LOGGER_GET_FORMAT(struct log_logger *l, const char *name))
LOGGING_FUNC_DEF(
void LOGGING_INIT_FORMAT_DYNAMIC(struct log_record *r, struct log_logger *l),
{
    const char *fname = l->format_conf;
    if (fname == NULL) {
        LOGGING_INIT_FORMAT_STATIC(r, l);
        return;
    }
    LOGGING_PRINTF("FORMAT_CONF_STR: %s\n", fname);

    log_format_init_t init;
    while (fname && strlen(fname) >= 4) {
        init = LOGGING_LOGGER_GET_FORMAT(l, fname);
        if (init) {
            init(r, l);
        }
        fname = strpbrk(fname, " ");
        fname = fname != NULL ? fname+1 : fname;
    }
}
)

/// Interface
# ifdef LOGGING_CONF_DYNAMIC_LOG_FORMAT
#  define LOGGING_INIT_FORMAT LOGGING_INIT_FORMAT_DYNAMIC
# else
#  define LOGGING_INIT_FORMAT LOGGING_INIT_FORMAT_STATIC
# endif

/******************************************************************************/
// Logging Logger Functions
/******************************************************************************/
/// logger_get_caller_context
# ifdef LOGGING_LOG_LEVELFLAG
#  define LOGGING_LOGGER_GET_LEVELFLAG(l) do \
   { \
     const char *lvl_flag[] = { \
         "", LOGGING_DEBUG_FLAG, LOGGING_INFO_FLAG, \
         LOGGING_WARN_FLAG, LOGGING_ERROR_FLAG, \
     }; \
     (l)->levelflag = lvl_flag[(l)->level]; \
   } while (0)
# else
#  define LOGGING_LOGGER_GET_LEVELFLAG(l) (l)->levelflag = NULL;
# endif
# ifdef LOGGING_LOG_MODULE
#  define LOGGING_LOGGER_GET_MODULE(l) (l)->name = LOGGING_LOG_MODULE;
# else
#  define LOGGING_LOGGER_GET_MODULE(l) (l)->name = NULL;
# endif
# define LOGGING_LOGGER_GET_CALLER_CONTEXT(l) do \
  { \
    LOGGING_LOGGER_GET_MODULE(l); \
    LOGGING_LOGGER_GET_LEVELFLAG(l); \
    (l)->fileline = __FILE__ "(" LOGGING_STR(__LINE__) ")"; \
    (l)->function = __FUNCTION__; \
    (l)->flie = __FILE__; \
    (l)->line = __LINE__; \
  } while (0)
/// logger_add_format
LOGGING_FUNC_DEF(
int LOGGING_LOGGER_ADD_FORMAT(struct log_logger *l,
                               const char *name, log_format_init_t init),
{
    if (l->format_count < LOGGING_LOG_LOGGER_FORMAT_COUNT) {
        l->formats[l->format_count].name = name;
        l->formats[l->format_count].init = init;
        l->format_count += 1;
        LOGGING_PRINTF("logger add format: %s, %p\n", name, init);
        return !0;
    }
    return 0;
}
)
/// logger_get_format
LOGGING_FUNC_DEF(
log_format_init_t LOGGING_LOGGER_GET_FORMAT(struct log_logger *l,
                                            const char *name),
{
    for (int i = 0; i < l->format_count; ++i) {
        if (*(uint32_t *)name == *(uint32_t *)(l->formats[i].name)) {
            LOGGING_PRINTF("%s init found\n", l->formats[i].name);
            return l->formats[i].init;
        }
    }
    return NULL;
}
)
/// logger_add_builtin_format
/*
  This should be a macro function so that we can add builtin according to user
  configuration.
*/
# define LOGGING_LOGGER_ADD_BUILTIN_FORMAT(l) do \
{ \
    LOGGING_LEVELFLAG_BUILTIN(l); \
    LOGGING_DATETIME_BUILTIN(l); \
    LOGGING_TIME_BUILTIN(l); \
    LOGGING_PROCID_BUILTIN(l); \
    LOGGING_MODULE_BUILTIN(l); \
    LOGGING_FILELINE_BUILTIN(l); \
    LOGGING_FUNCTION_BUILTIN(l); \
} while (0)

/// logger_add_custom_format
# ifdef LOGGING_CONF_DYNAMIC_LOG_FORMAT
#  ifdef LOGGING_LOGGER_ADD_CUSTOM_FORMAT_AS_FUNCTION
    extern void LOGGING_LOGGER_ADD_CUSTOM_FORMAT(log_logger_t *l);
#  elif !defined(LOGGING_LOGGER_ADD_CUSTOM_FORMAT)
#   define LOGGING_LOGGER_ADD_CUSTOM_FORMAT(l)
#  endif
# else
#  define LOGGING_LOGGER_ADD_CUSTOM_FORMAT(l)
# endif
/// init_logger
# define LOGGING_INIT_LOGGER(l, LEVEL) do \
  { \
      memset(l, 0, sizeof(struct log_logger)); \
      (l)->level = LEVEL; \
      LOGGING_LOGGER_GET_CALLER_CONTEXT(l); \
      LOGGING_LOGGER_ADD_BUILTIN_FORMAT(l); \
      LOGGING_LOGGER_ADD_CUSTOM_FORMAT(l); \
      LOGGING_GET_FORMAT_CONF_STR(&l->format_conf); \
  } while (0)

/******************************************************************************/
// Logging Format Builder
/******************************************************************************/
# if defined(LOGGING_FEAT_WITH_FORMAT)
/// get_format
#  ifdef LOGGING_EVIL_MODE
    LOGGING_FUNC_DEF(
    void LOGGING_GET_FORMAT(log_record_t *r, log_format_data_t fs[], int *fl),
    {
        log_format_t *fmt = (r)->fmt;
        int i = 0;
        while (fmt) {
            LOGGING_PRINTF("format %p ", fmt);
            LOGGING_PRINTF("add %s\n", fmt->name);
            fs[i].format = fmt->format; fs[i].data = (void *)fmt; i++;
            fmt = fmt->next;
        }
        *(fl) = i;
    }
    )
#  else
    LOGGING_FUNC_DEF(
    void LOGGING_GET_FORMAT(log_record_t *r, log_format_data_t fs[], int *fl),
    {
        log_format_t *fmt = r->fmt;
        log_format_format_t *fmters = (log_format_format_t *)fmt;
        for (int i = 0; i < fmt->count; ++i) {
            fs[i].format = fmters[-1-i]; fs[i].data = (void *)r->fmt;
            LOGGING_PRINTF("format: %p\n", fs[i]);
        }
        *(fl) = fmt->count;
    }
    )
#  endif
/// build_format
#  define FORMAT_SPACE " "
#  define FORMAT_COLON ":" FORMAT_SPACE
#  define LOGGING_LOG_SEPERATOR_FMT "%s"
#  define LOGGING_LOG_SEPERATOR_VAL(r) , ((log_record_t *)(r))->seperator
   LOGGING_FUNC_DEF(
   void LOGGING_BUILD_FORMAT(log_record_t *r),
   {
       LOGGING_EVIL_DEBUG_BLOCK(
           log_format_t *f = r->fmt;
           LOGGING_PRINTF("formats: \n");
           while (f) {
               LOGGING_PRINTF("  %p", f);
               LOGGING_PRINTF("  %s\n", f->name);
               f = f->next;
           }
       )

       log_format_data_t fs[10];
       int fl = 0;
       LOGGING_PRINTF("parser format conf\n");
       LOGGING_GET_FORMAT(r, fs, &fl);
       LOGGING_PRINTF("format count: %d\n", fl);

       LOGGING_PRINTF("build format\n");
       int would_written;
       for (int i = 0; i < fl; ++i) {
           if (fs[i].format) {
               would_written = fs[i].format(fs[i].data,
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
// Logging Record Functions
/******************************************************************************/
/// malloc
LOGGING_FUNC_DEF(
void *LOGGING_RECORD_MALLOC(log_record_t *r, size_t size),
{
    uint8_t *msg_end = (uint8_t *)(&(r->message)+(r->message_size));
    LOGGING_PRINTF("msg end: %p\n", msg_end);

    if (r->message_size < size) {
        return NULL;
    }
    r->message_size -= (int)size;

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
           LOGGING_RECORD_MALLOC(r, sizeof(log_format_t)), (r) \
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
       LOGGING_PRINTF("write with color\n"); \
       LOGGING_COLOR_SET(color[r->level]); \
       (r)->d.write(r->d.dir, m, l); \
       LOGGING_COLOR_SET(color[0]); \
   } while (0)
# else
#  define LOGGING_WRITE_WITH_COLOR(r, m, l) (r)->d.write(r->d.dir, m, l)
# endif

# define LOGGING_RECORD_WRITE(r) do \
{ \
    char *msg = &((r)->message); \
    size_t msg_len = strlen(msg); \
    LOGGING_PRINTF("logging record write\n"); \
    LOGGING_WRITE_WITH_COLOR(r, msg, msg_len); \
    LOGGING_LOG_ROLLBACK((r)->d.dir); \
    LOGGING_OTHER_DIR_ITER(r) \
} while (0)

/// record_add_format
# if defined(LOGGING_EVIL_MODE)
LOGGING_FUNC_DEF(
log_format_t *LOGGING_RECORD_ADD_FORMAT(log_record_t *r, size_t s,
    const char *n, log_format_format_t f),
{
    log_format_t *fmt, **tail;
    LOGGING_PRINTF("alloc %d+%d, name: %s, fmt: %p\n",
                   sizeof(log_format_t), s, n, f);

    fmt = (log_format_t *)LOGGING_RECORD_MALLOC(r, sizeof(log_format_t)+s);
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
/// build_format
LOGGING_FUNC_DEF(
void LOGGING_BUILD_RECORD(log_record_t *r, const char *fmt, ...),
{
    LOGGING_BUILD_FORMAT(r);

    va_list args;
    va_start(args, fmt);
    /* TODO: if message_len > message_size, than error */
    r->message_len = vsnprintf(&(r->message)+(r->message_len),
        (r->message_size)-(r->message_len), fmt, args);
    va_end(args);
}
)

/// init_record
# define LOGGING_INIT_RECORD(record, level, seperator) \
  LOGGING_MALLOC(record, LOGGING_LOG_RECORD_SIZE); \
  record = LOGGING_LOG_RECORD_INIT(record, LOGGING_LOG_RECORD_SIZE, \
                                   level, seperator);

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
       LOGGING_RECORD_WRITE(tail_record); \
       LOGGING_FREE(tail_record); \
   } while (0)
# else
/// directly write
#  define LOGGING_WRITE_RECORD(log_record) do \
   { \
       LOGGING_LOCK(); \
       LOGGING_RECORD_WRITE(log_record); \
       LOGGING_UNLOCK(); \
       LOGGING_FREE(log_record); \
   } while (0)
#  define LOGGING_MALLOC(ptr, size) char mem[size]; ptr = (log_record_t*)mem
#  define LOGGING_FREE(ptr)
#  define LOGGING_THREAD_LOOP(dummy)
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
    log_logger_t _l, *l = &_l; \
    LOGGING_INIT_RECORD(r, level, FORMAT_COLON); \
    LOGGING_INIT_LOGGER(l, level); \
    LOGGING_INIT_DIRECTION(r, l); \
    LOGGING_INIT_FORMAT(r, l); \
    LOGGING_BUILD_RECORD(r, fmt "\n", ##__VA_ARGS__); \
    LOGGING_WRITE_RECORD(r); \
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
       log_record_t *r; \
       struct log_logger _l, *l = &_l; \
       LOGGING_INIT_RECORD(r, LOGGING_DEBUG_LEVEL, FORMAT_SPACE); \
       LOGGING_INIT_LOGGER(l, LOGGING_DEBUG_LEVEL); \
       LOGGING_INIT_FORMAT(r, l); \
       LOGGING_INIT_DIRECTION(r, l); \
       LOGGING_BUILD_FORMAT(r); \
       /* TODO: if message_len > message_size, then error */ \
       r->message_len += snprintf( \
           &(r->message)+r->message_len,  \
           r->message_size-r->message_len, \
           "%s", msg); \
       for (int i = 0; i < cnt; ++i) { \
           r->message_len += snprintf( \
               &(r->message)+r->message_len,   \
               r->message_size-r->message_len, \
               " %02X%s"+!i, (uint8_t)(buff[i]), ((i+1)==cnt) ? "\n" : ""); \
       } \
       LOGGING_WRITE_RECORD(r); \
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

/******************************************************************************/
// All Interfaces (invalid mode)
/******************************************************************************/
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

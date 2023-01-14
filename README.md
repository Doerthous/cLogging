## A Simple Header File Only Logging Library

### Features

- Cross Platform
- Logging Level
- Logging Direction (Console or File)
- Logging Format (Level Flag, Timestamp, Datetime, Module, Process ID, File & Line, Funtion name)
- Logging Color
- Multi-Threading
- Multi-Direction (Log to multiple files or console)
- Multi-Processing (On Linux with open option O_APPEND)
- Dynamic Level Control
- Dynamic Log Format Control
- Evil Mode
  You can use Logging as .h and .c file with only one file(Logging.h). e.g:

  use as .h in some modules:
  ```C
  #define LOGGING_LOG_MODULE "ModuleA"
  #define LOGGING_AS_HEADER
  #include "Logging.h"

  LOG_DEBUG("...");
  ```

  use as .c in a module, e.g: logging.c:
  ```C
  #define LOGGING_AS_HEADER
  #include "Logging.h"
  ```

  nm logging.o
  ```sh
  0000000000000758 T LOGGING_BUILD_FORMAT
  0000000000000425 T LOGGING_FMT_FMT_DATETIME
  0000000000000228 T LOGGING_FMT_FMT_FILELINE
  0000000000000598 T LOGGING_FMT_FMT_FUNCTION
  0000000000000096 T LOGGING_FMT_FMT_LEVELFLAG
  000000000000015f T LOGGING_FMT_FMT_MODULE
  0000000000000661 T LOGGING_FMT_FMT_PROCID
  00000000000002f1 T LOGGING_FMT_FMT_TIME
  0000000000000413 T LOGGING_FMT_GET_DATETIME
  0000000000000216 T LOGGING_FMT_GET_FILELINE
  0000000000000586 T LOGGING_FMT_GET_FUNCTION
  0000000000000084 T LOGGING_FMT_GET_LEVELFLAG
  000000000000014d T LOGGING_FMT_GET_MODULE
  000000000000064f T LOGGING_FMT_GET_PROCID
  00000000000002df T LOGGING_FMT_GET_TIME
  00000000000003c6 T LOGGING_FMT_SET_DATETIME
  00000000000001c9 T LOGGING_FMT_SET_FILELINE
  0000000000000539 T LOGGING_FMT_SET_FUNCTION
  0000000000000037 T LOGGING_FMT_SET_LEVELFLAG
  0000000000000100 T LOGGING_FMT_SET_MODULE
  0000000000000602 T LOGGING_FMT_SET_PROCID
  0000000000000292 T LOGGING_FMT_SET_TIME
  00000000000006cb T LOGGING_GET_FORMATTERS
  00000000000004ff T LOGGING_GET_LOG_DATETIME
  0000000000000381 T LOGGING_GET_LOG_TIME
  000000000000096c T LOGGING_LOG_RECORD_FMT
  00000000000009eb T LOGGING_LOG_RECORD_FORMAT_ALLOC
  00000000000008ab T LOGGING_LOG_RECORD_MALLOC
  000000000000091c T LOGGING_LOG_RECORD_WRITE
  ```


### Usage

#### The Simplest Usage

```C
#include "Logging.h"

int main()
{
    LOG_DEBUG("%s", "debug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");

    return 0;
}
```

#### Extend Interfaces

```C
#include "Logging.h"

int main()
{
    int a[] = { 1, 2, 2, 3 };
    const char *ret[] = { "ok", "ok", "error", "ok" };
    for (int i = 0; i < 4; ++i) {
        LOG_IF_CHANGED(a[i], "%s", ret[i]);
    }

    int b = 1;
    LOG_IF(b == 1, "ok");
    LOG_IF(b == 0, "error");
    b = !b;
    LOG_IF(b == 1, "error");
    LOG_IF(b == 0, "ok");

    LOG_BUFFER("a: ", a, 4);

    return 0;
}
```

#### Multi-Processing

see `example/logfile2.cpp`.

### Format

The following formats are supported for logging. Each element can be controlled through macros, and the order of elements can be changed if `LOGGING_CONF_DYNAMIC_LOG_FORMAT` is enable.

```txt
[level] [datetime] [time] [process id] [module] [file line] [function]: message
```

### Configuration

- LOGGING_LOG_LEVEL

  There are four logging levels:

  - LOGGING_DEBUG_LEVEL (4)
  - LOGGING_INFO_LEVEL (3)
  - LOGGING_WARN_LEVEL (2)
  - LOGGING_ERROR_LEVEL (1)

  `LOGGING_LOG_LEVEL` macro control the availability of logging interfaces. The default value is `LOGGING_DEBUG_LEVEL`.

  e.g:

  ```C
  #include "Logging.h"
  LOG_DEBUG("foo"); // foo
  LOG_INFO("bar"); // bar
  ```

  ```C
  #define LOGGING_LOG_LEVEL LOGGING_INFO_LEVEL
  #include "Logging.h"
  LOG_DEBUG("foo"); // This macro will evaluate to an empty line, so "foo" will no output
  LOG_INFO("bar"); // bar
  ```

- LOGGING_LOG_RECORD_SIZE

  This macro control the max size of a single log record. default value is 1024.

- LOGGING_LOG_LEVELFLAG

  This macro will enable logging level flag output.

  ```C
  #define LOGGING_LOG_LEVELFLAG
  #include "logging.h"
  LOG_DEBUG("xxx"); // [D]: xxx
  LOG_INFO("xxx"); // [I]: xxx
  ```

  You can change the logging level flag by define the following macros:

  ```C
  #define LOGGING_DEBUG_FLAG "[DEBUG]"
  #define LOGGING_INFO_FLAG "[INFO]"
  #define LOGGING_WARN_FLAG "[WARNING]"
  #define LOGGING_ERROR_FLAG "[ERROR]"
  #define LOGGING_LOG_LEVELFLAG
  #include "logging.h"
  LOG_DEBUG("xxx"); // [DEBUG]: xxx
  LOG_INFO("xxx"); // [INFO]: xxx
  ```

- LOGGING_LOG_DIRECTION

  `LOGGING_DIRECTION` macro, which defines a FILE * handle, determines where the log record will be written to.

  ```C
  #define LOGGING_LOG_DIRECTION stdout
  #define LOGGING_DIRECTION stdout
  #include "logging.h"
  ```

- LOGGING_LOG_MODULE

  This macro defines a name for module.

  ```C
  #define LOGGING_LOG_MODULE "ModuleA"
  #include "logging.h"
  LOG_DEBUG("xxx"); // ModuleA: xxx
  ```

- LOGGING_LOG_FILELINE

  This macro enable logging with file name and line number.

  ```C
  #define LOGGING_LOG_FILELINE
  #include "logging.h"
  LOG_DEBUG("xxx"); // foo.c(9): xxx
  ```

- LOGGING_LOG_TIME

  This macro enable logging with timestamp.

  ```C
  #define LOGGING_LOG_TIME
  #include "logging.h"
  LOG_DEBUG("xxx"); // [167336785.891470]: xxx
  ```

- LOGGING_LOG_DATETIME

  This macro enable logging with datetime.

  ```C
  #define LOGGING_LOG_DATETIME
  #include "logging.h"
  LOG_DEBUG("xxx"); // [2023-01-11 00:24:11]: xxx
  ```

- LOGGING_LOG_FUNCTION

  This macro enable logging with function name.

  ```C
  #define LOGGING_LOG_FUNCTION
  #include "logging.h"
  LOG_DEBUG("xxx"); // main: xxx
  ```

- LOGGING_LOG_COLOR

  This macro enable logging with different color for different logging level. It can only be used when logging to stdout.

  ```C
  #define LOGGING_LOG_COLOR
  #include "logging.h"
  ```

- LOGGING_LOG_LOCKING
- LOGGING_LOG_THREAD
- LOGGING_CONF_DYNAMIC_LOG_LEVEL

  This macro enable dynamic logging level control. It use two environment variable(module_LOGGING_LOG_LEVEL & LOGGING_LOG_LEVEL) to control logging switch. For example:

  ```C
  #define LOGGING_LOG_MODULE "moduleA"
  #define LOGGING_CONF_DYNAMIC_LOG_LEVEL
  #include "logging.h"
  // ...
  LOG_DEBUG("abc");
  LOG_ERROR("foo");
  ```

  with environment variable set as:

  ```sh
  export moduleA_LOGGING_LOG_LEVEL=2
  ```

  "abc" will not output.

- LOGGING_CONF_DYNAMIC_LOG_FORMAT

  This macro enable dynamic logging format control. It use two environment variable(module_LOGGING_LOG_FORMAT & LOGGING_LOG_FORMAT) to control logging format. There are 7 elements support currently (If enable by LOGGING_LOG_XXX):

  1. Level Flag (LVFG)
  2. Datetime (DTTM)
  3. Time (TIME)
  4. Process ID (PCID)
  5. Thread ID (TRID) // Not support now
  6. Module (MODU)
  7. File & Line (FLLN)
  8. Function (FUNC)

  Suppose you want to format log record as style below:

  ```sh
  [167336785.891470] [2023-01-11 00:24:11] [ERROR] test ../format.c(21) main: debug
  ```

  you can setup the LOGGING_LOG_FORMAT environment as follow:

  ```sh
  export LOGGING_LOG_FORMAT="TIME DTTM LVFG MODU FLLN FUNC"
  ```

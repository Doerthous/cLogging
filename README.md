## A Simple Header File Only Logging Library

### Features

- Cross Platform
- Logging Level
- Logging Direction (Console or File)
- Logging Format (Timestamp, Datetime, Module, File & Line, Funtion name)
- Logging Color

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

### Log Format

The following formats are supported for logging. Each element can be controlled through macros, and the order of elements cannot be changed.

```txt
[level] [time] [datetime] [module] [file line] [function]: message
```

### Log Configuration

- LOGGING_LOG_RECORD_SIZE
  
  This macro control the max size of a single log record. default value is 1024.

- LOGGING_LOG_LEVELFLAG

  This macro will enable logging level flag output.
  ```C
  #define LOGGING_LOG_LEVELFLAG
  #include "logging.h"
  ```
  You can change the logging level flag by define the following macros:
  ```C
  #define LOGGING_DEBUG_FLAG "[DEBUG]"
  #define LOGGING_INFO_FLAG "[INFO]"
  #define LOGGING_WARN_FLAG "[WARNING]"
  #define LOGGING_ERROR_FLAG "[ERROR]"
  #define LOGGING_LOG_LEVELFLAG
  #include "logging.h"
  ```
- LOGGING_LOG_DIRECTION
  
  This macro, which defines a FILE * handle, determines where the log record will be written to.
  ```C
  #define LOGGING_LOG_DIRECTION stdout
  #include "logging.h"
  ```
- LOGGING_LOG_MODULE  

  This macro defines a name for module.
  ```C
  #define LOGGING_LOG_MODULE "ModuleA"
  #include "logging.h"
  ```
- LOGGING_LOG_FILELINE

  This macro enable logging with file name and line number.
  ```C
  #define LOGGING_LOG_FILELINE
  #include "logging.h"
  ```
  
- LOGGING_LOG_TIME

  This macro enable logging with timestamp.
  ```C
  #define LOGGING_LOG_TIME
  #include "logging.h"
  ```
  
- LOGGING_LOG_DATETIME

  This macro enable logging with datetime.
  ```C
  #define LOGGING_LOG_DATETIME
  #include "logging.h"
  ```
  
- LOGGING_LOG_FUNCTION

  This macro enable logging with function name.
  ```C
  #define LOGGING_LOG_FUNCTION
  #include "logging.h"
  ```
  
- LOGGING_LOG_COLOR

  This macro enable logging with different color for different logging level. It can only be used when logging to stdout.
  ```C
  #define LOGGING_LOG_COLOR
  #include "logging.h"
  ```
  
- LOGGING_LOG_LOCKING
- LOGGING_LOG_THREAD

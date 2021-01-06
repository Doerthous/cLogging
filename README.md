## A Simple Header File Only Logging Library

### Features

- Cross Platform
- Logging Level
- Logging Direction (Console or File)
- Logging Format (Timestamp, Datetime, Module, File & Line)
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

[level] [time] [datetime] module [file line]: message

### Log Configuration

- LOGGING_LOG_LEVELFLAG
- LOGGING_LOG_DIRECTION
- LOGGING_LOG_MODULE
- LOGGING_LOG_FILELINE
- LOGGING_LOG_TIME
- LOGGING_LOG_DATETIME
- LOGGING_LOG_COLOR
- LOGGING_LOG_LOCKING
- LOGGING_LOG_THREAD

### TODO

- multi-direction output
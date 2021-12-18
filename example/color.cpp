// #define LOGGING_LOG_FILELINE
// #define LOGGING_LOG_TIME
// #define LOGGING_LOG_DATETIME
#define LOGGING_LOG_COLOR
#include "Logging.h"

int main()
{
    LOG_DEBUG("%s", "debug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");

    return 0;
}
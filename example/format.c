#define LOGGING_LOG_LEVELFLAG
#define LOGGING_LOG_MODULE "test"
#define LOGGING_LOG_FILELINE
#define LOGGING_LOG_TIME
#define LOGGING_LOG_DATETIME
#define LOGGING_LOG_FUNCTION
#include "Logging.h"

int main()
{
    LOG_DEBUG("%s", "debug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");

    return 0;
}
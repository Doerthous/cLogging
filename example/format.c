#define LOGGING_LOG_LEVELFLAG
#define LOGGING_LOG_MODULE "test"
#define LOGGING_LOG_FILELINE
#define LOGGING_LOG_TIME
#define LOGGING_LOG_DATETIME
#define LOGGING_LOG_FUNCTION

#define LOGGING_DEBUG_FLAG "[DEBUG]"
#define LOGGING_INFO_FLAG "[INFO]"
#define LOGGING_WARN_FLAG "[WARNING]"
#define LOGGING_ERROR_FLAG "[ERROR]"

#include "Logging.h"

int main()
{
    LOG_DEBUG("%s", "debug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");

    return 0;
}
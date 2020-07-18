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
    LOG_BUFFER("buff: ", b, 2);
    LOG_BUFFER("buff 2: ", b, 1);
    LOG_ERROR("error x %d", 1);
    LOG_ERROR("error ss");

    return 0;
}
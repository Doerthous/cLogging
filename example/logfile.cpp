#define LOGGING_LOG_TIME
#define LOGGING_LOG_MAX_SIZE 102400
#define LOGGING_LOG_DIRECTION log_file
#include "Logging.h"

int main()
{
    FILE *log_file;

    log_file = fopen("log.txt", "w");

    for (int i = 0; i < 10000; ++i) {
        LOG_DEBUG("%d", i);
    }

    fclose(log_file);

    return 0;
}
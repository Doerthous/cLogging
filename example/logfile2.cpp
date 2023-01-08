#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void logging_dir_write(void *dir, const void *data, size_t len)
{
    write(*(int *)dir, data, len);
}
void logging_dir_fwrite(void *dir, const void *data, size_t size)
{
    fwrite(data, 1, size, *(FILE **)dir);
}

#define LOGGING_LOG_TIME
#define LOGGING_LOG_MAX_SIZE 102400
#define LOGGING_LOG_DIRECTION ((void *)(&fd))
#define LOGGING_DIR_WRITE dw
#define LOGGING_LOG_ROLLBACK(x)
#include "Logging.h"

int main()
{
    void (*dw)(void *, const void *, size_t) = logging_dir_write;
    int fd = open("log2.txt", O_WRONLY|O_APPEND|O_CREAT, 666);

    if (fd <= 0) {
        perror("logfile2: ");
        return -1;
    }

    for (int i = 0; i < 10000; ++i) {
        LOG_DEBUG("%d", i); // log to file

        {
            void (*dw)(void *, const void *, size_t) = logging_dir_fwrite;
            FILE *fd = stdout;
            LOG_DEBUG("%d", i); // log to stdout
        }
    }

    close(fd);

    return 0;
}

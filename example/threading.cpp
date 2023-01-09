#include <mutex>
#include <thread>
#include <chrono>
std::mutex lock;
FILE *log_file;

#define LOGGING_LOG_LEVELFLAG
#define LOGGING_LOG_TIME
#define LOGGING_LOG_LOCKING
#define LOGGING_LOCK() lock.lock()
#define LOGGING_UNLOCK() lock.unlock()
#define LOGGING_LOG_THREAD
#define LOGGING_LOG_RECORD_LIST logging_log_record_list
#define LOGGING_LOG_MAX_SIZE 102400000
//#define LOGGING_LOG_DIRECTION log_file
#include "Logging.h"
log_record_t *logging_log_record_list;

int main()
{
    log_file = fopen("log.txt", "w");

    bool running = true;
    std::thread t4 = std::thread([](){
        for (int i = 0; i < 500; ++i) {
            LOG_INFO("t4 %d", i);
        }
    });
    std::thread t3 = std::thread([](){
        for (int i = 0; i < 500; ++i) {
            LOG_ERROR("t3 %d", i);
        }
    });
    std::thread t2 = std::thread([](){
        for (int j = 0; j < 500; ++j) {
            LOG_WARN("t2 %d", j);
        }
    });
    std::thread t1 = std::thread([&running](){
        while (running) {
            LOGGING_THREAD_LOOP(logging_log_record_list);
        }
    });

    t4.join();
    t3.join();
    t2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    running = false;
    t1.join();

    fclose(log_file);

    return 0;
}

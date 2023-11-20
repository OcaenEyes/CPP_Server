#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "log.h"
#include <pthread.h>

log::log()
{
}

log::~log()
{
}

void *log::async_write_log()
{
}

log* log::get_instance()
{
    
}

void* log::flusg_log_thread(void *args)
{
    
}

bool log::init(const char *file_name, int close_log, int log_buf_size, int split_lines, int max_queue_size)
{
    
}

void log::write_log(int level, const char *format, ...)
{
    
}


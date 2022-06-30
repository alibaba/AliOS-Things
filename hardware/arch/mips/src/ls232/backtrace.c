#include <k_api.h>

int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    return 0;
}

void backtrace_task(void *task, int (*print_func)(const char *fmt, ...))
{
}

int backtrace_caller(char *PC, int *SP, int (*print_func)(const char *fmt, ...))
{
    return 0;
}

int backtrace_callee(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...))
{
    return 0;
}

int backtrace_now_get(void *trace[], int size, int offset)
{
    return 0;
}

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...))
{
}

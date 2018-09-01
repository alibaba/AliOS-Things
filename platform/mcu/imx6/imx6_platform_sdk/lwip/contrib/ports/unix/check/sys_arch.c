#include <sys/time.h>
#include "lwip/opt.h"

u32_t
sys_now(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL))
        return 0;
    else
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

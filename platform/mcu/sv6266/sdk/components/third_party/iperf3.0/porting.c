
//#include <rtos.h>
#include <lwip/sockets.h>
#include "porting.h"
#if 0
const unsigned long ticks_1sec = 1000/os_tick2ms(1);
int iperf_gettimeofday(struct timeval*tv, void *tz)
{
    if (tv == NULL)
        return -1;

    u32 systick;
    systick = OS_GetSysTick();
    tv->tv_sec = systick/ticks_1sec;
    tv->tv_usec = os_tick2ms(systick%ticks_1sec)*1000;
    return 0;
}

char * iperf_strdup (char * s)
{
 char	*t;				/* New string pointer */

  if (s == NULL)
    return (NULL);

  if ((t = malloc(strlen(s) + 1)) == NULL)
    return (NULL);

  return (strcpy(t, s));
}
#endif
int
inet_ntop4(const unsigned char *src, char *dst, size_t size) {
    char tmp[sizeof "255.255.255.255"];   
    static const char *fmt = "%u.%u.%u.%u";
    int tmp_len = sizeof "255.255.255.255";
    if ( (size_t)snprintf(tmp, tmp_len, fmt, src[0], src[1], src[2], src[3]) >= size ) {
        return 0;
    }
    memcpy(dst, tmp, sizeof(tmp));//strcpy(dst, tmp);

    return 1;
}

#ifndef CONFIG_LWIP_VER2_x_x    //lwip2.0.x have this function already!
int
inet_ntop(int af, const void *src, char *dst, size_t size) {
    switch ( af ) {
        case AF_INET:
            return(inet_ntop4(src, dst, size));
#ifdef HAVE_IPV6
        case AF_INET6:
            return(inet_ntop6(src, dst, size));
#endif
        default:
            return 0;
    }
    /* NOTREACHED */
}
#endif

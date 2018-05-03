/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
/* iperf_util.c
 *
 * Iperf utility functions
 *
 */
 
//#include <os_wrapper.h>
#include "iperf_config.h"
//#include <rtos.h>
//#include <log.h>

#include "iperf.h"
//#include "cjson.h"

//#if (IPERF3_ENABLE == 1)

/* make_cookie
 *
 * Generate and return a cookie string
 *
 * Iperf uses this function to create test "cookies" which
 * server as unique test identifiers. These cookies are also
 * used for the authentication of stream connections.
 */

void
make_cookie(char *cookie)
{
    char hostname[COOKIE_SIZE];
    struct timeval tv;
    char temp[200];

    //if ( ! randomized )
    //    srandom(time(NULL));

    /* Generate a string based on hostname, time, randomness, and filler. */
    //(void) gethostname(hostname, sizeof(hostname));
    STRCPY(hostname, "iCommCooperation IOT");
    (void) iperf_gettimeofday(&tv, 0);
    (void) sprintf(temp, "%s.%ld.%06ld.%08lx%08lx.%s", 
            hostname, 
            (unsigned long int) tv.tv_sec, 
            (unsigned long int) tv.tv_usec, 
            (unsigned long int) (tv.tv_sec+tv.tv_usec), 
            (unsigned long int) (tv.tv_sec*tv.tv_usec), 
            "1234567890123456789012345678901234567890");

    /* Now truncate it to 36 bytes and terminate. */
    MEMCPY(cookie, temp, COOKIE_SIZE-1);
    cookie[COOKIE_SIZE-1] = '\0';
}


/* is_closed
 *
 * Test if the file descriptor fd is closed.
 * 
 * Iperf uses this function to test whether a TCP stream socket
 * is closed, because accepting and denying an invalid connection
 * in iperf_tcp_accept is not considered an error.
 */

int
is_closed(int fd)
{
    if (fcntl(fd, F_GETFL, 0) < 0) {
        if (errnoicomm == EBADF)
            return 1;
    }
    return 0;
}

int
timeval_equals(struct timeval * tv0, struct timeval * tv1)
{
    if ( tv0->tv_sec == tv1->tv_sec && tv0->tv_usec == tv1->tv_usec )
    	return 1;
    else
    	return 0;
}

#if 0
double
timeval_diff(struct timeval * tv0, struct timeval * tv1)
{
    double time1, time2;
    
    time1 = tv0->tv_sec - tv1->tv_sec;
    time2 = (tv0->tv_usec - tv1->tv_usec) / 1000000.0;

    time1 += time2;
    if (time1 < 0)
        time1 = -time1;
    return time1;
}
#endif

s32
second_delta(struct timeval * tv0, struct timeval * tv1)
{
    s32 time1, time2;
    
    time1 = tv0->tv_sec - tv1->tv_sec;
    time2 = (tv0->tv_usec - tv1->tv_usec) / 1000000;

    time1 += time2;
    return time1;
}

s32
msecond_delta(struct timeval * tv0, struct timeval * tv1)
{
    s32 time1, time2;
    
    time1 = (tv0->tv_sec - tv1->tv_sec)*1000;
    time2 = (tv0->tv_usec - tv1->tv_usec)/1000;

    time1 += time2;
    return time1;
}
//#endif

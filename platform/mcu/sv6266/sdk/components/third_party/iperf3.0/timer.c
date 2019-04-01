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
 *
 * Based on timers.c by Jef Poskanzer. Used with permission.
 */
//#include <os_wrapper.h>
#include <lwip/sockets.h>
#include "iperf.h"

//#if (IPERF3_ENABLE == 1)

//static Timer* timers = NULL;
//static Timer* free_timers = NULL;

TimerClientData JunkClientData;
const unsigned long ticks_1sec = 1000/os_tick2ms(1);
int iperf_gettimeofday(struct timeval*tv, void *tz)
{
    if (tv == NULL)
        return -1;

    #ifndef __SSV_UNIX_SIM__
    {
    u32 systick;
    systick = OS_GetSysTick();
    tv->tv_sec = systick/ticks_1sec;
    tv->tv_usec = os_tick2ms(systick%ticks_1sec)*1000;
    return 0;
    }
    #else
    return gettimeofday(tv, NULL);    
    #endif    
}

/* This is an efficiency tweak.  All the routines that need to know the
** current time get passed a pointer to a struct timeval.  If it's non-NULL
** it gets used, otherwise we do our own gettimeofday() to fill it in.
** This lets the caller avoid extraneous gettimeofday()s when efficiency
** is needed, and not bother with the extra code when efficiency doesn't
** matter too much.
*/
static void
getnow( struct timeval* nowP, struct timeval* nowP2 )
{
    if ( nowP != NULL )
    	*nowP2 = *nowP;
    else
    	(void) iperf_gettimeofday( nowP2, NULL );
}


static void
t_list_add( Timer* t, void *iperf_test )
{
    Timer* t2;
    Timer* t2prev;
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    if ( test->timers == NULL ) {
    	/* The list is empty. */
    	test->timers = t;
    	t->prev = t->next = NULL;
    } else {
    	if ( t->time.tv_sec < test->timers->time.tv_sec ||
    	     ( t->time.tv_sec == test->timers->time.tv_sec &&
    	       t->time.tv_usec < test->timers->time.tv_usec ) ) {
    	    /* The new timer goes at the head of the list. */
    	    t->prev = NULL;
    	    t->next = test->timers;
    	    test->timers->prev = t;
    	    test->timers = t;
    	} else {
    	    /* Walk the list to find the insertion point. */
    	    for ( t2prev = test->timers, t2 = test->timers->next; t2 != NULL;
    		    t2prev = t2, t2 = t2->next ) {
        		if ( t->time.tv_sec < t2->time.tv_sec ||
    		     ( t->time.tv_sec == t2->time.tv_sec &&
    		       t->time.tv_usec < t2->time.tv_usec ) ) {
        		    /* Found it. */
        		    t2prev->next = t;
        		    t->prev = t2prev;
        		    t->next = t2;
        		    t2->prev = t;
        		    return;
        		}
    	    }
    	    /* Oops, got to the end of the list.  Add to tail. */
    	    t2prev->next = t;
    	    t->prev = t2prev;
    	    t->next = NULL;
    	}
    }
}


static void
t_list_remove( Timer* t, void *iperf_test )
{
    struct iperf_test *test = (struct iperf_test *)iperf_test;
    if ( t->prev == NULL )
    	test->timers = t->next;
    else
    	t->prev->next = t->next;
    if ( t->next != NULL )
    	t->next->prev = t->prev;
}


static void
t_list_resort( Timer* t, void *iperf_test )
{
    /* Remove the timer from the list. */
    t_list_remove( t, iperf_test );
    /* And add it back in, sorted correctly. */
    t_list_add( t, iperf_test );
}


static void
add_msecs( Timer* ptimer, s32 msecs )
{
    struct timeval* t = &ptimer->time;
    struct timeval* t0 = &ptimer->start_time;
    s32 total_msecs = (ptimer->periods+1)*msecs;
    
    t->tv_sec = t0->tv_sec + total_msecs/1000;
    t->tv_usec = t0->tv_usec + (total_msecs % 1000L)*1000;
    
    if ( t->tv_usec >= 1000000L ) {
    	t->tv_sec += t->tv_usec / 1000000L;
    	t->tv_usec %= 1000000L;
    }
}


Timer*
tmr_create(
    struct timeval* nowP, TimerProc* timer_proc, TimerClientData client_data,
    s32 msecs, int periodic, void *iperf_test )
{
    struct timeval now;
    Timer* t;
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    getnow( nowP, &now );

    if ( test->free_timers != NULL ) {
    	t = test->free_timers;
    	test->free_timers = t->next;
    } else {
    	t = (Timer*) iperf_malloc( sizeof(Timer) );
    	if ( t == NULL )
    	    return NULL;
    }

    t->timer_proc = timer_proc;
    t->client_data = client_data;
    t->msecs = msecs;
    t->periodic = periodic;
    t->periods = 0;
    t->time = now;
    t->start_time = now;
    add_msecs( t, t->msecs );
    /* Add the new timer to the active list. */
    t_list_add( t, test );

    return t;
}


struct timeval*
tmr_timeout( struct timeval* nowP, void *iperf_test )
{
    struct timeval now;
    s32 usecs;
    static struct timeval timeout;
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    getnow( nowP, &now );
    /* Since the list is sorted, we only need to look at the first timer. */
    if ( test->timers == NULL )
    	return NULL;
    usecs = ( test->timers->time.tv_sec - now.tv_sec ) * 1000000L +
    	    ( test->timers->time.tv_usec - now.tv_usec );
    if ( usecs <= 0 )
    	usecs = 0;
    timeout.tv_sec = usecs / 1000000L;
    timeout.tv_usec = usecs % 1000000L;
    return &timeout;
}


void
tmr_run( struct timeval* nowP, void *iperf_test )
{
    struct timeval now;
    Timer* t;
    Timer* next;
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    getnow( nowP, &now );
    for ( t = test->timers; t != NULL; t = next ) {
    	next = t->next;
    	/* Since the list is sorted, as soon as we find a timer
    	       ** that isn't ready yet, we are done.
    	       */
    	if ( t->time.tv_sec > now.tv_sec ||
    	     ( t->time.tv_sec == now.tv_sec &&
    	       t->time.tv_usec > now.tv_usec ) )
    	    break;
    	(t->timer_proc)( t->client_data, &now );
    	if ( t->periodic ) {
    	    /* Reschedule. */
    	    t->periods++;
    	    add_msecs( t, t->msecs );
    	    t_list_resort( t, test );
    	} else
    	    tmr_cancel( t, test );
    }
}


void
tmr_reset( struct timeval* nowP, Timer* t, void *iperf_test )
{
    struct timeval now;
    struct iperf_test *test = (struct iperf_test *)iperf_test;
    
    getnow( nowP, &now );
    t->time = now;
    t->start_time = now;
    t->periods = 0;
    add_msecs( t, t->msecs );
    t_list_resort( t, test);
}


void
tmr_cancel( Timer* t, void *iperf_test )
{
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    /* Remove it from the active list. */
    t_list_remove( t, test );
    /* And put it on the free list. */
    t->next = test->free_timers;
    test->free_timers = t;
    t->prev = NULL;
}


void
tmr_cleanup( void *iperf_test )
{
    Timer* t;
    struct iperf_test *test = (struct iperf_test *)iperf_test;

    while ( test->free_timers != NULL ) {
    	t = test->free_timers;
    	test->free_timers = t->next;
    	iperf_free( (void*) t );
    }
}


void
tmr_destroy( void* iperf_test )
{
    struct iperf_test *test = (struct iperf_test *)iperf_test;
    while ( test->timers != NULL )
    	tmr_cancel( test->timers, test );
    tmr_cleanup (test);
}


u32
timeval_diff(struct timeval * tv0, struct timeval * tv1)
{
    u32 time1, time2;
    
    time1 = tv0->tv_sec * 1000 + (tv0->tv_usec / 1000);
    time2 = tv1->tv_sec * 1000 + (tv1->tv_usec / 1000);

    if (time1 > time2)
        return (time1 - time2);
    else
        return (time2 - time1);
}
//#endif

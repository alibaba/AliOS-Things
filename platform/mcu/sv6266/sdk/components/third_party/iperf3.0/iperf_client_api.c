/*
 * iperf, Copyright (c) 2014, 2015, The Regents of the University of
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
//#include <errno.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/types.h>

//#include <os_wrapper.h>
//#include <rtos.h>
//#include <log.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_util.h"
#include "iperf_locale.h"
#include "net.h"
#include "timer.h"

//#if (IPERF3_ENABLE== 1)

int
iperf_create_streams(struct iperf_test *test)
{
    int i, s;
    struct iperf_stream *sp;

    int orig_bind_port = test->bind_port;
    for (i = 0; i < test->num_streams; ++i) {
        test->bind_port = orig_bind_port;
    	if (orig_bind_port)
    	    test->bind_port += i;

        if ((s = test->protocol->sock_connect(test)) < 0)
            return -1;

    	if (test->sender)
    	    FD_SET(s, &test->write_set);
    	else
    	    FD_SET(s, &test->read_set);
    	if (s > test->max_fd) test->max_fd = s;

        sp = iperf_new_stream(test, s);
        if (!sp)
            return -1;

        /* Perform the new stream callback */
        if (test->on_new_stream)
            test->on_new_stream(sp);
    }

    return 0;
}

static void
test_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    test->timer = NULL;
    test->done = 1;
}

static void
client_stats_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->stats_callback)
    	test->stats_callback(test);
}

static void
client_reporter_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->reporter_callback)
    	test->reporter_callback(test);
}

static int
create_client_timers(struct iperf_test * test)
{
    struct timeval now;
    TimerClientData cd;

    if (iperf_gettimeofday(&now, NULL) < 0) {
    	test->i_errno = IEINITTEST;
    	return -1;
    }
    cd.p = test;
    test->timer = test->stats_timer = test->reporter_timer = NULL;
    if (test->duration != 0) {
    	test->done = 0;
        test->timer = tmr_create(&now, test_timer_proc, cd, ( test->duration + test->omit ) * SEC_TO_MS, 0, test);
        if (test->timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
    	}
    } 
    if (test->stats_interval != 0) {
        test->stats_timer = tmr_create(&now, client_stats_timer_proc, cd, test->stats_interval * SEC_TO_MS, 1, test);
        if (test->stats_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
    	}
    }
    if (test->reporter_interval != 0) {
        test->reporter_timer = tmr_create(&now, client_reporter_timer_proc, cd, test->reporter_interval * SEC_TO_MS, 1, test);
        if (test->reporter_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
    	}
    }
    return 0;
}

static void
client_omit_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    test->omit_timer = NULL;
    test->omitting = 0;
    iperf_reset_stats(test);
    if (test->verbose && test->reporter_interval == 0)
        iperf_printf(test, "%s", report_omit_done);

    /* Reset the timers. */
    if (test->stats_timer != NULL)
        tmr_reset(nowP, test->stats_timer, test);
    if (test->reporter_timer != NULL)
        tmr_reset(nowP, test->reporter_timer, test);
}

static int
create_client_omit_timer(struct iperf_test * test)
{
    struct timeval now;
    TimerClientData cd;

    if (test->omit == 0) {
    	test->omit_timer = NULL;
        test->omitting = 0;
    } else {
        if (iperf_gettimeofday(&now, NULL) < 0) {
            test->i_errno = IEINITTEST;
            return -1;
        }
        test->omitting = 1;
        cd.p = test;
        test->omit_timer = tmr_create(&now, client_omit_timer_proc, cd, test->omit * SEC_TO_MS, 0, test);
        if (test->omit_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
        }
    }
    return 0;
}

int
iperf_handle_message_client(struct iperf_test *test)
{
    int rval;
    s32 err;
    u8 oldstate;

    /*!!! Why is this read() and not Nread()? */
    if ((rval = Nread(test->ctrl_sck, (char*) &test->state, sizeof(signed char), Ptcp)) <= 0) {
        if (rval == 0) {
            test->i_errno = IECTRLCLOSE;
            return -1;
        } else {
            test->i_errno = IERECVMESSAGE;
            return -1;
        }
    }

    if (test->debug)
        PRINTF("iperf_handle_message_client read state: %d\r\n", test->state);
    switch (test->state) {
        case PARAM_EXCHANGE:
            if (iperf_exchange_parameters(test) < 0)
                return -1;
            if (test->on_connect)
                test->on_connect(test);
            break;
        case CREATE_STREAMS:
            if (iperf_create_streams(test) < 0)
                return -1;
            break;
        case TEST_START:
            if (iperf_init_test(test) < 0)
                return -1;
            if (create_client_timers(test) < 0)
                return -1;
            if (create_client_omit_timer(test) < 0)
                return -1;
    	    if (!test->reverse)
        		if (iperf_create_send_timers(test) < 0)
        		    return -1;
            break;
        case TEST_RUNNING:
            break;
        case EXCHANGE_RESULTS:
            if (iperf_exchange_results(test) < 0)
                return -1;
            break;
        case DISPLAY_RESULTS:
            if (test->on_test_finish)
                test->on_test_finish(test);
            iperf_client_end(test);
            break;
        case IPERF_DONE:
            break;
        case SERVER_TERMINATE:
            test->i_errno = IESERVERTERM;

    	    /*
        	     * Temporarily be in DISPLAY_RESULTS phase so we can get
        	     * ending summary statistics.
        	     */
    	    oldstate = test->state;
            iperf_set_state(test, DISPLAY_RESULTS);
    	    test->reporter_callback(test);
            iperf_set_state(test, oldstate);
            return -1;
        case ACCESS_DENIED:
            test->i_errno = IEACCESSDENIED;
            return -1;
        case SERVER_ERROR:
            if (Nread(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                test->i_errno = IECTRLREAD;
                return -1;
            }
    	    test->i_errno = ntohl(err);
            if (Nread(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                test->i_errno = IECTRLREAD;
                return -1;
            }
            test->t_errno = ntohl(err);
            return -1;
        default:
            test->i_errno = IEMESSAGE;
            return -1;
    }

    if (test->debug)
        PRINTF("iperf_handle_message_client done \r\n");

    return 0;
}



/* iperf_connect -- client to server connection function */
int
iperf_connect(struct iperf_test *test)
{
    FD_ZERO(&test->read_set);
    FD_ZERO(&test->write_set);

    make_cookie(test->cookie);

    /* Create and connect the control channel */
    if (test->ctrl_sck < 0)
	// Create the control channel using an ephemeral port
    	test->ctrl_sck = netdial(test->settings->domain, Ptcp, test->bind_address, 0, test->server_hostname, test->server_port);
    if (test->ctrl_sck < 0) {
        test->i_errno = IECONNECT;
        return -1;
    }

    if (test->debug)
        PRINTF("iperf connect to server:%s, port:%u success \r\n", test->server_hostname, test->server_port);

    if (Nwrite(test->ctrl_sck, test->cookie, COOKIE_SIZE, Ptcp) < 0) {
        test->i_errno = IESENDCOOKIE;
        return -1;
    }

    FD_SET(test->ctrl_sck, &test->read_set);
    if (test->ctrl_sck > test->max_fd) test->max_fd = test->ctrl_sck;

    return 0;
}


int
iperf_client_end(struct iperf_test *test)
{
    struct iperf_stream *sp;

    /* Close all stream sockets */
    SLIST_FOREACH(sp, &test->streams, streams) {
        close(sp->socket);
    }

    /* show final summary */
    test->reporter_callback(test);

    if (iperf_set_send_state(test, IPERF_DONE) != 0)
        return -1;

    return 0;
}

extern bool iperf_get_runflag();

int
iperf_run_client(struct iperf_test * test)
{
    int startup;
    int result = 0;
    fd_set read_set, write_set;
    struct timeval now;
    struct timeval* timeout = NULL, sele_tmo;
    struct iperf_stream *sp;

    if (test->verbose) {
    	iperf_printf(test, "%s\r\n", iperf_version);
    }
    
    /* Start the client and connect to the server */
    if (iperf_connect(test) < 0)
        return -1;

    startup = 1;
    MEMSET(&read_set, 0, sizeof(read_set));
    MEMSET(&write_set, 0, sizeof(write_set));
    while (test->state != IPERF_DONE && iperf_get_runflag()) {
        //printf("test->state: %d \r\n", test->state);
    	MEMCPY(&read_set, &test->read_set, sizeof(fd_set));

        //test->write_set has been set before entering TEST_RUNNING
    	if (test->state == TEST_RUNNING){  
            MEMCPY(&write_set, &test->write_set, sizeof(fd_set));
        }
    	(void) iperf_gettimeofday(&now, NULL);
    	timeout = tmr_timeout(&now, test);
    	if (timeout == NULL || timeout->tv_sec >= 1 || timeout->tv_usec > 50000)
    	{
    	    timeout = &sele_tmo;
    	    sele_tmo.tv_sec = 0;
    	    sele_tmo.tv_usec = 50000;
    	}
    	result = select(test->max_fd + 1, &read_set, &write_set, NULL, timeout);
    	if (result < 0 && errnoicomm != EINTR) {
      	    test->i_errno = IESELECT;
    	    return -1;
    	}
    	if (result > 0) {
    	    if (FD_ISSET(test->ctrl_sck, &read_set)) {
     	        if (iperf_handle_message_client(test) < 0) {
        		    return -1;
        		}
        		FD_CLR(test->ctrl_sck, &read_set);
    	    }
    	}
    	if (test->state == TEST_RUNNING) {
    	    /* Is this our first time really running? */
    	    if (startup) {
    	        startup = 0;

        		// Set non-blocking for non-UDP tests
        		if (test->protocol->id != Pudp) {
        		    SLIST_FOREACH(sp, &test->streams, streams) {
            			/*debug only!!!*/
            			setnonblocking(sp->socket, 1);
        		    }
        		}
    	    }

    	    if (test->reverse) {
        		// Reverse mode. Client receives.
        		if (iperf_recv(test, &read_set) < 0)
        		    return -1;
    	    } else {
        		// Regular mode. Client sends.
        		if (iperf_send(test, &write_set) < 0)
        		    return -1;
    	    }

            /* Run the timers. */
            (void) iperf_gettimeofday(&now, NULL);
            tmr_run(&now, test);

    	    /* Is the test done yet? */
    	    if ((!test->omitting) &&
    	        ((test->duration != 0 && test->done) ||
    	         (test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes) ||
    	         (test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks))) {

            	// Unset non-blocking for non-UDP tests
            	if (test->protocol->id != Pudp) {
            	    SLIST_FOREACH(sp, &test->streams, streams) {
                		//setnonblocking(sp->socket, 0);
            	    }
        		}

        		/* Yes, done!  Send TEST_END. */
                test->done = 1;
                test->stats_callback(test);
                if (iperf_set_send_state(test, TEST_END) != 0)
        		    return -1;
    	    }
    	}

    	// If we're in reverse mode, continue draining the data
    	// connection(s) even if test is over.  This prevents a
    	// deadlock where the server side fills up its pipe(s)
    	// and gets blocked, so it can't receive state changes
    	// from the client side.
    	else if (test->reverse && test->state == TEST_END) {
    	    if (iperf_recv(test, &read_set) < 0)
        		return -1;
    	}
        else if (test->state == CREATE_STREAMS) 
        {
            /* avoid all the streams was connected but one of them is reset
             * At this stage, all the streams should connect to server,
             * don't need to check the number of connected streams
            */
            SLIST_FOREACH(sp, &test->streams, streams) 
            {
                if(iperf_init_stream(sp, test) < 0)
                {             
                    test->stats_callback(test);
                    test->state = DISPLAY_RESULTS; /* change local state only */
                    if (test->on_test_finish)
                        test->on_test_finish(test);
                    test->reporter_callback(test);

                    if (test->ctrl_sck >= 0) {
                        
                        test->state = CLIENT_TERMINATE;
                        (void) Nwrite(test->ctrl_sck, (char*) &test->state, sizeof(signed char), Ptcp);
                    }
                    test->i_errno = IECLIENTTERM;
                    return -1;
                }                
            }
                        
        }
    }

	iperf_printf(test, "\r\n");
	iperf_printf(test, "%s", iperf_report_done);

    return 0;
}
//#endif //#if(IPERF3_ENABLE==1)


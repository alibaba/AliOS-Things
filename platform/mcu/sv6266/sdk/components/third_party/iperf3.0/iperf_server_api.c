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
/* iperf_server_api.c: Functions to be used by an iperf server
*/


//#include <os_wrapper.h>
//#include <rtos.h>
//#include <log.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/tcp.h"

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_udp.h"
#include "iperf_tcp.h"
#include "iperf_util.h"
#include "timer.h"
#include "net.h"
#include "units.h"
#include "iperf_util.h"
#include "iperf_locale.h"

//#if (IPERF3_ENABLE == 1)

#define select_timeout_in_us 50000
#define timeval_sec select_timeout_in_us/1000000
#define timeval_usec select_timeout_in_us
#define create_strm_timeo_cnt 1000000/select_timeout_in_us

int
iperf_server_listen(struct iperf_test *test)
{
    retry:
    if((test->listener = netannounce(test->settings->domain, Ptcp, test->bind_address, test->server_port)) < 0) {
    	if (test->t_errno == EAFNOSUPPORT && test->settings->domain != AF_INET) {
    	    /* If we get "Address family not supported by protocol", that
            	        ** probably means we were compiled with IPv6 but the running
            	        ** kernel does not actually do IPv6.  This is not too unusual,
            	        ** v6 support is and perhaps always will be spotty.
            	        */
    	    warning("this system does not seem to support IPv6 - trying IPv4");
    	    test->settings->domain = AF_INET;
    	    goto retry;
    	} else {
    	    PRINTF("listener = netannounce return %d \r\n", test->listener);
    	    test->i_errno = IELISTEN;
    	    return -1;
    	}
    }

	iperf_printf(test, "-----------------------------------------------------------\r\n");
	iperf_printf(test, "Server listening on %d, socket:%d\r\n", test->server_port, test->listener);
	iperf_printf(test, "-----------------------------------------------------------\r\n");

    FD_ZERO(&test->read_set);
    FD_ZERO(&test->write_set);
    FD_SET(test->listener, &test->read_set);
    if (test->listener > test->max_fd) 
        test->max_fd = test->listener;

    return 0;
}

int
iperf_accept(struct iperf_test *test)
{
    int s;
    signed char rbuf = ACCESS_DENIED;
    socklen_t len;
    //struct sockaddr_storage addr;
    struct sockaddr_in addr;

    len = sizeof(addr);
    if ((s = accept(test->listener, (struct sockaddr *) &addr, &len)) < 0) {
        test->i_errno = IEACCEPT;
        return -1;
    }

    if (test->debug)
        PRINTF("iperf_accept new sock:%d, test->ctrl_sck:%d \r\n", s, test->ctrl_sck);
    
    if (test->ctrl_sck == -1) {
        /* Server free, accept new client */
        test->ctrl_sck = s;
        if (Nread(test->ctrl_sck, test->cookie, COOKIE_SIZE, Ptcp) < 0) {
            test->i_errno = IERECVCOOKIE;
            return -1;
        }
    	FD_SET(test->ctrl_sck, &test->read_set);
    	if (test->ctrl_sck > test->max_fd) test->max_fd = test->ctrl_sck;

    	if (iperf_set_send_state(test, PARAM_EXCHANGE) != 0)
            return -1;
        if (iperf_exchange_parameters(test) < 0)
            return -1;
        if (test->on_connect)
            test->on_connect(test);
    } else {
	/*
	 * Don't try to read from the socket.  It could block an ongoing test. 
	 * Just send ACCESS_DENIED.
	 */
        if (Nwrite(s, (char*) &rbuf, sizeof(rbuf), Ptcp) < 0) {
            test->i_errno = IESENDMESSAGE;
            return -1;
        }
        close(s);
    }

    return 0;
}


/**************************************************************************/
int
iperf_handle_message_server(struct iperf_test *test)
{
    int rval;
    struct iperf_stream *sp;
    u8 oldstate;

    // XXX: Need to rethink how this behaves to fit API
    if ((rval = Nread(test->ctrl_sck, (char*) &test->state, sizeof(signed char), Ptcp)) <= 0) {
        if (rval == 0) {
    	    PRINTF("the client has unexpectedly closed the connection \r\n");
            test->i_errno = IECTRLCLOSE;
            iperf_set_state(test, IPERF_DONE);
            return 0;
        } else {
            test->i_errno = IERECVMESSAGE;
            return -1;
        }
    }

    if (test->debug)
        PRINTF("iperf_handle_message_server read state: %d \r\n", test->state);
    
    switch(test->state) {
        case TEST_START:
            break;
        case TEST_END:
            // try to receive traffic on the way?
            if (!test->reverse)
                iperf_recv(test, &test->read_set);

	        test->done = 1;
            //test->stats_callback(test);
            
            SLIST_FOREACH(sp, &test->streams, streams) {
                FD_CLR(sp->socket, &test->read_set);
                FD_CLR(sp->socket, &test->write_set);
                close(sp->socket);
//                sp->socket = -1;
			     
			}
            sys_msleep(500);
	        if (iperf_set_send_state(test, EXCHANGE_RESULTS) != 0)
                return -1;
            if (iperf_exchange_results(test) < 0)
                return -1;
	        if (iperf_set_send_state(test, DISPLAY_RESULTS) != 0)
                return -1;
            if (test->on_test_finish)
                test->on_test_finish(test);
            test->reporter_callback(test);            
            break;
        case IPERF_DONE:
            break;
        case CLIENT_TERMINATE:
            test->i_errno = IECLIENTTERM;

    	    // Temporarily be in DISPLAY_RESULTS phase so we can get
    	    // ending summary statistics.
    	    oldstate = test->state;
            iperf_set_state(test, DISPLAY_RESULTS);
    	    test->reporter_callback(test);
            iperf_set_state(test, oldstate);

            // XXX: Remove this line below!
    	    PRINTF("the client has terminated \r\n");
            SLIST_FOREACH(sp, &test->streams, streams) {
                FD_CLR(sp->socket, &test->read_set);
                FD_CLR(sp->socket, &test->write_set);
                close(sp->socket);
                sp->socket = -1;
            }
             iperf_set_state(test, IPERF_DONE);
            break;
        default:
            test->i_errno = IEMESSAGE;
            return -1;
    }

    return 0;
}

static void
server_stats_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->stats_callback)
	    test->stats_callback(test);
}

static void
server_reporter_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->reporter_callback)
	    test->reporter_callback(test);
}

static int
create_server_timers(struct iperf_test * test)
{
    struct timeval now;
    TimerClientData cd;

    if (iperf_gettimeofday(&now, NULL) < 0) {
    	test->i_errno = IEINITTEST;
    	return -1;
    }
    cd.p = test;
    test->stats_timer = test->reporter_timer = NULL;
    if (test->stats_interval != 0) {
        test->stats_timer = tmr_create(&now, server_stats_timer_proc, cd, test->stats_interval * SEC_TO_MS, 1, test);
        if (test->stats_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
    	}
    }
    if (test->reporter_interval != 0) {
        test->reporter_timer = tmr_create(&now, server_reporter_timer_proc, cd, test->reporter_interval * SEC_TO_MS, 1, test);
        if (test->reporter_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
    	}
    }
    return 0;
}

static void
server_omit_timer_proc(TimerClientData client_data, struct timeval *nowP)
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
create_server_omit_timer(struct iperf_test * test)
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
        test->omit_timer = tmr_create(&now, server_omit_timer_proc, cd, test->omit * SEC_TO_MS, 0, test); 
        if (test->omit_timer == NULL) {
            test->i_errno = IEINITTEST;
            return -1;
        }
    }

    return 0;
}

static void
cleanup_server(struct iperf_test *test)
{
    /* Close open test sockets */
    if (test->ctrl_sck >= 0)
    {
        close(test->ctrl_sck);
        test->ctrl_sck = -1;
    }
    if (test->listener >= 0)
    {
        close(test->listener);
        test->listener = -1;
    }

    if (test->prot_listener >= 0)
    {
        close(test->prot_listener);
        test->prot_listener = -1;
    }
    
    /* Cancel any remaining timers. */
    if (test->stats_timer != NULL) {
    	tmr_cancel(test->stats_timer, test);
    	test->stats_timer = NULL;
    }
    if (test->reporter_timer != NULL) {
    	tmr_cancel(test->reporter_timer, test);
    	test->reporter_timer = NULL;
    }
    if (test->omit_timer != NULL) {
    	tmr_cancel(test->omit_timer, test);
    	test->omit_timer = NULL;
    }
}

extern bool iperf_get_runflag();
int
iperf_run_server(struct iperf_test *test)
{
    int result, s, streams_accepted;
    fd_set read_set, write_set;
    struct iperf_stream *sp;
    struct timeval now;
    struct timeval* timeout = NULL, sele_tmo;
    int strm_timeout_cnt = 0;

     if (test->verbose) {
	    iperf_printf(test, "%s\r\n", iperf_version);
    }

    // Open socket and listen
    if (iperf_server_listen(test) < 0) {
        return -1;
    }

    iperf_set_state(test, IPERF_START);
    streams_accepted = 0;

    while (test->state != IPERF_DONE && iperf_get_runflag()) {
        MEMCPY(&read_set, &test->read_set, sizeof(fd_set));
        MEMCPY(&write_set, &test->write_set, sizeof(fd_set));

        (void) iperf_gettimeofday(&now, NULL);
        timeout = tmr_timeout(&now, test);

        //PRINTF("iperf_run_server: state:%d \r\n", (int)test->state);
        //if (timeout != NULL)
        //    PRINTF("iperf_run_server: select timeout: %ld,%ld\r\n", timeout->tv_sec, timeout->tv_usec);

    	if (timeout == NULL || timeout->tv_sec >= 1 || timeout->tv_usec > 50000)
    	{
    	    timeout = &sele_tmo;
    	    sele_tmo.tv_sec = timeval_sec;
    	    sele_tmo.tv_usec = timeval_usec;
    	}
        result = select(test->max_fd + 1, &read_set, &write_set, NULL, timeout);
        //PRINTF("select test->max_fd:%d, result: %d \r\n", test->max_fd, result);
        
        if (result < 0 && errnoicomm != EINTR) {
	    	cleanup_server(test);
            test->i_errno = IESELECT;
            return -1;
        }
		if (result > 0) {
            if (FD_ISSET(test->listener, &read_set)) {
                if (test->state != CREATE_STREAMS) {
                    if (iperf_accept(test) < 0) {
						cleanup_server(test);
                        return -1;
                    }
                    FD_CLR(test->listener, &read_set);
                }
            }
            if (FD_ISSET(test->ctrl_sck, &read_set)) {
                if (iperf_handle_message_server(test) < 0) {
		    		cleanup_server(test);
                    return -1;
    			}
                FD_CLR(test->ctrl_sck, &read_set);                
            }

            if (test->state == CREATE_STREAMS) {
                if (FD_ISSET(test->prot_listener, &read_set)) {

                    if ((s = test->protocol->sock_accept(test)) < 0) {
    					cleanup_server(test);
                        return -1;
    	    		}

                    if (!is_closed(s)) {
                        sp = iperf_new_stream(test, s);
                        if (!sp) {
    			    		cleanup_server(test);
                            return -1;
    					}

    					if (test->sender)
    					    FD_SET(s, &test->write_set);
    					else
    					    FD_SET(s, &test->read_set);
    					if (s > test->max_fd) test->max_fd = s;

    					/* 
        					 * If the protocol isn't UDP, or even if it is but
        					 * we're the receiver, set nonblocking sockets.
        					 * We need this to allow a server receiver to
        					 * maintain interactivity with the control channel.
        					 */
    					if (test->protocol->id != Pudp
    					    || !test->sender) {
    					    setnonblocking(s, 1);
    					}

                        streams_accepted++;
                        //reset stream create timeout counter 
                        strm_timeout_cnt = 0;
                        if (test->on_new_stream)
                            test->on_new_stream(sp);
                    }
                    FD_CLR(test->prot_listener, &read_set);
                }
                               
                if (streams_accepted == test->num_streams) {
                    if (test->protocol->id != Ptcp) {
                        FD_CLR(test->prot_listener, &test->read_set);
                        close(test->prot_listener);
                        test->prot_listener = -1;
                    } else { 
                        if (test->no_delay || test->settings->mss || test->settings->socket_bufsize) { /*why */
                            FD_CLR(test->listener, &test->read_set);
                            close(test->listener);
                            test->listener = -1;
                            if ((s = netannounce(test->settings->domain, Ptcp, test->bind_address, test->server_port)) < 0) {
    							cleanup_server(test);
                                test->i_errno = IELISTEN;
                                return -1;
                            }
                            test->listener = s;
                            FD_SET(test->listener, &test->read_set);
    					    if (test->listener > test->max_fd) test->max_fd = test->listener;
                        }
                    }
                    test->prot_listener = -1;
    		    	if (iperf_set_send_state(test, TEST_START) != 0) {
        			    cleanup_server(test);
                        return -1;
    		        }
                    if (iperf_init_test(test) < 0) {
    			        cleanup_server(test);
                        return -1;
    		        }
    		        if (create_server_timers(test) < 0) {
    			        cleanup_server(test);
                        return -1;
    		        }
                    if (create_server_omit_timer(test) < 0) {
                        cleanup_server(test);
                        return -1;
                    }
    		        if (test->reverse)
    			        if (iperf_create_send_timers(test) < 0) {
    			            cleanup_server(test);
    			            return -1;
    			        }
    		        if (iperf_set_send_state(test, TEST_RUNNING) != 0) {
    			        cleanup_server(test);
                        return -1;
    		        }
                }
            }

            if (test->state == TEST_RUNNING) {
                if (test->reverse) {
                    // Reverse mode. Server sends.
                    if (iperf_send(test, &write_set) < 0) {
    		        	cleanup_server(test);
                        return -1;
    		        }
                } else {
                    // Regular mode. Server receives.
                    if (iperf_recv(test, &read_set) < 0) {
    			        cleanup_server(test);
                        return -1;
    		        }
                }
            }
        }

    	if (result == 0 || (timeout != NULL && timeout->tv_sec == 0 && timeout->tv_usec == 0)) {
    	    /* Run the timers. */
    	    (void) iperf_gettimeofday(&now, NULL);
    	    tmr_run(&now, test);
            if (test->state == CREATE_STREAMS)
            {
                //accumulate stream create timeout counter 
                strm_timeout_cnt++;
                if(test->debug)
                    PRINTF("iperf_stream create timeout counter ticks = %d \r\n", strm_timeout_cnt);
                if(strm_timeout_cnt > create_strm_timeo_cnt) //timeout of select - 50ms, counter 20 times for 1 seconds
                {
                    if (iperf_set_send_state(test, SERVER_TERMINATE) != 0) 
                    {
                        test->i_errno = IECLIENTTERM;

                        iperf_set_state(test, DISPLAY_RESULTS);
                        test->reporter_callback(test);

                        PRINTF("the server has terminated \r\n");
                        SLIST_FOREACH(sp, &test->streams, streams) 
                        {
                            FD_CLR(sp->socket, &test->read_set);
                            FD_CLR(sp->socket, &test->write_set);
                            close(sp->socket);
                            sp->socket = -1;
                        }
                        iperf_set_state(test, IPERF_DONE);
                    }
                }
            }
    	}
    }

    cleanup_server(test);
    return 0;
}
//#endif

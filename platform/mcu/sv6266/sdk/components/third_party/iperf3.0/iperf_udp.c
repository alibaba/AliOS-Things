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
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
//#include <unistd.h>
//#include <assert.h>
//#include <sys/types.h>
//#ifdef HAVE_STDINT_H
//#include <stdint.h>
//#endif
//#include <sys/time.h>

//#include <os_wrapper.h>
//#include <rtos.h>
//#include <log.h>

#include <lwip/sockets.h>

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_util.h"
#include "iperf_udp.h"
#include "timer.h"
#include "net.h"

//#if (IPERF3_ENABLE == 1)

/* iperf_udp_recv
 *
 * receives the data for UDP
 */
extern const unsigned long ticks_1sec;
int
iperf_udp_recv(struct iperf_stream *sp)
{
    long   sec, usec;
    u32    pcount;
    int    r;
    int    size = sp->settings->blksize;
    iperf_jit_t    J = 0;
    struct timeval sent_time, arrival_time;

    r = Nread(sp->socket, sp->buffer, size, Pudp);

    /*
     * If we got an error in the read, or if we didn't read anything
     * because the underlying read(2) got a EAGAIN, then skip packet
     * processing.
     */
    //PRINTF("iperf_udp_recv r:%d\r\n", r);
    if (r <= 0)
    {
        return r;
    }
    
    sp->result->bytes_received += r;
    sp->result->bytes_received_this_interval += r;

    {
    	long pc;
    	MEMCPY(&sec, sp->buffer, sizeof(sec));
    	MEMCPY(&usec, sp->buffer+4, sizeof(usec));
    	MEMCPY(&pc, sp->buffer+8, sizeof(pc));
    	sec = ntohl(sec);
    	usec = ntohl(usec);
    	pcount = ntohl(pc);
    	sent_time.tv_sec = sec;
    	sent_time.tv_usec = usec;
    }

    /* Out of order packets */
    if (pcount >= sp->packet_count + 1) {
        if (pcount > sp->packet_count + 1) {
            sp->cnt_error += (pcount - 1) - sp->packet_count;
        }
        sp->packet_count = pcount;
    } else {
        sp->outoforder_packets++;
        if (sp->test->debug)
        {
            if (sp->packet_count == pcount)
                PRINTF("DUPLICATES - received packet = %d \r\n", (int)sp->packet_count);
            else
        	    PRINTF("OUT OF ORDER - received packet = %d AND SP = %d, incoming packet = %u \r\n", (int)sp->packet_count, sp->socket, (int)pcount);
    	}
    }

    arrival_time.tv_sec = OS_GetSysTick()/ticks_1sec;
    arrival_time.tv_usec = (OS_GetSysTick()%ticks_1sec)*os_tick2ms(1)*1000;
    if (sp->prev_transit.tv_sec != 0 || sp->prev_transit.tv_usec != 0)
    {
        // XXX: This is NOT the way to calculate jitter
        //      J = |(R1 - S1) - (R0 - S0)| [/ number of packets, for average]            
        J = (iperf_jit_t)msecond_delta(&arrival_time, &sp->prev_receive);
        J += (iperf_jit_t)msecond_delta(&sp->prev_transit, &sent_time);
        if (J < 0)
            J = -J;
        sp->jitter += J;
        if (sp->test->debug)
        {
            //PRINTF("arr_sec:%ld, arr_usec:%ld, pre_sec:%ld, pre_usec:%ld \r\n", arrival_time.tv_sec, arrival_time.tv_usec, sp->prev_receive.tv_sec, sp->prev_receive.tv_usec);
            //PRINTF("snt_sec:%ld, snt_usec:%ld, pre_sec:%ld, pre_usec:%ld \r\n", sent_time.tv_sec, sent_time.tv_usec, sp->prev_transit.tv_sec, sp->prev_transit.tv_usec);
            //PRINTF("J=%ld, jitter:%ld \r\n", J, sp->jitter);
        }
    }
    sp->prev_receive = arrival_time;
    sp->prev_transit = sent_time;
    
    if (sp->test->debug) {
    	//PRINTF("packet_count %d\r\n", sp->packet_count);
    }

    return r;
}


/* iperf_udp_send
 *
 * sends the data for UDP
 */
int
iperf_udp_send(struct iperf_stream *sp)
{
    int r;
    int       size = sp->settings->blksize;
    struct timeval before;
    
    iperf_gettimeofday(&before, 0);

    ++sp->packet_count;

    {
    	u32  sec, usec, pcount;

    	sec = htonl(before.tv_sec);
    	usec = htonl(before.tv_usec);
    	pcount = htonl(sp->packet_count);
    	
    	MEMCPY(sp->buffer, &sec, sizeof(sec));
    	MEMCPY(sp->buffer+4, &usec, sizeof(usec));
    	MEMCPY(sp->buffer+8, &pcount, sizeof(pcount));
	
    }

    r = Nwrite(sp->socket, sp->buffer, size, Pudp);

    if (r < 0)
    	return r;

    sp->result->bytes_sent += r;
    sp->result->bytes_sent_this_interval += r;

    if (sp->test->debug){
        //PRINTF("send_id:%ld udp_send:%d, bytes_sent:%u, sec:%ld, usec:%ld \r\n", send_id++, r, sp->result->bytes_sent_this_interval, before.tv_sec, before.tv_usec );
    }
    return r;
}


/**************************************************************************/

/*
 * The following functions all have to do with managing UDP data sockets.
 * UDP of course is connectionless, so there isn't really a concept of
 * setting up a connection, although connect(2) can (and is) used to
 * bind the remote end of sockets.  We need to simulate some of the
 * connection management that is built-in to TCP so that each side of the
 * connection knows about each other before the real data transfers begin.
 */

/*
 * iperf_udp_accept
 *
 * Accepts a new UDP "connection"
 */
int
iperf_udp_accept(struct iperf_test *test)
{
    struct sockaddr_in sa_peer;
    //struct sockaddr_storage sa_peer;
    int       buf;
    socklen_t len;
    int       sz, s;
    int opt;

    /*
     * Get the current outstanding socket.  This socket will be used to handle
     * data transfers and a new "listening" socket will be created.
     */
    s = test->prot_listener;

    /*
     * Grab the UDP packet sent by the client.  From that we can extract the
     * client's address, and then use that information to bind the remote side
     * of the socket to the client.
     */
    len = sizeof(sa_peer);
    if ((sz = recvfrom(test->prot_listener, &buf, sizeof(buf), 0, (struct sockaddr *) &sa_peer, &len)) < 0) {
        test->i_errno = IESTREAMACCEPT;
        return -1;
    }

    if (connect(s, (struct sockaddr *) &sa_peer, len) < 0) {
        test->i_errno = IESTREAMACCEPT;
        return -1;
    }

    /*
     * Set socket buffer size if requested.  Do this for both sending and
     * receiving so that we can cover both normal and --reverse operation.
     */
    if ((opt = test->settings->socket_bufsize)) {
        if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
            test->i_errno = IESETBUF;
            return -1;
        }
        /* unimplemented yet!
        if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) < 0) {
            test->i_errno = IESETBUF;
            return -1;
        }*/
    }

    /*
     * Create a new "listening" socket to replace the one we were using before.
     */
    test->prot_listener = netannounce(test->settings->domain, Pudp, test->bind_address, test->server_port);
    if (test->prot_listener < 0) {
        test->i_errno = IESTREAMLISTEN;
        close(s);
        return -1;
    }

    FD_SET(test->prot_listener, &test->read_set);
    test->max_fd = (test->max_fd < test->prot_listener) ? test->prot_listener : test->max_fd;

    /* Let the client know we're ready "accept" another UDP "stream" */
    buf = 987654321;		/* any content will work here */
    if (write(s, &buf, sizeof(buf)) < 0) {
        test->i_errno = IESTREAMWRITE;
        close(s);
        return -1;
    }

    return s;
}


/*
 * iperf_udp_listen
 *
 * Start up a listener for UDP stream connections.  Unlike for TCP,
 * there is no listen(2) for UDP.  This socket will however accept
 * a UDP datagram from a client (indicating the client's presence).
 */
int
iperf_udp_listen(struct iperf_test *test)
{
    int s;

    if ((s = netannounce(test->settings->domain, Pudp, test->bind_address, test->server_port)) < 0) {
        test->i_errno = IESTREAMLISTEN;
        return -1;
    }

    /*
     * The caller will put this value into test->prot_listener.
     */
    return s;
}


/*
 * iperf_udp_connect
 *
 * "Connect" to a UDP stream listener.
 */
int
iperf_udp_connect(struct iperf_test *test)
{
    int s, buf, sz;
    int opt;
    int tv_sec = 30000;

    /* Create and bind our local socket. */
    if ((s = netdial(test->settings->domain, Pudp, test->bind_address, test->bind_port, test->server_hostname, test->server_port)) < 0) {
        test->i_errno = IESTREAMCONNECT;
        return -1;
    }

    /*
     * Set socket buffer size if requested.  Do this for both sending and
     * receiving so that we can cover both normal and --reverse operation.
     */
    if ((opt = test->settings->socket_bufsize)) {
        if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
            close(s);
            test->i_errno = IESETBUF;
            return -1;
        }
        /* umimplemented yet!
        if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) < 0) {
            test->i_errno = IESETBUF;
            return -1;
        }*/        
    }

#ifdef SO_RCVTIMEO
    #if 0
    /* 30 sec timeout for a case when there is a network problem. */
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    #endif
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv_sec, sizeof(tv_sec));
#endif

    /*
     * Write a datagram to the UDP stream to let the server know we're here.
     * The server learns our address by obtaining its peer's address.
     */
    buf = 123456789;		/* this can be pretty much anything */
    if (write(s, &buf, sizeof(buf)) < 0) {
        // XXX: Should this be changed to IESTREAMCONNECT? 
        close(s);
        test->i_errno = IESTREAMWRITE;
        return -1;
    }

    /*
     * Wait until the server replies back to us.
     */
    if ((sz = recv(s, &buf, sizeof(buf), 0)) < 0) {
        close(s);
        test->i_errno = IESTREAMREAD;
        return -1;
    }

    return s;
}


/* iperf_udp_init
 *
 * initializer for UDP streams in TEST_START
 */
int
iperf_udp_init(struct iperf_test *test)
{
    return 0;
}
//#endif

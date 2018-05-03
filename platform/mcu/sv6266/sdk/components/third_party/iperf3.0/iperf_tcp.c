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
 
//#include <os_wrapper.h>
#include "iperf_config.h"

//#include <rtos.h>
//#include <log.h>

#include "lwip/netdb.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_tcp.h"
#include "net.h"

//#if (IPERF3_ENABLE == 1)

/* iperf_tcp_recv
 *
 * receives the data for TCP
 */
int
iperf_tcp_recv(struct iperf_stream *sp)
{
    int r;
    size_t read;
    
    r = Nread_partly(sp->socket, sp->buffer, sp->settings->blksize, &read, Ptcp);
    //if (r < 0)
    //    return r;

    if (read > 0){
        sp->result->bytes_received += read;
        sp->result->bytes_received_this_interval += read;
    }    
    return r;
}


/* iperf_tcp_send 
 *
 * sends the data for TCP
 */
int
iperf_tcp_send(struct iperf_stream *sp)
{
    int r;
    size_t written;
    
	r = Nwrite_partly(sp->socket, sp->buffer, sp->settings->blksize, &written, Ptcp);
    //if (r < 0)
    //    return r;

    if (written > 0){
        sp->result->bytes_sent += written;
        sp->result->bytes_sent_this_interval += written;
    }
    
    return r;
}


/* iperf_tcp_accept
 *
 * accept a new TCP stream connection
 */
int
iperf_tcp_accept(struct iperf_test * test)
{
    int     s;
    signed char rbuf = ACCESS_DENIED;
    char    cookie[COOKIE_SIZE];
    socklen_t len;
    //struct sockaddr_storage addr;
    struct sockaddr_in addr;

    len = sizeof(addr);
    if ((s = accept(test->listener, (struct sockaddr *) &addr, &len)) < 0) {
        test->i_errno = IESTREAMCONNECT;
        return -1;
    }

    if (Nread(s, cookie, COOKIE_SIZE, Ptcp) < 0) {
        test->i_errno = IERECVCOOKIE;
        return -1;
    }

    if (STRCMP(test->cookie, cookie) != 0) {
        if (Nwrite(s, (char*) &rbuf, sizeof(rbuf), Ptcp) < 0) {
            test->i_errno = IESENDMESSAGE;
            return -1;
        }
        close(s);
        s = -1;
    }

    return s;
}


/* iperf_tcp_listen
 *
 * start up a listener for TCP stream connections
 */
int
iperf_tcp_listen(struct iperf_test *test)
{
    struct sockaddr_in sock_addr;
    int s, opt;
    u32_t optlen = sizeof(opt);

    s = test->listener;

    /*
     * If certain parameters are specified (such as socket buffer
     * size), then throw away the listening socket (the one for which
     * we just accepted the control connection) and recreate it with
     * those parameters.  That way, when new data connections are
     * set, they'll have all the correct parameters in place.
     *
     * It's not clear whether this is a requirement or a convenience.
     */
    if (test->no_delay || test->settings->mss || test->settings->socket_bufsize) {
        FD_CLR(s, &test->read_set);
        close(s);

        /*
        	 * If binding to the wildcard address with no explicit address
        	 * family specified, then force us to get an AF_INET6 socket.
        	 * More details in the comments in netanounce().
        	 */
    	if (test->settings->domain == AF_UNSPEC && !test->bind_address) {
            test->i_errno = IEV4ONLY;
            return -1;
    	}

        if ((s = socket(test->settings->domain, SOCK_STREAM, 0)) < 0) {
            test->i_errno = IESTREAMLISTEN;
            return -1;
        }

        if (test->no_delay) {
            opt = 1;
            if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
        		test->t_errno = getsockerr(s);
        		close(s);
                test->i_errno = IESETNODELAY;
                return -1;
            }
            if( getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, &optlen) >= 0)
            {
                if (test->debug){
                    if( opt )
                        PRINTF( " sock(%d) TCP_NODELAY : ENABLED \r\n", s);
                    else
                        PRINTF( " sock(%d) TCP_NODELAY : DISABLED \r\n", s);
                }
            }
            else if (test->debug)
                    PRINTF( " sock(%d) getsockopt failed \r\n", s);
        }
        
        if ((opt = test->settings->socket_bufsize)) {
            if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
        		test->t_errno = getsockerr(s);
        		close(s);
                test->i_errno = IESETBUF;
                return -1;
            }
            /* unimplemented yet!
            if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) < 0) {
        		test->t_errno = getsockerr(s);
        		close(s);
                test->i_errno = IESETBUF;
                return -1;
            }*/
        }
    	if (test->debug) {
            /* unimplemented yet!
    	    socklen_t optlen = sizeof(opt);
    	    if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, &optlen) < 0) {
        		test->t_errno = getsockerr(s);
        		close(s);
        		test->i_errno = IESETBUF;
        		return -1;
    	    }
    	    printf("SO_SNDBUF is %u\r\n", opt);
    	    */
    	}
        
        opt = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    	    test->t_errno = getsockerr(s);
            close(s);
            test->i_errno = IEREUSEADDR;
            return -1;
        }

    	/*
    	 * If we got an IPv6 socket, figure out if it shoudl accept IPv4
    	 * connections as well.  See documentation in netannounce() for
    	 * more details.
    	 */
        MEMSET(&sock_addr, 0, sizeof(sock_addr));
        sock_addr.sin_family = test->settings->domain;
        sock_addr.sin_port = htons(test->server_port);
        sock_addr.sin_len = sizeof(sock_addr);
		//if (test->bind_address)
		//	inet_aton(test->bind_address, &sock_addr.sin_addr);
	    if (bind(s, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
            test->t_errno = getsockerr(s);
            close(s);
            test->i_errno = IESTREAMLISTEN;
            return -1;
        }

        if (listen(s, 5) < 0) {
            test->i_errno = IESTREAMLISTEN;
            return -1;
        }

        test->listener = s;
    }
    
    return s;
}


/* iperf_tcp_connect
 *
 * connect to a TCP stream listener
 */
int
iperf_tcp_connect(struct iperf_test *test)
{
    struct sockaddr_in s_sockaddr;
    int s, opt;
    u32_t optlen = sizeof(opt);
    int sock_err;

    MEMSET(&s_sockaddr, 0, sizeof(s_sockaddr));
    s_sockaddr.sin_family = test->settings->domain;
    s_sockaddr.sin_port = htons(test->server_port);
    inet_aton(test->server_hostname, &s_sockaddr.sin_addr);
    s_sockaddr.sin_len = sizeof(s_sockaddr);
    
    if ((s = socket(test->settings->domain, SOCK_STREAM, 0)) < 0) {
        test->i_errno = IESTREAMCONNECT;
        return -1;
    }

    if (test->bind_address) {
        struct sockaddr_in l_sockaddr;
        MEMSET(&l_sockaddr, 0, sizeof(l_sockaddr));
        l_sockaddr.sin_family = test->settings->domain;
        l_sockaddr.sin_port = htons(test->bind_port);
        inet_aton(test->bind_address, &l_sockaddr.sin_addr);
        l_sockaddr.sin_len = sizeof(l_sockaddr);

        if (bind(s, (struct sockaddr *) &l_sockaddr, sizeof(l_sockaddr)) < 0) {
    	    test->t_errno = getsockerr(s);
    	    close(s);
            test->i_errno = IESTREAMCONNECT;
            return -1;
        }
    }

    /* Set socket options */
    if (test->no_delay) {
        opt = 1;
        if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
    	    test->t_errno = getsockerr(s);
    	    close(s);
            test->i_errno = IESETNODELAY;
            return -1;
        }
        
        if( getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, &optlen) >= 0){
            if (test->debug){
                if( opt )
                    PRINTF( " sock(%d) TCP_NODELAY : ENABLED \r\n", s);
                else
                     PRINTF( " sock(%d) TCP_NODELAY : DISABLED \r\n", s);
             }
        }  else if (test->debug)
               PRINTF( " sock(%d) getsockopt failed \r\n", s);
    }
    
    if ((opt = test->settings->socket_bufsize)) {
        if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
    	    test->t_errno = getsockerr(s);
    	    close(s);
            test->i_errno = IESETBUF;
            return -1;
        }
        /* unimplemented yet!
        if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) < 0) {
    	    test->t_errno = getsockerr(s);
    	    close(s);
            test->i_errno = IESETBUF;
            return -1;
        }*/
    }
    if (test->debug) {
        /* unimplemented yet!
    	socklen_t optlen = sizeof(opt);
    	if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, &optlen) < 0) {
    	    test->t_errno = getsockerr(s);
    	    close(s);
    	    test->i_errno = IESETBUF;
    	    return -1;
    	}
    	printf("SO_SNDBUF is %u\r\n", opt);
    	*/
    }
      
    if (connect(s, (struct sockaddr *) &s_sockaddr, sizeof(s_sockaddr)) < 0){
        sock_err = getsockerr(s);
        if (sock_err != EINPROGRESS) {
        	test->t_errno = sock_err;
        	close(s);
            test->i_errno = IESTREAMCONNECT;
            return -1;
        }
    }

    /* Send cookie for verification */
    if (Nwrite(s, test->cookie, COOKIE_SIZE, Ptcp) < 0) {
    	test->t_errno = getsockerr(s);
    	close(s);
        test->i_errno = IESENDCOOKIE;
        return -1;
    }

    return s;
}
//#endif

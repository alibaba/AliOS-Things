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

#include "lwip/sockets.h"
#include "lwip/tcp.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "iperf.h"
#include "iperf_util.h"
#include "net.h"
#include "timer.h"
#include "iperf_api.h"

//#if (IPERF3_ENABLE == 1)

/* netdial and netannouce code comes from libtask: http://swtch.com/libtask/
 * Copyright: http://swtch.com/libtask/COPYRIGHT
*/

/* make connection to server */
int
netdial(int domain, int proto, char *local, int local_port, char *server, int port)
{
    struct sockaddr_in s_sockaddr, l_sockaddr;
    int s;
    int sock_err;

    s = socket(domain, proto, 0);
    if (s < 0) 
        return -1;

    if (local) {
        MEMSET(&l_sockaddr, 0, sizeof(l_sockaddr));
        l_sockaddr.sin_len = sizeof(l_sockaddr);
        l_sockaddr.sin_family = domain;
        inet_aton(local, &l_sockaddr.sin_addr);
        l_sockaddr.sin_port = htons(local_port);

        if (bind(s, (struct sockaddr *) &l_sockaddr, sizeof(l_sockaddr)) < 0) {
    	    close(s);
            return -1;
    	}
    }

    MEMSET(&s_sockaddr, 0, sizeof(s_sockaddr));
    s_sockaddr.sin_len = sizeof(s_sockaddr);
    s_sockaddr.sin_family = domain;
    inet_aton(server, &s_sockaddr.sin_addr);
    s_sockaddr.sin_port = htons(port);
    if (connect(s, (struct sockaddr *) &s_sockaddr, sizeof(s_sockaddr)) < 0){
        sock_err = getsockerr(s);        
        if (sock_err != EINPROGRESS) {
        	close(s);
            return -1;
        }
    }

    return s;
}

/***************************************************************/

int
netannounce(int domain, int proto, char *local, int port)
{
    int s, opt;
    struct sockaddr_in sock_addr;
    
    /* 
     * If binding to the wildcard address with no explicit address
     * family specified, then force us to get an AF_INET6 socket.  On
     * CentOS 6 and MacOS, getaddrinfo(3) with AF_UNSPEC in ai_family,
     * and ai_flags containing AI_PASSIVE returns a result structure
     * with ai_family set to AF_INET, with the result that we create
     * and bind an IPv4 address wildcard address and by default, we
     * can't accept IPv6 connections.
     *
     * On FreeBSD, under the above circumstances, ai_family in the
     * result structure is set to AF_INET6.
     */
    if (domain == AF_UNSPEC && !local) {
    	return -1;
    }

    s = socket(domain, proto, 0);
    if (s < 0) {
        return -3;
    }

    opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, 
		   (char *) &opt, sizeof(opt)) < 0) {
    	close(s);
    	return -4;
    }

    MEMSET(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = domain;
    sock_addr.sin_port = htons(port);
    if (local)
        inet_aton(local, &sock_addr.sin_addr);
    sock_addr.sin_len = sizeof(sock_addr);
    if (bind(s, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        close(s);
        return -5;
    }

    if (proto == SOCK_STREAM) {
        if (listen(s, 5) < 0) {
    	    close(s);
            return -6;
        }
    }

    return s;
}


/*******************************************************************/
/* reads 'count' bytes from a socket  */
/********************************************************************/
int
Nread(int fd, char *buf, size_t count, int prot)
{
    register int r;
    register size_t nleft = count;
    int sock_err;
    
    while (nleft > 0) {
        r = read(fd, buf, nleft);
        if (r < 0) {
            sock_err = getsockerr(fd);
            if (sock_err == EINTR || sock_err == EAGAIN)
                break;
            else
            {
                PRINTF("Nread err = %d r = %d\r\n", sock_err, r);
        		return NET_HARDERROR;
        	}
        } else if (r == 0)
            break;

        nleft -= r;
        buf += r;
    }
    return count - nleft;
}


int
Nread_partly(int fd, char *buf, size_t count, size_t *read_len, int prot)
{
    register int r;
    register size_t nleft = count;
    int sock_err;
    
    *read_len = 0;
    while (nleft > 0) {
        r = read(fd, buf, nleft);
        if (r < 0) {
            sock_err = getsockerr(fd);
            if (sock_err == EINTR || sock_err == EAGAIN)
                break;
            else
            {
                PRINTF("Nread_partly err = %d r = %d\r\n", sock_err, r);
                return NET_HARDERROR;
            }
        } else if (r == 0)
            break;

        nleft -= r;
        buf += r;
        *read_len += r;
    }
    return count - nleft;
}


/*
 *                      N W R I T E
 */

int
Nwrite(int fd, const char *buf, size_t count, int prot)
{
    register int r;
    register size_t nleft = count;
    int sock_err;
    
    while (nleft > 0) {
    	r = write(fd, buf, nleft);
    	if (r < 0) {
    	    sock_err = getsockerr(fd);
    	    switch (sock_err) {
        		case EINTR:
        		case EAGAIN:
            		//return count - nleft;
                case ENOMEM:
        		case ENOBUFS:
            		return NET_SOFTERROR;

        		default:
                {
                    PRINTF("Nwrite err = %d r = %d\r\n", sock_err, r);
            		return NET_HARDERROR;
        		}
    	    }
    	} else if (r == 0)
    	    return NET_SOFTERROR;
    	nleft -= r;
    	buf += r;
    }
    return count;
}

/*
 *                      N W R I T E
 */

int
Nwrite_partly(int fd, const char *buf, size_t count, size_t *written, int prot)
{
    register int r;
    register size_t nleft = count;
    int sock_err;
    
    *written = 0;
    while (nleft > 0) {
    	r = write(fd, buf, nleft);
    	if (r < 0) {
    	    sock_err = getsockerr(fd);
    	    switch (sock_err) {
    	    case 0:   ///0 ever appears!!!
    		case EINTR:
    		case EAGAIN:
        		//return count - nleft;
        		{
                    if (*written > 0)
                        return *written;
                    else
                        return NET_SOFTERROR;
                }

            case ENOMEM:    
            case ENOBUFS:
                {                   
                    if (*written > 0)
                        return *written;
                    else
                        return NET_SOFTERROR;
                }

    		default:
                {
                    PRINTF("Nwrite_partly err = %d r = %d\r\n", sock_err, r);
                    return NET_HARDERROR;
                }
    	    }
    	} else if (r == 0)
    	    return NET_SOFTERROR;
    	nleft -= r;
    	buf += r;
    	*written += r;
    }
    return count;
}

int
setnonblocking(int fd, int nonblocking)
{
    int flags, newflags;

    //printf("setnonblocking: fd:%d, flag:%d \n", fd, nonblocking);
    flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        PRINTF("fcntl(F_GETFL)");
        return -1;
    }
    if (nonblocking)
    	newflags = flags | (int) O_NONBLOCK;
    else
    	newflags = flags & ~((int) O_NONBLOCK);
    if (newflags != flags)
    	if (lwip_fcntl(fd, F_SETFL, newflags) < 0) {
    	    PRINTF("lwip_fcntl(F_SETFL)");
    	    return -1;
    	}
    return 0;
}

/****************************************************************************/

int
getsockdomain(int sock)
{
    struct sockaddr sa;
    socklen_t len = sizeof(sa);

    if (getsockname(sock, &sa, &len) < 0)
    	return -1;
    
    return sa.sa_family;
}

int getsockerr (int fd)
{
    int opt;
    u32_t optlen = sizeof(opt);
    getsockopt(fd,SOL_SOCKET,SO_ERROR,&opt,&optlen);

    return opt;
}
//#endif

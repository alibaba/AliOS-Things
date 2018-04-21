#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "platform.h"

static int network_create_socket( pplatform_netaddr_t netaddr, int type, struct sockaddr_in *paddr, long *psock)
{
	struct hostent *hp;
	struct in_addr in;
	uint32_t ip;
	int opt_val = 1;

    assert(paddr && psock);

    if (NULL == netaddr->host)
    {
    	ip = htonl(INADDR_ANY);
    }
    else
    {
        /*
         * in some platform gethostbyname() will return bad result
         * if host is "255.255.255.255", so calling inet_aton first
         */
        if (inet_aton(netaddr->host, &in)) {
            ip = *(uint32_t *)&in;
        } else {
            hp = gethostbyname(netaddr->host);
            if (!hp) {
                printf("can't resolute the host address \n");
                return -1;
            }
            ip = *(uint32_t *)(hp->h_addr);
		}
    }

    *psock = socket(AF_INET, type, 0);
    if (*psock < 0)
    {
        perror("socket");
    	return -1;
    }

    memset(paddr, 0, sizeof(struct sockaddr_in));

    if (0 != setsockopt(*psock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)))
    {
        perror("setsockopt");
        close((int)*psock);
        return -1;
    }
    
    if (type == SOCK_DGRAM) {
        if (0 != setsockopt(*psock, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val)))
        {
            perror("setsockopt");
            close((int)*psock);
            return -1;
        }
    }

    paddr->sin_addr.s_addr = ip;
	paddr->sin_family = AF_INET;
	paddr->sin_port = htons( netaddr->port );

	return 0;
}


void *platform_udp_server_create(_IN_ uint16_t port)
{
	struct sockaddr_in addr;
	long server_socket;
	platform_netaddr_t netaddr = {NULL, port};

	if (0 != network_create_socket(&netaddr, SOCK_DGRAM, &addr, &server_socket))
	{
		return PLATFORM_INVALID_FD;
	}

    if (-1 == bind(server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        platform_udp_close((void *)server_socket);
    	return PLATFORM_INVALID_FD;
    }

    return (void *)server_socket;
}




void *platform_udp_client_create(void)
{
	struct sockaddr_in addr;
	long sock;
	platform_netaddr_t netaddr = {NULL, 0};

	if (0 != network_create_socket(&netaddr, SOCK_DGRAM, &addr, &sock))
	{
		return PLATFORM_INVALID_FD;
	}

	return (void *)sock;
}



void *platform_udp_multicast_server_create(pplatform_netaddr_t netaddr, const char *ifname)
{
	int option = 1;
	struct sockaddr_in addr;
    long sock = 0;
    struct ip_mreq mreq;
    /* Note: ignore host right now */
    platform_netaddr_t netaddr_client = {NULL, netaddr->port};

    memset(&addr, 0, sizeof(addr));
    memset(&mreq, 0, sizeof(mreq));

    if (0 != network_create_socket(&netaddr_client, SOCK_DGRAM, &addr, &sock)) {
        return PLATFORM_INVALID_FD;
    }

    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0) {
        perror("setsockopt");
        goto err;
    }

    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        perror("bind");
        goto err;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
    if(NULL == ifname || strlen(ifname) == 0){
        mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);      //defaut router if ip
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq)) < 0) {
            printf("setsockopt membership error, ip:default router, multicast addr:%s\n", netaddr->host);
            goto err;
        }
    }
    else{
        char ip_str[PLATFORM_IP_LEN] = {0};
        if (platform_wifi_get_ip(ip_str, ifname) < 0) {
            printf("get %s interface ip address fail\n", ifname);
            goto err;
        }
        mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
        mreq.imr_interface.s_addr = inet_addr(ip_str); //lan ip
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq)) < 0) {
            printf("setsockopt membership error , ip:%s,  multicast addr:%s\n", ip_str, netaddr->host);
            goto err;
        }
    }

    return (void *)sock;

err:
    if(sock > 0){
        platform_udp_close((void *)sock);
        return PLATFORM_INVALID_FD;
    }
}



void platform_udp_close(void *handle)
{
	close((long)handle);
}



int platform_udp_sendto(
		_IN_ void *handle,
		_IN_ const char *buffer,
		_IN_ uint32_t length,
		_IN_ pplatform_netaddr_t netaddr)
{
	int ret;
	uint32_t ip;
    struct in_addr in;
	struct hostent *hp;
	struct sockaddr_in addr;

    /*
     * in some platform gethostbyname() will return bad result
     * if host is "255.255.255.255", so calling inet_aton first
     */
    if (inet_aton(netaddr->host, &in)) {
        ip = *(uint32_t *)&in;
    } else {
        hp = gethostbyname(netaddr->host);
        if (!hp) {
            printf("can't resolute the host address \n");
            return -1;
        }
        ip = *(uint32_t *)(hp->h_addr);
    }

	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(netaddr->port);

	ret = sendto((long)handle,
					   buffer,
					   length,
					   0,
					   (struct sockaddr *)&addr,
					   sizeof(struct sockaddr_in));

    if (ret < 0)
        perror("sendto");

	return (ret) > 0 ? ret : -1;
}


int platform_udp_recvfrom(
		_IN_ void *handle,
		_OUT_ char *buffer,
		_IN_ uint32_t length,
		_OUT_OPT_ pplatform_netaddr_t netaddr)
{
	int ret;
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);

	ret = recvfrom((long)handle, buffer, length, 0, (struct sockaddr *)&addr, &addr_len);
	if (ret > 0)
	{
		if (NULL != netaddr)
		{
            netaddr->port = ntohs(addr.sin_port);
            
            if (NULL != netaddr->host)
            {
                strcpy(netaddr->host, inet_ntoa(addr.sin_addr));
            }
		}

		return ret;
	}

	return -1;
}



void *platform_tcp_server_create(_IN_ uint16_t port)
{
	struct sockaddr_in addr;
	long server_socket;
	platform_netaddr_t netaddr = {NULL, port};

	if (0 != network_create_socket(&netaddr, SOCK_STREAM, &addr, &server_socket))
	{
		return PLATFORM_INVALID_FD;
	}

    if (-1 == bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
    	platform_tcp_close((void *)server_socket);
    	return PLATFORM_INVALID_FD;
    }

    if (0 != listen(server_socket, SOMAXCONN))
	{
    	platform_tcp_close((void *)server_socket);
		return PLATFORM_INVALID_FD;
	}

    return (void *)server_socket;
}




void *platform_tcp_server_accept(_IN_ void *server)
{
	struct sockaddr_in addr;
	int addr_length = sizeof(addr);
	long new_client;

	if ((new_client = accept((long)server,(struct sockaddr*)&addr, &addr_length)) <= 0)
	{
		return PLATFORM_INVALID_FD;
	}

	return (void *)new_client;
}




void *platform_tcp_client_connect(_IN_ pplatform_netaddr_t netaddr)
{
	struct sockaddr_in addr;
	long sock;

	if (0 != network_create_socket(netaddr, SOCK_STREAM, &addr, &sock))
	{
		return PLATFORM_INVALID_FD;
	}

	if (-1 == connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
	{
		platform_tcp_close((void *)sock);
		return PLATFORM_INVALID_FD;
	}

	return (void *)sock;
}



int platform_tcp_send(_IN_ void *handle, _IN_ const char *buffer, _IN_ uint32_t length)
{
	int bytes_sent;

	bytes_sent = send((long)handle, buffer, length, 0);
	return bytes_sent > 0 ? bytes_sent : -1;
}



int platform_tcp_recv(_IN_ void *handle, _OUT_ char *buffer, _IN_ uint32_t length)
{
	int bytes_received;

	bytes_received = recv((long)handle, buffer, length, 0);

	return bytes_received > 0 ? bytes_received : -1;
}




void platform_tcp_close(_IN_ void *handle)
{
	close((long)handle);
}



int platform_select(void *read_fds[PLATFORM_SOCKET_MAXNUMS],
		void *write_fds[PLATFORM_SOCKET_MAXNUMS],
		int timeout_ms)
{
	int i, ret = -1;
	struct timeval timeout_value;
	struct timeval *ptimeval = &timeout_value;
	fd_set r_set, w_set;

	if (PLATFORM_WAIT_INFINITE == timeout_ms)
		ptimeval = NULL;
	else {
		ptimeval->tv_sec = timeout_ms / 1000;
		ptimeval->tv_usec = (timeout_ms % 1000) * 1000;
	}

    FD_ZERO(&r_set);
    FD_ZERO(&w_set);

    if (read_fds) {
        for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
            if (PLATFORM_INVALID_FD != read_fds[i])
                FD_SET((long)read_fds[i], &r_set);
        }
    }

    if (write_fds) {
        for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
            if ( PLATFORM_INVALID_FD != write_fds[i] )
                FD_SET((long)write_fds[i], &w_set);
        }
    }

	ret = select(FD_SETSIZE, &r_set, &w_set, NULL, ptimeval);

	if (ret > 0) {
		if (read_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
				if (PLATFORM_INVALID_FD != read_fds[i]
                        && !FD_ISSET((long)read_fds[i], &r_set))
					read_fds[i] = PLATFORM_INVALID_FD;
			}
		}

		if (write_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
				if (PLATFORM_INVALID_FD != write_fds[i]
                        && !FD_ISSET((long)write_fds[i], &w_set))
					write_fds[i] = PLATFORM_INVALID_FD;
			}
		}
	} else {/* clear all fd */
		if (read_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i)
					read_fds[i] = PLATFORM_INVALID_FD;
		}

		if (write_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i)
					write_fds[i] = PLATFORM_INVALID_FD;
		}
    }

	return (ret >= 0) ? ret : -1;
}

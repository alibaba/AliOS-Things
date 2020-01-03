/**
 * @file sockopt.c  Networking socket options
 *
 * Copyright (C) 2010 Creytiv.com
 */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_net.h>


#define DEBUG_MODULE "sockopt"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


/** Platform independent buffer type cast */
#ifdef WIN32
#define BUF_CAST (char *)
#else
#define BUF_CAST
#endif


/**
 * Set socket option blocking or non-blocking
 *
 * @param fd       Socket file descriptor
 * @param blocking true for blocking, false for non-blocking
 *
 * @return 0 if success, otherwise errorcode
 */
int net_sockopt_blocking_set(int fd, bool blocking)
{
#ifdef WIN32
	unsigned long noblock = !blocking;
	int err = 0;

	if (0 != ioctlsocket(fd, FIONBIO, &noblock)) {
		err = WSAGetLastError();
		DEBUG_WARNING("nonblock set: fd=%d err=%d (%m)\n",
			      fd, err, err);
	}
	return err;
#else
	int flags;
	int err = 0;

	flags = fcntl(fd, F_GETFL, 0);
	if (-1 == flags) {
		err = errno;
		DEBUG_WARNING("sockopt set: fnctl F_GETFL: (%m)\n", err);
		goto out;
	}

	if (blocking)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;

	if (-1 == fcntl(fd, F_SETFL, flags)) {
		err = errno;
		DEBUG_WARNING("sockopt set: fcntl F_SETFL non-block (%m)\n",
			      err);
	}

 out:
	return err;
#endif
}


/**
 * Set socket option to reuse address and port
 *
 * @param fd     Socket file descriptor
 * @param reuse  true for reuse, false for no reuse
 *
 * @return 0 if success, otherwise errorcode
 */
int net_sockopt_reuse_set(int fd, bool reuse)
{
	int r = reuse;

#ifdef SO_REUSEADDR
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			     BUF_CAST &r, sizeof(r))) {
		DEBUG_WARNING("SO_REUSEADDR: %m\n", errno);
		return errno;
	}
#endif

#ifdef SO_REUSEPORT
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
			     BUF_CAST &r, sizeof(r))) {
		DEBUG_INFO("SO_REUSEPORT: %m\n", errno);
		return errno;
	}
#endif

#if !defined(SO_REUSEADDR) && !defined(SO_REUSEPORT)
	(void)r;
	(void)fd;
	(void)reuse;
	return ENOSYS;
#else
	return 0;
#endif
}

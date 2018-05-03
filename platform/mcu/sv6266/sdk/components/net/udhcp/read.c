/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#include "udhcp_common.h"
#if DHCPD_SUPPORT

extern struct sockaddr * dest_sin;
extern socklen_t dest_length;
extern int errno;

ssize_t FAST_FUNC safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;
#if 0
	do {
//		n = read(fd, buf, count);
		n = recvfrom(fd, buf, count, 0, dest_sin, &dest_length);
	} while (n < 0 && errno == EINTR);
#else
    fd_set r_set;
    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
    int sel;
    
    FD_ZERO(&r_set);
    FD_SET(fd, &r_set);
    do {
        if((sel = select((fd+1), &r_set, NULL, NULL, &timeout)) != 0)
            n = recvfrom(fd, buf, count, 0, dest_sin, &dest_length);
        else
            return -1;
	} while (n < 0 && errno == EINTR);
    
    
    
#endif

	return n;
}

/*
 * Read all of the supplied buffer from a file.
 * This does multiple reads as necessary.
 * Returns the amount read, or -1 on an error.
 * A short read is returned on an end of file.
 */
ssize_t FAST_FUNC full_read(int fd, void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_read(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already have some! */
				/* user can do another read to know the error code */
				return total;
			}
			return cc; /* read() returns -1 on failure. */
		}
		if (cc == 0)
			break;
		buf = ((char *)buf) + cc;
		total += cc;
		len -= cc;
	}

	return total;
}

ssize_t FAST_FUNC read_close(int fd, void *buf, size_t size)
{
	/*int e;*/
	size = full_read(fd, buf, size);
	/*e = errno;*/
	close(fd);
	/*errno = e;*/
	return size;
}

/*
ssize_t FAST_FUNC open_read_close(const char *filename, void *buf, size_t size)
{
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		return fd;
	return read_close(fd, buf, size);
}
*/

ssize_t FAST_FUNC safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
//		n = write(fd, buf, count);
		n = sendto(fd, buf, count, 0, dest_sin, dest_length);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t FAST_FUNC full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;  /* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

#endif //#if DHCPD_SUPPORT
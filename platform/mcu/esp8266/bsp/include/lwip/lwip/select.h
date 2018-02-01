/* select.h
   Copyright 1998, 1999, 2000, 2001, 2005, 2009 Red Hat, Inc.

   Written by Geoffrey Noer <noer@cygnus.com>

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H


#  define _SYS_TYPES_FD_SET
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#  ifndef	FD_SETSIZE
#	define	FD_SETSIZE	64
#  endif

typedef	unsigned long	fd_mask;
#  define	NFDBITS	(sizeof (fd_mask) * 8)	/* bits per mask */
#  ifndef	_howmany
#	define	_howmany(x,y)	(((x)+((y)-1))/(y))
#  endif

/* We use a macro for fd_set so that including Sockets.h afterwards
   can work.  */
typedef	struct _types_fd_set {
	fd_mask	fds_bits[_howmany(FD_SETSIZE, NFDBITS)];
} _types_fd_set;

#define fd_set _types_fd_set

#  define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#  define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#  define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#  define	FD_ZERO(p)	(__extension__ (void)({ \
     size_t __i; \
     char *__tmp = (char *)p; \
     for (__i = 0; __i < sizeof (*(p)); ++__i) \
       *__tmp++ = 0; \
}))


#endif /* sys/select.h */

/**
 * @file str_error.c System error messages
 *
 * Copyright (C) 2010 Creytiv.com
 */
#define _GNU_SOURCE 1
#define __EXTENSIONS__ 1
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>


/**
 * Look up an error message string corresponding to an error number.
 *
 * @param errnum Error Code
 * @param buf    Buffer for storing error message
 * @param sz     Buffer size
 *
 * @return Error message string
 */
const char *str_error(int errnum, char *buf, size_t sz)
{
	const char *s;

	if (!buf || !sz)
		return NULL;

	buf[0] = '\0';
#ifdef HAVE_STRERROR_R

#ifdef __GLIBC__
	s = strerror_r(errnum, buf, sz);
#else
	(void)strerror_r(errnum, buf, sz);
	s = buf;
#endif

#elif defined (WIN32) & !defined (__MINGW32__)
	(void)strerror_s(buf, sz, errnum);
	s = buf;
#else
	/* fallback */
	(void)errnum;
	s = "unknown error";
#endif

	buf[sz - 1] = '\0';

	return s;
}

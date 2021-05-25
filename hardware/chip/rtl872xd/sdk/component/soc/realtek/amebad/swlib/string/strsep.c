/* BSD strsep function */

/* Copyright 2002, Red Hat Inc. */

/* undef STRICT_ANSI so that strsep prototype will be defined */
#undef  __STRICT_ANSI__

#include <section_config.h>
#include <basic_types.h>

#include <string.h>
#include <_ansi.h>
#include <reent.h>

extern char *__strtok_r(char *, const char *, char **, int);

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
char * _strsep(register char **source_ptr , register const char *delim)
{
	return __strtok_r(*source_ptr, delim, source_ptr, 0);
}

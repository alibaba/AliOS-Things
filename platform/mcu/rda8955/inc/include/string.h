/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef STRING_H
#define STRING_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stddef.h"

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
extern char * strsep(char **stringp, const char *delim);

/*
 * Awkward, bug-prone, non-reentrant, Bad Thing altogether
 * Use strsep instead if possible
 */
extern char * strtok(char *,const char *);
/*
 * Same as strtok, but reentrant, and needs its own buffer
 * Still awkward and bug-prone...
 */
extern char * strtok_r(char *,const char *, char **);

extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *, size_t);

extern char * strcat(char *, const char *);
extern char * strncat(char *, const char *,size_t);

extern int strcmp(const char *,const char *);
extern int strncmp(const char *,const char *,size_t);
extern int strnicmp(const char *, const char *, size_t);

extern char * strchr(const char *,int);
extern char * strrchr(const char *,int);
extern char * strstr(const char *,const char *);

extern size_t strlen(const char *);
extern size_t strnlen(const char *,size_t);

extern void * memset(void *,int,size_t);
extern void * memcpy(void *,const void *,size_t);
extern void * memmove(void *,const void *,size_t);

extern void * memscan(void *,int,size_t);
extern int memcmp(const void *,const void *,size_t);
extern void * memchr(const void *,int,size_t);

size_t  strcspn (const char *s, const char *reject);
size_t  strspn(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif


#endif /* STRING_H */


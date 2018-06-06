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

#ifndef STDLIB_H
#define    STDLIB_H
#ifdef __cplusplus
extern "C" {
#endif

#include "cs_types.h"
#include "stddef.h"

/* Standard atoi() function. Work as the libc one. */
int atoi(const char *s);
#define atol(str) ((long) atoi((str)))
int atox(const char *s, int len);

long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);

/* Standard random functions, work as the libc ones. */
#define RAND_MAX        32767

int rand (void);
void srand(unsigned int seed);


typedef INT STD_COMPAR_FUNC_T (const VOID*, const VOID*);

PUBLIC VOID  qsort (VOID *baseP, INT32 nElem, INT32 width, STD_COMPAR_FUNC_T *compar);


void *
bsearch (const void *key, const void *base, size_t nmemb, size_t size,
         int (*compar) __P ((const void *, const void *)));
#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */

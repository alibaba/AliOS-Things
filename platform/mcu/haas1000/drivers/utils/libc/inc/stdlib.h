#ifndef STDLIB_H
#define STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"

/* Standard atoi() function. Work as the libc one. */
int atoi(const char *s);
#define atol(str) ((long) atoi((str)))

long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);

/* Standard random functions, work as the libc ones. */
#define RAND_MAX        32767

int rand(void);
void srand(unsigned int seed);

void qsort(void *baseP, size_t nElem, size_t width, int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
     int (*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif

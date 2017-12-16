/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 *
 */

#include "test_bed.h"

int8_t *strcpy(int8_t *dest, const int8_t *src)
{
    register int8_t *ret = dest;

    if (NULL == dest || NULL == src) {
        return NULL;
    }

    while((*dest++ = *src++));
    return ret;
}

int8_t *strncpy(int8_t *dest, const int8_t *src, size_t n)
{
    register int8_t *ret = dest;

    if (NULL == dest || NULL == src) {
        return NULL;
    }

    while(n && (*dest++ = *src++))
        n--;
    while(n--) {
        *dest++ = '\0';
    }
    return ret;
}

int32_t strcmp(const int8_t *s1, const int8_t *s2)
{
    register int32_t ret = 0;

    if (NULL == s1 || NULL == s2) {
        while(1); /* FIXME(junlin) should be panic? */
    }
    while(!(ret = (*s1 - *s2++)) && *s1++);
    return ret;
}

int32_t strncmp(const int8_t *s1, const int8_t *s2, size_t n)
{
    register int32_t ret = 0;

    if (NULL == s1 || NULL == s2) {
        while(1); /* FIXME(junlin) should be panic? */
    }
    while(n-- && !(ret = (*s1 - *s2++)) && *s1++);
    return ret;
}

size_t strlen(const int8_t *s)
{
    register int32_t ret = 0;

    if (NULL == s) {
        return 0;
    }

    while(s[ret++]);
    return ret-1;
}

int8_t *strrchr(const int8_t *s, int32_t c)
{
    register int8_t *ret = NULL;

    if (NULL == s) {
        return NULL;
    }
    do {
        if (*s == (int8_t)c) {
            ret = (int8_t *)s;
        }
    } while (*s++);
    return ret;
}

void *memset(void *s, int32_t c, size_t n)
{
    int8_t *xs = s;

    if (NULL == s) {
        return NULL;
    }
    while(n--) {
        *xs++ = (int8_t)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    register const int8_t *s = src;
    register int8_t *d = dest;

    if (NULL == dest || NULL == src) {
        return NULL;
    }
    while(n--) {
        d[n] = s[n];
    }
    return dest;
}

int32_t memcmp(const void *s1, const void *s2, size_t n)
{
    register const int8_t *s = s1;
    register const int8_t *d = s2;
    register int32_t ret = 0;

    if (NULL == s1 || NULL == s2) {
        while(1); /* FIXME(junlin) should be panic? */
    }
    while(n-- && !(ret = (*s++ - *d++)));
    return ret;
}

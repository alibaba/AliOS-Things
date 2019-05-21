#include "include.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

INT32 os_memcmp(const void *s1, const void *s2, UINT32 n)
{
    return memcmp(s1, s2, (unsigned int)n);
}

void *os_memmove(void *out, const void *in, UINT32 n)
{
    return memmove(out, in, n);
}

void *os_memcpy(void *out, const void *in, UINT32 n)
{
    return memcpy(out, in, n);
}

void *os_memset(void *b, int c, UINT32 len)
{
    return (void *)memset(b, c, (unsigned int)len);
}

void *os_realloc(void *ptr, size_t size)
{
	return (void *)aos_realloc(ptr, size);
}

int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	size_t i;
	u8 res;

	for (res = 0, i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];

	return res;
}

void *os_malloc(size_t size)
{
	return (void *)aos_malloc(size);
}

void * os_zalloc(size_t size)
{
	return (void *)aos_zalloc(size);
}

void os_free(void *ptr)
{
	if(ptr)
	{
		aos_free(ptr);
	}
}
// EOF

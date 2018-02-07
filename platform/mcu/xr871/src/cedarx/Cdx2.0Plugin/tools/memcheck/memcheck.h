#ifndef MEMCHECK_H
#define MEMCHECK_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECSIZE 8*1024

typedef struct _mymeminfo mymeminfo;

struct _mymeminfo
{
	int size;
	unsigned int ptr;
	const char* name;
	int  line;
	int  malloc;
	int  free;
};

void* ckmalloc(int size, const char* name, int line);
void ckfree(void* ptr);
int getmallocsz();
int getfreesz();
void showmeinfo();
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
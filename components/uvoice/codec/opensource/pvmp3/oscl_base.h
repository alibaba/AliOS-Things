#ifndef OSCL_BASE_INCLUDED_H
#define OSCL_BASE_INCLUDED_H
#include "stdint.h"

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define OSCL_UNUSED_ARG(vbl) (void)(vbl)

#define OSCL_IMPORT_REF
#define OSCL_EXPORT_REF

#define OSCL_LEAVE(err_code) do \
{ \
	return NULL; \
} while(0)

#endif
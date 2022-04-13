#ifndef OSCL_MEM_INCLUDED_H
#define OSCL_MEM_INCLUDED_H

#include "stdlib.h"
#include "string.h"

#define oscl_memcpy memcpy
#define oscl_memset memset
#define oscl_memmove memmove

#define OSCL_ARRAY_DELETE(ptr) delete [] ptr
#define OSCL_ARRAY_NEW(T, count) new T[count]

#define OSCL_DELETE(ptr) {\
    if(ptr){delete(ptr);}\
}

#endif

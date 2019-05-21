#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdint.h>
//#include <stddef.h>

typedef unsigned char  		  uint8;          /* unsigned  8 bit quantity        */
typedef signed   char  		  int8;           /* signed    8 bit quantity        */
typedef unsigned short 		  uint16;         /* unsigned 16 bit quantity        */
typedef signed   short 		  int16;          /* signed   16 bit quantity        */
typedef unsigned int   		  uint32;         /* unsigned 32 bit quantity        */
typedef signed   int   		  int32;          /* signed   32 bit quantity        */
typedef unsigned long long    uint64;			/* unsigned 32 bit quantity        */
typedef signed   long long    int64;			/* signed   32 bit quantity        */

typedef unsigned char  		  UINT8;          /* Unsigned  8 bit quantity        */
typedef signed   char  		  INT8;           /* Signed    8 bit quantity        */
typedef unsigned short 		  UINT16;         /* Unsigned 16 bit quantity        */
typedef signed   short 		  INT16;          /* Signed   16 bit quantity        */
typedef unsigned int   		  UINT32;         /* Unsigned 32 bit quantity        */
typedef signed   int   		  INT32;          /* Signed   32 bit quantity        */
typedef unsigned long long    UINT64;			/* Unsigned 32 bit quantity        */
typedef signed   long long    INT64;			/* Signed   32 bit quantity        */
typedef float         		  FP32;			/* Single precision floating point */
typedef double         		  FP64;			/* Double precision floating point */
typedef unsigned int          size_t;
typedef unsigned char         BOOLEAN;
typedef unsigned char         BOOL;
//typedef unsigned char         bool;

#define LPVOID              void *
#define VOID                void

typedef volatile signed long  VS32;
typedef volatile signed short VS16;
typedef volatile signed char  VS8;

typedef volatile signed long  const VSC32;  
typedef volatile signed short const VSC16;  
typedef volatile signed char  const VSC8;  

typedef volatile unsigned long  VU32;
typedef volatile unsigned short VU16;
typedef volatile unsigned char  VU8;

typedef volatile unsigned long  const VUC32;  
typedef volatile unsigned short const VUC16;  
typedef volatile unsigned char  const VUC8; 

#ifndef HAVE_UTYPES
typedef unsigned char              u8;
typedef signed char                s8;
typedef unsigned short             u16;
typedef signed short               s16;
typedef unsigned int               u32;
typedef signed int                 s32;
#endif /* HAVE_UTYPES */

typedef unsigned long long         u64;
typedef long long                  s64;

typedef unsigned int __u32;
typedef int __s32;
typedef unsigned short __u16;
typedef signed short __s16;
typedef unsigned char __u8;

#endif // _TYPEDEF_H_
// eof


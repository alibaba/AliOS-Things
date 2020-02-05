#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdint.h>
#include <stdbool.h>

typedef unsigned char  		  UINT8;          /* Unsigned  8 bit quantity        */
typedef signed   char  		  INT8;           /* Signed    8 bit quantity        */
typedef unsigned short 		  UINT16;         /* Unsigned 16 bit quantity        */
typedef signed   short 		  INT16;          /* Signed   16 bit quantity        */
typedef uint32_t   		      UINT32;         /* Unsigned 32 bit quantity        */
typedef int32_t   		      INT32;          /* Signed   32 bit quantity        */
typedef unsigned long long    UINT64;			/* Unsigned 32 bit quantity        */
typedef signed   long long    INT64;			/* Signed   32 bit quantity        */
typedef float         		  FP32;			/* Single precision floating point */
typedef double         		  FP64;			/* Double precision floating point */
typedef unsigned char         BOOLEAN;
#undef BOOL
typedef unsigned char         BOOL;
typedef unsigned int          size_t;

#define LPVOID              void *
#define DWORD               UINT32
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

typedef unsigned char              u8;
typedef signed char                s8;
typedef unsigned short             u16;
typedef signed short               s16;
typedef unsigned int               u32;
typedef signed int                 s32;
typedef unsigned long long         u64;
typedef long long                  s64;

typedef unsigned int __u32;
typedef int __s32;
typedef unsigned short __u16;
typedef signed short __s16;
typedef unsigned char __u8;

#endif // _TYPEDEF_H_
// eof


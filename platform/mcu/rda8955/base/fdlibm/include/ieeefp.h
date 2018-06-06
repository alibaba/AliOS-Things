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

#ifndef __CLASSPATH_IEEEFP_H__
#define __CLASSPATH_IEEEFP_H__

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN

#ifdef __alpha__
#define __IEEE_LITTLE_ENDIAN
#endif

#if defined(__arm__) || defined(__thumb__)
/* ARM traditionally used big-endian words; and within those words the
   byte ordering was big or little endian depending upon the target.
   Modern floating-point formats are naturally ordered; in this case
   __VFP_FP__ will be defined, even if soft-float.  */
#ifdef __VFP_FP__
#ifdef __ARMEL__
#define __IEEE_LITTLE_ENDIAN
#else
#define __IEEE_BIG_ENDIAN
#endif
#else
#define __IEEE_BIG_ENDIAN
#ifdef __ARMEL__
#define __IEEE_BYTES_LITTLE_ENDIAN
#endif
#endif
#endif

#ifdef __hppa__
#define __IEEE_BIG_ENDIAN
#endif

#if defined (__sparc) || defined (__sparc__)
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __m32r__
#ifdef __LITTLE_ENDIAN__
#define __IEEE_LITTLE_ENDIAN
#else
#define __IEEE_BIG_ENDIAN
#endif
#endif

#if defined(__m68k__) || defined(__mc68000__)
#define __IEEE_BIG_ENDIAN
#endif

#if defined (__H8300__) || defined (__H8300H__)
#define __IEEE_BIG_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#ifdef __H8500__
#define __IEEE_BIG_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#ifdef __sh__
#ifdef __LITTLE_ENDIAN__
#define __IEEE_LITTLE_ENDIAN
#else
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __SH3E__
#define _DOUBLE_IS_32BITS
#endif
#endif

#ifdef _AM29K
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __i386__
#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __x86_64__
#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __i960__
#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __AVR32__
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __MIPSEL__
#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __MIPSEB__
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __pj__
#ifdef __pjl__
#define __IEEE_LITTLE_ENDIAN
#else
#define __IEEE_BIG_ENDIAN
#endif
#endif

/* necv70 was __IEEE_LITTLE_ENDIAN. */

#ifdef __W65__
#define __IEEE_LITTLE_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#if defined(__Z8001__) || defined(__Z8002__)
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __m88k__
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __v800
#define __IEEE_LITTLE_ENDIAN
#endif

#if defined (__PPC__) || defined (__ppc__) || defined (__powerpc__) || defined (__ppc64__) || defined (_POWER) || defined (_IBMR2)
#if (defined(_BIG_ENDIAN) && _BIG_ENDIAN) || (defined(_AIX) && _AIX) || defined (__BIG_ENDIAN__)|| defined (__APPLE__)
#define __IEEE_BIG_ENDIAN
#else
#if (defined(_LITTLE_ENDIAN) && _LITTLE_ENDIAN) || (defined(__sun__) && __sun__) || (defined(__WIN32__) && __WIN32__)
#define __IEEE_LITTLE_ENDIAN
#endif
#endif
#endif

#ifdef __fr30__
#define __IEEE_BIG_ENDIAN
#endif

#ifdef __mcore__
#define __IEEE_BIG_ENDIAN
#endif


#ifdef __ia64__
#ifdef __BIG_ENDIAN__
#define __IEEE_BIG_ENDIAN
#else
#define __IEEE_LITTLE_ENDIAN
#endif
#endif

#ifdef __s390__
#define __IEEE_BIG_ENDIAN
#endif

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN
#error Endianess not declared!!
#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

#endif /* __CLASSPATH_IEEEFP_H__ */

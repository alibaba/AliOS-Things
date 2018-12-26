#ifndef _INTTYPES_H
#define _INTTYPES_H 1

#if !defined __cplusplus || defined __STDC_FORMAT_MACROS

# if __WORDSIZE == 64
#  define __PRI64_PREFIX    "l"
#  define __PRIPTR_PREFIX   "l"
# else
#  define __PRI64_PREFIX    "ll"
#  define __PRIPTR_PREFIX
# endif

/* Unsigned integers.  */
# define PRIu8      "u"
# define PRIu16     "u"
# define PRIu32     "u"
# define PRIu64     __PRI64_PREFIX "u"

#endif

#endif

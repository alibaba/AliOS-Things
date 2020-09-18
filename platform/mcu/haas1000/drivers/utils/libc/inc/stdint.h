#ifndef STDINT_H
#define STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 7.18.2 Limits of specified-width integer types
 */

/* 7.18.2.1 Limits of exact-width integer types */

/* minimum values of exact-width signed integer types */
#define	INT8_MIN	(-INT8_MAX-1)			/* int8_t	  */
#define	INT16_MIN	(-INT16_MAX-1)			/* int16_t	  */
#define	INT32_MIN	(-INT32_MAX-1)			/* int32_t	  */
#define	INT64_MIN	(-INT64_MAX-1)			/* int64_t	  */

#define SCHAR_MIN	(-SCHAR_MAX-1)			/* signed char */
#define SHRT_MIN	(-SHRT_MAX-1)			/* short      */
#define	INT_MIN		(-INT_MAX-1)			/* int	      */
#define	LONG_MIN	(-LONG_MAX-1)			/* long	      */
#define	LLONG_MIN	(-LLONG_MAX-1)			/* long long  */

#ifdef __INT8_MAX__
/* maximum values of exact-width signed integer types */
#define	INT8_MAX	__INT8_MAX__			/* int8_t	  */
#define	INT16_MAX	__INT16_MAX__			/* int16_t	  */
#define	INT32_MAX	__INT32_MAX__			/* int32_t	  */
#define	INT64_MAX	__INT64_MAX__			/* int64_t	  */

#define	SCHAR_MAX	__SCHAR_MAX__			/* signed char */
#define	SHRT_MAX	__SHRT_MAX__			/* short      */
#define	INT_MAX		__INT_MAX__				/* int        */
#define	LONG_MAX	__LONG_MAX__			/* long       */
#define	LLONG_MAX	__LONG_LONG_MAX__		/* long long  */

/* maximum values of exact-width unsigned integer types */
#define	UINT8_MAX	__UINT8_MAX__			/* uint8_t	  */
#define	UINT16_MAX	__UINT16_MAX__			/* uint16_t	  */
#define	UINT32_MAX	__UINT32_MAX__			/* uint32_t	  */
#define	UINT64_MAX	__UINT64_MAX__			/* uint64_t	  */

#else
/* maximum values of exact-width signed integer types */
#define	INT8_MAX	0x7f					/* int8_t	  */
#define	INT16_MAX	0x7fff					/* int16_t	  */
#define	INT32_MAX	0x7fffffff				/* int32_t	  */
#define	INT64_MAX	0x7fffffffffffffffLL	/* int64_t	  */

#define	SCHAR_MAX	0x7f					/* signed char */
#define	SHRT_MAX	0x7fff					/* short      */
#define	INT_MAX		0x7fffffff				/* int        */
#define	LONG_MAX	0x7fffffff				/* long       */
#define	LLONG_MAX	0x7fffffffffffffffLL	/* long long  */

/* maximum values of exact-width unsigned integer types */
#define	UINT8_MAX	0xff					/* uint8_t	  */
#define	UINT16_MAX	0xffff					/* uint16_t	  */
#define	UINT32_MAX	0xffffffffU				/* uint32_t	  */
#define	UINT64_MAX	0xffffffffffffffffULL	/* uint64_t	  */
#endif

#define	USCHAR_MAX	(2U*SCHAR_MAX+1)			/* unsigned char */
#define	USHRT_MAX	(2U*SHRT_MAX+1)			/* unsigned short */
#define	UINT_MAX	(2U*INT_MAX+1)			/* unsigned int */
#define	ULONG_MAX	(2UL*LONG_MAX+1)			/* unsigned long */
#define	ULLONG_MAX	(2ULL*LLONG_MAX+1)			/* unsigned long long */

/* 7.18.2.2 Limits of minimum-width integer types */

/* minimum values of minimum-width signed integer types */
#define	INT_LEAST8_MIN	(-INT_LEAST8_MAX-1)		/* int_least8_t	  */
#define	INT_LEAST16_MIN	(-INT_LEAST16_MAX-1)	/* int_least16_t  */
#define	INT_LEAST32_MIN	(-INT_LEAST32_MAX-1)	/* int_least32_t  */
#define	INT_LEAST64_MIN	(-INT_LEAST64_MAX-1)	/* int_least64_t  */

#ifdef __INT_LEAST8_MAX__
/* maximum values of minimum-width signed integer types */
#define	INT_LEAST8_MAX	__INT_LEAST8_MAX__		/* int_least8_t	  */
#define	INT_LEAST16_MAX	__INT_LEAST16_MAX__		/* int_least16_t  */
#define	INT_LEAST32_MAX	__INT_LEAST32_MAX__		/* int_least32_t  */
#define	INT_LEAST64_MAX	__INT_LEAST64_MAX__		/* int_least64_t  */

/* maximum values of minimum-width unsigned integer types */
#define	UINT_LEAST8_MAX	 __UINT_LEAST8_MAX__	/* uint_least8_t  */
#define	UINT_LEAST16_MAX __UINT_LEAST16_MAX__	/* uint_least16_t */
#define	UINT_LEAST32_MAX __UINT_LEAST32_MAX__	/* uint_least32_t */
#define	UINT_LEAST64_MAX __UINT_LEAST64_MAX__	/* uint_least64_t */
#else
/* maximum values of minimum-width signed integer types */
#define	INT_LEAST8_MAX	0x7f				/* int_least8_t	  */
#define	INT_LEAST16_MAX	0x7fff				/* int_least16_t  */
#define	INT_LEAST32_MAX	0x7fffffff			/* int_least32_t  */
#define	INT_LEAST64_MAX	0x7fffffffffffffffLL	/* int_least64_t  */

/* maximum values of minimum-width unsigned integer types */
#define	UINT_LEAST8_MAX	 0xff				/* uint_least8_t  */
#define	UINT_LEAST16_MAX 0xffff				/* uint_least16_t */
#define	UINT_LEAST32_MAX 0xffffffffU		/* uint_least32_t */
#define	UINT_LEAST64_MAX 0xffffffffffffffffULL	/* uint_least64_t */
#endif

/* 7.18.2.3 Limits of fastest minimum-width integer types */

/* minimum values of fastest minimum-width signed integer types */
#define	INT_FAST8_MIN	(-INT_FAST8_MAX-1)	/* int_fast8_t	  */
#define	INT_FAST16_MIN	(-INT_FAST16_MAX-1)	/* int_fast16_t	  */
#define	INT_FAST32_MIN	(-INT_FAST32_MAX-1)	/* int_fast32_t	  */
#define	INT_FAST64_MIN	(-INT_FAST64_MAX-1)	/* int_fast64_t	  */

#ifdef INT_FAST8_MAX
/* maximum values of fastest minimum-width signed integer types */
#define	INT_FAST8_MAX	__INT_FAST8_MAX__	/* int_fast8_t	  */
#define	INT_FAST16_MAX	__INT_FAST16_MAX__	/* int_fast16_t	  */
#define	INT_FAST32_MAX	__INT_FAST32_MAX__	/* int_fast32_t	  */
#define	INT_FAST64_MAX	__INT_FAST64_MAX__	/* int_fast64_t	  */

/* maximum values of fastest minimum-width unsigned integer types */
#define	UINT_FAST8_MAX	__UINT_FAST8_MAX__	/* uint_fast8_t	  */
#define	UINT_FAST16_MAX	__UINT_FAST16_MAX__	/* uint_fast16_t  */
#define	UINT_FAST32_MAX	__UINT_FAST32_MAX__	/* uint_fast32_t  */
#define	UINT_FAST64_MAX	__UINT_FAST64_MAX__	/* uint_fast64_t  */
#else
/* maximum values of fastest minimum-width signed integer types */
#define	INT_FAST8_MAX	0x7fffffff			/* int_fast8_t	  */
#define	INT_FAST16_MAX	0x7fffffff			/* int_fast16_t	  */
#define	INT_FAST32_MAX	0x7fffffff			/* int_fast32_t	  */
#define	INT_FAST64_MAX	0x7fffffffffffffffLL	/* int_fast64_t	  */

/* maximum values of fastest minimum-width unsigned integer types */
#define	UINT_FAST8_MAX	0xffffffffU			/* uint_fast8_t	  */
#define	UINT_FAST16_MAX	0xffffffffU			/* uint_fast16_t  */
#define	UINT_FAST32_MAX	0xffffffffU			/* uint_fast32_t  */
#define	UINT_FAST64_MAX	0xffffffffffffffffULL	/* uint_fast64_t  */
#endif

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#define	INTPTR_MIN	(-INTPTR_MAX-1)			/* intptr_t	  */
#ifdef __INTPTR_MAX__
#define	INTPTR_MAX	__INTPTR_MAX__			/* intptr_t	  */
#define	UINTPTR_MAX	__UINTPTR_MAX__			/* uintptr_t  */
#else
#define	INTPTR_MAX	0x7fffffffL				/* intptr_t	  */
#define	UINTPTR_MAX	0xffffffffUL			/* uintptr_t  */
#endif

/* 7.18.2.5 Limits of greatest-width integer types */

#define	INTMAX_MIN	(-INTMAX_MAX-1)			/* intmax_t	  */
#ifdef __INTMAX_MAX__
#define	INTMAX_MAX	__INTMAX_MAX__			/* intmax_t	  */
#define	UINTMAX_MAX	__UINTMAX_MAX__			/* uintmax_t  */
#else
#define	INTMAX_MAX	0x7fffffffffffffffLL	/* intmax_t	  */
#define	UINTMAX_MAX	0xffffffffffffffffULL	/* uintmax_t  */
#endif

/*
 * 7.18.3 Limits of other integer types
 */

/* limits of ptrdiff_t */
#define	PTRDIFF_MIN	(-PTRDIFF_MAX-1)		/* ptrdiff_t	  */
#ifdef __PTRDIFF_MAX__
#define	PTRDIFF_MAX	__PTRDIFF_MAX__			/* ptrdiff_t	  */
#else
#define	PTRDIFF_MAX	0x7fffffffL				/* ptrdiff_t	  */
#endif

/* limits of sig_atomic_t */
#define	SIG_ATOMIC_MIN	(-SIG_ATOMIC_MAX-1)	/* sig_atomic_t	  */
#ifdef __SIG_ATOMIC_MAX__
#define	SIG_ATOMIC_MAX	__SIG_ATOMIC_MAX__	/* sig_atomic_t	  */
#else
#define	SIG_ATOMIC_MAX	0x7fffffff			/* sig_atomic_t	  */
#endif

/* limit of size_t */
#ifdef __SIZE_MAX__
#define	SIZE_MAX	__SIZE_MAX__			/* size_t	  */
#else
#define	SIZE_MAX	0xffffffffUL			/* size_t	  */
#endif


typedef unsigned char               uint8_t;
typedef signed char                 int8_t;
typedef unsigned short int          uint16_t;
typedef signed short int            int16_t;
typedef unsigned int                uint32_t;
typedef signed int                  int32_t;
typedef char                        boolean;

typedef unsigned long long          uint64_t;
typedef signed long long            int64_t;

#ifndef size_t
typedef unsigned int                size_t;
#endif

#ifndef ssize_t
typedef int                         ssize_t;
#endif

typedef unsigned long long          uintmax_t;
typedef long long                   intmax_t;

typedef int                         intptr_t;

typedef unsigned int                uintptr_t;

typedef int                         ptrdiff_t;

typedef int                         sig_atomic_t;

#ifdef __cplusplus
}
#endif

#endif


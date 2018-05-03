#ifndef _SSV_ASSERT_H_
#define _SSV_ASSERT_H_


#define ASSERT_COND(x) \
{ \
    if (!(x)) \
    { \
        while(1); \
    } \
}

// aMH todo: remove printf
// in rom no stop_and_halt support.
#if defined(BUILD_ROM)
#define ASSERT(x) \
{ \
    extern void stop_and_halt (void); \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        while(1) {} \
    } \
}
#define ASSERT_RET(x, ret) \
{ \
    extern void stop_and_halt (void); \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        while(1) {} \
        return ret; \
    } \
}
#else

#define assert ASSERT
#define ASSERT(x) \
{ \
    extern void stop_and_halt (void); \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        stop_and_halt(); \
    } \
}
#define ASSERT_RET(x, ret) \
{ \
    extern void stop_and_halt (void); \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        stop_and_halt(); \
        return ret; \
    } \
}
#endif

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
  #define STATIC_ASSERT(e,m) \
        ;enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
  /* This can't be used twice on the same line so ensure if using in headers
   * that the headers are not included twice (by wrapping in #ifndef...#endif)
   * Note it doesn't cause an issue when used on same line of separate modules
   * compiled with gcc -combine -fwhole-program.  */
  #define STATIC_ASSERT(e,m) \
        ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif


#define ASSERT_PKT(x, pkt) \
{ \
    extern void stop_and_dump_and_halt (const void *data, u32 size); \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        stop_and_dump_and_halt((const void *)pkt, 64); \
    } \
}


#endif /* _ASSERT_H _*/

#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#undef assert
#endif

#define STRINGIFY_VALUE(s) STRINGIFY(s)
#define STRINGIFY(s) #s

void __assert(const char* fmt);

#define assert(boolcondition) { if(!(boolcondition)) __assert( \
    "ASSERTION FAILED in file : " \
    __FILE__ \
    " at line" \
    STRINGIFY_VALUE(__LINE__)); }

#ifdef __cplusplus
} // extern C
#endif

#endif /* _ASSERT_H_ */

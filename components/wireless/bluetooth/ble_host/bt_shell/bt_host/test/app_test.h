#include <aos/kernel.h>
#include <stddef.h>

/*
#define ASSERT(expr)    \
     (expr?0: printf("ASSERTION Failed %s: %d\n", __FILE__,__LINE__))
#define TEST_ASSERT_EQUAL(a,b)   ASSERT(a==b)
#define TEST_ASSERT_NOT_EQUAL(a,b)   ASSERT(a!=b)

#define TEST_ASSERT(expr)      ASSERT(expr)

#define TEST_ASSERT_TRUE(expr)   ASSERT(expr)

#define TEST_ASSERT_FALSE(expr)   ASSERT(!(expr))

#define ASSERT_STRING(a, b ,c)    \
    if(memcmp(a,b,c) != 0) {   \
        printf("ASSERTION Failed %s: %d\n", __FILE__,__LINE__); \
    }

#define ASSERT_STRING_FALSE(a, b ,c)    \
    if(memcmp(a,b,c) == 0) {   \
        printf("ASSERTION Failed %s: %d\n", __FILE__,__LINE__); \
    }
*/

#define TEST_ASSERT_EQUAL(a,b,fail_q,args...)   \
     if( a!= b) {\
         printf("ASSERTION Failed %s: %d", __FILE__,__LINE__); \
         printf(" >>> "args); \
         printf("\n"); \
         printf("Expected value is %d, actual value is %d\n", a, b); \
		 if (fail_q == 1) { \
         	fail_count++; \
		} \
     }

#define TEST_ASSERT_NOT_EQUAL(a,b,fail_q,args...)   \
     if( a== b) {\
         printf("ASSERTION Failed %s: %d", __FILE__,__LINE__); \
         printf(" >>> "args); \
         printf("\n"); \
         printf("NOT Expected value is  %d, but actual value is %d\n", a, b); \
		 if (fail_q == 1) { \
            fail_count++; \
        } \
     }

#define ASSERT_NULL(a,fail_q,args...)   \
     if( a != NULL) {\
         printf("ASSERTION Failed %s: %d", __FILE__,__LINE__); \
         printf(" >>> "args); \
         printf("\n"); \
         printf("Expected value is NULL, but actual value is %p\n", a); \
		 if (fail_q == 1) { \
            fail_count++; \
        } \
     }


#define ASSERT_NOT_NULL(a,fail_q,args...)   \
     if( a == NULL) {\
         printf("ASSERTION Failed %s: %d", __FILE__,__LINE__); \
         printf(" >>> "args); \
         printf("\n"); \
         printf("Expected value is NULL, but actual value is %p\n", a); \
         if (fail_q == 1) { \
            fail_count++; \
        } \
     }
















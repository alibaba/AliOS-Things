#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "unity.h"

extern volatile int TestingTheFallback;
extern volatile int TestingTheFallbackId;

//Optionally define the exception type (something like an int which can be directly assigned)
#define CEXCEPTION_T    int

// Optionally define the reserved value representing NO EXCEPTION
#define CEXCEPTION_NONE (1234)

// Optionally define a special handler for unhandled exceptions
#define CEXCEPTION_NO_CATCH_HANDLER(id)             \
{                                                   \
    if (!TestingTheFallback)                        \
    {                                               \
        TEST_FAIL_MESSAGE("Unexpected Exception!"); \
    }                                               \
    else                                            \
    {                                               \
        TestingTheFallbackId = id;                  \
        TestingTheFallback--;                       \
    }                                               \
}

// Multi-Tasking environments will need a couple of macros defined to make this library
// properly handle  multiple exception stacks.  You will need to include and required
// definitions, then define the following macros:
//    EXCEPTION_GET_ID - returns the id of the current task indexed 0 to (numtasks - 1)
//    EXCEPTION_NUM_ID - returns the number of tasks that might be returned
//
// For example, Quadros might include the following implementation:
#ifndef TEST
#include "OSAPI.h"
#define CEXCEPTION_GET_ID    (KS_GetTaskID())
#define CEXCEPTION_NUM_ID    (NTASKS + 1)
#endif

//This could be a good place to define/include some error ID's:
#define ERROR_ID_EVERYTHING_IS_BROKEN  (0x88)
#define ERROR_ID_ONLY_THIS_IS_BROKEN   (0x77)

#endif // _EXCEPTION_H

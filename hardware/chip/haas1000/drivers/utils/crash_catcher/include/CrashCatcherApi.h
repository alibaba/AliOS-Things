#ifndef __CRASH_CATCHER_API_H__
#define  __CRASH_CATCHER_API_H__

/* This structure is filled in by the Hard Fault exception handler (or unit test) and then passed in as a parameter to
   CrashCatcher_Entry(). */
typedef struct
{
    uint32_t msp;
    uint32_t psp;
    uint32_t exceptionPSR;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t exceptionLR;
} CrashCatcherExceptionRegisters;

/* The main entry point into CrashCatcher.  Is called from the HardFault exception handler and unit tests. */
void CrashCatcher_Entry(const CrashCatcherExceptionRegisters* pExceptionRegisters);

/* The main entry point When software Assert.*/
void AssertCatcher_Entry(void);

#endif /* __CRASH_CATCHER_API_H__*/

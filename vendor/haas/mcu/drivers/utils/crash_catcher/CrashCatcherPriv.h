/* Copyright (C) 2017  Adam Green (https://github.com/adamgreen)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/* Private header file shared with unit tests. */
#ifndef _CRASH_CATCHER_PRIV_H_
#define _CRASH_CATCHER_PRIV_H_


/* Definitions used by assembly language and C code. */
#define CRASH_CATCHER_STACK_WORD_COUNT 125


/* Definitions only required from C code. */
#if !__ASSEMBLER__

#include <stdint.h>
#include "CrashCatcherApi.h"


/* Bit in LR to indicate whether PSP was used for automatic stacking of registers during exception entry. */
#define LR_PSP (1 << 2)

/* Bit in LR set to 0 when automatic stacking of floating point registers occurs during exception handling. */
#define LR_FLOAT (1 << 4)


/* Bit in auto stacked xPSR which indicates whether stack was force 8-byte aligned. */
#define PSR_STACK_ALIGN (1 << 9)


/* This structure contains the integer registers that are automatically stacked by Cortex-M processor when it enters
   an exception handler. */
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
    /* The following floating point registers are only stacked when the LR_FLOAT bit is set in exceptionLR. */
    uint32_t floats[16];
    uint32_t fpscr;
    uint32_t reserved; /* keeps 8-byte alignment */
} CrashCatcherStackedRegisters;


/* This is the area of memory that would normally be used for the stack when running on an actual Cortex-M
   processor.  Unit tests can write to this buffer to simulate stack overflow. */
extern uint32_t g_crashCatcherStack[CRASH_CATCHER_STACK_WORD_COUNT];


/* Called from CrashCatcher core to copy all floating point registers to supplied buffer. The supplied buffer must be
   large enough to contain 33 32-bit values (S0-S31 & FPCSR). */
void CrashCatcher_CopyAllFloatingPointRegisters(uint32_t* pBuffer);

/* Called from CrashCatcher core to copy upper 16 floating point registers to supplied buffer. The supplied buffer must be
   large enough to contain 16 32-bit values (S16-S31). */
void CrashCatcher_CopyUpperFloatingPointRegisters(uint32_t* pBuffer);

#endif // #if !__ASSEMBLER__


#endif /* _CRASH_CATCHER_PRIV_H_ */

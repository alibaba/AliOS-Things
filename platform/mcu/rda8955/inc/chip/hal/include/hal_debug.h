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


#ifndef _HAL_DBG_H_
#define _HAL_DBG_H_

#include "cs_types.h"
#include "chip_id.h"


/// @defgroup debugPort HAL Debug Port Driver
/// This chapter contains a brief description of the Debug Port
/// possibilities.
///
/// @par Description
/// The chip include 10 lines dedicated to debug.
///
/// @par
/// The pins of the debug port have 4 modes, they can be used to output EXL (@ref debugExl)
/// and/or, PXTS (@ref debugPxts) data, or they can be used to spy signal (@ref signalSpy).
/// Except for PXTS and EXL, those mode cannot be used simultaneously.
///
/// @{



// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_DBG_PXTS_LEVEL_T
// -----------------------------------------------------------------------------
/// The PXTS level on which to send the code. This level is used to
/// filter the PXTS without having to recompile. Generally, each software
/// module has a level attributed.
// =============================================================================
typedef enum
{
    HAL_DBG_PXTS_RESERVED_0  = 0,
    HAL_DBG_PXTS_BOOT        = 1,
    HAL_DBG_PXTS_HAL         = 2,
    HAL_DBG_PXTS_SX          = 3,
    HAL_DBG_PXTS_PAL         = 4,
    HAL_DBG_PXTS_EDRV        = 5,
    HAL_DBG_PXTS_SVC         = 6,
    HAL_DBG_PXTS_STACK       = 7,
    HAL_DBG_PXTS_CSW         = 8,
    HAL_DBG_PXTS_MMI         = 9,
    HAL_DBG_PXTS_RESERVED_10 = 10,
    HAL_DBG_PXTS_RESERVED_11 = 11,
    HAL_DBG_PXTS_BB_IRQ      = 12,
    HAL_DBG_PXTS_SPAL        = 13,
    HAL_DBG_PXTS_SPP         = 14,
    HAL_DBG_PXTS_SPC         = 15,
} HAL_DBG_PXTS_LEVEL_T;



// =============================================================================
// HAL_DBG_PORT_SEL_T
// -----------------------------------------------------------------------------
/// The debug port can be used in five modes:
// =============================================================================
typedef enum
{
    /// The debug port is not used.
    HAL_DBG_NONE,
    /// The debug port is used by the execution logger.
    HAL_DBG_EXL,
    /// The debug port is used for profiling.
    HAL_DBG_PXTS,
    /// Together.
    HAL_DBG_EXL_AND_PXTS,
    /// The debug port is used as a signal spy.
    HAL_DBG_DEBUG_BUS
} HAL_DBG_PORT_SEL_T;



// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// hal_DbgPxtsSetup
// -----------------------------------------------------------------------------
/// Enable the PXTS on the level passed as parameter.
/// That function configure the debug_port in debug and enable the
/// PXTS mode.
/// @param mask A bitfield where a '1' means that the corresponding
/// PXTS level is enabled.
// =============================================================================
PUBLIC VOID hal_DbgPxtsSetup(UINT16 mask);


// =============================================================================
// hal_DbgPortSetup
// -----------------------------------------------------------------------------
/// Select the mode under which use the debug port.
/// @param mode Mode under which the debug port is used. Refer to the type
/// definition for details.
// =============================================================================
PUBLIC VOID hal_DbgPortSetup(HAL_DBG_PORT_SEL_T mode);

/// @} End of the debugPort group







/// @defgroup debugExl HAL Debug EXL
/// This section describes the Embedded Execution Logger.
///
/// It is an embedded hardware that allows non intrusive debug of software
/// which allows a better debug of real time code.
///
/// @par EXL Operation
/// The main problem with real time software is to know what happened before
/// the crash or what happened after a given state is reached. Setting a
/// breakpoint before the time of crash will modify the behavior and will not
/// give any relevant information. \n
/// EXL in combination with the romulator hardware allows to log the cpu
/// program counter before or after a trigger condition has occurred. The
/// program counter can be recorded every time a branch, jump or interrupt jump
/// is detected. Up to 512k addresses can be stored in the romulator hardware
/// (2MB). This allows to completely retrace the execution of the software.
/// To reduce the number of addresses to record and increase the time over
/// which the record occurs, only selected branches and/or interrupt can be
/// recorded.
/// @par
/// EXL tab in Romulator software allows to define a trigger condition (8
/// possible concurrent trigger conditions) and to define the trigger position
/// (record from trigger or record up to trigger). \n
/// In the EXL Viewer software, the recorded log can be matched with the
/// application disassembly for easier debug. This EXL Viewer is not ready yet
/// but should be available soon.
/// @par
/// To use EXL, you must initialize and enable the EXL feature.
/// All fatal error treatment functions (exception handler, assert,
/// raise....) should set the trigger. It should be easy to
/// log the execution before the crash. For specific purposes, trigger bits can
/// be set from your application when you want to start a log. \n
/// Please also note that the EXL pins are shared with the debug port.
/// #hal_DbgPortSetup must be called with #HAL_DBG_EXL or #HAL_DBG_EXL_AND_PXTS,
/// which disables the signal spy feature.
///
/// @par The EXL Trigger
/// The recording of addresses on the romulator stops at a special
/// condition called the trigger condition. The trigger condition is configured
/// from the romulator. To allow the recording to stop at specific
/// part of your code, a call to the #hal_DbgFatalTrigger function.
/// The main usage of EXL is to record what happened before a fatal situation,
/// the software already detects such situation and set the
/// trigger.
///
/// @par Remainder
/// The pins of the debug port have 4 modes, they can be used to output EXL (@ref debugExl)
/// and/or, PXTS (@ref debugPxts) data, or they can be used to spy signal (@ref signalSpy).
/// Except for PXTS and EXL, those mode cannot be used simultaneously.
///
/// @{



// =============================================================================
//  MACROS
// =============================================================================
#define HAL_DBG_EXL_FATAL 1



// =============================================================================
//  TYPES
// =============================================================================
/**
    @brief Code Execution Logger

    Those are the different kind of instruction which are recorded
*/

typedef enum
{
    /// Any jump inst, Any branch inst (taken or not)
    HAL_DBG_EXL_RECORD_JUMP_AND_BRANCH,
    /// Selected branch inst (taken or not)
    HAL_DBG_EXL_RECORD_BRANCH,
    /// Selected branch inst, branch not taken
    HAL_DBG_EXL_RECORD_BRANCH_NOT_TAKEN,
    /// Selected branch inst, branch taken
    HAL_DBG_EXL_RECORD_BRANCH_TAKEN,
    /// No jump/branch inst (useful for tracing interrupts only)
    HAL_DBG_EXL_RECORD_NO_JUMP_NO_BRANCH
} HAL_DBG_EXL_RECORD_TYPE_T;



/**
    @brief Branch types

    This can be used to select only a type of branch
*/
typedef enum
{
    /// select branch taken if <0
    HAL_DBG_EXL_BRACH_TYPE_BLTZ,
    /// select branch taken if >=0
    HAL_DBG_EXL_BRACH_TYPE_BGEZ,
    /// select branch taken if =
    HAL_DBG_EXL_BRACH_TYPE_BEQ,
    /// select branch taken if !=
    HAL_DBG_EXL_BRACH_TYPE_BNE,
    /// select branch taken if <=0
    HAL_DBG_EXL_BRACH_TYPE_BLEZ,
    /// select branch taken if >0
    HAL_DBG_EXL_BRACH_TYPE_BGTZ,
    /// select branch taken if coprocessor 1 false
    HAL_DBG_EXL_BRACH_TYPE_BC1F,
    /// select branch taken if coprocessor 1 true
    HAL_DBG_EXL_BRACH_TYPE_BC1T,
    /// select branch taken if coprocessor 2 false
    HAL_DBG_EXL_BRACH_TYPE_BC2F,
    /// select branch taken if coprocessor 2 true
    HAL_DBG_EXL_BRACH_TYPE_BC2T,
    /// select no branch, useful to track only jump instructions
    HAL_DBG_EXL_BRACH_TYPE_NO_BRANCH,
    /// select all branch
    HAL_DBG_EXL_BRACH_TYPE_ANY_BRANCH
} HAL_DBG_EXL_BRANCH_TYPE_T;

/**
    @brief Used to choose which CPU is observed
*/
typedef enum
{
    HAL_DBG_EXL_XCPU, ///< Record PC from the XCPU
    HAL_DBG_EXL_BCPU  ///< Record PC from the BCPU
} HAL_DBG_EXL_CPU_SEL_T;

/**
    @brief Configure logging

    This function is used to configure the logging parameters: which CPU is
    recorded, what is the branch type to be looked at, and what is the record
    type (branch taken or not, both, etc ...) \n
    This function doesn't start the logging.

    @param RecordType Type of the record (jump and branch, branch not taken, ...)
    @param EnableInterruptLog Trigs also when an interrupt occurs
    @param BranchType Type of the branch (BLTZ, BNE,...)
    @param CPU Selects the CPU whose PCs are recorded

*/
PUBLIC VOID hal_DbgExlCfg(
    HAL_DBG_EXL_RECORD_TYPE_T recordType,
    BOOL ENableInterruptLog,            // triggers also when an interrupt occurs
    HAL_DBG_EXL_BRANCH_TYPE_T branchType,
    HAL_DBG_EXL_CPU_SEL_T cpu);



// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_DbgFatalTrigger
// -----------------------------------------------------------------------------
/// Triggers a fatal error.
/// This trigger is sent to the Flamulator tool.
// =============================================================================
PUBLIC VOID hal_DbgFatalTrigger(UINT8 trigId);



///  @} <- End of the exl group



/// @defgroup debugPxts HAL Debug PXTS
/// This describes the profiling features providing by HAL. These functions
/// can be used with a profiling tool to trace the execution of the software
/// on the chip from a host computer, to know when functions are called,
/// when tasks are executed, etc ...
///
/// For all those functions to be effective, the PXTS mode must be enabled
/// on the debug port, which is done by a call to #hal_DbgPortSetup with the
/// #HAL_DBG_PXTS or #HAL_DBG_EXL_AND_PXTS. This prevent from using the
/// signal spy feature.
///
/// @par Remainder
/// The pins of the debug port have 4 modes, they can be used to output EXL (@ref debugExl)
/// and/or, PXTS (@ref debugPxts) data, or they can be used to spy signal (@ref signalSpy).
/// Except for PXTS and EXL, those mode cannot be used simultaneously.
/// @{


// =============================================================================
// hal_DbgPxtsSetEnabled
// -----------------------------------------------------------------------------
/// Enable or disable all profiling.
// =============================================================================
PUBLIC VOID hal_DbgPxtsSetEnabled(BOOL enabled);


// =============================================================================
// hal_DbgPxtsSendTrigger
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pre-defined "trigger
/// tag". If the "PXTS Trigger" mode is enabled in the profiling tool, this
/// "trigger tag" will stop the PXTS recording. This can be used to stop the
/// PXTS recording from the embedded code.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsSendTrigger(HAL_DBG_PXTS_LEVEL_T level);



// =============================================================================
// hal_DbgPxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId);



// =============================================================================
// hal_DbgPxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId);



// =============================================================================
// hal_DbgPxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window.
///
/// @param windowId Code representing the window which has been entered in.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId);



// =============================================================================
// hal_DbgPxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window.
///
/// @param windowId Code representing the window which has been exited.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId);



// =============================================================================
// hal_DbgPxtsProfileTaskEnter
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code corresponding to scheduled task Id.
///
/// @param taskId Task id of the scheduled task.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileTask(HAL_DBG_PXTS_LEVEL_T level,
                                   UINT16 taskId);



// =============================================================================
// hal_DbgPxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse.
///
/// @param code Code representing the pulse
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_LEVEL_T level,
                                    UINT16 code);



// =============================================================================
// hal_DbgPxtsProfileJobEnter
// -----------------------------------------------------------------------------
/// This job has to be called when entering the job you want to profile.
///
/// @param jobId Representing the job.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileJobEnter(HAL_DBG_PXTS_LEVEL_T level,
                                       UINT16 jobId);



// =============================================================================
// hal_DbgPxtsProfileJobExit
// -----------------------------------------------------------------------------
/// This job has to be called when exiting the job you want to profile.
///
/// @param jobId Code representing the job.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID hal_DbgPxtsProfileJobExit(HAL_DBG_PXTS_LEVEL_T level,
                                      UINT16 jobId);


#if  defined(HAL_PROFILE_NOT_ON_ROMULATOR)
// =============================================================================
// hal_DbgPxtsNotOnRomulatorEnableFromWithin
// -----------------------------------------------------------------------------
/// Enable the profiling not on the romulator, from the embedded code, so that
/// the beginning of the execution can be profiled, even before CoolWatcher
/// Profiler Plugin has time to configure profiling.
///
/// @param ramMode. If \c TRUE, the profiling is done in a Ram buffer, whose
/// size is passed as the second parameter. If \c FALSE, profiling is done through
/// a trace flow, of id SXS_PROFILE_RMC.
/// @param size Size of the buffer to allocate.
/// @param maskLevel Bitfield of the profiling levels to enable.
// =============================================================================
PUBLIC VOID hal_DbgPxtsNotOnRomulatorEnableFromWithin(BOOL ramMode, UINT32 bufferSize,
        UINT16 maskLevel);

#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR)

/// @} // End of the profiling group







/// @defgroup error_handling HAL Debug Error Handling
/// This chapter describes the error handling mechanism.
///
/// @par Error Handling Operation
/// When an error occurs, the error handler will call this CSW function:
/// @code   extern CSW_ErrorEntry( UINT32 cpuid,
///                         HAL_ERROR_CODE_T errorCode,
///                         UINT32 status,
///                         HHAL_DBG_ERROR_CTX_T* ctx)
///                         @endcode
/// where:
///     - cpuid : 0 XCPU, 1 BCPU
///     - Error_Code : see type definition below (#HAL_DBG_ERROR_CODE_T)
///     - status : see below
///     - ctx : pointer to full context (cf. #HAL_DBG_ERROR_CTX_T)
///
/// When the function returns:
/// - If compiled in "release", the error handler will do a soft reset
/// - If compiled in "debug", the error handler will fallback to gdb
///
/// @{
///



// =============================================================================
//  TYPES
// =============================================================================



// =============================================================================
// HAL_DBG_ERROR_CTX_T
// -----------------------------------------------------------------------------
/// Error context saving structure
/// This structure is used to store all the cpu's registers
// =============================================================================
typedef struct
{
    /// r0
    UINT32 zero;   // r0           //0
    /// r1
    UINT32 at;     // r1           //4
    /// r2
    UINT32 v0;     // r2           //8
    /// r3
    UINT32 v1;     // r3           //c
    /// r4
    UINT32 a0;     // r4           //10
    /// r5
    UINT32 a1;     // r5           //14
    /// r6
    UINT32 a2;     // r6           //18
    /// r7
    UINT32 a3;     // r7           //1c
    /// r8
    UINT32 t0;     // r8           //20
    /// r9
    UINT32 t1;     // r9           //24
    /// r10
    UINT32 t2;     // r10          //28
    /// r11
    UINT32 t3;     // r11          //2c
    /// r12
    UINT32 t4;     // r12          //30
    /// r13
    UINT32 t5;     // r13          //34
    /// r14
    UINT32 t6;     // r14          //38
    /// r15
    UINT32 t7;     // r15          //3c
    /// r16
    UINT32 s0;     // r16          //40
    /// r17
    UINT32 s1;     // r17          //44
    /// r18
    UINT32 s2;     // r18          //48
    /// r19
    UINT32 s3;     // r19          //4c
    /// r20
    UINT32 s4;     // r20          //50
    /// r21
    UINT32 s5;     // r21          //54
    /// r22
    UINT32 s6;     // r22          //58
    /// r23
    UINT32 s7;     // r23          //5c
    /// r24
    UINT32 t8;     // r24          //60
    /// r25
    UINT32 t9;     // r25          //64
    /// r26
    UINT32 k0;     // r26          //68
    /// r27
    UINT32 k1;     // r27          //6c
    /// r28
    UINT32 gp;     // r28          //70
    /// r29
    UINT32 sp;     // r29          //74
    /// r30
    UINT32 fp;     // r30          //78
    /// r31
    UINT32 ra;     // r31          //7c
    ///
    UINT32 sr;                     //80
    UINT32 lo;                     //84
    UINT32 hi;                     //88
    UINT32 bad;                    //8c
    UINT32 cause;                  //90
    /// Program counter
    UINT32 pc;     // pc           //94
} HAL_DBG_ERROR_CTX_T;



// =============================================================================
// HAL_DBG_ERROR_CODE_T
// -----------------------------------------------------------------------------
/// GDB Exception word
/// Those are the error detected by the error handler
// =============================================================================
typedef enum
{
    HAL_DBG_ERROR_NO =           0xC0010000,  ///< no error
    HAL_DBG_ERROR_IT =           0x1717,      ///< cpu exception: value of cause placed id status
    HAL_DBG_ERROR_ASSERT =       0xA55E,      ///< assert (n/a with os)
    HAL_DBG_ERROR_GDB =          0x06DB,      ///< gdb (breakpoint/softbreak)
    HAL_DBG_ERROR_PAGE =         0xFA9E,      /**< page exception: status build
                                        with: (address &0x01FFFFFF) |
                                        ((master&0xF)<<28) |
                                        (page<<26) */

    HAL_DBG_ERROR_WDOG =         0xAD09,      ///< watchdog (n/a no gdb restoration)
    HAL_DBG_ERROR_DIV0 =         0xD1B0,      ///< divide by 0
    HAL_DBG_ERROR_RAISE =        0xA15E,      ///< Raise (stack asserts or pal asserts with os)
    HAL_DBG_ERROR_BCPU =         0xBBBB,      ///< BCPU triggered exception (internal use)
} HAL_DBG_ERROR_CODE_T;


// =============================================================================
// HAL_DBG_ERROR_HANDLER_T
// -----------------------------------------------------------------------------
/// Type used to describe the upper level handler called to manage the error.
/// The first parameter (UINT32) is the cpu Id triggering the error.
/// The second (HAL_DBG_ERROR_CODE_T) is the error code.
/// The third (UINT32) is the error status.
/// The fourth (HAL_DBG_ERROR_CTX_T) is a pointer to the error context.
/// This function is called when an error occurs. If the user doesn't defines
/// any, no function is called.
// =============================================================================
typedef VOID (*HAL_DBG_ERROR_HANDLER_T) (UINT32, HAL_DBG_ERROR_CODE_T, UINT32, HAL_DBG_ERROR_CTX_T*);

// =============================================================================
// HAL_DBG_WATCHDOG_CTX_T
// -----------------------------------------------------------------------------
/// Watchdog context saving structure
/// This is the structure where the context is stored after a watchdog reset
/// has occured
// =============================================================================
typedef struct
{
    UINT32 validityMarker;            /// Marker to check WD context validity
    HAL_DBG_ERROR_CTX_T errorContext;  /// Error context with all the registers
} HAL_DBG_WATCHDOG_CTX_T;


// error handler will call this CSW function
// when the function returns
//   * if compiled in "release", the error handler will do a soft reset
//   * if compiled in "debug", the error handler will fallback to gdb
// cpuid : 0 XCPU, 1 BCPU
// errorCode : see type definition above
// status : see above
// ctx : pointer to full context
//extern CSW_ErrorEntry(UINT32 cpuid, HAL_ERROR_CODE_T errorCode, UINT32 status, HHAL_DBG_ERROR_CTX_T* ctx);
//


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_DbgGdbPrintf
// -----------------------------------------------------------------------------
/// Print to the gdb console
/// @param string Pointer to a string to print to a gdb console
// =============================================================================
PUBLIC VOID hal_DbgGdbPrintf(CONST CHAR *string);


// =============================================================================
// hal_DbgGdbBreakPoint
// -----------------------------------------------------------------------------
/// Set a break point in the source code
/// This function sets a breakpoint for gdb in the directly in the source code.
// =============================================================================
#define hal_DbgGdbBreakPoint() asm volatile("break 1\n\tnop\n\tnop\n\tnop")


// =============================================================================
// HAL_BREAKABLE_FUNCTION
// -----------------------------------------------------------------------------
/// Special flag to let a function being "GDB breakable" (by putting it into
/// RAM). This is necessary if you your code is executed from flash (i.e.
/// the flag WITH_FLASH equals 1) or else GDB cannot write its breakpoint.
/// This flag must be used as follows:
/// VOID mod_MyFunc(VOID) HAL_BREAKABLE_FUNCTION { ... }
// =============================================================================
#define HAL_BREAKABLE_FUNCTION __attribute__((section(".sramtext")))


// =============================================================================
// hal_DbgWatchdogGetContext
// -----------------------------------------------------------------------------
/// Watchdog context saving variable
///
/// This function returns a pointer to the global variable used to store the
/// context of a watchdog reset.
/// As it is read after a reset, (during the boot process) the field \c
/// softResetMarker is used to know if this boot is normal or due to
/// the watchdog, in which case the \c errorContext field is filled with
/// relevant information.
// =============================================================================
PUBLIC HAL_DBG_WATCHDOG_CTX_T* hal_DbgWatchdogGetContext(VOID);


// =============================================================================
// hal_DbgErrorHandlerSet
// -----------------------------------------------------------------------------
/// Set the error handler from the upper layers.
/// @param handler Handler called when an error occurs on one of the CPU.
// =============================================================================
PUBLIC VOID hal_DbgErrorHandlerSet(HAL_DBG_ERROR_HANDLER_T handler);


// =============================================================================
// hal_DbgTriggerBcpuDebugIrq
// -----------------------------------------------------------------------------
/// Trigger BCPU debug irq.
// =============================================================================
PUBLIC VOID hal_DbgTriggerBcpuDebugIrq(VOID);


///  @} <- End of the error_handling group
















/// @defgroup hwBkpt HAL Debug Hardware BreakPoint
/// This document describes the characteristics of the Hardware
/// Breakpoint and how to use them via its Hardware Abstraction Layer API.
///
/// This feature allows to define one hardware breakpoint for the XCPU. This
/// breakpoint can be either on:
/// - an instruction fetch
/// - a data read
/// - a data write
/// .
/// @par
/// When the breakpoint is reached, the CPU is stalled. A status bit allows to
/// know if the breakpoint has been reached or not. When the CPU is stalled,
/// all its internal registers are accessible through the host port. If the
/// breakpoint is released, the CPU resumes its execution.
/// @par
///
/// @{

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_BKPT_TYPE_T
// -----------------------------------------------------------------------------
/// Describes the type of hardware breakpoint
// =============================================================================
typedef enum
{
    HAL_DBG_BKPT_INSTRUCTION=0,     ///< On an instruction
    HAL_DBG_BKPT_DATA_LOAD=1,       ///< On a data load
    HAL_DBG_BKPT_DATA_STORE=2,      ///< On a data store
    HAL_DBG_BKPT_DATA_LOADSTORE=3   ///< On a data access (read or write)
} HAL_DBG_BKPT_TYPE_T;


// =============================================================================
// hal_DbgXcpuBkptSetup
// -----------------------------------------------------------------------------
/// Setups a hardware breakpoint on the XCPU
///
/// @param bkptType Type of the breakpoint
/// @param bkptAddr Address of the hardware breakpoint
// =============================================================================
PUBLIC VOID hal_DbgXcpuBkptSetup(HAL_DBG_BKPT_TYPE_T bkptType, UINT32 bkptAddr);


// =============================================================================
// hal_DbgXcpuBkptDisable
// -----------------------------------------------------------------------------
/// Disables all previously setup breakpoints on the XCPU
// =============================================================================
PUBLIC VOID hal_DbgXcpuBkptDisable(VOID);

// =============================================================================
// hal_DbgBcpuBkptSetup
// -----------------------------------------------------------------------------
/// Setups a hardware breakpoint on the BCPU
///
/// @param bkptType Type of the breakpoint
/// @param bkptAddr Address of the hardware breakpoint
// =============================================================================
PUBLIC VOID hal_DbgBcpuBkptSetup(HAL_DBG_BKPT_TYPE_T bkptType, UINT32 bkptAddr);


// =============================================================================
// hal_DbgBcpuBkptDisable
// -----------------------------------------------------------------------------
/// Disables all previously setup breakpoints on the BCPU
// =============================================================================
PUBLIC VOID hal_DbgBcpuBkptDisable(VOID);

// =============================================================================
// hal_DbgXcpuBkptForce
// -----------------------------------------------------------------------------
/// Stall the CPU where this function is called.
// =============================================================================
PUBLIC VOID hal_DbgXcpuBkptForce(VOID);


///  @} <- End of the hw_bp group








/// @defgroup pageProtection HAL Debug Page Protection Driver
/// This Document describes the page protection mechanism.
///
///
/// This feature allows to track memory corruption by enabling a protection on t
/// he selected area at the beginning of the execution and by disabling it
/// only when it is legitimate to access this area. The access type can be
/// a read, a write or both. If another task tries to
/// access the area, the execution will immediately be halted and you will
/// be able to find the culprit function in the backtrace. Four pages are
/// available, 3 of them are reserved for low level protection, 1 of them are
/// available to the user ( #HAL_DBG_PAGE_UD0). Setting up those
/// pages is for debug only and no protection should be left in a library as
/// it might conflict with other User Defined protected area. To set up a
/// protected page use the #hal_DbgPageProtectSetup function. Once this is done,
/// the protection must be enabled to be active. You can then enable and
/// disable the protection without having to redo the setup.
///
///
/// @par Features
/// - Four memory spaces can be spied
/// - Access type spied can be read, write or both
/// - A memory space is described by a start address (inclusive) and an
/// end address (exclusive)
/// - For each space a status bit gives the master that triggered the spy, and
/// the address that caused it.
///
/// @{




// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// HAL_DBG_PAGE_NUM_T
// -----------------------------------------------------------------------------
/// Those are the 4 pages which can be protected
// =============================================================================
typedef enum
{
    HAL_DBG_PAGE_UD0=0,          ///< User defined, page 0
    HAL_DBG_PAGE_IRQ_STACKWATCH=1, ///< Reserved for IRQ stack overflow protection
    HAL_DBG_PAGE_CODEWATCH=2,    ///< Reserved for code protection
    HAL_DBG_PAGE_INT_CODEWATCH=3, ///< Reserved for internal SRAM code protection
    HAL_DBG_PAGE_4,
    HAL_DBG_PAGE_5,
    HAL_DBG_PAGE_6,
    HAL_DBG_PAGE_7,
    HAL_DBG_PAGE_8,
    HAL_DBG_PAGE_9,
    HAL_DBG_PAGE_10,
    HAL_DBG_PAGE_11,
    HAL_DBG_PAGE_12,
    HAL_DBG_PAGE_13,
    HAL_DBG_PAGE_14,
    HAL_DBG_PAGE_15,

    HAL_DBG_PAGE_QTY
} HAL_DBG_PAGE_NUM_T;

// =============================================================================
// HAL_DBG_PAGE_SPY_MODE_T
// -----------------------------------------------------------------------------
/// Type defining the possible Page Spy Mode
/// triggering an interrupt
// =============================================================================
typedef enum
{
    /// No interrupt is triggered
    HAL_DBG_PAGE_NO_TRIGGER=0,
    /// Triggered on reading of the protected page
    HAL_DBG_PAGE_READ_TRIGGER=1,
    /// Triggered on writing of the protected page
    HAL_DBG_PAGE_WRITE_TRIGGER=2,
    /// Triggered on reading or writing of the protected page
    HAL_DBG_PAGE_READWRITE_TRIGGER=3
} HAL_DBG_PAGE_SPY_MODE_T;



// =============================================================================
// hal_PageProtectSetup
// -----------------------------------------------------------------------------
/// This function setups a protection page
///
/// @param pageNum Name of the page we want to configure (#HAL_DBG_PAGE_UD0)
/// @param mode Protection mode
/// @param startAddr Address of the beginning of the page
/// @param endAddr  End address of the page. This address is not included in
/// the page
// =============================================================================
PUBLIC VOID hal_DbgPageProtectSetup(
    HAL_DBG_PAGE_NUM_T pageNum, HAL_DBG_PAGE_SPY_MODE_T mode,
    UINT32 startAddr, UINT32 endAddr);



// =============================================================================
// hal_DbgPageProtectEnable
// -----------------------------------------------------------------------------
/// Enable the protection of a given page.
///
/// In case of a user defined page, don't forget to configure it with
/// #hal_DbgPageProtectSetup before.
///
/// #HAL_DBG_PAGE_IRQ_STACKWATCH, #HAL_DBG_PAGE_CODEWATCH and
/// #HAL_DBG_PAGE_INT_CODEWATCH are already
/// enabled by default during the boot.
///
/// @param pageNum Page to enable
// =============================================================================
PUBLIC VOID hal_DbgPageProtectEnable(HAL_DBG_PAGE_NUM_T pageNum);



// =============================================================================
// hal_DbgPageProtectDisable
// -----------------------------------------------------------------------------
/// Disable the protection of a given page
/// You shouldn't want to disable the protection on
/// #HAL_DBG_PAGE_IRQ_STACKWATCH,
/// #HAL_DBG_PAGE_CODEWATCH and #HAL_DBG_PAGE_INT_CODEWATCH.
///
/// @param pageNum Page to disable
// =============================================================================
PUBLIC VOID hal_DbgPageProtectDisable(HAL_DBG_PAGE_NUM_T pageNum);

///  @} <- End of the page_protection group







/// @defgroup dbgAssert HAL Assert Driver
/// @{

// =============================================================================
// hal_DbgAssert
// -----------------------------------------------------------------------------
/// Function to call when 'building' an assert, if the condition is false.
/// @param fmt String format a-la printf, displayed in the trace.
// =============================================================================
PUBLIC VOID hal_DbgAssert(CONST CHAR *fmt, ...);

/// @}






/// @defgroup dbgTrace HAL Trace Driver
/// @{


// =============================================================================
// hal_DbgTrace
// -----------------------------------------------------------------------------
/// Function to call when 'building' a trace to print an a host computer
/// plugged to the mobile.
/// @param level Trace level.
/// @param tsmap Bitfield which defines which parameter is a string.
/// @param fmt String format a-la printf, displayed in the trace. 6 parameters
/// maximum are allowed !
// =============================================================================
PUBLIC VOID hal_DbgTrace(UINT32 level, UINT32 tsmap, CONST CHAR *fmt, ...);

/// @}
#endif // _HAL_DBG_H_







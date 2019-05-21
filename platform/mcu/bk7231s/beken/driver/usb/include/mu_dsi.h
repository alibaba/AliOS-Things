/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2006              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW USB Driver System Interface (UDSI).
 * $Revision: 1.12 $
 */

#ifndef __MUSB_SYSTEM_H__
#define __MUSB_SYSTEM_H__

#include "mu_tools.h"

/****************** SYSTEM INTERFACE CONSTANTS ********************/

/** Current controller struct version */
#define MUSB_CONTROLLER_VERSION		1

/** Current system utils struct version */
#define MUSB_SYSTEM_UTILS_VERSION	1

/** Current system services struct version */
#define MUSB_SYSTEM_SERVICES_VERSION	2

/** The controller type for the FDRC */
#define MUSB_CONTROLLER_FDRC		1

/** The controller type for the HDRC */
#define MUSB_CONTROLLER_HDRC		2

/** The controller type for the MHDRC */
#define MUSB_CONTROLLER_MHDRC		3

/** The controller type for the LSFC */
#define MUSB_CONTROLLER_LSFC		4

/** The controller type for the FSFC */
#define MUSB_CONTROLLER_FSFC		5

/** The controller type for the HSFC */
#define MUSB_CONTROLLER_HSFC		6

/******************** SYSTEM INTERFACE TYPES **********************/

/**
 * Controller ISR.
 * System glue calls this upon controller interrupt.
 *
 * @param 1 pIsrParam from MUSB_Controller
 *
 * @return -1: the interrupt did not seem to be ours (spurious or shared)
 * @return 0: interrupt processed; no need to wake IST
 * @return >0: number of IST work items queued; please wake IST
 */
typedef int (*MUSB_pfControllerIsr)(void *);

/**
 * Controller BSR (Background Service Routine).
 * When the ISR return value so indicates,
 * the system glue should call this function for non-interrupt-time processing.
 * This is typically done from a dedicated high-priority task/thread or
 * the main loop in a non-concurrent system.
 *
 * @param 1 pBsrParam from MUSB_Controller
 */
typedef uint32_t(*MUSB_pfControllerBsr)(void *);

/**
 * Timer expiration callback.
 * System glue calls this when a timer expires.
 * This must be done from a context in which it is safe to call the
 * lock/unlock services.
 *
 * @param 1 pPrivateData from MUSB_Controller
 * @see #MUSB_pfLock
 * @see #MUSB_pfUnlock
 */
typedef void (*MUSB_pfTimerExpired)(void *);

/**
 * MUSB_Controller.
 * A controller instance.
 * This is returned by a controller initialization function,
 * and provides everything necessary for the system glue
 * to support the controller's operation.
 *
 * @field wVersion the controller fills this with its interface version,
 * so the System can check for compatibility
 *
 * @field pPrivateData controller's private data;
 * not to be interpreted by the System
 *
 * @field wQueueLength the maximum number of items the controller requires
 * in the background message queue
 *
 * @field wQueueItemSize the size (in bytes) of each item
 * the controller will place in the background queue
 *
 * @field wTimerCount the number of timers required for the controller's operation;
 * the System should reserve and/or prepare this many timers for the controller's use
 *
 * @field wLockCount the number of locks required for the controller's operation;
 * the System should reserve and/or prepare this many locks for the controller's use
 *
 * @field pfIsr the controller's ISR;
 * the System must call this upon interrupt receipt
 *
 * @field pIsrParam the parameter the System must pass to the ISR
 *
 * @field pfBsr the controller's background processing code;
 * the System must call this if the ISR's return value so indicates
 *
 * @field pBsrParam the parameter the System must pass to the BSR
 */
typedef struct
{
    uint16_t wVersion;
    void *pPrivateData;
    uint16_t wQueueLength;
    uint16_t wQueueItemSize;
    uint16_t wTimerCount;
    const uint32_t *adwTimerResolutions;
    uint16_t wLockCount;
    MUSB_pfControllerIsr pfIsr;
    void *pIsrParam;
    MUSB_pfControllerBsr pfBsr;
    void *pBsrParam;
} MUSB_Controller;

/**
 * Buffer-overrun-safe strcat.
 * A controller calls this to concatenate a string to a message buffer.
 * The controller is responsible for initializing the buffer.
 * Most platforms can create a very simple implementation
 * based on strlen/strcat.
 *
 * @param 1 pointer to NUL-terminated message buffer
 * @param 2 size (in bytes) of message buffer
 * @param 3 NUL-terminated string to append
 *
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
typedef uint8_t (*MUSB_pfMessageString)(char *, uint16_t, const char *);

/**
 * Simplified snprintf.
 * A controller calls this to format a number and append
 * the result to a message buffer.
 * Most platforms can use a very simple implemenation
 * using sprintf, strlen and strncat.
 *
 * @param 1 pointer to diagnostic message buffer
 * @param 2 size (in bytes) of message buffer
 * @param 3 number to format
 * @param 4 base: 2, 8, 10, or 16 for binary, octal, decimal, or hex
 * @param 4 justification: if non-zero, pad with leading 0s as necessary
 * to output this length
 *
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
typedef uint8_t (*MUSB_pfMessageNumber)(char *, uint16_t, uint32_t,
                                        uint8_t, uint8_t);

/**
 * Get high-resolution time.
 * If overhead timing measurement is enabled,
 * a controller calls this to get the current absolute time
 * in some high-resolution unit like microseconds.
 * Naturally, it is expected that this counter will roll over frequently.
 * @return absolute time in whatever high-resolution unit is supported
 */
typedef uint32_t (*MUSB_pfGetTime)(void);

/**
 * MUSB_SystemUtils.
 * System utilities for a controller.
 *
 * This is provided by System-specific code to support a controller's
 * initialization and operation.
 *
 * @field wVersion the System fills this with its current interface version
 * so the controller can check for compatibility
 *
 * @field pfMessageString the function the controller should use to
 * append a string to a message buffer
 * (usually in preparation for printing diagnostics)
 *
 * @field pfMessageNumber the function the controller should use to
 * format and append a number to a message buffer
 * (usually in preparation for printing diagnostics)
 *
 * @field pfGetTime the function the controller should use to get
 * the current time in high-resolution units for timing statistics
 */
typedef struct
{
    uint16_t wVersion;
    MUSB_pfMessageString pfMessageString;
    MUSB_pfMessageNumber pfMessageNumber;
    MUSB_pfGetTime pfGetTime;
} MUSB_SystemUtils;

/**
 * Get address for DMA.
 * A controller calls this to get a bus address (for DMA)
 * from a system address.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 system address
 *
 * @return bus address on success; NULL on failure
 */
typedef void *(*MUSB_pfSystemToBusAddress)(void *, const void *);

/**
 * Enqueue background task.
 * A controller calls this to enqueue a background item.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 pointer to an item of wQueueItemSize bytes;
 * data should be copied into the queue
 *
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfQueueBackgroundItem)(void *, const void *);

/**
 * Dequeue background task.
 * A controller calls this to dequeue a background item.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 pointer to storage for an item of wQueueItemSize bytes;
 * the implementation should copy data here
 *
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfDequeueBackgroundItem)(void *, void *);

/**
 * Flush background queue.
 * A controller calls this to flush the background item queue
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 */
typedef void (*MUSB_pfFlushBackgroundQueue)(void *);

/**
 * Arm a timer.
 * A controller calls this to arm a timer (periodic or one-shot).
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 index of timer (counting from 0)
 * @param 3 time (milliseconds)
 * @param 4 TRUE for periodic; FALSE for one-shot
 * @param 5 function to call on expiration
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfArmTimer)(void *, uint16_t, uint32_t, uint8_t,
                                   MUSB_pfTimerExpired);

/**
 * Disarm a timer.
 * A controller calls this to cancel a timer.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 index of timer (counting from 0)
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfCancelTimer)(void *, uint16_t);

/**
 * Lock.
 * A controller calls this to enter a lock.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 lock index (counting from 0)
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfLock)(void *, uint16_t);

/**
 * Unlock.
 * A controller calls this to exit a lock.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 lock index (counting from 0)
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfUnlock)(void *, uint16_t);

/**
 * Print diagnostic.
 * A controller calls this to print a diagnostic message.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 NUL-terminated (C-style) string
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfPrintDiag)(void *, const char *);

/**
 * [OPTIONAL] A new power load is attached to the controller.
 * A controller calls upon discovering a device's
 * configuration's power requirement.
 * This gives the system the opportunity to veto,
 * in case a special power management policy is implemented
 * (e.g. "no device other than top hub may draw more than half available power"
 * or "no bus-powered hubs allowed").
 * It also allows the system to take action to make more power available
 * if possible when power reserves are low.
 * NOTE: this function should not take a long time
 * (i.e. no user interaction) because the stack is blocked during the call.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 additional power load, in 2 mA units
 * @param 3 TRUE if the controller wishes to add the load;
 * FALSE if the load cannot be added due to power requirements
 * (in the latter case this call is informational and its
 * return value is not relevant)
 * @param 4 device port information - an array of port numbers
 * (0 marks the array's end)
 * @param 5 device's (or first interface's) class code
 * @return TRUE on success
 * @return FALSE on failure (causes the device to be rejected)
 */
typedef uint8_t (*MUSB_pfNewPowerLoad)(void *, uint16_t, uint8_t,
                                       const uint8_t *, uint8_t);

/**
 * [OPTIONAL] Remove a power load from the controller.
 * A controller calls this after it has
 * removed a power from to the system.
 * NOTE: this function should not take a long time
 * (i.e. no user interaction) because the stack is blocked during the call.
 *
 * @param 1 pPrivateData from MUSB_SystemServices
 * @param 2 power load removed, in 2 mA units
 * @param 3 TRUE if this is a normal removal;
 * FALSE if this was forced to avoid power delivery failure
 * @param 4 device port information - an array of port numbers
 * (0 marks the array's end)
 * @param 5 device's (or first interface's) class code
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MUSB_pfRemovePowerLoad)(void *, uint16_t, uint8_t,
        const uint8_t *, uint8_t);

/**
 * MUSB_SystemServices.
 * Services for a controller.
 *
 * This is provided by System-specific code to support a controller's operation.
 *
 * @field wVersion the System fills this with its current interface version
 * so the controller can check for compatibility
 *
 * @field pPrivateData System data; not to be interpreted by controller
 *
 * @field pfSystemToBusAddress function to convert system to bus address for DMA
 *
 * @field pfQueueBackgroundItem function to add an item to the background queue
 *
 * @field pfDequeueBackgroundItem function to remove an item
 * from the background queue
 *
 * @field pfFlushBackgroundQueue function to flush the background queue
 *
 * @field pfArmTimer function to arm a timer
 *
 * @field pfCancelTimer function to cancel a previsouly-armed timer
 *
 * @field pfLock function to lock (obtain a mutex)
 *
 * @field pfUnlock function to unlock (release a mutex)
 *
 * @field pfPrintDiag function to print a diagnostic message
 *
 * @field pfNewPowerLoad function to add a power load to the port
 *
 * @field pfRemovePowerLoad function to remove a power load from the port
 */
typedef struct
{
    uint16_t wVersion;
    void *pPrivateData;
    MUSB_pfSystemToBusAddress pfSystemToBusAddress;
    MUSB_pfQueueBackgroundItem pfQueueBackgroundItem;
    MUSB_pfDequeueBackgroundItem pfDequeueBackgroundItem;
    MUSB_pfFlushBackgroundQueue pfFlushBackgroundQueue;
    MUSB_pfArmTimer pfArmTimer;
    MUSB_pfCancelTimer pfCancelTimer;
    MUSB_pfLock pfLock;
    MUSB_pfUnlock pfUnlock;
    MUSB_pfPrintDiag pfPrintDiag;
    MUSB_pfNewPowerLoad pfNewPowerLoad;
    MUSB_pfRemovePowerLoad pfRemovePowerLoad;
} MUSB_SystemServices;

/****************** SYSTEM INTERFACE FUNCTIONS ********************/

/**
 * REQUIRED Initialization.
 * Initialize the USB system.
 * This is typically called by startup code, like the application
 * on a single-application system.
 * @param dwBsrPriority the priority to use for the UCD's BSR(s).
 * Run-time configuration is necessary to allow applications
 * built on binary distributions to choose a workable priority scheme.
 * The interpretation and allowed range is necessarily system-specific,
 * so please consult the system-specific documentation for this information.
 * @return TRUE on success
 * @return FALSE on failure (out of memory; system-specific reasons)
 */
extern uint8_t MUSB_InitSystem(unsigned long dwBsrPriority);

/**
 * Shut down the USB system.
 * @return TRUE on success
 * @return FALSE on failure (system-specific reasons)
 */
//extern uint8_t MUSB_DestroySystem(void);

/**
 * Create a controller.
 * Instantiate a controller (perhaps from a pool defined at build time).
 * If MUSB_SetControllerHostPower is not called for this controller,
 * a 100 mA host-mode power delivery capability will be assumed.
 *
 * @param pUtils pointer to utilities provided by the System glue
 * @param wControllerType the controller type (0 to discover)
 * @param pControllerAddressIsr the address of the controller's registers
 * as seen by the ISR
 * @param pControllerAddressBsr the address of the controller's registers
 * as seen by the BSR
 *
 * @return a non-NULL instance on success
 */
extern MUSB_Controller *MUSB_NewController(
    MUSB_SystemUtils *pUtils,
    uint16_t wControllerType,
    void *pControllerAddressIsr,
    void *pControllerAddressBsr
);

/**
 * [OPTIONAL] Set maximum host-mode power information.
 * The system glue calls this to set a controller's
 * maximum power delivery capability in host mode.
 * This may be called whenever the power capability
 * changes (e.g. if the host is battery-powered).
 * For battery-powered hosts, the system glue should use
 * the value projected in the near future rather than
 * the actual present value.
 * This gives the stack time to react, e.g. by
 * suspending and/or unconfiguring one or more devices.
 *
 * @param 1 pController an instance returned by one of the init functions
 * @param 2 wPower the maximum power the controller can provide in host mode,
 * in 2 mA units
 *
 * @return status code
 */
extern uint32_t MUSB_SetControllerHostPower(MUSB_Controller *pController,
        uint16_t wPower);

/**
 * Start (or restart) a controller.
 * The system glue must not call this until it is ready
 * to accept calls to its services
 *
 * @param pController an instance returned by one of the init functions
 * @param pSystemServices the services provided by the System glue
 *
 * @return status code
 */
extern uint32_t MUSB_StartController(MUSB_Controller *pController,
                                     MUSB_SystemServices *pSystemServices
                                    );

/**
 * Stop a controller.
 * Stop a controller (useful to prepare for warm reset or power-saving mode).
 *
 * @param pController an instance returned by one of the init functions
 *
 * @return status code
 */
//extern uint32_t MUSB_StopController(MUSB_Controller* pController);

/**
 * Destroy a controller.
 * Stop a controller and free any resources associated with it
 * (useful for system shutdown or plug-and-play device removal).
 *
 * @param pController an instance returned by one of the init functions
 *
 * @return status code
 */
//extern uint32_t MUSB_DestroyController(MUSB_Controller* pController);

/**
 * Read a character from the console.
 * Read an ASCII/ISO-Latin-1 character from the console;
 * blocking until one is available.
 * The console may be implemented in many different ways,
 * including scripting, so no assumption of true interactivity should be made.
 * @return the character
 */
extern char MUSB_ReadConsole(void);

/**
 * Write a character to the console.
 * Write an ASCII/ISO-Latin-1 character to the console.
 * @param bChar the character
 */
extern void MUSB_WriteConsole(const char bChar);

#endif	/* multiple inclusion protection */

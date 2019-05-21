/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_NONE_SYSTEM_H__
#define __MUSB_NONE_SYSTEM_H__


/**
 * Introduction to non-OS UDSI implementation.
 * The non-OS UDSI implementation must delegate more to
 * the target-specific code.
 * This is because each non-OS target can be vastly different.
 * For example, it is likely that each non-OS target has a unique
 * memory-mapped I/O mechanism, memory protection strategy (if any),
 * ISR installation mechanism, shared interrupt mechanism (if any),
 * required implementation of timers, and console I/O mechanism (if any).
 * Target-specific code needs to implement:
 * <dl>
 * <dt>MUSB_ReadConsole
 * <dd>Blocking read of a console character
 * <dt>MUSB_WriteConsole
 * <dd>Blocking write of a console character
 * <dt>MUSB_BoardMessageString
 * <dd>Only used if diagnostic messages enabled
 * <dt>MUSB_BoardMessageNumber
 * <dd>Only used if diagnostic messages enabled
 * <dt>MUSB_BoardGetTime
 * <dd>Only used if overhead statistics are enabled
 * <dt>MUSB_BoardInitController
 * <dd>Called by the non-OS implementation of MUSB_InitSystem
 * <dt>MUSB_BoardInitTimers
 * <dd>Called by the non-OS implementation of MUSB_InitSystem
 * <dt>MUSB_BoardDestroyController
 * <dd>Called by the non-OS implementation of MUSB_DestroySystem
 * <dt>MUSB_BoardRunBackground
 * <dd>Board-specific background processing
 * <dt>MUSB_BoardArmTimer
 * <dd>Arm a timer
 * <dt>MUSB_BoardCancelTimer
 * <dd>Cancel a timer
 * <dt>MUSB_BoardPrintDiag
 * <dd>Only used if diagnostic messages enabled
 * <dt>MUSB_BoardSystemToBusAddress
 * <dd>Translate CPU address to bus address (for programming a DMA controller)
 * </dl>
 */

#include "mu_dsi.h"

/***************************** TYPES ******************************/

/**
 * Board-specific information about a controller
 * @field wType one of the MUSB_CONTROLLER_* constants
 * @field pBase base address for hard-wired controller; vendor ID for PCI
 * @field dwInfo IRQ for hard-wired controller; device ID for PCI
 * @field bIsPci TRUE if PCI
 */
typedef struct
{
    uint16_t wType;
    void *pBase;
    uint32_t dwInfo;
    uint8_t bIsPci;
} MUSB_NoneController;

/**
 * Non-OS ISR
 * @param pPrivateData non-OS UDS instance data
 * @return TRUE if interrupt was from controller
 * @return FALSE if interrupt was not from controller
 */
typedef uint8_t (*MUSB_NoneIsr)(void *pPrivateData);

extern MUSB_NoneController MUSB_aNoneController[];

/**
 * Target-specific code should call this from its main loop.
 * This performs the UCD's background processing, if any.
 */
extern uint32_t MUSB_NoneRunBackground(void);

/**
 * Buffer-overrun-safe strcat.
 * A controller calls this to concatenate a string to a message buffer.
 * The controller is responsible for initializing the buffer.
 * Most platforms can create a very simple implementation
 * based on strlen/strcat.
 *
 * @param pBuffer pointer to NUL-terminated message buffer
 * @param wBufferSize size (in bytes) of message buffer
 * @param pString NUL-terminated string to append
 *
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
extern uint8_t MUSB_BoardMessageString(char *pBuffer, uint16_t wBufferSize,
                                       const char *pString);

/**
 * Simplified snprintf.
 * A controller calls this to format a number and append
 * the result to a message buffer.
 * Most platforms can use a very simple implemenation
 * using sprintf, strlen and strncat.
 *
 * @param pBuffer pointer to diagnostic message buffer
 * @param wBufferSize size (in bytes) of message buffer
 * @param dwNumber number to format
 * @param bBase base: 2, 8, 10, or 16 for binary, octal, decimal, or hex
 * @param bJustification justification: if non-zero,
 * pad with leading 0s as necessary to output this length
 *
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
extern uint8_t MUSB_BoardMessageNumber(char *pBuffer, uint16_t wBufferSize,
                                       uint32_t dwNumber, uint8_t bBase, uint8_t bJustification);

/*
 * Board-specific get high-resolution time
 * @return current time in target-specific high-resolution units
 */
extern uint32_t MUSB_BoardGetTime(void);

/**
 * Target-specific per-controller initialization.
 * @param pPrivateData non-OS UDS instance data
 * @param pfIsr ISR to call (pass pPrivateData)
 * @param pControllerInfo controller information
 * @param ppBaseIsr pointer to controller base address required for
 * use by ISR; updated by target-specific code (PCI only)
 * @param ppBaseBsr pointer to controller base address required for
 * use by BSR; updated by target-specific code (PCI only)
 * @return non-NULL pointer to private data on success
 * @return NULL on failure
 */
extern void *MUSB_BoardInitController(void *pPrivateData, MUSB_NoneIsr pfIsr,
                                      const MUSB_NoneController *pControllerInfo,
                                      uint8_t **ppBaseIsr, uint8_t **ppBaseBsr);

/**
 * Target-specific per-controller timer initialization.
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * @param wTimerCount the number of timers required
 * @param adwTimerResolutions an array of timer resolutions
 * (in milliseconds); one element for each timer
 * @return TRUE on success
 * @return NULL on failure
 */
extern uint8_t MUSB_BoardInitTimers(void *pPrivateData, uint16_t wTimerCount,
                                    const uint32_t *adwTimerResolutions);

/**
 * Board-specific controller destruction
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * on the corresponding controller
 */
extern void MUSB_BoardDestroyController(void *pPrivateData);

/**
extern void MUSB_BoardRunBackground(void* pPrivateData);
 * Board-specific timer arm
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * on the corresponding controller
 * @param wIndex index of the timer to arm
 * @param dwTime timeout (in milliseconds)
 * @param bPeriodic TRUE for periodic; FALSE for one-shot
 * @param pfExpireCallback function to call on expiration
 * @param pParam expiration callback parameter
 * @return TRUE on success
 * @return FALSE on failure
 */
extern uint8_t MUSB_BoardArmTimer(void *pPrivateData, uint16_t wIndex,
                                  uint32_t dwTime, uint8_t bPeriodic,
                                  MUSB_pfTimerExpired pfExpireCallback,
                                  void *pParam);

/**
 * Board-specific timer disarm
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * on the corresponding controller
 * @param wIndex index of the timer to cancel
 */
extern uint8_t MUSB_BoardCancelTimer(void *pPrivateData, uint16_t wIndex);

/**
 * Print diagnostic.
 * A controller calls this to print a diagnostic message.
 *
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * on the corresponding controller
 * @param pMessage NUL-terminated (C-style) string
 * @return TRUE on success
 * @return FALSE on failure
 */
extern uint8_t MUSB_BoardPrintDiag(void *pPrivateData, const char *pMessage);

/**
 * Board- and bus- specific address translation
 * @param pPrivateData result of a successful call to MUSB_BoardInitController
 * on the corresponding controller
 * @param pSystemAddress address in CPU space
 * @return address as seen by a master on the bus where the controller resides
 */
extern void *MUSB_BoardSystemToBusAddress(void *pPrivateData,
        const void *pSystemAddress);

#endif	/* multiple inclusion protection */

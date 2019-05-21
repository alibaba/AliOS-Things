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

/*
 * USB Core Behavioral Simulator (CBS) interface.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_CBS_H__
#define __MUSB_CBS_H__

#include "mu_tools.h"

/**
 * Introduction.
 * Introduction to the Core Behavioral Simulator (CBS).
 * The CBS simulates the behavior of one of the USB cores,
 * from the viewpoint of the register values and FIFO data seen by software.
 * A client of the CBS is responsible for:
 * <ul>
 * <li>Creating an "event" (semaphore) for the CBS to use indirectly via the calls below
 * <li>Creating and starting a thread to run the CBS entrypoint after successful initialization
 * <li>Exposing a function to set the simulation timescale
 * <li>Exposing a function to simulate an interrupt from the core
 * <li>Exposing a function to set the event mentioned above
 * <li>Exposing a function to wait for the event to be set
 * </ul>
 */

/*************************** CBS TYPES ****************************/

/**
 * Set the scaling factor for timers
 * @param pPrivateData from the client struct
 * @param wScale 1 => no scaling; 2 => double intervals, etc.
 */
typedef void (*MGC_pfCbsSetTimeScale)(void *pPrivateData, uint16_t wScale);

/**
 * Simulate an interrupt
 * @param pPrivateData from the client struct
 */
typedef void (*MGC_pfCbsSimulateInterrupt)(void *pPrivateData);

/**
 * Signal an event
 * @param pPrivateData from the client struct
 */
typedef void (*MGC_pfCbsSetEvent)(void *pPrivateData);

/**
 * Wait for an event to be signalled
 * @param pPrivateData from the client struct
 */
typedef uint8_t (*MGC_pfCbsWaitEvent)(void *pPrivateData);

/**
 * MGC_CbsClient.
 * A client of the CBS.
 * @field pPrivateData client-specific data; not to be interpreted by the CBS
 * @field pfCbsSetTimeScale client-provided function to set the simulation timescale
 * @field pfCbsSimulateInterrupt client-provided function to simulate an interrupt
 * from the core
 * @field pfCbsSetEvent client-provided function to set an event
 * @field pfCbsWaitEvent client-provided function to wait for an event
 */
typedef struct
{
    void *pPrivateData;
    MGC_pfCbsSetTimeScale pfCbsSetTimeScale;
    MGC_pfCbsSimulateInterrupt pfCbsSimulateInterrupt;
    MGC_pfCbsSetEvent pfCbsSetEvent;
    MGC_pfCbsWaitEvent pfCbsWaitEvent;
} MGC_CbsClient;

/**
 * Entrypoint for the CBS thread
 * @param pPrivateData pPrivateData from the CBS instance data struct
 */
typedef void (*MGC_pfCbsThreadEntry)(void *pPrivateData);

/**
 * MGC_Cbs.
 * CBS instance data.
 * @field pPrivateData CBS data; not to be interpreted by the client
 * @field wControllerType controller type being simulated
 * @field pfCbsThreadEntry CBS thread entrypoint;
 * client must create and start a thread to run this after successful initialization
 */
typedef struct
{
    void *pPrivateData;
    uint16_t wControllerType;
    MGC_pfCbsThreadEntry pfCbsThreadEntry;
} MGC_Cbs;

/************************* CBS FUNCTIONS **************************/

/**
 * Initialize the CBS
 * @param pClient client pointer
 * @param pConfigFilePath path to config file
 * @return non-NULL CBS on success
 * @return NULL on failure
 */
extern MGC_Cbs *MGC_CbsInit(MGC_CbsClient *pClient, const char *pConfigFileName);

extern uint8_t MGC_CbsRead8(void *pBase, uint32_t dwOffset);
extern uint16_t MGC_CbsRead16(void *pBase, uint32_t dwOffset);
extern uint32_t MGC_CbsRead32(void *pBase, uint32_t dwOffset);
extern void MGC_CbsWrite8(void *pBase, uint32_t dwOffset, uint8_t bData);
extern void MGC_CbsWrite16(void *pBase, uint32_t dwOffset, uint16_t wData);
extern void MGC_CbsWrite32(void *pBase, uint32_t dwOffset, uint32_t dwData);

#endif	/* multiple inclusion protection */

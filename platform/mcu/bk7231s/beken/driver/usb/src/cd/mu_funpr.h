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

#ifndef __MUSB_FUNCTION_PROTO_H__
#define __MUSB_FUNCTION_PROTO_H__

/*
 * Prototypes for function-specific functions
 * $Revision: 1.3 $
 */

#include "mu_impl.h"

/**
 * Register a function client.
 * Register the given function client.
 * @param pPort port pointer
 * @param pClient client pointer
 * @return status code
 */
extern uint32_t MGC_FunctionRegisterClient(MGC_Port *pPort,
        MUSB_FunctionClient *pClient);

/**
 * Function cleanup.
 * Perform function-specific cleanup.
 * @param pPort port pointer
 * @return status code
 */
extern uint32_t MGC_FunctionDestroy(MGC_Port *pPort);

/**
 * Perform function-specific operations when the connection speed is known
 * @param pPort port pointer (speed should be set before call)
 */
extern void MGC_FunctionSpeedSet(MGC_Port *pPort);

/**
 * Parse/handle setup as function.
 * Parse a setup and possibly handle it based on descriptors
 * @param pPort port pointer (request and response buffers here)
 * @param pbStatus filled with status (e.g. MUSB_STATUS_STALL to stall request)
 * @return TRUE if handled
 * @return FALSE to call client
 */
extern uint8_t MGC_FunctionParseSetup(MGC_Port *pPort, uint8_t *pbStatus);

/**
 * Change USB state.
 * Change to a new USB state; invoking client callback.
 * @param pPort port pointer
 * @param State the new state
 */
extern void MGC_FunctionChangeState(MGC_Port *pPort, MUSB_State State);

extern int MGC_FcIsr(MGC_Controller *pController, uint8_t bIntrUsbValue,
                     uint16_t wIntrTxValue, uint16_t wIntrRxValue);
extern uint32_t MGC_FcBsr(void *pParam);
extern uint32_t MGC_FcResetPort(MGC_Port *pPort);
extern MGC_EndpointResource *MGC_FcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx);
/**
 * Prepare a function pipe.
 * Prepare a pipe for the given local endpoint.
 * @param pPort port pointer
 * @param pPipe pointer to pipe in which to fill fields
 * @param hBus bus handle for pipe
 * @param pResource local resource to assume for use
 * @param pEndpoint pointer to USB endpoint descriptor
 */
extern void MGC_FunctionPreparePipe(MGC_Port *pPort, MGC_Pipe *pPipe,
                                    MUSB_BusHandle hBus,
                                    MGC_EndpointResource *pResource,
                                    const MUSB_EndpointDescriptor *pEndpoint);

#endif	/* multiple inclusion protection */

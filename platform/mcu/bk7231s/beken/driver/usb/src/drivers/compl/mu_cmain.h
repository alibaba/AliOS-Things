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

/*****************************************************************************
 * FILE NAME    : mu_cmain.h
 * DATE         :
 * DESCRIPTION  : It Contains Function prototypes of main functions of
 *                compliance device driver.
 ****************************************************************************/

#ifndef __MUSB_COMPLIANCE_MAIN_H__
#define __MUSB_COMPLIANCE_MAIN_H__


#define MGC_COMP_DIAG_MSG_SIZE      256

#define MGC_COMP_DIAG_NUM_SIZE       33


/* Macro to prepare setup packet for compliance device */
#define MGC_COMPL_PREPARE_SETUP_PACKET(pSetup,\
                                       bmActualRequestType,\
                                       bActualRequest,\
                                       wActualValue,\
                                       wActualIndex,\
                                       wActualLength)\
{\
    (pSetup)->bmRequestType = (uint8_t) bmActualRequestType;\
    (pSetup)->bRequest      = (uint8_t) bActualRequest;\
    (pSetup)->wValue        = (uint16_t) MUSB_SWAP16(wActualValue);\
    (pSetup)->wIndex        = (uint16_t) MUSB_SWAP16(wActualIndex);\
    (pSetup)->wLength       = (uint16_t) MUSB_SWAP16(wActualLength);\
}

/* Macro to fill control Irp for compliance device */
#define MGC_COMPL_FILL_CONTROL_IRP(pComplDeviceInfo,\
                                   pControlIrp,\
                                   pActualOutBuffer,\
                                   dwRequestedOutLength,\
                                   pActualInBuffer,\
                                   dwRequestedInLength,\
                                   pfControlIrpComplete)\
{\
    (pControlIrp)->pDevice           = pComplDeviceInfo->pUsbDevice;\
    (pControlIrp)->pOutBuffer        = pActualOutBuffer;\
    (pControlIrp)->dwOutLength       = dwRequestedOutLength;\
    (pControlIrp)->pInBuffer         = pActualInBuffer;\
    (pControlIrp)->dwInLength        = dwRequestedInLength;\
    (pControlIrp)->dwStatus          = 0;\
    (pControlIrp)->dwActualOutLength = 0;\
    (pControlIrp)->dwActualInLength  = 0;\
    (pControlIrp)->pfIrpComplete     = pfControlIrpComplete;\
    (pControlIrp)->pCompleteParam    = (void *) pComplDeviceInfo;\
}

/**
 * State Handler for compliance device driver.
 * This function is called by various compliance driver functions, to start next actions,
 * This function decides driver's next action depending on the current state.
 * when set of actions for current state are done.
 * @param pComplDeviceInfo Pointer to top level device driver context.
 * @return None.
 */
extern void MGC_ComplStateHandler(MUSB_ComplDeviceInfo *pComplDeviceInfo);

/**
 * Top level Control Callback function to call state handler.
 * This function is registered with different control request when a function wants to call state
 * handler after completing the current control request.
 * This function resets the in and out data buffer length from control IRP and calls state handler.
 * @param pContext Pointer to top level device driver context.
 * @param pControlIrp Pointer to control Irp for which this callback is executed.
 * @return None.
 */
extern void MGC_ComplMainCallback(void *pContext, MUSB_ControlIrp *pControlIrp);



/**
 * Transfer Test Handler.
 * This function is called by state handler after configuring the device or, after receiving the
 * bus resume callback from the device.
 * This function decides the endpoint index and type of transfer test to be executed on this endpoint.
 * @param pComplDeviceInfo Pointer to Top Level Device Driver Context.
 * @return MGC_COMPL_STATUS_XFER_COMPL if all testcases are executed for all endpoints exported by device.
 * @return MUSB_SUCCESS otherwise.
 */
extern uint32_t MGC_ComplRunTransferTest(MUSB_ComplDeviceInfo *pComplDeviceInfo);

/**
 * Disconnect Test Handler.
 * This function is called by state handler after transfer tests are completed and the connected
 * device supports VEN_Do_Disconnect Control Request.
 * This function prepares the setup packet with Disocnnect Request and submits the control Irp
 * with this request.
 * @param pComplDeviceInfo Pointer to Top Level Device Driver Context.
 * @return None.
 */
extern void MGC_ComplRunDisconnectTest(MUSB_ComplDeviceInfo *pComplDeviceInfo);

/**
 * Remote Wakeup Test Handler.
 * This function is called by state handler after transfer(if disconnect test is not supported) tests or,
 * disconnect tests are over.
 * This function prepares the setup packet with Remote Wakeup Request and submits the control Irp
 * with this request.
 * @param pComplDeviceInfo Pointer to Top Level Device Driver Context.
 * @return None.
 */
extern void MGC_ComplRunRemoteWakeupTest(MUSB_ComplDeviceInfo *pComplDeviceInfo);

/**
 * Displays contents of endpoint capability table.
 * This function is called by various driver function to display endpoint capability table.
 * This function displays capability of each of the endpoints mentioned in endpoint capability
 * table.
 */
extern void
MGC_ComplDisplayEpCapabilityTable(MUSB_ComplCapabilityTable *pEpCapabilityTable);


#endif

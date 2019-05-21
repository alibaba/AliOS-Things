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
 * Function-specific functionality (response to control requests
 * on the default endpoint).
 * $Revision: 1.35 $
 */
#include "include.h"

#include "mu_impl.h"
#include "mu_mem.h"

#include "mu_descs.h"
#include "mu_funpr.h"

#if CFG_USB
/**
 * Fill a pipe struct as per the given
 */
void MGC_FunctionPreparePipe(MGC_Port *pPort, MGC_Pipe *pPipe,
                             MUSB_BusHandle hBus,
                             MGC_EndpointResource *pResource,
                             const MUSB_EndpointDescriptor *pEndpoint)
{
    uint8_t bTrafficType = pEndpoint->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK;
    pPipe->hSession = hBus;
    pPipe->pPort = pPort;
    pPipe->pLocalEnd = pResource;
    if(pEndpoint->bEndpointAddress & MUSB_DIR_IN)
    {
        pPipe->bmFlags = MGC_PIPEFLAGS_TRANSMIT;
        pResource->pTxIrp = NULL;
        pResource->dwTxOffset = 0;
    }
    else
    {
        pPipe->bmFlags = 0;
        pResource->pRxIrp = NULL;
        pResource->dwRxOffset = 0;
    }
    pPipe->pDevice = NULL;
    pPipe->bTrafficType = bTrafficType;
    pPipe->wMaxPacketSize = MUSB_SWAP16P((uint8_t *) & (pEndpoint->wMaxPacketSize));
    pResource->dwWaitFrameCount = 0;
}

/**
 * Set a new interface alternate.
 * Update pPort->bMaxFunctionEnds if necessary.
 * @param pPort port pointer
 * @param bIfaceIndex interface index
 * @param bAltSetting new alternate value
 * @return status code
 */
static uint32_t MGC_FunctionSetInterface(MGC_Port *pPort, uint8_t bIfaceIndex,
        uint8_t bAltSetting)
{
    uint8_t bOk = FALSE;
    uint8_t bEndIndex;
    MUSB_DeviceEndpoint Endpoint;
    MGC_EndpointResource *pResource;
    const MUSB_InterfaceDescriptor *pIfaceDesc;
    const MUSB_EndpointDescriptor *pEndDesc;
    uint32_t dwStatus = MUSB_STATUS_OK;

    Endpoint.wNakLimit = 0xffff;

    pIfaceDesc = MUSB_FindInterfaceDescriptor(pPort->pCurrentConfig,
                 bIfaceIndex, bAltSetting);
    if(pIfaceDesc)
    {

        /* clear & re-bind each end */
        for(bEndIndex = 0; bEndIndex < pIfaceDesc->bNumEndpoints; bEndIndex++)
        {
            pEndDesc = MUSB_FindEndpointDescriptor(pPort->pCurrentConfig,
                                                   pIfaceDesc, bEndIndex);
            if(pEndDesc)
            {
                pResource = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds),
                            pEndDesc->bEndpointAddress & MUSB_ENDPOINT_NUMBER_MASK);
                if(pResource)
                {
                    /* clear state from last use */
#ifdef MUSB_C_DYNFIFO_DEF
                    pResource->wMaxPacketSizeTx = pResource->wMaxPacketSizeRx = 0;
#endif
                    pResource->bIsClaimed = pResource->bRxClaimed = FALSE;

                    /* bind */
                    MUSB_MemCopy((void *) & (Endpoint.UsbDescriptor),
                                 (void *)pEndDesc, sizeof(MUSB_EndpointDescriptor));
                    pResource = NULL;
                    if (pPort->pfBindEndpoint)
                    {
                        pResource = pPort->pfBindEndpoint(pPort, &Endpoint, NULL, TRUE);
                    }

                    /* break on failure */
                    if(!pResource)
                    {
                        dwStatus = MUSB_STATUS_NO_RESOURCES;
                        break;
                    }
                    /* prepare pipe */
                    MGC_FunctionPreparePipe(pPort,
                                            pPort->apPipe[bEndIndex + pPort->abIfaceOffsets[pIfaceDesc->bInterfaceNumber]],
                                            (MUSB_BusHandle)pPort, pResource, pEndDesc);
                }
            }
        }
    }
    else
    {
        dwStatus = MUSB_STATUS_BAD_DESCRIPTORS;
    }

    if(MUSB_STATUS_OK == dwStatus)
    {
        if (pPort->pFunctionClient->pfInterfaceSet)
        {
            bOk = pPort->pFunctionClient->pfInterfaceSet(
                      pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
                      bIfaceIndex, bAltSetting, (MUSB_PipePtr *)pPort->apPipe);
            dwStatus = bOk ? MUSB_STATUS_OK : MUSB_STATUS_STALLED;
        }
    }

    return dwStatus;
}

/**
 * Find/bind a configuration.
 * Update pPort->bMaxFunctionEnds if necessary.
 * @param pPort port pointer
 * @param pConfigDesc configuration descriptor
 * @param bBind TRUE to actually bind; FALSE to just check possibility
 * @return status code
 */
static uint32_t MGC_FunctionBindConfig(MGC_Port *pPort,
                                       const MUSB_ConfigurationDescriptor *pConfigDesc,
                                       uint8_t bBind)
{
    uint8_t bLastIface, bIfaceIndex, bEndIndex;
    uint8_t bReallyBind, bMaxIfaceEnds;
    uint16_t wLength, wLimit;
    MUSB_Device Device;
    MUSB_DeviceEndpoint Endpoint;
    MGC_EndpointResource *pResource;
    const uint8_t *pDesc;
    const MUSB_InterfaceDescriptor *pIfaceDesc;
    const MUSB_EndpointDescriptor *pEndDesc;
    uint8_t bEndCount;
    uint8_t bMaxFunctionEnds = 0;
    uint32_t dwStatus = MUSB_STATUS_OK;

    MUSB_MemSet(&Device, 0, sizeof(Device));
    Device.pPort = pPort->pInterfacePort;
    Endpoint.pDevice = &Device;
    Endpoint.wNakLimit = 0xffff;

    /* check overflow condition for abIfaceOffsets[] */
    if ( pConfigDesc->bNumInterfaces >= MGC_MAX_CONFIG_IFACES )
    {
        return MUSB_STATUS_BAD_DESCRIPTORS;
    }

    /* clear all current endpoints */
    bEndCount = (uint8_t)MUSB_ArrayLength(&(pPort->LocalEnds));
    for(bEndIndex = 1; bEndIndex < bEndCount; bEndIndex++)
    {
        pResource = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEndIndex);
        if(pResource)
        {
#ifdef MUSB_C_DYNFIFO_DEF
            pResource->wMaxPacketSizeTx = pResource->wMaxPacketSizeRx = 0;
#endif
            pResource->bIsClaimed = pResource->bRxClaimed = FALSE;
        }
    }

    /* search for interfaces/endpoints, staying within config length */
    wLength = MUSB_SWAP16P((uint8_t *) & (pConfigDesc->wTotalLength));
    bLastIface = 0;
    bMaxIfaceEnds = 0;
    for(bIfaceIndex = 0; !dwStatus; bIfaceIndex++)
    {
        /* find interface */
        pDesc = MGC_FindDescriptor((uint8_t *)pConfigDesc,
                                   wLength, MUSB_DT_INTERFACE, bIfaceIndex);
        /* break when no more */
        if(!pDesc)
        {
            /* update max endpoints for this config */
            bMaxFunctionEnds += bMaxIfaceEnds;
            /* fail if we didn't find all interfaces stated in the config */
            if(bLastIface < (pConfigDesc->bNumInterfaces - 1))
            {
                dwStatus = MUSB_STATUS_BAD_DESCRIPTORS;
            }
            break;
        }
        /* interface found */
        pIfaceDesc = (MUSB_InterfaceDescriptor *)pDesc;
        bLastIface = pIfaceDesc->bInterfaceNumber;

        /*
         * USB 2.0 section 9.2.3 says alternates may re-define NUMBER and types
         * of endpoints.  We use alternates only for counting and verifying
         * endpoints, because MGC_FunctionSetInterface() performs the actual
         * binding of alternates.
         */
        if(pIfaceDesc->bAlternateSetting)
        {
            bReallyBind = FALSE;
            if(pIfaceDesc->bNumEndpoints > bMaxIfaceEnds)
            {
                /* update max ends for this iface */
                bMaxIfaceEnds = pIfaceDesc->bNumEndpoints;
            }
            if ( bBind ) continue;
        }
        else
        {
            bReallyBind = bBind;
            /* update max ends for this config */
            bMaxFunctionEnds += bMaxIfaceEnds;
            if(pIfaceDesc->bInterfaceNumber > 0)
            {
                /* set offset for this iface */
                pPort->abIfaceOffsets[pIfaceDesc->bInterfaceNumber] =
                    pPort->abIfaceOffsets[pIfaceDesc->bInterfaceNumber - 1] + bMaxIfaceEnds;
            }
            else
            {
                pPort->abIfaceOffsets[0] = 0;
            }
            /* set max to new iface's primary */
            bMaxIfaceEnds = pIfaceDesc->bNumEndpoints;
        }

        wLimit = wLength - (uint16_t)((uint8_t *)pIfaceDesc - (uint8_t *)pConfigDesc);
        for(bEndIndex = 0; (bEndIndex < pIfaceDesc->bNumEndpoints) && !dwStatus;
                bEndIndex++)
        {
            /* find endpoint */
            pDesc = MGC_FindDescriptor((uint8_t *)pIfaceDesc, wLimit,
                                       MUSB_DT_ENDPOINT, bEndIndex);
            /* break on failure */
            if(!pDesc)
            {
                dwStatus = MUSB_STATUS_BAD_DESCRIPTORS;
                break;
            }
            /* endpoint found; bind (or just verify) it */
            pEndDesc = (MUSB_EndpointDescriptor *)pDesc;
            MUSB_MemCopy((void *) & (Endpoint.UsbDescriptor),
                         (void *)pEndDesc, sizeof(MUSB_EndpointDescriptor));
            pResource = pPort->pfBindEndpoint(pPort, &Endpoint, NULL, bReallyBind);
            /* break on failure */
            if(!pResource)
            {
                dwStatus = MUSB_STATUS_NO_RESOURCES;
                break;
            }
            /* if really binding, prepare pipe */
            if(bReallyBind)
            {
                MGC_FunctionPreparePipe(pPort,
                                        pPort->apPipe[bEndIndex + pPort->abIfaceOffsets[pIfaceDesc->bInterfaceNumber]],
                                        (MUSB_BusHandle)pPort, pResource, pEndDesc);
            }
        }
    }

    /*
     * Update the maximum end count if needed.
     */
    if(bMaxFunctionEnds > pPort->bMaxFunctionEnds)
    {
        pPort->bMaxFunctionEnds = bMaxFunctionEnds;
    }

    return dwStatus;
}

/**
 * Verify that resources are available to support all given configurations
 * @param pPort port pointer
 * @param pDescriptorBuffer descriptor buffer
 * @param wDescriptorBufferLength size of descriptor buffer
 * @param bNumConfigurations how many configuration descriptors to expect in buffer
 * @param bType configuration descriptor type (CONFIGURATION or OTHER_SPEED_CONFIG)
 * @param bVerify TRUE to actually verify resources; FALSE to just set pointers in port
 */
static uint32_t MGC_FunctionVerifyResources(MGC_Port *pPort, const uint8_t *pDescriptorBuffer,
        uint16_t wDescriptorBufferLength,
        uint8_t bNumConfigurations, uint8_t bType,
        uint8_t bVerify)
{
    uint8_t bConfigIndex;
    const uint8_t *pDesc;
    const MUSB_ConfigurationDescriptor *pConfigDesc;
    uint32_t dwStatus = MUSB_STATUS_OK;

    for(bConfigIndex = 0;
            (bConfigIndex < bNumConfigurations) && !dwStatus;
            bConfigIndex++)
    {
        pDesc = MGC_FindDescriptor(pDescriptorBuffer, wDescriptorBufferLength,
                                   bType, bConfigIndex);
        if(pDesc)
        {
            /* config found */
            pConfigDesc = (MUSB_ConfigurationDescriptor *)pDesc;
            /* verify or set pointer */
            if(bVerify)
            {
                dwStatus = MGC_FunctionBindConfig(pPort, pConfigDesc, FALSE);
            }
            else
            {
                pPort->apConfigDescriptors[bConfigIndex] = pConfigDesc;
            }
        }
        else
        {
            dwStatus = MUSB_STATUS_BAD_DESCRIPTORS;
            break;
        }
    }

    return dwStatus;
}

/*
* Register a client
*/
uint32_t MGC_FunctionRegisterClient(MGC_Port *pPort,
                                    MUSB_FunctionClient *pClient)
{
    uint8_t bEndIndex;
    MGC_Pipe *pPipe;
    uint32_t dwStatus = MUSB_STATUS_OK;
    const MUSB_DeviceDescriptor *pDeviceDesc =
        (MUSB_DeviceDescriptor *)pClient->pStandardDescriptors;
    const MUSB_QualifierDescriptor *pQualifierDesc =
        (MUSB_QualifierDescriptor *)pClient->pHighSpeedDescriptors;
    uint8_t bNumConfigurations = pDeviceDesc->bNumConfigurations;

    pPort->bParse = TRUE;
    /* allocate convenience pointers for each config */
    if(pClient->wHighSpeedDescriptorLength >= sizeof(MUSB_QualifierDescriptor))
    {
        bNumConfigurations = MUSB_MAX(bNumConfigurations, pQualifierDesc->bNumConfigurations);
    }


    pPort->apConfigDescriptors = (const MUSB_ConfigurationDescriptor **)MUSB_MemAlloc(
                                     bNumConfigurations * sizeof(MUSB_ConfigurationDescriptor *));
    if (pPort->apConfigDescriptors)
    {
        /* find each config; verify resources and set config pointers */
        pPort->bMaxFunctionEnds = 0;
        dwStatus = MGC_FunctionVerifyResources(pPort, pClient->pStandardDescriptors,
                                               pClient->wDescriptorBufferLength, pDeviceDesc->bNumConfigurations,
                                               MUSB_DT_CONFIG, TRUE);

        MUSB_PRT("[MGC] FunctionRegisterClient :%x\r\n", dwStatus);
        if(!dwStatus && (pClient->wHighSpeedDescriptorLength > sizeof(MUSB_QualifierDescriptor)))
        {
            dwStatus = MGC_FunctionVerifyResources(pPort, pClient->pHighSpeedDescriptors,
                                                   pClient->wHighSpeedDescriptorLength, pQualifierDesc->bNumConfigurations,
                                                   MUSB_DT_OTHER_SPEED, TRUE);
            MUSB_PRT("[MGC] FunctionRegisterClient dwstatus :%x\r\n", dwStatus);
        }
    }
    else
    {
        dwStatus = MUSB_STATUS_NO_MEMORY;
    }

    /* if we are OK; allocate pipe memory now */
    if(pPort->bMaxFunctionEnds && (0 == dwStatus))
    {
        pPort->apPipe = (MGC_Pipe **)MUSB_MemAlloc(
                            pPort->bMaxFunctionEnds * sizeof(MGC_Pipe *));
        if (pPort->apPipe)
        {
            pPipe = (MGC_Pipe *)MUSB_MemAlloc(
                        pPort->bMaxFunctionEnds * sizeof(MGC_Pipe));
            if (pPipe)
            {
                for (bEndIndex = 0; bEndIndex < pPort->bMaxFunctionEnds; bEndIndex++)
                {
                    pPort->apPipe[bEndIndex] = pPipe++;
                }
            }
            else
            {
                MUSB_MemFree(pPort->apPipe);
                pPort->apPipe = NULL;
                dwStatus = MUSB_STATUS_NO_MEMORY;
            }
        }
        else
        {
            dwStatus = MUSB_STATUS_NO_MEMORY;
        }
    }

    /* possibly clean up */
    if(dwStatus && pPort->apConfigDescriptors)
    {
        MUSB_MemFree((void *)pPort->apConfigDescriptors);
        pPort->apConfigDescriptors = NULL;
    }

    return dwStatus;
}

uint32_t MGC_FunctionDestroy(MGC_Port *pPort)
{
    if(pPort->apConfigDescriptors)
    {
        MUSB_MemFree(pPort->apConfigDescriptors);
        pPort->apConfigDescriptors = NULL;
    }
    if(pPort->apPipe)
    {
        MUSB_MemFree(pPort->apPipe[0]);
        MUSB_MemFree(pPort->apPipe);
        pPort->apPipe = NULL;
    }
    return MUSB_STATUS_OK;
}

/*
 * Handle config selected
 * @return TRUE on success
 * @return FALSE on failure (caller needs to STALL)
 */
static uint8_t MGC_FunctionConfigSelected(MGC_Port *pPort, uint8_t bConfigValue)
{
    uint32_t dwStatus;
    uint8_t bConfig;
    const MUSB_ConfigurationDescriptor *pConfigDesc;
    uint8_t bOk = FALSE;
    const MUSB_DeviceDescriptor *pDeviceDesc =
        (MUSB_DeviceDescriptor *)pPort->pFunctionClient->pStandardDescriptors;
    const MUSB_QualifierDescriptor *pQualifierDesc =
        (MUSB_QualifierDescriptor *)pPort->pFunctionClient->pHighSpeedDescriptors;
    uint8_t bNumConfigurations = pDeviceDesc->bNumConfigurations;

    if(pPort->bIsHighSpeed && pQualifierDesc)
    {
        bNumConfigurations = pQualifierDesc->bNumConfigurations;
    }

    /* find config */
    pPort->pCurrentConfig = NULL;
    for(bConfig = 0; bConfig < bNumConfigurations; bConfig++)
    {
        pConfigDesc = (MUSB_ConfigurationDescriptor *)pPort->apConfigDescriptors[bConfig];
        if(pConfigDesc->bConfigurationValue == bConfigValue)
        {
            pPort->pCurrentConfig = pConfigDesc;
            break;
        }
    }

    if(pPort->pCurrentConfig)
    {
        /* try to really bind config */
        dwStatus = MGC_FunctionBindConfig(pPort, pPort->pCurrentConfig, TRUE);
        if(MUSB_STATUS_OK == dwStatus)
        {
            /* commit config and call client */
            pPort->bConfigValue = bConfigValue;
            pPort->pFunctionClient->pfDeviceConfigSelected(
                pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
                bConfigValue, (MUSB_PipePtr *)pPort->apPipe);
            MGC_FunctionChangeState(pPort, MUSB_CONFIGURED);
            bOk = TRUE;
        }
    }

    return bOk;
}

/**
 * Based on connection speed, set config pointer array to prepare for SET_CONFIGURATION()
 */
void MGC_FunctionSpeedSet(MGC_Port *pPort)
{
    uint32_t dwStatus = MUSB_STATUS_OK;
    const MUSB_DeviceDescriptor *pDeviceDesc;
    const MUSB_QualifierDescriptor *pQualifierDesc;

    if(pPort->pFunctionClient)
    {
        pDeviceDesc = (MUSB_DeviceDescriptor *)pPort->pFunctionClient->pStandardDescriptors;
        pQualifierDesc = (MUSB_QualifierDescriptor *)pPort->pFunctionClient->pHighSpeedDescriptors;
        if(pPort->bIsHighSpeed && pQualifierDesc)
        {
            dwStatus = MGC_FunctionVerifyResources(pPort,
                                                   pPort->pFunctionClient->pHighSpeedDescriptors,
                                                   pPort->pFunctionClient->wHighSpeedDescriptorLength,
                                                   pQualifierDesc->bNumConfigurations, MUSB_DT_OTHER_SPEED, FALSE);
        }
        else
        {
            dwStatus = MGC_FunctionVerifyResources(pPort,
                                                   pPort->pFunctionClient->pStandardDescriptors,
                                                   pPort->pFunctionClient->wDescriptorBufferLength,
                                                   pDeviceDesc->bNumConfigurations, MUSB_DT_CONFIG, FALSE);
        }
    }
    dwStatus ++;//avoid warning
}

/*
* Parse a setup and possibly handle it based on descriptors
* Returns: TRUE if handled; FALSE to call client
*/
uint8_t MGC_FunctionParseSetup(MGC_Port *pPort, uint8_t *pbStatus)
{
    uint8_t bOurs = FALSE;
    uint8_t bOK;
    uint8_t bEnd;
    uint8_t bRecip;	/* from standard request */
    uint8_t bType;	/* requested descriptor type */
    uint8_t bQueryType; /* descriptor type to query (for CONFIGURATION handling) */
    uint16_t wWhich;	/* requested descriptor index */
    uint16_t wIter;	/* found descriptor index */
    uint16_t wValue;	/* from standard request */
    uint16_t wIndex;	/* from standard request */
    uint16_t wLength;	/* from standard request */
    const MUSB_DeviceRequest *pRequest;
    const uint8_t *pDesc;
    const MUSB_QualifierDescriptor *pQualifierDesc;
    MUSB_QualifierDescriptor *pGeneratedQualifier;
    MUSB_DeviceDescriptor *pGeneratedDevice;
    MUSB_Irp *pIrp;
    MGC_Pipe *pPipe;
    uint16_t wLang;
    uint8_t bFixed = FALSE;
    const uint16_t *pwLangId = NULL;	/* points to LANGID from descriptor 0 */
    uint16_t wLangCount = 0;
    uint8_t bSwitchDescriptor = FALSE;
    MGC_EndpointResource *pEnd = NULL;
    const MUSB_DeviceDescriptor *pDeviceDesc =
        (MUSB_DeviceDescriptor *)pPort->pFunctionClient->pStandardDescriptors;
    const uint8_t *pScan = pPort->pFunctionClient->pStandardDescriptors;
    const uint8_t *pScanLimit = pScan + pPort->pFunctionClient->wDescriptorBufferLength;

    /* seed success */
    *pbStatus = MUSB_STATUS_OK;
    pPort->pSetupData = pPort->pFunctionClient->pControlBuffer;
    pRequest = (MUSB_DeviceRequest *)pPort->pSetupData;
    if (pRequest && (MUSB_TYPE_STANDARD == (pRequest->bmRequestType & MUSB_TYPE_MASK)))
    {
        bRecip = pRequest->bmRequestType & MUSB_RECIP_MASK;
        wValue = MUSB_SWAP16P((uint8_t *) & (pRequest->wValue));
        wIndex = MUSB_SWAP16P((uint8_t *) & (pRequest->wIndex));
        MGC_DIAG2(3, pPort->pController, "ParseSetup: std request, type=",
                  pRequest->bmRequestType, "/req=", pRequest->bRequest, 16, 2);
        MGC_DIAG2(3, pPort->pController, "ParseSetup: USB state=",
                  pPort->bUsbState, "/wLength=", pRequest->wLength, 16, 2);
        MGC_DIAG2(3, pPort->pController, "ParseSetup: wValue=",
                  wValue, "/wIndex=", wIndex, 16, 2);
        switch (pRequest->bRequest)
        {
        case MUSB_REQ_GET_STATUS:
            if((pPort->bUsbState >= MUSB_ADDRESS) || (0 == wIndex))
            {
                /* we handle device, interface, and endpoint recipients */
                if(MUSB_RECIP_DEVICE == bRecip)
                {
                    bOurs = TRUE;
                    pPort->pSetupData[0] = *(pPort->pFunctionClient->pbSelfPowered) ? 1 : 0;
                    pPort->pSetupData[0] |= pPort->bCanWakeup ? 2 : 0;
                    pPort->pSetupData[1] = 0;
                    pPort->wSetupDataSize = 2;
                }
                else if(MUSB_RECIP_INTERFACE == bRecip)
                {
                    pDesc = MGC_FindDescriptor((uint8_t *)pPort->pCurrentConfig,
                                               MUSB_SWAP16P((uint8_t *) & (pPort->pCurrentConfig->wTotalLength)),
                                               MUSB_DT_INTERFACE, (uint8_t)wIndex);
                    if(pDesc)
                    {
                        pPort->pSetupData[0] = 0;
                        pPort->pSetupData[1] = 0;
                        pPort->wSetupDataSize = 2;
                    }
                    else
                    {
                        *pbStatus = MUSB_STATUS_STALLED;
                    }
                }
                else if(MUSB_RECIP_ENDPOINT == bRecip)
                {
                    bOurs = TRUE;
                    bEnd = (uint8_t)wIndex;
                    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                               &(pPort->LocalEnds), bEnd & MUSB_ENDPOINT_NUMBER_MASK);
                    if(pEnd)
                    {
                        pPort->pSetupData[0] = pEnd->bIsHalted ? 1 : 0;
                        pPort->pSetupData[1] = 0;
                        pPort->wSetupDataSize = 2;
                    }
                    else
                    {
                        *pbStatus = MUSB_STATUS_STALLED;
                    }
                }
                else
                {
                    *pbStatus = MUSB_STATUS_STALLED;
                }
            }	/* END: if valid USB state */
            break;

        case MUSB_REQ_CLEAR_FEATURE:
            /* we handle device, interface (nothing defined) and endpoint */
            if((pPort->bUsbState >= MUSB_ADDRESS) || (0 == wIndex))
            {
                switch(bRecip)
                {
                case MUSB_RECIP_DEVICE:
                    bOurs = TRUE;
                    pPort->wSetupDataSize = 0;
                    if(MUSB_FEATURE_DEVICE_REMOTE_WAKEUP == wValue)
                    {
                        pPort->bCanWakeup = FALSE;
                    }
                    break;

                case MUSB_RECIP_INTERFACE:
                    pPort->wSetupDataSize = 0;
                    break;

                case MUSB_RECIP_ENDPOINT:
                    bOurs = TRUE;
                    pPort->wSetupDataSize = 0;
                    bEnd = (uint8_t)wIndex;
                    /* seed failure and prove success */
                    *pbStatus = MUSB_STATUS_STALLED;
                    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                               &(pPort->LocalEnds), bEnd & MUSB_ENDPOINT_NUMBER_MASK);
                    if(pEnd)
                    {
                        *pbStatus = MUSB_STATUS_OK;
                        pEnd->bIsHalted = FALSE;
                        pPort->pfProgramHaltEndpoint(pPort, pEnd,
                                                     bEnd & MUSB_ENDPOINT_DIR_MASK, FALSE);
                        /* start any pending bulk/interrupt Tx */
                        switch(pEnd->bTrafficType)
                        {
                        case MUSB_ENDPOINT_XFER_BULK:
                        case MUSB_ENDPOINT_XFER_INT:
                            pIrp = (MUSB_Irp *)pEnd->pTxIrp;
                            if(pIrp)
                            {
                                pPipe = (MGC_Pipe *)pIrp->hPipe;
                                if(pPipe && (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT))
                                {
                                    pPort->pfProgramStartTransmit(pPort, pEnd,
                                                                  pIrp->pBuffer, pIrp->dwLength, pIrp);
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            else
            {
                *pbStatus = MUSB_STATUS_STALLED;
            }
            break;

        case MUSB_REQ_SET_FEATURE:
            /* we handle device, interface (nothing defined) and endpoint */
            if((pPort->bUsbState >= MUSB_ADDRESS) || (0 == wIndex))
            {
                switch(bRecip)
                {
                case MUSB_RECIP_DEVICE:
                    bOurs = TRUE;
                    pPort->wSetupDataSize = 0;
                    /* seed failure and prove success */
                    *pbStatus = MUSB_STATUS_STALLED;
                    if(MUSB_FEATURE_DEVICE_REMOTE_WAKEUP == wValue)
                    {
                        pPort->bCanWakeup = TRUE;
                        *pbStatus = MUSB_STATUS_OK;
                    }
                    else if(pPort->bIsHighSpeed && (MUSB_FEATURE_TEST_MODE == wValue))
                    {
                        pPort->bWantTestMode = TRUE;
                        pPort->bTestMode = (uint8_t)(wIndex >> 8);
                        *pbStatus = MUSB_STATUS_OK;
                    }
#ifdef MUSB_OTG
                    else if(MUSB_b_hnp_enable == wValue)
                    {
                        /* only succeed if we support HNP */
                        if(pPort->bIsHnpSupported)
                        {
                            pPort->bIsHnpAllowed = TRUE;
                            if(*(pPort->pOtgClient->pbDesireHostRole))
                            {
                                pPort->bWantHost = TRUE;
                                pPort->pfProgramBusState(pPort);
                            }
                            *pbStatus = MUSB_STATUS_OK;
                        }
                    }
                    else if(MUSB_a_hnp_support == wValue)
                    {
                        /* nothing to remember */
                        /*MGC_DIAG(2, pPort->pController, "A-device and connection supports HNP");*/
                        *pbStatus = MUSB_STATUS_OK;
                    }
                    else if(MUSB_a_alt_hnp_support == wValue)
                    {
                        /* nothing to remember */
                        /*MGC_DIAG(2, pPort->pController, "A-device supports HNP on another port");*/
                        *pbStatus = MUSB_STATUS_OK;
                    }
#endif
                    break;

                case MUSB_RECIP_INTERFACE:
                    bOurs = TRUE;
                    pPort->wSetupDataSize = 0;
                    break;

                case MUSB_RECIP_ENDPOINT:
                    bOurs = TRUE;
                    pPort->wSetupDataSize = 0;
                    bEnd = (uint8_t)wIndex;
                    /* seed failure and prove success */
                    *pbStatus = MUSB_STATUS_STALLED;
                    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                               &(pPort->LocalEnds), bEnd & MUSB_ENDPOINT_NUMBER_MASK);
                    if(pEnd)
                    {
                        *pbStatus = MUSB_STATUS_OK;
                        pEnd->bIsHalted = TRUE;
                        pPort->pfProgramHaltEndpoint(pPort, pEnd,
                                                     bEnd & MUSB_ENDPOINT_DIR_MASK, TRUE);
                    }
                    break;
                }
            }
            break;

        case MUSB_REQ_SET_ADDRESS:
            bOurs = TRUE;
            pPort->wSetupDataSize = 0;
            if((pPort->bUsbState < MUSB_ADDRESS) || pPort->bSetAddress)
            {
                pPort->bSetAddress = TRUE;
                pPort->bFuncAddr = (uint8_t)(wValue & 0x7f);
                MGC_FunctionChangeState(pPort, MUSB_ADDRESS);
            }
            else
            {
                *pbStatus = MUSB_STATUS_STALLED;
            }
            break;

        case MUSB_REQ_GET_DESCRIPTOR:
            if(MUSB_RECIP_DEVICE == bRecip)
            {
                bOurs = TRUE;
                wLength = MUSB_SWAP16P((uint8_t *) & (pRequest->wLength));
                bOK = FALSE;
                bType = bQueryType = (uint8_t)(wValue >> 8);
                wWhich = wValue & 0xff;
                wIter = 0;

                /* do a little dance if we have high-speed descriptors */
                if(pPort->pFunctionClient->pHighSpeedDescriptors)
                {
                    switch(bType)
                    {
                    case MUSB_DT_DEVICE:
                        if(pPort->bIsHighSpeed)
                        {
                            /* we are at high-speed, so use full-speed and switch the type */
                            bSwitchDescriptor = TRUE;
                        }
                        break;

                    case MUSB_DT_DEVICE_QUALIFIER:
                        if(pPort->bIsHighSpeed)
                        {
                            /* we are at high-speed, so use full-speed and switch the type */
                            bSwitchDescriptor = TRUE;
                            bQueryType = MUSB_DT_DEVICE;
                        }
                        else
                        {
                            /* we are at full-speed, so search high-speed and no need to switch */
                            pScan = pPort->pFunctionClient->pHighSpeedDescriptors;
                            pScanLimit = pScan + pPort->pFunctionClient->wHighSpeedDescriptorLength;
                        }
                        break;

                    case MUSB_DT_CONFIG:
                        if(pPort->bIsHighSpeed)
                        {
                            /* we are at high-speed, so search high-speed and switch the type */
                            pScan = pPort->pFunctionClient->pHighSpeedDescriptors;
                            pScanLimit = pScan + pPort->pFunctionClient->wHighSpeedDescriptorLength;
                            bSwitchDescriptor = TRUE;
                            bQueryType = MUSB_DT_OTHER_SPEED;
                        }
                        break;

                    case MUSB_DT_OTHER_SPEED:
                        if(pPort->bIsHighSpeed)
                        {
                            /* we are at high-speed, so use full-speed and switch the type */
                            bSwitchDescriptor = TRUE;
                            bQueryType = MUSB_DT_CONFIG;
                        }
                        else
                        {
                            /* we are at full-speed, so search high-speed and no need to switch */
                            pScan = pPort->pFunctionClient->pHighSpeedDescriptors;
                            pScanLimit = pScan + pPort->pFunctionClient->wHighSpeedDescriptorLength;
                        }
                        break;
                    }	/* END: switch on descriptor type */
                }	/* END: if we have high-speed descriptors */

                /* search the descriptor buffer */
                while(!bOK && (pScan < pScanLimit))
                {
                    if(bQueryType == pScan[1])
                    {
                        /* special case for strings */
                        if(MUSB_DT_STRING == bQueryType)
                        {
                            /* remember where langid's are */
                            if(!wIter)
                            {
                                wLangCount = (pScan[0] - 2) >> 1;
                                pwLangId = (const uint16_t *) & (pScan[2]);
                            }
                            else if(wWhich && !bFixed)
                            {
                                bFixed = TRUE;
                                /* scale wWhich based on langid */
                                for(wLang = 0; wLang < wLangCount; wLang++)
                                {
                                    if(wIndex == MUSB_SWAP16P((uint8_t *) & (pwLangId[wLang])))
                                    {
                                        break;
                                    }
                                }
                                if(wLang >= wLangCount)
                                {
                                    /* langid not found */
                                    *pbStatus = MUSB_STATUS_STALLED;
                                    break;
                                }
                                wWhich += (wLang * pPort->pFunctionClient->wStringDescriptorCount);
                            }
                        }
                        if(wIter == wWhich)
                        {
                            bOK = TRUE;
                            break;
                        }
                        wIter++;
                    }
                    pScan += pScan[0];
                }
                if(pScan < pScanLimit)
                {
                    /* descriptor found; see if we need to finish the dance */
                    if(bSwitchDescriptor)
                    {
                        /* we need to generate/modify */
                        switch(bType)
                        {
                        case MUSB_DT_DEVICE:
                            /* generate a device descriptor based on a qualifier descriptor */
                            pQualifierDesc = (MUSB_QualifierDescriptor *)pPort->pFunctionClient->pHighSpeedDescriptors;
                            pPort->wSetupDataSize = MUSB_MIN(wLength,
                                                             sizeof(MUSB_DeviceDescriptor));
                            MUSB_MemCopy(pPort->pSetupData, pScan,
                                         pPort->wSetupDataSize);
                            pGeneratedDevice = (MUSB_DeviceDescriptor *)pPort->pSetupData;
                            /* buffer must be large enough */
                            pGeneratedDevice->bcdUSB = pQualifierDesc->bcdUSB;
                            pGeneratedDevice->bDeviceClass = pQualifierDesc->bDeviceClass;
                            pGeneratedDevice->bDeviceSubClass = pQualifierDesc->bDeviceSubClass;
                            pGeneratedDevice->bDeviceProtocol = pQualifierDesc->bDeviceProtocol;
                            pGeneratedDevice->bMaxPacketSize0 = pQualifierDesc->bMaxPacketSize0;
                            pGeneratedDevice->bNumConfigurations = pQualifierDesc->bNumConfigurations;
                            break;

                        case MUSB_DT_DEVICE_QUALIFIER:
                            /* generate a qualifier descriptor based on a device descriptor */
                            pQualifierDesc = (MUSB_QualifierDescriptor *)pPort->pFunctionClient->pHighSpeedDescriptors;
                            pPort->wSetupDataSize = MUSB_MIN(wLength,
                                                             sizeof(MUSB_QualifierDescriptor));
                            MUSB_MemCopy(pPort->pSetupData, (uint8_t *)pQualifierDesc,
                                         pPort->wSetupDataSize);
                            pGeneratedQualifier = (MUSB_QualifierDescriptor *)pPort->pSetupData;
                            /* buffer must be large enough */
                            pGeneratedQualifier->bcdUSB = pDeviceDesc->bcdUSB;
                            pGeneratedQualifier->bDeviceClass = pDeviceDesc->bDeviceClass;
                            pGeneratedQualifier->bDeviceSubClass = pDeviceDesc->bDeviceSubClass;
                            pGeneratedQualifier->bDeviceProtocol = pDeviceDesc->bDeviceProtocol;
                            pGeneratedQualifier->bMaxPacketSize0 = pDeviceDesc->bMaxPacketSize0;
                            pGeneratedQualifier->bNumConfigurations = pDeviceDesc->bNumConfigurations;
                            break;

                        case MUSB_DT_CONFIG:
                        case MUSB_DT_OTHER_SPEED:
                            /* same format, just change the type */
                            pPort->wSetupDataSize = (uint16_t)MUSB_MIN(wLength, pScanLimit - pScan);
                            MUSB_MemCopy( pPort->pSetupData, pScan, pPort->wSetupDataSize );
                            pPort->pSetupData[1] = bType;
                            break;
                        }
                    }
                    else
                    {
                        /* no dance; just send it from the client buffer */
                        switch(bQueryType)
                        {
                        case MUSB_DT_CONFIG:
                        case MUSB_DT_OTHER_SPEED:
                            pPort->wSetupDataSize = (uint16_t)MUSB_MIN(wLength, pScanLimit - pScan);
                            break;
                        default:
                            pPort->wSetupDataSize = (uint16_t)MUSB_MIN(wLength, pScan[0]);
                        }
                        pPort->pSetupData = (uint8_t *)pScan;
                    }
                }
                else
                {
                    /* no such descriptor found */
                    *pbStatus = MUSB_STATUS_STALLED;
                }
            }
            else if (MUSB_RECIP_INTERFACE == bRecip)
            {
                wIter = 0;
                bOK = FALSE;
                bOurs = TRUE;

                wLength = MUSB_SWAP16P((uint8_t *) & (pRequest->wLength));
                bType = bQueryType = (uint8_t)(wValue >> 8);
                wWhich = wValue & 0x00ff;

                switch (bType)
                {
                case MUSB_DT_HID_REPORT:
#ifdef CFG_ENABLE_DEV_HID
                    switch (wIndex)
                    {
                    case 0:
                        MUSB_MemCopy(pPort->pSetupData, gHidMouseReportDescriptor, ulgHidMouseReportDescriptorLen);
                        pPort->wSetupDataSize = ulgHidMouseReportDescriptorLen;
                        break;

                    case 1:
                        MUSB_MemCopy(pPort->pSetupData, gHidRMCVoiceReportDescriptor, ulgHidRMCVoiceReportDescriptorLen);
                        pPort->wSetupDataSize = ulgHidRMCVoiceReportDescriptorLen;
                        break;

                    case 2:
                        MUSB_MemCopy(pPort->pSetupData, gHidKeyboardReportDescriptor, ulgHidKeyboardReportDescriptorLen);
                        pPort->wSetupDataSize = ulgHidKeyboardReportDescriptorLen;
                        break;

                    default:
                        break;
                    }
#endif // CFG_ENABLE_DEV_HID
                    break;

                default:
                    break;
                }
            }
            break;

        case MUSB_REQ_GET_CONFIGURATION:
            bOurs = TRUE;
            pPort->pSetupData[0] = pPort->bConfigValue;
            pPort->wSetupDataSize = 1;
            break;

        case MUSB_REQ_SET_CONFIGURATION:
            bOurs = TRUE;
            pPort->wSetupDataSize = 0;
            bOK = FALSE;
            if(pPort->bUsbState >= MUSB_ADDRESS)
            {
                bOK = FALSE;
                if(!wValue)
                {
                    bOK = TRUE;
                    pPort->bConfigValue = 0;
                    MGC_FunctionChangeState(pPort, MUSB_ADDRESS);
                }
                else
                {
                    bOK = MGC_FunctionConfigSelected(pPort, (uint8_t)(wValue & 0xff));
                }
            }
            if(!bOK)
            {
                *pbStatus = MUSB_STATUS_STALLED;
            }
            break;

        case MUSB_REQ_GET_INTERFACE:
        case MUSB_REQ_SET_INTERFACE:
            pPort->wSetupDataSize = 0;
            /* if improper state, stall */
            if(MUSB_CONFIGURED != pPort->bUsbState)
            {
                bOurs = TRUE;
                *pbStatus = MUSB_STATUS_STALLED;
            }
            else if(MUSB_REQ_SET_INTERFACE == pRequest->bRequest)
            {
                /* re-bind */
                bOurs = TRUE;
                if(MUSB_STATUS_OK != MGC_FunctionSetInterface(pPort,
                        (uint8_t)(wIndex & 0xff), (uint8_t)(wValue & 0xff)))
                {
                    *pbStatus = MUSB_STATUS_STALLED;
                }
            }
            break;
        }   /* END: switch on request type */
    }	/* END: if standard request */
    else if (pRequest && (MUSB_TYPE_CLASS == (pRequest->bmRequestType & MUSB_TYPE_MASK)))
    {
        bOurs = TRUE;
        wValue = ((((pRequest->wValue) & 0x00FF) << 8) | (((pRequest->wValue) & 0xFF00) >> 8));
        wIndex = ((((pRequest->wIndex) & 0x00FF) << 8) | (((pRequest->wIndex) & 0xFF00) >> 8));
        MUSB_PRT("CLASS: pRequest->bRequest = 0x%x, wIndex = 0x%x, wValue = 0x%x\r\n",
                 pRequest->bRequest, wIndex, wValue);
        switch (pRequest->bRequest)
        {
        case MUSB_CLASS_SET_CURRENT:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_GET_IDLE:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_GET_PROTOCOL:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_SET_REPORT:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_SET_IDLE:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_SET_PROTOCOL:
            pPort->wSetupDataSize = 0;
            break;

        case MUSB_CLASS_GET_CURRENT:
            break;

        case MUSB_CLASS_GET_LEN:
            break;

        case MUSB_CLASS_GET_INFO:
            break;

        case MUSB_CLASS_GET_MINIUM:

            break;

        case MUSB_CLASS_GET_MAXIUM:

            break;

        case MUSB_CLASS_GET_RES:

            break;

        case MUSB_CLASS_GET_DEF:
            break;

        default:
            MUSB_PRT("bRequest=0x%x\r\n", pRequest->bRequest);
            bOurs = FALSE;
            break;
        }
    }	/* END: if class request */
    else if (pRequest && (MUSB_TYPE_VENDOR == (pRequest->bmRequestType & MUSB_TYPE_MASK)))
    {
    }	/* END: if class request */

    MGC_DIAG(3, pPort->pController, bOurs ?
             "ParseSetup: handled" : "ParseSetup: NOT handled");

    return bOurs;
}

/*
 * Change state
 */
void MGC_FunctionChangeState(MGC_Port *pPort, MUSB_State State)
{
    /* only signal a change */
    if(pPort->bUsbState != State)
    {
        MGC_DIAG1(2, pPort->pController, "USB state change: ", (uint32_t)State, 10, 0);
        pPort->bUsbState = State;
        if(pPort->pFunctionClient)//¾ÍÊÇMGC_MsdFunctionClient
        {
            pPort->pFunctionClient->pfUsbState(pPort->pFunctionClient->pPrivateData,
                                               (MUSB_BusHandle)pPort, State);
        }
    }
}

/*
* Respond to host setup
*/
uint32_t MUSB_DeviceResponse(MUSB_BusHandle hBus, uint32_t dwSequenceNumber,
                             const uint8_t *pResponseData, uint16_t wResponseDataLength, uint8_t bStall)
{
    uint32_t dwStatus = MUSB_STATUS_TIMEOUT;
    MGC_Port *pPort = (MGC_Port *)hBus;

    MGC_DIAG1(3, pPort->pController, "DeviceResponse: seqNum=",
              dwSequenceNumber, 16, 8);
    MGC_DIAG2(3, pPort->pController, "DeviceResponse: wLength=",
              wResponseDataLength, "/stall=", bStall, 16, 4);
    if(dwSequenceNumber == pPort->dwSequenceNumber)
    {
        pPort->pSetupData = (uint8_t *)pResponseData;
        pPort->wSetupDataSize = wResponseDataLength;
        pPort->pfDefaultEndResponse(pPort, bStall);
        dwStatus = MUSB_STATUS_OK;
    }

    return dwStatus;
}

void MUSB_SetFunctionParse(MUSB_BusHandle hBus, uint8_t bParse)
{
    MGC_Port *pPort = (MGC_Port *)hBus;

    if(pPort)
    {
        pPort->bParse = bParse;
    }
}
#endif
// eof


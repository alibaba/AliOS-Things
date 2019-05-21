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
 * Descriptor analysis library.
 * $Revision: 1.8 $
 */
#include "include.h"

#include "mu_descs.h"
#include "mu_arch.h"

#if CFG_USB
/*
 * Find an arbitrary descriptor
 */
const uint8_t *MUSB_FindDescriptor(const MUSB_ConfigurationDescriptor *pConfig,
                                   uint8_t bDescriptorType, uint8_t bIndex)
{
    uint16_t wBufferLength;
    int16_t wFoundIndex = -1;
    uint16_t wOffset = 0;
    const uint8_t *pBuffer = (uint8_t *)pConfig;
    const MUSB_DescriptorHeader *pHeader = (MUSB_DescriptorHeader *)pBuffer;

    if(pHeader &&
            ((pConfig->bDescriptorType == MUSB_DT_CONFIG) ||
             (pConfig->bDescriptorType == MUSB_DT_OTHER_SPEED)))
    {
        wBufferLength = pConfig->wTotalLength;
        /* handle trivial case */
        if((pHeader->bDescriptorType == bDescriptorType) && !bIndex)
        {
            return (uint8_t *)pHeader;
        }

        /* general case */
        while((wOffset < wBufferLength) && (wFoundIndex < (int16_t)bIndex))
        {
            pHeader = (MUSB_DescriptorHeader *)((uint8_t *)pBuffer + wOffset);
            if(pHeader->bLength < 1)
            {
                /* we're in the weeds */
                return NULL;
            }
            wOffset += pHeader->bLength;
            if(pHeader->bDescriptorType == bDescriptorType)
            {
                wFoundIndex++;
            }
        }
        if((wOffset <= wBufferLength) && (wFoundIndex == (int16_t)bIndex))
        {
            return (uint8_t *)pHeader;
        }
    }
    return NULL;
}

/*
 * Count alternate interfaces for each interface in a configuration
 */
uint8_t MUSB_CountAlternates(uint8_t *aCount,
                             const MUSB_ConfigurationDescriptor *pConfig)
{
    const uint8_t *pDescriptor;
    const MUSB_InterfaceDescriptor *pIfaceDescriptor;
    uint8_t bAlternate = 0;
    uint8_t bInterface = 0;
    uint8_t bIndex = 0;
    uint8_t bResult = 0;

    /* sanity check */
    if(!aCount || !pConfig)
    {
        return 0;
    }

    /* find first interface */
    pDescriptor = MUSB_FindDescriptor(pConfig, MUSB_DT_INTERFACE, bIndex);

    /* count */
    while(pDescriptor && (bInterface < pConfig->bNumInterfaces))
    {
        bAlternate++;
        pIfaceDescriptor = (const MUSB_InterfaceDescriptor *)pDescriptor;
        if(pIfaceDescriptor->bInterfaceNumber > bInterface)
        {
            /* we jumped to next interface; fill current and move on */
            aCount[bResult++] = bAlternate - 1;
            bAlternate = 1;
            bInterface = pIfaceDescriptor->bInterfaceNumber;
        }
        pDescriptor = MUSB_FindDescriptor(pConfig, MUSB_DT_INTERFACE, ++bIndex);
    }
    if(bAlternate)
    {
        /* this should always happen for a properly-formed descriptor set */
        aCount[bResult++] = bAlternate;
    }
    return bResult;

}

/*
 * Find an interface descriptor in a configuration
 */
const MUSB_InterfaceDescriptor *MUSB_FindInterfaceDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    uint8_t bInterfaceNumber,
    uint8_t bAlternateSetting)
{
    uint16_t wBufferLength, wOffset;
    const MUSB_DescriptorHeader *pHeader;
    const MUSB_InterfaceDescriptor *pInterface = NULL;
    int16_t wFound = -1;

    if(pConfig &&
            ((pConfig->bDescriptorType == MUSB_DT_CONFIG) ||
             (pConfig->bDescriptorType == MUSB_DT_OTHER_SPEED)))
    {
        wBufferLength = MUSB_SWAP16P((uint8_t *) & (pConfig->wTotalLength));
        wOffset = pConfig->bLength;
        /* search for the number */
        while((wOffset < wBufferLength) && (wFound != (int16_t)bInterfaceNumber))
        {
            pHeader = (MUSB_DescriptorHeader *)((uint8_t *)pConfig + wOffset);
            if(pHeader->bLength < 1)
            {
                /* we're in the weeds */
                return NULL;
            }
            wOffset += pHeader->bLength;
            if(pHeader->bDescriptorType == MUSB_DT_INTERFACE)
            {
                pInterface = (MUSB_InterfaceDescriptor *)((uint8_t *)pHeader);
                wFound = (int16_t)pInterface->bInterfaceNumber;
            }
        }
        if((wOffset == wBufferLength) &&
                (wFound == (int16_t)bInterfaceNumber) &&
                (pInterface->bAlternateSetting == bAlternateSetting))
        {
            return pInterface;
        }
        else if(wOffset < wBufferLength)
        {
            /* found number; now find alternate */
            wFound = -1;
            /* trivial case */
            if(!bAlternateSetting)
            {
                return pInterface;
            }
            /* general case */
            while((wOffset < wBufferLength) && (wFound != (int16_t)bAlternateSetting))
            {
                pHeader = (MUSB_DescriptorHeader *)((uint8_t *)pConfig + wOffset);
                if(pHeader->bLength < 1)
                {
                    /* we're in the weeds */
                    return NULL;
                }
                wOffset += pHeader->bLength;
                if(pHeader->bDescriptorType == MUSB_DT_INTERFACE)
                {
                    pInterface = (MUSB_InterfaceDescriptor *)((uint8_t *)pHeader);
                    wFound = (int16_t)pInterface->bAlternateSetting;
                }
            }
            if((wOffset < wBufferLength) &&
                    (pInterface->bInterfaceNumber == bInterfaceNumber))
            {
                return pInterface;
            }
        }
    }
    return NULL;
}

/*
 * Find an endpoint descriptor in an interface
 */
const MUSB_EndpointDescriptor *MUSB_FindEndpointDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    const MUSB_InterfaceDescriptor *pInterfaceDescriptor,
    uint8_t bEndpointIndex)
{
    return (MUSB_EndpointDescriptor *)MUSB_FindDescriptorInBlock(
               pConfig, (uint8_t *)pInterfaceDescriptor,
               MUSB_DT_ENDPOINT, bEndpointIndex);
}

/*
 * Find an arbitrary descriptor within a descriptor block.
 */
const uint8_t *MUSB_FindDescriptorInBlock(
    const MUSB_ConfigurationDescriptor *pConfig,
    const uint8_t *pDescriptorBlock,
    uint8_t bDescriptorType,
    uint8_t bIndex)
{
    uint8_t bLimitType;
    uint16_t wOffset;
    uint16_t wBufferLength;
    int16_t wFoundIndex = -1;
    const MUSB_DescriptorHeader *pHeader = (MUSB_DescriptorHeader *)pDescriptorBlock;

    if(pConfig &&
            ((pConfig->bDescriptorType == MUSB_DT_CONFIG) ||
             (pConfig->bDescriptorType == MUSB_DT_OTHER_SPEED)) &&
            pDescriptorBlock)
    {
        bLimitType = pHeader->bDescriptorType;
        wOffset = pHeader->bLength;
        pHeader = (MUSB_DescriptorHeader *)(pDescriptorBlock + wOffset);
        wBufferLength = pConfig->wTotalLength - (uint16_t)(pDescriptorBlock - (uint8_t *)pConfig);
        while((wOffset < wBufferLength) && (pHeader->bDescriptorType != bLimitType)
                && (wFoundIndex < (int16_t)bIndex))
        {
            pHeader = (MUSB_DescriptorHeader *)(pDescriptorBlock + wOffset);
            if(pHeader->bLength < 1)
            {
                /* we're in the weeds */
                return NULL;
            }
            wOffset += pHeader->bLength;
            if(pHeader->bDescriptorType == bDescriptorType)
            {
                wFoundIndex++;
            }
        }
        if((wOffset < wBufferLength) && (pHeader->bDescriptorType != bLimitType))
        {
            return (uint8_t *)pHeader;
        }
        /* also the tail case */
        if((wOffset == wBufferLength) && (wFoundIndex == (uint16_t)bIndex) &&
                (pHeader->bDescriptorType == bDescriptorType))
        {
            return (uint8_t *)pHeader;
        }
    }
    return NULL;
}
#endif // CFG_USB
// eof

/*******************************************************************************
  Ethernet Driver Library Source Code

  Summary:
    This file contains the source code for the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip PIC32MX family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
File Name:      drv_ethmac_lib.c
Processor:      PIC32MX
Compiler:       Microchip MPLAB C32 v1.00 or higher

Copyright © 2008-2009 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END


#include "driver/ethmac/src/dynamic/drv_ethmac_lib.h"

// Function that adjusts the alignment of stack descriptor lists that need to be 16 bytes aligned!
// This fixes a gcc allocation issue:
//      not properly allocating aligned data in the stack!
// Note that the size of allocated space needs to be large enough 
// to allow for this adjustment!:
//  allocSize >= align + ObjSize
//  We're using align == 16, ObjSize == 16
#define _EthAlignAdjust(pL)  ((DRV_ETHMAC_DCPT_LIST*)(((uint32_t)(pL) + (DRV_ETHMAC_DCPT_LIST_ALIGN - 1)) & (~(DRV_ETHMAC_DCPT_LIST_ALIGN -1))))
        


static void  _EnetPoolFreeDcptList(DRV_ETHMAC_DCPT_LIST* pList, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam);
static DRV_ETHMAC_DCPT_NODE* _EnetFindPacket(const void* pBuff, DRV_ETHMAC_DCPT_LIST* pList);
static int _EnetDescriptorsCount(DRV_ETHMAC_DCPT_LIST* pList, bool isHwCtrl);

// *****************************************************************************
// *****************************************************************************
// Section: Code from _eth_append_busy_list_lib.c
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function:        _EthAppendBusyList
 *
 * PreCondition:    None
 *
 * Input:           pBusyList  - busy list under hw control to add nodes to
 *                  pNewList   - list with descriptors to be added to the busy one
 *                  rxAck      - acknowledge the hw, it's an rx list
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function appends a list to the list of busy descriptors owned by the hw.
 *                  The new list to be added is supposed to have all the descriptors properly set.
 *
 * Note:            The late acknowledge for the head of the append list is to avoid a deadlock:
 *                  if hw is stuck on the end of list descriptors, decrementing the bufcnt will force re-fetch of the same
 *                  end descriptor and won't do much good.
 *                  We have to specifically write the BUFCDEC when there's a new descriptor seen by the hw that has EOWN=1.
 *****************************************************************************/
static void _EthAppendBusyList(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_DCPT_LIST* pBusyList, DRV_ETHMAC_DCPT_LIST* pNewList, int rxAck)
{
    DRV_ETHMAC_DCPT_NODE   *head, *tail, *pN;
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    tail=pBusyList->tail;
    head=DRV_ETHMAC_LIB_ListRemoveHead(pNewList);
    head->hwDcpt.hdr.EOWN=0;    // not hw owned yet!

    // add all the required new descriptors/buffers
    while((pN=DRV_ETHMAC_LIB_ListRemoveHead(pNewList)))
    {
        DRV_ETHMAC_LIB_ListAddTail(pBusyList, pN);
        if(rxAck && !pN->hwDcpt.hdr.rx_nack)
        {
            PLIB_ETH_RxBufferCountDecrement(ethId);
        }
    }

    head->next=tail->next;
    head->hwDcpt.next_ed=tail->hwDcpt.next_ed;  // proper connection needed
    *tail=*head;            // replace the prev tail

    head->hwDcpt.hdr.w=0;   // make it invalid descriptor for searches!
    head->hwDcpt.pEDBuff = 0;
    DRV_ETHMAC_LIB_ListAddTail(pBusyList, head); // end properly, with EOWN=0;
    tail->hwDcpt.hdr.EOWN=1;    // ready to go!
    if(rxAck && !tail->hwDcpt.hdr.rx_nack)
    {
        PLIB_ETH_RxBufferCountDecrement(ethId);
    }

} //_EthAppendBusyList


static void _EthMacReset(ETH_MODULE_ID ethId)
{
    PLIB_ETH_MIIResetEnable(ethId);
    PLIB_ETH_MIIResetDisable(ethId);
}


static void _EthMacInit(ETH_MODULE_ID ethId)
{
    _EthMacReset(ethId);
    PLIB_ETH_MaxFrameLengthSet(ethId,0x600);
}


/****************************************************************************
 * Function:        DRV_ETHMAC_LibInit
 *****************************************************************************/
void DRV_ETHMAC_LibInit(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;
    PLIB_ETH_Disable(ethId);
    PLIB_ETH_TxRTSDisable(ethId);
    PLIB_ETH_RxDisable(ethId);

    while( PLIB_ETH_EthernetIsBusy(ethId) )
    {
        //Do Nothing.
    }
    PLIB_ETH_Enable(ethId);

    while(PLIB_ETH_RxPacketCountGet(ethId) > 0 )
    {
        PLIB_ETH_RxBufferCountDecrement(ethId);
    }

    // initialize the Ethernet TX/RX lists
    pMacD->mData._EnetTxFreePtr = DRV_ETHMAC_LIB_ListInit(&pMacD->mData._EnetTxFreeList);
    pMacD->mData._EnetTxBusyPtr = DRV_ETHMAC_LIB_ListInit(&pMacD->mData._EnetTxBusyList);
    pMacD->mData._EnetRxFreePtr = DRV_ETHMAC_LIB_ListInit(&pMacD->mData._EnetRxFreeList);
    pMacD->mData._EnetRxBusyPtr = DRV_ETHMAC_LIB_ListInit(&pMacD->mData._EnetRxBusyList);


    PLIB_ETH_InterruptClear(ethId,ETH_ALL_INTERRUPT_SOURCES);
    PLIB_ETH_TxPacketDescAddrSet(ethId,(uint8_t *)NULL);
    PLIB_ETH_RxPacketDescAddrSet(ethId,(uint8_t *)NULL);

    // leave filtering and ETHIEN as they were

    _EthMacInit(ethId);
}


/****************************************************************************
 * Function:        DRV_ETHMAC_LibClose
 *****************************************************************************/
void DRV_ETHMAC_LibClose(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_CLOSE_FLAGS cFlags)
{
    // disable Rx, Tx, Eth controller itself
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    if(cFlags&DRV_ETHMAC_CLOSE_GRACEFUL)
    {
        PLIB_ETH_TxRTSDisable(ethId);
        while (PLIB_ETH_TransmitIsBusy(ethId))
        {
            //Do Nothing
        }
        while (PLIB_ETH_ReceiveIsBusy(ethId))
        {
            //Do Nothing
        }
    }

    PLIB_ETH_TxRTSDisable(ethId);
    PLIB_ETH_RxDisable(ethId);

    _EthMacReset(ethId);

    PLIB_ETH_Disable(ethId);
    while( PLIB_ETH_EthernetIsBusy(ethId) )
    {
        //Do Nothing.
    }

    PLIB_ETH_InterruptClear(ethId,ETH_ALL_INTERRUPT_SOURCES);

}


/****************************************************************************
 * Function:        DRV_ETHMAC_LibMACOpen
 *****************************************************************************/
void DRV_ETHMAC_LibMACOpen(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)
{
    unsigned int    cfg1;
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    cfg1=_EMACxCFG1_RXENABLE_MASK|((oFlags&TCPIP_ETH_OPEN_MAC_LOOPBACK)?_EMACxCFG1_LOOPBACK_MASK:0);

    if(oFlags&TCPIP_ETH_OPEN_FDUPLEX)
    {
        if(pauseType&TCPIP_ETH_PAUSE_TYPE_EN_TX)
        {
            cfg1|=_EMACxCFG1_TXPAUSE_MASK;
        }
        if(pauseType&TCPIP_ETH_PAUSE_TYPE_EN_RX)
        {
            cfg1|=_EMACxCFG1_RXPAUSE_MASK;
        }
    }

    EMACxCFG1=cfg1;

    EMACxCFG2=_EMACxCFG2_EXCESSDFR_MASK|_EMACxCFG2_AUTOPAD_MASK|_EMACxCFG2_PADENABLE_MASK|_EMACxCFG2_CRCENABLE_MASK|
            ((oFlags&TCPIP_ETH_OPEN_HUGE_PKTS)?_EMACxCFG2_HUGEFRM_MASK:0)|_EMACxCFG2_LENGTHCK_MASK|
            ((oFlags&TCPIP_ETH_OPEN_HDUPLEX)?0:_EMACxCFG2_FULLDPLX_MASK);

    PLIB_ETH_BackToBackIPGSet(ethId,(oFlags&TCPIP_ETH_OPEN_HDUPLEX)?0x12:0x15);

    PLIB_ETH_NonBackToBackIPG1Set(ethId,0x0C);
    PLIB_ETH_NonBackToBackIPG2Set(ethId,0x12);

    PLIB_ETH_CollisionWindowSet(ethId,0x37);
    PLIB_ETH_ReTxMaxSet(ethId,0x0F);

    if(oFlags&TCPIP_ETH_OPEN_RMII)
    {
        PLIB_ETH_RMIIResetEnable(ethId);
        PLIB_ETH_RMIIResetDisable(ethId);
        PLIB_ETH_RMIISpeedSet(ethId,(oFlags&TCPIP_ETH_OPEN_100)?ETH_RMII_100Mps:ETH_RMII_10Mbps);
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_descriptors_add_lib.c
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    int DRV_ETHMAC_LibDescriptorsPoolAdd (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType,
                            DRV_ETHMAC_DCPT_AllocF fAlloc, void* fParam )

  Summary:
    Adds transmit & receive descriptors to the pool.
 *****************************************************************************/

int DRV_ETHMAC_LibDescriptorsPoolAdd (DRV_ETHMAC_INSTANCE_DCPT* pMacD, int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_AllocF fAlloc, void* fParam )
{
    DRV_ETHMAC_DCPT_NODE*  pDcpt;
    DRV_ETHMAC_DCPT_LIST    *pFList, *pBList;   // free and busy lists
    int     nCreated;

    if(fAlloc==0)
    {
        return 0;
    }
    else if(dType == DRV_ETHMAC_DCPT_TYPE_TX)
    {
        pFList=pMacD->mData._EnetTxFreePtr;
        pBList=pMacD->mData._EnetTxBusyPtr;
    }
    else if(dType == DRV_ETHMAC_DCPT_TYPE_RX)
    {
        pFList=pMacD->mData._EnetRxFreePtr;
        pBList=pMacD->mData._EnetRxBusyPtr;
    }
    else
    {
        return 0;
    }

    if(DRV_ETHMAC_LIB_ListIsEmpty(pBList))
    {   // first time we're creating descriptors for this list...
        pDcpt=(DRV_ETHMAC_DCPT_NODE*)(*fAlloc)(1, sizeof(*pDcpt), fParam);
        if(!pDcpt)
        {
            return 0;
        }
        DRV_ETHMAC_LIB_ListAddHead(pBList, pDcpt);   // the busy list must always have a dummy sw owned tail descriptor
    }

    // create the descriptors
    nCreated=0;
    while(nDescriptors--)
    {
        pDcpt=(DRV_ETHMAC_DCPT_NODE*)(*fAlloc)(1, sizeof(*pDcpt), fParam);
        if(!pDcpt)
        {
            break;
        }
        DRV_ETHMAC_LIB_ListAddTail(pFList, pDcpt);
        nCreated++;
    }

    return nCreated;
} //DRV_ETHMAC_LibDescriptorsPoolAdd


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_descriptors_remove_lib.c
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    int DRV_ETHMAC_LibDescriptorsPoolRemove (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType,
                                                 DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )

  Summary:
    Tries to remove the specified number of descriptors from the pool.
 *****************************************************************************/
int DRV_ETHMAC_LibDescriptorsPoolRemove (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )
{
    DRV_ETHMAC_DCPT_LIST*   pList;
    DRV_ETHMAC_DCPT_NODE*  pN;
    int     removed=0;

    if(dType == DRV_ETHMAC_DCPT_TYPE_TX)
    {
        pList=pMacD->mData._EnetTxFreePtr;
    }
    else if(dType == DRV_ETHMAC_DCPT_TYPE_RX)
    {
        pList=pMacD->mData._EnetRxFreePtr;
    }
    else
    {
        return 0;
    }

    while(nDescriptors--)
    {
        pN=DRV_ETHMAC_LIB_ListRemoveHead(pList);
        if(!pN)
        {
            break;
        }
        if(fFree)
        {
            (*fFree)(pN, fParam);
        }
        removed++;
    }

    return removed;

}//DRV_ETHMAC_LibDescriptorsPoolRemove


/*******************************************************************************
  Function:
    void DRV_ETHMAC_LibDescriptorsPoolCleanUp (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )

  Summary:
    Removes all the specified descriptors from the pool.
 *****************************************************************************/
void DRV_ETHMAC_LibDescriptorsPoolCleanUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )
{
    // free all allocated descriptors

    if(dType&DRV_ETHMAC_DCPT_TYPE_TX)
    {
        _EnetPoolFreeDcptList(pMacD->mData._EnetTxFreePtr, fFree, fParam);
        _EnetPoolFreeDcptList(pMacD->mData._EnetTxBusyPtr, fFree, fParam);
    }

    if(dType&DRV_ETHMAC_DCPT_TYPE_RX)
    {
        _EnetPoolFreeDcptList(pMacD->mData._EnetRxFreePtr, fFree, fParam);
        _EnetPoolFreeDcptList(pMacD->mData._EnetRxBusyPtr, fFree, fParam);
    }

} //DRV_ETHMAC_LibDescriptorsPoolCleanUp


/*******************************************************************************
  Function:
    void* DRV_ETHMAC_LibDescriptorGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *pDcpt )

  Summary:
    Returns the associated descriptor buffer.
 *****************************************************************************/
void* DRV_ETHMAC_LibDescriptorGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *pDcpt )
{
    void*       pBuff=0;
    DRV_ETHMAC_DCPT_NODE*  pEDcpt=(DRV_ETHMAC_DCPT_NODE*) pDcpt;

    if(pEDcpt->hwDcpt.pEDBuff != 0)
    {
        pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
    }

    return pBuff;

} //DRV_ETHMAC_LibDescriptorGetBuffer


/****************************************************************************
 * Function:        _EnetPoolFreeDcptList
 *
 * PreCondition:    None
 *
 * Input:           pList - list to be cleaned-up
 *                  fFree - function to return the allocated memory
 *                  fParam - function call parameter
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Helper to clean-up a list.
 *
 * Note:            None
 *****************************************************************************/
static void _EnetPoolFreeDcptList(DRV_ETHMAC_DCPT_LIST* pList, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam)
{
    DRV_ETHMAC_DCPT_NODE*  pN;

    while((pN=DRV_ETHMAC_LIB_ListRemoveHead(pList)))
    {
        if(fFree)
        {
            (*fFree)(pN, fParam);
        }
    }

}//_EnetPoolFreeDcptList


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_rx_buffers_append_lib.c
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function:        DRV_ETHMAC_LibRxBuffersAppend
 *
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxBuffersAppend(DRV_ETHMAC_INSTANCE_DCPT* pMacD, void* ppBuff[], int nBuffs, DRV_ETHMAC_BUFF_FLAGS rxFlags)
{
    void*       pBuff;
    DRV_ETHMAC_DCPT_NODE   *pEDcpt;
    DRV_ETHMAC_RESULT     res;
    ETH_MODULE_ID   ethId;
    DRV_ETHMAC_DCPT_LIST*   pNewList;
    uint8_t newList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1) + sizeof(DRV_ETHMAC_DCPT_LIST)];

    if(nBuffs==0)
    {
        nBuffs=0x7fffffff;
    }

    pNewList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(newList));

    res=DRV_ETHMAC_RES_OK;
    ethId = pMacD->mData.macConfig.ethModuleId;

    for(pBuff=*ppBuff; pBuff!=0 && nBuffs; pBuff=*(++ppBuff), nBuffs--)
    {
        pEDcpt=DRV_ETHMAC_LIB_ListRemoveHead(pMacD->mData._EnetRxFreePtr);
        if(!pEDcpt)
        {   // we've run out of descriptors...
            res=DRV_ETHMAC_RES_NO_DESCRIPTORS;
            break;
        }
        // ok valid descriptor
        // pas it to hw, always use linked descriptors
        pEDcpt->hwDcpt.pEDBuff=(unsigned char*)KVA_TO_PA(pBuff);

        pEDcpt->hwDcpt.hdr.w=_SDCPT_HDR_NPV_MASK_|_SDCPT_HDR_EOWN_MASK_;    // hw owned

        if(rxFlags&DRV_ETHMAC_BUFF_FLAG_RX_STICKY)
        {
            pEDcpt->hwDcpt.hdr.sticky=1;
        }

        if(rxFlags&DRV_ETHMAC_BUFF_FLAG_RX_UNACK)
        {
            pEDcpt->hwDcpt.hdr.rx_nack=1;
        }

        if(IS_KVA0(pBuff))
        {
            pEDcpt->hwDcpt.hdr.kv0=1;
        }
        else if(!IS_KVA(pBuff))
        {
            res=DRV_ETHMAC_RES_USPACE_ERR;
            break;
        }

        DRV_ETHMAC_LIB_ListAddTail(pNewList, pEDcpt);
    }

    if(pBuff && nBuffs)
    {   // failed, still buffers in the descriptors, put back the removed nodes
        DRV_ETHMAC_LIB_ListAppendTail(pMacD->mData._EnetRxFreePtr, pNewList);
        return res;
    }

    // all's well
    if(!DRV_ETHMAC_LIB_ListIsEmpty(pNewList))
    {
        _EthAppendBusyList(pMacD, pMacD->mData._EnetRxBusyPtr, pNewList, 1);
        if ( NULL == PLIB_ETH_RxPacketDescAddrGet(ethId) )
        {   // 1st time transmission!
            PLIB_ETH_RxPacketDescAddrSet(ethId, (uint8_t *)KVA_TO_PA(&pMacD->mData._EnetRxBusyPtr->head->hwDcpt) );
        }
        PLIB_ETH_RxEnable(ethId);  // and we're running!
    }

    return DRV_ETHMAC_RES_OK;

} //DRV_ETHMAC_LibRxBuffersAppend


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_tx_packet_lib.c
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function:        _EthTxSchedBuffer
 *
 * PreCondition:    pBuff, nBytes, pList - valid
 *
 * Input:           pBuff   - adress of buffer to be sent
 *                  nBytes  - size of the buffer in bytes
 *                  pList   - list to append to
 *
 * Output:          DRV_ETHMAC_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied buffer for transmission.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHMAC_RESULT _EthTxSchedBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, unsigned short int nBytes, DRV_ETHMAC_DCPT_LIST* pList)
{
    DRV_ETHMAC_DCPT_NODE   *pEDcpt;
    DRV_ETHMAC_RESULT     res;

    if(!IS_KVA(pBuff))
    {
        return DRV_ETHMAC_RES_USPACE_ERR;
    }

    pEDcpt=DRV_ETHMAC_LIB_ListRemoveHead(pMacD->mData._EnetTxFreePtr);

    if(pEDcpt)
    {   // ok valid descriptor
        // pass it to hw, always use linked descriptors, set proper size
        pEDcpt->hwDcpt.pEDBuff=(unsigned char*)KVA_TO_PA(pBuff);
        pEDcpt->hwDcpt.hdr.w=_SDCPT_HDR_NPV_MASK_|_SDCPT_HDR_EOWN_MASK_|(nBytes<<_SDCPT_HDR_BCOUNT_POS_);   // hw owned

        if(IS_KVA0(pBuff))
        {
            pEDcpt->hwDcpt.hdr.kv0=1;
        }
        DRV_ETHMAC_LIB_ListAddTail(pList, pEDcpt);
        res=DRV_ETHMAC_RES_OK;
    }
    else
    {   // we've run out of descriptors...
        res=DRV_ETHMAC_RES_NO_DESCRIPTORS;
    }

    return res;

} //_EthTxSchedBuffer


/****************************************************************************
 * Function:        _EthTxSchedList
 *
 * PreCondition:    pList - valid
 *
 * Input:           pBuff   - adress of buffer to be sent
 *                  nBytes  - size of the buffer in bytes
 *                  pList   - list to append to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied list for transmission.
 *
 * Note:            None
 *****************************************************************************/
static void _EthTxSchedList(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_DCPT_LIST* pList)
{
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    if(!DRV_ETHMAC_LIB_ListIsEmpty(pList))
    {
        (pList->head)->hwDcpt.hdr.SOP=1;
        (pList->tail)->hwDcpt.hdr.EOP=1;
        _EthAppendBusyList(pMacD, pMacD->mData._EnetTxBusyPtr, pList, 0);

        if ( NULL == PLIB_ETH_TxPacketDescAddrGet(ethId) )
        {   // 1st time transmission!
            PLIB_ETH_TxPacketDescAddrSet(ethId,(uint8_t *)KVA_TO_PA(&pMacD->mData._EnetTxBusyPtr->head->hwDcpt) );
        }
        PLIB_ETH_TxRTSEnable(ethId);

    }

} //_EthTxSchedList


/****************************************************************************
 * Function:        DRV_ETHMAC_LibTxSendBuffer
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, unsigned short int nBytes)
{
    DRV_ETHMAC_RESULT res;
    DRV_ETHMAC_DCPT_LIST*   pNewList;
    uint8_t newList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1) + sizeof(DRV_ETHMAC_DCPT_LIST)];

    pNewList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(newList));

    res=_EthTxSchedBuffer(pMacD, pBuff, nBytes, pNewList);
    if(res==DRV_ETHMAC_RES_OK)
    {
        _EthTxSchedList(pMacD, pNewList);
    }
    else if(!DRV_ETHMAC_LIB_ListIsEmpty(pNewList))
    {   // we failed, put back the removed nodes
        DRV_ETHMAC_LIB_ListAppendTail(pMacD->mData._EnetTxFreePtr, pNewList);
    }

    return res;

} //DRV_ETHMAC_LibTxSendBuffer


/****************************************************************************
 * Function:        DRV_ETHMAC_LibTxSendPacket
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt)
{
    DRV_ETHMAC_RESULT res;
    DRV_ETHMAC_DCPT_LIST*   pNewList;
    uint8_t newList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1) + sizeof(DRV_ETHMAC_DCPT_LIST)];

    pNewList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(newList));

    res=DRV_ETHMAC_RES_OK;
    while(pPkt!=0 && pPkt->pBuff!=0 && pPkt->nBytes!=0 && res==DRV_ETHMAC_RES_OK)
    {

        res=_EthTxSchedBuffer(pMacD, pPkt->pBuff, pPkt->nBytes, pNewList);
        pPkt=pPkt->next;    // next buffer in packet
    }


    if(res==DRV_ETHMAC_RES_OK)
    {   // all's well
        _EthTxSchedList(pMacD, pNewList);
    }
    else if(!DRV_ETHMAC_LIB_ListIsEmpty(pNewList))
    {   // we failed, put back the removed nodes
        DRV_ETHMAC_LIB_ListAppendTail(pMacD->mData._EnetTxFreePtr, pNewList);
    }

    return res;

} //DRV_ETHMAC_LibTxSendPacket


/****************************************************************************
 * Function:        DRV_ETHMAC_LibTxGetBufferStatus
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxGetBufferStatus(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, const DRV_ETHMAC_PKT_STAT_TX** pTxStat)
{
    DRV_ETHMAC_DCPT_NODE   *pHead;
    DRV_ETHMAC_RESULT     res;

    if(pTxStat)
    {
        *pTxStat=0;
    }

    if((pHead=_EnetFindPacket(pBuff, pMacD->mData._EnetTxBusyPtr)))
    {
        if(pHead->hwDcpt.hdr.EOWN)
        {
            res=DRV_ETHMAC_RES_PACKET_QUEUED;  // not done
        }
        else
        {   // that's it, got it! The 1st descriptor to be updated by the hw is the packet header!
            if(pTxStat)
            {
                *pTxStat=(const DRV_ETHMAC_PKT_STAT_TX*)&((DRV_ETHMAC_DCPT_NODE_TX*)pHead)->hwDcpt.stat;
            }
            res=DRV_ETHMAC_RES_OK;
        }
    }
    else
    {
        res=DRV_ETHMAC_RES_NO_PACKET;
    }

    return res;

} //DRV_ETHMAC_LibTxGetBufferStatus


/****************************************************************************
 * Function:        _EnetFindPacket
 *
 * PreCondition:    None
 *
 * Input:           pBuff   - packet to search
 *                  pList   - list to look into
 *
 * Output:          packet pointer or 0
 *
 * Side Effects:    None
 *
 * Overview:        This function is a helper to find a specific packet in a list.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHMAC_DCPT_NODE* _EnetFindPacket(const void* pBuff, DRV_ETHMAC_DCPT_LIST* pList)
{
    DRV_ETHMAC_DCPT_NODE*  pEDcpt;
    const void* pPhysPkt;

    pPhysPkt=(const void*)KVA_TO_PA(pBuff);

    for(pEDcpt=pList->head; pEDcpt!=0; pEDcpt=pEDcpt->next)
    {
        if(pEDcpt->hwDcpt.hdr.SOP)
        { // found the beg of the packet
            if(pPhysPkt==pEDcpt->hwDcpt.pEDBuff)
            {   // ok, found it
                break;
            }
        }
    }

    return pEDcpt;

} //_EnetFindPacket


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_ack_packet_lib.c
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function:        _EthAckPacket
 *
 * PreCondition:    None
 *
 * Input:           pPkt       - buffer/packet to be acknowledged or NULL
 *                  pRemList   - list to look for done packets and to remove the packets from
 *                  pAddList   - list were to add the removed packets
 *                  ackFnc     - function to be called for each acknowledged buffer in turn
 *                  fParam     - function argument
 *
 * Output:          DRV_ETHMAC_RES_OK - success
 *                  DRV_ETHMAC_RES_PACKET_QUEUED - there are packets queued
 *                  DRV_ETHMAC_RES_NO_PACKET - no packets available in the queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a packet.
 *                  The supplied packet has to have been completed otherwise the call will fail.
 *                  When pPkt==NULL, all packets with EOWN==0 will be acknowledged.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHMAC_RESULT _EthAckPacket(const void* pPkt, DRV_ETHMAC_DCPT_LIST* pRemList, DRV_ETHMAC_DCPT_LIST* pAddList, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam)
{

    DRV_ETHMAC_DCPT_NODE   *pEDcpt;
    DRV_ETHMAC_DCPT_NODE   *prev, *next;
    int     nAcks;
    int     pktFound;
    int     buffIx;

    prev=next=0;
    nAcks=pktFound=0;

    for(pEDcpt=pRemList->head; pEDcpt!=0; pEDcpt=next)
    {
        if(pEDcpt->hwDcpt.hdr.SOP && (pPkt==0 || pEDcpt->hwDcpt.pEDBuff==(unsigned char*)KVA_TO_PA(pPkt)))
        { // found the beg of a packet
            pktFound=1;

            if(pEDcpt->hwDcpt.hdr.EOWN)
            {
                break;      // hw not done with it
            }

            next=pEDcpt;
            buffIx=0;
            do
            {
                pEDcpt=next;
                next=pEDcpt->next;
                while(pEDcpt->hwDcpt.hdr.EOWN);         // shouldn't happen
                DRV_ETHMAC_LIB_ListAddTail(pAddList, pEDcpt);    // ack this node
                if(ackFnc)
                {
                    void* pBuff;
                    pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
                    (*ackFnc)(pBuff, buffIx++, fParam); // call user's acknowledge
                }

            }while(!pEDcpt->hwDcpt.hdr.EOP);

            nAcks++;
            // reconstruct the removed list
            if(prev)
            {
                prev->next=next;
                // prev->next_ed shouldn't matter here!
            }
            else
            {
                pRemList->head=next;
            }

            if(pPkt)
            {   // done, just one packet ack-ed
                break;  // done
            }
        }
        else
        {
            prev=pEDcpt;
            next=pEDcpt->next;
        }
    }

    return nAcks?DRV_ETHMAC_RES_OK:(pktFound?DRV_ETHMAC_RES_PACKET_QUEUED:DRV_ETHMAC_RES_NO_PACKET);

} //_EthAckPacket


/****************************************************************************
 * Function:        _EthRxAckBuffer
 *
 * PreCondition:    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend, DRV_ETHMAC_LibRxGetPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL
 *                  ackFnc - function to be called for the acknowledged buffers or NULL
 *                  fParam - argument to be used in ackFnc callback
 *
 * Output:          DRV_ETHMAC_RES_OK - success
 *                  DRV_ETHMAC_RES_PACKET_QUEUED - there are packets in the receiving queue
 *                  DRV_ETHMAC_RES_NO_PACKET - no packets available in the receiving queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a received buffer/packet.
 *                  The supplied packet has to have been previously received otherwise the call will fail or the packet will be discarded.
 *                  When pBuff==NULL, all currently received packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any received packet has to be acknowledged, otherwise the Eth API will run out of descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to call an acknowledge function for each received buffer in turn.
 *****************************************************************************/
static DRV_ETHMAC_RESULT _EthRxAckBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam)
{
    DRV_ETHMAC_RESULT     res;
    DRV_ETHMAC_DCPT_NODE*  pEDcpt;
    ETH_MODULE_ID   ethId;
    DRV_ETHMAC_DCPT_LIST*   pAckList;
    DRV_ETHMAC_DCPT_LIST*   pStickyList;
    uint8_t ackList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1) + sizeof(DRV_ETHMAC_DCPT_LIST)];
    uint8_t stickyList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1)+ sizeof(DRV_ETHMAC_DCPT_LIST)];
    
    ethId = pMacD->mData.macConfig.ethModuleId;


    pAckList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(ackList));
    pStickyList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(stickyList));


    res=_EthAckPacket(pBuff, pMacD->mData._EnetRxBusyPtr, pAckList, ackFnc, fParam);

    while((pEDcpt=DRV_ETHMAC_LIB_ListRemoveHead(pAckList)))
    {
        if(pEDcpt->hwDcpt.hdr.sticky)
        {
            // add it to the busy list...
            pEDcpt->hwDcpt.hdr.SOP=pEDcpt->hwDcpt.hdr.EOP=pEDcpt->hwDcpt.hdr.rx_wack=0;
            pEDcpt->hwDcpt.hdr.EOWN=1;  // hw owned
            DRV_ETHMAC_LIB_ListAddTail(pStickyList, pEDcpt);
        }
        else
        {
            DRV_ETHMAC_LIB_ListAddTail(pMacD->mData._EnetRxFreePtr, pEDcpt);
            pEDcpt->hwDcpt.pEDBuff = 0; // corresponding buffer is not owned
            if(!pEDcpt->hwDcpt.hdr.rx_nack)
            {
                PLIB_ETH_RxBufferCountDecrement(ethId);
            }
        }
    }

    if(!DRV_ETHMAC_LIB_ListIsEmpty(pStickyList))
    {
        _EthAppendBusyList(pMacD, pMacD->mData._EnetRxBusyPtr, pStickyList, 1);    // append the descriptors that have valid buffers
    }

    return res;

} //_EthRxAckBuffer


/****************************************************************************
 * Function:        DRV_ETHMAC_LibRxAcknowledgeBuffer
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxAcknowledgeBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam)
{
    return _EthRxAckBuffer(pMacD, pBuff, ackFnc, fParam);
}


/****************************************************************************
 * Function:        EthTxAcknowledgeBuffer
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxAcknowledgeBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const void* pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam)
{
    DRV_ETHMAC_RESULT     res;
    DRV_ETHMAC_DCPT_NODE*  pEDcpt;
    DRV_ETHMAC_DCPT_LIST*   pAckList;
    uint8_t ackList [(DRV_ETHMAC_DCPT_LIST_ALIGN -1) + sizeof(DRV_ETHMAC_DCPT_LIST)];
    
    pAckList = DRV_ETHMAC_LIB_ListInit(_EthAlignAdjust(ackList));
    
    res = _EthAckPacket(pBuff, pMacD->mData._EnetTxBusyPtr, pAckList, ackFnc, fParam);

    while((pEDcpt=DRV_ETHMAC_LIB_ListRemoveHead(pAckList)))
    {
        pEDcpt->hwDcpt.pEDBuff = 0; // corresponding buffer is not owned
        DRV_ETHMAC_LIB_ListAddTail(pMacD->mData._EnetTxFreePtr, pEDcpt);
    }

    return res;
}


// *****************************************************************************
// *****************************************************************************
// Section: Code from eth_rx_get_packet_lib.c
// *****************************************************************************
// *****************************************************************************

/****************************************************************************
 * Function:        DRV_ETHMAC_LibRxGetBuffer
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetBuffer(DRV_ETHMAC_INSTANCE_DCPT* pMacD, void** ppBuff, const DRV_ETHMAC_PKT_STAT_RX** pRxStat)
{

    DRV_ETHMAC_RESULT         res;
    DRV_ETHMAC_PKT_DCPT     pktDcpt;
    int             nBuffs;     // buffers per packet

    pktDcpt.next=0;     // create a single buffer packet descriptor;

    res=DRV_ETHMAC_LibRxGetPacket(pMacD, &pktDcpt, &nBuffs, pRxStat);

    *ppBuff=pktDcpt.pBuff;

    return res;

} //DRV_ETHMAC_LibRxGetBuffer


/****************************************************************************
 * Function:        DRV_ETHMAC_LibRxGetPacket
 *****************************************************************************/
DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_PKT_DCPT* pPkt, int* pnBuffs, const DRV_ETHMAC_PKT_STAT_RX** pRxStat)
{
    DRV_ETHMAC_DCPT_NODE   *pEDcpt, *pHead;
    DRV_ETHMAC_RESULT     res;

    res=DRV_ETHMAC_RES_NO_PACKET;

    if(pPkt)
    {
        pPkt->pBuff=0;
        pPkt->nBytes=0;
    }

    for(pEDcpt=pMacD->mData._EnetRxBusyPtr->head; pEDcpt!=0; pEDcpt=pEDcpt->next)
    {
        if(pEDcpt->hwDcpt.hdr.EOWN)
        {
            res=DRV_ETHMAC_RES_PACKET_QUEUED;
            break;      // not done
        }
        else if(pEDcpt->hwDcpt.hdr.SOP && pEDcpt->hwDcpt.hdr.rx_wack==0)
        { // found the beg of a packet
            DRV_ETHMAC_PKT_DCPT*    pBuffDcpt;
            int     nBuffs, reportBuffs;

            pHead=pEDcpt;
            nBuffs=reportBuffs=0;
            pBuffDcpt=pPkt;
            res=DRV_ETHMAC_RES_OK;

            if(pRxStat)
            {
                *pRxStat=(const DRV_ETHMAC_PKT_STAT_RX*)&((DRV_ETHMAC_DCPT_NODE_RX*)pEDcpt)->hwDcpt.stat;
            }

            while(pBuffDcpt || pnBuffs)
            {   // either way, we have to parse the packet
                if(pBuffDcpt)
                {
                    pBuffDcpt->pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
                    pBuffDcpt->nBytes=pEDcpt->hwDcpt.hdr.bCount;
                    pBuffDcpt=pBuffDcpt->next;
                    reportBuffs++;
                }
                nBuffs++;

                while(pEDcpt->hwDcpt.hdr.EOWN);     // shouldn't happen
                if(pEDcpt->hwDcpt.hdr.EOP)
                {   // end of packet
                    if(pnBuffs)
                    {
                        *pnBuffs=nBuffs;
                    }

                    if(pBuffDcpt)
                    {
                        pBuffDcpt->pBuff=0; // end it properly
                        pBuffDcpt->nBytes=0;
                    }

                    if(pPkt)
                    {
                        if(reportBuffs!=nBuffs)
                        {
                            res=DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR;
                        }
                        else
                        {
                            pHead->hwDcpt.hdr.rx_wack=1;        // reported ok, waiting to be acknowledged
                        }
                    }
                    break;
                }
                pEDcpt=pEDcpt->next;
            }

            break;
        }
    }

    return res;

} //DRV_ETHMAC_LibRxGetPacket

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)
{
    if(pnBuffs)
    {
        *pnBuffs = _EnetDescriptorsCount(pMacD->mData._EnetRxBusyPtr, false);
    }

    return DRV_ETHMAC_RES_OK; 

}

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxScheduledBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)
{
    if(pnBuffs)
    {
        *pnBuffs = _EnetDescriptorsCount(pMacD->mData._EnetRxBusyPtr, true);
    }

    return DRV_ETHMAC_RES_OK; 

}


DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)
{
    if(pnBuffs)
    {
        *pnBuffs = _EnetDescriptorsCount(pMacD->mData._EnetTxBusyPtr, false);
    }

    return DRV_ETHMAC_RES_OK; 

}

static int _EnetDescriptorsCount(DRV_ETHMAC_DCPT_LIST* pList, bool isHwCtrl)
{
	DRV_ETHMAC_DCPT_NODE	*pEDcpt;
	int	 	nDcpts=0;
	
	
	for(pEDcpt=pList->head; pEDcpt!=0 && pEDcpt->next!=0; pEDcpt=pEDcpt->next)
	{	// don't count the ending dummy descriptor 
		if(pEDcpt->hwDcpt.hdr.EOWN == isHwCtrl)
		{
			nDcpts++;
		}
	}
	
	return nDcpts;	
}

/////  generic single linked lists manipulation ///////////
//

// removes the head node
DRV_ETHMAC_SGL_LIST_NODE*  DRV_ETHMAC_SingleListHeadRemove(DRV_ETHMAC_SGL_LIST* pL)
{
	DRV_ETHMAC_SGL_LIST_NODE* pN = pL->head;
    if(pN)
    {
        if(pL->head == pL->tail)
        {
            pL->head = pL->tail = 0;
        }
        else
        {
            pL->head = pN->next;
        }
        pL->nNodes--;
    }

	return pN;
}

// adds node to tail
void  DRV_ETHMAC_SingleListTailAdd(DRV_ETHMAC_SGL_LIST* pL, DRV_ETHMAC_SGL_LIST_NODE* pN)
{
	pN->next = 0;
	if(pL->tail == 0)
	{
		pL->head = pL->tail = pN;
	}
	else
	{
		pL->tail->next = pN;
		pL->tail = pN;
	}
    pL->nNodes++;
}



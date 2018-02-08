/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/


#define _MAILBOX_C_

#include "mailbox.h"

/******************************************************************************
 * Function Prototype Declaration
 ******************************************************************************/
static PRTL_MAILBOX RtlMBoxIdToHdl(IN u8 MBoxId);

PRTL_MAILBOX RtlMailboxCreate(IN u8 MboxID, IN u32 MboxSize, IN _Sema *pWakeSema);

VOID RtlMailboxDel(IN PRTL_MAILBOX MboxHdl);

u8 RtlMailboxSendToBack(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

u8 RtlMailboxSendToFront(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

u8 RtlMailboxReceive(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

u8 RtlMailboxPeek(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

u32 RtlMailboxMsgWaiting(
    IN u8 MboxID, 
    IN u8 IsFromISR
);

/******************************************************************************
 * Global Variable Declaration
 ******************************************************************************/
static RTL_MBOX_ROOT MBox_Entry;

/******************************************************************************
 * External Function & Variable Declaration
 ******************************************************************************/


/******************************************************************************
 * Function: RtlMBoxIdToHdl
 * Desc: Map a mailbox ID to the mailbox pointer.
 * Para:
 *	MBoxId: The Mailbox ID
 * Return: The pointer of the mailbox. If didn't found match mailbox, 
 *			return NULL.
 *
 ******************************************************************************/
static PRTL_MAILBOX RtlMBoxIdToHdl(
	IN u8 MBoxId
)
{
	RTL_MAILBOX *pMbox=NULL;
	RTL_MAILBOX *pTmpMbox;
	_LIST *pHead;
	_LIST *pList;

    // if the Mailbox root entry initialed ? if not, initial it
    if (!MBox_Entry.isInitialed) {
        RtlMutexInit(&MBox_Entry.Mutex);   // Init the Mutex for the mailbox add/delete procedure protection
        RtlInitListhead(&MBox_Entry.mbox_list);    // Init the link list head to chain all created mailbox
        MBox_Entry.isInitialed = 1;
        MSG_MBOX_INFO("MBox Entry Initial...\n");
    }

	pHead = &MBox_Entry.mbox_list;
	RtlDownMutex(&MBox_Entry.Mutex);
	pList = RtlListGetNext(&MBox_Entry.mbox_list);
	while (pList != pHead) {
		pTmpMbox = CONTAINER_OF(pList, RTL_MAILBOX, mbox_list);		
		if (MBoxId == pTmpMbox->mbox_id) {
			pMbox = pTmpMbox;
			break;
		}
		pList = RtlListGetNext(pList);
	}
	RtlUpMutex(&MBox_Entry.Mutex);

	return pMbox;
}

/******************************************************************************
 * Function: RtlMailboxCreate
 * Desc: To create a mailbox with a given mailbox ID and size
 * Para:
 * 	MboxID: A number to identify this created mailbox. A message block can 
 *          be send to a mailbox by a given MboxID. The MboxID must be unique 
 *          in the whole system. If this MboxID is conflict with a created 
 *          mailbox, the mailbox creation will fail and return NULL.
 *  MboxSize: The size of this mailbox to be created. It means maximum number 
 *          of message blocks can be stored in this mailbox.
 *  pWakeSema: The semaphore to wake up the receiving task to receive the new 
 *          message. If the receiving task doesn't need a semaphore to wakeup 
 *          it, then just let this pointer is NULL.
 * Return: The created mailbox pointer. If it failed, return NULL.
 ******************************************************************************/
PRTL_MAILBOX RtlMailboxCreate(
    IN u8 MboxID, 
    IN u32 MboxSize, 
    IN _Sema *pWakeSema
)
{
	PRTL_MAILBOX pMBox=NULL;

    // if the Mailbox root entry initialed ? if not, initial it
    if (!MBox_Entry.isInitialed) {
        RtlMutexInit(&MBox_Entry.Mutex);   // Init the Mutex for the mailbox add/delete procedure protection
        RtlInitListhead(&MBox_Entry.mbox_list);    // Init the link list head to chain all created mailbox
        MBox_Entry.isInitialed = 1;
        MSG_MBOX_INFO("MBox Entry Initial...\n");
    }
    
	// check if this mailbox ID is ocupied ?
	pMBox = RtlMBoxIdToHdl(MboxID);
	if (NULL != pMBox) {
		MSG_MBOX_ERR("RtlMailboxCreate: The Mailbox ID %d is used by someone!!\n", MboxID);
		return NULL;
	}

	pMBox = (RTL_MAILBOX *)RtlZmalloc(sizeof(RTL_MAILBOX));
	if (NULL==pMBox) {
		MSG_MBOX_ERR("RtlMailboxCreate: MAlloc Failed\n");
		return NULL;
	}

	RtlInitListhead(&pMBox->mbox_list);	// Init the link list to be chained into the created mailbox list
	pMBox->mbox_id = MboxID;
	pMBox->pWakeSema = pWakeSema;
#ifdef PLATFORM_FREERTOS
    pMBox->mbox_hdl = xQueueCreate(MboxSize, sizeof(MSG_BLK));
    if (NULL == pMBox->mbox_hdl) {
		MSG_MBOX_ERR("RtlMailboxCreate: xQueueCreate Failed\n");
        RtlMfree((void *)pMBox, sizeof(RTL_MAILBOX));        
        return NULL;
    }
#endif
#ifdef PLATFORM_ECOS
// TODO: Create mailbox
#endif

	// Add this mailbox to the link list of created mailbox
	RtlDownMutex(&MBox_Entry.Mutex);
	RtlListInsertTail(&pMBox->mbox_list, &MBox_Entry.mbox_list);
	RtlUpMutex(&MBox_Entry.Mutex);

    MSG_MBOX_INFO("A Mailbox Created: Size=%d\n", MboxSize);

	return pMBox;
}

/******************************************************************************
 * Function: RtlMailboxDel
 * Desc: To delete a mailbox by a given mailbox handle.
 * Para:
 *	MboxHdl: The handle of the mailbox to be deleted.
 * Return: None.
 ******************************************************************************/
VOID RtlMailboxDel(
    IN PRTL_MAILBOX MboxHdl
)
{
	if (NULL == MboxHdl) {
		MSG_MBOX_ERR("RtlMailboxDel: Try to delete a NULL mailbox\n");
		return;
	}

	// Remove this mailbox from the link list of created mailbox
	RtlDownMutex(&MBox_Entry.Mutex);
	RtlListDelete(&MboxHdl->mbox_list);
	RtlUpMutex(&MBox_Entry.Mutex);

    // delete the Queue/Mailbox
#ifdef PLATFORM_FREERTOS
    vQueueDelete((xQueueHandle)(MboxHdl->mbox_hdl));
#endif
#ifdef PLATFORM_ECOS
    // TODO: Delete mailbox
#endif

	RtlMfree((void *)MboxHdl, sizeof(RTL_MAILBOX));
}

/******************************************************************************
 * Function: RtlMailboxSendToBack
 * Desc: To put a message block to the tail of a given mailbox.
 * Para:
 *  MboxID: The identifier of the target mailbox.
 *  pMsg: The pointer of the message block to be put into the mailbox.
 *  MSToWait: If the mailbox is full, this value gives a time to wait to put 
 *            this message. The time unit is millisecond. 
 *            The special values are: 
 *               0: no waiting; 
 *               0xffffffff: wait without timeout. 
 *            If the waiting is timeout, the message sending is failed and 
 *            return _FAIL.
 *  IsFromISR: Is this function is called from an ISR ?
 * Return: _SUCCESS or _FAIL.
 ******************************************************************************/
u8 RtlMailboxSendToBack(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
)
{
    RTL_MAILBOX *pMbox=NULL;
    u32 wait_ticks;
#ifdef PLATFORM_FREERTOS
    portBASE_TYPE ret;
#endif

    pMbox = RtlMBoxIdToHdl(MboxID);

    if (NULL == pMbox) {
   	    MSG_MBOX_ERR("RtlMailboxSendToBack: Didn't find matched MBoxID=%d\n", MboxID);
        return _FAIL;
    }

#ifdef PLATFORM_FREERTOS
    if (MBOX_WAIT_NO_TIMEOUT == MSToWait) {
        wait_ticks = portMAX_DELAY;
    }
    else if (MBOX_WAIT_NONE == MSToWait) {
        wait_ticks = 0;
    }
    else {
        wait_ticks = ((MSToWait/portTICK_RATE_MS)>0)?(MSToWait/portTICK_RATE_MS):(1);
    }

    if (IsFromISR) {
        ret = xQueueSendToBackFromISR(pMbox->mbox_hdl, (void *)pMsg, NULL);//(portTickType) wait_ticks);
    }
    else {
        ret = xQueueSendToBack(pMbox->mbox_hdl, (void *)pMsg, (portTickType) wait_ticks);
    }
    
    if(ret != pdPASS ) {
        // send message to the queue failed
   	    MSG_MBOX_ERR("RtlMailboxSendToBack: Put Msg to Queue Failed, MBoxID=%d\n", MboxID);
        ret = _FAIL;
    }
    else {
        // try to give a semaphore to wake up the receiving task
        if (pMbox->pWakeSema) {
            RtlUpSema(pMbox->pWakeSema);  
        }
        ret = _SUCCESS;
    }

    return ret;
#endif

#ifdef PLATFORM_ECOS
    // TODO: Put the message to a mailbox
#endif

}


/******************************************************************************
 * Function: RtlMailboxSendToFront
 * Desc: To put a message block to the head of a mailbox.
 * Para:
 *  MboxID: The identifier of the target mailbox.
 *  pMsg: The pointer of the message block to be put into the mailbox.
 *  MSToWait: If the mailbox is full, this value gives a time to wait to put 
 *            this message. The time unit is millisecond. 
 *            The special values are: 
 *               0: no waiting; 
 *               0xffffffff: wait without timeout. 
 *            If the waiting is timeout, the message sending is failed and 
 *            return _FAIL.
 *  IsFromISR: Is this function is called from an ISR ?
 * Return: _SUCCESS or _FAIL.
 ******************************************************************************/
u8 RtlMailboxSendToFront(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
)
{
    RTL_MAILBOX *pMbox=NULL;
    u32 wait_ticks;
#ifdef PLATFORM_FREERTOS
    portBASE_TYPE ret;
#endif

    pMbox = RtlMBoxIdToHdl(MboxID);

    if (NULL == pMbox) {
   	    MSG_MBOX_ERR("RtlMailboxSendToBack: Didn't find matched MBoxID=%d\n", MboxID);
        return _FAIL;
    }

#ifdef PLATFORM_FREERTOS
    if (MBOX_WAIT_NO_TIMEOUT == MSToWait) {
        wait_ticks = portMAX_DELAY;
    }
    else if (MBOX_WAIT_NONE == MSToWait) {
        wait_ticks = 0;
    }
    else {
        wait_ticks = ((MSToWait/portTICK_RATE_MS)>0)?(MSToWait/portTICK_RATE_MS):(1);
    }

    if (IsFromISR) {
        ret = xQueueSendToFrontFromISR(pMbox->mbox_hdl, (void *)pMsg, NULL);//(portTickType) wait_ticks);
    }
    else {
        ret = xQueueSendToFront(pMbox->mbox_hdl, (void *)pMsg, (portTickType) wait_ticks);
    }
    
    if(ret != pdPASS ) {
        // send message to the queue failed
   	    MSG_MBOX_ERR("RtlMailboxSendToBack: Put Msg to Queue Failed, MBoxID=%d\n", MboxID);
        ret = _FAIL;
    }
    else {
        // try to give a semaphore to wake up the receiving task
        if (pMbox->pWakeSema) {
            RtlUpSema(pMbox->pWakeSema);  
        }
        ret = _SUCCESS;
    }

    return ret;
#endif

#ifdef PLATFORM_ECOS
    // TODO: eCos has no API to put message to the head of a mailbox
#endif

}

/******************************************************************************
 * Function: RtlMailboxSendToFront
 * Desc: To get a message block from a given mailbox.
 * Para:
 *  MboxID: The identifier of the target mailbox.
 *  pMsg: The message block to store the gotten message.
 *  MSToWait: If the mailbox is full, this value gives a time to wait to put 
 *            this message. The time unit is millisecond. 
 *            The special values are: 
 *               0: no waiting; 
 *               0xffffffff: wait without timeout. 
 *            If the waiting is timeout, the message sending is failed and 
 *            return _FAIL.
 *  IsFromISR: Is this function is called from an ISR ?
 * Return: _SUCCESS or _FAIL.
 ******************************************************************************/
u8 RtlMailboxReceive(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
)
{
	RTL_MAILBOX *pMbox=NULL;
    u32 wait_ticks;
#ifdef PLATFORM_FREERTOS
    portBASE_TYPE ret;
#endif

	pMbox = RtlMBoxIdToHdl(MboxID);

	if (NULL == pMbox) {
		MSG_MBOX_ERR("RtlMailboxReceive: Didn't find the MBox with ID=%d\n", MboxID);
		return _FAIL;
	}

#ifdef PLATFORM_FREERTOS
    if (MBOX_WAIT_NONE == MSToWait) {
        wait_ticks = 0;
    }
    else if (MBOX_WAIT_NO_TIMEOUT == MSToWait) {
        wait_ticks = portMAX_DELAY;
    }
    else {
        wait_ticks = ((MSToWait/portTICK_RATE_MS)>0)?(MSToWait/portTICK_RATE_MS):(1);
    }

    if (IsFromISR) {
        ret = xQueueReceiveFromISR(pMbox->mbox_hdl, (void *)pMsg, NULL);//( portTickType ) wait_ticks);
    }
    else {
        ret = xQueueReceive(pMbox->mbox_hdl, (void *)pMsg, ( portTickType ) wait_ticks);

    }
    
    if(ret != pdTRUE ) {
        // receive message failed
        if (0 != MSToWait) {
            MSG_MBOX_ERR("RtlMailboxReceive: Receive Msg Failed, MBoxID=%d\n", MboxID);
        }
        ret = _FAIL;
    }
    else {
        ret = _SUCCESS;
    }

    return ret;
#endif

#ifdef PLATFORM_ECOS
    // TODO: Get a message from the mailbox
#endif

}

/******************************************************************************
 * Function: RtlMailboxPeek
 * Desc: To copy the head message from a given mailbox without move this 
 *       message block out from the mailbox.
 * Para:
 *  MboxID: The identifier of the target mailbox.
 *  pMsg: The message block to store the gotten message.
 *  MSToWait: If the mailbox is full, this value gives a time to wait to put 
 *            this message. The time unit is millisecond. 
 *            The special values are: 
 *               0: no waiting; 
 *               0xffffffff: wait without timeout. 
 *            If the waiting is timeout, the message sending is failed and 
 *            return _FAIL.
 *  IsFromISR: Is this function is called from an ISR ?
 * Return: _SUCCESS or _FAIL.
 ******************************************************************************/
u8 RtlMailboxPeek(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
)
{
	RTL_MAILBOX *pMbox=NULL;
    u32 wait_ticks;
#ifdef PLATFORM_FREERTOS
    portBASE_TYPE ret;
#endif

	pMbox = RtlMBoxIdToHdl(MboxID);

	if (NULL == pMbox) {
		MSG_MBOX_ERR("RtlMailboxPeek: Didn't find the MBox with ID=%d\n", MboxID);
		return _FAIL;
	}

#ifdef PLATFORM_FREERTOS
    if (MBOX_WAIT_NONE == MSToWait) {
        wait_ticks = 0;
    }
    else if (MBOX_WAIT_NO_TIMEOUT == MSToWait) {
        wait_ticks = portMAX_DELAY;
    }
    else {
        wait_ticks = ((MSToWait/portTICK_RATE_MS)>0)?(MSToWait/portTICK_RATE_MS):(1);
    }

    if (IsFromISR) {
//        ret = xQueuePeekFromISR(pMbox->mbox_hdl, (void *)pMsg, ( portTickType ) wait_ticks);
        // TODO: check why we have no "xQueuePeekFromISR"
        MSG_MBOX_ERR("RtlMailboxPeek: Current version has no 'xQueuePeekFromISR'\n");
        ret = pdFALSE;
    }
    else {
        ret = xQueuePeek(pMbox->mbox_hdl, (void *)pMsg, ( portTickType ) wait_ticks);

    }
    
    if(ret != pdTRUE ) {
        // receive message failed
        MSG_MBOX_ERR("RtlMailboxReceive: Receive Msg Failed, MBoxID=%d\n", MboxID);
        ret = _FAIL;
    }
    else {
        ret = _SUCCESS;
    }

    return ret;
#endif

#ifdef PLATFORM_ECOS
    // TODO: Get a message from the mailbox
#endif

}


/******************************************************************************
 * Function: RtlMailboxMsgWaiting
 * Desc: To get the number of message blocks are storing in a given mailbox.
 * Para:
 *  MboxID: The identifier of the target mailbox.
 *  IsFromISR: Is this function is called from an ISR ?
 * Return: The number of message blocks are storing in this mailbox.
 ******************************************************************************/
u32 RtlMailboxMsgWaiting(
    IN u8 MboxID, 
    IN u8 IsFromISR
)
{
	RTL_MAILBOX *pMbox=NULL;
    u32 msg_num=0;

	pMbox = RtlMBoxIdToHdl(MboxID);

	if (NULL == pMbox) {
		MSG_MBOX_ERR("RtlMailboxMsgWaiting: Didn't find the MBox with ID=%d\n", MboxID);
		return 0;
	}

#ifdef PLATFORM_FREERTOS
    if (IsFromISR) {
        msg_num = uxQueueMessagesWaitingFromISR(pMbox->mbox_hdl);
    }
    else {
        msg_num = uxQueueMessagesWaiting(pMbox->mbox_hdl);
    }
#endif

#ifdef PLATFORM_ECOS
    // TODO: call eCos API to implement this function
#endif

    return msg_num;

}


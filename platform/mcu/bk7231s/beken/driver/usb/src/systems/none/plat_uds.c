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
 * No-OS implementation of a system for Controller Driver
 * $Revision: 1.17 $
 */
#include "include.h"

#include "mu_none.h"
#include "mu_stat.h"
#include "mu_mem.h"
#include "mu_impl.h"
#include "plat_cnf.h"
#include "board.h"
#include "usb.h"

#if CFG_USB
#define SYSTEM_QUEUE_DATA_LEN                640

/***************************** TYPES ******************************/
#define QUEUE_EMPTY_STATUS            0
#define QUEUE_ENS_STATUS              1
#define QUEUE_FULL_STATUS             2

typedef struct _MGC_NoneSystem
{
    MUSB_Controller *pController;
    void *pBoardPrivateData;

    uint16_t wNextQueueItem;
    uint16_t wNextDequeueItem;
    uint8_t *pQueueData;
    uint8_t bQueueStatus;

    uint8_t bBsrRequest;
    uint8_t bBsrOver;
    uint8_t bBsrRan;
    MUSB_SystemServices Services;
} MGC_NoneSystem;

/**************************** GLOBALS *****************************/
static MGC_NoneSystem *MGC_apNoneSystem;
static MUSB_SystemUtils MGC_gNoneUtils =
{
    MUSB_SYSTEM_UTILS_VERSION,
    MUSB_BoardMessageString,
    MUSB_BoardMessageNumber,
    MUSB_BoardGetTime
};

extern void MUSB_Start_Fun(void *pParam);
extern void MUSB_HfiDeviceRemoved(void);
extern void MUSB_Disconn_Fun(void *pParam);

/*************************** FUNCTIONS ****************************/
/*
* Controller calls this to enqueue a background item
*/
static uint8_t MGC_NoneQueueBackgroundItem(void *pPrivate, const void *pItem)
{
    uint8_t *pQueueItem;
    MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivate;

    if(QUEUE_FULL_STATUS == pSystem->bQueueStatus)
    {
        MUSB_PRT("mgc_queue_full:%x\r\n", pSystem->pController->wQueueLength);
        return FALSE;
    }

    pQueueItem = pSystem->pQueueData +
                 (pSystem->pController->wQueueItemSize * pSystem->wNextQueueItem++);

    MUSB_MemCopy(pQueueItem, pItem, pSystem->pController->wQueueItemSize);
    if(pSystem->wNextQueueItem >= pSystem->pController->wQueueLength)
    {
        pSystem->wNextQueueItem = 0;
    }

    if(pSystem->wNextDequeueItem == pSystem->wNextQueueItem)
    {
        pSystem->bQueueStatus = QUEUE_FULL_STATUS;
    }
    else
    {
        pSystem->bQueueStatus = QUEUE_ENS_STATUS;
    }

    USB_NPRT("usb_event_post:%02x\r\n", ((MGC_BsrItem *)pItem)->bCause);

    usb_event_post();

    return TRUE;
}

/*
* Controller calls this to dequeue a background item
*/
static uint8_t MGC_NoneDequeueBackgroundItem(void *pPrivate, void *pItem)
{
    MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivate;
    uint8_t *pQueueItem = NULL;

    if(QUEUE_EMPTY_STATUS == pSystem->bQueueStatus)
    {
        return FALSE;
    }

    pQueueItem = pSystem->pQueueData +
                 (pSystem->pController->wQueueItemSize * pSystem->wNextDequeueItem++);
    MUSB_MemCopy(pItem, pQueueItem, pSystem->pController->wQueueItemSize);

    if(pSystem->wNextDequeueItem >= pSystem->pController->wQueueLength)
    {
        pSystem->wNextDequeueItem = 0;
    }

    if(pSystem->wNextDequeueItem == pSystem->wNextQueueItem)
    {
        pSystem->bQueueStatus = QUEUE_EMPTY_STATUS;
    }

    return TRUE;
}

/*
 * Controller calls this to flush the ISR/BSR queue
 */
static void MGC_NoneFlushBackgroundQueue(void *pPrivate)
{
    MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivate;
    pSystem->wNextQueueItem = 0;
    pSystem->wNextDequeueItem = 0;
    pSystem->bQueueStatus = QUEUE_EMPTY_STATUS;
}

/*
* Controller calls this to enter a lock
*/
static uint8_t MGC_NoneLock(void *pPrivate, uint16_t wIndex)
{
    /*
     * No locking needed in non-concurrent environment
     * (ISR restores any "damage" it does)
     */
    return TRUE;
}

/*
* Controller calls this to exit a lock
*/
static uint8_t MGC_NoneUnlock(void *pPrivate, uint16_t wIndex)
{
    /*
     * No locking needed in non-concurrent environment
     * (ISR restores any "damage" it does)
     */
    return TRUE;
}

/*
 * non-OS ISR, arranged to be called by board-specific code.
 */
static uint8_t MGC_NoneControllerIsr(void *pPrivateData)
{
    int isrValue;
    volatile MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivateData;

    MUSB_NPRT("[MGC] NoneControllerIsr\r\n");
    if ((pSystem)
            && (pSystem->pController)
            && (pSystem->pController->pfIsr))
    {
        isrValue = pSystem->pController->pfIsr(pSystem->pController->pIsrParam);

    }

    if(isrValue < 0)
    {
        return FALSE;
    }

    if(isrValue > 0)
    {
        pSystem->bBsrRequest++;
        if(!pSystem->bBsrRequest)
        {
            pSystem->bBsrOver++;
        }
    }

    return TRUE;
}

/*
 * Arm a timer.
 * Call the board-specific function.
 */
uint8_t MGC_NoneArmTimer(void *pPrivateData, uint16_t wIndex,
                         uint32_t dwTime, uint8_t bPeriodic,
                         MUSB_pfTimerExpired pfExpireCallback)
{
    MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivateData;

    return MUSB_BoardArmTimer(pSystem->pBoardPrivateData,
                              wIndex,
                              dwTime,
                              bPeriodic,
                              pfExpireCallback,
                              pSystem->pController->pPrivateData);
}

/*
 * Cancel a timer.
 * Call the board-specific function.
 */
uint8_t MGC_NoneCancelTimer(void *pPrivateData, uint16_t wIndex)
{
    MGC_NoneSystem *pSystem = (MGC_NoneSystem *)pPrivateData;

    return MUSB_BoardCancelTimer(pSystem->pBoardPrivateData, wIndex);
}

uint32_t MUSB_NoneRunBackground(void)
{
    uint8_t bBsrOver;
    uint32_t RetVal = 0;
    volatile MGC_NoneSystem *pSystem;

    pSystem = MGC_apNoneSystem;
    bBsrOver = pSystem->bBsrOver;
    if((pSystem->bBsrRequest > pSystem->bBsrRan) || bBsrOver)
    {
        pSystem->bBsrRan++;

        RetVal = pSystem->pController->pfBsr(pSystem->pController->pBsrParam);
        if(!pSystem->bBsrRan)
            pSystem->bBsrOver = 0;
    }

    MUSB_NPRT("[MGC]RunBackground:%d\r\n", RetVal);

    return RetVal;
}

void MUSB_Host_init(void)
{
    volatile MGC_NoneSystem *pSystem;

    MUSB_PRT("[MGC] Host_init\r\n");

    pSystem = MGC_apNoneSystem;
    MUSB_Start_Fun(pSystem->pController->pBsrParam);
}

void MUSB_Host_uninit(void)
{
    volatile MGC_NoneSystem *pSystem;
    MUSB_HfiDeviceRemoved();

    MUSB_NoneRunBackground();
    pSystem = MGC_apNoneSystem;
    MUSB_Disconn_Fun(pSystem->pController->pBsrParam);
}

static void *MGC_NoneInitController(const MUSB_NoneController *pControllerInfo)
{
    uint32_t dwStatus;
    uint8_t *pBaseIsr;
    uint8_t *pBaseBsr;
    static MGC_NoneSystem pSystem;
    static uint8_t QueueData[SYSTEM_QUEUE_DATA_LEN];

    MUSB_PRT("\r\n\r\n[MGC]NoneInitController\r\n");

    MUSB_MemSet(&pSystem, 0, sizeof(MGC_NoneSystem));
    pSystem.Services.wVersion = MUSB_SYSTEM_SERVICES_VERSION;
    pSystem.Services.pPrivateData = (void *)(&pSystem);

    pSystem.Services.pfSystemToBusAddress    = MUSB_BoardSystemToBusAddress;
    pSystem.Services.pfQueueBackgroundItem   = MGC_NoneQueueBackgroundItem;
    pSystem.Services.pfDequeueBackgroundItem = MGC_NoneDequeueBackgroundItem;
    pSystem.Services.pfFlushBackgroundQueue  = MGC_NoneFlushBackgroundQueue;

    pSystem.Services.pfArmTimer    = MGC_NoneArmTimer;
    pSystem.Services.pfCancelTimer = MGC_NoneCancelTimer;
    pSystem.Services.pfLock        = MGC_NoneLock;
    pSystem.Services.pfUnlock      = MGC_NoneUnlock;
    pSystem.Services.pfPrintDiag   = MUSB_BoardPrintDiag;

    do
    {
        pBaseIsr = pBaseBsr = pControllerInfo->pBase;
        pSystem.pBoardPrivateData = MUSB_BoardInitController(&pSystem,
                                    MGC_NoneControllerIsr,
                                    pControllerInfo,
                                    &pBaseIsr,
                                    &pBaseBsr);

        /* try UCD init */
        pSystem.pController = MUSB_NewController(&MGC_gNoneUtils,
                              pControllerInfo->wType,
                              pBaseIsr,
                              pBaseBsr);
        if(!pSystem.pController)
        {
        }

        /* try target-specific timer init now that we know the requirements */
        if(!MUSB_BoardInitTimers(pSystem.pBoardPrivateData,
                                 pSystem.pController->wTimerCount,
                                 pSystem.pController->adwTimerResolutions))
        {
        }

        /* prepare queue */
        MUSB_PRT("[MGC]pQueueDataSize:%d \r\n", SYSTEM_QUEUE_DATA_LEN);
        pSystem.pQueueData = & QueueData[0];

        MUSB_MemSet(pSystem.pQueueData, 0,
                    pSystem.pController->wQueueLength *
                    pSystem.pController->wQueueItemSize);

        dwStatus = MUSB_StartController(pSystem.pController,
                                        &(pSystem.Services));
        if(MUSB_STATUS_OK != dwStatus)
        {
        }
    }
    while(FALSE);

    return &pSystem;

}

/*
 * Startup code calls this.
 * Loop through all controllers and initialize them.
 */
uint8_t MUSB_InitSystem(unsigned long dwBsrPriority)
{
    MGC_NoneSystem *pSystem;

    MGC_apNoneSystem = 0;

    pSystem = MGC_NoneInitController(&(MUSB_aNoneController[0]));
    if(pSystem)
    {
        MGC_apNoneSystem = pSystem;
    }

    return TRUE;
}

void MUSB_Host_Startup(void)
{
    volatile MGC_NoneSystem *pSystem;

    MUSB_PRT("[MGC] Host_Startup\r\n");

    pSystem = MGC_apNoneSystem;
    MUSB_Start_Fun(pSystem->pController->pBsrParam);
}

#endif

// eof


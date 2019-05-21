#ifndef __MUSB_NONE_BOARD_H__
#define __MUSB_NONE_BOARD_H__

#include "mu_dsi.h"
#include "mu_none.h"

#include "include.h"
#include "rtos_pub.h"

#if CFG_USB
/**
 * @field iVector uHAL's vector for reverse-lookup
 * @field iIndex uHAL's timer index
 * @field pfExpired expiration callback
 * @field pParam expiration callback parameter
 * @field dwTime remaining time, due to uHAL's MAX_PERIOD limitation
 * @field bPeriodic whether currently set for periodic
 */
typedef struct
{
    /* timer implementation, and it depends on operating system*/
    beken2_timer_t timer;

    unsigned int iVector;
    unsigned int iIndex;
    MUSB_pfTimerExpired pfExpired;

    void *pParam;
    uint32_t dwTime;
    uint8_t bPeriodic;
    uint8_t bTimerStart;
} MGC_AfsTimerWrapper;

/**
 * MGC_AfsUds.
 * Board-specific UDS instance data.
 * @param pNonePrivateData non-OS UDS instance data
 * @param pfNoneIsr non-OS UDS ISR
 * @field aTimerWrapper timer wrappers
 * @field wTimerCount how many wrappers
 * @field bIndex our index into the global array
 */
typedef struct
{
    char aIsrName[8];
    void *pNonePrivateData;
    MUSB_NoneIsr pfNoneIsr;
    void *pPciAck;

    MGC_AfsTimerWrapper *aTimerWrapper;
    unsigned int dwIrq;
    uint16_t wTimerCount;
    uint8_t bIndex;
} MGC_AfsUds;

extern MUSB_NoneController MUSB_aNoneController[];
extern void MGC_AfsUdsIsr(void);
#endif

#endif	/* multiple inclusion protection */

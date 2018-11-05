/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 *
 * @brief Timer Module for EFM32
 *
 */

#include "BCDS_Essentials.h"

/* Overwrite BCDS_MODULE_ID */
#ifdef BCDS_MODULE_ID
#undef BCDS_MODULE_ID
#endif

#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_TIMER

#include "BCDS_Basics.h"

#include "BCDS_MCU_Timer.h"

#if( BCDS_FEATURE_TIMER )

/* Preprocessor check for integrity of module id*/

#if(BCDS_MODULE_ID != BCDS_HAL_MODULE_ID_TIMER)  /*lint !e553 not properly resolved by lint*/
#error "Module ID mismatch"
#endif

/* EFM32 Timer Handle */
#include "BCDS_MCU_Timer_Handle.h"

/* */
#define MCU_TIMER_COMPARE_MATCH     UINT8_C(1)
#define MCU_TIMER_OVERFLOW          UINT8_C(1)
#define MCU_TIMER_UNDERFLOW         UINT8_C(1)
#define MCU_TIMER_COMPARE_CHANNEL0        UINT32_C(1 << 0)
#define MCU_TIMER_COMPARE_CHANNEL1        UINT32_C(1 << 1)
#define MCU_TIMER_COMPARE_CHANNEL2        UINT32_C(1 << 2)
#define MCU_TIMER_COMPARE_CHANNEL3        UINT32_C(1 << 3)
#define MCU_TIMER_COMPARE_CHANNEL4        UINT32_C(1 << 4)
#define MCU_TIMER_COMPARE_CHANNEL5        UINT32_C(1 << 5)
#define MCU_TIMER_COMPARE_CHANNEL6        UINT32_C(1 << 6)
#define MCU_TIMER_COMPARE_CHANNEL7        UINT32_C(1 << 7)

/*lint -esym(613,timerHandle) */
/*lint -esym(613,value) */
/*lint -esym(613,countValue) */
/*lint -esym(613,maxCount) */

/**
 * @brief Validate a Timer Handle
 *
 * Common function used to validate the given timer handle and HW handle
 */
static BCDS_INLINE Retcode_T ValidateTimerHandle(struct MCU_Timer_Handle_S * timerHandle);

/**
 * @brief Extract the timer event
 */
static struct MCU_Timer_Event_S getTimerEvent(struct MCU_Timer_Handle_S * timerHandle);

#if (BCDS_FEATURE_EFM32_RTC)
/**< RTC-Specific Flag extraction */
static struct MCU_Timer_Event_S extractFlags_RTC(uint32_t flags);
#endif

#if (BCDS_FEATURE_EFM32_BURTC)
/**< BURTC-Specific Flag extraction */
static struct MCU_Timer_Event_S extractFlags_BURTC(uint32_t flags);
#endif

#if (BCDS_FEATURE_EFM32_LETIMER)
/**< LETIMER-Specific Flag extraction */
static struct MCU_Timer_Event_S extractFlags_LETIMER(uint32_t flags);
#endif

#if (BCDS_FEATURE_EFM32_TIMER)
/**< TIMER-Specific Flag extraction */
static struct MCU_Timer_Event_S extractFlags_TIMER(uint32_t flags);
#endif

/* ************************************************************************** */
/* * API implementations **************************************************** */
/* ************************************************************************** */

/* Refer to API header for documentation */
Retcode_T MCU_Timer_Initialize(Timer_T timer, MCU_TIMER_Callback_T Callback)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL == timerHandle->InitHandle.None)
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMER_NOINIT);
        }
    }

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            RTC_Init(timerHandle->InitHandle.Rtc);
            RTC_IntDisable((RTC_IEN_COMP0 | RTC_IEN_COMP1));
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            BURTC_Init(timerHandle->InitHandle.Burtc);
            BURTC_IntDisable((BURTC_IEN_COMP0));
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            TIMER_Init(timerHandle->HwHandle.Timer, timerHandle->InitHandle.Timer);
            TIMER_IntDisable(timerHandle->HwHandle.Timer, (TIMER_IEN_CC0 | TIMER_IEN_CC1 | TIMER_IEN_CC2));
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            LETIMER_Init(timerHandle->HwHandle.Letimer, timerHandle->InitHandle.Letimer);
            LETIMER_IntDisable(timerHandle->HwHandle.Letimer, (LETIMER_IEN_COMP0 | LETIMER_IEN_COMP0));
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        } /* End of switch */
    }

    /* Register Callback */
    if (RETCODE_OK == rc)
    {
        timerHandle->Status = MCU_TIMER_STATUS_INITIALIZED;
        timerHandle->Callback = Callback;
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_Deinitialize(Timer_T timer)
{
    Retcode_T rc = RETCODE_OK;
    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    /* Test Handle structure for validity */
    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL != timerHandle->DeInitHandle.None)
        {
            switch (timerHandle->HwType)
            {
                case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
                RTC_Init(timerHandle->DeInitHandle.Rtc);
#else
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
                break;
                case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
                BURTC_Init(timerHandle->DeInitHandle.Burtc);
#else
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
                break;
                case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
                TIMER_Init(timerHandle->HwHandle.Timer, timerHandle->DeInitHandle.Timer);
#else
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
                break;
                case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
                LETIMER_Init(timerHandle->HwHandle.Letimer, timerHandle->DeInitHandle.Letimer);
#else
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
                break;
                default:
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                break;
            } /* End of switch */
        }
    }

    if (RETCODE_OK == rc)
    {
        timerHandle->Callback = NULL;
        timerHandle->Status = MCU_TIMER_STATUS_DEINITIALIZED;
    }

    return rc;

}

Retcode_T MCU_Timer_SetCompareValue(Timer_T timer, uint32_t channel, uint32_t value)
{
    Retcode_T rc = RETCODE_OK;
    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            RTC_CompareSet(channel, value);
            if(UINT32_C(0) != value)
            {
                RTC_IntEnable(true << (channel + _RTC_IEN_COMP0_SHIFT));
            }
            else
            {
                RTC_IntDisable(true << (channel + _RTC_IEN_COMP0_SHIFT));
            }

#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            BURTC_CompareSet(channel, value);
            if(UINT32_C(0) != value)
            {
                BURTC_IntEnable(BURTC_IEN_COMP0);
            }
            else
            {
                BURTC_IntDisable(BURTC_IEN_COMP0);
            }
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            TIMER_CompareSet(timerHandle->HwHandle.Timer, channel, value);
            if(UINT32_C(0) != value)
            {
                TIMER_IntEnable(timerHandle->HwHandle.Timer, (true << (channel + _TIMER_IEN_CC0_SHIFT)));
            }
            else
            {
                TIMER_IntDisable(timerHandle->HwHandle.Timer, (true << (channel + _TIMER_IEN_CC0_SHIFT)));
            }

#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            LETIMER_CompareSet(timerHandle->HwHandle.Letimer, channel, value);
            if(UINT32_C(0) != value)
            {
                LETIMER_IntEnable(timerHandle->HwHandle.Letimer, (true << (channel + _LETIMER_IEN_COMP0_SHIFT)));
            }
            else
            {
                LETIMER_IntDisable(timerHandle->HwHandle.Letimer, (true << (channel + _LETIMER_IEN_COMP0_SHIFT)));
            }

#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }
    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_GetCompareValue(Timer_T timer, uint32_t channel, uint32_t *value)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL == (void *) value)
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }

    }

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            *value = RTC_CompareGet(channel);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            *value = BURTC_CompareGet(channel);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            *value = TIMER_CaptureGet(timerHandle->HwHandle.Timer, channel);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            *value = LETIMER_CompareGet(timerHandle->HwHandle.Letimer, channel);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_GetCountValue(Timer_T timer, uint32_t * countValue)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL == (void *) countValue)
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
    }

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            *countValue = RTC_CounterGet();
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            *countValue = BURTC_CounterGet();
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            *countValue = TIMER_CounterGet(timerHandle->HwHandle.Timer);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            *countValue = LETIMER_CounterGet(timerHandle->HwHandle.Letimer);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }

    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_GetMaxCount(Timer_T timer, uint32_t * maxCount)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL == (void *) maxCount)
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
    }

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            *maxCount = (_RTC_CNT_CNT_MASK >> _RTC_CNT_CNT_SHIFT);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            *maxCount = (_BURTC_CNT_CNT_MASK >> _BURTC_CNT_CNT_SHIFT);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            *maxCount = (_TIMER_CNT_CNT_MASK >> _BURTC_CNT_CNT_SHIFT);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            *maxCount = (_LETIMER_CNT_MASK >> _LETIMER_CNT_CNT_SHIFT);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_Start(Timer_T timer)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            RTC_Enable(true);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            BURTC_Enable(true);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            TIMER_Enable(timerHandle->HwHandle.Timer, true);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            LETIMER_Enable(timerHandle->HwHandle.Letimer, true);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            timerHandle->Status = MCU_TIMER_STATUS_ERROR;
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }

    }

    if(RETCODE_OK == rc)
    { /* Timer was started and should be active */
        timerHandle->Status = MCU_TIMER_STATUS_ACTIVE;
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_Stop(Timer_T timer)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
            RTC_Enable(false);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
            BURTC_Enable(false);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
            TIMER_Enable(timerHandle->HwHandle.Timer, false);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
            LETIMER_Enable(timerHandle->HwHandle.Letimer, false);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }

    if(RETCODE_OK == rc)
    { /* Timer was stoped and should be idle */
        timerHandle->Status = MCU_TIMER_STATUS_IDLE;
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Timer_GetClockFrequency(Timer_T timer, uint32_t * timerFrequency)
{
    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    Retcode_T rc = ValidateTimerHandle(timerHandle);

    if (RETCODE_OK == rc)
    {
        if (NULL == (void *) timerFrequency)
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
    }

    /*lint -esym(613, timerFrequency) NULL check has been performed before*/
    if (RETCODE_OK == rc)
    {
        switch (timerHandle->HwType)
        {
            case EM_RTC:
#if (BCDS_FEATURE_EFM32_RTC)
            *timerFrequency = CMU_ClockFreqGet(cmuClock_RTC);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_BURTC:
#if (BCDS_FEATURE_EFM32_BURTC)
            *timerFrequency = BURTC_ClockFreqGet();
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_TIMER:
#if (BCDS_FEATURE_EFM32_TIMER)
            *timerFrequency = UINT32_C(0); /* Currently not supported */
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            case EM_LETIMER:
#if (BCDS_FEATURE_EFM32_LETIMER)
            *timerFrequency = CMU_ClockFreqGet(cmuClock_LETIMER0);
#else
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
#endif
            break;
            default:
            *timerFrequency = UINT32_C(0);
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }
    /*lint +esym(613, timerFrequency re-enable*/

    return rc;
}

/* Active or Idle */
MCU_Timer_Status_T MCU_Timer_GetStatus(Timer_T timer)
{
    Retcode_T rc = RETCODE_OK;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    MCU_Timer_Status_T Status;

    rc = ValidateTimerHandle(timerHandle);

    if (rc == RETCODE_OK)
    {
        Status = timerHandle->Status;
    }
    else
    {
        Status = MCU_TIMER_STATUS_ERROR;
    }

    return Status;
}

/* Timer Callback */
void MCU_Timer_Callback(Timer_T timer)
{
    Retcode_T rc = RETCODE_OK;
    struct MCU_Timer_Event_S event;

    struct MCU_Timer_Handle_S * timerHandle = (struct MCU_Timer_Handle_S *) timer;

    if (NULL == (void *) timerHandle)
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else if (NULL == timerHandle->Callback)
    {
        (void) getTimerEvent(timerHandle);
    }
    else
    {
        event = getTimerEvent(timerHandle);

        timerHandle->Callback(timer, event);
    }

    if (RETCODE_OK != rc)
    {
        Retcode_RaiseError(rc);
    }
}

static BCDS_INLINE Retcode_T ValidateTimerHandle(struct MCU_Timer_Handle_S * timerHandle)
{
    Retcode_T rc = RETCODE_OK;

    if (NULL == (void *) timerHandle)
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else if (NULL == timerHandle->HwHandle.None)
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMER_NOHWHANDLE);
    }

    return rc;
}

/* */
static struct MCU_Timer_Event_S getTimerEvent(struct MCU_Timer_Handle_S * timerHandle)
{
    struct MCU_Timer_Event_S event;

    event.CompareChannel = UINT32_C(0);
    event.CompareMatch = UINT8_C(0);
    event.ErrorCode = UINT32_C(0);
    event.Overflow = UINT32_C(0);
    event.reserved = UINT32_C(0);
    event.Underflow = UINT8_C(0);

    uint32_t flags;

    switch (timerHandle->HwType)
    {
        case EM_RTC:

#if (BCDS_FEATURE_EFM32_RTC)
        flags = RTC_IntGet(); /* Get Flags */
        event = extractFlags_RTC(flags); /* Extract information */
        RTC_IntClear(flags); /* Clear flags */
#else
        // event.ErrorCode;
#endif
        break;
        case EM_BURTC:

#if (BCDS_FEATURE_EFM32_BURTC)
        flags = BURTC_IntGet();
        event = extractFlags_BURTC(flags);
        BURTC_IntClear(flags);
#else
        // event.ErrorCode;
#endif
        break;
        case EM_TIMER:

#if (BCDS_FEATURE_EFM32_TIMER)
        flags = TIMER_IntGet(timerHandle->HwHandle.Timer);
        event = extractFlags_TIMER(flags);
        TIMER_IntClear(timerHandle->HwHandle.Timer,flags);
#else
        // event.ErrorCode;
#endif
        break;
        case EM_LETIMER:

#if (BCDS_FEATURE_EFM32_LETIMER)
        flags = LETIMER_IntGet(timerHandle->HwHandle.Letimer);
        event = extractFlags_LETIMER(flags);
        LETIMER_IntClear(timerHandle->HwHandle.Letimer,flags);
#else
        // event.ErrorCode;
#endif
        break;
        default:
        // event.ErrorCode;
        break;
    }

    return event;
}

/* */
#if (BCDS_FEATURE_EFM32_RTC)
static struct MCU_Timer_Event_S extractFlags_RTC(uint32_t flags)
{
    struct MCU_Timer_Event_S event;

    event.CompareChannel = UINT32_C(0);
    event.CompareMatch = UINT8_C(0);
    event.ErrorCode = UINT32_C(0);
    event.Overflow = UINT32_C(0);
    event.reserved = UINT32_C(0);
    event.Underflow = UINT8_C(0);

    /* Compare Match in Channel 0 */
    if (RTC_IF_COMP0 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL0;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Compare Match in Channel 1*/
    if (RTC_IF_COMP1 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL1;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* RTC Overflow flag */
    if (RTC_IF_OF & flags)
    {
        event.Overflow = MCU_TIMER_OVERFLOW;
    }

    return event;
}
#endif /* BCDS_FEATURE_EFM32_RTC*/

/* */
#if (BCDS_FEATURE_EFM32_BURTC)
static struct MCU_Timer_Event_S extractFlags_BURTC(uint32_t flags)
{
    struct MCU_Timer_Event_S event;

    event.CompareChannel = UINT32_C(0);
    event.CompareMatch = UINT8_C(0);
    event.ErrorCode = UINT32_C(0);
    event.Overflow = UINT32_C(0);
    event.reserved = UINT32_C(0);
    event.Underflow = UINT8_C(0);

    /* Compare Match in Channel 0 */
    if (BURTC_IF_COMP0 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL0;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Timer Overflow */
    if(BURTC_IF_OF & flags)
    {
        event.Overflow = MCU_TIMER_OVERFLOW;
    }

    /* LFXO Failure */
    if (BURTC_IF_LFXOFAIL & flags)
    {
        event.ErrorCode = (uint32_t)TIMER_LFXCO_FAILURE;
    }

    return event;
}
#endif  /* BCDS_FEATURE_EFM32_BURTC */

/* */
#if (BCDS_FEATURE_EFM32_LETIMER)
static struct MCU_Timer_Event_S extractFlags_LETIMER(uint32_t flags)
{
    struct MCU_Timer_Event_S event; /* Probably nor initialized */

    event.CompareChannel = UINT32_C(0);
    event.CompareMatch = UINT8_C(0);
    event.ErrorCode = UINT32_C(0);
    event.Overflow = UINT32_C(0);
    event.reserved = UINT32_C(0);
    event.Underflow = UINT8_C(0);

    /* Compare Match in Channel 0 */
    if (LETIMER_IF_COMP0 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL0;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Compare Match in Channel 1 */
    if (LETIMER_IF_COMP1 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL1;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Underflow */
    if (LETIMER_IF_UF & flags)
    {
        event.Underflow = MCU_TIMER_UNDERFLOW;
    }

    /* Repeat Counter 0 */
    if (LETIMER_IF_REP0 & flags)
    {

    }

    /* Repeat Counter 1 */
    if (LETIMER_IF_REP1 & flags)
    {

    }

    return event;
}
#endif  /* BCDS_FEATURE_EFM32_LETIMER */

/* */
#if (BCDS_FEATURE_EFM32_TIMER)
static struct MCU_Timer_Event_S extractFlags_TIMER(uint32_t flags)
{
    struct MCU_Timer_Event_S event; /* Probably nor initialized */

    event.CompareChannel = UINT32_C(0);
    event.CompareMatch = UINT8_C(0);
    event.ErrorCode = UINT32_C(0);
    event.Overflow = UINT32_C(0);
    event.reserved = UINT32_C(0);
    event.Underflow = UINT8_C(0);

    /* Timer Overflow */
    if (TIMER_IF_OF & flags)
    {
        event.Overflow = MCU_TIMER_OVERFLOW;
    }

    /* Timer Underflow */
    if (TIMER_IF_UF & flags)
    {
        event.Underflow = MCU_TIMER_UNDERFLOW;
    }

    /* Compare Capture Channel 0 */
    if (TIMER_IF_CC0 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL0;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Compare Capture Channel 1 */
    if (TIMER_IF_CC1 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL1;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Compare Capture Channel 2 */
    if (TIMER_IF_CC2 & flags)
    {
        event.CompareChannel |= MCU_TIMER_COMPARE_CHANNEL2;
        event.CompareMatch = MCU_TIMER_COMPARE_MATCH;
    }

    /* Input Buffer Overflow */
    if (TIMER_IF_ICBOF0 & flags)
    {
        //   event.CompareChannel;
        //   event.CompareMatch;
        //   event.Overflow;
        ///event.ErrorCode;
    }

    /* Input Buffer Overflow */
    if (TIMER_IF_ICBOF1 & flags)
    {
        //   event.CompareChannel;
        //   event.CompareMatch;
        //   event.Overflow;
        //event.ErrorCode;
    }

    /* Input Buffer Overflow */
    if (TIMER_IF_ICBOF2 & flags)
    {
        //   event.CompareChannel;
        //   event.CompareMatch;
        //   event.Overflow;
        //   event.ErrorCode;
    }

    return event;
}
#endif /* BCDS_FEATURE_EFM32_TIMER */

#endif /* BCDS_FEATURE_TIMER */

/* ************************************************************************** */
/* Driver Handles  ********************************************************** */
/* ************************************************************************** */

#if (BCDS_FEATURE_EFM32_BURTC)
/* BURTC Handle */
static struct MCU_Timer_Handle_S burtcHandle = {
    .HwHandle = {.Burtc = BURTC},
    .HwType = EM_BURTC,
    .InitHandle = {.Burtc = (BURTC_Init_TypeDef *) NULL},
    .DeInitHandle = {.Burtc = (BURTC_Init_TypeDef *) NULL},
    .Status = MCU_TIMER_STATUS_INVALID,
    .Callback =  NULL
};

HWHandle_T MCU_Timer_GetBurtcHandle(void)
{
    return ((HWHandle_T) &burtcHandle);
}

/* Calls the Burtc IRQ Handler */
void MCU_Timer_HandleBurtcIrq(void)
{
    MCU_Timer_Callback((Timer_T) & burtcHandle);
}
#endif

#if (BCDS_FEATURE_EFM32_RTC)
/* RTC Handle  */
static struct MCU_Timer_Handle_S rtcHandle = {
        .HwHandle = {.Rtc = RTC},
        .HwType = EM_RTC,
        .InitHandle = {.Rtc = (RTC_Init_TypeDef *) NULL},
        .DeInitHandle = {.Rtc = (RTC_Init_TypeDef *) NULL},
        .Status = MCU_TIMER_STATUS_INVALID,
        .Callback =  NULL
};

HWHandle_T MCU_Timer_GetRtcHandle(void)
{
    return ((HWHandle_T) &rtcHandle);
}

/* Calls the RTC IRQ Handler*/
void MCU_Timer_HandleRtcIrq(void)
{
    MCU_Timer_Callback((Timer_T) & rtcHandle);
}
#endif
/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

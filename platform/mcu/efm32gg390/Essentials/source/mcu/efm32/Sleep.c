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
 * @brief MCU Sleep management for EFM32
 *
 */

#include "BCDS_Essentials.h"

/* Overwrite BCDS_MODULE_ID */
#ifdef BCDS_MODULE_ID
#undef BCDS_MODULE_ID
#endif

#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_SLEEP

#include "BCDS_Basics.h"

#include "BCDS_MCU_Sleep.h"

/* Include the BSP Board */
#include "BCDS_BSP_Board.h"

#if( BCDS_FEATURE_SLEEP )



#if(BCDS_MODULE_ID != BCDS_HAL_MODULE_ID_SLEEP)  /*lint !e553 not properly resolved by lint*/
#error "Module ID mismatch"
#endif

/* Include EFM32 API */
#include "em_device.h"
#include "em_int.h"     /* EFM32 Interrupts */
#include "em_emu.h"     /* EFM32 Energy Management unit */

#define MCU_SLEEP_COUNTER_MAX_VALUE         UINT32_C(0xFF) /**< Saturating point for the nested counter */

#define MCU_SLEEP_AVAILABLE_LEVELS          UINT8_C(3) /**< Amount of sleep levels available */

/**
 * Available sleep modes on the EFM32 Giant Gecko MCUs
 */
enum MCU_SleepMode_E
{
    SleepMode_EM0 = UINT32_C(0), /**< Active Mode */
    SleepMode_EM1 = UINT32_C(1), /**< Sleep Mode */
    SleepMode_EM2 = UINT32_C(2), /**< Deep Sleep Mode */
    SleepMode_EM3 = UINT32_C(3), /**< Stop Mode */
    SleepMode_EM4 = UINT32_C(4), /**< Shutdown Mode */
};

/**
 * Nested Sleep Counter
 */
static uint32_t SleepBlockCounter[MCU_SLEEP_AVAILABLE_LEVELS];

/**
 *
 * @param[in] Decrease counter on the selected sleep mode
 *
 * @returns Counter value for given mode
 *
 * @warning This function should only be called by MCU_Sleep_BlockSleepMode
 */
static uint32_t MCU_Sleep_IncreaseCounter(enum MCU_SleepMode_E mode);

/**
 * @param[in] Increase counter on the selected sleep mode
 *
 * @returns Counter value for given mode
 *
 * @warning This function should only be called by MCU_Sleep_UnblockSleepMode
 */
static uint32_t MCU_Sleep_DecreaseCounter(enum MCU_SleepMode_E mode);

/* ************************************************************************* */
/* * API implementations *************************************************** */
/* ************************************************************************* */

/* Refer to API header for documentation */
Retcode_T MCU_Sleep_Initialize(void)
{
    /* Reset counters */
    for (uint8_t i = 0; i != MCU_SLEEP_AVAILABLE_LEVELS; i++)
    {
        SleepBlockCounter[i] = UINT32_C(0);
    }

    return RETCODE_OK;
}

/* Refer to API header for documentation */
Retcode_T MCU_Sleep_BlockSleepMode(uint32_t mode)
{
    Retcode_T rc = RETCODE_OK;
    uint8_t blockingCounterValue = UINT8_C(0);

    enum MCU_SleepMode_E sleepMode = (enum MCU_SleepMode_E) mode;

    if ((SleepMode_EM1 == sleepMode) || (SleepMode_EM3 == sleepMode))
    {
        (void)MCU_Sleep_IncreaseCounter(sleepMode);
    }
    else if (SleepMode_EM2 == sleepMode)
    {
        blockingCounterValue = MCU_Sleep_IncreaseCounter(sleepMode);

        if (UINT8_C(1) == blockingCounterValue)
        {
            EMU_EM2Block(); /* Issue a block on the EM2 */
        }

    }
    else
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    return rc;
}

/* Refer to API header for documentation */
Retcode_T MCU_Sleep_UnblockSleepMode(uint32_t mode)
{
    Retcode_T rc = RETCODE_OK;
    uint8_t blockingCounterValue = UINT8_C(0);

    enum MCU_SleepMode_E sleepMode = (enum MCU_SleepMode_E) mode;

    if ((SleepMode_EM1 == sleepMode) || (SleepMode_EM3 == sleepMode))
    {
        (void)MCU_Sleep_DecreaseCounter(sleepMode);
    }
    else if (SleepMode_EM2 == sleepMode)
    {
        blockingCounterValue = MCU_Sleep_DecreaseCounter(sleepMode);

        if (UINT8_C(0) == blockingCounterValue)
        {
            EMU_EM2UnBlock(); /* Unblock the EM2 */
        }

    }
    else
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    return rc;
}

/* Refer to API header for documentation */
void MCU_Sleep_EnterSleep(uint32_t mode)
{
    /* Disable interrupt */
    (void) INT_Disable();

    __DSB(); /* Data Synchronization Barrier before Sleep */

    switch (mode)
    {
        case SleepMode_EM1:
        EMU_EnterEM1();
        break;
        case SleepMode_EM2:
        EMU_EnterEM2(true); /* Enter into sleep mode and restore clock and oscillator settings */
        break;
        case SleepMode_EM3:
        EMU_EnterEM3(true); /* Enter into sleep mode and restore clock and oscillator settings */
        break;
        default:
        __NOP(); /* Do nothing */
        break;
    }

    __ISB(); /* Pipeline Flush after sleep */

    (void) INT_Enable();
}

/* Refer to API header for documentation */
uint32_t MCU_Sleep_GetLowestSleepMode(void)
{
    uint32_t mode;

    for (mode = UINT32_C(0); mode != MCU_SLEEP_AVAILABLE_LEVELS; mode++)
    {
        if (UINT32_C(0) != SleepBlockCounter[mode])
        {
            break;
        }
    }

    return mode;
}

/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

/* This function should only be called by MCU_Sleep_BlockSleepMode */
static uint32_t MCU_Sleep_IncreaseCounter(enum MCU_SleepMode_E mode)
{
    size_t index = (size_t) mode - 1;

    if (MCU_SLEEP_COUNTER_MAX_VALUE != SleepBlockCounter[index])
    {
        SleepBlockCounter[index]++;
    }

    return (SleepBlockCounter[index]);
}

/* This function should only be called by MCU_Sleep_UnblockSleepMode */
static uint32_t MCU_Sleep_DecreaseCounter(enum MCU_SleepMode_E mode)
{
    size_t index = (size_t) mode - 1;

    if (UINT32_C(0) != SleepBlockCounter[index])
    {
        SleepBlockCounter[index]--;
    }

    return (SleepBlockCounter[index]);
}

/** @} */
#endif /* BCDS_FEATURE_MCU_SLEEP */

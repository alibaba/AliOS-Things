/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _HAL_PERF_H_
#define _HAL_PERF_H_

#include "cs_types.h"

/// @defgroup hal_ahb_monitor HAL AHB monitor driver
/// This chapter describes the performance monitoring capabilities of the AHB monitor
///
/// @par Description
/// The chip includes 2 bus monitors able to gather statistics about the bus usage but
/// also some specific module usage (Cpu, Memory controler)
/// Using these 2 monitors, you can get human readable information through the trace
/// about the system usage.
///
/// @{

typedef enum
{
/// Default target when the perfmeter is not opened
/// In that mode, calls to hal_PerfMeterGetStatistics always return PERFMETER_NOT_READY
    HAL_PERF_NOT_PROGRAMMED,
/// Monitors the percentage of cycles where the internal AHB bus is not in Idle
    HAL_PERF_AHB_BUS_USAGE,
/// Monitors the percentage of cycles where the external memory bus is not in Idle
    HAL_PERF_EBC_USAGE,
/// Monitors the percentage of cycles where the internal memory (Ram/Rom) is used
    HAL_PERF_INT_RAMROM,
/// Monitors the percentage of the time where the Xcpu is in Icache miss
    HAL_PERF_ICACHE_MISS,
/// Monitors the percentage of the time where the Xcpu is in Dcache miss
    HAL_PERF_DCACHE_MISS,
/// Monitors the percentage of the time where the Xcpu is Fetching Instructions
    HAL_PERF_I_FETCH,
/// Monitors the percentage of the time where the Xcpu is Fetching Data
    HAL_PERF_D_FETCH
} HAL_PERFMETER_TARGET_T;

typedef struct
{
/// When TRUE, do one measure and stop, when FALSE, measure continuously
    BOOL singleShot;
/// Parameter to monitor
    HAL_PERFMETER_TARGET_T target;
} HAL_PERFMETER_CFG_T;

///
#define PERFMETER_NOT_READY 0xff

// ============================================================================
// hal_PerfMeterOpen
// ----------------------------------------------------------------------------
/// This function is called to configure and enable the monitoring
/// Monitoring is done over 16M system clock cyles
/// @param cfg pointer on the configuration structure
// ============================================================================
PUBLIC VOID hal_PerfMeterOpen(HAL_PERFMETER_CFG_T *cfg);

// ============================================================================
// hal_PerfMeterClose
// ----------------------------------------------------------------------------
/// This function is called to disable the monitoring
// ============================================================================
PUBLIC VOID hal_PerfMeterClose(VOID);

// ============================================================================
// hal_PerfMeterGetStatistics
// ----------------------------------------------------------------------------
/// This function is called to display the statistics through the trace
/// (on level HAL_DBG_TRC).
/// In Single shot mode, this will automatically relaunch a new measure
/// @return percentage measured for the selected parameter
/// returns PERFMETER_NOT_READY in single shot mode when the measure is not finished
// ============================================================================
PUBLIC UINT32 hal_PerfMeterGetStatistics(VOID);

/// @}
#endif // _HAL_PERF_H_

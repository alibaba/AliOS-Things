/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ----------------------------------------------------------------------
 *
 * Project:      ARMv8-M System Recovery demo
 * Title:        IncidentLog_s.c   record system faults
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#include "IncidentLog_s.h"
#include "Hardfault.h"
#include <stdio.h>
#include <string.h>
#include <arm_cmse.h>
#include <arm_compat.h>

// locate IncidentLog buffer in uninitialized section
// and use the linker scatter file to ensure that it is not initialized
static IncidentLog_t __attribute__((section( ".bss.noinit"))) IncidentLog;


// time in seconds is maintained by the SysTick_Handler
// and is captured for each incident that gets logged
extern unsigned int Seconds ;



uint32_t InitIncidentLog (void) {
  if (INCIDENT_LOG_MAGIC_NUMBER != IncidentLog.MagicNumber) {
    /* magig number not there, so initialization required */
    memset( &IncidentLog, 0, sizeof( IncidentLog ));
    IncidentLog.MagicNumber = INCIDENT_LOG_MAGIC_NUMBER;
  }
  return (IncidentLog.Entries[IncidentLog.RecentEntry].Time);
}



void LogIncident (IncidentReason_t Reason, uint32_t Location, uint8_t Flags)  {
  IncidentLog.Entries[IncidentLog.RecentEntry].Reason   = Reason;
  IncidentLog.Entries[IncidentLog.RecentEntry].Location = Location;
  IncidentLog.Entries[IncidentLog.RecentEntry].Time     = Seconds;
  IncidentLog.Entries[IncidentLog.RecentEntry].Flags    = Flags;
  IncidentLog.RecentEntry++;
  if (IncidentLog.RecentEntry >= INCIDENT_LOG_ENTRIES_MAX) {
    IncidentLog.RecentEntry = 0;
  }
}


__attribute__((cmse_nonsecure_entry))
void LogIncident_s (IncidentReason_t Reason, uint32_t Location, uint8_t Flags)  {
  LogIncident (Reason, Location, Flags);
}


__attribute__((cmse_nonsecure_entry))
void GetIncidentLog_s (IncidentLog_t *IncidentLog_p) {
  struct IncidentLog_t *IncidentLog_p_ok;

  /* cmse_check_pointed_object */
  IncidentLog_p_ok = cmse_check_address_range (IncidentLog_p, sizeof(IncidentLog_t), CMSE_NONSECURE);
  if (IncidentLog_p_ok != NULL)  {
    /* requested copy range is completely in non-secure memory */
    memcpy (IncidentLog_p_ok, &IncidentLog, sizeof (IncidentLog_t));
  }
  else
  {  /* requested copy range is not comnpletely in non-secure memory */
    LogIncident (IR_SECDAT, __current_pc(), IS_SECURE);
    PerformReset ();
  }
}

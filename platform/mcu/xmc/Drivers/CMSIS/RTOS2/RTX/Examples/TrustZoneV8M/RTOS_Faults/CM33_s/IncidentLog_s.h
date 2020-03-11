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
 * Title:        IncidentLog.h   record system faults
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/


#ifndef _INCIDENTLOG_H
#define _INCIDENTLOG_H


#include <stdint.h>


/*
 * possible incident reasons used in this test program
 */
typedef enum  {
  IR_UNKNOW,  /* unknown reason                                        */
  IR_DIVBY0,  /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1    */
  IR_STKOF,   /* UsageFault, stack overflow                            */ 
  IR_INVEP,   /* SecureFault, invalid Secure state entry point         */
  IR_WDTEXP,  /* secure systick watchdog timeout                       */
  IR_SECDAT   /* pointer pointing to secure instead non-secure memory  */
} IncidentReason_t;


// incident flag
#define IS_SECURE    (1UL << 0)    /* incident happend in secure state */


typedef struct  {
  IncidentReason_t       Reason;  // incident reason 
  uint8_t                 Flags;  // secure / non-secure state
	uint16_t             Reserved;  // reserved (not used)
  uint32_t                 Time;  // time stamp
  uint32_t             Location;  // PC address of incident
} IncidentLogEntry_t;


#define INCIDENT_LOG_ENTRIES_MAX   (4)
#define INCIDENT_LOG_MAGIC_NUMBER  (0xABABABAB)

typedef struct {
  unsigned int       MagicNumber;
  unsigned int       RecentEntry;
  IncidentLogEntry_t Entries [INCIDENT_LOG_ENTRIES_MAX];
} IncidentLog_t;


uint32_t InitIncidentLog (void) ;
void LogIncident_s    (IncidentReason_t Reason, uint32_t Location, uint8_t Flags);
void LogIncident      (IncidentReason_t Reason, uint32_t Location, uint8_t Flags);
void GetIncidentLog_s (IncidentLog_t *IncidentLog_p);
void PerformReset     (void);
void hard_fault_handler_c (uint32_t *hardfault_args);
#endif  /* !_INCIDENTLOG_H */

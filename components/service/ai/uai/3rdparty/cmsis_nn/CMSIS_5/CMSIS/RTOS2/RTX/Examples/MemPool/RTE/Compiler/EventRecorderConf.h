/*------------------------------------------------------------------------------
 * MDK - Component ::Event Recorder
 * Copyright (c) 2016-2018 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    EventRecorderConf.h
 * Purpose: Event Recorder Configuration
 * Rev.:    V1.1.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Event Recorder

//   <o>Number of Records
//     <8=>8 <16=>16 <32=>32 <64=>64 <128=>128 <256=>256 <512=>512 <1024=>1024
//     <2048=>2048 <4096=>4096 <8192=>8192 <16384=>16384 <32768=>32768
//     <65536=>65536
//   <i>Configures size of Event Record Buffer (each record is 16 bytes)
//   <i>Must be 2^n (min=8, max=65536)
#define EVENT_RECORD_COUNT      64U

//   <o>Time Stamp Source
//      <0=> DWT Cycle Counter  <1=> SysTick  <2=> CMSIS-RTOS2 System Timer
//      <3=> User Timer (Normal Reset)  <4=> User Timer (Power-On Reset)
//   <i>Selects source for 32-bit time stamp
#define EVENT_TIMESTAMP_SOURCE  0

//   <o>Time Stamp Clock Frequency [Hz] <0-1000000000>
//   <i>Defines default time stamp clock frequency (0 when not used)
#define EVENT_TIMESTAMP_FREQ    0U

// </h>

//------------- <<< end of configuration section >>> ---------------------------

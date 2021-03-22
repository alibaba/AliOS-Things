/* -------------------------------------------------------------------------- 
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
 * $Date:        20. April 2016
 * $Revision:    V1.4
 *
 * Driver:       Driver_CAN0/1
 * Configured:   via RTE_Device.h configuration file
 * Project:      CAN Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   CAN Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_CAN# = 0       use CAN0
 *   Connect to hardware via Driver_CAN# = 1       use CAN1
 * --------------------------------------------------------------------------
 * Defines used for driver configuration (at compile time):
 *
 *   CAN_CLOCK_TOLERANCE:  defines maximum allowed clock tolerance in 1/1024 steps
 *     - default value:    15 (approx. 1.5 %)
 *   CAN0_OBJ_NUM:         number of message objects used by CAN0 controller
 *                         this value can be reduced to save some RAM space
 *     - default value:    32 (also this is maximum value)
 *   CAN1_OBJ_NUM:         number of message objects used by CAN1 controller
 *                         this value can be reduced to save some RAM space
 *     - default value:    32 (also this is maximum value)
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.4
 *    Corrected receive overrun clearing and signaling
 *  Version 1.3
 *    Corrected interrupt routine (status interrupt could case lockup)
 *  Version 1.2
 *    Corrected functionality when NULL pointer is provided for one or both 
 *    signal callbacks in Initialize function call
 *  Version 1.1
 *    Corrected CAN1 IRQ routine
 *    Corrected MessageSend function to return busy if transmission is in progress
 *  Version 1.0
 *    Initial release
 */

#include "CAN_LPC18xx.h"

// Externally overridable configuration definitions

// Maximum allowed clock tolerance in 1/1024 steps
#ifndef CAN_CLOCK_TOLERANCE
#define CAN_CLOCK_TOLERANCE             (15U)   // 15/1024 approx. 1.5 %
#endif

// Maximum number of Message Objects used for CAN0 controller
#ifndef CAN0_OBJ_NUM
#define CAN0_OBJ_NUM                    (32U)
#endif
#if    (CAN0_OBJ_NUM > 32U)
#error  Too many Message Objects defined for CAN0, maximum number of Message Objects is 32 !!!
#endif

// Maximum number of Message Objects used for CAN1 controller
#ifndef CAN1_OBJ_NUM
#define CAN1_OBJ_NUM                    (32U)
#endif
#if    (CAN1_OBJ_NUM > 32U)
#error  Too many Message Objects defined for CAN1, maximum number of Message Objects is 32 !!!
#endif


// External Functions
extern uint32_t GetClockFreq (uint32_t clk_src);


// CAN Driver ******************************************************************

#define ARM_CAN_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,4) // CAN driver version

// Driver Version
static const ARM_DRIVER_VERSION can_driver_version = { ARM_CAN_API_VERSION, ARM_CAN_DRV_VERSION };

// Driver Capabilities
static const ARM_CAN_CAPABILITIES can_driver_capabilities[2] = {
{                       // CAN0 driver capabilities
  CAN0_OBJ_NUM,         // Number of CAN Objects available
  0U,                   // Does not support reentrant calls to ARM_CAN_MessageSend, ARM_CAN_MessageRead, ARM_CAN_ObjectConfigure and abort message sending used by ARM_CAN_Control.
  0U,                   // Does not support CAN with flexible data-rate mode (CAN_FD)
  0U,                   // Does not support restricted operation mode
  1U,                   // Supports bus monitoring mode
  1U,                   // Supports internal loopback mode
  1U,                   // Supports external loopback mode
}, 
{                       // CAN1 driver capabilities
  CAN1_OBJ_NUM,         // Number of CAN Objects available
  0U,                   // Does not support reentrant calls to ARM_CAN_MessageSend, ARM_CAN_MessageRead, ARM_CAN_ObjectConfigure and abort message sending used by ARM_CAN_Control.
  0U,                   // Does not support CAN with flexible data-rate mode (CAN_FD)
  0U,                   // Does not support restricted operation mode
  1U,                   // Supports bus monitoring mode
  1U,                   // Supports internal loopback mode
  1U,                   // Supports external loopback mode
}
};

// Object Capabilities
static const ARM_CAN_OBJ_CAPABILITIES can_object_capabilities = {
  1U,                   // Object supports transmission
  1U,                   // Object supports reception
  1U,                   // Object supports RTR reception and automatic Data transmission
  1U,                   // Object supports RTR transmission and automatic Data reception
  0U,                   // Object does not allow assignment of multiple filters to it
  1U,                   // Object supports exact identifier filtering
  0U,                   // Object does not support range identifier filtering
  1U,                   // Object supports mask identifier filtering
  1U                    // Object can buffer 1 message
};

static LPC_C_CANn_Type * const ptr_CANx[2] = { (LPC_C_CANn_Type *)LPC_C_CAN0_BASE, (LPC_C_CANn_Type *)LPC_C_CAN1_BASE };

// Local variables and structures
static uint8_t                     can_driver_powered    [CAN_CTRL_NUM];
static uint8_t                     can_driver_initialized[CAN_CTRL_NUM];
static uint8_t                     can_obj_cfg           [CAN_CTRL_NUM][(CAN0_OBJ_NUM > CAN1_OBJ_NUM) ? CAN0_OBJ_NUM : CAN1_OBJ_NUM];
static uint8_t                     can_stat_last         [CAN_CTRL_NUM];
static ARM_CAN_SignalUnitEvent_t   CAN_SignalUnitEvent   [CAN_CTRL_NUM];
static ARM_CAN_SignalObjectEvent_t CAN_SignalObjectEvent [CAN_CTRL_NUM];


// Helper Functions

/**
  \fn          void CANx_HW_Reset (uint8_t x)
  \brief       Reset CAN hardware (reset message objects and clear interrupts).
  \param[in]   x      Controller number (0..1)
*/
static void CANx_HW_Reset (uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;
  uint8_t          obj, obj_end;

  ptr_CAN = ptr_CANx[x];

  can_stat_last[x] = 0U;

  ptr_CAN->CNTL = 1U;                                   // Initialization

  obj_end = ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM);
  for (obj = 0U; obj < obj_end; obj++) {
    while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);
    ptr_CAN->IF1_CMDMSK_W = IF_CMDMSK_ARB_Msk | IF_CMDMSK_WR_RD_Msk;
    ptr_CAN->IF1_ARB2     = 0U;                         // Invalidate message object (MSGVAL = 0)
    ptr_CAN->IF1_CMDREQ   = obj + 1U;
    while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);

    ptr_CAN->IF1_CMDMSK_R = IF_CMDMSK_CLRINTPND_Msk;    // Clear interrupt pending
    ptr_CAN->IF1_CMDREQ   = obj + 1U;
    while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);

    can_obj_cfg[x][obj]    = ARM_CAN_OBJ_INACTIVE;
  }

  ptr_CAN->STAT = 0U;                                   // Clear interrupt status
}

/**
  \fn          void CANx_AbortSendMessage (uint32_t obj, uint8_t x)
  \brief       Abort send message.
  \param[in]   obj    Message object index
  \param[in]   x      Controller number (0..1)
*/
static void CANx_AbortSendMessage (uint32_t obj, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;

  ptr_CAN = ptr_CANx[x];

  while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);
  ptr_CAN->IF1_CMDMSK_R =  IF_CMDMSK_CTRL_Msk;
  ptr_CAN->IF1_CMDREQ   =  obj + 1U;
  while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);

  ptr_CAN->IF1_MCTRL   &= ~IF_MCTRL_TXRQST_Msk;         // Clear TXRQST bit

  ptr_CAN->IF1_CMDMSK_W =  IF_CMDMSK_CTRL_Msk |
                           IF_CMDMSK_WR_RD_Msk;
  ptr_CAN->IF1_CMDREQ   =  obj + 1U;
  while ((ptr_CAN->IF1_CMDREQ & IF_CMDREQ_BUSY_Msk) != 0U);
}


// CAN Driver Functions

/**
  \fn          ARM_DRIVER_VERSION CAN_GetVersion (void)
  \brief       Get driver version.
  \return      ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION CAN_GetVersion (void) { return can_driver_version; }

/**
  \fn          ARM_CAN_CAPABILITIES CAN0_GetCapabilities (void)
  \fn          ARM_CAN_CAPABILITIES CAN1_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      ARM_CAN_CAPABILITIES
*/
#if (RTE_CAN_CAN0 == 1U)
static ARM_CAN_CAPABILITIES CAN0_GetCapabilities (void) { return can_driver_capabilities[0U]; }
#endif
#if (RTE_CAN_CAN1 == 1U)
static ARM_CAN_CAPABILITIES CAN1_GetCapabilities (void) { return can_driver_capabilities[1U]; }
#endif
/**
  \fn          int32_t CANx_Initialize (ARM_CAN_SignalUnitEvent_t   cb_unit_event,
                                        ARM_CAN_SignalObjectEvent_t cb_object_event,
                                        uint8_t                     x)
  \brief       Initialize CAN interface and register signal (callback) functions.
  \param[in]   cb_unit_event   Pointer to ARM_CAN_SignalUnitEvent callback function
  \param[in]   cb_object_event Pointer to ARM_CAN_SignalObjectEvent callback function
  \param[in]   x               Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_Initialize (ARM_CAN_SignalUnitEvent_t   cb_unit_event,
                                ARM_CAN_SignalObjectEvent_t cb_object_event,
                                uint8_t                     x) {

  if (x >= CAN_CTRL_NUM)               { return ARM_DRIVER_ERROR; }
  if (can_driver_initialized[x] != 0U) { return ARM_DRIVER_OK;    }

  CAN_SignalUnitEvent  [x] = cb_unit_event;
  CAN_SignalObjectEvent[x] = cb_object_event;

  if (x == 0U) {
#if (RTE_CAN0_RD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN0_RD_PORT, RTE_CAN0_RD_BIT, RTE_CAN0_RD_FUNC | SCU_PIN_CFG_PULLUP_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN);
#endif
#if (RTE_CAN0_TD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN0_TD_PORT, RTE_CAN0_TD_BIT, RTE_CAN0_TD_FUNC | SCU_PIN_CFG_PULLUP_DIS);
#endif
  } else {
#if (RTE_CAN1_RD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN1_RD_PORT, RTE_CAN1_RD_BIT, RTE_CAN1_RD_FUNC | SCU_PIN_CFG_PULLUP_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN);
#endif
#if (RTE_CAN1_TD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN1_TD_PORT, RTE_CAN1_TD_BIT, RTE_CAN1_TD_FUNC | SCU_PIN_CFG_PULLUP_DIS);
#endif
  }

  can_driver_initialized[x] = 1U;

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_Initialize (ARM_CAN_SignalUnitEvent_t cb_unit_event, ARM_CAN_SignalObjectEvent_t cb_object_event) { return CANx_Initialize (cb_unit_event, cb_object_event, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_Initialize (ARM_CAN_SignalUnitEvent_t cb_unit_event, ARM_CAN_SignalObjectEvent_t cb_object_event) { return CANx_Initialize (cb_unit_event, cb_object_event, 1U); }
#endif

/**
  \fn          int32_t CANx_Uninitialize (uint8_t x)
  \brief       De-initialize CAN interface.
  \param[in]   x      Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_Uninitialize (uint8_t x) {

  if (x >= CAN_CTRL_NUM) { return ARM_DRIVER_ERROR; }

  if (x == 0U) {
#if (RTE_CAN0_RD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN0_RD_PORT, RTE_CAN0_RD_BIT, 0U);
#endif
#if (RTE_CAN0_TD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN0_TD_PORT, RTE_CAN0_TD_BIT, 0U);
#endif
  } else {
#if (RTE_CAN1_RD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN1_RD_PORT, RTE_CAN1_RD_BIT, 0U);
#endif
#if (RTE_CAN1_TD_PIN_EN == 1)
    SCU_PinConfigure (RTE_CAN1_TD_PORT, RTE_CAN1_TD_BIT, 0U);
#endif
  }

  can_driver_initialized[x] = 0U;

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_Uninitialize (void) { return CANx_Uninitialize (0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_Uninitialize (void) { return CANx_Uninitialize (1U); }
#endif

/**
  \fn          int32_t CANx_PowerControl (ARM_POWER_STATE state, uint8_t x)
  \brief       Control CAN interface power.
  \param[in]   state  Power state
                 - ARM_POWER_OFF :  power off: no operation possible
                 - ARM_POWER_LOW :  low power mode: retain state, detect and signal wake-up events
                 - ARM_POWER_FULL : power on: full operation at maximum performance
  \param[in]   x      Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_PowerControl (ARM_POWER_STATE state, uint8_t x) {

  if (x >= CAN_CTRL_NUM) { return ARM_DRIVER_ERROR; }

  switch (state) {
    case ARM_POWER_OFF:
      can_driver_powered[x] = 0U;
      if (x == 0U) {
        NVIC_DisableIRQ (C_CAN0_IRQn);

        LPC_CGU->BASE_APB3_CLK = (1U << 11) |   // Auto-block enable
                                 (9U << 24) ;   // Clock source: PLL1
        LPC_CCU1->CLK_APB3_CAN0_CFG |=  1U;     // Enable C_CAN0 Base Clock
        while ((LPC_CCU1->CLK_APB3_CAN0_CFG & 1U) == 0U);
      } else {
        NVIC_DisableIRQ (C_CAN1_IRQn);

        LPC_CGU->BASE_APB1_CLK = (1U << 11) |   // Auto-block enable
                                 (9U << 24) ;   // Clock source: PLL1
        LPC_CCU1->CLK_APB1_CAN1_CFG |=  1U;     // Enable C_CAN1 Base Clock
        while ((LPC_CCU1->CLK_APB1_CAN1_CFG & 1U) == 0U);
      }

      CANx_HW_Reset(x);

      if (x == 0U) {
        LPC_CCU1->CLK_APB3_CAN0_CFG &= ~1U;     // Disable C_CAN0 Base Clock
        while ((LPC_CCU1->CLK_APB3_CAN0_CFG & 1U) != 0U);
      } else {
        LPC_CCU1->CLK_APB1_CAN1_CFG &= ~1U;     // Disable C_CAN1 Base Clock
        while ((LPC_CCU1->CLK_APB1_CAN1_CFG & 1U) != 0U);
      }
      break;

    case ARM_POWER_FULL:
      if (can_driver_initialized[x] == 0U) { return ARM_DRIVER_ERROR; }
      if (can_driver_powered[x]     != 0U) { return ARM_DRIVER_OK;    }

      if (x == 0U) {
        LPC_CGU->BASE_APB3_CLK = (1U << 11) |   // Auto-block enable
                                 (9U << 24) ;   // Clock source: PLL1
        LPC_CCU1->CLK_APB3_CAN0_CFG |=  1U;     // Enable C_CAN0 Base Clock
        while ((LPC_CCU1->CLK_APB3_CAN0_CFG & 1U) == 0U);
      } else {
        LPC_CGU->BASE_APB1_CLK = (1U << 11) |   // Auto-block enable
                                 (9U << 24) ;   // Clock source: PLL1
        LPC_CCU1->CLK_APB1_CAN1_CFG |=  1U;     // Enable C_CAN1 Base Clock
        while ((LPC_CCU1->CLK_APB1_CAN1_CFG & 1U) == 0U);
      }

      CANx_HW_Reset(x);

      can_driver_powered[x] = 1U;

      if (x == 0U) {
        NVIC_ClearPendingIRQ (C_CAN0_IRQn);
        NVIC_EnableIRQ       (C_CAN0_IRQn);
      } else {
        NVIC_ClearPendingIRQ (C_CAN1_IRQn);
        NVIC_EnableIRQ       (C_CAN1_IRQn);
      }
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_PowerControl (ARM_POWER_STATE state) { return CANx_PowerControl (state, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_PowerControl (ARM_POWER_STATE state) { return CANx_PowerControl (state, 1U); }
#endif

/**
  \fn          uint32_t CAN_GetClock (void)
  \brief       Retrieve CAN base clock frequency.
  \return      base clock frequency
*/
uint32_t CAN_GetClock (void) {
  return GetClockFreq(9U);
}

/**
  \fn          int32_t CANx_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments, uint8_t x)
  \brief       Set bitrate for CAN interface.
  \param[in]   select       Bitrate selection
                 - ARM_CAN_BITRATE_NOMINAL : nominal (flexible data-rate arbitration) bitrate
                 - ARM_CAN_BITRATE_FD_DATA : flexible data-rate data bitrate
  \param[in]   bitrate      Bitrate
  \param[in]   bit_segments Bit segments settings
  \param[in]   x            Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;
  uint32_t         cntl, clkdiv, sjw, prop_seg, phase_seg1, phase_seg2, pclk, brp, tq_num;

  if (x >= CAN_CTRL_NUM)                 { return ARM_DRIVER_ERROR;               }
  if (select != ARM_CAN_BITRATE_NOMINAL) { return ARM_CAN_INVALID_BITRATE_SELECT; }
  if (can_driver_powered[x] == 0U)       { return ARM_DRIVER_ERROR;               }

  prop_seg   = (bit_segments & ARM_CAN_BIT_PROP_SEG_Msk  ) >> ARM_CAN_BIT_PROP_SEG_Pos;
  phase_seg1 = (bit_segments & ARM_CAN_BIT_PHASE_SEG1_Msk) >> ARM_CAN_BIT_PHASE_SEG1_Pos;
  phase_seg2 = (bit_segments & ARM_CAN_BIT_PHASE_SEG2_Msk) >> ARM_CAN_BIT_PHASE_SEG2_Pos;
  sjw        = (bit_segments & ARM_CAN_BIT_SJW_Msk       ) >> ARM_CAN_BIT_SJW_Pos;

  if (((prop_seg + phase_seg1) < 2U) || ((prop_seg + phase_seg1) > 16U)) { return ARM_CAN_INVALID_BIT_PROP_SEG;   }
  if (( phase_seg2             < 1U) || ( phase_seg2             >  8U)) { return ARM_CAN_INVALID_BIT_PHASE_SEG2; }
  if (( sjw                    < 1U) || ( sjw                    >  4U)) { return ARM_CAN_INVALID_BIT_SJW;        }

  ptr_CAN = ptr_CANx[x];

  tq_num = 1U + prop_seg + phase_seg1 + phase_seg2;
  pclk   = GetClockFreq(9U);
  clkdiv = 1U;
  while (1U) {
    if (clkdiv == 16U)  { return ARM_DRIVER_ERROR; }
    if (((pclk / clkdiv) <= 50000000U) && (((pclk / clkdiv) % (tq_num * bitrate)) == 0U)) { break; }
    clkdiv++;
  }
  brp    = pclk / (tq_num * bitrate * clkdiv);
  if (brp > 1024U) { return ARM_CAN_INVALID_BITRATE; }
  if (pclk >= (brp * tq_num * bitrate * clkdiv)) {
    if (((pclk - (brp * tq_num * bitrate * clkdiv)) * 1024U) > CAN_CLOCK_TOLERANCE) { return ARM_CAN_INVALID_BITRATE; }
  } else {
    if ((((brp * tq_num * bitrate * clkdiv) - pclk) * 1024U) > CAN_CLOCK_TOLERANCE) { return ARM_CAN_INVALID_BITRATE; }
  }

  cntl = ptr_CAN->CNTL;
  if ((cntl & (CNTL_CCE_Msk | CNTL_INIT_Msk)) != (CNTL_CCE_Msk | CNTL_INIT_Msk)) {
    ptr_CAN->CNTL = CNTL_CCE_Msk  |             // Configuration change enable
                    CNTL_INIT_Msk ;             // Initialization
  }

  ptr_CAN->BT     = ((brp - 1U) & BT_BRP_Msk) | ((sjw - 1U) << 6) | (((prop_seg + phase_seg1) - 1U) << 8) | ((phase_seg2 - 1U) << 12);
  ptr_CAN->BRPE   = ((brp - 1U) >> 6);
  ptr_CAN->CLKDIV =  clkdiv - 1U;
  ptr_CAN->CNTL   =  cntl;

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments) { return CANx_SetBitrate (select, bitrate, bit_segments, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments) { return CANx_SetBitrate (select, bitrate, bit_segments, 1U); }
#endif

/**
  \fn          int32_t CANx_SetMode (ARM_CAN_MODE mode, uint8_t x)
  \brief       Set operating mode for CAN interface.
  \param[in]   mode   Operating mode
                 - ARM_CAN_MODE_INITIALIZATION :    initialization mode
                 - ARM_CAN_MODE_NORMAL :            normal operation mode
                 - ARM_CAN_MODE_RESTRICTED :        restricted operation mode
                 - ARM_CAN_MODE_MONITOR :           bus monitoring mode
                 - ARM_CAN_MODE_LOOPBACK_INTERNAL : loopback internal mode
                 - ARM_CAN_MODE_LOOPBACK_EXTERNAL : loopback external mode
  \param[in]   x      Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_SetMode (ARM_CAN_MODE mode, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;
  uint32_t         event;

  if (x >= CAN_CTRL_NUM)           { return ARM_DRIVER_ERROR; }
  if (can_driver_powered[x] == 0U) { return ARM_DRIVER_ERROR; }

  ptr_CAN = ptr_CANx[x];

  event = 0U;
  switch (mode) {
    case ARM_CAN_MODE_INITIALIZATION:
      ptr_CAN->CNTL = CNTL_INIT_Msk;
      event = ARM_CAN_EVENT_UNIT_BUS_OFF;
      break;
    case ARM_CAN_MODE_NORMAL:
      ptr_CAN->CNTL = CNTL_IE_Msk;
      if (CAN_SignalUnitEvent[x] != NULL) { ptr_CAN->CNTL |= CNTL_SIE_Msk | CNTL_EIE_Msk; }
      event = ARM_CAN_EVENT_UNIT_ACTIVE;
      break;
    case ARM_CAN_MODE_RESTRICTED:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_CAN_MODE_MONITOR:
      ptr_CAN->CNTL = CNTL_IE_Msk | CNTL_TEST_Msk;
      if (CAN_SignalUnitEvent[x] != NULL) { ptr_CAN->CNTL |= CNTL_SIE_Msk | CNTL_EIE_Msk; }
      ptr_CAN->TEST  = TEST_SILENT_Msk;
      event = ARM_CAN_EVENT_UNIT_PASSIVE;
      break;
    case ARM_CAN_MODE_LOOPBACK_INTERNAL:
      ptr_CAN->CNTL = CNTL_IE_Msk | CNTL_TEST_Msk;
      if (CAN_SignalUnitEvent[x] != NULL) { ptr_CAN->CNTL |= CNTL_SIE_Msk | CNTL_EIE_Msk; }
      ptr_CAN->TEST  = TEST_LBACK_Msk | TEST_SILENT_Msk;
      event = ARM_CAN_EVENT_UNIT_PASSIVE;
      break;
    case ARM_CAN_MODE_LOOPBACK_EXTERNAL:
      ptr_CAN->CNTL = CNTL_IE_Msk | CNTL_TEST_Msk;
      if (CAN_SignalUnitEvent[x] != NULL) { ptr_CAN->CNTL |= CNTL_SIE_Msk | CNTL_EIE_Msk; }
      ptr_CAN->TEST  = TEST_LBACK_Msk;
      event = ARM_CAN_EVENT_UNIT_ACTIVE;
      break;
    default:
      return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((CAN_SignalUnitEvent[x] != NULL) && (event != 0U)) { CAN_SignalUnitEvent[x](event); }

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_SetMode (ARM_CAN_MODE mode) { return CANx_SetMode (mode, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_SetMode (ARM_CAN_MODE mode) { return CANx_SetMode (mode, 1U); }
#endif

/**
  \fn          ARM_CAN_OBJ_CAPABILITIES CANx_ObjectGetCapabilities (uint32_t obj_idx, uint8_t x)
  \brief       Retrieve capabilities of an object.
  \param[in]   obj_idx  Object index
  \param[in]   x        Controller number (0..1)
  \return      ARM_CAN_OBJ_CAPABILITIES
*/
ARM_CAN_OBJ_CAPABILITIES CANx_ObjectGetCapabilities (uint32_t obj_idx, uint8_t x) {
  ARM_CAN_OBJ_CAPABILITIES obj_cap_null;

  if ((x >= CAN_CTRL_NUM) || (obj_idx >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM))) {
    memset (&obj_cap_null, 0U, sizeof(ARM_CAN_OBJ_CAPABILITIES));
    return obj_cap_null;
  }

  return can_object_capabilities;
}
#if (RTE_CAN_CAN0 == 1U)
ARM_CAN_OBJ_CAPABILITIES CAN0_ObjectGetCapabilities (uint32_t obj_idx) { return CANx_ObjectGetCapabilities (obj_idx, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
ARM_CAN_OBJ_CAPABILITIES CAN1_ObjectGetCapabilities (uint32_t obj_idx) { return CANx_ObjectGetCapabilities (obj_idx, 1U); }
#endif

/**
  \fn          int32_t CANx_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg, uint8_t x)
  \brief       Add or remove filter for message reception.
  \param[in]   obj_idx      Object index of object that filter should be or is assigned to
  \param[in]   operation    Operation on filter
                 - ARM_CAN_FILTER_ID_EXACT_ADD :       add    exact id filter
                 - ARM_CAN_FILTER_ID_EXACT_REMOVE :    remove exact id filter
                 - ARM_CAN_FILTER_ID_RANGE_ADD :       add    range id filter
                 - ARM_CAN_FILTER_ID_RANGE_REMOVE :    remove range id filter
                 - ARM_CAN_FILTER_ID_MASKABLE_ADD :    add    maskable id filter
                 - ARM_CAN_FILTER_ID_MASKABLE_REMOVE : remove maskable id filter
  \param[in]   id           ID or start of ID range (depending on filter type)
  \param[in]   arg          Mask or end of ID range (depending on filter type)
  \param[in]   x            Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;

  if (x >= CAN_CTRL_NUM)                              { return ARM_DRIVER_ERROR;           }
  if (obj_idx >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM)) { return ARM_DRIVER_ERROR_PARAMETER; }
  if (can_driver_powered[x] == 0U)                    { return ARM_DRIVER_ERROR;           }

  ptr_CAN = ptr_CANx[x];
  if ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U)   { return ARM_DRIVER_ERROR_BUSY;      }

  ptr_CAN->IF1_CMDMSK_R =                               // Read
                           IF_CMDMSK_ARB_Msk   ;        // Access arbitration
  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Read from message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for read to finish
                                                        // If arbitration in non-zero means filter is already set
  switch (operation) {
    case ARM_CAN_FILTER_ID_EXACT_ADD:
    case ARM_CAN_FILTER_ID_MASKABLE_ADD:
      if ((ptr_CAN->IF1_ARB1 != 0U) || (ptr_CAN->IF1_ARB2 != 0U)) { return ARM_DRIVER_ERROR; }
      break;
    case ARM_CAN_FILTER_ID_EXACT_REMOVE:
    case ARM_CAN_FILTER_ID_MASKABLE_REMOVE:
      if ((ptr_CAN->IF1_ARB1 == 0U) && (ptr_CAN->IF1_ARB2 == 0U)) { return ARM_DRIVER_OK;    }
      break;
    case ARM_CAN_FILTER_ID_RANGE_ADD:
    case ARM_CAN_FILTER_ID_RANGE_REMOVE:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  ptr_CAN->IF1_CMDMSK_W =  IF_CMDMSK_WR_RD_Msk |        // Write
                           IF_CMDMSK_ARB_Msk   ;        // Access arbitration
  ptr_CAN->IF1_ARB2     =  0U;                          // Invalidate message object (MSGVAL = 0)
  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Write to message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for write to finish

  ptr_CAN->IF1_CMDMSK_W =  IF_CMDMSK_WR_RD_Msk |        // Write
                           IF_CMDMSK_MASK_Msk  |        // Access mask
                           IF_CMDMSK_ARB_Msk   |        // Access arbitration
                           IF_CMDMSK_CTRL_Msk  ;        // Access control bits

  switch (operation) {
    case ARM_CAN_FILTER_ID_EXACT_ADD:
      if ((id & ARM_CAN_ID_IDE_Msk) != 0U) {            // Extended Identifier
        ptr_CAN->IF1_MSK1 =   0xFFFFU;
        ptr_CAN->IF1_MSK2 =   0xBFFFU;
        ptr_CAN->IF1_ARB1 =   id         & IF_ARB1_ID15_0_Msk;
        ptr_CAN->IF1_ARB2 = ((id  >> 16) & IF_ARB2_ID28_16_Msk) | IF_ARB2_XTD_Msk;
      } else {                                          // Standard Identifier
        ptr_CAN->IF1_MSK1 =   0U;
        ptr_CAN->IF1_MSK2 =   0xBFFCU;
        ptr_CAN->IF1_ARB1 =   0U;
        ptr_CAN->IF1_ARB2 =  (id  <<  2) & IF_ARB2_ID28_16_Msk;
      }
      break;
    case ARM_CAN_FILTER_ID_MASKABLE_ADD:
      if ((id & ARM_CAN_ID_IDE_Msk) != 0U) {            // Extended Identifier
        ptr_CAN->IF1_MSK1 =   arg        & IF_MSK1_MSK15_0_Msk;
        ptr_CAN->IF1_MSK2 = ((arg >> 16) & IF_MSK2_MSK28_16_Msk) | IF_MSK2_MXTD_Msk;
        ptr_CAN->IF1_ARB1 =   id         & IF_ARB1_ID15_0_Msk;
        ptr_CAN->IF1_ARB2 = ((id  >> 16) & IF_ARB2_ID28_16_Msk)  | IF_ARB2_XTD_Msk;
      } else {                                          // Standard Identifier
        ptr_CAN->IF1_MSK1 =   0U;
        ptr_CAN->IF1_MSK2 = ((arg <<  2) & IF_MSK2_MSK28_16_Msk) | IF_MSK2_MXTD_Msk;
        ptr_CAN->IF1_ARB1 =   0U;
        ptr_CAN->IF1_ARB2 =  (id  <<  2) & IF_ARB2_ID28_16_Msk;
      }
      break;
    case ARM_CAN_FILTER_ID_EXACT_REMOVE:
    case ARM_CAN_FILTER_ID_MASKABLE_REMOVE:
      ptr_CAN->IF1_MSK1 = 0U;
      ptr_CAN->IF1_MSK2 = 0U;
      ptr_CAN->IF1_ARB1 = 0U;
      ptr_CAN->IF1_ARB2 = 0U;
      break;
    case ARM_CAN_FILTER_ID_RANGE_ADD:
    case ARM_CAN_FILTER_ID_RANGE_REMOVE:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Write to message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for write to finish

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg) { return CANx_ObjectSetFilter (obj_idx, operation, id, arg, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg) { return CANx_ObjectSetFilter (obj_idx, operation, id, arg, 1U); }
#endif

/**
  \fn          int32_t CANx_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg, uint8_t x)
  \brief       Configure object.
  \param[in]   obj_idx  Object index
  \param[in]   obj_cfg  Object configuration state
                 - ARM_CAN_OBJ_INACTIVE :       deactivate object
                 - ARM_CAN_OBJ_RX :             configure object for reception
                 - ARM_CAN_OBJ_TX :             configure object for transmission
                 - ARM_CAN_OBJ_RX_RTR_TX_DATA : configure object that on RTR reception automatically transmits Data Frame
                 - ARM_CAN_OBJ_TX_RTR_RX_DATA : configure object that transmits RTR and automatically receives Data Frame
  \param[in]   x        Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;

  if (x >= CAN_CTRL_NUM)                              { return ARM_DRIVER_ERROR;           }
  if (obj_idx >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM)) { return ARM_DRIVER_ERROR_PARAMETER; }
  if (can_driver_powered[x] == 0U)                    { return ARM_DRIVER_ERROR;           }

  ptr_CAN = ptr_CANx[x];
  if ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U)   { return ARM_DRIVER_ERROR_BUSY;      }

  ptr_CAN->IF1_CMDMSK_R =                               // Read
                           IF_CMDMSK_MASK_Msk  |        // Access mask
                           IF_CMDMSK_ARB_Msk   |        // Access arbitration
                           IF_CMDMSK_CTRL_Msk  ;        // Access control bits
  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Write to message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for read to finish

  ptr_CAN->IF1_CMDMSK_W =  IF_CMDMSK_WR_RD_Msk |        // Write
                           IF_CMDMSK_MASK_Msk  |        // Access mask
                           IF_CMDMSK_ARB_Msk   |        // Access arbitration
                           IF_CMDMSK_CTRL_Msk  ;        // Access control bits

  switch (obj_cfg) {
    case ARM_CAN_OBJ_INACTIVE:
      ptr_CAN->IF1_ARB2   =  0U;                        // Invalidate message object (MSGVAL = 0)
      ptr_CAN->IF1_MCTRL  =  0U;                        // Clear control register
      break;
    case ARM_CAN_OBJ_TX:
      ptr_CAN->IF1_MSK2  |=  IF_MSK2_MDIR_Msk   ;       // Use DIR for acceptance filtering
      ptr_CAN->IF1_ARB2  |=  IF_ARB2_DIR_Msk    |       // Tx object
                             IF_ARB2_MSGVAL_Msk ;       // Message is Valid
      ptr_CAN->IF1_MCTRL  =  IF_MCTRL_EOB_Msk   |       // End of Buffer
                             IF_MCTRL_TXIE_Msk  ;       // Tx Interrupt Enable
      break;
    case ARM_CAN_OBJ_RX:
      ptr_CAN->IF1_MSK2  |=  IF_MSK2_MDIR_Msk   ;       // Use DIR for acceptance filtering
      ptr_CAN->IF1_ARB2  &= ~IF_ARB2_DIR_Msk    ;       // Rx object
      ptr_CAN->IF1_ARB2  |=  IF_ARB2_MSGVAL_Msk ;       // Message is Valid
      ptr_CAN->IF1_MCTRL  =  IF_MCTRL_UMASK_Msk |       // Use mask for acceptance filtering
                             IF_MCTRL_EOB_Msk   |       // End of Buffer
                             IF_MCTRL_RXIE_Msk  |       // Rx Interrupt Enable
                             8U                 ;       // Receive 8 bytes
      break;
    case ARM_CAN_OBJ_RX_RTR_TX_DATA:
      ptr_CAN->IF1_MSK2  &= ~IF_MSK2_MDIR_Msk   ;       // Do not use DIR for acceptance filtering
      ptr_CAN->IF1_ARB2  |=  IF_ARB2_DIR_Msk    |       // Tx object
                             IF_ARB2_MSGVAL_Msk ;       // Message is Valid
      ptr_CAN->IF1_MCTRL  =  IF_MCTRL_UMASK_Msk |       // Use mask for acceptance filtering
                             IF_MCTRL_EOB_Msk   |       // End of Buffer
                             IF_MCTRL_TXIE_Msk  |       // Tx Interrupt Enable
                             IF_MCTRL_RMTEN_Msk ;       // Remote Enable
      break;
    case ARM_CAN_OBJ_TX_RTR_RX_DATA:
      ptr_CAN->IF1_MSK2  &= ~IF_MSK2_MDIR_Msk   ;       // Do not use DIR for acceptance filtering
      ptr_CAN->IF1_ARB2  &= ~IF_ARB2_DIR_Msk    ;       // Rx object
      ptr_CAN->IF1_ARB2  |=  IF_ARB2_MSGVAL_Msk ;       // Message is Valid
      ptr_CAN->IF1_MCTRL  =  IF_MCTRL_UMASK_Msk |       // Use mask for acceptance filtering
                             IF_MCTRL_EOB_Msk   |       // End of Buffer
                             IF_MCTRL_RXIE_Msk  ;       // Rx Interrupt Enable
      break;
    default:
      return ARM_DRIVER_ERROR;
  }
  can_obj_cfg[x][obj_idx] = obj_cfg;

  ptr_CAN->IF1_CMDREQ = obj_idx + 1U;                   // Write to message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for write to finish

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg) { return CANx_ObjectConfigure (obj_idx, obj_cfg, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg) { return CANx_ObjectConfigure (obj_idx, obj_cfg, 1U); }
#endif

/**
  \fn          int32_t CANx_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size, uint8_t x)
  \brief       Send message on CAN bus.
  \param[in]   obj_idx  Object index
  \param[in]   msg_info Pointer to CAN message information
  \param[in]   data     Pointer to data buffer
  \param[in]   size     Number of data bytes to send
  \param[in]   x        Controller number (0..1)
  \return      value >= 0  number of data bytes accepted to send
  \return      value < 0   execution status
*/
static int32_t CANx_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;
  uint32_t         arb1, arb2, mctrl;

  if (x >= CAN_CTRL_NUM)                              { return ARM_DRIVER_ERROR;           }
  if (obj_idx >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM)) { return ARM_DRIVER_ERROR_PARAMETER; }
  if (can_driver_powered[x]   == 0U)                  { return ARM_DRIVER_ERROR;           }
  if (can_obj_cfg[x][obj_idx] == ARM_CAN_OBJ_RX)      { return ARM_DRIVER_ERROR;           }

  ptr_CAN = ptr_CANx[x];

  if ((((ptr_CAN->TXREQ2 << 16) | (ptr_CAN->TXREQ1 & 0xFFFFU)) & (1U << obj_idx)) != 0U) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  if ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U)   { return ARM_DRIVER_ERROR_BUSY;      }

  ptr_CAN->IF1_CMDMSK_R =                               // Read
                           IF_CMDMSK_ARB_Msk   |        // Access arbitration
                           IF_CMDMSK_CTRL_Msk  |        // Access control bits
                           IF_CMDMSK_NEWDAT_Msk;        // Clear NEWDAT bit
  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Read from message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for read to finish

  mctrl = ptr_CAN->IF1_MCTRL;                           // Store current value of MCTRL register
  arb1  = ptr_CAN->IF1_ARB1;                            // Store current value of ARB1 register
  arb2  = ptr_CAN->IF1_ARB2;                            // Store current value of ARB2 register

  // Prepare arb1..2
  if (can_obj_cfg[x][obj_idx] == ARM_CAN_OBJ_TX) {      // For Tx object id is prepared here
    if (msg_info->id & ARM_CAN_ID_IDE_Msk) {            // Extended Identifier
      arb1 =   msg_info->id        & IF_ARB1_ID15_0_Msk;
      arb2 = ((msg_info->id >> 16) & IF_ARB2_ID28_16_Msk)  | IF_ARB2_XTD_Msk | IF_ARB2_MSGVAL_Msk;
    } else {                                            // Standard Identifier
      arb1 =   0U;
      arb2 = ((msg_info->id <<  2) & IF_ARB2_ID28_16_Msk)                    | IF_ARB2_MSGVAL_Msk;
    }
  }

  switch (can_obj_cfg[x][obj_idx]) {
    case ARM_CAN_OBJ_INACTIVE:
    case ARM_CAN_OBJ_RX:
      return ARM_DRIVER_ERROR;
    case ARM_CAN_OBJ_TX:
      if (msg_info->rtr == 0U) {                        // If Data frame transmit requested
        arb2 |=  IF_ARB2_DIR_Msk;                       // Tx object
      } else {                                          // else if Remote Transmission Request frame transmit requested
        size     =  msg_info->dlc;                      // Prepare number of data bytes to request
      }
      if (size > 8U) { size = 8U; }
      mctrl      = (mctrl & ~0xFU)      |
                    IF_MCTRL_TXRQST_Msk |               // Set Tx Request
                    size                ;               // Number of bytes to transmit or requested by RTR
      break;
    case ARM_CAN_OBJ_RX_RTR_TX_DATA:
      mctrl      = (mctrl & ~0xFU)      |
                    size                ;               // Number of bytes to transmit or requested by RTR
      break;
    case ARM_CAN_OBJ_TX_RTR_RX_DATA:
      if (msg_info->rtr) {                              // If Remote Transmission Request frame transmit requested
        size     =  msg_info->dlc;                      // Prepare number of data bytes to request
      }
      if (size > 8U) { size = 8U; }
      mctrl      = (mctrl & ~0xFU)      |
                    IF_MCTRL_TXRQST_Msk |               // Set Tx Request
                    size                ;               // Number of bytes requested by RTR
      break;
    default:
      return ARM_DRIVER_ERROR;
  }                                                     // Store object type information

  ptr_CAN->IF1_CMDMSK_W =  IF_CMDMSK_WR_RD_Msk |        // Write
                           IF_CMDMSK_ARB_Msk   |        // Access arbitration
                           IF_CMDMSK_CTRL_Msk  ;        // Access control bits
  ptr_CAN->IF1_ARB1  = arb1;
  ptr_CAN->IF1_ARB2  = arb2;
  ptr_CAN->IF1_MCTRL = mctrl;

  if (can_obj_cfg[x][obj_idx] != ARM_CAN_OBJ_TX_RTR_RX_DATA) {
    ptr_CAN->IF1_DA1   = (((uint16_t)(data[1])) << 8) | data[0];
    ptr_CAN->IF1_DA2   = (((uint16_t)(data[3])) << 8) | data[2];
    ptr_CAN->IF1_CMDMSK_W |= IF_CMDMSK_DATA_A_Msk;      // Access data bytes 0..3
    if (size > 4) {
      ptr_CAN->IF1_DB1 = (((uint16_t)(data[5])) << 8) | data[4];
      ptr_CAN->IF1_DB2 = (((uint16_t)(data[7])) << 8) | data[6];
      ptr_CAN->IF1_CMDMSK_W |= IF_CMDMSK_DATA_B_Msk;    // Access data bytes 4..7
    }
  }

  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Write to message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for write to finish

  return ((int32_t)size);
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size) { return CANx_MessageSend (obj_idx, msg_info, data, size, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size) { return CANx_MessageSend (obj_idx, msg_info, data, size, 1U); }
#endif

/**
  \fn          int32_t CANx_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size, uint8_t x)
  \brief       Read message received on CAN bus.
  \param[in]   obj_idx  Object index
  \param[out]  msg_info Pointer to read CAN message information
  \param[out]  data     Pointer to data buffer for read data
  \param[in]   size     Maximum number of data bytes to read
  \param[in]   x        Controller number (0..1)
  \return      value >= 0  number of data bytes read
  \return      value < 0   execution status
*/
static int32_t CANx_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;

  if (x >= CAN_CTRL_NUM)                                     { return ARM_DRIVER_ERROR;           }
  if (obj_idx >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM))        { return ARM_DRIVER_ERROR_PARAMETER; }
  if (can_driver_powered[x]   == 0U)                         { return ARM_DRIVER_ERROR;           }
  if (can_obj_cfg[x][obj_idx] == ARM_CAN_OBJ_TX)             { return ARM_DRIVER_ERROR;           }
  if (can_obj_cfg[x][obj_idx] == ARM_CAN_OBJ_RX_RTR_TX_DATA) { return ARM_DRIVER_ERROR;           }

  ptr_CAN = ptr_CANx[x];

  if (size > 8U) { size = 8U; }

  ptr_CAN->IF1_CMDMSK_R =                               // Read
                           IF_CMDMSK_ARB_Msk    |       // Access arbitration
                           IF_CMDMSK_CTRL_Msk   |       // Access control bits
                           IF_CMDMSK_NEWDAT_Msk |       // Access New Data bit (clear it)
                           IF_CMDMSK_DATA_B_Msk |       // Access data bytes 4..7
                           IF_CMDMSK_DATA_A_Msk ;       // Access data bytes 0..3
  ptr_CAN->IF1_CMDREQ   =  obj_idx + 1U;                // Read from message object
  while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U); // Wait for read to finish

  if ((ptr_CAN->IF1_ARB2 & IF_ARB2_XTD_Msk) != 0U) {    // Extended Identifier
    msg_info->id = (0x1FFFFFFFU & (((ptr_CAN->IF1_ARB2 & IF_ARB2_ID28_16_Msk) << 16) | (ptr_CAN->IF1_ARB1 & IF_ARB1_ID15_0_Msk))) | ARM_CAN_ID_IDE_Msk;
  } else {                                              // Standard Identifier
    msg_info->id = (0x000007FFU &  ((ptr_CAN->IF1_ARB2 & IF_ARB2_ID28_16_Msk) >>  2));
  }
  msg_info->rtr = 0U;
  msg_info->dlc = ptr_CAN->IF1_MCTRL & IF_MCTRL_DLC3_0_Msk;

  if (size > msg_info->dlc) { size = msg_info->dlc; }

  if (size > 0U) {               data[0] = (ptr_CAN->IF1_DA1);
    if (size > 1U) {             data[1] = (ptr_CAN->IF1_DA1 >> 8);
      if (size > 2U) {           data[2] = (ptr_CAN->IF1_DA2);
        if (size > 3U) {         data[3] = (ptr_CAN->IF1_DA2 >> 8);
          if (size > 4U) {       data[4] = (ptr_CAN->IF1_DB1);
            if (size > 5U) {     data[5] = (ptr_CAN->IF1_DB1 >> 8);
              if (size > 6U) {   data[6] = (ptr_CAN->IF1_DB2);
                if (size > 7U) { data[7] = (ptr_CAN->IF1_DB2 >> 8); }
              }
            }
          }
        }
      }
    }
  }

  if (ptr_CAN->IF1_MCTRL & IF_MCTRL_MSGLST_Msk) {
    // If message was lost (MSGLST=1), clear this bit for new reception as now
    // the message was read-out and is free for new reception
    ptr_CAN->IF1_MCTRL   &= ~(IF_MCTRL_MSGLST_Msk | IF_MCTRL_NEWDAT_Msk | IF_MCTRL_INTPND_Msk);
    ptr_CAN->IF1_CMDMSK_W =   IF_CMDMSK_CTRL_Msk |
                              IF_CMDMSK_WR_RD_Msk;
    ptr_CAN->IF1_CMDREQ   =   obj_idx + 1U;
    while ((ptr_CAN->IF1_CMDREQ&IF_CMDREQ_BUSY_Msk)!=0U);       // Wait for read to finish
  }

  return ((int32_t)size);
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size) { return CANx_MessageRead (obj_idx, msg_info, data, size, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size) { return CANx_MessageRead (obj_idx, msg_info, data, size, 1U); }
#endif

/**
  \fn          int32_t CANx_Control (uint32_t control, uint32_t arg, uint8_t x)
  \brief       Control CAN interface.
  \param[in]   control  Operation
                 - ARM_CAN_SET_FD_MODE :            set FD operation mode
                 - ARM_CAN_ABORT_MESSAGE_SEND :     abort sending of CAN message
                 - ARM_CAN_CONTROL_RETRANSMISSION : enable/disable automatic retransmission
                 - ARM_CAN_SET_TRANSCEIVER_DELAY :  set transceiver delay
  \param[in]   arg      Argument of operation
  \param[in]   x        Controller number (0..1)
  \return      execution status
*/
static int32_t CANx_Control (uint32_t control, uint32_t arg, uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;

  if (x >= CAN_CTRL_NUM)           { return ARM_DRIVER_ERROR; }
  if (can_driver_powered[x] == 0U) { return ARM_DRIVER_ERROR; }

  ptr_CAN = ptr_CANx[x];

  switch (control & ARM_CAN_CONTROL_Msk) {
    case ARM_CAN_ABORT_MESSAGE_SEND:
      if (arg >= ((x) ? CAN1_OBJ_NUM : CAN0_OBJ_NUM)) { return ARM_DRIVER_ERROR_PARAMETER; }
      if (can_obj_cfg[x][arg] != ARM_CAN_OBJ_TX)      { return ARM_DRIVER_ERROR;           }
      CANx_AbortSendMessage (arg, x);
      break;
    case ARM_CAN_CONTROL_RETRANSMISSION:
      switch (arg) {
        case 0:
          ptr_CAN->CNTL |=  CNTL_DAR_Msk;       // Disable automatic retransmission
          break;
        case 1:
          ptr_CAN->CNTL &= ~CNTL_DAR_Msk;       // Enable automatic retransmission
          break;
        default:
          return ARM_DRIVER_ERROR_PARAMETER;
      }
      break;
    case ARM_CAN_SET_FD_MODE:
    case ARM_CAN_SET_TRANSCEIVER_DELAY:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}
#if (RTE_CAN_CAN0 == 1U)
static int32_t CAN0_Control (uint32_t control, uint32_t arg) { return CANx_Control (control, arg, 0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static int32_t CAN1_Control (uint32_t control, uint32_t arg) { return CANx_Control (control, arg, 1U); }
#endif

/**
  \fn          ARM_CAN_STATUS CANx_GetStatus (uint8_t x)
  \brief       Get CAN status.
  \param[in]   x      Controller number (0..1)
  \return      CAN status ARM_CAN_STATUS
*/
static ARM_CAN_STATUS CANx_GetStatus (uint8_t x) {
  LPC_C_CANn_Type *ptr_CAN;
  ARM_CAN_STATUS   can_status;
  uint32_t         stat, ec;

  if ((x >= CAN_CTRL_NUM) || (can_driver_powered[x] == 0U)) {
    memset(&can_status, 0U, sizeof(ARM_CAN_STATUS));
    return can_status;
  }

  ptr_CAN = ptr_CANx[x];
  stat    = ptr_CAN->STAT;
  ec      = ptr_CAN->EC;

  if       ((ptr_CAN->CNTL & CNTL_INIT_Msk)    != 0U)  { can_status.unit_state = ARM_CAN_UNIT_STATE_INACTIVE; }
  else if  (((ptr_CAN->CNTL & CNTL_TEST_Msk)   != 0U) &&
            ((ptr_CAN->TEST & TEST_SILENT_Msk) != 0U)) { can_status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;  }
  else if  ((stat & STAT_BOFF_Msk)  != 0U)             { can_status.unit_state = ARM_CAN_UNIT_STATE_INACTIVE; }
  else if  ((stat & STAT_EPASS_Msk) != 0U)             { can_status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;  }
  else                                                 { can_status.unit_state = ARM_CAN_UNIT_STATE_ACTIVE;   }

  switch (stat & STAT_LEC_Msk) {
    case 0:
      can_status.last_error_code = ARM_CAN_LEC_NO_ERROR;
      break;
    case 1:
      can_status.last_error_code = ARM_CAN_LEC_STUFF_ERROR;
      break;
    case 2:
      can_status.last_error_code = ARM_CAN_LEC_FORM_ERROR;
      break;
    case 3:
      can_status.last_error_code = ARM_CAN_LEC_ACK_ERROR;
      break;
    case 4:
    case 5:
      can_status.last_error_code = ARM_CAN_LEC_BIT_ERROR;
      break;
    case 6:
      can_status.last_error_code = ARM_CAN_LEC_CRC_ERROR;
      break;
    case 7:
      can_status.last_error_code = ARM_CAN_LEC_NO_ERROR;
      break;
  }

  can_status.tx_error_count = (uint8_t)((ec & EC_TEC_7_0_Msk));
  if ((ec & EC_RP_Msk) != 0U) {
    can_status.rx_error_count = 128U;
  } else {
    can_status.rx_error_count = (uint8_t)((ec & EC_REC_6_0_Msk) >> 8);
  }

  return can_status;
}
#if (RTE_CAN_CAN0 == 1U)
static ARM_CAN_STATUS CAN0_GetStatus (void) { return CANx_GetStatus (0U); }
#endif
#if (RTE_CAN_CAN1 == 1U)
static ARM_CAN_STATUS CAN1_GetStatus (void) { return CANx_GetStatus (1U); }
#endif

/**
  \fn          void CAN0_IRQHandler (void)
  \brief       CAN0 Interrupt Routine (IRQ).
*/
#if (RTE_CAN_CAN0 == 1U)
void CAN0_IRQHandler (void) {
  uint32_t       obj_idx, stat;

  while (1) {
    obj_idx = LPC_C_CAN0->INT; if (obj_idx == 0U) { break; }
    if (obj_idx && (obj_idx <= 0x20U)) {        // Message Object Interrupt
      LPC_C_CAN0->IF2_CMDMSK_R = IF_CMDMSK_CTRL_Msk | IF_CMDMSK_CLRINTPND_Msk;
      LPC_C_CAN0->IF2_CMDREQ   = obj_idx;
      while (LPC_C_CAN0->IF2_CMDREQ & IF_CMDREQ_BUSY_Msk);
      obj_idx --;
      if (CAN_SignalObjectEvent[0] != NULL) {
        switch (can_obj_cfg[0][obj_idx]) {
          case ARM_CAN_OBJ_INACTIVE:
            break;
          case ARM_CAN_OBJ_TX:
            CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_SEND_COMPLETE);
            break;
          case ARM_CAN_OBJ_RX:
            if (LPC_C_CAN0->IF2_MCTRL & IF_MCTRL_MSGLST_Msk) {
              CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_RECEIVE | ARM_CAN_EVENT_RECEIVE_OVERRUN);
            } else {
              CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_RECEIVE);
            }
            break;
          case ARM_CAN_OBJ_RX_RTR_TX_DATA:
            CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_SEND_COMPLETE);
            break;
          case ARM_CAN_OBJ_TX_RTR_RX_DATA:
            if (LPC_C_CAN0->IF2_MCTRL & IF_MCTRL_NEWDAT_Msk) {
              if (LPC_C_CAN0->IF2_MCTRL & IF_MCTRL_MSGLST_Msk) {
                CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_RECEIVE | ARM_CAN_EVENT_RECEIVE_OVERRUN);
              } else {
                CAN_SignalObjectEvent[0](obj_idx, ARM_CAN_EVENT_RECEIVE);
              }
          }
          break;
        default:
          break;
      }
    }
    } else if (obj_idx == 0x8000U) {            // Status interrupt
      stat = LPC_C_CAN0->STAT;
      if (CAN_SignalUnitEvent[0] != NULL) { 
        if ((stat ^ can_stat_last[0]) & (STAT_BOFF_Msk | STAT_EPASS_Msk | STAT_EWARN_Msk)) {
          if      (stat & (stat ^ can_stat_last[0]) & STAT_BOFF_Msk )        { CAN_SignalUnitEvent[0](ARM_CAN_EVENT_UNIT_BUS_OFF); }
          else if (stat & (stat ^ can_stat_last[0]) & STAT_EPASS_Msk)        { CAN_SignalUnitEvent[0](ARM_CAN_EVENT_UNIT_PASSIVE); }
          else if (stat & (stat ^ can_stat_last[0]) & STAT_EWARN_Msk)        { CAN_SignalUnitEvent[0](ARM_CAN_EVENT_UNIT_WARNING); }
          else if (stat & (STAT_BOFF_Msk | STAT_EPASS_Msk | STAT_EWARN_Msk)) { CAN_SignalUnitEvent[0](ARM_CAN_EVENT_UNIT_ACTIVE);  }
        }
        can_stat_last[0] = stat;
      }
    }
  }
}
#endif

/**
  \fn          void CAN1_IRQHandler (void)
  \brief       CAN1 Interrupt Routine (IRQ).
*/
#if (RTE_CAN_CAN1 == 1U)
void CAN1_IRQHandler (void) {
  uint32_t       obj_idx, stat;

  while (1) {
    obj_idx = LPC_C_CAN1->INT; if (obj_idx == 0U) { break; }
    if (obj_idx && (obj_idx <= 0x20U)) {        // Message Object Interrupt
      LPC_C_CAN1->IF2_CMDMSK_R = IF_CMDMSK_CTRL_Msk | IF_CMDMSK_CLRINTPND_Msk;
      LPC_C_CAN1->IF2_CMDREQ   = obj_idx;
      while (LPC_C_CAN1->IF2_CMDREQ & IF_CMDREQ_BUSY_Msk);
      obj_idx --;
      if (CAN_SignalObjectEvent[1] != NULL) {
        switch (can_obj_cfg[1][obj_idx]) {
          case ARM_CAN_OBJ_INACTIVE:
            break;
          case ARM_CAN_OBJ_TX:
            CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_SEND_COMPLETE);
            break;
          case ARM_CAN_OBJ_RX:
            if (LPC_C_CAN1->IF2_MCTRL & IF_MCTRL_MSGLST_Msk) {
              CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_RECEIVE | ARM_CAN_EVENT_RECEIVE_OVERRUN);
            } else {
              CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_RECEIVE);
            }
            break;
          case ARM_CAN_OBJ_RX_RTR_TX_DATA:
            CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_SEND_COMPLETE);
            break;
          case ARM_CAN_OBJ_TX_RTR_RX_DATA:
            if (LPC_C_CAN1->IF2_MCTRL & IF_MCTRL_NEWDAT_Msk) {
              if (LPC_C_CAN1->IF2_MCTRL & IF_MCTRL_MSGLST_Msk) {
                CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_RECEIVE | ARM_CAN_EVENT_RECEIVE_OVERRUN);
              } else {
                CAN_SignalObjectEvent[1](obj_idx, ARM_CAN_EVENT_RECEIVE);
              }
          }
          break;
        default:
          break;
      }
    }
    } else if (obj_idx == 0x8000U) {            // Status interrupt
      stat = LPC_C_CAN1->STAT;
      if (CAN_SignalUnitEvent[1] != NULL) { 
        if ((stat ^ can_stat_last[1]) & (STAT_BOFF_Msk | STAT_EPASS_Msk | STAT_EWARN_Msk)) {
          if      (stat & (stat ^ can_stat_last[1]) & STAT_BOFF_Msk )        { CAN_SignalUnitEvent[1](ARM_CAN_EVENT_UNIT_BUS_OFF); }
          else if (stat & (stat ^ can_stat_last[1]) & STAT_EPASS_Msk)        { CAN_SignalUnitEvent[1](ARM_CAN_EVENT_UNIT_PASSIVE); }
          else if (stat & (stat ^ can_stat_last[1]) & STAT_EWARN_Msk)        { CAN_SignalUnitEvent[1](ARM_CAN_EVENT_UNIT_WARNING); }
          else if (stat & (STAT_BOFF_Msk | STAT_EPASS_Msk | STAT_EWARN_Msk)) { CAN_SignalUnitEvent[1](ARM_CAN_EVENT_UNIT_ACTIVE);  }
          can_stat_last[1] = stat;
        }
}
    }
  }
}
#endif


#if (RTE_CAN_CAN0 == 1U)
ARM_DRIVER_CAN Driver_CAN0 = {
  CAN_GetVersion,
  CAN0_GetCapabilities,
  CAN0_Initialize,
  CAN0_Uninitialize,
  CAN0_PowerControl,
  CAN_GetClock,
  CAN0_SetBitrate,
  CAN0_SetMode,
  CAN0_ObjectGetCapabilities,
  CAN0_ObjectSetFilter,
  CAN0_ObjectConfigure,
  CAN0_MessageSend,
  CAN0_MessageRead,
  CAN0_Control,
  CAN0_GetStatus
};
#endif

#if (RTE_CAN_CAN1 == 1U)
ARM_DRIVER_CAN Driver_CAN1 = {
  CAN_GetVersion,
  CAN1_GetCapabilities,
  CAN1_Initialize,
  CAN1_Uninitialize,
  CAN1_PowerControl,
  CAN_GetClock,
  CAN1_SetBitrate,
  CAN1_SetMode,
  CAN1_ObjectGetCapabilities,
  CAN1_ObjectSetFilter,
  CAN1_ObjectConfigure,
  CAN1_MessageSend,
  CAN1_MessageRead,
  CAN1_Control,
  CAN1_GetStatus
};
#endif

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
 * $Date:        02. March 2016
 * $Revision:    V2.4
 *
 * Driver:       Driver_I2C0, Driver_I2C1
 * Configured:   via RTE_Device.h configuration file
 * Project:      I2C Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   I2C Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_I2C# = 0       use I2C0
 *   Connect to hardware via Driver_I2C# = 1       use I2C1
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.4
 *   - Added Bus Clear Control 
 *   - Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.3
 *    - Pending IRQ flag cleared after aborted transfer
 *  Version 2.2
 *    - Updated initialization, uninitialization and power procedures
 *  Version 2.1
 *    - Updated to CMSIS Driver API V2.02
 *    - Added Multi-master support
 *  Version 2.0
 *    - Based on API V2.00
 *  Version 1.1
 *    - Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    - Initial release
 */

#include <string.h>

#include "I2C_LPC18xx.h"
#include "SCU_LPC18xx.h"

#include "Driver_I2C.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_I2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,4) /* driver version */

#if ((defined(RTE_Drivers_I2C0) || \
      defined(RTE_Drivers_I2C1))   \
     && !RTE_I2C0                  \
     && !RTE_I2C1)
#error "I2C not configured in RTE_Device.h!"
#endif

/* I2C core clock (system_LPC18xx.c) */
extern uint32_t GetClockFreq (uint32_t clk_src);


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_I2C_API_VERSION,
  ARM_I2C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES DriverCapabilities = {
  0            /* supports 10-bit addressing */
};


#if (RTE_I2C0)
/* I2C0 Control Information */
static I2C_CTRL I2C0_Ctrl = { 0 };

/* I2C0 Resources */
static I2C_RESOURCES I2C0_Resources = {
  LPC_I2C0,
  I2C0_IRQn,
  &LPC_CGU->BASE_APB1_CLK,
  &LPC_CCU1->CLK_APB1_I2C0_CFG,
  &LPC_CCU1->CLK_APB1_I2C0_STAT,
  RGU_RESET_I2C0,
  &I2C0_Ctrl
};
#endif /* RTE_I2C0 */


#if (RTE_I2C1)
/* I2C1 Control Information */
static I2C_CTRL I2C1_Ctrl = { 0 };

/* I2C1 Resources */
static I2C_RESOURCES I2C1_Resources = {
  LPC_I2C1,
  I2C1_IRQn,
  &LPC_CGU->BASE_APB3_CLK,
  &LPC_CCU1->CLK_APB3_I2C1_CFG,
  &LPC_CCU1->CLK_APB3_I2C1_STAT,
  RGU_RESET_I2C1,
  &I2C1_Ctrl
};
#endif /* RTE_I2C1 */


/**
  \fn          ARM_DRIVER_VERSION I2C_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION I2C_GetVersion (void) {
  return DriverVersion;
}

/**
  \fn          ARM_I2C_CAPABILITIES I2C_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_I2C_CAPABILITIES
*/
static ARM_I2C_CAPABILITIES I2C_GetCapabilities (void) {
  return DriverCapabilities;
}

/**
  \fn          int32_t I2Cx_Initialize (ARM_I2C_SignalEvent_t cb_event,
                                        I2C_RESOURCES         *i2c)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event  Pointer to \ref ARM_I2C_SignalEvent
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c) {

  if (i2c->ctrl->flags & I2C_FLAG_INIT) { return ARM_DRIVER_OK; }

  /* Configure I2C Pins */
  if (i2c->reg == LPC_I2C0) {
    SCU_I2C_PinConfigure (SCU_I2C_PIN_MODE_STANDARD_FAST);
  }
  else if (i2c->reg == LPC_I2C1) { 
    SCU_PinConfigure (RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_PIN, SCU_SFS_EZI | RTE_I2C1_SCL_FUNC);
    SCU_PinConfigure (RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_PIN, SCU_SFS_EZI | RTE_I2C1_SDA_FUNC);
  }

  /* Reset Run-Time information structure */
  memset (i2c->ctrl, 0x00, sizeof (I2C_CTRL));

  i2c->ctrl->cb_event = cb_event;
  i2c->ctrl->flags    = I2C_FLAG_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_Uninitialize (I2C_RESOURCES *i2c)
  \brief       De-initialize I2C Interface.
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_Uninitialize (I2C_RESOURCES *i2c) {

  i2c->ctrl->flags = 0;

  /* Unconfigure SCL and SDA pins */
  if (i2c->reg == LPC_I2C0) {
    SCU_I2C_PinConfigure (0);
  }
  else if (i2c->reg == LPC_I2C1) { 
    SCU_PinConfigure (RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_PIN, 0);
    SCU_PinConfigure (RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_PIN, 0);
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_PowerControl (ARM_POWER_STATE state,
                                          I2C_RESOURCES   *i2c)
  \brief       Control I2C Interface Power.
  \param[in]   state  Power state
  \param[in]   i2c    Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *i2c) {
  uint32_t conset;

  switch (state) {
    case ARM_POWER_OFF:
      /* Disable I2C interrupts */
      NVIC_DisableIRQ (i2c->i2c_ev_irq);

      if (i2c->ctrl->status.busy) {
        /* Master: send STOP to I2C bus           */
        /* Slave:  enter non-addressed Slave mode */
        conset = I2C_CON_STO | i2c->ctrl->con_aa;
        i2c->reg->CONSET = conset; 
        i2c->reg->CONCLR = conset ^ I2C_CON_FLAGS;
      }

      i2c->ctrl->status.busy             = 0U;
      i2c->ctrl->status.mode             = 0U;
      i2c->ctrl->status.direction        = 0U;
      i2c->ctrl->status.general_call     = 0U;
      i2c->ctrl->status.arbitration_lost = 0U;
      i2c->ctrl->status.bus_error        = 0U;

      i2c->ctrl->stalled = 0U;
      i2c->ctrl->snum    = 0U;

      i2c->ctrl->flags  &= ~I2C_FLAG_POWER;

      /* Reset I2C peripheral */
      LPC_RGU->RESET_CTRL1 = i2c->rgu_val;
      while (!(LPC_RGU->RESET_ACTIVE_STATUS1 & i2c->rgu_val));

      /* Disable I2C peripheral clock */
      *i2c->pclk_cfg_reg &= ~CCU_CLK_CFG_RUN;
      break;

    case ARM_POWER_FULL:
      if ((i2c->ctrl->flags & I2C_FLAG_INIT)  == 0U) { return ARM_DRIVER_ERROR; }
      if ((i2c->ctrl->flags & I2C_FLAG_POWER) != 0U) { return ARM_DRIVER_OK; }

      /* Connect base clock */
      *i2c->base_clk_reg = (1    << 11) |     /* Autoblock En               */
                           (0x09 << 24) ;     /* PLL1 is APB  clock source  */

      /* Enable I2C peripheral clock */
      *i2c->pclk_cfg_reg = CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while (!((*i2c->pclk_stat_reg) & CCU_CLK_STAT_RUN));

      /* Reset I2C peripheral */
      LPC_RGU->RESET_CTRL1 = i2c->rgu_val;
      while (!(LPC_RGU->RESET_ACTIVE_STATUS1 & i2c->rgu_val));

      /* Enable I2C Operation */
      i2c->reg->CONCLR = I2C_CON_FLAGS;
      i2c->reg->CONSET = I2C_CON_I2EN;

      i2c->ctrl->stalled = 0;
      i2c->ctrl->con_aa  = 0;

      /* Enable I2C interrupts */
      NVIC_ClearPendingIRQ (i2c->i2c_ev_irq);
      NVIC_EnableIRQ (i2c->i2c_ev_irq);

      i2c->ctrl->flags |= I2C_FLAG_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_MasterTransmit (uint32_t       addr,
                                            const uint8_t *data,
                                            uint32_t       num,
                                            bool           xfer_pending,
                                            I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[in]   data          Pointer to buffer with data to transmit to I2C Slave
  \param[in]   num           Number of data bytes to transmit
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_MasterTransmit (uint32_t       addr,
                                    const uint8_t *data,
                                    uint32_t       num,
                                    bool           xfer_pending,
                                    I2C_RESOURCES *i2c) {

  if (!data || !num || (addr > 0x7F)) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(i2c->ctrl->flags & I2C_FLAG_SETUP)) {
    /* Driver not yet configured */
    return ARM_DRIVER_ERROR;
  }

  if (i2c->ctrl->status.busy || (i2c->ctrl->stalled & I2C_SLAVE)) {
    /* Transfer operation in progress, or Slave stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }

  NVIC_DisableIRQ (i2c->i2c_ev_irq);

  /* Set control variables */
  i2c->ctrl->sla_rw  = addr << 1;
  i2c->ctrl->pending = xfer_pending;
  i2c->ctrl->data    = (uint8_t *)data;
  i2c->ctrl->num     = num;
  i2c->ctrl->cnt     = -1;

  /* Update driver status */
  i2c->ctrl->status.busy             = 1;
  i2c->ctrl->status.mode             = 1;
  i2c->ctrl->status.direction        = 0;
  i2c->ctrl->status.arbitration_lost = 0;
  i2c->ctrl->status.bus_error        = 0;
  if (!i2c->ctrl->stalled) {
    i2c->reg->CONSET = I2C_CON_STA | i2c->ctrl->con_aa;
  }

  NVIC_EnableIRQ (i2c->i2c_ev_irq);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_MasterReceive (uint32_t       addr,
                                           uint8_t       *data,
                                           uint32_t       num,
                                           bool           xfer_pending,
                                           I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[out]  data          Pointer to buffer for data to receive from I2C Slave
  \param[in]   num           Number of data bytes to receive
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_MasterReceive (uint32_t       addr,
                                   uint8_t       *data,
                                   uint32_t       num,
                                   bool           xfer_pending,
                                   I2C_RESOURCES *i2c) {

  if (!data || !num || (addr > 0x7F)) {
    /* Invalid parameters */ 
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(i2c->ctrl->flags & I2C_FLAG_SETUP)) {
    /* Driver not yet configured */
    return ARM_DRIVER_ERROR;
  }

  if (i2c->ctrl->status.busy || (i2c->ctrl->stalled & I2C_SLAVE)) {
    /* Transfer operation in progress, or Slave stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }

  NVIC_DisableIRQ (i2c->i2c_ev_irq);

  /* Set control variables */
  i2c->ctrl->sla_rw  = (addr << 1) | 0x01;
  i2c->ctrl->pending = xfer_pending;
  i2c->ctrl->data    = data;
  i2c->ctrl->num     = num;
  i2c->ctrl->cnt     = -1;

  /* Update driver status */
  i2c->ctrl->status.busy             = 1;
  i2c->ctrl->status.mode             = 1;
  i2c->ctrl->status.direction        = 0;
  i2c->ctrl->status.arbitration_lost = 0;
  i2c->ctrl->status.bus_error        = 0;
  if (!i2c->ctrl->stalled) {
    i2c->reg->CONSET = I2C_CON_STA | i2c->ctrl->con_aa;
  }

  NVIC_EnableIRQ (i2c->i2c_ev_irq);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_SlaveTransmit (const uint8_t *data,
                                           uint32_t       num,
                                           I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Slave.
  \param[in]   data  Pointer to buffer with data to transmit to I2C Master
  \param[in]   num   Number of data bytes to transmit
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_SlaveTransmit (const uint8_t *data,
                                   uint32_t       num,
                                   I2C_RESOURCES *i2c) {

  if (!data || !num) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->ctrl->status.busy || (i2c->ctrl->stalled & (I2C_MASTER | I2C_SLAVE_RX))) {
    /* Transfer operation in progress, Master stalled or Slave receive stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }

  NVIC_DisableIRQ (i2c->i2c_ev_irq);

  /* Set control variables */
  i2c->ctrl->flags &= ~I2C_FLAG_SLAVE_RX;
  i2c->ctrl->sdata  = (uint8_t *)data;
  i2c->ctrl->snum   = num;
  i2c->ctrl->cnt    = -1;

  /* Update driver status */
  i2c->ctrl->status.general_call = 0;
  i2c->ctrl->status.bus_error    = 0;

  NVIC_EnableIRQ (i2c->i2c_ev_irq);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_SlaveReceive (uint8_t       *data,
                                          uint32_t       num,
                                          I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Slave.
  \param[out]  data  Pointer to buffer for data to receive from I2C Master
  \param[in]   num   Number of data bytes to receive
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_SlaveReceive (uint8_t       *data,
                                  uint32_t       num,
                                  I2C_RESOURCES *i2c) {

  if (!data || !num) {
    /* Invalid parameters */ 
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->ctrl->status.busy || (i2c->ctrl->stalled & (I2C_MASTER | I2C_SLAVE_TX))) {
    /* Transfer operation in progress, Master stalled or Slave transmit stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }

  NVIC_DisableIRQ (i2c->i2c_ev_irq);

  /* Set control variables */
  i2c->ctrl->flags |= I2C_FLAG_SLAVE_RX;
  i2c->ctrl->sdata  = data;
  i2c->ctrl->snum   = num;
  i2c->ctrl->cnt    = -1;

  /* Update driver status */
  i2c->ctrl->status.general_call = 0;
  i2c->ctrl->status.bus_error    = 0;

  NVIC_EnableIRQ (i2c->i2c_ev_irq);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2Cx_GetDataCount (I2C_RESOURCES *i2c)
  \brief       Get transferred data count.
  \return      number of data bytes transferred; -1 when Slave is not addressed by Master
*/
static int32_t I2Cx_GetDataCount (I2C_RESOURCES *i2c) {
  return (i2c->ctrl->cnt);
}

/**
  \fn          int32_t I2Cx_Control (uint32_t       control,
                                     uint32_t       arg,
                                     I2C_RESOURCES *i2c)
  \brief       Control I2C Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   i2c      pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2Cx_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c) {
  uint32_t val,clk,conset;

  if (!(i2c->ctrl->flags & I2C_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }
  switch (control) {
    case ARM_I2C_OWN_ADDRESS:
      /* Set Own Slave Address */
      val = (arg << 1) & 0xFF;
      if (arg & ARM_I2C_ADDRESS_GC) {
        /* General call enable */
        val |= 0x01;
      }
      i2c->reg->ADR0 = val;

      /* Enable assert acknowledge */
      if (val) val = I2C_CON_AA;
      i2c->ctrl->con_aa = val;
      i2c->reg->CONSET  = val;
      break;

    case ARM_I2C_BUS_SPEED:
      /* Set Bus Speed */
      clk = GetClockFreq (CLK_SRC_PLL1);
      switch (arg) {
        case ARM_I2C_BUS_SPEED_STANDARD:
          /* Standard Speed (100kHz) */
          clk /= 100000;
          break;
        case ARM_I2C_BUS_SPEED_FAST:
          /* Fast Speed     (400kHz) */
          clk /= 400000;
          break;
        case ARM_I2C_BUS_SPEED_FAST_PLUS:
          /* Fast+ Speed    (  1MHz) */
          if (i2c->reg == LPC_I2C0) {
            clk /= 1000000;
            break;
          }
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      /* Improve accuracy */
      i2c->reg->SCLH = clk / 2;
      i2c->reg->SCLL = clk - i2c->reg->SCLH;

      /* Speed configured, I2C Master active */
      i2c->ctrl->flags |= I2C_FLAG_SETUP;
      break;

    case ARM_I2C_BUS_CLEAR:
      /* Execute Bus clear */
      NVIC_DisableIRQ (i2c->i2c_ev_irq);

      i2c->reg->CONSET = I2C_CON_STA;
      __NOP(); __NOP(); __NOP();
      if ((i2c->reg->CONSET & I2C_CON_SI) == 0) {
        for (val = 0; val < 2048; val++);
      }
      /* Clear start and interrupt flag */
      i2c->reg->CONCLR = I2C_CON_STA | I2C_CON_SI;
      /* Send STOP to end the transaction */
      i2c->reg->CONSET = I2C_CON_STO;

      NVIC_ClearPendingIRQ (i2c->i2c_ev_irq);
      NVIC_EnableIRQ (i2c->i2c_ev_irq);
      return ARM_DRIVER_OK;

    case ARM_I2C_ABORT_TRANSFER:
      /* Abort Master/Slave transfer */
      NVIC_DisableIRQ (i2c->i2c_ev_irq);

      i2c->ctrl->status.busy = 0;
      i2c->ctrl->stalled = 0;
      i2c->ctrl->snum    = 0;
      /* Master: send STOP to I2C bus           */
      /* Slave:  enter non-addressed Slave mode */
      conset = I2C_CON_STO | i2c->ctrl->con_aa;
      i2c->reg->CONSET = conset; 
      i2c->reg->CONCLR = conset ^ I2C_CON_FLAGS;

      NVIC_ClearPendingIRQ (i2c->i2c_ev_irq);
      NVIC_EnableIRQ (i2c->i2c_ev_irq);
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_I2C_STATUS I2Cx_GetStatus (I2C_RESOURCES *i2c)
  \brief       Get I2C status.
  \param[in]   i2c      pointer to I2C resources
  \return      I2C status \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2Cx_GetStatus (I2C_RESOURCES *i2c) {
  return (i2c->ctrl->status);
}

/**
  \fn          void I2Cx_MasterHandler (I2C_RESOURCES *i2c)
  \brief       I2C Master state event handler.
  \param[in]   i2c  Pointer to I2C resources
  \return      I2C event notification flags
*/
static uint32_t I2Cx_MasterHandler (I2C_RESOURCES *i2c) {
  uint32_t conset = i2c->ctrl->con_aa;
  uint32_t event  = 0;

  if (i2c->ctrl->stalled) {
    /* Master resumes with repeated START here */
    /* Stalled states: I2C_STAT_MA_DT_A        */
    /*                 I2C_STAT_MA_DR_NA       */
    i2c->ctrl->stalled = 0;
    conset |= I2C_CON_STA;
    goto write_con;
  }

  switch (i2c->reg->STAT & 0xF8) {
    case I2C_STAT_BUSERR:
      /* I2C Bus error */
      i2c->ctrl->status.bus_error = 1;
      i2c->ctrl->status.busy      = 0;
      i2c->ctrl->status.mode      = 0;
      event = ARM_I2C_EVENT_BUS_ERROR      |
              ARM_I2C_EVENT_TRANSFER_DONE  |
              ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      conset |= I2C_CON_STO;
      break;

    case I2C_STAT_MA_START:
      /* START transmitted */
    case I2C_STAT_MA_RSTART:
      /* Repeated START transmitted */
      i2c->reg->DAT = i2c->ctrl->sla_rw;
      break;

    case I2C_STAT_MA_SLAW_NA:
      /* SLA+W transmitted, no ACK received */
    case I2C_STAT_MA_SLAR_NA:
      /* SLA+R transmitted, no ACK received */
      i2c->ctrl->status.busy = 0;
      i2c->ctrl->status.mode = 0;
      event = ARM_I2C_EVENT_ADDRESS_NACK   |
              ARM_I2C_EVENT_TRANSFER_DONE  |
              ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      conset |= I2C_CON_STO;
      break;

    case I2C_STAT_MA_SLAW_A:
      /* SLA+W transmitted, ACK received */
      i2c->ctrl->cnt = 0;
      i2c->reg->DAT  = i2c->ctrl->data[0];
      break;

    case I2C_STAT_MA_DT_A:
      /* Data transmitted, ACK received */
      i2c->ctrl->cnt++;
      i2c->ctrl->num--;
      if (!i2c->ctrl->num) {
        goto xfer_done;
      }
      /* Send next byte */
      i2c->reg->DAT = i2c->ctrl->data[i2c->ctrl->cnt];
      break;

    case I2C_STAT_MA_DT_NA:
      /* Data transmitted, no ACK received */
      i2c->ctrl->status.busy = 0;
      i2c->ctrl->status.mode = 0;
      event = ARM_I2C_EVENT_TRANSFER_DONE  |
              ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      conset |= I2C_CON_STO;
      break;

    case I2C_STAT_MA_ALOST:
      /* Arbitration lost */
      i2c->ctrl->status.arbitration_lost = 1;
      i2c->ctrl->status.busy             = 0;
      i2c->ctrl->status.mode             = 0;
      event = ARM_I2C_EVENT_ARBITRATION_LOST |
              ARM_I2C_EVENT_TRANSFER_DONE    |
              ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      break;

    case I2C_STAT_MA_SLAR_A:
      /* SLA+R transmitted, ACK received */
      i2c->ctrl->cnt = 0;
      i2c->ctrl->status.direction = 1;
      goto upd_conset;

   case I2C_STAT_MA_DR_A:
      /* Data received, ACK returned */
      i2c->ctrl->data[i2c->ctrl->cnt++] = i2c->reg->DAT;
      i2c->ctrl->num--;
upd_conset:
      conset = 0;
      if (i2c->ctrl->num > 1) {
        conset = I2C_CON_AA;
      }
      break;

    case I2C_STAT_MA_DR_NA:
      /* Data received, no ACK returned */
      i2c->ctrl->data[i2c->ctrl->cnt++] = i2c->reg->DAT;
      i2c->ctrl->num--;
xfer_done:
      i2c->ctrl->status.busy = 0;
      event = ARM_I2C_EVENT_TRANSFER_DONE;
      if (i2c->ctrl->pending) {
        /* Stall I2C transaction */
        NVIC_DisableIRQ (i2c->i2c_ev_irq);
        i2c->ctrl->stalled = I2C_MASTER;
        return (event);
      }
      /* Generate STOP */
      conset |= I2C_CON_STO;
      break;
  }
write_con:
  /* Set/clear control flags */
  i2c->reg->CONSET = conset;
  i2c->reg->CONCLR = conset ^ I2C_CON_FLAGS;
  return (event);
}

/**
  \fn          void I2Cx_SlaveHandler (I2C_RESOURCES *i2c)
  \brief       I2C Slave state event handler.
  \param[in]   i2c  Pointer to I2C resources
  \return      I2C event notification flags
*/
static uint32_t I2Cx_SlaveHandler (I2C_RESOURCES *i2c) {
  uint32_t conset = 0;
  uint32_t event  = 0;

  switch (i2c->reg->STAT & 0xF8) {
    case I2C_STAT_SL_ALOST_GC:
      /* Arbitration lost in General call */
      i2c->ctrl->status.arbitration_lost = 1;
    case I2C_STAT_SL_GCA_A:
      /* General address recvd, ACK returned */
      i2c->ctrl->status.general_call     = 1;
      goto slaw_a;

    case I2C_STAT_SL_ALOST_MW:
      /* Arbitration lost SLA+W */
      i2c->ctrl->status.arbitration_lost = 1;
    case I2C_STAT_SL_SLAW_A:
      /* SLA+W received, ACK returned */
slaw_a:
      /* Stalled Slave receiver also resumes here */
      if (!i2c->ctrl->snum || !(i2c->ctrl->flags & I2C_FLAG_SLAVE_RX)) {
        /* Receive buffer unavailable */
        if (i2c->ctrl->stalled) {
          /* Already stalled, abort transaction to prevent dead-loops */
          event = ARM_I2C_EVENT_TRANSFER_DONE |
                  ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
          conset = I2C_CON_STO | i2c->ctrl->con_aa;
          break;
        }
        /* Stall I2C transaction */
        NVIC_DisableIRQ (i2c->i2c_ev_irq);
        i2c->ctrl->stalled = I2C_SLAVE_RX;
        return (ARM_I2C_EVENT_SLAVE_RECEIVE);
      }
      i2c->ctrl->status.direction = 1;
      i2c->ctrl->status.busy      = 1;
      i2c->ctrl->cnt     = 0;
      i2c->ctrl->stalled = 0;
      conset = I2C_CON_AA;
      break;

    case I2C_STAT_SL_DRGC_A:
      /* Data recvd General call, ACK returned */
    case I2C_STAT_SL_DR_A:
      /* Data received, ACK returned */
      i2c->ctrl->sdata[i2c->ctrl->cnt++] = i2c->reg->DAT;
      i2c->ctrl->snum--;
      if (i2c->ctrl->snum) {
        conset = I2C_CON_AA;
      }
      break;

    case I2C_STAT_SL_ALOST_MR:
      /* Arbitration lost SLA+R */
      i2c->ctrl->status.arbitration_lost = 1;
    case I2C_STAT_SL_SLAR_A:
      /* SLA+R received, ACK returned */
      /* Stalled Slave transmitter also resumes here */
      if (!i2c->ctrl->snum || (i2c->ctrl->flags & I2C_FLAG_SLAVE_RX)) {
        /* Transmit buffer unavailable */
        if (i2c->ctrl->stalled) {
          /* Already stalled, abort transaction to prevent dead-loops */
          event = ARM_I2C_EVENT_TRANSFER_DONE |
                  ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
          conset = I2C_CON_STO | i2c->ctrl->con_aa;
          break;
        }
        NVIC_DisableIRQ (i2c->i2c_ev_irq);
        i2c->ctrl->stalled = I2C_SLAVE_TX;
        return (ARM_I2C_EVENT_SLAVE_TRANSMIT);
      }
      i2c->ctrl->status.direction = 0;
      i2c->ctrl->status.busy      = 1;
      i2c->ctrl->cnt     = 0;
      i2c->ctrl->stalled = 0;
    case I2C_STAT_SL_DT_A:
      /* Data transmitted, ACK received */
      i2c->reg->DAT = i2c->ctrl->sdata[i2c->ctrl->cnt++];
      i2c->ctrl->snum--;
      if (i2c->ctrl->snum) {
        conset = I2C_CON_AA;
      }
      break;

    case I2C_STAT_SL_DT_NA:
      /* Data transmitted, no ACK received */
    case I2C_STAT_SL_LDT_A:
      /* Last data transmitted, ACK received */
    case I2C_STAT_SL_DR_NA:
      /* Data received, no ACK returned */
    case I2C_STAT_SL_DRGC_NA:
      /* Data recvd General call, no ACK returned */
    case I2C_STAT_SL_STOP:
      /* STOP received while addressed */
      i2c->ctrl->status.busy = 0;
      /* Slave operation completed, generate events */
      event = ARM_I2C_EVENT_TRANSFER_DONE;
      if (i2c->ctrl->status.arbitration_lost) {
        event |= ARM_I2C_EVENT_ARBITRATION_LOST;
      }
      if (i2c->ctrl->status.general_call) {
        event |= ARM_I2C_EVENT_GENERAL_CALL;
      }
      if (i2c->ctrl->snum) {
        event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      }
      conset = i2c->ctrl->con_aa;
      break;
  }
  /* Set/clear control flags */
  i2c->reg->CONSET = conset;
  i2c->reg->CONCLR = conset ^ I2C_CON_FLAGS;

  return (event);
}

/**
  \fn          void I2Cx_IRQHandler (I2C_RESOURCES *i2c)
  \brief       I2C Event Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2Cx_IRQHandler (I2C_RESOURCES *i2c) {
  uint32_t event;

  if (i2c->reg->STAT < I2C_STAT_SL_SLAW_A) {
    event = I2Cx_MasterHandler (i2c);
  }
  else {
    event = I2Cx_SlaveHandler (i2c);
  }
  /* Callback event notification */
  if (event && i2c->ctrl->cb_event) {
    i2c->ctrl->cb_event (event);
  }
}

#if (RTE_I2C0)
/* I2C0 Driver wrapper functions */
static int32_t I2C0_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return (I2Cx_Initialize (cb_event, &I2C0_Resources));
}
static int32_t I2C0_Uninitialize (void) {
  return (I2Cx_Uninitialize (&I2C0_Resources));
}
static int32_t I2C0_PowerControl (ARM_POWER_STATE state) {
  return (I2Cx_PowerControl (state, &I2C0_Resources));
}
static int32_t I2C0_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return (I2Cx_MasterTransmit (addr, data, num, xfer_pending, &I2C0_Resources));
}
static int32_t I2C0_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return (I2Cx_MasterReceive (addr, data, num, xfer_pending, &I2C0_Resources));
}
static int32_t I2C0_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return (I2Cx_SlaveTransmit (data, num, &I2C0_Resources));
}
static int32_t I2C0_SlaveReceive (uint8_t *data, uint32_t num) {
  return (I2Cx_SlaveReceive (data, num, &I2C0_Resources));
}
static int32_t I2C0_GetDataCount (void) {
  return (I2Cx_GetDataCount (&I2C0_Resources));
}
static int32_t I2C0_Control (uint32_t control, uint32_t arg) {
  return (I2Cx_Control (control, arg, &I2C0_Resources));
}
static ARM_I2C_STATUS I2C0_GetStatus (void) {
  return (I2Cx_GetStatus (&I2C0_Resources));
}
void I2C0_IRQHandler (void) {
  I2Cx_IRQHandler (&I2C0_Resources);
}

/* I2C0 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C0 = {
  I2C_GetVersion,
  I2C_GetCapabilities,
  I2C0_Initialize,
  I2C0_Uninitialize,
  I2C0_PowerControl,
  I2C0_MasterTransmit,
  I2C0_MasterReceive,
  I2C0_SlaveTransmit,
  I2C0_SlaveReceive,
  I2C0_GetDataCount,
  I2C0_Control,
  I2C0_GetStatus
};
#endif

#if (RTE_I2C1)
/* I2C1 Driver wrapper functions */
static int32_t I2C1_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return (I2Cx_Initialize (cb_event, &I2C1_Resources));
}
static int32_t I2C1_Uninitialize (void) {
  return (I2Cx_Uninitialize (&I2C1_Resources));
}
static int32_t I2C1_PowerControl (ARM_POWER_STATE state) {
  return (I2Cx_PowerControl (state, &I2C1_Resources));
}
static int32_t I2C1_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return (I2Cx_MasterTransmit (addr, data, num, xfer_pending, &I2C1_Resources));
}
static int32_t I2C1_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return (I2Cx_MasterReceive (addr, data, num, xfer_pending, &I2C1_Resources));
}
static int32_t I2C1_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return (I2Cx_SlaveTransmit (data, num, &I2C1_Resources));
}
static int32_t I2C1_SlaveReceive (uint8_t *data, uint32_t num) {
  return (I2Cx_SlaveReceive (data, num, &I2C1_Resources));
}
static int32_t I2C1_GetDataCount (void) {
  return (I2Cx_GetDataCount (&I2C1_Resources));
}
static int32_t I2C1_Control (uint32_t control, uint32_t arg) {
  return (I2Cx_Control (control, arg, &I2C1_Resources));
}
static ARM_I2C_STATUS I2C1_GetStatus (void) {
  return (I2Cx_GetStatus (&I2C1_Resources));
}
void I2C1_IRQHandler (void) {
  I2Cx_IRQHandler (&I2C1_Resources);
}

/* I2C1 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C1 = {
  I2C_GetVersion,
  I2C_GetCapabilities,
  I2C1_Initialize,
  I2C1_Uninitialize,
  I2C1_PowerControl,
  I2C1_MasterTransmit,
  I2C1_MasterReceive,
  I2C1_SlaveTransmit,
  I2C1_SlaveReceive,
  I2C1_GetDataCount,
  I2C1_Control,
  I2C1_GetStatus
};
#endif

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
 * $Revision:    V2.5
 *
 * Driver:       Driver_MCI0
 * Configured:   via RTE_Device.h configuration file
 * Project:      MCI Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value
 *   ---------------------                 -----
 *   Connect to hardware via Driver_MCI# = 0
 * -------------------------------------------------------------------------- */

/* Note:
    For designs that need to support legacy MMC cards in open-drain mode,
    an external pull-up controlled with a general purpose output and FET
    will be needed for the CMD line.
 */

/* History:
 *  Version 2.5
 *    - Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.4
 *    - Updated initialization, uninitialization and power procedures
 *    - IRQ processing optimized
 *    - Data timeout handling corrected
 *    - Include file definitions moved to header file
 *  Version 2.3
 *    - FIFO Threshold Watermark Register values corrected
 *  Version 2.2
 *    - High speed enabled under capabilities
 *    - Block size handling added to SetupTransfer function
 *  Version 2.1
 *    - DMA descriptor handling corrected
 *    - Minor functional corrections
 *    - Card Detect event handling added
 *    - SD_RST pin handling added
 *  Version 2.0
 *    - Updated to CMSIS Driver API V2.01
 *  Version 1.1
 *    - Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    - Initial release
 */

#include "MCI_LPC18xx.h"

#define ARM_MCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,5)  /* driver version */

/* External function (system_LPC18xx.c) used to get SDMMC peripheral clock */
extern uint32_t GetClockFreq (uint32_t clk_src);

static SDMMC_DMA_DESC SDMMC_DMA_Descriptor[SDMMC_DMA_DESC_CNT];
static MCI_INFO       MCI;


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_MCI_API_VERSION,
  ARM_MCI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_MCI_CAPABILITIES DriverCapabilities = {
  RTE_SD_CD_PIN_EN,                               /* cd_state          */
  RTE_SD_CD_PIN_EN,                               /* cd_event          */
  RTE_SD_WP_PIN_EN,                               /* wp_state          */
  RTE_SD_POW_PIN_EN,                              /* vdd               */
  0,                                              /* vdd_1v8           */
  0,                                              /* vccq              */
  0,                                              /* vccq_1v8          */
  0,                                              /* vccq_1v2          */
  RTE_SDMMC_BUS_WIDTH_4,                          /* data_width_4      */
  RTE_SDMMC_BUS_WIDTH_4 && RTE_SDMMC_BUS_WIDTH_8, /* data_width_8      */
  0,                                              /* data_width_4_ddr  */
  0,                                              /* data_width_8_ddr  */
  1,                                              /* high_speed        */
  0,                                              /* uhs_signaling     */
  0,                                              /* uhs_tuning        */
  0,                                              /* uhs_sdr50         */
  0,                                              /* uhs_sdr104        */
  0,                                              /* uhs_ddr50         */
  0,                                              /* uhs_driver_type_a */
  0,                                              /* uhs_driver_type_c */
  0,                                              /* uhs_driver_type_d */
  1,                                              /* sdio_interrupt    */
  1,                                              /* read_wait         */
  0,                                              /* suspend_resume    */
  0,                                              /* mmc_interrupt     */
  0,                                              /* mmc_boot          */
  RTE_SD_RST_PIN_EN,                              /* rst_n             */
  0,                                              /* ccs               */
  0                                               /* ccs_timeout       */
};

/**
  \fn            void SetupDMADescriptor (MCI_XFER *xfer)
  \brief         Setup Internal DMA descriptors for data transfer
*/
static void SetupDMADescriptor (MCI_XFER *xfer, bool first) {
  uint32_t i, n;

  for (i = 0; (i < SDMMC_DMA_DESC_CNT) && (xfer->cnt); i++) {
    n = (xfer->cnt > 7680) ? (7680) : (xfer->cnt);

    SDMMC_DMA_Descriptor[i].CtrlStat = SDMMC_DMA_DESC_OWN;
    SDMMC_DMA_Descriptor[i].BufSize  = n;
    SDMMC_DMA_Descriptor[i].BufAddr1 = (uint32_t)xfer->buf;
    SDMMC_DMA_Descriptor[i].BufAddr2 = (uint32_t)xfer->buf + n;

    xfer->buf += n;
    xfer->cnt -= n;

    n = (xfer->cnt > 7680) ? (7680) : (xfer->cnt);
    if (n) {
      SDMMC_DMA_Descriptor[i].BufSize |= n << 13;

      xfer->buf += n;
      xfer->cnt -= n;
    }
  }
  if (xfer->cnt == 0) {
    SDMMC_DMA_Descriptor[i-1].CtrlStat |= SDMMC_DMA_DESC_LD;
  }
  if (first) {
    SDMMC_DMA_Descriptor[0].CtrlStat |= SDMMC_DMA_DESC_FS;
  }
  SDMMC_DMA_Descriptor[SDMMC_DMA_DESC_CNT-1].CtrlStat |= SDMMC_DMA_DESC_ER;
}


/**
  \fn            ARM_DRIVER_VERSION GetVersion (void)
  \brief         Get driver version.
  \return        \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn            ARM_MCI_CAPABILITIES GetCapabilities (void)
  \brief         Get driver capabilities.
  \return        \ref ARM_MCI_CAPABILITIES
*/
static ARM_MCI_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn            int32_t Initialize (ARM_MCI_SignalEvent_t cb_event)
  \brief         Initialize the Memory Card Interface
  \param[in]     cb_event  Pointer to \ref ARM_MCI_SignalEvent
  \return        \ref execution_status
*/
static int32_t Initialize (ARM_MCI_SignalEvent_t cb_event) {

  if (MCI.flags & MCI_INIT)  { return ARM_DRIVER_OK; }

  /* Enable GPIO register interface clock */
  LPC_CCU1->CLK_M3_GPIO_CFG |= CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
  while (!(LPC_CCU1->CLK_M3_GPIO_STAT & CCU_CLK_STAT_RUN));

  /* Configure SD_CLK, SD_CMD and SD_DAT0 */
  if (RTE_SD_CLK_PORT == 0x10) {
    SCU_CLK_PinConfigure (RTE_SD_CLK_PIN, RTE_SD_CLK_FUNC              |
                                          SCU_PIN_CFG_PULLUP_DIS       |
                                          SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                          SCU_PIN_CFG_INPUT_FILTER_DIS);
  }
  else {
    SCU_PinConfigure(RTE_SD_CLK_PORT, RTE_SD_CLK_PIN, RTE_SD_CLK_FUNC              |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  }
                                                    
  SCU_PinConfigure(RTE_SD_CMD_PORT, RTE_SD_CMD_PIN, RTE_SD_CMD_FUNC              |
                                                    SCU_PIN_CFG_PULLUP_DIS       |
                                                    SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                    SCU_PIN_CFG_INPUT_FILTER_DIS);
  SCU_PinConfigure(RTE_SD_DAT0_PORT, RTE_SD_DAT0_PIN, RTE_SD_DAT0_FUNC             |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  #if (RTE_SDMMC_BUS_WIDTH_4)
  /* SD_DAT[3..1] */
  SCU_PinConfigure(RTE_SD_DAT3_PORT, RTE_SD_DAT3_PIN, RTE_SD_DAT3_FUNC             |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  SCU_PinConfigure(RTE_SD_DAT2_PORT, RTE_SD_DAT2_PIN, RTE_SD_DAT2_FUNC             |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  SCU_PinConfigure(RTE_SD_DAT1_PORT, RTE_SD_DAT1_PIN, RTE_SD_DAT1_FUNC             |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);

    #if (RTE_SDMMC_BUS_WIDTH_8)
    /* SD_DAT[7..4] */
    SCU_PinConfigure(RTE_SD_DAT4_PORT, RTE_SD_DAT4_PIN, RTE_SD_DAT4_FUNC             |
                                                        SCU_PIN_CFG_PULLUP_DIS       |
                                                        SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                        SCU_PIN_CFG_INPUT_FILTER_DIS);
    SCU_PinConfigure(RTE_SD_DAT5_PORT, RTE_SD_DAT5_PIN, RTE_SD_DAT5_FUNC             |
                                                        SCU_PIN_CFG_PULLUP_DIS       |
                                                        SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                        SCU_PIN_CFG_INPUT_FILTER_DIS);
    SCU_PinConfigure(RTE_SD_DAT6_PORT, RTE_SD_DAT6_PIN, RTE_SD_DAT6_FUNC             |
                                                        SCU_PIN_CFG_PULLUP_DIS       |
                                                        SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                        SCU_PIN_CFG_INPUT_FILTER_DIS);
    SCU_PinConfigure(RTE_SD_DAT7_PORT, RTE_SD_DAT7_PIN, RTE_SD_DAT7_FUNC             |
                                                        SCU_PIN_CFG_PULLUP_DIS       |
                                                        SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                        SCU_PIN_CFG_INPUT_FILTER_DIS);
    #endif /* RTE_SDMMC_BUS_WIDTH_8 */

  #endif /* RTE_SDMMC_BUS_WIDTH_4 */

  #if (RTE_SD_CD_PIN_EN)
  /* Configure SD_CD (Card Detect) Pin */
  SCU_PinConfigure(RTE_SD_CD_PORT, RTE_SD_CD_PIN, RTE_SD_CD_FUNC               |
                                                  SCU_PIN_CFG_PULLUP_DIS       |
                                                  SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                  SCU_PIN_CFG_INPUT_FILTER_DIS);
  #endif

  #if (RTE_SDIO_WP_PIN_EN)
  /* Configure SD_WP (Write Protect) Pin */
  SCU_PinConfigure(RTE_SD_WP_PORT, RTE_SD_WP_PIN, RTE_SD_WP_FUNC               |
                                                  SCU_PIN_CFG_PULLUP_DIS       |
                                                  SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                  SCU_PIN_CFG_INPUT_FILTER_DIS);
  #endif

  /* Configure SD_POW Pin */
  #if (RTE_SD_POW_PIN_EN)
    SCU_PinConfigure(RTE_SD_POW_PORT, RTE_SD_POW_PIN, RTE_SD_POW_FUNC              |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  #endif

  /* Configure SD_RST Pin */
  #if (RTE_SD_RST_PIN_EN)
    SCU_PinConfigure(RTE_SD_RST_PORT, RTE_SD_RST_PIN, RTE_SD_RST_FUNC              |
                                                      SCU_PIN_CFG_PULLUP_DIS       |
                                                      SCU_PIN_CFG_INPUT_BUFFER_EN  |
                                                      SCU_PIN_CFG_INPUT_FILTER_DIS);
  #endif

  /* Connect SDIO base clock to PLL1 */
  LPC_CGU->BASE_SDIO_CLK  = (0x01 << 11) | (SDIO_CLK_SEL_PLL1 << 24);

  /* Clear control structure */
  memset (&MCI, 0, sizeof (MCI_INFO));

  MCI.cb_event = cb_event;
  MCI.flags    = MCI_INIT;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t Uninitialize (void)
  \brief         De-initialize Memory Card Interface.
  \return        \ref execution_status
*/
static int32_t Uninitialize (void) {

  /* Change SDIO base clock from PLL1 to IRC */
  LPC_CGU->BASE_SDIO_CLK  = (0x01 << 11) | (0x01 << 24);

  /* Unconfigure SD_CLK and SD_CMD and SD_DAT0 */
  if (RTE_SD_CLK_PORT == 0x10) {
    SCU_CLK_PinConfigure (RTE_SD_CLK_PIN, 0);
  }
  else {
    SCU_PinConfigure(RTE_SD_CLK_PORT,  RTE_SD_CLK_PIN,  0);
  }
  SCU_PinConfigure(RTE_SD_CMD_PORT,  RTE_SD_CMD_PIN,  0);
  SCU_PinConfigure(RTE_SD_DAT0_PORT, RTE_SD_DAT0_PIN, 0);
  
  #if (RTE_SDMMC_BUS_WIDTH_4)
  /* SD_DAT[3..1] */
  SCU_PinConfigure(RTE_SD_DAT1_PORT, RTE_SD_DAT1_PIN, 0);
  SCU_PinConfigure(RTE_SD_DAT2_PORT, RTE_SD_DAT2_PIN, 0);
  SCU_PinConfigure(RTE_SD_DAT3_PORT, RTE_SD_DAT3_PIN, 0);
  
    #if (RTE_SDMMC_BUS_WIDTH_8)
    /* SD_DAT[7..4] */
    SCU_PinConfigure(RTE_SD_DAT4_PORT, RTE_SD_DAT4_PIN, 0);
    SCU_PinConfigure(RTE_SD_DAT5_PORT, RTE_SD_DAT5_PIN, 0);
    SCU_PinConfigure(RTE_SD_DAT6_PORT, RTE_SD_DAT6_PIN, 0);
    SCU_PinConfigure(RTE_SD_DAT7_PORT, RTE_SD_DAT7_PIN, 0);
    #endif /* RTE_SDMMC_BUS_WIDTH_8 */
  
  #endif /* RTE_SDMMC_BUS_WIDTH_4 */

  /* Unconfigure SD_CD (Card Detect) Pin */
  #if (RTE_SD_CD_PIN_EN)
    SCU_PinConfigure(RTE_SD_CD_PORT, RTE_SD_CD_PIN, 0);
  #endif

  /* Unconfigure SD_WP (Write Protect) Pin */
  #if (RTE_SD_WP_PIN_EN)
    SCU_PinConfigure(RTE_SD_WP_PORT, RTE_SD_WP_PIN, 0);
  #endif
  
  /* Unconfigure SD_POW Pin */
  #if (RTE_SD_POW_PIN_EN)
    SCU_PinConfigure(RTE_SD_POW_PORT, RTE_SD_POW_PIN, 0);
  #endif

  /* Unconfigure SD_RST Pin */
  #if (RTE_SD_RST_PIN_EN)
    SCU_PinConfigure(RTE_SD_RST_PORT, RTE_SD_RST_PIN, 0);
  #endif

  MCI.flags = 0;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t PowerControl (ARM_POWER_STATE state)
  \brief         Control Memory Card Interface Power.
  \param[in]     state   Power state \ref ARM_POWER_STATE
  \return        \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      /* Disable SDIO interrupts */
      NVIC_DisableIRQ(SDIO_IRQn);

      MCI.flags &= ~MCI_POWER;

      /* Clear status */
      MCI.status.command_active   = 0U;
      MCI.status.command_timeout  = 0U;
      MCI.status.command_error    = 0U;
      MCI.status.transfer_active  = 0U;
      MCI.status.transfer_timeout = 0U;
      MCI.status.transfer_error   = 0U;
      MCI.status.sdio_interrupt   = 0U;
      MCI.status.ccs              = 0U;

      /* Reset peripheral */
      LPC_RGU->RESET_CTRL0 = RGU_RESET_CTRL0_SDIO_RST;
      __NOP();

      /* Disable SDIO interface clock */
      LPC_CCU2->CLK_SDIO_CFG    = 0;
      LPC_CCU1->CLK_M3_SDIO_CFG = 0;
      break;

    case ARM_POWER_FULL:
      if ((MCI.flags & MCI_INIT)  == 0U) { return ARM_DRIVER_ERROR; }
      if ((MCI.flags & MCI_POWER) != 0U) { return ARM_DRIVER_OK; }

      /* Clear response and transfer variables */
      MCI.response = NULL;
      MCI.xfer.cnt = NULL;

      /* Enable SDIO clocks */
      LPC_CCU1->CLK_M3_SDIO_CFG |= CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while (!(LPC_CCU1->CLK_M3_SDIO_CFG & CCU_CLK_STAT_RUN));

      LPC_CCU2->CLK_SDIO_CFG |= CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while (!(LPC_CCU2->CLK_SDIO_CFG & CCU_CLK_STAT_RUN));

      /* Reset controller, FIFO and DMA and wait until reset done */
      LPC_SDMMC->CTRL = SDMMC_CTRL_RESET_BITMASK;
      while (LPC_SDMMC->CTRL & SDMMC_CTRL_RESET_BITMASK);

      LPC_SDMMC->BMOD = SDMMC_BMOD_SWR;
      while (LPC_SDMMC->BMOD & SDMMC_BMOD_SWR);

      /* Enable internal DMAC interrupts */
      LPC_SDMMC->IDINTEN = SDMMC_IDINTEN_FBE |
                           SDMMC_IDINTEN_DU  ;

      /* Enable SD/MMC peripheral interrupts */
      LPC_SDMMC->INTMASK = SDMMC_INTMASK_RE    |
                           #if (RTE_SD_CD_PIN_EN)
                           SDMMC_INTMASK_CDET  |
                           #endif
                           SDMMC_INTMASK_CDONE |
                           SDMMC_INTMASK_DTO   |
                           SDMMC_INTMASK_RCRC  |
                           SDMMC_INTMASK_DCRC  |
                           SDMMC_INTMASK_RTO   |
                           SDMMC_INTMASK_DRTO  |
                           SDMMC_INTMASK_SBE   |
                           SDMMC_INTMASK_EBE   ;
      /* Enable Global Interrupt and select internal DMA for data transfer */
      LPC_SDMMC->CTRL = SDMMC_CTRL_INT_ENABLE | SDMMC_CTRL_USE_INTERNAL_DMAC;

      /* Set FIFO Threshold watermark */
      LPC_SDMMC->FIFOTH = SDMMC_FIFOTH_DMA_MTS(0)   |
                          SDMMC_FIFOTH_RX_WMARK(14) |
                          SDMMC_FIFOTH_TX_WMARK(15) ;

      /* Set Bus Mode */
      LPC_SDMMC->BMOD = SDMMC_BMOD_DE;

      /* Set descriptor address */
      LPC_SDMMC->DBADDR = (uint32_t)&SDMMC_DMA_Descriptor;

      /* Enable SDMMC peripheral interrupts in NVIC */
      NVIC_ClearPendingIRQ(SDIO_IRQn);
      NVIC_EnableIRQ(SDIO_IRQn);

      MCI.flags |= MCI_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}



/**
  \fn            int32_t CardPower (uint32_t voltage)
  \brief         Set Memory Card supply voltage.
  \param[in]     voltage  Memory Card supply voltage
  \return        \ref execution_status
*/
static int32_t CardPower (uint32_t voltage) {

  if (!(MCI.flags & MCI_POWER)) { return ARM_DRIVER_ERROR; }

  #if (RTE_SD_POW_PIN_EN)
  /* Power on/off is supported */
  switch (voltage & ARM_MCI_POWER_VDD_Msk) {
    case ARM_MCI_POWER_VDD_OFF:
      LPC_SDMMC->PWREN &= ~SDMMC_PWREN_POWER_ENABLE;
      return ARM_DRIVER_OK;

    case ARM_MCI_POWER_VDD_3V3:
      LPC_SDMMC->PWREN |=  SDMMC_PWREN_POWER_ENABLE;
      return ARM_DRIVER_OK;
    
    default:
      break;
  }
  #endif
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}


/**
  \fn            int32_t ReadCD (void)
  \brief         Read Card Detect (CD) state.
  \return        1:card detected, 0:card not detected, or error
*/
static int32_t ReadCD (void) {

  if (!(MCI.flags & MCI_POWER)) { return ARM_DRIVER_ERROR; }

  #if (RTE_SD_CD_PIN_EN)
  return !(LPC_SDMMC->CDETECT & 1);
  #else
  return (0);
  #endif
}


/**
  \fn            int32_t ReadWP (void)
  \brief         Read Write Protect (WP) state.
  \return        1:write protected, 0:not write protected, or error
*/
static int32_t ReadWP (void) {

  if (!(MCI.flags & MCI_POWER)) { return ARM_DRIVER_ERROR; }

  #if (RTE_SD_WP_PIN_EN)
  return (LPC_SDMMC->WRTPRT & 1);
  #else
  return (0);
  #endif
}


/**
  \fn            int32_t SendCommand (uint32_t  cmd,
                                      uint32_t  arg,
                                      uint32_t  flags,
                                      uint32_t *response)
  \brief         Send Command to card and get the response.
  \param[in]     cmd       Memory Card command
  \param[in]     arg       Command argument
  \param[in]     flags     Command flags
  \param[out]    response  Pointer to buffer for response
  \return        \ref execution_status
*/
static int32_t SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response) {

  if ((flags & MCI_RESPONSE_EXPECTED_Msk) && (response == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(MCI.flags & MCI_SETUP)) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.command_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  MCI.status.command_active = 1;

  /* Set command register value */
  cmd = SDMMC_CMD_CMD_INDEX(cmd) | SDMMC_CMD_WAIT_PRVDATA_COMPLETE | SDMMC_CMD_START_CMD;

  if (flags & ARM_MCI_CARD_INITIALIZE) {
    cmd |= SDMMC_CMD_SEND_INITIALIZATION;
  }

  MCI.response = response;
  MCI.flags   &= ~MCI_RESP_LONG;

  switch (flags & ARM_MCI_RESPONSE_Msk) {
    case ARM_MCI_RESPONSE_NONE:
      /* No response expected */
      MCI.response = NULL;
      break;

    case ARM_MCI_RESPONSE_SHORT:
    case ARM_MCI_RESPONSE_SHORT_BUSY:
      /* Short response expected */
      cmd |= SDMMC_CMD_RESPONSE_EXPECT;
      break;

    case ARM_MCI_RESPONSE_LONG:
      MCI.flags |= MCI_RESP_LONG;
      /* Long response expected */
      cmd |= SDMMC_CMD_RESPONSE_EXPECT | SDMMC_CMD_RESPONSE_LENGTH;
      break;
  }

  if (flags & ARM_MCI_RESPONSE_CRC) {
    cmd |= SDMMC_CMD_CHECK_RESPONSE_CRC;
  }

  if (flags & ARM_MCI_TRANSFER_DATA) {
    cmd |= SDMMC_CMD_DATA_EXPECTED;

    if (MCI.flags & MCI_WRITE)  { cmd |= SDMMC_CMD_READ_WRITE;    }
    if (MCI.flags & MCI_STREAM) { cmd |= SDMMC_CMD_TRANSFER_MODE; }

    MCI.status.transfer_active = 1;
  }

  /* Send the command */
  LPC_SDMMC->CMDARG = arg;
  LPC_SDMMC->CMD    = cmd;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t SetupTransfer (uint8_t *data,
                                        uint32_t block_count,
                                        uint32_t block_size,
                                        uint32_t mode)
  \brief         Setup read or write transfer operation.
  \param[in,out] data         Pointer to data block(s) to be written or read
  \param[in]     block_count  Number of blocks
  \param[in]     block_size   Size of a block in bytes
  \param[in]     mode         Transfer mode
  \return        \ref execution_status
*/
static int32_t SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode) {

  if ((data == NULL) || (block_count == 0) || (block_size == 0)) return ARM_DRIVER_ERROR_PARAMETER;

  if (!(MCI.flags & MCI_SETUP)) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.transfer_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  /* Remember if write or read transfer requested */
  if (mode & ARM_MCI_TRANSFER_WRITE) { MCI.flags |=  MCI_WRITE; }
  else                               { MCI.flags &= ~MCI_WRITE; }

  /* Remember if stream or block transfer mode requested */
  if (mode & ARM_MCI_TRANSFER_STREAM) { MCI.flags |=  MCI_STREAM; }
  else                                { MCI.flags &= ~MCI_STREAM; }

  MCI.xfer.buf = data;
  MCI.xfer.cnt = block_count * block_size;

  LPC_SDMMC->BLKSIZ = block_size;
  LPC_SDMMC->BYTCNT = MCI.xfer.cnt;
  
  SetupDMADescriptor (&MCI.xfer, true);

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t AbortTransfer (void)
  \brief         Abort current read/write data transfer.
  \return        \ref execution_status
*/
static int32_t AbortTransfer (void) {

  if (!(MCI.flags & MCI_SETUP)) { return ARM_DRIVER_ERROR; }

  /* Disable global interrupt */
  LPC_SDMMC->CTRL &= ~SDMMC_CTRL_INT_ENABLE;

  /* Reset Controller, FIFO and internal DMA */
  LPC_SDMMC->CTRL |= SDMMC_CTRL_CONTROLLER_RESET |
                     SDMMC_CTRL_FIFO_RESET       |
                     SDMMC_CTRL_DMA_RESET        ;

  /* Clear DMA Interrupt flags */
  LPC_SDMMC->IDSTS = SDMMC_IDSTS_TI  |
                     SDMMC_IDSTS_RI  |
                     SDMMC_IDSTS_FBE |
                     SDMMC_IDSTS_DU  |
                     SDMMC_IDSTS_CES |
                     SDMMC_IDSTS_NIS |
                     SDMMC_IDSTS_AIS ;
  /* Clear RAW Interrupt flags */
  LPC_SDMMC->RINTSTS = 0xFFFF;

  MCI.status.command_active  = 0;
  MCI.status.transfer_active = 0;
  MCI.status.sdio_interrupt  = 0;
  MCI.status.ccs             = 0;

  /* Enable global interrupt */
  LPC_SDMMC->CTRL |= SDMMC_CTRL_INT_ENABLE;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t Control (uint32_t control, uint32_t arg)
  \brief         Control MCI Interface.
  \param[in]     control  Operation
  \param[in]     arg      Argument of operation (optional)
  \return        \ref execution_status
*/
static int32_t Control (uint32_t control, uint32_t arg) {
  uint32_t div, bps, pclk;
  
  if (!(MCI.flags & MCI_POWER)) { return ARM_DRIVER_ERROR; }

  switch (control) {
    case ARM_MCI_BUS_SPEED:
      /* Get peripheral clock and calculate clock divider */
      pclk = GetClockFreq (SDIO_CLK_SEL_PLL1);
      bps  = arg;

      LPC_SDMMC->CLKENA &= ~SDMMC_CLKENA_CCLK_ENABLE;

      if (bps) {
        /* bps = pclk / (2 * div) */
        div   = (pclk + bps - 1) / bps;
        if (div & 1) { div += 1; }

        bps = pclk / div;

        LPC_SDMMC->CLKSRC  =  SDMMC_CLKSRC_CLK_SOURCE (0);
        LPC_SDMMC->CLKDIV  =  SDMMC_CLKDIV_CLK_DIVIDER0 (div >> 1);
        LPC_SDMMC->CLKENA |=  SDMMC_CLKENA_CCLK_ENABLE   |
                              SDMMC_CLKENA_CCLK_LOW_POWER;
      }

      /* Send "update clock registers" command and wait until finished */
      LPC_SDMMC->CMD = SDMMC_CMD_UPDATE_CLOCK_REGISTERS_ONLY |
                       SDMMC_CMD_WAIT_PRVDATA_COMPLETE       |
                       SDMMC_CMD_START_CMD                   ;

      while (LPC_SDMMC->CMD & SDMMC_CMD_START_CMD);

      /* Bus speed configured */
      MCI.flags |= MCI_SETUP;

      return (bps);

    case ARM_MCI_BUS_SPEED_MODE:
      switch (arg) {
        case ARM_MCI_BUS_DEFAULT_SPEED:
          /* Speed mode up to 25/26MHz */
        case ARM_MCI_BUS_HIGH_SPEED:
          /* Speed mode up to 50MHz */
          return ARM_DRIVER_OK;

        default:
          break;
      }
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_MCI_BUS_CMD_MODE:
      /* Implement external pull-up control to support MMC cards in open-drain mode */
      /* Default mode is push-pull and is configured in Driver_MCI0.Initialize()    */
      if (arg == ARM_MCI_BUS_CMD_PUSH_PULL) {
        /* Configure external circuit to work in push-pull mode */
      }
      else if (arg == ARM_MCI_BUS_CMD_OPEN_DRAIN) {
        /* Configure external circuit to work in open-drain mode */
      }
      else {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_BUS_DATA_WIDTH:
      LPC_SDMMC->CTYPE &= ~(SDMMC_CTYPE_CARD_WIDTH0 | SDMMC_CTYPE_CARD_WIDTH1);

      switch (arg) {
        case ARM_MCI_BUS_DATA_WIDTH_1:
          break;
        case ARM_MCI_BUS_DATA_WIDTH_4:
          LPC_SDMMC->CTYPE |= SDMMC_CTYPE_CARD_WIDTH0;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_8:
          LPC_SDMMC->CTYPE |= SDMMC_CTYPE_CARD_WIDTH1;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    #if (RTE_SD_RST_PIN_EN)
    case ARM_MCI_CONTROL_RESET:
      if (arg) {
        /* Assert RST_n pin */
        LPC_SDMMC->RST_N = 0;
      }
      else {
        /* Deassert RST_n pin */
        LPC_SDMMC->RST_N = 1;
      }
      break;
    #endif

    case ARM_MCI_CONTROL_CLOCK_IDLE:
      if (arg) {
        /* Clock generation enabled when idle */
        LPC_SDMMC->CLKENA &= ~SDMMC_CLKENA_CCLK_LOW_POWER;
      }
      else {
        /* Clock generation disabled when idle */
        LPC_SDMMC->CLKENA |=  SDMMC_CLKENA_CCLK_LOW_POWER;
      }
      /* Send "update clock registers" command and wait until finished */
      LPC_SDMMC->CMD = SDMMC_CMD_UPDATE_CLOCK_REGISTERS_ONLY |
                       SDMMC_CMD_WAIT_PRVDATA_COMPLETE       |
                       SDMMC_CMD_START_CMD                   ;

      while (LPC_SDMMC->CMD & SDMMC_CMD_START_CMD);
      break;

    case ARM_MCI_DATA_TIMEOUT:
      if (arg > 0xFFFFFF) {
        /* Max timeout @ 50MHz is ~335ms: this could cause */
        /* data timeout issues on slow devices             */
        arg = 0xFFFFFF;
      }
      LPC_SDMMC->TMOUT = (arg << 8) | 0x40;
      break;
    
    case ARM_MCI_MONITOR_SDIO_INTERRUPT:
      MCI.status.sdio_interrupt = 0;
      LPC_SDMMC->INTMASK |= SDMMC_INTMASK_SDIO_INT_MASK;
      break;
    
    case ARM_MCI_CONTROL_READ_WAIT:
      if (arg) {
        /* Assert read wait */
        LPC_SDMMC->CTRL |= SDMMC_CTRL_READ_WAIT;
      }
      else {
        /* Clear read wait */
        LPC_SDMMC->CTRL &= ~SDMMC_CTRL_READ_WAIT;
      }
      break;
    
    case ARM_MCI_DRIVER_STRENGTH:
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}


/**
  \fn            ARM_MCI_STATUS GetStatus (void)
  \brief         Get MCI status.
  \return        MCI status \ref ARM_MCI_STATUS
*/
static ARM_MCI_STATUS GetStatus (void) {
  return MCI.status;
}


/**
  \fn          SDIO_IRQHandler (void)
  \brief       Abort current read/write data transfer.
  \return      \ref MCI_STATUS execution status
*/
void SDIO_IRQHandler (void) {
  uint32_t rintsts, idsts;
  uint32_t rintclr, idclr;
  uint32_t event;

  event   = 0;
  rintclr = 0;
  idclr   = 0;
  rintsts = LPC_SDMMC->RINTSTS;
  idsts   = LPC_SDMMC->IDSTS;

  /* Abnormal Interrupt Summary */
  if (idsts & SDMMC_IDSTS_DU) {
    idclr |= SDMMC_IDSTS_DU;
    /* Descriptor Unavailable Interrupt */
    if (MCI.xfer.cnt) {
      SetupDMADescriptor (&MCI.xfer, false);

      LPC_SDMMC->PLDMND = 1;
    }
  }
  if (idsts & SDMMC_IDSTS_FBE) {
    idclr |= SDMMC_IDSTS_FBE;
    /* Fatal Bus Error Interrupt */
    event |= ARM_MCI_EVENT_TRANSFER_ERROR;
  }
  #if (RTE_SD_CD_PIN_EN)
  if (rintsts & SDMMC_RINTSTS_CDET) {
    rintclr |= SDMMC_RINTSTS_CDET;
    /* Card detect */
    if (LPC_SDMMC->CDETECT & 1) {
      event |= ARM_MCI_EVENT_CARD_REMOVED;
    }
    else {
      event |= ARM_MCI_EVENT_CARD_INSERTED;
    }
  }
  #endif
  
  if (rintsts & SDMMC_RINT_ERR_SDIO_Msk) {
    if (rintsts & SDMMC_RINTSTS_RE) {
      rintclr |= SDMMC_RINTSTS_RE;
      /* Response error */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_RCRC) {
      rintclr |= SDMMC_RINTSTS_RCRC;
      /* Response CRC error */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_DCRC) {
      rintclr |= SDMMC_RINTSTS_DCRC;
      /* Data CRC error */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_RTO_BAR) {
      rintclr |= SDMMC_RINTSTS_RTO_BAR;
      /* Response time-out/Boot Ack Received */
      event |= ARM_MCI_EVENT_COMMAND_TIMEOUT;
    }
    if (rintsts & SDMMC_RINTSTS_DRTO_BDS) {
      rintclr |= SDMMC_RINTSTS_DRTO_BDS;
      /* Data read time-out / Boot Data Start              */
      /* Card has not sent data within the time-out period */
      event |= ARM_MCI_EVENT_TRANSFER_TIMEOUT;
    }
    if (rintsts & SDMMC_RINTSTS_HLE) {
      rintclr |= SDMMC_RINTSTS_HLE;
      /* Hardware locked error (command buffer full) */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_SBE) {
      rintclr |= SDMMC_RINTSTS_SBE;
      /* Start-bit error */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_EBE) {
      rintclr |= SDMMC_RINTSTS_EBE;
      /* End-bit error (read)/write no CRC */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    if (rintsts & SDMMC_RINTSTS_SDIO_INTERRUPT) {
      rintclr |= SDMMC_RINTSTS_SDIO_INTERRUPT;
      /* Interrupt from SDIO card */
      event |= ARM_MCI_EVENT_SDIO_INTERRUPT;
      MCI.status.sdio_interrupt = 1;

      /* Disable interrupt (must be re-enabled using Control) */
      LPC_SDMMC->INTMASK &= ~SDMMC_INTMASK_SDIO_INT_MASK;
    }
  }
  if (rintsts & SDMMC_RINTSTS_CDONE) {
    rintclr |= SDMMC_RINTSTS_CDONE;
    /* Command done */
    event |= ARM_MCI_EVENT_COMMAND_COMPLETE;

    if (MCI.response) {
      /* Read response registers */
      MCI.response[0] = LPC_SDMMC->RESP0;

      if (MCI.flags & MCI_RESP_LONG) {
        MCI.response[1] = LPC_SDMMC->RESP1;
        MCI.response[2] = LPC_SDMMC->RESP2;
        MCI.response[3] = LPC_SDMMC->RESP3;
      }
    }
  }
  if (rintsts & SDMMC_RINTSTS_DTO) {
    rintclr |= SDMMC_RINTSTS_DTO;
    /* Data transfer over */
    event |= ARM_MCI_EVENT_TRANSFER_COMPLETE;
  }

  LPC_SDMMC->RINTSTS = rintclr;
  LPC_SDMMC->IDSTS   = idclr;

  if (event & MCI_TRANSFER_EVENT_Msk) {
    MCI.status.transfer_active = 0;

    if (MCI.cb_event) {
      if (event & ARM_MCI_EVENT_TRANSFER_ERROR) {
        (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_ERROR);
      }
      else if (event & ARM_MCI_EVENT_TRANSFER_TIMEOUT) {
        (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_TIMEOUT);
      }
      else {
        (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_COMPLETE);
      }
    }
  }
  
  if (event & MCI_COMMAND_EVENT_Msk) {
    MCI.status.command_active = 0;

    if (MCI.cb_event) {
      if (event & ARM_MCI_EVENT_COMMAND_ERROR) {
        (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_ERROR);
      }
      else if (event & ARM_MCI_EVENT_COMMAND_TIMEOUT) {
        (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_TIMEOUT);
      }
      else {
        (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_COMPLETE);
      }
    }
  }

  if (event & MCI_CONTROL_EVENT_Msk) {
    if (MCI.cb_event) {
      (MCI.cb_event)(event & MCI_CONTROL_EVENT_Msk);
    }
  }
}

/* MCI Driver Control Block */
ARM_DRIVER_MCI Driver_MCI0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  CardPower,
  ReadCD,
  ReadWP,
  SendCommand,
  SetupTransfer,
  AbortTransfer,
  Control,
  GetStatus
};

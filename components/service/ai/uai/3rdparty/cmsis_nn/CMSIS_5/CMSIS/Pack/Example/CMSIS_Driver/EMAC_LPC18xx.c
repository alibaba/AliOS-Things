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
 * $Revision:    V2.6
 *
 * Driver:       Driver_ETH_MAC0
 * Configured:   via RTE_Device.h configuration file
 * Project:      Ethernet Media Access (MAC) Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_MAC# = 0
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.6
 *    - Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.5
 *    - Corrected return value of the ReadFrame function
 *  Version 2.4
 *    - Updated initialization, uninitialization and power procedures
 *  Version 2.3
 *    - Corrected return value of PHY_Read and PHY_Write functions on timeout
 *  Version 2.2
 *    - GetMacAddress function implemented in Ethernet driver
 *  Version 2.1
 *    - Added Sleep mode and Wake-up on Magic Packet 
 *    - Improved robustness and error control
 *    - Added CLK0 pin option support
 *  Version 2.0
 *    - Based on API V2.00
 *    - Added multicast MAC address filtering
 *  Version 1.1
 *    - Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    - Initial release
 */

/* IEEE 1588 time stamping enable (PTP) */
#ifndef EMAC_TIME_STAMP
#define EMAC_TIME_STAMP         0
#endif

#include "EMAC_LPC18xx.h"

#define ARM_ETH_MAC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,6) /* driver version */

/* Timeouts */
#define PHY_TIMEOUT         200         /* PHY Register access timeout in us  */

/* EMAC Memory Buffer configuration */
#define NUM_RX_BUF          4           /* 0x1800 for Rx (4*1536=6K)          */
#define NUM_TX_BUF          2           /* 0x0C00 for Tx (2*1536=3K)          */
#define ETH_BUF_SIZE        1536        /* ETH Receive/Transmit buffer size   */

/* EMAC core clock (system_LPC43xx.c) */
extern uint32_t GetClockFreq (uint32_t clk_src);

/* Ethernet Pin definitions */
static const PIN_ID eth_pins[] = {
  { RTE_ENET_MDI_MDC_PORT,     RTE_ENET_MDI_MDC_PIN,                   SCU_SFS_EZI | RTE_ENET_MDI_MDC_FUNC       },
  { RTE_ENET_MDI_MDIO_PORT,    RTE_ENET_MDI_MDIO_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MDI_MDIO_FUNC      },
#if (RTE_ENET_RMII)
  { RTE_ENET_RMII_TXD0_PORT,   RTE_ENET_RMII_TXD0_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_TXD0_FUNC     },
  { RTE_ENET_RMII_TXD1_PORT,   RTE_ENET_RMII_TXD1_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_TXD1_FUNC     },
  { RTE_ENET_RMII_TX_EN_PORT,  RTE_ENET_RMII_TX_EN_PIN,  SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_TX_EN_FUNC    },
  { RTE_ENET_RMII_REF_CLK_PORT,RTE_ENET_RMII_REF_CLK_PIN,SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_REF_CLK_FUNC  },
  { RTE_ENET_RMII_RXD0_PORT,   RTE_ENET_RMII_RXD0_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_RXD0_FUNC     },
  { RTE_ENET_RMII_RXD1_PORT,   RTE_ENET_RMII_RXD1_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_RXD1_FUNC     },
  { RTE_ENET_RMII_RX_DV_PORT,  RTE_ENET_RMII_RX_DV_PIN,  SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_RMII_RX_DV_FUNC    }
#endif
#if (RTE_ENET_MII)
  { RTE_ENET_MII_TXD0_PORT,    RTE_ENET_MII_TXD0_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TXD0_FUNC      },
  { RTE_ENET_MII_TXD1_PORT,    RTE_ENET_MII_TXD1_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TXD1_FUNC      },
  { RTE_ENET_MII_TXD2_PORT,    RTE_ENET_MII_TXD2_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TXD2_FUNC      },
  { RTE_ENET_MII_TXD3_PORT,    RTE_ENET_MII_TXD3_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TXD3_FUNC      },
  { RTE_ENET_MII_TX_EN_PORT,   RTE_ENET_MII_TX_EN_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TX_EN_FUNC     },
  { RTE_ENET_MII_TX_CLK_PORT,  RTE_ENET_MII_TX_CLK_PIN,  SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TX_CLK_FUNC    },
#if (RTE_ENET_MII_TX_ER_PIN_EN)
  { RTE_ENET_MII_TX_ER_PORT,   RTE_ENET_MII_TX_ER_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_TX_ER_FUNC     },
#endif
  { RTE_ENET_MII_RXD0_PORT,    RTE_ENET_MII_RXD0_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RXD0_FUNC      },
  { RTE_ENET_MII_RXD1_PORT,    RTE_ENET_MII_RXD1_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RXD1_FUNC      },
  { RTE_ENET_MII_RXD2_PORT,    RTE_ENET_MII_RXD2_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RXD2_FUNC      },
  { RTE_ENET_MII_RXD3_PORT,    RTE_ENET_MII_RXD3_PIN,    SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RXD3_FUNC      },
  { RTE_ENET_MII_RX_DV_PORT,   RTE_ENET_MII_RX_DV_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RX_DV_FUNC     },
  { RTE_ENET_MII_RX_CLK_PORT,  RTE_ENET_MII_RX_CLK_PIN,  SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RX_CLK_FUNC    },
  { RTE_ENET_MII_RX_ER_PORT,   RTE_ENET_MII_RX_ER_PIN,   SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_RX_ER_FUNC     },
  { RTE_ENET_MII_COL_PORT,     RTE_ENET_MII_COL_PIN,     SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_COL_FUNC       },
  { RTE_ENET_MII_CRS_PORT,     RTE_ENET_MII_CRS_PIN,     SCU_SFS_EHS | SCU_SFS_EZI | RTE_ENET_MII_CRS_FUNC       },
#endif
};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_MAC_API_VERSION,
  ARM_ETH_MAC_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_ETH_MAC_CAPABILITIES DriverCapabilities = {
  0,                                /* checksum_offload_rx_ip4  */
  0,                                /* checksum_offload_rx_ip6  */
  0,                                /* checksum_offload_rx_udp  */
  0,                                /* checksum_offload_rx_tcp  */
  0,                                /* checksum_offload_rx_icmp */
  0,                                /* checksum_offload_tx_ip4  */
  0,                                /* checksum_offload_tx_ip6  */
  0,                                /* checksum_offload_tx_udp  */
  0,                                /* checksum_offload_tx_tcp  */
  0,                                /* checksum_offload_tx_icmp */
  (RTE_ENET_RMII) ?
  ARM_ETH_INTERFACE_RMII :
  ARM_ETH_INTERFACE_MII,            /* media_interface          */
  0,                                /* mac_address              */
  1,                                /* event_rx_frame           */
  1,                                /* event_tx_frame           */
  1,                                /* event_wakeup             */
  (EMAC_TIME_STAMP) ? 1 : 0         /* precision_timer          */
};

/* Local variables */
static EMAC_CTRL  emac_control = { 0 };
#define emac     (emac_control)
static RX_Desc   rx_desc[NUM_RX_BUF];
static TX_Desc   tx_desc[NUM_TX_BUF];
static uint32_t  rx_buf [NUM_RX_BUF][ETH_BUF_SIZE>>2];
static uint32_t  tx_buf [NUM_TX_BUF][ETH_BUF_SIZE>>2];

/* Local functions */
static void init_rx_desc (void);
static void init_tx_desc (void);
static uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val);
static uint32_t crc32_data (const uint8_t *data, uint32_t len);

/**
  \fn          void init_rx_desc (void)
  \brief       Initialize Rx DMA descriptors.
  \return      none.
*/
static void init_rx_desc (void) {
  uint32_t i,next;

  for (i = 0; i < NUM_RX_BUF; i++) {
    rx_desc[i].Stat = EMAC_RDES0_OWN;
    rx_desc[i].Ctrl = EMAC_RDES1_RCH | ETH_BUF_SIZE;
    rx_desc[i].Addr = (uint8_t *)&rx_buf[i];
    next = i + 1;
    if (next == NUM_RX_BUF) next = 0;
    rx_desc[i].Next = &rx_desc[next];
  }
  ENET->DMA_REC_DES_ADDR = (uint32_t)&rx_desc[0];
  emac.rx_index = 0;
}

/**
  \fn          void init_tx_desc (void)
  \brief       Initialize Tx DMA descriptors.
  \return      none.
*/
static void init_tx_desc (void) {
  uint32_t i,next;

  for (i = 0; i < NUM_TX_BUF; i++) {
    tx_desc[i].CtrlStat = EMAC_TDES0_TCH | EMAC_TDES0_LS | EMAC_TDES0_FS;
    tx_desc[i].Addr     = (uint8_t *)&tx_buf[i];
    next = i + 1;
    if (next == NUM_TX_BUF) next = 0;
    tx_desc[i].Next     = &tx_desc[next];
  }
  ENET->DMA_TRANS_DES_ADDR = (uint32_t)&tx_desc[0];
  emac.tx_index = 0;
}

/**
  \fn          uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val)
  \brief       Calculate 32-bit CRC (Polynom: 0x04C11DB7, data bit-reversed).
  \param[in]   crc32  CRC initial value
  \param[in]   val    Input value
  \return      Calculated CRC value
*/
static uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val) {
  uint32_t n;

  crc32 ^= __RBIT (val);
  for (n = 8; n; n--) {
    if (crc32 & 0x80000000) {
      crc32 <<= 1;
      crc32  ^= 0x04C11DB7;
    } else {
      crc32 <<= 1;
    }
  }
  return (crc32);
}

/**
  \fn          uint32_t crc32_data (const uint8_t *data, uint32_t len)
  \brief       Calculate standard 32-bit Ethernet CRC.
  \param[in]   data  Pointer to buffer containing the data
  \param[in]   len   Data length in bytes
  \return      Calculated CRC value
*/
static uint32_t crc32_data (const uint8_t *data, uint32_t len) {
  uint32_t crc;

  for (crc = 0xFFFFFFFF; len; len--) {
    crc = crc32_8bit_rev (crc, *data++);
  }
  return (crc ^ 0xFFFFFFFF);
}


/* Ethernet Driver functions */

/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_ETH_MAC_CAPABILITIES GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_ETH_MAC_CAPABILITIES
*/
static ARM_ETH_MAC_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)
  \brief       Initialize Ethernet MAC Device.
  \param[in]   cb_event  Pointer to \ref ARM_ETH_MAC_SignalEvent
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_MAC_SignalEvent_t cb_event) {
  const PIN_ID *pin;

  if (emac.flags & EMAC_FLAG_INIT) { return ARM_DRIVER_OK; }

  /* Configure EMAC pins */
  for (pin = eth_pins; pin != &eth_pins[sizeof(eth_pins)/sizeof(PIN_ID)]; pin++) {
    if (pin->port == 0x10) {
      SCU_CLK_PinConfigure (pin->num, pin->config_val);
      continue;
    }
    SCU_PinConfigure(pin->port, pin->num, pin->config_val);
  }
  
  /* Clear control structure */
  memset (&emac, 0, sizeof (EMAC_CTRL));

  emac.cb_event = cb_event;
  emac.flags    = EMAC_FLAG_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet MAC Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {
  const PIN_ID *pin;

  emac.flags = 0;

  /* Unconfigure ethernet pins */
  for (pin = eth_pins; pin != &eth_pins[sizeof(eth_pins)/sizeof(PIN_ID)]; pin++) {
    if (pin->port == 0x10) {
      SCU_CLK_PinConfigure (pin->num, 0);
      continue;
    }
    SCU_PinConfigure(pin->port, pin->num, 0);
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet MAC Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  uint32_t clk;

  switch (state) {
    case ARM_POWER_OFF:
      /* Disable EMAC interrupts */
      NVIC_DisableIRQ(ETHERNET_IRQn);

      /* Reset EMAC peripheral */
      LPC_RGU->RESET_CTRL0 = RGU_RESET_EMAC;
      while (!(LPC_RGU->RESET_ACTIVE_STATUS0 & RGU_RESET_EMAC));

      /* Disable EMAC peripheral clock */
      LPC_CCU1->CLK_M3_ETHERNET_CFG &= ~CCU_CLK_CFG_RUN;

      emac.flags &= ~EMAC_FLAG_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((emac.flags & EMAC_FLAG_INIT)  == 0) { return ARM_DRIVER_ERROR; }
      if ((emac.flags & EMAC_FLAG_POWER) != 0) { return ARM_DRIVER_OK; }

      /* Enable EMAC peripheral clock */
      LPC_CCU1->CLK_M3_ETHERNET_CFG |=  CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while (!(LPC_CCU1->CLK_M3_ETHERNET_STAT & CCU_CLK_STAT_RUN));

      /* Configure Ethernet PHY interface mode (MII/RMII) */
      /* EMAC must be reset after changing PHY interface! */
      #if (RTE_ENET_RMII)
        LPC_CREG->CREG6 = (LPC_CREG->CREG6 & ~EMAC_CREG6_ETH_MASK) | EMAC_CREG6_ETH_RMII;
      #else
        LPC_CREG->CREG6 = (LPC_CREG->CREG6 & ~EMAC_CREG6_ETH_MASK) | EMAC_CREG6_ETH_MII;
      #endif

      /* Reset EMAC peripheral */
      LPC_RGU->RESET_CTRL0 = RGU_RESET_EMAC;
      while (!(LPC_RGU->RESET_ACTIVE_STATUS0 & RGU_RESET_EMAC));

      /* Soft reset EMAC DMA controller */
      ENET->DMA_BUS_MODE |= EMAC_DBMR_SWR;
      while (ENET->DMA_BUS_MODE & EMAC_DBMR_SWR);

      /* MDC clock range selection */
      clk = GetClockFreq (CLK_SRC_PLL1);
      if      (clk >= 150000000) emac.mmar_cr_val = EMAC_MMAR_CR_Div102;
      else if (clk >= 100000000) emac.mmar_cr_val = EMAC_MMAR_CR_Div62;
      else if (clk >= 60000000)  emac.mmar_cr_val = EMAC_MMAR_CR_Div42;
      else if (clk >= 35000000)  emac.mmar_cr_val = EMAC_MMAR_CR_Div26;
      else if (clk >= 25000000)  emac.mmar_cr_val = EMAC_MMAR_CR_Div16;
      else                       return ARM_DRIVER_ERROR_UNSUPPORTED;
      ENET->MAC_MII_ADDR = emac.mmar_cr_val;

      #if (EMAC_TIME_STAMP)
        /* Enhanced DMA descriptor enable */
        ENET->DMA_BUS_MODE |= EMAC_DBMR_ATDS;

        /* Set clock accuracy to 20ns (50MHz) or 50ns (20MHz) */
        if (clk >= 51000000) {
          ENET->SUBSECOND_INCR = 20;
          ENET->ADDEND         = (50000000ULL << 32) / clk;
        }
        else {
          ENET->SUBSECOND_INCR = 50;
          ENET->ADDEND         = (20000000ULL << 32) / clk;
        }

        /* Enable timestamp fine update */
        ENET->MAC_TIMESTP_CTRL = EMAC_MTCR_TSIPV4E | EMAC_MTCR_TSIPV6E |
                                 EMAC_MTCR_TSCTRL  | EMAC_MTCR_TSADDR  |
                                 EMAC_MTCR_TSCFUP  | EMAC_MTCR_TSENA;
        emac.tx_ts_index = 0;
      #endif

      /* Initialize MAC configuration */
      ENET->MAC_CONFIG = EMAC_MCR_DO | EMAC_MCR_PS;

      /* Initialize Filter registers */
      ENET->MAC_FRAME_FILTER = EMAC_MFFR_DBF;
      ENET->MAC_FLOW_CTRL    = EMAC_MFCR_DZPQ;

      /* Initialize Address register */
      ENET->MAC_ADDR0_HIGH = 0x00000000;
      ENET->MAC_ADDR0_LOW  = 0x00000000;

      /* Disable MAC interrupts */
      ENET->MAC_INTR_MASK  = EMAC_MIMR_PMTIM | EMAC_MIMR_TSIM;

      /* Initialize DMA Descriptors */
      init_rx_desc ();
      init_tx_desc ();

      /* Enable DMA interrupts */
      ENET->DMA_STAT   = 0xFFFFFFFF;
      ENET->DMA_INT_EN = EMAC_DIER_NIE | EMAC_DIER_RIE | EMAC_DIER_TIE;

      /* Enable ethernet interrupts */
      NVIC_ClearPendingIRQ(ETHERNET_IRQn);
      NVIC_EnableIRQ(ETHERNET_IRQn);

      emac.frame_end = NULL;
      emac.flags    |= EMAC_FLAG_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Get Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr) {
  uint32_t val;

  if (!ptr_addr) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  val = ENET->MAC_ADDR0_HIGH;
  ptr_addr->b[5] = (uint8_t)(val >> 8);
  ptr_addr->b[4] = (uint8_t)(val);
  val = ENET->MAC_ADDR0_LOW;
  ptr_addr->b[3] = (uint8_t)(val >> 24);
  ptr_addr->b[2] = (uint8_t)(val >> 16);
  ptr_addr->b[1] = (uint8_t)(val >>  8);
  ptr_addr->b[0] = (uint8_t)(val);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Set Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr) {

  if (!ptr_addr) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  /* Set Ethernet MAC Address registers */
  ENET->MAC_ADDR0_HIGH = (ptr_addr->b[5] <<  8) |  ptr_addr->b[4];
  ENET->MAC_ADDR0_LOW  = (ptr_addr->b[3] << 24) | (ptr_addr->b[2] << 16) |
                         (ptr_addr->b[1] <<  8) |  ptr_addr->b[0];
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr,
                                               uint32_t          num_addr)
  \brief       Configure Address Filter.
  \param[in]   ptr_addr  Pointer to addresses
  \param[in]   num_addr  Number of addresses to configure
  \return      \ref execution_status
*/
static int32_t SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr) {
  uint32_t crc;

  if (!ptr_addr && num_addr) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  ENET->MAC_FRAME_FILTER &= ~(EMAC_MFFR_HPF | EMAC_MFFR_HMC);
  ENET->MAC_HASHTABLE_HIGH = 0x00000000;
  ENET->MAC_HASHTABLE_LOW  = 0x00000000;

  if (num_addr == 0) {
    return ARM_DRIVER_OK;
  }

  /* Calculate 64-bit Hash table for MAC addresses */
  for ( ; num_addr; ptr_addr++, num_addr--) {
    crc = crc32_data (&ptr_addr->b[0], 6) >> 26;
    if (crc & 0x20) {
      ENET->MAC_HASHTABLE_HIGH |= (1 << (crc & 0x1F));
    }
    else {
      ENET->MAC_HASHTABLE_LOW  |= (1 << crc);
    }
  }
  /* Enable both, unicast and hash address filtering */
  ENET->MAC_FRAME_FILTER |= EMAC_MFFR_HPF | EMAC_MFFR_HMC;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
  \brief       Send Ethernet frame.
  \param[in]   frame  Pointer to frame buffer with data to send
  \param[in]   len    Frame buffer length in bytes
  \param[in]   flags  Frame transmit flags (see ARM_ETH_MAC_TX_FRAME_...)
  \return      \ref execution_status
*/
static int32_t SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags) {
  uint8_t *dst;
  uint32_t ctrl;

  if (!frame || !len) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  dst = emac.frame_end;
  if (dst == NULL) {
    /* Start of a new transmit frame */
    if (tx_desc[emac.tx_index].CtrlStat & EMAC_TDES0_OWN) {
      /* Transmitter is busy, wait */
      return ARM_DRIVER_ERROR_BUSY;
    }
    dst = tx_desc[emac.tx_index].Addr;
    tx_desc[emac.tx_index].Size = len;
  }
  else {
    /* Sending data fragments in progress */
    tx_desc[emac.tx_index].Size += len;
  }
  /* Fast-copy data fragments to EMAC-DMA buffer */
  for ( ; len > 7; dst += 8, frame += 8, len -= 8) {
    ((__packed uint32_t *)dst)[0] = ((__packed uint32_t *)frame)[0];
    ((__packed uint32_t *)dst)[1] = ((__packed uint32_t *)frame)[1];
  }
  /* Copy remaining 7 bytes */
  for ( ; len > 1; dst += 2, frame += 2, len -= 2) {
    ((__packed uint16_t *)dst)[0] = ((__packed uint16_t *)frame)[0];
  }
  if (len > 0) dst++[0] = frame++[0];

  if (flags & ARM_ETH_MAC_TX_FRAME_FRAGMENT) {
    /* More data to come, remember current write position */
    emac.frame_end = dst;
    return ARM_DRIVER_OK;
  }

  /* Frame is now ready, send it to DMA */
  ctrl = tx_desc[emac.tx_index].CtrlStat & ~(EMAC_TDES0_IC | EMAC_TDES0_TTSE);
  if (flags & ARM_ETH_MAC_TX_FRAME_EVENT)     ctrl |= EMAC_TDES0_IC;
#if (EMAC_TIME_STAMP)
  if (flags & ARM_ETH_MAC_TX_FRAME_TIMESTAMP) ctrl |= EMAC_TDES0_TTSE;
  emac.tx_ts_index = emac.tx_index;
#endif
  tx_desc[emac.tx_index].CtrlStat = ctrl | EMAC_TDES0_OWN;

  if (++emac.tx_index == NUM_TX_BUF) emac.tx_index = 0;
  emac.frame_end = NULL;

  /* Start frame transmission */
  ENET->DMA_STAT = EMAC_DSR_TPS;
  ENET->DMA_TRANS_POLL_DEMAND = 0;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ReadFrame (uint8_t *frame, uint32_t len)
  \brief       Read data of received Ethernet frame.
  \param[in]   frame  Pointer to frame buffer for data to read into
  \param[in]   len    Frame buffer length in bytes
  \return      number of data bytes read or execution status
                 - value >= 0: number of data bytes read
                 - value < 0: error occurred, value is execution status as defined with \ref execution_status 
*/
static int32_t ReadFrame (uint8_t *frame, uint32_t len) {
  uint8_t const *src;
  int32_t cnt = (int32_t)len;

  if (!frame && len) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  /* Fast-copy data to packet buffer */
  src = rx_desc[emac.rx_index].Addr;
  for ( ; len > 7; frame += 8, src += 8, len -= 8) {
    ((__packed uint32_t *)frame)[0] = ((uint32_t *)src)[0];
    ((__packed uint32_t *)frame)[1] = ((uint32_t *)src)[1];
  }
  /* Copy remaining 7 bytes */
  for ( ; len > 1; frame += 2, src += 2, len -= 2) {
    ((__packed uint16_t *)frame)[0] = ((uint16_t *)src)[0];
  }
  if (len > 0) frame[0] = src[0];

  /* Return this block back to EMAC-DMA */
  rx_desc[emac.rx_index].Stat = EMAC_RDES0_OWN;

  if (++emac.rx_index == NUM_RX_BUF) emac.rx_index = 0;

  if (ENET->DMA_STAT & EMAC_DSR_RU) {
    /* Receive buffer unavailable, resume DMA */
    ENET->DMA_STAT = EMAC_DSR_RU;
    ENET->DMA_REC_POLL_DEMAND = 0;
  }
  return (cnt);
}

/**
  \fn          uint32_t GetRxFrameSize (void)
  \brief       Get size of received Ethernet frame.
  \return      number of bytes in received frame
*/
static uint32_t GetRxFrameSize (void) {
  uint32_t stat;

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return (0);
  }

  stat = rx_desc[emac.rx_index].Stat;
  if (stat & EMAC_RDES0_OWN) {
    /* Owned by DMA */
    return (0);
  }

  if ((stat & EMAC_RDES0_ES) || !(stat & EMAC_RDES0_FS) || !(stat & EMAC_RDES0_LS)) {
    /* Error, this block is invalid */
    return (0xFFFFFFFF);
  }
  return (((stat & EMAC_RDES0_FL) >> 16) - 4);
}

/**
  \fn          int32_t GetRxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of received Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t GetRxFrameTime (ARM_ETH_MAC_TIME *time) {
#if (EMAC_TIME_STAMP)
  RX_Desc *rxd;

  if (!time) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  rxd = &rx_desc[emac.rx_index];
  if (rxd->Stat & EMAC_RDES0_OWN) {
    /* Owned by DMA */
    return ARM_DRIVER_ERROR_BUSY;
  }
  time->ns  = rxd->TimeLo;
  time->sec = rxd->TimeHi;

  return ARM_DRIVER_OK;
#else
  return ARM_DRIVER_ERROR_UNSUPPORTED;
#endif
}

/**
  \fn          int32_t GetTxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of transmitted Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t GetTxFrameTime (ARM_ETH_MAC_TIME *time) {
#if (EMAC_TIME_STAMP)
  TX_Desc *txd;

  if (!time) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  txd = &tx_desc[emac.tx_ts_index];
  if (txd->CtrlStat & EMAC_TDES0_OWN) {
    /* Owned by DMA */
    return ARM_DRIVER_ERROR_BUSY;
  }
  if (!(txd->CtrlStat & EMAC_TDES0_TTSS)) {
    /* No transmit time stamp available */
    return ARM_DRIVER_ERROR;
  }
  time->ns  = txd->TimeLo;
  time->sec = txd->TimeHi;
  return ARM_DRIVER_OK;
#else
  return ARM_DRIVER_ERROR_UNSUPPORTED;
#endif
}

/**
  \fn          int32_t Control (uint32_t control, uint32_t arg)
  \brief       Control Ethernet Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      \ref execution_status
*/
static int32_t Control (uint32_t control, uint32_t arg) {
  uint32_t maccr;
  uint32_t macffr;

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_ETH_MAC_CONFIGURE:
      maccr = ENET->MAC_CONFIG & ~(EMAC_MCR_FES | EMAC_MCR_DM | EMAC_MCR_LM);

      /* Configure 100MBit/10MBit mode */
      switch (arg & ARM_ETH_MAC_SPEED_Msk) {
        case ARM_ETH_MAC_SPEED_10M:
#if (RTE_ENET_RMII)
          /* RMII Half Duplex Collision detection does not work */
          maccr |= EMAC_MCR_DM;
#endif
          break;
        case ARM_ETH_SPEED_100M:
          maccr |= EMAC_MCR_FES;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      /* Configure Half/Full duplex mode */
      switch (arg & ARM_ETH_MAC_DUPLEX_Msk) {
        case ARM_ETH_MAC_DUPLEX_FULL:
          maccr |= EMAC_MCR_DM;
          break;
      }

      /* Configure loopback mode */
      if (arg & ARM_ETH_MAC_LOOPBACK) {
        maccr |= EMAC_MCR_LM;
      }

      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) ||
          (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX)) {
        /* Checksum offload is disabled in the driver */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      ENET->MAC_CONFIG = maccr;

      macffr = ENET->MAC_FRAME_FILTER & ~(EMAC_MFFR_PR | EMAC_MFFR_PAM | EMAC_MFFR_DBF);
      /* Enable broadcast frame receive */
      if (!(arg & ARM_ETH_MAC_ADDRESS_BROADCAST)) {
        macffr |= EMAC_MFFR_DBF;
      }

      /* Enable all multicast frame receive */
      if (arg & ARM_ETH_MAC_ADDRESS_MULTICAST) {
        macffr |= EMAC_MFFR_PAM;
      }

      /* Enable promiscuous mode (no filtering) */
      if (arg & ARM_ETH_MAC_ADDRESS_ALL) {
        macffr |= EMAC_MFFR_PR;
      }
      ENET->MAC_FRAME_FILTER = macffr;

      break;

    case ARM_ETH_MAC_CONTROL_TX:
      /* Enable/disable MAC transmitter */
      if (arg != 0) {
        ENET->MAC_CONFIG  |= EMAC_MCR_TE;
        ENET->DMA_OP_MODE |= EMAC_DOMR_ST;
      }
      else {
        ENET->DMA_OP_MODE &= ~EMAC_DOMR_ST;
        ENET->MAC_CONFIG  &= ~EMAC_MCR_TE;
      }
      break;

    case ARM_ETH_MAC_CONTROL_RX:
      /* Enable/disable MAC receiver */
      if (arg != 0) {
        ENET->MAC_CONFIG  |= EMAC_MCR_RE;
        ENET->DMA_OP_MODE |= EMAC_DOMR_SR;
      }
      else {
        ENET->DMA_OP_MODE &= ~EMAC_DOMR_SR;
        ENET->MAC_CONFIG  &= ~EMAC_MCR_RE;
      }
      break;

    case ARM_ETH_MAC_FLUSH:
      /* Flush Tx and Rx buffers */
      if (arg & ARM_ETH_MAC_FLUSH_RX) {
        /* Stop/Start DMA Receive */
        uint32_t domr = ENET->DMA_OP_MODE;
        ENET->DMA_OP_MODE &= ~EMAC_DOMR_SR;
        init_rx_desc ();
        ENET->DMA_OP_MODE = domr;
      }
      if (arg & ARM_ETH_MAC_FLUSH_TX) {
        /* Stop/Start DMA Transmit */
        uint32_t domr = ENET->DMA_OP_MODE;
        ENET->DMA_OP_MODE &= ~EMAC_DOMR_ST;
        /* Flush transmit FIFO */
        ENET->DMA_OP_MODE |= EMAC_DOMR_FTF;
        init_tx_desc ();
        ENET->DMA_OP_MODE = domr;
      }
      break;

    case ARM_ETH_MAC_SLEEP:
      /* Enable/disable Sleep mode */
      if (arg != 0) {
        /* Enable Power Management interrupts */
        ENET->MAC_INTR_MASK    &= ~EMAC_MIMR_PMTIM;
        /* Enter Power-down, Magic packet enable */
        ENET->MAC_PMT_CTRL_STAT = EMAC_PMTR_MPE | EMAC_PMTR_PD;
      }
      else {
        /* Disable Power Management interrupts */
        ENET->MAC_INTR_MASK    |= EMAC_MIMR_PMTIM;
        ENET->MAC_PMT_CTRL_STAT = 0x00000000;
      }
      break;

    case ARM_ETH_MAC_VLAN_FILTER:
      /* Configure VLAN filter */
      ENET->MAC_VLAN_TAG = arg;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)
  \brief       Control Precision Timer.
  \param[in]   control  Operation
  \param[in]   time     Pointer to time structure
  \return      \ref execution_status
*/
static int32_t ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time) {

#if (EMAC_TIME_STAMP)
  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_ETH_MAC_TIMER_GET_TIME:
      /* Get current time */
      time->sec = ENET->SECONDS;
      time->ns  = ENET->NANOSECONDS;
      break;

    case ARM_ETH_MAC_TIMER_SET_TIME:
      /* Set new time */
      ENET->SECONDSUPDATE     = time->sec;
      ENET->NANOSECONDSUPDATE = time->ns;
      /* Initialize precision timer */
      ENET->MAC_TIMESTP_CTRL |= EMAC_MTCR_TSINIT;
      break;

    case ARM_ETH_MAC_TIMER_INC_TIME:
      /* Increment current time */
      ENET->SECONDSUPDATE     = time->sec;
      ENET->NANOSECONDSUPDATE = time->ns;
      /* Update precision timer */
      ENET->MAC_TIMESTP_CTRL |=  EMAC_MTCR_TSUPDT;
      break;

    case ARM_ETH_MAC_TIMER_DEC_TIME:
      /* Decrement current time */
      ENET->SECONDSUPDATE     = time->sec;
      ENET->NANOSECONDSUPDATE = time->ns | 0x80000000;
      /* Update precision timer */
      ENET->MAC_TIMESTP_CTRL |=  EMAC_MTCR_TSUPDT;
      break;

    case ARM_ETH_MAC_TIMER_SET_ALARM:
      /* Set alarm time */
      ENET->TARGETSECONDS     = time->sec;
      ENET->TARGETNANOSECONDS = time->ns;
      /* Enable timestamp interrupt trigger */
      ENET->MAC_TIMESTP_CTRL |= EMAC_MTCR_TSTRIG;
      if (time->sec || time->ns) {
        /* Enable timestamp interrupts */
        ENET->MAC_INTR_MASK &= ~EMAC_MIMR_TSIM;
        break;
      }
      /* Disable timestamp interrupts */
      ENET->MAC_INTR_MASK |= EMAC_MIMR_TSIM;
      break;

    case ARM_ETH_MAC_TIMER_ADJUST_CLOCK:
      /* Adjust current time, fine correction */
      /* Correction factor is Q31 (0x80000000 = 1.000000000) */
      ENET->ADDEND = ((uint64_t)time->ns * ENET->ADDEND) >> 31;
      /* Update addend register */
      ENET->MAC_TIMESTP_CTRL |= EMAC_MTCR_TSADDR;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
#else
  return ARM_DRIVER_ERROR_UNSUPPORTED;
#endif
}

/**
  \fn          int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
  \brief       Read Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[out]  data      Pointer where the result is written to
  \return      \ref execution_status
*/
static int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data) {
  uint32_t tick;

  if (!data) {
    /* Invalid parameter */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  ENET->MAC_MII_ADDR  = emac.mmar_cr_val | EMAC_MMAR_GB |
                        (phy_addr << 11) | (reg_addr << 6);

  /* Wait until operation completed */
  tick = osKernelSysTick();
  do {
    if (!(ENET->MAC_MII_ADDR & EMAC_MMAR_GB)) {
      *data = ENET->MAC_MII_DATA & EMAC_MMDR_GD;
      return ARM_DRIVER_OK;
    }
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(PHY_TIMEOUT));

  if (!(ENET->MAC_MII_ADDR & EMAC_MMAR_GB)) {
    *data = ENET->MAC_MII_DATA & EMAC_MMDR_GD;
    return ARM_DRIVER_OK;
  }
  return ARM_DRIVER_ERROR_TIMEOUT;
}

/**
  \fn          int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
  \brief       Write Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[in]   data      16-bit data to write
  \return      \ref execution_status
*/
static int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data) {
  uint32_t tick;

  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  ENET->MAC_MII_DATA  = data;
  ENET->MAC_MII_ADDR  = emac.mmar_cr_val | EMAC_MMAR_GB | EMAC_MMAR_W |
                        (phy_addr << 11) | (reg_addr << 6);

  /* Wait until operation completed */
  tick = osKernelSysTick();
  do {
    if (!(ENET->MAC_MII_ADDR & EMAC_MMAR_GB)) {
      return ARM_DRIVER_OK;
    }
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(PHY_TIMEOUT));

  if (!(ENET->MAC_MII_ADDR & EMAC_MMAR_GB)) {
    return ARM_DRIVER_OK;
  }
  return ARM_DRIVER_ERROR_TIMEOUT;
}

/**
  \fn          void ETH_IRQHandler (void)
  \brief       Ethernet Interrupt handler.
*/
void ETH_IRQHandler (void) {
  uint32_t stat,event = 0;

  stat = ENET->DMA_STAT;
  ENET->DMA_STAT = stat & (EMAC_DSR_NIS | EMAC_DSR_RI | EMAC_DSR_TI);
  if (stat & EMAC_DSR_TI) {
    /* Transmit interrupt */
    event |= ARM_ETH_MAC_EVENT_TX_FRAME;
  }
  if (stat & EMAC_DSR_RI) {
    /* Receive interrupt */
    event |= ARM_ETH_MAC_EVENT_RX_FRAME;
  }
  stat = ENET->MAC_INTR;
#if (EMAC_TIME_STAMP)
  if (stat & EMAC_MISR_TS) {
    /* Timestamp interrupt */
    if (ENET->TIMESTAMPSTAT & EMAC_MTSR_TSTARGT) {
      /* Alarm trigger interrupt */
      event |= ARM_ETH_MAC_EVENT_TIMER_ALARM;
    }
  }
#endif
  if (stat & EMAC_MISR_PMT) {
    /* Power management interrupt */
    if (ENET->MAC_PMT_CTRL_STAT & EMAC_PMTR_MPR) {
      /* Magic packet received */
      event |= ARM_ETH_MAC_EVENT_WAKEUP;
    }
  }
  /* Callback event notification */
  if (event && emac.cb_event) {
    emac.cb_event (event);
  }
}

/* MAC Driver Control Block */
ARM_DRIVER_ETH_MAC Driver_ETH_MAC0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  GetMacAddress,
  SetMacAddress,
  SetAddressFilter,
  SendFrame,
  ReadFrame,
  GetRxFrameSize,
  GetRxFrameTime,
  GetTxFrameTime,
  ControlTimer,
  Control,
  PHY_Read,
  PHY_Write
};

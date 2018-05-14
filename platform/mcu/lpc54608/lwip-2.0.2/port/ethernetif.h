/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SDRVL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include "lwip/err.h"
#include "fsl_enet.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef ENET_RXBD_NUM
    #define ENET_RXBD_NUM (5)
#endif
#ifndef ENET_TXBD_NUM
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
    #define ENET_TXBD_NUM (5)
#else
    #define ENET_TXBD_NUM (3)
#endif
#endif
#ifndef ENET_RXBUFF_SIZE
    #define ENET_RXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#endif
#ifndef ENET_TXBUFF_SIZE
    #define ENET_TXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#endif

#define ENET_OK             (0U)
#define ENET_ERROR          (0xFFU)
#define ENET_TIMEOUT        (0xFFFU)

/* ENET IRQ priority. Used in FreeRTOS. */
/* Interrupt priorities. */
#ifdef __CA7_REV
#ifndef ENET_PRIORITY
    #define ENET_PRIORITY       (21U)
#endif
#ifndef ENET_1588_PRIORITY
    #define ENET_1588_PRIORITY  (20U)
#endif
#else
#ifndef ENET_PRIORITY
    #define ENET_PRIORITY       (6U)
#endif
#ifndef ENET_1588_PRIORITY
    #define ENET_1588_PRIORITY  (5U)
#endif
#endif

/*  Defines Ethernet Autonegotiation Timeout during initialization. 
 *  Set it to 0 to disable the waiting. */ 
#ifndef ENET_ATONEGOTIATION_TIMEOUT
    #define ENET_ATONEGOTIATION_TIMEOUT     (0xFFFU)
#endif

/**
 * Helper struct to hold data for configuration of ethernet interface.
 */
typedef struct ethernetif_config
{
    uint32_t phyAddress;
    clock_name_t clockName;
    uint8_t macAddress[NETIF_MAX_HWADDR_LEN];
} ethernetif_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the first ENET interface.
 */
err_t ethernetif0_init(struct netif *netif);

#if (defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 1)) \
 || (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 1))
/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the second ENET interface.
 */
err_t ethernetif1_init(struct netif *netif);
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */

/**
 * This function should be called when a packet is ready to be read
 * from the interface. 
 * It is used by bare-metal applications.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input( struct netif *netif);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

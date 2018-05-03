/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Include file for the Contiki low-layer network stack (NETSTACK)
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef WIFINETSTACK_H
#define WIFINETSTACK_H
#include "soc_types.h"

/**
 * The structure of a device driver for a radio in Contiki.
 */
struct radio_driver {

  int (* init)(void);
  
  /** Prepare the radio with a packet to be sent. */
  int (* prepare)(const void *payload, unsigned short payload_len);

  /** Send the packet that has previously been prepared. */
  int (* transmit)(unsigned short transmit_len);

  /** Prepare & transmit a packet. */
  int (* send)(u8 id, void *payload);

  /** Read a received packet into a buffer. */
  int (* read)(void *buf, unsigned short buf_len);

  /** Perform a Clear-Channel Assessment (CCA) to find out if there is
      a packet in the air or not. */
  int (* channel_clear)(void);

  /** Check if the radio driver is currently receiving a packet */
  int (* receiving_packet)(void);

  /** Check if the radio driver has just received a packet */
  int (* pending_packet)(void);

  /** Turn the radio on. */
  int (* on)(void);

  /** Turn the radio off. */
  int (* off)(void);
};

/**
 * The structure of a MAC protocol driver in Contiki.
 */
struct mac_driver {
  char *name;

  /** Initialize the MAC driver */
  void (* init)(void);

  /** Send a packet from the Rime buffer  */
  void (* send)(char *ifname, u8 *data, u32 datalen);

  /** Callback for getting notified of incoming packet. */
  void (* input)(void);
  
  /** Turn the MAC layer on. */
  int (* on)(void);

  /** Turn the MAC layer off. */
  int (* off)(int keep_radio_on);

  /** Returns the channel check interval, expressed in clock_time_t ticks. */
  unsigned short (* channel_check_interval)(void);
};

/**
 * The structure of a network driver in Contiki.
 */
struct network_driver {
  char *name;

  /** Initialize the network driver */
  int (* init)(void* data);

  /** Callback for getting notified of incoming packet. */
  int (* input)(char *ifname, void *data, u32 len);
  
  int (* output)(char *name, void *data, u32 len);
};

extern const struct network_driver ieee80211_net;
extern const struct mac_driver ieee80211_mac;
extern const struct radio_driver ssv6200_radio_driver;

#define NETSTACK_CONF_NETWORK 	ieee80211_net
#define NETSTACK_CONF_MAC		ieee80211_mac
#define NETSTACK_CONF_RADIO     ssv6200_radio_driver

#define NETSTACK_NETWORK NETSTACK_CONF_NETWORK
#define NETSTACK_MAC    NETSTACK_CONF_MAC
#define NETSTACK_RADIO  NETSTACK_CONF_RADIO

#include "radio.h"



//extern const struct network_driver NETSTACK_NETWORK;
/*extern const struct rdc_driver     NETSTACK_RDC;
extern const struct mac_driver     NETSTACK_MAC;
extern const struct radio_driver   NETSTACK_RADIO;*/

//void netstack_init(void);

#endif /* WIFINETSTACK_H */

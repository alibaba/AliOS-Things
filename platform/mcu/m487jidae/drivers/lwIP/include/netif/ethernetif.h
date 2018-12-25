/*
 * Copyright (c) 2013 Nuvoton Technology Corp.
 * Description:  EMAC driver header file
 */
#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

#endif

/************************************************************************/
/**	\file	c674emac.h
 *	\brief	LwIP network interface for Blackfin built-in EMAC
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/
#ifndef _C674EMAC_H_
#define _C674EMAC_H_

/* Includes */
#include "lwip/netif.h"

/* Macros */

/* Types */

/* Externals */

/* Functions */
#ifdef __cplusplus
extern "C" {
#endif

	err_t c674emac_init(struct netif *netif);
#if NO_SYS
	void c674emac_poll(struct netif* netif);
#endif

#ifdef __cplusplus
}
#endif


/* Include Guard */
#endif

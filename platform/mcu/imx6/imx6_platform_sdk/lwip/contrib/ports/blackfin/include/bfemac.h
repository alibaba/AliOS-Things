/************************************************************************/
/**	\file	bfemac.h
 *	\brief	LwIP network interface for Blackfin built-in EMAC
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _BFEMAC_H_
#define _BFEMAC_H_

/* Includes */
#include "lwip/netif.h"

/* Macros */

/* Types */

/* Externals */
extern u8_t		bfemac_rmii;				//!< RMII mode, default 0 for MII

/* Functions */
extern "C"
{
	err_t bfemac_init(struct netif *netif);
#if NO_SYS
	void bfemac_poll(struct netif* netif);
#endif
}

/* Include Guard */
#endif

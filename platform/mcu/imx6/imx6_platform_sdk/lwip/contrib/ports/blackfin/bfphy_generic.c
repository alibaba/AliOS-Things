/************************************************************************/
/**	\file	bfphy_generic.c
 *	\brief	LwIP network interface for generic PHY
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include <blackfin.h>
#include <sys/exception.h>
#include "bfphy.h"
#include "lwip/sys.h"

/* Macros */

/* Types */

/* Externals */
extern volatile u8_t bfphy_event;

/* Variables */

/* Functions */

void bfphy_init_vendor()
{
}

void bfphy_interrupt_callback(void* ctx)
{
	SYS_ARCH_DECL_PROTECT(old_level);

	// Read Registers
	u16_t stat = bfphy_read(PHYREG_MODESTAT);
	bfphy_event = 0;

	// Speed/Duplex Mode
	SYS_ARCH_PROTECT(old_level);
	u32_t opmode = *pEMAC_OPMODE;
	if (stat & (PHY_MODESTAT_100BASE_FULL|PHY_MODESTAT_10BASE_FULL))	opmode |= FDMODE;
	else																opmode &= ~FDMODE;
	if (opmode & RMII)
	{
		if (stat & (PHY_MODESTAT_100BASE_FULL|PHY_MODESTAT_100BASE_HALF))	opmode &= ~RMII_10;
		else																opmode |= RMII_10;
	}
	*pEMAC_OPMODE = opmode;
	SYS_ARCH_UNPROTECT(old_level);

	// Link state
	struct netif* netif = (struct netif*) ctx;
	if (stat & PHY_MODESTAT_LINK_STATUS)	netif_set_link_up(netif);
	else									netif_set_link_down(netif);
}

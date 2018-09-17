/************************************************************************/
/**	\file	bfphy_smsc.c
 *	\brief	LwIP network interface for SMSC PHY
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
#define PHYREG_INTS						0x001d	//!< SMSC Interrupt Source
#define PHYREG_INTM						0x001e	//!< SMSC Interrupt Mask
#define PHYREG_SCS						0x001f	//!< SMSC Special Control/Status

#define PHY_INT_ENERGY_ON				0x0080
#define PHY_INT_AUTONEG_COMPLETE		0x0040
#define PHY_INT_REMOTE_FAULT			0x0020
#define PHY_INT_LINKDOWN				0x0010
#define PHY_INT_AUTONEG_LP_ACK			0x0008
#define PHY_INT_PARALLEL_FAULT			0x0004
#define PHY_INT_AUTONEG_PAGE_RECEIVED	0x0002

#define PHY_SCS_FULL					0x0010
#define PHY_SCS_SPEED_100				0x0008
#define PHY_SCS_SPEED_10				0x0004

/* Types */

/* Externals */
extern volatile u8_t bfphy_event;

/* Variables */

/* Functions */

void bfphy_init_vendor()
{
	bfphy_write(PHYREG_INTM, PHY_INT_AUTONEG_COMPLETE|PHY_INT_LINKDOWN);
	bfphy_read(PHYREG_INTS);
}

void bfphy_interrupt_callback(void* ctx)
{
	SYS_ARCH_DECL_PROTECT(old_level);

	// Read Registers
	u16_t ints = bfphy_read(PHYREG_INTS);	// acknowledge interrupts
	u16_t mode = bfphy_read(PHYREG_SCS);
	u16_t stat = bfphy_read(PHYREG_MODESTAT);
	bfphy_event = 0;

	// Speed/Duplex Mode
	SYS_ARCH_PROTECT(old_level);
	u32_t opmode = *pEMAC_OPMODE;
	if (mode & PHY_SCS_FULL)	opmode |= FDMODE;
	else						opmode &= ~FDMODE;
	if (opmode & RMII)
	{
		if (mode & PHY_SCS_SPEED_10)	opmode |= RMII_10;
		else							opmode &= ~RMII_10;
	}
	*pEMAC_OPMODE = opmode;
	SYS_ARCH_UNPROTECT(old_level);

	// Link state
	struct netif* netif = (struct netif*) ctx;
	if (stat & PHY_MODESTAT_AUTONEG_COMPLETE)	netif_set_link_up(netif);
	else										netif_set_link_down(netif);
}

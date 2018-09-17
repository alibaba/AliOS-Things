/************************************************************************/
/**	\file	c674phy_smsc.c
 *	\brief	LwIP network interface for SMSC PHY
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "lwip/sys.h"
#include "c674phy.h"
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_emac1.h>

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
typedef volatile CSL_EmacRegs         *CSL_EmacRegsOvly;

/* Externals */
extern volatile u8_t c674phy_event;

/* Variables */

/* Functions */

void c674phy_init_vendor()
{
//	c674phy_write(PHYREG_INTM, PHY_INT_AUTONEG_COMPLETE|PHY_INT_LINKDOWN);	//|PHY_INT_ENERGY_ON
//	c674phy_read(PHYREG_INTS);
}

void c674phy_interrupt_callback(void* ctx)
{
	SYS_ARCH_DECL_PROTECT(old_level);
	struct netif* netif = (struct netif*) ctx;
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;

	/* Read Registers */
	u16_t ints = c674phy_read(PHYREG_INTS);
	u16_t mode = c674phy_read(PHYREG_SCS);
	u16_t stat = c674phy_read(PHYREG_MODESTAT);
	c674phy_event = 0;

	/* Speed/Duplex Mode */
	SYS_ARCH_PROTECT(old_level);

	if (mode & PHY_SCS_FULL)
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, FULLDUPLEX);
	else
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, HALFDUPLEX);

	if (mode & PHY_SCS_SPEED_10)
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_RMIISPEED, 10MBIT);
	else
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_RMIISPEED, 100MBIT);

	SYS_ARCH_UNPROTECT(old_level);

	/* Link state */
	if (stat & PHY_MODESTAT_AUTONEG_COMPLETE)	netif_set_link_up(netif);
	else										netif_set_link_down(netif);
}

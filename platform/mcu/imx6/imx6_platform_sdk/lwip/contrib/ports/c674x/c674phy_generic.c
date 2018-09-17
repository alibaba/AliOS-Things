/************************************************************************/
/**	\file	c674phy_generic.c
 *	\brief	LwIP network interface for generic PHY
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "lwip/sys.h"
#include "c674phy.h"
#include "c674emac.h"
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_emac1.h>

/* Macros */

/* Types */
typedef volatile CSL_EmacRegs         *CSL_EmacRegsOvly;

/* Externals */

/* Variables */

/* Functions */

void c674phy_init_vendor()
{
}

void c674phy_interrupt_callback(void* ctx)
{
	SYS_ARCH_DECL_PROTECT(old_level);
	struct netif* netif = (struct netif*) ctx;
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;

	u16_t stat = c674phy_read(PHYREG_MODESTAT);	// TODO
	c674phy_event = 0;

	SYS_ARCH_PROTECT(old_level);

	if (stat & (PHY_MODESTAT_100BASE_FULL|PHY_MODESTAT_10BASE_FULL))
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, FULLDUPLEX);
	else
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, HALFDUPLEX);
		
	if (stat & (PHY_MODESTAT_100BASE_FULL|PHY_MODESTAT_100BASE_HALF))
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_RMIISPEED, 100MBIT);
	else
		CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_RMIISPEED, 10MBIT);

	SYS_ARCH_UNPROTECT(old_level);

	if (stat & PHY_MODESTAT_LINK_STATUS)	netif_set_link_up(netif);		// TODO
	else									netif_set_link_down(netif);
}

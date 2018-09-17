/************************************************************************/
/**	\file	c674phy_ksz8893.c
 *	\brief	LwIP network interface for micrel KSZ8893 Switch
 *
 *	\date	2011/10/04
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "lwip/sys.h"
#include "c674phy.h"
#include "c674emac.h"
#include "../ACCESS/i2c.h"
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_emac1.h>

/* Macros */
#define I2C_KSZ8893 (0x5F)

/* Types */
typedef volatile CSL_EmacRegs         *CSL_EmacRegsOvly;

/* Externals */
extern volatile u8_t c674phy_event;

/* Variables */

/* Functions */

u32_t switch_readstat(u8_t regaddr)
{
	u8_t buf[4];
	buf[0] = 0x1C | (regaddr>>8);
	buf[1] = regaddr&0xFF;
	I2C_Reg8Write(I2C_KSZ8893, 121, buf, 2);
	I2C_Reg8Read(I2C_KSZ8893, 128, buf, 4);
	u32_t ret = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	ret &= 0x3FFFFFFF;
	return ret;
}

void switch_printstats(void)
{
	u32_t rx3under = switch_readstat(0x42);
	u32_t rx3frag = switch_readstat(0x43);
	u32_t rx3over = switch_readstat(0x44);
	u32_t rx3jabber = switch_readstat(0x45);
	u32_t rx3symerr = switch_readstat(0x46);
	u32_t rx3crcerr = switch_readstat(0x47);
	u32_t rx3alignerr = switch_readstat(0x48);
	u32_t rx3broadcast = switch_readstat(0x4B);
	u32_t rx3unicast = switch_readstat(0x4D);
	u32_t tx1broadcast = switch_readstat(0x18);
	u32_t tx1unicast = switch_readstat(0x1A);

	printf(	"-------------\n"
			"rx3under=%d\n"
			"rx3frag=%d\n"
			"rx3over=%d\n"
			"rx3jabber=%d\n"
			"rx3symerr=%d\n"
			"rx3crcerr=%d\n"
			"rx3alignerr=%d\n"
			"rx3broadcast=%d\n"
			"rx3unicast=%d\n"
			"tx1broadcast=%d\n"
			"tx1unicast=%d\n",
			rx3under, rx3frag, rx3over, rx3jabber, rx3symerr, rx3crcerr, rx3alignerr,
			rx3broadcast, rx3unicast, tx1broadcast, tx1unicast);
}

void c674phy_init_vendor()
{
	/* Start Autonegociation on Port 2 */
	c674phy_addr = 2;
	c674phy_write(PHYREG_ANAR, c674phy_anar|PHY_ANAR_SYM_PAUSE|PHY_ANAR_ASYM_PAUSE|PHY_ANAR_IEEE802_3);
	c674phy_write(PHYREG_MODECTL, PHY_MODECTL_AUTONEG_ENABLE|PHY_MODECTL_RESTART_AUTONEG);
	c674phy_addr = 1;

	/* Start Switch */
	u8_t regval;
//	I2C_Reg8Read(I2C_KSZ8893, 1, &regval, 1);
	regval = 1;
	I2C_Reg8Write(I2C_KSZ8893, 1, &regval, 1);

	// Debug
	//switch_printstats();
}

void c674phy_interrupt_callback(void* ctx)
{
	struct netif* netif = (struct netif*) ctx;

	// TODO Read Status in MDIO Registers
	u16_t stat = c674phy_read(PHYREG_MODESTAT);
	c674phy_event = 0;

	if (stat & PHY_MODESTAT_LINK_STATUS)	netif_set_link_up(netif);
	else									netif_set_link_down(netif);
}

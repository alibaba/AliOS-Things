/************************************************************************/
/**	\file	c674phy.h
 *	\brief	LwIP network interface for C674 PHY
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/
#ifndef _C674PHY_H_
#define _C674PHY_H_

/* Includes */
#include "lwip/netif.h"

/* Macros */
#define PHYREG_MODECTL					0x0000	//!< Control
#define PHYREG_MODESTAT					0x0001	//!< Status
#define PHYREG_PHYID1					0x0002	//!< Phy Identification 1
#define PHYREG_PHYID2					0x0003	//!< Phy Identification 2
#define PHYREG_ANAR						0x0004	//!< Auto-Negociation Advertisement Register
#define PHYREG_ANLPAR					0x0005	//!< Auto-Negociation Link Partner Ability Register
#define PHYREG_ANER						0x0006	//!< Auto-Negociation Expansion Register

#define PHY_MODECTL_RESET				0x8000
#define PHY_MODECTL_LOOPBACK			0x4000
#define PHY_MODECTL_SPEED_SELECT		0x2000
#define PHY_MODECTL_AUTONEG_ENABLE		0x1000
#define PHY_MODECTL_POWER_DOWN			0x0800
#define PHY_MODECTL_ISOLATE				0x0400
#define PHY_MODECTL_RESTART_AUTONEG		0x0200
#define PHY_MODECTL_DUPLEX_MODE			0x0100
#define PHY_MODECTL_COLLISION_TEST		0x0080

#define PHY_MODESTAT_100BASE_T4			0x8000
#define PHY_MODESTAT_100BASE_FULL		0x4000
#define PHY_MODESTAT_100BASE_HALF		0x2000
#define PHY_MODESTAT_10BASE_FULL		0x1000
#define PHY_MODESTAT_10BASE_HALF		0x0800
#define PHY_MODESTAT_AUTONEG_COMPLETE	0x0020
#define PHY_MODESTAT_REMOTE_FAULT		0x0010	
#define PHY_MODESTAT_AUTONEG_ABILITY	0x0008
#define PHY_MODESTAT_LINK_STATUS		0x0004
#define PHY_MODESTAT_JABBER_DETECTED	0x0002
#define PHY_MODESTAT_EXT_REGISTERS		0x0001

#define PHY_ANAR_NEXT_PAGE				0x8000	// RO
#define PHY_ANAR_ACK					0x4000	// RO in ANLPAR only
#define PHY_ANAR_REMOTE_FAULT			0x2000
#define PHY_ANAR_SYM_PAUSE				0x0800	// in ANAR only
#define PHY_ANAR_ASYM_PAUSE				0x0400
#define PHY_ANAR_100BASE_T4				0x0200	// RO
#define PHY_ANAR_100BASE_FULL			0x0100
#define PHY_ANAR_100BASE				0x0080
#define PHY_ANAR_10BASE_FULL			0x0040
#define PHY_ANAR_10BASE					0x0020
#define	PHY_ANAR_IEEE802_3				0x0001

/* Types */

/* Externals */
extern u32_t	c674phy_sysclk4;			//!< EMAC Peripheral Clock in Hz
extern u16_t	c674phy_anar;				//!< Auto-Negociation Advertisement, default all-capable
extern u8_t		c674phy_addr;				//!< PHY address, default 1

/* Functions */
#ifdef __cplusplus
extern "C" {
#endif

	u16_t c674phy_read(u16_t regaddr);
	void c674phy_write(u16_t regaddr, u16_t data);
	void c674phy_init(struct netif* netif);
#if NO_SYS
	void c674phy_poll(struct netif* netif);
#endif

	void c674phy_init_vendor(void);
	void c674phy_interrupt_callback(void* ctx);

	u32_t switch_readstat(u8_t regaddr);
	void switch_printstats(void);

#ifdef __cplusplus
}
#endif

/* Include Guard */
#endif

/************************************************************************/
/**	\file	c674phy.c
 *	\brief	LwIP network interface for C674x PHY
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#if !NO_SYS
#include <ti/sysbios/family/c64p/hwi.h>
#endif
#include "c674phy.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include <c6x.h>
#include <ti/pspiom/cslr/cslr_chip.h>
#include <ti/pspiom/cslr/cslr_mdio.h>
#include <ti/pspiom/cslr/cslr_emac1.h>
#include <ti/pspiom/cslr/cslr_ectl1.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>

/* Macros */

/* Types */
typedef volatile CSL_EctlRegs	*CSL_EctlRegsOvly;
typedef volatile CSL_EmacRegs   *CSL_EmacRegsOvly;

/* Externals */

/* Variables */
u32_t	c674phy_sysclk4	= 50000000;
u16_t	c674phy_anar	= PHY_ANAR_100BASE_FULL|PHY_ANAR_100BASE|PHY_ANAR_10BASE_FULL|PHY_ANAR_10BASE;
u8_t	c674phy_addr	= 1;

volatile u8_t c674phy_event = 0;
#if !NO_SYS
struct tcpip_callback_msg* c674phy_msg;
#endif

sys_mutex_t c674phy_mutex;

/* PHY Interrupt */

void c674phy_interrupt(void *arg)
{
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	CSL_MdioRegsOvly MDIO_Regs = (CSL_MdioRegsOvly) CSL_MDIO_0_REGS;

	#if !NO_SYS
	if (!c674phy_event) tcpip_trycallback(c674phy_msg);
	#endif
	c674phy_event = 1;

	/* Confirm handling */
	MDIO_Regs->LINKINTRAW = CSL_FMK(MDIO_LINKINTRAW_USERPHY0, 1);
	EMAC_Regs->MACEOIVECTOR = CSL_EMAC_MACEOIVECTOR_INTVECT_C0MISC;
}

/* Functions */

static void c674phy_wait(void)
{
	CSL_MdioRegsOvly MDIO_Regs = (CSL_MdioRegsOvly) CSL_MDIO_0_REGS;
	while( CSL_FEXT(MDIO_Regs->USERACCESS0, MDIO_USERACCESS0_GO ) );
}


void c674phy_write(u16_t regaddr, u16_t data)
{
	CSL_MdioRegsOvly MDIO_Regs = (CSL_MdioRegsOvly) CSL_MDIO_0_REGS;
	sys_mutex_lock(&c674phy_mutex);

	/* Wait For The MDIO to be Ready */
	c674phy_wait();

	MDIO_Regs->USERACCESS0 = CSL_FMK(MDIO_USERACCESS0_DATA, data)
							|CSL_FMK(MDIO_USERACCESS0_PHYADR, c674phy_addr)
							|CSL_FMK(MDIO_USERACCESS0_REGADR, regaddr)
							|CSL_FMK(MDIO_USERACCESS0_WRITE, 1)
							|CSL_FMK(MDIO_USERACCESS0_GO, 1u);

	c674phy_wait();
	sys_mutex_unlock(&c674phy_mutex);
}

u16_t c674phy_read(u16_t regaddr)
{
	CSL_MdioRegsOvly MDIO_Regs = (CSL_MdioRegsOvly) CSL_MDIO_0_REGS;
	sys_mutex_lock(&c674phy_mutex);

	/* Wait For The MDIO to be Ready */
	c674phy_wait();

	MDIO_Regs->USERACCESS0 = CSL_FMK(MDIO_USERACCESS0_PHYADR, c674phy_addr)
							|CSL_FMK(MDIO_USERACCESS0_REGADR, regaddr)
							|CSL_FMK(MDIO_USERACCESS0_WRITE, 0)
							|CSL_FMK(MDIO_USERACCESS0_GO, 1u);

	c674phy_wait();
	u16_t data = CSL_FEXT(MDIO_Regs->USERACCESS0, MDIO_USERACCESS0_DATA);
	sys_mutex_unlock(&c674phy_mutex);
	return data;
}

void c674phy_init(struct netif *netif)
{
	#if !NO_SYS
	/* Static message for interrupt callback */
	c674phy_msg = tcpip_callbackmsg_new(c674phy_interrupt_callback, netif);

	/* Mutex to protect Management Interface */
	err_t err = sys_mutex_new(&c674phy_mutex);
	#endif

	CSL_MdioRegsOvly MDIO_Regs = (CSL_MdioRegsOvly) CSL_MDIO_0_REGS;
	MDIO_Regs->CONTROL = CSL_FMKT(MDIO_CONTROL_PREAMBLE, ENABLE)
					|  CSL_FMK(MDIO_CONTROL_CLKDIV, c674phy_sysclk4/2000000)
					|  CSL_FMKT(MDIO_CONTROL_ENABLE, YES);

	/* PHY monitoring */
	MDIO_Regs->USERPHYSEL0 = CSL_FMK(MDIO_USERPHYSEL0_PHYADRMON, c674phy_addr)
							|CSL_FMK(MDIO_USERPHYSEL0_LINKSEL, 0)
							|CSL_FMK(MDIO_USERPHYSEL0_LINKINTENB, 1);

	/* Reset PHY */
	c674phy_write(PHYREG_MODECTL, PHY_MODECTL_RESET);
	#if !NO_SYS
	sys_msleep(500);
	#endif
	
	/* Start Autonegociation */
	c674phy_write(PHYREG_ANAR, c674phy_anar|PHY_ANAR_SYM_PAUSE|PHY_ANAR_ASYM_PAUSE|PHY_ANAR_IEEE802_3);
	c674phy_write(PHYREG_MODECTL, PHY_MODECTL_AUTONEG_ENABLE|PHY_MODECTL_RESTART_AUTONEG);

	/* Vendor Specific initializations (interrupt setup) */
	c674phy_init_vendor();

	/* Clear PHY Interrupts */
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	MDIO_Regs->LINKINTRAW = CSL_FMK(MDIO_LINKINTRAW_USERPHY0, 1);
	EMAC_Regs->MACEOIVECTOR = CSL_EMAC_MACEOIVECTOR_INTVECT_C0MISC;
	
	/* Enable PHY Interrupts */
	CSL_EctlRegsOvly EMAC_Ctrl = (CSL_EctlRegsOvly) CSL_ECTL_0_REGS;
	EMAC_Ctrl->C0MISCEN = CSL_FMKT(ECTL_C0MISCEN_LINKINT0EN, ENABLE);
	EMAC_Ctrl->C1MISCEN = 0;

#if NO_SYS
	CSL_DspintcRegsOvly DspInt_Regs = (CSL_DspintcRegsOvly) CSL_INTC_0_REGS;
	CSL_FINS(DspInt_Regs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_EMAC_MISCC0);
	CSL_FINST(IER, CHIP_IER_IE11, ENABLE);
#else
	Hwi_eventMap(11, CSL_INTC_EVENTID_EMAC_MISCC0);
 	Hwi_Handle miscisr = Hwi_create(11, (Hwi_FuncPtr) c674phy_interrupt, NULL, NULL);
#endif
}

#if NO_SYS
void c674phy_poll(struct netif* netif)
{
	if (c674phy_event)		c674phy_interrupt_callback(netif);
}
#endif

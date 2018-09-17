/************************************************************************/
/**	\file	bfphy.c
 *	\brief	LwIP network interface for Blackfin PHY
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include <blackfin.h>
#include <sys/exception.h>
#include <services/services.h>
#include "bfphy.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"

/* Macros */

/* Types */

/* Externals */

/* Variables */
u32_t	bfphy_sclk	= 133333333;
u16_t	bfphy_anar	= PHY_ANAR_100BASE_FULL|PHY_ANAR_100BASE|PHY_ANAR_10BASE_FULL|PHY_ANAR_10BASE;
u8_t	bfphy_addr	= 1;

volatile u8_t bfphy_event = 0;
#if !NO_SYS
struct tcpip_callback_msg* bfphy_msg;
#endif

sys_mutex_t bfphy_mutex;

/* PHY Interrupt */

static ADI_INT_HANDLER(bfphy_interrupt) 
{
	ADI_INT_HANDLER_RESULT result = ADI_INT_RESULT_NOT_PROCESSED;
	int isr = *pEMAC_SYSTAT;
	if (isr & PHYINT)
	{
		// Confirm handling
		*pEMAC_SYSTAT = PHYINT;
		result = ADI_INT_RESULT_PROCESSED;

		#if !NO_SYS
		if (!bfphy_event)	tcpip_trycallback(bfphy_msg);
		#endif
		bfphy_event = 1;
	}
	return result;
}

/* Functions */

static void bfphy_wait(void)
{
	while ((*pEMAC_STAADD) & STABUSY) {};
}

void bfphy_write(u16_t regaddr, u16_t data)
{
	sys_mutex_lock(&bfphy_mutex);
	*pEMAC_STADAT = data;
	*pEMAC_STAADD = SET_PHYAD(bfphy_addr) | SET_REGAD(regaddr) | STAOP | STABUSY;	// | STAIE ;
	bfphy_wait();
	sys_mutex_unlock(&bfphy_mutex);
}

u16_t bfphy_read(u16_t regaddr)
{
	sys_mutex_lock(&bfphy_mutex);
	*pEMAC_STAADD = SET_PHYAD(bfphy_addr) | SET_REGAD(regaddr) | STABUSY;	// | STAIE ;
	bfphy_wait();
	u16_t data = *pEMAC_STADAT;
	sys_mutex_unlock(&bfphy_mutex);
	return data;
}

void bfphy_init(struct netif *netif)
{
	#if !NO_SYS
	// Static message for interrupt callback
	bfphy_msg = tcpip_callbackmsg_new(bfphy_interrupt_callback, netif);

	// Mutex to protect Management Interface
	err_t err = sys_mutex_new(&bfphy_mutex);
	#endif

	// Management Clock
	int n = (bfphy_sclk/2+2499999) / 2500000;
	*pEMAC_SYSCTL = SET_MDCDIV(n-1);

	// Reset
	bfphy_write(PHYREG_MODECTL, PHY_MODECTL_RESET);
	#if !NO_SYS
	sys_msleep(500);
	#endif
	
	// Start Autonegociation
	bfphy_write(PHYREG_ANAR, bfphy_anar|PHY_ANAR_SYM_PAUSE|PHY_ANAR_ASYM_PAUSE|PHY_ANAR_IEEE802_3);
	bfphy_write(PHYREG_MODECTL, PHY_MODECTL_AUTONEG_ENABLE|PHY_MODECTL_RESTART_AUTONEG);

	// Vendor Specific initializations (interrupt setup)
	bfphy_init_vendor();

	// PHY Interrupt
	#ifdef _DEF_BF52X_H
	*pSIC_IMASK0 |= IRQ_MAC_ERR;
	#else
	*pSIC_IMASK |= IRQ_ERROR2;
	#endif
	ADI_INT_RESULT res = adi_int_CECHook(7, bfphy_interrupt, NULL, true);

	u32_t sysctl = *pEMAC_SYSCTL;
	sysctl |= PHYIE;
	*pEMAC_SYSCTL = sysctl;
}

#if NO_SYS
void bfphy_poll(struct netif* netif)
{
	if (bfphy_event)	bfphy_interrupt_callback(netif);
}
#endif

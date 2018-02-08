#include <platform_opts.h>

#if CONFIG_EXAMPLE_PPPOE
#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <lwip_netconf.h>
#include <../netif/ppp/ppp_impl.h>
#include <netif/ppp_oe.h>

#define PPPOE_USER      "user"
#define PPPOE_PASSWD    "passwd"

extern struct netif xnetif[];

void pppLinkStatusCallback(void *ctx, int errCode, void *arg)
{
	switch(errCode) {
		case PPPERR_NONE: {
			struct ppp_addrs *ppp_addrs = arg;
			printf("pppLinkStatusCallback: PPPERR_NONE\n");
			printf(" our_ipaddr=%s\n", ip_ntoa(&ppp_addrs->our_ipaddr));
			printf(" his_ipaddr=%s\n", ip_ntoa(&ppp_addrs->his_ipaddr));
			printf(" netmask   =%s\n", ip_ntoa(&ppp_addrs->netmask));
			printf(" dns1      =%s\n", ip_ntoa(&ppp_addrs->dns1));
			printf(" dns2      =%s\n", ip_ntoa(&ppp_addrs->dns2));
			dns_setserver(0, &ppp_addrs->dns1);
			dns_setserver(1, &ppp_addrs->dns2);
			break;
		}
		default:
			printf("pppLinkStatusCallback: errCode(%d)\n", errCode);
			break;
	}
}

static void example_pppoe_thread(void *param)
{
	int ppp_desc = -1;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: PPPOE\n");

	xnetif[0].flags &= ~(NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP); /* no ARP */
	xnetif[0].flags |= NETIF_FLAG_ETHERNET;                    /* pure ethernet */

	pppInit();
	pppSetAuth(PPPAUTHTYPE_ANY, PPPOE_USER, PPPOE_PASSWD);
	ppp_desc = pppOverEthernetOpen(&xnetif[0], NULL, NULL, pppLinkStatusCallback, NULL);

	if(ppp_desc >= 0) {
		while(1) {
			int if_up = 0;
			pppIOCtl(ppp_desc, PPPCTLG_UPSTATUS, &if_up);
			if(if_up)
				break;
			else
				vTaskDelay(1000);
		}

		printf("Close PPPOE after 10 seconds\n");
		vTaskDelay(10000);
		pppClose(ppp_desc);
	}

exit:
	vTaskDelete(NULL);
}

void example_pppoe(void)
{
	if(xTaskCreate(example_pppoe_thread, ((const char*)"example_pppoe_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#endif /* CONFIG_EXAMPLE_PPPOE */
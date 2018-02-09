#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>

#include <mDNS/mDNS.h>

#include <lwip_netconf.h>
#include <lwip/netif.h>
extern struct netif xnetif[];

static void example_mdns_thread(void *param)
{
	DNSServiceRef dnsServiceRef = NULL;
	TXTRecordRef txtRecord;
	unsigned char txt_buf[100];	// use fixed buffer for text record to prevent malloc/free

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);

	printf("\nmDNS Init\n");
	if(mDNSResponderInit() == 0) {
		printf("mDNS Register service\n");
		TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);
		TXTRecordSetValue(&txtRecord, "text1", strlen("text1_content"), "text1_content");
		TXTRecordSetValue(&txtRecord, "text2", strlen("text2_content"), "text2_content");
		dnsServiceRef = mDNSRegisterService("ameba", "_service1._tcp", "local", 5000, &txtRecord);
		TXTRecordDeallocate(&txtRecord);
		printf("wait for 30s ... \n");
		vTaskDelay(30*1000);
		
		printf("mDNS Update service\n");
		TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);
		TXTRecordSetValue(&txtRecord, "text1", strlen("text1_content_new"), "text1_content_new");
		mDNSUpdateService(dnsServiceRef, &txtRecord, 0);
		TXTRecordDeallocate(&txtRecord);
		printf("wait for 30s ... \n");
		vTaskDelay(30*1000);

		if(dnsServiceRef)
			mDNSDeregisterService(dnsServiceRef);

		// deregister service before mdns deinit is not necessary
		// mDNS deinit will also deregister all services
		printf("mDNS Deinit\n");
		mDNSResponderDeinit();
	}

	vTaskDelete(NULL);
}

void example_mdns(void)
{
	if(xTaskCreate(example_mdns_thread, ((const char*)"example_mdns_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#include "FreeRTOS.h"
#include "task.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "google/google_nest.h"
#include "flash_api.h"
#include "wigadget.h"
#include <lwip/netif.h>
#include "shtc1.h"

#define CLOUD_PORT		443
extern struct netif xnetif[NET_IF_NUM];

void cloud_link_task(void *param){
	googlenest_context googlenest;
	unsigned char URI[64];
	unsigned char data[97] = {0};
	unsigned char host_addr[64] = {0};	
	flash_t cloud_flash;        
	u8 *mac = (u8 *)LwIP_GetMAC(&xnetif[0]);
        char i[16], j[16];
	float temperature = 1.123f;
	float humidity = 2.456f;
	int ret = 0;

	vTaskDelay(2000);

	
	sprintf(URI, "ht_sensor/%02x%02x%02x%02x%02x%02x.json", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	memset(host_addr, 0, sizeof(host_addr));
	if(flash_stream_read(&cloud_flash, FLASH_IOT_DATA, 97, (uint8_t *) data) == 1){

		memset(host_addr, 0 , 64);
		memcpy(host_addr, data+33, 64);
		while(1) {
			printf("\r\n\r\n\r\n\r\n\r\n\r\n=====>START CLOUD LINKING\r\n\r\n");
			memset(i, 0, 16);
		        memset(j, 0, 16);
#if PSEUDO_DATA
			sprintf(i,"%.2f", temperature++);
			sprintf(j, "%.2f", humidity++);
			if(temperature > 60)
				temperature = 1.123f;
			if(humidity > 98)
				humidity = 2.456f;                        
#else
			ret = SHTC_GetTempAndHumi(&temperature, &humidity);
			sprintf(i, "%.2f", temperature);
			sprintf(j, "%.2f", humidity);
#endif
			if(ret < 0)
				printf("\r\n\r\n<-----LOCAL LINK FAILED!!(get infor failed)\r\n\r\n");
			else{
				gen_json_data(i, j, data);
	            printf("\r\nCLOUD-LINK--Sending data : \r\n%s\r\n", data);
				memset(&googlenest, 0, sizeof(googlenest_context));
				if(gn_connect(&googlenest, host_addr, CLOUD_PORT) == 0) {
					if(gn_put(&googlenest, URI, data) != 0)
						printf("\r\n\r\nPUT data failed!\r\n\r\n");
					gn_close(&googlenest);
					printf("\r\n\r\n<=====CLOUD LINK OK!!\r\n\r\n");
				}
				else{
					printf("\r\n\r\n<=====CLOUD LINK FAILED!!(google nest connecting)\r\n\r\n");
				}
				free(data);
				vTaskDelay(10000);
			}
		}
			
	}
	else 
		printf("\r\n\r\n<=====CLOUD LINK FAILED!!(flash reading)\r\n\r\n");

}

void start_cloud_link(void)
{
if(xTaskCreate(cloud_link_task, ((const char*)"cloud_link_task"), 3584, NULL, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
	printf("\n\r%s xTaskCreate failed", __FUNCTION__);
}
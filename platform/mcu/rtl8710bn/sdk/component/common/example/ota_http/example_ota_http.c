#include <ota_8195a.h>

#define PORT	8082
#define HOST	"192.168.1.53"  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
#define RESOURCE ""     //"051103061600.bin"


#ifdef HTTP_OTA_UPDATE
void http_update_ota_task(void *param){
	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 1 minute to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	vTaskDelay(60*1000);
	int ret = -1;
	
	ret = http_update_ota(HOST, PORT, RESOURCE);

exit:	
	printf("\n\r[%s] Update task exit", __FUNCTION__);
	if(!ret){
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);	
		ota_platform_reset();
	}
	vTaskDelete(NULL);	
}


void example_ota_http(void){
	if(xTaskCreate(http_update_ota_task, (char const *)"http_update_ota_task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS){
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}
#endif


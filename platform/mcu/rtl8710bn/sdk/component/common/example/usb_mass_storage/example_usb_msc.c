#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "platform_opts.h"

#include "usb.h"
#include "msc/inc/usbd_msc_config.h"
#include "msc/inc/usbd_msc.h"

void example_mass_storage_thread(void* param){
	int status = 0;

	_usb_init();
	status = wait_usb_ready();
	if(status != USB_INIT_OK){
		if(status == USB_NOT_ATTACHED)
			printf("\r\n NO USB device attached\n");
		else
			printf("\r\n USB init fail\n");
		goto exit;
	}

	// load usb mass storage driver
	status = usbd_msc_init(MSC_NBR_BUFHD, MSC_BUFLEN, DISK_SDCARD);

	if(status){
		printf("USB MSC driver load fail.\n");
	}else
		printf("USB MSC driver load done, Available heap [0x%x]\n", xPortGetFreeHeapSize());
exit:
	vTaskDelete(NULL);
}


void example_mass_storage(void)
{
	if(xTaskCreate(example_mass_storage_thread, ((const char*)"example_fatfs_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_fatfs_thread) failed", __FUNCTION__);
}


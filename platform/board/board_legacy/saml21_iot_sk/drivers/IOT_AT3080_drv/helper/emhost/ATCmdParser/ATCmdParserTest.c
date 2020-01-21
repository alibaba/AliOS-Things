
#include <stdio.h>

#include "mx_hal.h"
#include "emh_api.h"

char* return_str;

void ATCmdParser_test(void)
{
	mx_status ret = kNoErr;
	
	mx_hal_ms_ticker_init();
	
	ret = emh_init();
	if (ret == kNoErr) printf("module init success\r\n");
	
	printf("FW version: %s\r\n", emh_module_get_fw_version());
	printf("System tick: %d\r\n", (int)emh_module_get_tick());

	while(1);
}

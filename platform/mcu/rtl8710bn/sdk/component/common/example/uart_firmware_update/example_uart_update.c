#include "FreeRTOS.h"
#include "task.h"
#include "example_uart_update.h"
#include <platform/platform_stdlib.h>

#define XMODEM_UART_IDX 0
#define XMODEM_UART_MUX 2
#define XMODEM_UART_BAUDRATE 115200

extern void OTU_FW_Update(u8, u8, u32);

int is_update_image_enable(gpio_t *gpio_uart_update_eable)
{

	GPIO_InitTypeDef  GPIO_Pin;
    u32 active_state;
//	gpio_t gpio_uart_update_eable;
	int ret = 0;

	gpio_init(gpio_uart_update_eable, PIN_NAME);
	gpio_dir(gpio_uart_update_eable, PIN_INPUT);     // Direction: Input
	gpio_mode(gpio_uart_update_eable, PullUp);       // Pull-High
//	ret = gpio_read(&gpio_uart_update_eable);

	return ret;
}
void example_uart_update_thread(void *param)
{
	int count = 0;
	gpio_t gpio_uart_update_eable;

	is_update_image_enable(&gpio_uart_update_eable);
	//polling MAX_WAIT_TIME*50ms 
	while(count <= MAX_WAIT_TIME)
	{
		printf("waitting update enable\r\n");
		if(gpio_read(&gpio_uart_update_eable) == 0){
			printf("update image enabled with xmodem protocol!\r\n");
			//uart_ymodem();
			OTU_FW_Update(XMODEM_UART_IDX, XMODEM_UART_MUX, XMODEM_UART_BAUDRATE);
			break;
		}
		else{
			//RtlMsleepOS(50);		
			rtw_msleep_os(50);
			count++;
		}		
	}
	
	vTaskDelete(NULL);
}

void example_uart_update(void)
{
	if(xTaskCreate(example_uart_update_thread, ((const char*)"example_uart_update_thread"), 512, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}



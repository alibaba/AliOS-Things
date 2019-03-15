#include "stm32l4xx_hal.h"
#include "hal_boot_process.h"

typedef void (*p_func)(void);
void hal_uart_send_string( char *str);
unsigned int hal_get_boot_bank(void)
{
	uint32_t memrmp = 0;
	uint32_t current_bank = FLASH_BANK_1;
	memrmp = READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE);
	current_bank = memrmp ? FLASH_BANK_2 : FLASH_BANK_1;
	return current_bank;
}

void sw_bank(void)
{
	FLASH_OBProgramInitTypeDef OBInit;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	HAL_FLASH_OB_Unlock();
	HAL_FLASHEx_OBGetConfig(&OBInit);
	OBInit.OptionType = OPTIONBYTE_USER;
	OBInit.USERType   = OB_USER_BFB2;
	if (((OBInit.USERConfig) & (OB_BFB2_ENABLE)) == OB_BFB2_ENABLE) {
		OBInit.USERConfig = OB_BFB2_DISABLE;
	}
	else {
		OBInit.USERConfig = OB_BFB2_ENABLE;
	}
	if(HAL_FLASHEx_OBProgram (&OBInit) != HAL_OK) {
		hal_uart_send_string("HAL_FLASHEx_OBProgram failed\r\n");
	}
	if (HAL_FLASH_OB_Launch() != HAL_OK) {
	    while (1) {
                hal_uart_send_string("HAL_FLASH_OB_Launch failed\r\n");
	    }
	}
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
}

void load_app(unsigned int app_addr)
{
	unsigned int jump_addr = 0;
	p_func app_func = NULL;

	SysTick->CTRL = 0;
	jump_addr = *(__IO uint32_t*)(app_addr + 4);
	app_func = (p_func)jump_addr;
	__set_MSP(*(__IO uint32_t*)app_addr);
	app_func();
}


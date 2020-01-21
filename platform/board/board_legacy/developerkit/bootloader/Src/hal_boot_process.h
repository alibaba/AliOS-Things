#ifndef __HAL_BOOT_PROCESS_H__
#define __HAL_BOOT_PROCESS_H__

#define APP_LOAD_ADDR                   0x08009000
unsigned int hal_get_boot_bank(void);
void sw_bank(void);
#endif

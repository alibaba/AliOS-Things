#include <xm580_reg.h>
#include <common.h>
#include <uart.h>
#include <flash.h>
#include <gic.h>
#include <cmd.h>
#include <nand_flash.h>
#include <mmc.h>
#include <time.h>

/* 定义变量  */

volatile uint8 g_bootmode_enable = CMD_BOOTMODE_DISABLE;

volatile uint32 g_uart_send_timeout = UART_SEND_TIMEOUT;
volatile uint32 g_uart_recv_timeout = UART_RECV_TIMEOUT;
volatile uint32 g_flash_operate_timeout = FLASH_OP_TIMEOUT;

volatile uint8 g_uart_baudrate = E_BAUDRATE_9600;
volatile uint8 g_uart_parity = E_PARITY_NONE;

//根据不同型号的FLASH芯片 区域插除命令可能不同视具体芯片手册而定
volatile uint32 g_ulFLASH_EARSE_CMD = FL_4K_SECTOR_EARSE;
volatile uint32 g_ulFLASH_EARSE_SIZE = FL_4K_SECTOR_SIZE;

volatile uint32 g_nandflash_page_num = 64;
volatile uint32 g_nandflash_page_size = 2048;




void para_init(void)
{
	g_uart_baudrate = E_BAUDRATE_115200;
	g_uart_parity = E_PARITY_NONE;                                                                   

	g_bootmode_enable = CMD_BOOTMODE_DISABLE;       

	g_uart_send_timeout = UART_SEND_TIMEOUT;                                       
	g_uart_recv_timeout = UART_RECV_TIMEOUT;                                       
	g_flash_operate_timeout = FLASH_OP_TIMEOUT;

	g_ulFLASH_EARSE_CMD = FL_4K_SECTOR_EARSE; // flash erase para                  
	g_ulFLASH_EARSE_SIZE = FL_4K_SECTOR_SIZE;                                      
}


int main()
{

	unsigned int bootset;
	void (*entry_point)(void) = NULL;

	para_init();
	uart0_init();
	gic_init();
	time_init();

	bootset = readl(BOOT_SET);	

	switch(bootset)
	{
		case 0x00:
			break;
		case 0x01:
			entry_point = (void (*)(void))FLASH_START_ADDR;
			(*entry_point)();
			break;
		case 0x02:
			putstr("NAND ");
			nand_flash_init();
			break;
		case 0x03:
			nand_flash_init();
			nand_flash_reset(g_flash_operate_timeout);
			nand_flash_configs(g_flash_operate_timeout);
			nand_flash_skip_bad_block(g_flash_operate_timeout);
			entry_point = (void (*)(void))NAND_FLASH_START_ADDR;
			(*entry_point)();
			break;
		case 0x04:
			putstr("EMMC ");
			break;
		case 0x05:
			mmc_initialize();
			mmc_bread(0, SECTOR, IMAGE_SIZE_SECTORS, (void *)IMAGE_LOAD_ADDR);
			entry_point = (void (*)(void))(IMAGE_LOAD_ADDR);
			(*entry_point)();
		default:
			break;
	}

	putstr("ROM START!\n");
	
	while(1)
	{
		cmd_process();
	}

	return 0;
}

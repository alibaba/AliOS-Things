#include <stdint.h>
#include "flash.h"
#include "user_cfg.h"
#include "osal.h"
#include <string.h>
#include <stdio.h>
#include "crc.h"
//#include "atcmd.h"
#include "error.h"

#define USER_CFG_ASSERT(cmp) \
    do { \
    } while(!(cmp))

const struct user_cfg g_user_cfg __attribute__((section(".user_data")));




void cfg_dump_user_pre_table() {
    int i;
    volatile uint8_t *ptr = (volatile uint8_t *)&g_user_cfg;
    ptr -= 256;
    for (i = 0; i < 256; i++) {
        if ((i!=0) && (i%36 == 0)) {
            printf("\n");
        }
        printf("%02X-", ptr[i]);
    }
    printf("\n");
}
void cfg_dump_user_post_table() {
    extern uint8_t __lds_user_data_end;
    int i;
    volatile uint8_t *ptr = (volatile uint8_t *)&__lds_user_data_end;
    for (i = 0; i < 256; i++) {
        if ((i!=0) && (i%36 == 0)) {
            printf("\n");
        }
        printf("%02X-", ptr[i]);
    }
    printf("\n");
}
void cfg_dump_user_table(int len) {
    int i;
    volatile uint8_t *ptr = (volatile uint8_t *)&g_user_cfg;
    for (i = 0; i < len; i++) {
        if ((i!=0) && (i%36 == 0)) {
            printf("\n");
        }
        printf("%02X-", ptr[i]);
    }
    printf("\n");
}

void cfg_user_write_cfg(uint8_t *new_user_data, uint16_t len) {
    OS_DeclareCritical();
	USER_CFG_ASSERT(len <= (FLASH_SECTOR_SIZE<<1));
    flash_init();
    uint8_t *buf = OS_MemAlloc(FLASH_SECTOR_SIZE<<1);
    memcpy(&(buf[0]), (uint8_t*)new_user_data, len);
    memcpy(&(buf[len]), (((uint8_t *)&g_user_cfg)+len), (FLASH_SECTOR_SIZE<<1)-len);
    uint32_t ptr = (uint32_t)((uint32_t)(&g_user_cfg) & (0xFFFFFF));
    OS_EnterCritical();
    flash_sector_erase((unsigned int)ptr);
    int i;
    
    for (i = 0; i < (FLASH_SECTOR_SIZE<<1); i+=FLASH_PAGE_SIZE) {
        flash_page_program(ptr+i, FLASH_PAGE_SIZE, &(buf[i]));
    }
    OS_ExitCritical();
    OS_MemFree(buf);
}

void cfg_user_read_cfg(uint8_t *flash_user_data, uint16_t len) {
    OS_DeclareCritical();
   	USER_CFG_ASSERT(len <= FLASH_SECTOR_SIZE<<1);
    flash_init();

	uint8_t *buf = OS_MemZalloc(FLASH_SECTOR_SIZE<<1);

//    uint32_t ptr = (uint32_t)((uint32_t)(&g_user_cfg) & (0xFFFFFF));
    OS_EnterCritical();
//    int i;
    
//    for (i = 0; i < FLASH_SECTOR_SIZE<<1; i+=FLASH_PAGE_SIZE) {
	//	flash_fast_read(ptr+i, FLASH_PAGE_SIZE, &(buf[i]));
//    }
	memcpy(buf, (void *)&g_user_cfg, len);

    OS_ExitCritical();
	memcpy(flash_user_data, buf, len);

    OS_MemFree(buf);
	buf = NULL;
}




#if defined(SUPPORT_UASCENT_USER_CFG)
u8 *user_cfg_buf = NULL;
u8 *user_read_buf = NULL;

u8 user_cfg_flag = 0;

extern uint16_t crc16_ccitt_cal(uint16_t crc_start, unsigned char *buf, int len);
int do_At_User_Flsh(u8 action, u8 write_offset ,u8 *write_buffer)
{
	int loop;
	uint16_t write_crc,read_crc;


	if( action == 1 )//write
	{


		if( user_cfg_flag == 0 )
		{
			user_cfg_buf = OS_MemAlloc(RW_TOTAL_SIZE);// 2k size
			if(user_cfg_buf == NULL)
			{
				printf("no memory\n");
				return ERROR_MEMORY_FAILED;
			}
		}
		SET_CFG(user_cfg_flag,write_offset);
		
		memcpy(user_cfg_buf + RW_BLOCK_SIZE*write_offset , write_buffer ,RW_BLOCK_SIZE);

		if(CHECK_CFG_OK(user_cfg_flag) )
		{
			write_crc = crc16_ccitt_cal(0, user_cfg_buf, RW_TOTAL_SIZE);
			cfg_user_write_cfg(user_cfg_buf,RW_TOTAL_SIZE);

			memset(user_cfg_buf,0,RW_TOTAL_SIZE);
			cfg_user_read_cfg(user_cfg_buf,RW_TOTAL_SIZE);//read 2K
			read_crc = crc16_ccitt_cal(0, user_cfg_buf, RW_TOTAL_SIZE);
			if(read_crc != write_crc)
			{
				printf("Flash crc check Fail\n");
				printf("write_crc=%d  read_crc=%d\n",write_crc,read_crc);
				return ERROR_NOT_IMPLEMENT;
			}
			printf("Write Flash and check crc OK %d\n",read_crc); 
			CLEAR_CFG(user_cfg_flag);
			OS_MemFree(user_cfg_buf);
			user_cfg_buf = NULL;
		}
	}
	else if( action == 0)//read
	{
		user_read_buf = OS_MemAlloc(RW_TOTAL_SIZE);// 2k size
		if(user_read_buf == NULL)
				return ERROR_MEMORY_FAILED;
		memset(user_read_buf,0,RW_TOTAL_SIZE);
		cfg_user_read_cfg(user_read_buf,RW_TOTAL_SIZE);//read 2K
		printf("read crc=%d\n",crc16_ccitt_cal(0, user_read_buf, RW_TOTAL_SIZE));
		printf("##########################################\n");
		for(loop=0; loop<RW_TOTAL_SIZE ; loop++)
			printf("%d\t",user_read_buf[loop]);
		printf("\n##########################################\n");
		OS_MemFree(user_read_buf);
	}
	return ERROR_SUCCESS;
}
#endif


#include "esp_common.h"
#include "lwip/mem.h"
#include "upgrade.h"
#include "aos/kernel.h"
#include "k_api.h"
#include "aos/log.h"
#define taskENTER_CRITICAL()  RHINO_CRITICAL_ENTER()
#define taskEXIT_CRITICAL()    RHINO_CRITICAL_EXIT()

#define TAG "esp8266_ota"
#define BUFSIZE     512
#define CRC_BLOCK_SIZE 512
uint16 start_sec;
static unsigned int *crc_table;

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

static int init_crc_table(void);
static unsigned int crc32(unsigned int crc, unsigned char * buffer, unsigned int size);

struct upgrade_param {
    uint32 fw_bin_addr;
    uint16 fw_bin_sec;
    uint16 fw_bin_sec_num;
    uint16 fw_bin_sec_earse;
    uint8 extra;
    uint8 save[4];
    uint8 *buffer;
};

LOCAL struct upgrade_param *upgrade;

//extern SpiFlashChip *flashchip;

LOCAL bool OUT_OF_RANGE(uint16 erase_sec)
{
    uint8 spi_size_map = system_get_flash_size_map();
    uint16 sec_num = 0;
    uint16 start_sec = 0;
    

    if (spi_size_map == FLASH_SIZE_8M_MAP_512_512 || 
            spi_size_map ==FLASH_SIZE_16M_MAP_512_512 ||
            spi_size_map ==FLASH_SIZE_32M_MAP_512_512){
            start_sec = (system_upgrade_userbin_check() == USER_BIN2)? 1:129;
            sec_num = 123;
    } else if(spi_size_map == FLASH_SIZE_16M_MAP_1024_1024 || 
            spi_size_map == FLASH_SIZE_32M_MAP_1024_1024){
            start_sec = (system_upgrade_userbin_check() == USER_BIN2)? 1:257;
            sec_num = 251;
    } else {
            start_sec = (system_upgrade_userbin_check() == USER_BIN2)? 1:65;
            sec_num = 59;
    }
    if((erase_sec >= start_sec) &&(erase_sec <= (start_sec + sec_num)))
    {
        return false;
    } else {
        return true;
    }
    
}


 void system_upgrade_erase(){

    uint16 secnm=0;
        upgrade->fw_bin_sec_earse=upgrade->fw_bin_sec;

        secnm=upgrade->fw_bin_sec+ upgrade->fw_bin_sec_num; //((upgrade->fw_bin_addr + APP_LEN_MAX)>>12) + (APP_LEN_MAX&0xfff?1:0);

         CPSR_ALLOC();
        while(upgrade->fw_bin_sec_earse < secnm) {
            taskENTER_CRITICAL();
            if( OUT_OF_RANGE( upgrade->fw_bin_sec_earse) )
            {
                LOGE(TAG,"~~~~~fw_bin_sec_earse:%d, Out of range\n",upgrade->fw_bin_sec_earse);
                break;
            
            }
            else
            {
                 spi_flash_erase_sector(upgrade->fw_bin_sec_earse);
                 upgrade->fw_bin_sec_earse++;
            }
            taskEXIT_CRITICAL();
            aos_msleep(10);
           // vTaskDelay(10 / portTICK_RATE_MS);
        }
        LOGE(TAG,"flash erase over\n");
        return true;
}
/******************************************************************************
 * FunctionName : user_upgrade_internal
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
LOCAL bool  
system_upgrade_internal(struct upgrade_param *upgrade, uint8 *data, u32 len)
{
    bool ret = false;
    uint16 secnm=0;
    if(data == NULL || len == 0)
    {
        return true;
    }

    
    upgrade->buffer = (uint8 *)zalloc(len + upgrade->extra);

    memcpy(upgrade->buffer, upgrade->save, upgrade->extra);
    memcpy(upgrade->buffer + upgrade->extra, data, len);

    len += upgrade->extra;
    upgrade->extra = len & 0x03;
    len -= upgrade->extra;

    if(upgrade->extra<=4)
        memcpy(upgrade->save, upgrade->buffer + len, upgrade->extra);
    else
        os_printf("ERR3:arr_overflow,%u,%d\n",__LINE__,upgrade->extra);

    do {
        if (upgrade->fw_bin_addr + len >= (upgrade->fw_bin_sec + upgrade->fw_bin_sec_num) * SPI_FLASH_SEC_SIZE) {
            printf("spi_flash_write exceed\n");
            break;
        }

        if (spi_flash_write(upgrade->fw_bin_addr, (uint32 *)upgrade->buffer, len) != SPI_FLASH_RESULT_OK) {
            printf("spi_flash_write failed\n");
            break;
        }
        
        ret = true;
        upgrade->fw_bin_addr += len;
    } while (0);

    free(upgrade->buffer);
    upgrade->buffer = NULL;
    return ret;
}

/******************************************************************************
 * FunctionName : system_get_fw_start_sec
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
uint16 system_get_fw_start_sec()
{
    if(upgrade != NULL) {
        return upgrade->fw_bin_sec;
    } else {
        return 0;
    }
}

/******************************************************************************
 * FunctionName : user_upgrade
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
bool system_upgrade(uint8 *data, uint32 len)
{
    bool ret;
    ret = system_upgrade_internal(upgrade, data, len);
    return ret;
}

/******************************************************************************
 * FunctionName : system_upgrade_init
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
void  
system_upgrade_init(void)
{
    uint32 user_bin2_start,user_bin1_start;
    uint8 spi_size_map = system_get_flash_size_map();
    
    if (upgrade == NULL) {
        upgrade = (struct upgrade_param *)zalloc(sizeof(struct upgrade_param));
    }
    
    user_bin1_start = 1; 
    LOG("~~~~~~~~~~~~~~~spi_size_map=%d~~~~~~~~~~~~~~~~~~~",spi_size_map);
    if (spi_size_map == FLASH_SIZE_8M_MAP_512_512 || 
            spi_size_map ==FLASH_SIZE_16M_MAP_512_512 ||
            spi_size_map ==FLASH_SIZE_32M_MAP_512_512){
            user_bin2_start = 129;
            upgrade->fw_bin_sec_num = 123;
    } else if(spi_size_map == FLASH_SIZE_16M_MAP_1024_1024 || 
            spi_size_map == FLASH_SIZE_32M_MAP_1024_1024){
            user_bin2_start = 257;
            upgrade->fw_bin_sec_num = 251;
    } else {
            user_bin2_start = 65;
            upgrade->fw_bin_sec_num = 59;
    }
   
    upgrade->fw_bin_sec = (system_upgrade_userbin_check() == USER_BIN1) ? user_bin2_start : user_bin1_start;

    upgrade->fw_bin_addr = upgrade->fw_bin_sec * SPI_FLASH_SEC_SIZE;
    
    upgrade->fw_bin_sec_earse = upgrade->fw_bin_sec;
}

/******************************************************************************
 * FunctionName : system_upgrade_deinit
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
void  
system_upgrade_deinit(void)
{
    if (upgrade != NULL) {
        free(upgrade);
        upgrade = NULL;
    }else {
        return;
    }
}




static int ICACHE_FLASH_ATTR
init_crc_table(void)
{
	unsigned int c;
	unsigned int i, j;

	crc_table = (unsigned int*)zalloc(256 * 4);
	if(crc_table == NULL){
		os_printf("malloc crc table failed\n");
		return -1;
	}
	for (i = 0; i < 256; i++) {
		c = (unsigned int)i;
		for (j = 0; j < 8; j++) {
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[i] = c;
	}
	return 0;
}

static unsigned int ICACHE_FLASH_ATTR
crc32(unsigned int crc,unsigned char *buffer, unsigned int size)
{
	unsigned int i;
	for (i = 0; i < size; i++) {
		crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
	}
	return crc ;
}


static int ICACHE_FLASH_ATTR
calc_img_crc(unsigned int sumlength,unsigned int *img_crc)
{
	int fd;
	int ret;
	int i = 0;
	uint8 error = 0;
	unsigned char *buf = (char *)zalloc(BUFSIZE);
	if(buf == NULL){
		os_printf("malloc crc buf failed\n");
		free(crc_table);
		return -1;
    }

	unsigned int crc = 0xffffffff; 

	uint16 sec_block = sumlength / CRC_BLOCK_SIZE ;
	uint32 sec_last = sumlength % CRC_BLOCK_SIZE;
	for (i = 0; i < sec_block; i++) {		
		if ( 0 != (error = spi_flash_read(start_sec * SPI_FLASH_SEC_SIZE + i * CRC_BLOCK_SIZE ,(uint32 *)buf, BUFSIZE))){
				free(crc_table);
				free(buf);
				os_printf("spi_flash_read error %d\n",error);
				return -1;
		}
		crc = crc32(crc, buf, BUFSIZE);		
	}
	if(sec_last > 0 ) {
		if (0 != (error = spi_flash_read(start_sec * SPI_FLASH_SEC_SIZE + i * CRC_BLOCK_SIZE, (uint32 *)buf, sec_last))){
			free(crc_table);
			free(buf);
			os_printf("spi_flash_read error %d\n",error);
			return -1;
		}
		crc = crc32(crc, buf, sec_last);
	}
	*img_crc = crc;
	free(crc_table);
	free(buf);
	return 0;
}

bool ICACHE_FLASH_ATTR
upgrade_crc_check(uint16 fw_bin_sec ,unsigned int sumlength)
{
	printf("fw_bin_sec %d sumlength %d\n",fw_bin_sec,sumlength);
	int ret;
	unsigned int img_crc;
	unsigned int flash_crc = 0xFF;
	start_sec = fw_bin_sec;
	if ( 0 != init_crc_table()) {
		return false;
	}
	ret = calc_img_crc(sumlength - 4,&img_crc);
	if (ret < 0) {
		return false;
	}
	img_crc = abs(img_crc);
	os_printf("img_crc = %u\n",img_crc);
	spi_flash_read(start_sec * SPI_FLASH_SEC_SIZE + sumlength - 4,&flash_crc, 4);
    os_printf("flash_crc = %u\n",flash_crc);
	if(img_crc == flash_crc) {
	    return true;
	} else {
		return false;
	}
}

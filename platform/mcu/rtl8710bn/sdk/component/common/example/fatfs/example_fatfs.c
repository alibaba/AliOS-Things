#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "platform_opts.h"
#include "section_config.h"

#if CONFIG_FATFS_EN
#include "ff.h"
#include <fatfs_ext/inc/ff_driver.h>
#if FATFS_DISK_USB
#include "usb.h"
#include <disk_if/inc/usbdisk.h>
#elif FATFS_DISK_SD
#include "sdio_host.h"
#include <disk_if/inc/sdcard.h>
#endif

#define TEST_SIZE	(512)

u8 test_info[]="\"Ameba memory card test information: Hello world.\"";

SRAM_BF_DATA_SECTION u8 WRBuf[TEST_SIZE];
SRAM_BF_DATA_SECTION u8 RDBuf[TEST_SIZE];

void example_fatfs_thread(void* param){
	int drv_num = 0;
	int Fatfs_ok = 0;

	FRESULT res; 
	FATFS 	m_fs;
	FIL		m_file;

	char	logical_drv[4]; /* root diretor */
	char path[64];
	char filename[64] = "TEST.TXT";
	int br,bw;
	int test_result = 1;
	int ret = 0;

	//1 init disk drive
	printf("Init Disk driver.\n");
#if FATFS_DISK_USB	
	_usb_init();
	ret = wait_usb_ready();
	if(ret != USB_INIT_OK){
		if(ret == USB_NOT_ATTACHED)
			printf("\r\n NO USB device attached\n");
		else
			printf("\r\n USB init fail\n");
		goto exit;
	}

#elif FATFS_DISK_SD
	// Set sdio debug level
	DBG_INFO_MSG_OFF(_DBG_SDIO_);  
	DBG_WARN_MSG_OFF(_DBG_SDIO_);
	DBG_ERR_MSG_ON(_DBG_SDIO_);
	
	if(sdio_init_host()!=0){
		printf("SDIO host init fail.\n");
		goto exit;
	}
#endif

	//1 register disk driver to fatfs
	printf("Register disk driver to Fatfs.\n");
#if FATFS_DISK_USB	
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
#elif FATFS_DISK_SD
	drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
#endif

	if(drv_num < 0){
		printf("Rigester disk driver to FATFS fail.\n");
	}else{
		Fatfs_ok = 1;

		logical_drv[0] = drv_num + '0';
		logical_drv[1] = ':';
		logical_drv[2] = '/';
		logical_drv[3] = 0;
	}
	//1 Fatfs write and read test 
	if(Fatfs_ok){

		printf("FatFS Write/Read test begin......\n\n");
		
		if(f_mount(&m_fs, logical_drv, 1)!= FR_OK){
			printf("FATFS mount logical drive fail.\n");
			goto fail;
		}
		// write and read test
		strcpy(path, logical_drv);

		sprintf(&path[strlen(path)],"%s",filename);

		//Open source file
    	res = f_open(&m_file, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if(res){
			printf("open file (%s) fail.\n", filename);
			goto fail;
		}

		printf("Test file name:%s\n\n",filename);

		// clean write and read buffer
		memset(&WRBuf[0], 0x00, TEST_SIZE);
		memset(&RDBuf[0], 0x00, TEST_SIZE);
		
		strcpy(&WRBuf[0], &test_info[0]);
		
		do{
			res = f_write(&m_file, WRBuf, strlen(WRBuf), (u32*)&bw);
			if(res){
				f_lseek(&m_file, 0); 
				printf("Write error.\n");
			}
			printf("Write %d bytes.\n", bw);
		} while (bw < strlen(WRBuf));

		printf("Write content:\n%s\n", WRBuf);
		printf("\n");
		
		/* move the file pointer to the file head*/
		res = f_lseek(&m_file, 0); 

		do{
			res = f_read(&m_file, RDBuf, strlen(WRBuf), (u32*)&br);
			if(res){
				f_lseek(&m_file, 0);
				printf("Read error.\n");
			}
			printf("Read %d bytes.\n", br);
		}while(br < strlen(WRBuf));
	
		printf("Read content:\n%s\n", RDBuf);

		// close source file
		res = f_close(&m_file);
		if(res){
			printf("close file (%s) fail.\n", filename);
		}

		//
		if(f_mount(NULL, logical_drv, 1) != FR_OK){
			printf("FATFS unmount logical drive fail.\n");
		}
		
		if(FATFS_UnRegisterDiskDriver(drv_num))	
			printf("Unregister disk driver from FATFS fail.\n");
	}

fail:
#if FATFS_DISK_USB	
	// deinit usb driver
#elif FATFS_DISK_SD
	sdio_deinit_host();
#endif

exit:
	vTaskDelete(NULL);
}


void example_fatfs(void)
{
	if(xTaskCreate(example_fatfs_thread, ((const char*)"example_fatfs_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_fatfs_thread) failed", __FUNCTION__);
}
#endif

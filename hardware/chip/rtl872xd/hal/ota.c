#include "ota/ota_service.h"

#include <platform/platform_stdlib.h>
#include <osdep_service.h>
#include <rtl8721d_ota.h>
#include <flash_api.h>
#include <device_lock.h>
#include "aos/hal/flash.h"
#include "aos/kernel.h"

#include <ameba_soc.h>

static u32 alinknewImg2Addr = 0xFFFFFFFF;

char *HeadBuffer = NULL;
#define OTA_HEADER_BUF_SIZE  1024
int alink_size = 0;
//u8 alink_signature[9] = {0};
//extern const update_file_img_id OtaImgId[2];
uint32_t alink_ota_target_index = OTA_INDEX_2;
update_ota_target_hdr OtaTargetHdr;
update_dw_info DownloadInfo[MAX_IMG_NUM];

static int aliota_total_len = 0, aliota_flag = 1, aliota_count = 0;
static uint32_t aliota_address;
static int aliota_RemainBytes[2] = {0};
static int aliota_ota_exit = 0;
static int aliota_ota_flag = 1;
static unsigned long aliota_tick1, aliota_tick2;
static u32 aliota_OtaFg = 0;
static u32 aliota_SigCnt = 0;
static int aliota_end_sig = 0;
static int aliota_flash_write_done = 0;
static int boot_part = HAL_PARTITION_OTA_TEMP;

hal_logic_partition_t *partition_info = NULL;
#ifndef OTA2_DEFAULT_ADDR
#define OTA2_DEFAULT_ADDR   (0x08106000)
#endif

int check_ota_index()
{
	uint32_t ota_index = -1;
	/* check OTA index we should update */
	if (ota_get_cur_index() == OTA_INDEX_1) {
		ota_index = OTA_INDEX_2;
		printf("OTA2 address space will be upgraded\n");
	} else {
		ota_index = OTA_INDEX_1;
		printf("OTA1 address space will be upgraded\n");
	}
	return ota_index;
}

bool prepare_ota_address(u32 ota_target_index, u32 * new_addr)
{
	update_file_img_hdr *OTAHdr = NULL;
	uint32_t OTALen = 0;
	uint32_t ota_addr;

	if (ota_target_index == OTA_INDEX_2)
		ota_addr = LS_IMG2_OTA2_ADDR;
	else if (ota_target_index == OTA_INDEX_1)
		ota_addr = LS_IMG2_OTA1_ADDR;
	printf("ota_addr = %x\n", ota_addr);

	if((ota_addr%4096) != 0) {
		printf("\n\r[%s] ota addr in sys data space not 4k aligned 0x%x", __FUNCTION__, ota_addr);
		goto error;
	}
	*new_addr = ota_addr;

	if(*new_addr == 0xFFFFFFFF) {
		printf("\n\r[%s] update address is invalid \n", __FUNCTION__);
		goto error;
	}
	printf("OTA update address: 0x%x\n", *new_addr);
	return TRUE;

error:
	return FALSE;
}

bool rtl8721d_ota_prepare()
{
	alink_size = 0;
	aliota_total_len = 0, aliota_flag = 1, aliota_count = 0;
	aliota_address = 0;
	aliota_RemainBytes[0] = 0;	
	aliota_RemainBytes[1] = 0;	
	aliota_ota_exit = 0;
	aliota_ota_flag = 1;
	aliota_tick1 = 0, aliota_tick2 = 0;
	aliota_OtaFg = 0;
	aliota_SigCnt = 0;
	aliota_end_sig = 0;
	aliota_flash_write_done = 0;

	memset((u8 *)&OtaTargetHdr, 0, sizeof(update_ota_target_hdr));
	//uint32_t part = HAL_PARTITION_OTA_TEMP;
	alink_ota_target_index = check_ota_index();
	if(prepare_ota_address(alink_ota_target_index, &alinknewImg2Addr) != TRUE){
		printf("\n get OTA address failed\n");
		return FALSE;
	}

	if(alink_ota_target_index == OTA_INDEX_1) {
		boot_part = HAL_PARTITION_APPLICATION;
		//alinknewImg2Addr = LS_IMG2_OTA1_ADDR;
	}else if(alink_ota_target_index == OTA_INDEX_2) {
		boot_part = HAL_PARTITION_OTA_TEMP;
		//alinknewImg2Addr = LS_IMG2_OTA2_ADDR;
	}else {
		printf("flash INDEX failed\n");
		return FALSE;
	}

	partition_info = hal_flash_get_info(boot_part);
	if(partition_info == NULL) {
		printf("hal_flash_get_info failed\n");
		return FALSE;
	}

	/*-------------------erase flash space for new firmware--------------*/
	erase_ota_target_flash(alinknewImg2Addr, partition_info->partition_length);
	//DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);
	HeadBuffer = rtw_malloc(OTA_HEADER_BUF_SIZE);
	if (HeadBuffer == NULL) {
	    printf("malloc headbuffer failed\n");
	    return FALSE;
	}
	return TRUE;
}

static int rtl8721d_ota_init(void *something)
{
    printf("rtl8721d_ota_init\n");
    uint32_t offset = *(uint32_t*)something;

    if(offset==0) {
        /* prepare to os update  */
        if (rtl8721d_ota_prepare() != TRUE) {
                return -1;
        }
    }
    return 0;
}

static int rtl8721d_ota_write_ota_cb(int* off_set, char *buffer, int len)
{
	char *buf;
	update_file_hdr OtaFileHdr;
	uint32_t i;
	int len_temp;
	unsigned int buf_offset = 0;;
	unsigned int copy_len =0;

	if (aliota_ota_flag) {
	    aliota_tick1 = rtw_get_current_time();
	    aliota_ota_flag = 0;
	}

	/*-----read firmware header from server*/
	//buf = HeadBuffer + aliota_total_len;
	aliota_total_len += len;
	if (aliota_total_len < 32) {//reach until get at least 8+24 bytes(one image)
		if(HeadBuffer != NULL) {
			buf = HeadBuffer+aliota_count;
			memcpy(buf, buffer, len);
			aliota_count += len;
                	printf("copy head len:%d \n",len);
		}
		return 0;
	} 
	else if ((aliota_total_len >= 32) && aliota_total_len <= OTA_HEADER_BUF_SIZE) {		
		if (aliota_flag == 1) {
			buf = HeadBuffer;// TODO :consider add protection to HeadBuffer
			memcpy((u8*)(&(OtaTargetHdr.FileHdr)), buf, sizeof(update_file_hdr));//get FileHdr info(hdr num)

			if (aliota_total_len < (8+OtaTargetHdr.FileHdr.HdrNum*24))//copy to buf and return to get more data
			{
				buf = HeadBuffer;
				memcpy(buf+aliota_count, buffer, len);
				aliota_count += len;
                		return 0;
			}

			//get enough data and start processing
			buf = HeadBuffer;
			memcpy(buf+aliota_count, buffer, aliota_total_len - aliota_count);

			if (!get_ota_tartget_header(buf, aliota_total_len, &OtaTargetHdr, alink_ota_target_index)) {
				printf("\n\rget OTA header failed\n");
				goto update_ota_exit;
			}



			/*Image size should not bigger than OTA_SIZE_MAX */
			if(partition_info != NULL) {
				uint32_t imglen = 0;
				for (i=0; i< OtaTargetHdr.ValidImgCnt; i++) {
					imglen += OtaTargetHdr.FileImgHdr[i].ImgLen;
					if (OtaTargetHdr.FileImgHdr[i].FlashAddr != alinknewImg2Addr)
						printf("\n\rOTA address not match\n");
				}
				
				if(imglen > partition_info->partition_length){
					printf("\n\r[%s] illegal new image length 0x%x", __FUNCTION__, imglen);
					goto update_ota_exit;
				}
			}
#if 0	//skip OTF MASK in AmebaD				
			/*the upgrade space should be masked, because the encrypt firmware is used 
			for checksum calculation*/
			OTF_Mask(1, (alinknewImg2Addr - SPI_FLASH_BASE), NewImg2BlkSize, 1);
#endif
			/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/

			memset((u8 *)DownloadInfo, 0, MAX_IMG_NUM*sizeof(update_dw_info));
			for(i = 0; i < OtaTargetHdr.ValidImgCnt; i++) {
				DownloadInfo[i].ImgId = OTA_IMAG;
				DownloadInfo[i].FlashAddr = OtaTargetHdr.FileImgHdr[i].FlashAddr - SPI_FLASH_BASE + 8; //aliota_address
				DownloadInfo[i].ImageLen = OtaTargetHdr.FileImgHdr[i].ImgLen - 8; /*skip the signature*/
				DownloadInfo[i].ImgOffset = OtaTargetHdr.FileImgHdr[i].Offset;
				aliota_RemainBytes[i] = DownloadInfo[i].ImageLen;
				printf("FlashAddr=%x,ImgLen=%x,ImgOff=%x\n",DownloadInfo[i].FlashAddr,DownloadInfo[i].ImageLen,DownloadInfo[i].ImgOffset);
			}
			aliota_flag = 0;
			if (HeadBuffer != NULL) {
				rtw_free(HeadBuffer);
			}		
		}
	}

	/*skip the signature*/
	for (i=0; i < OtaTargetHdr.ValidImgCnt; i++){
		u8 * signature;
		signature = &(OtaTargetHdr.Sign[i][0]);
		/*download the new firmware from server*/
		if(aliota_RemainBytes[i] > 0) {
			if(aliota_total_len > DownloadInfo[i].ImgOffset) {
				if(!aliota_OtaFg) {
					u32 Cnt = 0;
					/*reach the the desired image, the first packet process*/
					aliota_OtaFg = 1;
					Cnt = aliota_total_len -DownloadInfo[i].ImgOffset;
					if(Cnt < 8) {
						aliota_SigCnt = Cnt;
					} else {
						aliota_SigCnt = 8;
					}
					memcpy(signature, buffer + len -Cnt, aliota_SigCnt);
					if((aliota_SigCnt < 8) || (Cnt -8 == 0)) {
						return 0;
					}
					copy_len = Cnt -8;
					buf_offset = len - Cnt +8;
					goto WRITETOFLASH;

				} else {					
					/*normal packet process*/
					if(aliota_SigCnt < 8) {
						if(len < (8 -aliota_SigCnt)) {
							memcpy(signature + aliota_SigCnt, buffer, len);
							aliota_SigCnt += len;
							return 0;
						} else {
							memcpy(signature + aliota_SigCnt, buffer, (8 -aliota_SigCnt));
							aliota_end_sig = 8 -aliota_SigCnt;
							len_temp = len;
							len_temp -= (8 -aliota_SigCnt) ;
							aliota_SigCnt = 8;
							if(!len_temp) {
								return 0;
							}
							copy_len = len_temp;
							len_temp = aliota_RemainBytes[i] -len_temp;
							if (len_temp <= 0) {
								copy_len = aliota_RemainBytes[i];
							}

							buf_offset = aliota_end_sig;
							goto WRITETOFLASH;
							//return 0;
						}
					}
					copy_len = len;
					len_temp = aliota_RemainBytes[i] -len;
					if(len_temp <= 0) {
						copy_len = aliota_RemainBytes[i];
					}
					buf_offset = 0;
					goto WRITETOFLASH;

				}
	WRITETOFLASH:
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(ota_writestream_user(DownloadInfo[i].FlashAddr + alink_size, copy_len, buffer+buf_offset) < 0){
					printf("Write sector failed\r\n");
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					goto update_ota_exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				alink_size += copy_len;
				aliota_RemainBytes[i] -= copy_len;
				aliota_tick2 = rtw_get_current_time();
				if (aliota_tick2 - aliota_tick1 > 2000) {
					printf("Download OTA file: %d B, RemainBytes = %d\n", (alink_size), aliota_RemainBytes[i]);
					aliota_ota_flag = 1;
				}
				
				if (aliota_RemainBytes[i] <= 0)
				{
					printf("file size=%x\n",alink_size);
					aliota_OtaFg = 0;//clear flags when image[i] all writen to flash
					alink_size = 0;
					aliota_SigCnt = 0;
					aliota_end_sig = 0;
					aliota_flash_write_done = 1;
				}else{
					return 0;
				}	
			}
		}
	}
    return 0;
update_ota_exit:
	if (HeadBuffer != NULL) {
		rtw_free(HeadBuffer);
	}
        aliota_ota_exit = 1;
	printf("Update task exit");
	return -1;
}

static int rtl8721d_ota_finish_cb(void *something)
{
	//printf("size = %d, OtaTargetHdr.FileImgHdr.ImgLen = %d\n", alink_size, OtaTargetHdr.FileImgHdr[].ImgLen);
	//printf("buffer signature is: = %s", (u8 *)(&(OtaTargetHdr.Sign[0][0])));
	 /*------------- verify checksum and update signature-----------------*/
	if (aliota_flash_write_done){
		if(verify_ota_checksum( &OtaTargetHdr)){
                int ret = 0;
                ota_boot_param_t *param = (ota_boot_param_t *)something;
                if (param == NULL) {
                        return -1;
                }
                printf("checksum check ok upg_flag:0x%x \n", param->upg_flag);
                if (param->upg_flag == OTA_RAW) {
                        int offset = 0x00;
                        int param_part = HAL_PARTITION_PARAMETER_1;
                        hal_logic_partition_t *part_info = hal_flash_get_info(boot_part);
                        param->src_adr = part_info->partition_start_addr;
                        ota_boot_param_t param_r;
                        offset = 0x00;
                        hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
                        offset = 0x00;
                        hal_flash_write(param_part, (uint32_t*)&offset, param, sizeof(ota_boot_param_t));
                        offset = 0x00;
                        memset(&param_r, 0, sizeof(ota_boot_param_t));
                        hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
                        if(memcmp(param, &param_r, sizeof(ota_boot_param_t)) != 0) {
                               return -1;
                        }
                        printf("OTA finish dst:0x%08x src:0x%08x len:0x%08x upg_flag:0x%x \r\n", param_r.dst_adr, param_r.src_adr, param_r.len, param_r.upg_flag);
                 }
			if(!change_ota_signature(&OtaTargetHdr, alink_ota_target_index)) {
				printf("\n%s: change signature failed\n");
				return -1;
			}
	#if 0
			//reset board
			{
				WDG_InitTypeDef WDG_InitStruct;
				u32 CountProcess;
				u32 DivFacProcess;
				
				aos_msleep(100);
				
	#if defined(CONFIG_MBED_API_EN) && CONFIG_MBED_API_EN
				rtc_backup_timeinfo();
	#endif

				WDG_Scalar(50, &CountProcess, &DivFacProcess);
				WDG_InitStruct.CountProcess = CountProcess;
				WDG_InitStruct.DivFacProcess = DivFacProcess;
				WDG_Init(&WDG_InitStruct);

				WDG_Cmd(ENABLE);

				return 0;
			}
	#endif
		}else {
			printf("checksum check fail\n");
			return -1;
		}
	}else{
		printf("flash write incomplete\n");
		return -1;
	}

}

static int rtl8721d_ota_read_ota_cb(int* off_set, char* out_buf, int out_buf_len)
{
	//ota_readstream_user(off_set, out_buf_len, out_buf);
	return hal_flash_read(boot_part, (uint32_t*)off_set, out_buf, out_buf_len);
}

static int rtl8721d_ota_rollback(void *something)
{
    int offset = 0x00;
    int param_part = HAL_PARTITION_PARAMETER_1;
    ota_boot_param_t param_w, param_r;
    memset(&param_w, 0, sizeof(ota_boot_param_t));
    hal_flash_read(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
    if((param_w.boot_count != 0) && (param_w.boot_count != 0xff)) {
        param_w.boot_count = 0; /*Clear bootcount to avoid rollback*/
        printf("clear bootcount.\n");
        offset = 0x00;
        hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
        offset = 0x00;
        hal_flash_write(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
        offset = 0x00;
        memset(&param_r, 0, sizeof(ota_boot_param_t));
        hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
        if(memcmp(&param_w, &param_r, sizeof(ota_boot_param_t)) != 0) {
            printf("rollback clear failed\n");
            return -1;
        }
    }
    return 0;
}

const char *rtl8721d_ota_get_version(unsigned char dev_type)
{
	if(dev_type){
		return "v1.0.0-20180101-1000";
	}else{
		return SYSINFO_APP_VERSION;
	}
}

ota_hal_module_t ota_hal_module = {
    .init         = rtl8721d_ota_init,
    .write    = rtl8721d_ota_write_ota_cb,
    .read     = rtl8721d_ota_read_ota_cb,
    .boot = rtl8721d_ota_finish_cb,
    .rollback = rtl8721d_ota_rollback,
    .version = rtl8721d_ota_get_version,
};

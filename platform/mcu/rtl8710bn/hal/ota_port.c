
#include "hal/ota.h"

#include <platform/platform_stdlib.h>
#include <osdep_service.h>
#include <rtl8710b_ota.h>
#include <flash_api.h>
#include <device_lock.h>

static u32 alinknewImg2Addr = 0xFFFFFFFF;

char *HeadBuffer = NULL;
#define OTA_HEADER_BUF_SIZE		1452
int alink_size = 0; 
#define ALINK_BUF_SIZE		512
u8 alink_signature[9] = {0};
extern const update_file_img_id OtaImgId[2];
uint32_t alink_ota_target_index = OTA_INDEX_2;
update_ota_target_hdr OtaTargetHdr;


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

bool rtl8710bn_ota_prepare()
{
    alink_ota_target_index = check_ota_index();
    DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);
    HeadBuffer = rtw_malloc(OTA_HEADER_BUF_SIZE);
    if (HeadBuffer == NULL) {
        printf("malloc headbuffer failed\n");
        return FALSE;
    }
    return TRUE;
}

static int rtl8710bn_ota_init(hal_ota_module_t *m, void *something)
{    
        LOG("rtl8710bn_ota_init\n");
        uint32_t offset = *(uint32_t*)something;

        if(offset==0) {
            /* prepare to os update  */
            if (rtl8710bn_ota_prepare() != TRUE) { 
                return -1; 
            }
            
        }
        else {

        }
    
        return 0;
}

static int rtl8710bn_ota_write_ota_cb(hal_ota_module_t *m, volatile uint32_t* off_set,uint8_t *buffer,uint32_t len)
{
    char *buf;
    static int total_len = 0, flag = 1, count = 0;
    static uint32_t address;
    flash_t	flash;
    update_file_hdr OtaFileHdr;
    static int RemainBytes = 0;	
    static int ota_exit = 0;


    static int ota_flag = 1;
    static unsigned long tick1, tick2;
    if (ota_flag) {
        tick1 = rtw_get_current_time();
        ota_flag = 0;
    }


        //printf("write ota offset %x, len %d\r\n", off_set, len);
      
	/*-----read 4 Dwords from server, get image header number and header length*/
	buf = HeadBuffer + total_len;
	total_len += len;
	if (total_len < 128) {
		memcpy(buf, buffer, len);
		count += len;
		return -1;
		
	} else if (total_len >= 128 && total_len <= OTA_HEADER_BUF_SIZE) {
		if (flag == 1) {
			buf = HeadBuffer;
			memcpy(buf+count, buffer, total_len - count);
			memcpy((u8*)(&OtaTargetHdr.FileHdr), buf, sizeof(OtaTargetHdr.FileHdr));
			memcpy((u8*)(&OtaTargetHdr.FileImgHdr), buf+8, 8);
			/*------acquire the desired Header info from buffer*/
			buf = HeadBuffer;
			u8 *TempBuf = NULL;
			TempBuf = (u8 *)(&OtaImgId[alink_ota_target_index]);
			printf("TempBuf = %s\n",TempBuf);
			if (!get_ota_tartget_header(buf, total_len, &OtaTargetHdr, TempBuf)) {
				printf("\n\rget OTA header failed\n");
				goto update_ota_exit;
			}

			/*get new image addr and check new address validity*/
			if(!get_ota_address(alink_ota_target_index, &alinknewImg2Addr, &OtaTargetHdr)) {
				printf("\n get OTA address failed\n");
				goto update_ota_exit;
			}
			/*get new image length from the firmware header*/
			uint32_t NewImg2Len = 0, NewImg2BlkSize = 0;
			NewImg2Len = OtaTargetHdr.FileImgHdr.ImgLen;
			NewImg2BlkSize = ((NewImg2Len - 1)/4096) + 1;
			/*-------------------erase flash space for new firmware--------------*/
			erase_ota_target_flash(alinknewImg2Addr, NewImg2Len);
			
			/*the upgrade space should be masked, because the encrypt firmware is used 
			for checksum calculation*/
			OTF_Mask(1, (alinknewImg2Addr - SPI_FLASH_BASE), NewImg2BlkSize, 1);
			/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/
			address = alinknewImg2Addr -SPI_FLASH_BASE + 8;
			printf("NewImg2Addr = %x\n", alinknewImg2Addr);
			RemainBytes = OtaTargetHdr.FileImgHdr.ImgLen - 8;
			flag = 0;
			if (HeadBuffer != NULL) {
				rtw_free(HeadBuffer);
			}
		}
	}

	/*skip the signature*/
	static u32 OtaFg = 0;
	static u32 SigCnt = 0;

	/*download the new firmware from server*/
	if(RemainBytes > 0) {
		if(total_len > OtaTargetHdr.FileImgHdr.Offset) {
			if(!OtaFg) {
				u32 Cnt = 0;
				/*reach the the desired image, the first packet process*/
				OtaFg = 1;
				Cnt = total_len -OtaTargetHdr.FileImgHdr.Offset;
				if(Cnt < 8) {
					SigCnt = Cnt;
				} else {
					SigCnt = 8;
				}
				memcpy(alink_signature, buffer + len -Cnt, SigCnt);
				if((SigCnt < 8) || (Cnt -8 == 0)) {
					return 0;
				}				
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(flash_stream_write(&flash, address + alink_size, Cnt -8, buffer + (len -Cnt + 8)  ) < 0){
					printf("Write sector failed\r\n");
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					goto update_ota_exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				alink_size += (Cnt - 8);
				RemainBytes -= alink_size;
			} else {					
				/*normal packet process*/
				if(SigCnt < 8) {
					if(len < (8 -SigCnt)) {
						memcpy(alink_signature + SigCnt, buffer, len);
						SigCnt += len;
						return 0;
					} else {
						memcpy(alink_signature + SigCnt, buffer, (8 -SigCnt));
						static int end_sig = 0;
						end_sig = 8 -SigCnt;
						len -= (8 -SigCnt) ;
						SigCnt = 8;
						if(!len) {
							return 0;
						}
						RemainBytes -= len;
						if (RemainBytes <= 0) {
							len = len - (-RemainBytes);
						}
						device_mutex_lock(RT_DEV_LOCK_FLASH);
						if (flash_stream_write(&flash, address+alink_size, len, buffer+end_sig) < 0){
							printf("Write sector failed\r\n");
							device_mutex_unlock(RT_DEV_LOCK_FLASH);
							goto update_ota_exit;
						}
						device_mutex_unlock(RT_DEV_LOCK_FLASH);
						alink_size += len;
						return 0;
					}
				}
				RemainBytes -= len;
				int end_ota = 0;
				if(RemainBytes <= 0) {
					len = len - (-RemainBytes);
					end_ota = 1;
				}
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(flash_stream_write(&flash, address + alink_size, len, buffer) < 0){
					printf("Write sector failed\r\n");
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					goto update_ota_exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				alink_size += len;
				if (end_ota) {
					printf("size = %x\n", alink_size);
				}
				tick2 = rtw_get_current_time();
				if (tick2 - tick1 > 2000) {
					printf("Download OTA file: %d B, RemainBytes = %d\n", (alink_size), RemainBytes);
					ota_flag = 1;
				}
			}
		}
	}    

    return 0;

update_ota_exit:
	if (HeadBuffer != NULL) {
		rtw_free(HeadBuffer);
	}
        ota_exit = 1;
	printf("Update task exit");	
	return -1;	
    
}

static int rtl8710bn_ota_finish_cb(hal_ota_module_t *m, void *something)
{
	flash_t flash;
	printf("size = %d, OtaTargetHdr.FileImgHdr.ImgLen = %d\n", alink_size, OtaTargetHdr.FileImgHdr.ImgLen);
	printf("buffer signature is: = %s", alink_signature);
	 /*------------- verify checksum and update signature-----------------*/
	if(verify_ota_checksum(alinknewImg2Addr, alink_size, alink_signature, &OtaTargetHdr)){
		if(!change_ota_signature(alinknewImg2Addr, alink_signature, alink_ota_target_index)) {
			printf("\n%s: change signature failed\n");
			return -1;
		}
		return 0;
	} else {
		/*if checksum error, clear the signature zone which has been 
		written in flash in case of boot from the wrong firmware*/
		#if 1
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		flash_erase_sector(&flash, alinknewImg2Addr - SPI_FLASH_BASE);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		#endif
		printf("chack_sum error\n");
		return -1;
	}

}

static int rtl8710bn_ota_read_ota_cb(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    return 0;
}

hal_ota_module_t rtl8710bn_ota_module = {
    .init         = rtl8710bn_ota_init,
    .ota_write    = rtl8710bn_ota_write_ota_cb,
    .ota_read     = rtl8710bn_ota_read_ota_cb,
    .ota_set_boot = rtl8710bn_ota_finish_cb,
};


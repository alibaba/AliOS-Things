#include <sys.h>
#include <device_lock.h>
#include "ota_8195a.h"
#include "lwip/netdb.h"

sys_thread_t TaskOTA = NULL;
#define STACK_SIZE		1024
#define TASK_PRIORITY	tskIDLE_PRIORITY + 1
#if SWAP_UPDATE
static uint32_t OldImg2Addr;
#endif
static flash_t flash_ota;

#if CONFIG_CUSTOM_SIGNATURE
/* ---------------------------------------------------
  *  Customized Signature
  * ---------------------------------------------------*/
// This signature can be used to verify the correctness of the image
// It will be located in fixed location in application image
#include "section_config.h"
SECTION(".custom.validate.rodata")
const unsigned char cus_sig_demo[32] = "Customer Signature-modelxxx";
#endif

void* update_malloc(unsigned int size){
	return pvPortMalloc(size);
}

void update_free(void *buf){
	vPortFree(buf);
}

void ota_platform_reset(void){
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
}

#if WRITE_OTA_ADDR
int write_ota_addr_to_system_data(flash_t *flash, uint32_t ota_addr)
{
	uint32_t data, i = 0;
	//Get upgraded image 2 addr from offset
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_read_word(flash, OFFSET_DATA, &data);
	printf("\n\r[%s] data 0x%x ota_addr 0x%x", __FUNCTION__, data, ota_addr);
	if(~0x0 == data){
		flash_write_word(flash, OFFSET_DATA, ota_addr);
	}
	else{
		//erase backup sector
		flash_erase_sector(flash, BACKUP_SECTOR);
		//backup system data to backup sector
		for(i = 0; i < 0x1000; i+= 4){
			flash_read_word(flash, OFFSET_DATA + i, &data);
			if(0 == i)
				data = ota_addr;
			flash_write_word(flash, BACKUP_SECTOR + i,data);
		}
		//erase system data
		flash_erase_sector(flash, OFFSET_DATA);
		//write data back to system data
		for(i = 0; i < 0x1000; i+= 4){
			flash_read_word(flash, BACKUP_SECTOR + i, &data);
			flash_write_word(flash, OFFSET_DATA + i,data);
		}
		//erase backup sector
		flash_erase_sector(flash, BACKUP_SECTOR);
	}
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	return 0;
}
#endif

int update_ota_connect_server(int server_socket, update_cfg_local_t *cfg){
	struct sockaddr_in server_addr;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = cfg->ip_addr;
	server_addr.sin_port = cfg->port;

	if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		printf("\n\r[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	return server_socket;
}

uint32_t update_ota_prepare_addr(void){
	
	uint32_t Img2Len = 0;
	uint32_t IMAGE_x = 0, ImgxLen = 0, ImgxAddr = 0;
	uint32_t NewImg2Addr = 0; 
#if WRITE_OTA_ADDR
	uint32_t ota_addr = 0x80000;
#endif
	
	DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);
	// The upgraded image2 pointer must 4K aligned and should not overlap with Default Image2
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_read_word(&flash_ota, IMAGE_2, &Img2Len);
	IMAGE_x = IMAGE_2 + Img2Len + 0x10;
	flash_read_word(&flash_ota, IMAGE_x, &ImgxLen);
	flash_read_word(&flash_ota, IMAGE_x+4, &ImgxAddr);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	
	if(0x30000000 == ImgxAddr){
		printf("\n\r[%s] IMAGE_3 0x%x Img3Len 0x%x", __FUNCTION__, IMAGE_x, ImgxLen);
	}
	else{
		printf("\n\r[%s] There is no IMAGE_3", __FUNCTION__);
		IMAGE_x = IMAGE_2;
		ImgxLen = Img2Len;
	}
#if WRITE_OTA_ADDR
	if((ota_addr > IMAGE_x) && ((ota_addr < (IMAGE_x+ImgxLen))) || (ota_addr < IMAGE_x) ||
		((ota_addr & 0xfff) != 0) || (ota_addr == ~0x0)){
		printf("\n\r[%s] illegal ota addr 0x%x", __FUNCTION__, ota_addr);
		return -1;
	}
	else
		write_ota_addr_to_system_data(&flash_ota, ota_addr);
#endif

	//Get upgraded image 2 addr from offset
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_read_word(&flash_ota, OFFSET_DATA, &NewImg2Addr);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	if((NewImg2Addr > IMAGE_x) && ((NewImg2Addr < (IMAGE_x+ImgxLen))) || (NewImg2Addr < IMAGE_x) ||
		((NewImg2Addr & 0xfff) != 0) || (NewImg2Addr == ~0x0)){
		printf("\n\r[%s] Invalid OTA Address 0x%x", __FUNCTION__, NewImg2Addr);
		return -1;
	}

	return NewImg2Addr;
}

#if SWAP_UPDATE
uint32_t update_ota_swap_addr(uint32_t img_len, uint32_t NewImg2Addr){
	uint32_t SigImage0,SigImage1;
	uint32_t Part1Addr=0xFFFFFFFF, Part2Addr=0xFFFFFFFF;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_read_word(&flash_ota, 0x18, &Part1Addr);
	Part1Addr = (Part1Addr&0xFFFF)*1024;	//PART1 : 0x0000B000
	Part2Addr = NewImg2Addr;				//PART2 : 0x00080000
	
	// read Part1 signature
	flash_read_word(&flash_ota, Part1Addr+8, &SigImage0);
	flash_read_word(&flash_ota, Part1Addr+12, &SigImage1);
	printf("\n\r[%s] Part1 Sig %x", __FUNCTION__, SigImage0);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	if(SigImage0==0x35393138 && SigImage1==0x31313738)//Part1 is the new one with signature "81958711"
		OldImg2Addr = Part1Addr;	//Change Part1 to older version
	else{
		// read Part2 signature
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		flash_read_word(&flash_ota, Part2Addr+8, &SigImage0);
		flash_read_word(&flash_ota, Part2Addr+12, &SigImage1);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		printf("\n\r[%s] Part2 Sig %x", __FUNCTION__, SigImage0);
		if(SigImage0==0x30303030 && SigImage1==0x30303030){// ATSC signature "00000000"
			OldImg2Addr = Part1Addr;	//Store the new version to Part2	
		}
		else if(SigImage0==0x35393138 && SigImage1==0x31313738){//Part2 is the new one with signature "81958711"
			OldImg2Addr = Part2Addr;	//Change Part2 to older version
			NewImg2Addr = Part1Addr;
			if( img_len > (Part2Addr-Part1Addr) ){	// firmware size too large
				printf("\n\r[%s] Part1 size < OTA size", __FUNCTION__);
				return -1;
			}
		}
		else
			NewImg2Addr = Part2Addr;
	}
	
	printf("\n\r[%s] New %x, Old %x", __FUNCTION__, NewImg2Addr, OldImg2Addr);
	return NewImg2Addr;
}
#endif

int update_ota_erase_upg_region(uint32_t img_len, uint32_t NewImg2Len, uint32_t NewImg2Addr){
	uint32_t NewImg2BlkSize = 0;

	if(NewImg2Len == 0){
		NewImg2Len = img_len;
		printf("\n\r[%s] NewImg2Len %d  ", __FUNCTION__, NewImg2Len);
		if((int)NewImg2Len > 0){
			NewImg2BlkSize = ((NewImg2Len - 1)/4096) + 1;
			printf("\n\r[%s] NewImg2BlkSize %d  0x%8x", __FUNCTION__, NewImg2BlkSize, NewImg2BlkSize);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			for(int i = 0; i < NewImg2BlkSize; i++)
				flash_erase_sector(&flash_ota, NewImg2Addr + i * 4096);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
		}else{
			printf("\n\r[%s] Size INVALID", __FUNCTION__);
			return -1;
		}
	}	
	
	printf("\n\r[%s] NewImg2Addr 0x%x", __FUNCTION__, NewImg2Addr);
	return NewImg2Len;
}

int update_ota_checksum(_file_checksum *file_checksum, uint32_t flash_checksum, uint32_t NewImg2Addr){

#if CONFIG_CUSTOM_SIGNATURE
	char custom_sig[32] = "Customer Signature-modelxxx";
	uint32_t read_custom_sig[8];

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	for(int i = 0; i < 8; i ++){
		flash_read_word(&flash_ota, NewImg2Addr + 0x28 + i *4, read_custom_sig + i);
	}
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	printf("\n\r[%s] read_custom_sig %s", __FUNCTION__ , (char*)read_custom_sig);
#endif

	printf("\n\rflash checksum 0x%8x attached checksum 0x%8x", flash_checksum, file_checksum->u);

	// compare checksum with received checksum
	if( (file_checksum->u == flash_checksum)
#if CONFIG_CUSTOM_SIGNATURE
		&& !strcmp((char*)read_custom_sig,custom_sig)
#endif
	){
		//Set signature in New Image 2 addr + 8 and + 12
		uint32_t sig_readback0,sig_readback1;
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		flash_write_word(&flash_ota,NewImg2Addr + 8, 0x35393138);
		flash_write_word(&flash_ota,NewImg2Addr + 12, 0x31313738);
		flash_read_word(&flash_ota, NewImg2Addr + 8, &sig_readback0);
		flash_read_word(&flash_ota, NewImg2Addr + 12, &sig_readback1);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		printf("\n\r[%s] signature %x,%x", __FUNCTION__ , sig_readback0, sig_readback1);
#if SWAP_UPDATE
		if(OldImg2Addr != ~0x0){
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_write_word(&flash_ota,OldImg2Addr + 8, 0x35393130);
			flash_write_word(&flash_ota,OldImg2Addr + 12, 0x31313738);
			flash_read_word(&flash_ota, OldImg2Addr + 8, &sig_readback0);
			flash_read_word(&flash_ota, OldImg2Addr + 12, &sig_readback1);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			printf("\n\r[%s] old signature %x,%x", __FUNCTION__ , sig_readback0, sig_readback1);
		}
#endif			
		printf("\n\r[%s] Update OTA success!", __FUNCTION__);
		return 0;
	}
	return -1;
}

static void update_ota_local_task(void *param)
{
	int server_socket;
	unsigned char *buf, *alloc;
	_file_checksum *file_checksum;
	int read_bytes = 0, size = 0, i = 0;
	update_cfg_local_t *cfg = (update_cfg_local_t *)param;
	uint32_t address, flash_checksum=0;
	uint32_t NewImg2Len = 0, NewImg2Addr = 0, file_info[3];
	int ret = -1 ;

	printf("\n\r[%s] Update task start", __FUNCTION__);
	alloc = update_malloc(BUF_SIZE+4);
	if(!alloc){
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit;
	}
	buf = &alloc[4];
	file_checksum = (void*)alloc;

	// Connect server
	server_socket = update_ota_connect_server(server_socket, cfg);
	if(server_socket == -1){
		goto update_ota_exit;
	}

	NewImg2Addr = update_ota_prepare_addr();
	if(NewImg2Addr == -1){
		goto update_ota_exit;
	}
	
	//Clear file_info
	memset(file_info, 0, sizeof(file_info));
	
	if(file_info[0] == 0){
		printf("\n\r[%s] Read info first", __FUNCTION__);
		read_bytes = read(server_socket, file_info, sizeof(file_info));
		// !X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X
		// !W checksum !W padding 0 !W file size !W
		// !X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X
		printf("\n\r[%s] info %d bytes", __FUNCTION__, read_bytes);
		printf("\n\r[%s] tx chechsum 0x%x, file size 0x%x", __FUNCTION__, file_info[0],file_info[2]);
		if(file_info[2] == 0){
			printf("\n\r[%s] No checksum and file size", __FUNCTION__);
			goto update_ota_exit;
		}
	}
	
#if SWAP_UPDATE
	NewImg2Addr = update_ota_swap_addr(file_info[2], NewImg2Addr);
	if(NewImg2Addr == -1){
		goto update_ota_exit;
	}	
#endif

	NewImg2Len = update_ota_erase_upg_region(file_info[2], NewImg2Len, NewImg2Addr);
	if(NewImg2Len == -1){
		goto update_ota_exit;
	}

	// reset
	file_checksum->u = 0;
	// Write New Image 2 sector
	if(NewImg2Addr != ~0x0){
		address = NewImg2Addr;
		printf("\n\rStart to read data %d bytes\r\n", NewImg2Len);
		while(1){
			memset(buf, 0, BUF_SIZE);
			read_bytes = read(server_socket, buf, BUF_SIZE);
			if(read_bytes == 0) 
				break; // Read end
			if(read_bytes < 0){
				printf("\n\r[%s] Read socket failed", __FUNCTION__);
				goto update_ota_exit;
			}
			
			if(read_bytes<4)
				printf("\n\r[%s] Recv small packet", __FUNCTION__);
			printf(".");

			if((size+read_bytes)>NewImg2Len){
				printf("\n\r[%s] Redundant bytes received", __FUNCTION__);
				read_bytes = NewImg2Len-size;	
			}
			
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if(flash_stream_write(&flash_ota, address + size, read_bytes, buf) < 0){
				printf("\n\r[%s] Write stream failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			size += read_bytes;
			
			file_checksum->c[0] = alloc[4+read_bytes-4];      // checksum attached at file end
			file_checksum->c[1] = alloc[4+read_bytes-3];
			file_checksum->c[2] = alloc[4+read_bytes-2];
			file_checksum->c[3] = alloc[4+read_bytes-1];

			if(size == NewImg2Len)
				break;
		}
		printf("\n\rRead data finished\r\n");

		// read flash data back and calculate checksum
		for(i = 0; i < size-4; i += BUF_SIZE){
			int k;
			int rlen = (size-4-i) > BUF_SIZE ? BUF_SIZE : (size-4-i);
			flash_stream_read(&flash_ota, NewImg2Addr+i, rlen, buf);
			for(k = 0; k < rlen; k++)
				flash_checksum+=buf[k];
		}

		ret = update_ota_checksum(file_checksum, flash_checksum, NewImg2Addr);
		if(ret == -1){
			printf("\r\nThe checksume is wrong!\r\n");
			goto update_ota_exit;
		}
	}
update_ota_exit:
	if(alloc)
		update_free(alloc);	
	if(server_socket >= 0)
		close(server_socket);
	if(param)
		update_free(param);
	TaskOTA = NULL;
	printf("\n\r[%s] Update task exit", __FUNCTION__);	
	if(!ret){
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);	
		ota_platform_reset();
	}
	vTaskDelete(NULL);	
	return;
}

int update_ota_local(char *ip, int port){
	update_cfg_local_t *pUpdateCfg;
	
	if(TaskOTA){
		printf("\n\r[%s] Update task has created.", __FUNCTION__);
		return 0;
	}
	pUpdateCfg = update_malloc(sizeof(update_cfg_local_t));
	if(pUpdateCfg == NULL){
		printf("\n\r[%s] Alloc update cfg failed", __FUNCTION__);
		return -1;
	}
	pUpdateCfg->ip_addr = inet_addr(ip);
	pUpdateCfg->port = ntohs(port);
		
	if(xTaskCreate(update_ota_local_task, "OTA_server", STACK_SIZE, pUpdateCfg, TASK_PRIORITY, &TaskOTA) != pdPASS){
	  	update_free(pUpdateCfg);
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
	return 0;
}

void cmd_update(int argc, char **argv){
	int port;
	if(argc != 3){
		printf("\n\r[%s] Usage: update IP PORT", __FUNCTION__);
		return;
	}
	port = atoi(argv[2]);
	update_ota_local(argv[1], port);
}

void cmd_ota_image(bool cmd){
	flash_t	flash;
	uint32_t Part1Addr = 0xFFFFFFFF,Part2Addr = 0xFFFFFFFF;
	uint8_t *pbuf = NULL;
	
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_read_word(&flash, 0x18, &Part1Addr);
	Part1Addr = (Part1Addr&0xFFFF)*1024;	// first partition
	flash_read_word(&flash, OFFSET_DATA, &Part2Addr);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	if(Part2Addr == ~0x0)
		return;

	pbuf = update_malloc(FLASH_SECTOR_SIZE);
	if(!pbuf) return;
	
	device_mutex_lock(RT_DEV_LOCK_FLASH);	
	flash_stream_read(&flash, Part2Addr, FLASH_SECTOR_SIZE, pbuf);
	if (cmd == 1)
		memcpy((char*)pbuf+8, "81958711", 8);
	else
		memcpy((char*)pbuf+8, "01958711", 8);

	flash_erase_sector(&flash, Part2Addr);
	flash_stream_write(&flash, Part2Addr, FLASH_SECTOR_SIZE, pbuf);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	
#if SWAP_UPDATE
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&flash, Part1Addr, FLASH_SECTOR_SIZE, pbuf);
	if (cmd == 1)
		memcpy((char*)pbuf+8, "01958711", 8);
	else
		memcpy((char*)pbuf+8, "81958711", 8);

	flash_erase_sector(&flash, Part1Addr);
	flash_stream_write(&flash, Part1Addr, FLASH_SECTOR_SIZE, pbuf);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
#endif
	update_free(pbuf);
}

#ifdef HTTP_OTA_UPDATE
/******************************************************************************************************************
** Function Name  : update_ota_http_connect_server
** Description    : connect to the OTA server
** Input          : server_socket: the socket used
**					host: host address of the OTA server
**					port: port of the OTA server
** Return         : connect ok:	socket value
**					Failed:		-1
*******************************************************************************************************************/
int update_ota_http_connect_server(int server_socket, char *host, int port){
	struct sockaddr_in server_addr;
    struct hostent *server;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		return -1;
	}

    server = gethostbyname(host);
    if(server == NULL){ 
        printf("[ERROR] Get host ip failed\n");
		return -1;
    }
    
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr,server->h_addr,server->h_length);
    
    if (connect(server_socket,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("\n\r[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	return server_socket;
}

/******************************************************************************************************************
** Function Name  : parse_http_response
** Description    : Parse the http response to get some useful parameters
** Input          : response	: The http response got from server
**					response_len: The length of http response
**					result		: The struct that store the usful infor about the http response
** Return         : Parse OK:	1 -> Only got the status code
**								3 -> Got the status code and content_length, but didn't get the full header							
**								4 -> Got all the information needed
**					Failed:		-1
*******************************************************************************************************************/
int parse_http_response(uint8_t *response, uint32_t response_len, http_response_result_t *result) {
    uint32_t i, p, q, m;
    uint32_t header_end = 0;

    //Get status code
	if(0 == result->parse_status){//didn't get the http response
		uint8_t status[4] = {0};
		i = p = q = m = 0;
		for (; i < response_len; ++i) {
			if (' ' == response[i]) {
				++m;
				if (1 == m) {//after HTTP/1.1
					p = i;
				} 
				else if (2 == m) {//after status code
					q = i;
					break;
				}
			}
		}
		if (!p || !q || q-p != 4) {//Didn't get the status code
			return -1;
		}
		memcpy(status, response+p+1, 3);//get the status code
		result->status_code = atoi((char const *)status);
		if(result->status_code == 200)
			result->parse_status = 1;
		else{
			printf("\n\r[%s] The http response status code is %d", __FUNCTION__, result->status_code);
			return -1;
		}
	}

	//if didn't receive the full http header
	if(3 == result->parse_status){//didn't get the http response
		p = q = 0;
		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i+1] == '\n' &&
				response[i+2] == '\r' && response[i+3] == '\n') {//the end of header
				header_end = i+4;
				result->parse_status = 4;
				result->header_len = header_end;
				result->body = response + header_end;
				break;
			}
		}
		if (3 == result->parse_status) {//Still didn't receive the full header	
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen(result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

    //Get Content-Length
	if(1 == result->parse_status){//didn't get the content length
		uint32_t content_length = 0;
		const uint8_t *content_length_buf1 = "CONTENT-LENGTH";
		const uint8_t *content_length_buf2 = "Content-Length";
		const uint32_t content_length_buf_len = strlen(content_length_buf1);
		p = q = 0;
		
		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i+1] == '\n') {
				q = i;//the end of the line
				if (!memcmp(response+p, content_length_buf1, content_length_buf_len) ||
						!memcmp(response+p, content_length_buf2, content_length_buf_len)) {//get the content length
					int j1 = p+content_length_buf_len, j2 = q-1;
					while ( j1 < q && (*(response+j1) == ':' || *(response+j1) == ' ') ) ++j1;
					while ( j2 > j1 && *(response+j2) == ' ') --j2;
					uint8_t len_buf[12] = {0};
					memcpy(len_buf, response+j1, j2-j1+1);
					result->body_len = atoi((char const *)len_buf);
					result->parse_status = 2;
				}
				p = i+2;
			}
			if (response[i] == '\r' && response[i+1] == '\n' &&
					response[i+2] == '\r' && response[i+3] == '\n') {//Get the end of header
				header_end = i+4;//p is the start of the body
				if(result->parse_status == 2){//get the full header and the content length
					result->parse_status = 4;
					result->header_len = header_end;
					result->body = response + header_end;
				}
				else {//there are no content length in header	
					printf("\n\r[%s] No Content-Length in header", __FUNCTION__);
					return -1;
				}
				break;
			}	
		}
		
		if (1 == result->parse_status) {//didn't get the content length and the full header
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen(result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
		else if (2 == result->parse_status) {//didn't get the full header but get the content length
			result->parse_status = 3;
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen(result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}

int http_update_ota(char *host, int port, char *resource)
{
	int server_socket;
	unsigned char *buf, *alloc, *request;
	_file_checksum *file_checksum;
	int read_bytes = 0, i = 0;
	uint32_t address, flash_checksum = 0;
	uint32_t NewImg2Len = 0, NewImg2Addr = 0;
	int ret = -1;
	http_response_result_t rsp_result = {0};

	alloc = update_malloc(BUF_SIZE + 4);
	if(!alloc){
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit;
	}
	buf = &alloc[4];
	file_checksum = (void*)alloc;

	// Connect server
	server_socket = update_ota_http_connect_server(server_socket, host, port);
	if(server_socket == -1){
		goto update_ota_exit;
	}

	NewImg2Addr = update_ota_prepare_addr();
	if(NewImg2Addr == -1){
		goto update_ota_exit;
	}

	// reset
	file_checksum->u = 0;
	
	// Write New Image 2 sector
	if(NewImg2Addr != ~0x0){
		uint32_t idx = 0;
		int data_len = 0;
		printf("\n\r");
		
		//send http request
		request = (unsigned char *) update_malloc(strlen("GET /") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ") 
			+ strlen(host) + strlen("\r\n\r\n") + 1);
		sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);

		ret = write(server_socket, request, strlen(request));
		if(ret < 0){
			printf("\n\r[%s] Send HTTP request failed", __FUNCTION__);
			goto update_ota_exit;
		}

		while (3 >= rsp_result.parse_status){//still read header
			if(0 == rsp_result.parse_status){//didn't get the http response
				memset(buf, 0, BUF_SIZE);
				read_bytes = read(server_socket, buf, BUF_SIZE);
				if(read_bytes == 0) 
					continue;
				if(read_bytes < 0){
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}
	
				idx = read_bytes;
				memset(&rsp_result, 0, sizeof(rsp_result));
				if(parse_http_response(buf, idx, &rsp_result) == -1){
					goto update_ota_exit;
				}
			}
			else if((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)){//just get the status code
				memset(buf, 0, BUF_SIZE);
				memcpy(buf, rsp_result.header_bak, HEADER_BAK_LEN);
				update_free(rsp_result.header_bak);
				rsp_result.header_bak = NULL;
				read_bytes = read(server_socket, buf+ HEADER_BAK_LEN, (BUF_SIZE - HEADER_BAK_LEN));
				if(read_bytes == 0) 
					continue;
				if(read_bytes < 0){
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}
	
				idx = read_bytes + HEADER_BAK_LEN;

				if(parse_http_response(buf, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1){
					goto update_ota_exit;
				}
			}
			else if(3 == rsp_result.parse_status){
				printf("\n\r[%s] Get the content_length failed", __FUNCTION__);
				goto update_ota_exit;
			}
		}
		
		if (0 == rsp_result.body_len){
			printf("\n\r[%s] New firmware size = 0 !", __FUNCTION__);
			goto update_ota_exit;
		}
		else
			printf("\n\r[%s] Download new firmware begin, total size : %d\n\r", __FUNCTION__, rsp_result.body_len);

#if SWAP_UPDATE
		NewImg2Addr = update_ota_swap_addr(rsp_result.body_len, NewImg2Addr);
		if(NewImg2Addr == -1){
			goto update_ota_exit;
		}	
#endif
		address = NewImg2Addr;		
		NewImg2Len = update_ota_erase_upg_region(rsp_result.body_len, NewImg2Len, NewImg2Addr);
		if(NewImg2Len == -1){
			goto update_ota_exit;
		}

		//Write the body of http response into flash
		data_len = idx - rsp_result.header_len;	
		if(data_len > 0){
			file_checksum->c[0] = alloc[4+data_len-4];	  // checksum attached at file end
			file_checksum->c[1] = alloc[4+data_len-3];
			file_checksum->c[2] = alloc[4+data_len-2];
			file_checksum->c[3] = alloc[4+data_len-1];
	
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if(flash_stream_write(&flash_ota, address, data_len, (buf+rsp_result.header_len)) < 0){
				printf("\n\r[%s] Write sector failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
		}
		
		idx = 0;
		idx += data_len;
		while (idx < NewImg2Len){
			printf(".");
			data_len = NewImg2Len - idx;
			if(data_len > BUF_SIZE)
				data_len = BUF_SIZE;

			memset(buf, 0, BUF_SIZE);
			read_bytes = read(server_socket, buf, data_len);		
			if(read_bytes == 0)
				continue;
			if(read_bytes < 0){
				printf("\n\r[%s] Read socket failed", __FUNCTION__);
				goto update_ota_exit;
			}	

			if(read_bytes<4)
				printf("\n\r[%s] Recv small packet", __FUNCTION__);

			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if(flash_stream_write(&flash_ota, address + idx, read_bytes, buf) < 0){
				printf("\n\r[%s] Write sector failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);

			file_checksum->c[0] = alloc[4+read_bytes-4];      // checksum attached at file end
			file_checksum->c[1] = alloc[4+read_bytes-3];
			file_checksum->c[2] = alloc[4+read_bytes-2];
			file_checksum->c[3] = alloc[4+read_bytes-1];

			idx += read_bytes;			
		}
		printf("\n\r[%s] Download new firmware %d bytes completed\n\r", __FUNCTION__, idx);

		// read flash data back and calculate checksum
		for(i = 0; i < idx-4; i += BUF_SIZE){
			int k;
			int rlen = (idx-4-i)>BUF_SIZE?BUF_SIZE:(idx-4-i);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash_ota, NewImg2Addr+i, rlen, buf);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			for(k = 0; k < rlen; k++)
				flash_checksum+=buf[k];
		}

		ret = update_ota_checksum(file_checksum, flash_checksum, NewImg2Addr);
		if(ret == -1){
			printf("\n\r[%s] The checksume is wrong!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
	}
update_ota_exit:
	if(alloc)
		update_free(alloc);
	if(request)
		update_free(request);
	if(server_socket >= 0)
		close(server_socket);
	return ret;
}
#endif

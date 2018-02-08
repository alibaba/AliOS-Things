#ifndef OTA_8195A_H
#define OTA_8195A_H

#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <flash_api.h>
#include <lwip/sockets.h>

/************************Related setting****************************/
#define HTTP_OTA_UPDATE			//if define, using http protocol, if not, will use socket
#define CONFIG_CUSTOM_SIGNATURE		0	//if verify the custom signature(define in ota_8195a.c cus_sig)
#define WRITE_OTA_ADDR				1
#define SWAP_UPDATE					0


#define BUF_SIZE		512
#define HEADER_BAK_LEN	32

#define OFFSET_DATA		FLASH_SYSTEM_DATA_ADDR
#define IMAGE_2			0x0000B000
#if WRITE_OTA_ADDR
#define BACKUP_SECTOR	(FLASH_SYSTEM_DATA_ADDR - 0x1000)
#endif
/*******************************************************************/


/****************Define the structures used*************************/
typedef struct{
	uint32_t	ip_addr;
	uint16_t	port;
}update_cfg_local_t;

typedef struct {
	uint32_t	status_code;
	uint32_t	header_len;
	uint8_t		*body;
	uint32_t	body_len;
	uint8_t		*header_bak;
	uint32_t	parse_status;
} http_response_result_t;

typedef union { 
	uint32_t u; 
	unsigned char c[4]; 
} _file_checksum;
/*******************************************************************/


/****************General functions used by ota update***************/
void *update_malloc(unsigned int size);
void update_free(void *buf);
void ota_platform_reset(void);
#if WRITE_OTA_ADDR
int write_ota_addr_to_system_data(flash_t *flash, uint32_t ota_addr);
#endif
int update_ota_connect_server(int server_socket, update_cfg_local_t *cfg);
uint32_t update_ota_prepare_addr(void);
#if SWAP_UPDATE
uint32_t update_ota_swap_addr(uint32_t img_len, uint32_t NewImg2Addr);
#endif
int update_ota_erase_upg_region(uint32_t img_len, uint32_t NewImg2Len, uint32_t NewImg2Addr);
int update_ota_checksum(_file_checksum *file_checksum, uint32_t flash_checksum, uint32_t NewImg2Addr);
/*******************************************************************/


/*******************Functions called by AT CMD**********************/
void cmd_update(int argc, char **argv);
void cmd_ota_image(bool cmd);
/*******************************************************************/


/*************************************************************************************************
** Function Name  : update_ota_local
** Description    : Starting a thread of OTA updating through socket
** Input          : ip:The IP address of OTA server
**					port:The Port of OTA server
** Return         : 0: Task created OK
**					-1: Task created failed
**************************************************************************************************/
int update_ota_local(char *ip, int port);


#ifdef HTTP_OTA_UPDATE
int parse_http_response(uint8_t *response, uint32_t response_len, http_response_result_t *result);
int update_ota_http_connect_server(int server_socket, char *host, int port);

/*************************************************************************************************
** Function Name  : http_update_ota
** Description    : The process of OTA updating through http protocol
** Input          : cfg:struct update_cfg_local_t
** Return         : NULL
**************************************************************************************************/
int http_update_ota(char *host, int port, char *resource);
#endif

#endif

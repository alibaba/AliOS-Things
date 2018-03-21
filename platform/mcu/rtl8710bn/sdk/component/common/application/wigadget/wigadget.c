#include "FreeRTOS.h"
#include "task.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "sockets.h"
#include <mDNS/mDNS.h>
#include <lwip_netconf.h>
#include <lwip/netif.h>
#include "flash_api.h"
#include "encrypt.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "cJSON.h"
#include "cloud_link.h"
#include "wigadget.h"
#include "shtc1.h"

#define PORT		6866
#define MAX_BUFFER_SIZE         256
#define ENC_SIZE          64
#define CONTROL_TYPE		1
#ifdef CONFIG_PLATFORM_8195A
#define GPIO_SOFTAP_RESET_PIN		PC_4
#endif

#ifdef CONFIG_PLATFORM_8711B
#define GPIO_SOFTAP_RESET_PIN		PA_0
#endif

flash_t iot_flash;
uint8_t aes_key[16];
static unsigned char tx_buffer[MAX_BUFFER_SIZE];
static unsigned char rx_buffer[MAX_BUFFER_SIZE];

extern struct netif xnetif[NET_IF_NUM];

#define DEBUG_IOT   1

#define IOT_LOG(level, fmt, ...) printf("\n\r[IOT %s] %s: " fmt "\n", level, __FUNCTION__, ##__VA_ARGS__)
#if DEBUG_IOT == 2
#define IOT_DEBUG(fmt, ...) IOT_LOG("DEBUG", fmt, ##__VA_ARGS__)
#else
#define IOT_DEBUG(fmt, ...)
#endif
#if DEBUG_IOT
#define IOT_ERROR(fmt, ...) IOT_LOG("ERROR", fmt, ##__VA_ARGS__)
#else
#define IOT_ERROR(fmt, ...)
#endif

void encrypt_data_aes(unsigned char *plaint_text, unsigned char *enc_data);
void decrypt_data_aes(unsigned char *enc_data, unsigned char *dec_data, int data_len);



static unsigned int arc4random(void)
{
	unsigned int res = xTaskGetTickCount();
	static unsigned int seed = 0xDEADB00B;

	seed = ((seed & 0x007F00FF) << 7) ^
		((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
		(res << 13) ^ (res >> 9);    // using the clock too!
	return seed;

}

static char *iot_itoa(int value)
{
	char *val_str;
	int tmp = value, len = 1;

	while((tmp /= 10) > 0)
		len ++;

	val_str = (char *) malloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

 void gen_json_data(char *i, char *j, unsigned char *json_data)
{
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);
	memset(json_data, 0, ENC_SIZE);

	
	cJSON *IOTJSObject = NULL;
	char *data;

	
	if((IOTJSObject = cJSON_CreateObject()) != NULL) {

		cJSON_AddItemToObject(IOTJSObject, "TEM", cJSON_CreateString(i));
		cJSON_AddItemToObject(IOTJSObject, "HUM", cJSON_CreateString(j));

		data = cJSON_Print(IOTJSObject);
		memcpy(json_data, data, strlen(data));
		cJSON_Delete(IOTJSObject);
		free(data);
	}

}

void encrypt_data_aes(unsigned char *plaint_text, unsigned char *enc_data)
{
	unsigned char iv[16] = {0};
	unsigned char* iv_bak = "AAAAAAAAAAAAAAAA";
	aes_encrypt_ctx enc_ctx;
	
	memset(&enc_ctx, 0, sizeof(enc_ctx));
	memset(iv, 0, sizeof(iv));
	memcpy(iv, iv_bak, sizeof(iv));
	memset(enc_data, 0, sizeof(enc_data));

	aes_init();
	aes_encrypt_key(aes_key, 16, &enc_ctx);
	aes_cbc_encrypt(plaint_text, enc_data, ENC_SIZE, iv, &enc_ctx);
}

void decrypt_data_aes(unsigned char *enc_data, unsigned char *dec_data, int data_len)
{
	unsigned char iv[16] = {0};
	unsigned char* iv_bak = "AAAAAAAAAAAAAAAA";
	aes_decrypt_ctx dec_ctx;

	memset(&dec_ctx, 0, sizeof(dec_ctx));
	memset(iv, 0, sizeof(iv));
	memcpy(iv, iv_bak, sizeof(iv));
	memset(dec_data, 0, sizeof(dec_data));

	aes_init();
	aes_decrypt_key(aes_key, 16, &dec_ctx);
	aes_cbc_decrypt(enc_data, dec_data, data_len, iv, &dec_ctx);
	IOT_DEBUG("Decrypt data: %s\r\n",dec_data);
}

void iotapp_platform_reset(void)
{
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | 
	                              (1 << 2));
	while(1) osDelay(1000);
}

void iotapp_reset_irq_handler(uint32_t id, gpio_irq_event event) 
{
	printf("\n\r\n\r\n\r\n\r<<<<<<Reset the device>>>>>>>\n\r\n\r\n\r\n\r");
	flash_erase_sector(&iot_flash, FLASH_IOT_DATA);	
	iotapp_platform_reset();
}

int local_link(unsigned char *tx_data) 
{
	int sockfd, newsockfd;
	socklen_t client;
	struct sockaddr_in serv_addr, cli_addr;
	uint8_t rx_data[ENC_SIZE];
	unsigned char enc_data[ENC_SIZE];
	unsigned char dec_data[ENC_SIZE];
	int ret = 0, opt = 1, k = 1, j;
        char *result = NULL, *backup = NULL;
        unsigned char *data = NULL;
	char *delims = ", ";

	sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0) {
		IOT_ERROR("ERROR opening socket");
		ret = -1;
		goto exit2;
	}

	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))) < 0){
		IOT_ERROR("ERROR on setting socket option");
		ret = -1;
		goto exit2;
	} 
	
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
     
	if (bind(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
		IOT_ERROR("ERROR on binding");
		ret = -1;
		goto exit2;
	}
	if(listen(sockfd , 20) < 0){
		IOT_ERROR("ERROR on listening");
		ret = -1;
		goto exit2;
	}		     
	client = sizeof(cli_addr);
	if((newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&client)) < 0){
		IOT_ERROR("ERROR on accept");
		ret = -1;
		goto exit;
	}
	if ((ret = read(newsockfd,rx_buffer,sizeof(rx_buffer))) < 0){
		IOT_ERROR("ERROR reading from socket");
		ret = -1;		
		goto exit;
	}	     
	IOT_DEBUG("cmd received: %s, length: %d\r\n",rx_buffer, ret);
	
//Changing received data to string			
	if (!strncmp(rx_buffer, "[", strlen("["))){
		data = rx_buffer + strlen("[");
		for(j = 1; j < 5; j++){
			if (data[ret - j] == ']')
				data[ret -j] = '\0';
		}
	}
	else
		strcpy(data, rx_buffer);											
	memset(rx_data, 0, sizeof(rx_data));
	result = strtok_r(data, delims, &backup);						
	rx_data[0]=(uint8_t)atoi(result);
	while((result = strtok_r(NULL, delims, &backup)) != NULL)
		rx_data[k++]=(uint8_t)atoi(result);
	memset(dec_data, 0, sizeof(dec_data));	
	
//Decrpyt the received data
	decrypt_data_aes(rx_data, dec_data, 16);	

	if(strncmp(dec_data, "request", strlen("request")) == 0){
//Encrypt the sending data
		memset(enc_data, 0, strlen(enc_data));
		encrypt_data_aes(tx_data, enc_data);
//Changing encrpyt data to JAVA type string
		for (j = 0; j < ENC_SIZE; j++){
			char *temp;
			temp = iot_itoa(enc_data[j]);
			if(j == 0)
				strcpy(tx_buffer, "[");
			strcat(tx_buffer,temp);
			if (j == (ENC_SIZE - 1))
				strcat(tx_buffer,"]");
			else
				strcat(tx_buffer,",");
			free(temp);
			temp = NULL;		
		}
		IOT_DEBUG("Data reply to APP: %s\r\nLength of data: %d\r\n", tx_buffer, strlen(tx_buffer));
			
		if ((ret = write(newsockfd,tx_buffer,strlen(tx_buffer))) < 0){
			IOT_ERROR("ERROR writing to socket");
			ret = -1;	
			goto exit;
		}
		else
			IOT_DEBUG("Sending %d bytes data OK!\r\n", ret);
	}
	else if(strncmp(dec_data, "remove", strlen("remove")) == 0){
		printf("\n\r\n\r\n\r\n\r<<<<<<Reset the device >>>>>>>\n\r\n\r\n\r\n\r");
		flash_erase_sector(&iot_flash, FLASH_IOT_DATA);
		write(newsockfd,"Remove OK",strlen("Remove OK"));
		close(newsockfd);
		close(sockfd);
		iotapp_platform_reset();
	}
	else{
		IOT_ERROR("ERROR wrong KEY or wrong request!");
		write(newsockfd,"The KEY or the request is not correct!",strlen("The KEY or the request is not correct!"));
		ret = -1;	
		goto exit;
	}
		 
exit:   
	if(close(newsockfd) != 0)
		goto exit;

exit2:
	if(close(sockfd) != 0)
		goto exit2;
	return ret; 
}

static void local_link_task(void *param)
{
	unsigned char data[ENC_SIZE] = {0};
	vTaskDelay(1000);
        char i[16], j[16];
	float temperature = 1.123f;
	float humidity = 2.456f;
	int ret = 0;

	while(1){
			memset(i, 0, 16);
		        memset(j, 0, 16);
#if PSEUDO_DATA
			sprintf(i,"%.2f", temperature++);
			sprintf(j, "%.2f", humidity++);
                        if(temperature > 60)
                          temperature = 1.123f;
                        if(humidity > 98)
                          humidity = 2.456f;   
#else
			ret = SHTC_GetTempAndHumi(&temperature, &humidity);
			sprintf(i,"%.2f", temperature);
			sprintf(j, "%.2f", humidity);
#endif
			if(ret < 0)
				printf("\r\n\r\n<-----LOCAL LINK FAILED!!(get infor failed)\r\n\r\n");
			else{
				printf("\r\n\r\n----->START LOCAL LINKING\r\n\r\n");
				gen_json_data(i, j, data);
				printf("Sending data : %s\r\n", data);
				if (local_link(data) < 0)
					printf("\r\n\r\n<-----LOCAL LINK FAILED!!\r\n\r\n");
				else
					printf("\r\n\r\n<-----LOCAL LINK OK!!\r\n\r\n");
				vTaskDelay(1000);
			}
	}
}

void start_local_link(void)
{
	if(xTaskCreate(local_link_task, ((const char*)"local_link_task"), 5376, NULL, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
 		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
}
int pair_device(unsigned char *tx_buffer, unsigned char *rx_buffer, int handshake)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int ret = 0;
	int opt = 1;
  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);     
	if (sockfd < 0) {
		IOT_ERROR("ERROR opening socket");
		ret = -1;
		goto exit;
	}

	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))) < 0){
		IOT_ERROR("ERROR on setting socket option");
		ret = -1;
		goto exit;
	} 
	
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_len = sizeof(serv_addr);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	
	if ((bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
		IOT_ERROR("ERROR on binding");
		ret = -1;
		goto exit;
	}   
	if ((listen(sockfd, 5)) < 0){    
		IOT_ERROR("ERROR on listening tcp server socket fd");
		ret = -1;
		goto exit;
	}   
	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);    
	if (newsockfd < 0) {
		IOT_ERROR("ERROR on accept");
	   	ret = -1;
		goto exit2;
	}
	ret = read(newsockfd, rx_buffer, MAX_BUFFER_SIZE);     
	if (ret <= 0){
		IOT_ERROR("ERROR reading from socket");
		ret = -1;
		goto exit2;
	}
	IOT_DEBUG("Request received: %s, byte: %d\r\n",rx_buffer, ret);	
	if(handshake == 1){
		if(strncmp(rx_buffer,"PAIR", strlen("PAIR")) != 0){
			write(newsockfd, "ERROR", strlen("ERROR"));
			IOT_ERROR("ERROR on first handshake!");
	   		ret = -1;
			goto exit2;
		}
	}
	else if(handshake == 2){
		if((rx_buffer == NULL) ||(strlen(rx_buffer) < 32)){
			write(newsockfd, "ERROR", strlen("ERROR"));
			IOT_ERROR("ERROR on second handshake!");
	   		ret = -1;
			goto exit2;
		}
	}
	else if(handshake == 3){
		unsigned char account[64];
		unsigned char enc_acc[64];
	        char *result = NULL, *backup = NULL;
                unsigned char *data = NULL;
		char *delims = ", ";
		int j, k = 1;
		if (!strncmp(rx_buffer, "[", strlen("["))){
			data = rx_buffer + strlen("[");
			for(j = 1; j < 5; j++){
				if (data[ret - j] == ']')
					data[ret -j] = '\0';
			}
		}
		else
			strcpy(data, rx_buffer);	
		memset(enc_acc, 0, sizeof(enc_acc));
		result = strtok_r(data, delims, &backup);						
		enc_acc[0]=(uint8_t)atoi(result);
		while((result = strtok_r(NULL, delims, &backup)) != NULL)
			enc_acc[k++]=(uint8_t)atoi(result);
		IOT_DEBUG("The value of k: %d", k);
		memset(account, 0, sizeof(account));	
		decrypt_data_aes(enc_acc, account, k);

		if((strncmp(account,"https://", strlen("https://"))) != 0){
			write(newsockfd, "ERROR", strlen("ERROR"));
			IOT_ERROR("ERROR on third handshake!");
	   		ret = -1;
			goto exit2;
		}
		else{
			IOT_DEBUG("The received Firebase URL:%s", account);
			memset(rx_buffer, 0, strlen(rx_buffer));
			memcpy(rx_buffer, (account+strlen("https://")), (strlen(account) + strlen("https://")));
		}
	}
	ret = write(newsockfd, tx_buffer, strlen(tx_buffer));
	IOT_DEBUG("Data send: %s\r\n",tx_buffer);
     
	 if (ret < 0){
		IOT_ERROR("ERROR writing to socket");
	}
     	
exit:   
	if(close(newsockfd) != 0)
		goto exit;

exit2:
	if(close(sockfd) != 0)
		goto exit2;
	return ret; 

}

static void pair_device_task(void)
{
	int i, j, k, HANDSHAKE;
	uint8_t PAIR_STATE[1] = {0};
	
	if(CONTROL_TYPE == 1){
		printf("\r\n\r\n<<<<<<CONTROL_TYPE = 1  Need 3 times handshake>>>>>>\r\n\r\n");
		HANDSHAKE = 3;				
	}
	else{
		printf("\r\n\r\n<<<<<<CONTROL_TYPE = 0  Need 2 times handshake>>>>>>\r\n\r\n");
		HANDSHAKE = 2;
	}
	printf("\r\n\r\n=========>PAIR_STATE = 0  Start to pair\r\n\r\n");
	for(i = 0; i < HANDSHAKE; i++){
		static const uint8_t basepoint[32] = {9};
		uint8_t mysecret[32];
		uint8_t mypublic[32];
		uint8_t theirpublic[32] = {0};
		uint8_t shared_key[32];
//First handshake  
		if(i == 0){
			printf("\r\n\r\n===>Start the first handshake\r\n\r\n");
			memset(tx_buffer, 0, sizeof(tx_buffer));
			memset(rx_buffer, 0, sizeof(rx_buffer));
			for(j = 0; j < 32; j ++) 
				mysecret[j] = (uint8_t) arc4random();  
			mysecret[j] = '\0';
			curve25519_donna(mypublic, mysecret, basepoint);
			for (j = 0; j < 32; j++){
				char *temp;
				temp = iot_itoa(mypublic[j]);
				if(j == 0)
					strcpy(tx_buffer, "[");
				strcat(tx_buffer,temp);
				if (j == 31)
					strcat(tx_buffer,"]");
				else
					strcat(tx_buffer,",");
				free(temp);
				temp = NULL;
			}
			if(pair_device(tx_buffer, rx_buffer, 1) >= 0)
				printf("\r\n\r\n<===First handshake OK!!\r\n\r\n");
                    	else{
				i--;
				printf("\r\n\r\n<===First handshake FAILED!!\r\n\r\n");
                        }
		}
//Second handshake  
		if(i == 1){
			printf("\r\n\r\n=====>Start the second handshake\r\n\r\n");
			vTaskDelay(200);
			memset(tx_buffer, 0, sizeof(tx_buffer));
			if(CONTROL_TYPE == 1)
				memcpy(tx_buffer, "FIREBASE URL", sizeof("FIREBASE URL"));
			else
				memcpy(tx_buffer, "PAIR OK", sizeof("PAIR OK"));
			memset(rx_buffer, 0, sizeof(rx_buffer));

			if(pair_device(tx_buffer, rx_buffer, 2) >= 0){
				char *result = NULL, *backup = NULL;
                                unsigned char *data = NULL;
				char *delims = ", ";
				k = 1;				
				if (!strncmp(rx_buffer, "[", strlen("["))){
					data = rx_buffer + strlen("[");
					int len;
					len = strlen(data);
					for(j = 1; j < 5; j++){
						if (data[len - j] == ']')
							data[len -j] = '\0';
					}
				}
				else
					strcpy(data, rx_buffer);
												
				memset(theirpublic, 0, sizeof(theirpublic));

				result = strtok_r(data, delims, &backup);						
				theirpublic[0]=(uint8_t)atoi(result);

				while((result = strtok_r(NULL, delims, &backup)) != NULL)
					theirpublic[k++] = (uint8_t)atoi(result);
							
				curve25519_donna(shared_key, mysecret, theirpublic);	
				for(j = 0; j < 16; j ++)
					aes_key[j] = shared_key[j];						
//Store the KEY in FLASH
				if(CONTROL_TYPE == 0){
					PAIR_STATE[0] = 1;
                                        uint8_t data[33];
                                        memset(data, 0, 33);
					memcpy(data, PAIR_STATE, 1);
					memcpy(data+1, shared_key, 32);
					flash_erase_sector(&iot_flash, FLASH_IOT_DATA);
		    			flash_stream_write(&iot_flash, FLASH_IOT_DATA, 33, (uint8_t *) data);
					IOT_DEBUG("PAIR_STATE: %d\r\n", PAIR_STATE[0]);
				}
				printf("\r\n\r\n<=====Second handshake OK!!\r\n\r\n");
			}
			else{
				i = i - 2;
				printf("\r\n\r\n<=====Second handshake FAILED!!\r\n\r\n");
			}
		}
//Third handshake
		if(i == 2){
			printf("\r\n\r\n=======>Start the third handshake\r\n\r\n");
			vTaskDelay(200);

			memset(tx_buffer, 0, sizeof(tx_buffer));
			memcpy(tx_buffer, "PAIR OK", sizeof("PAIR OK"));
			memset(rx_buffer, 0, sizeof(rx_buffer));

			if(pair_device(tx_buffer, rx_buffer, 3) >= 0){
				IOT_DEBUG("rx_buffer: %s, sizeof rx_buffer:%d\r\n", rx_buffer, sizeof(rx_buffer));
				PAIR_STATE[0] = 1;
				uint8_t data[97];
				memset(data, 0, 97);
				memcpy(data, PAIR_STATE, 1);
				memcpy(data+1, shared_key, 32);
				memcpy(data+33, rx_buffer, 64);
				flash_erase_sector(&iot_flash, FLASH_IOT_DATA);
		    		flash_stream_write(&iot_flash, FLASH_IOT_DATA, 97, (uint8_t *) data);
				IOT_DEBUG("PAIR_STATE: %d\r\n", PAIR_STATE[0]);

				printf("\r\n\r\n<=======Third handshake OK!!\r\n\r\n");
			}
			else{
				i = i - 3;
				printf("\r\n\r\n<=======Third handshake FAILED!!\r\n\r\n");
			}
		}
	}
	printf("\r\n\r\n<=========Pairing OK!!\r\n\r\n");
}

static void mdns_task(void *param)
{
	DNSServiceRef dnsServiceRef = NULL;
	TXTRecordRef txtRecord;
	unsigned char txt_buf[128];
	uint8_t *mac, *ip;
	int j, ret = 0;
	uint8_t *flash_data;
	uint8_t PAIR_STATE[1] = {0};
	static unsigned char MAC_ADD[21];
	static unsigned char IP[16];
	static unsigned char port[6];
	uint16_t shtc1_id;

// Delay to wait for IP by DHCP and get the information of IP and MAC
	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 20 seconds to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	vTaskDelay(20000);
	ip = LwIP_GetIP(&xnetif[0]);
	mac = LwIP_GetMAC(&xnetif[0]);

	sprintf(MAC_ADD, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sprintf(IP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	sprintf(port, "%d", PORT);

	IOT_DEBUG("MAC  => %s\r\n", MAC_ADD) ;
	IOT_DEBUG("IP  => %s\r\n", IP);
	IOT_DEBUG("PORT  => %s\r\n", port);

//Get the value of PAIR_STATE and the AES key in flash
	flash_data = (uint8_t *)malloc(33);
	flash_stream_read(&iot_flash, FLASH_IOT_DATA, 33, (uint8_t *)flash_data);
	memcpy(PAIR_STATE, flash_data, 1);
	if(PAIR_STATE[0] != 0x1)
		PAIR_STATE[0] = 0;
	else{
		for(j = 0;j < 16; j++){
			aes_key[j] = flash_data[j+1];
		}
	}
	free(flash_data);
	IOT_DEBUG("PAIR_STATE now: %d\r\n", PAIR_STATE[0]);

	IOT_DEBUG("=>mDNS Init\r\n");
	if(mDNSResponderInit() == 0) {
		printf("\r\n\r\n========>Start to register mDNS service\r\n\r\n");
//The device not paired before		
		if(PAIR_STATE[0] == 0){	
			TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);

			TXTRecordSetValue(&txtRecord, "IP", strlen(IP), IP);
			TXTRecordSetValue(&txtRecord, "PORT", strlen(port), port);
			TXTRecordSetValue(&txtRecord, "MAC_ADDR", strlen(MAC_ADD), MAC_ADD);			
			TXTRecordSetValue(&txtRecord, "PAIR_STATE", strlen("0"), "0");
			TXTRecordSetValue(&txtRecord, "SERVICE_NAME", strlen("ht_sensor"), "ht_sensor");
			if(CONTROL_TYPE == 1)
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("1"), "1");
			else
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("0"), "0");
			dnsServiceRef = mDNSRegisterService("ht_sensor", "_Ameba._tcp", "local", PORT, &txtRecord);
			TXTRecordDeallocate(&txtRecord);			
			printf("\r\n\r\n<========Registering mDNS service OK!!\r\n\r\n");			
			pair_device_task();
                }
//The device was paired
		else if(PAIR_STATE[0] == 0x1){
			TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);

			TXTRecordSetValue(&txtRecord, "IP", strlen(ip), ip);
			TXTRecordSetValue(&txtRecord, "PORT", strlen(port), port);
			TXTRecordSetValue(&txtRecord, "MAC_ADDR", strlen(MAC_ADD), MAC_ADD);			
			TXTRecordSetValue(&txtRecord, "PAIR_STATE", strlen("1"), "1");
			TXTRecordSetValue(&txtRecord, "SERVICE_NAME", strlen("ht_sensor"), "ht_sensor");
			if(CONTROL_TYPE == 1)
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("1"), "1");
			else
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("0"), "0");
			
			dnsServiceRef = mDNSRegisterService("ht_sensor", "_Ameba._tcp", "local", PORT, &txtRecord);
			TXTRecordDeallocate(&txtRecord);
			printf("\r\n\r\n<========Registering mDNS service OK!! PAIR_STATE = 1\r\n\r\n");	
                }
#if PSEUDO_DATA
	printf("\r\n\r\n========>Using the speudo data\r\n\r\n");
	if(CONTROL_TYPE == 1) start_cloud_link();
		start_local_link();
#else
//Init the shtc1 sensor
	printf("\r\n\r\n========>Init the temperature and humidity sensor\r\n\r\n");
	ret = SHTC_Init(&shtc1_id);
	if ( ret == NO_ERROR ){
		printf("\r\n\r\n<========Senser init OK! ID = 0x%x \r\n\r\n", shtc1_id);
		if(CONTROL_TYPE == 1) start_cloud_link();
			start_local_link();
	}
	else {
		printf("\r\n\r\n<========Senser init FAILED! ID = 0x%x \r\n\r\n", shtc1_id);
	  	ret = -1;
	}
#endif

	}	
	else 
		ret = -1;
	if(ret == 0){
		while(1){
			IOT_DEBUG("Update the mDNS textrecord!\r\n");
			TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);
			TXTRecordSetValue(&txtRecord, "IP", strlen(IP), IP);
			TXTRecordSetValue(&txtRecord, "PORT", strlen(port), port);
			TXTRecordSetValue(&txtRecord, "MAC_ADDR", strlen(MAC_ADD), MAC_ADD);			
			TXTRecordSetValue(&txtRecord, "PAIR_STATE", strlen("1"), "1");
			if(CONTROL_TYPE == 1)
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("1"), "1");
			else
				TXTRecordSetValue(&txtRecord, "CONTROL_TYPE", strlen("0"), "0");
			TXTRecordSetValue(&txtRecord, "SERVICE_NAME", strlen("ht_sensor"), "ht_sensor");
			
			mDNSUpdateService(dnsServiceRef, &txtRecord, 0);
			TXTRecordDeallocate(&txtRecord);
                  	vTaskDelay(2*60*1000);
		}
	}
	else{
		if(dnsServiceRef)
			mDNSDeregisterService(dnsServiceRef);
		IOT_DEBUG("<=mDNS Deinit\r\n\r\n");
		mDNSResponderDeinit();
	}
}

void example_wigadget(void)
{
	if(xTaskCreate(mdns_task, ((const char*)"mdns_task"), 3072, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	
	gpio_t     gpio_softap_reset_button;
	gpio_irq_t gpioirq_softap_reset_button;

	gpio_irq_init(&gpioirq_softap_reset_button, GPIO_SOFTAP_RESET_PIN, iotapp_reset_irq_handler, (uint32_t)(&gpio_softap_reset_button));
	gpio_irq_set(&gpioirq_softap_reset_button, IRQ_FALL, 1);
	gpio_irq_enable(&gpioirq_softap_reset_button);
}


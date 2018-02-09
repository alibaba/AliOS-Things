#include "FreeRTOS.h"
#include "task.h"
#include <netif/etharp.h>
#include "wifi_conf.h"


#define RARP_THREAD_STACK_SIZE 200

const struct eth_addr macbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
extern struct netif xnetif[NET_IF_NUM];

void rarp_retrieve(uint8_t *rarp_ip, uint8_t *rarp_mac) {
	printf("\n\rThe IP of device %02x:%02x:%02x:%02x:%02x:%02x is: %d.%d.%d.%d\r\n", 
		rarp_mac[0], rarp_mac[1], rarp_mac[2], rarp_mac[3], rarp_mac[4], rarp_mac[5], 
		rarp_ip[0], rarp_ip[1], rarp_ip[2], rarp_ip[3]);	
}

static void rarp_thread(void *param)
{	
  	printf("\r\n\r\n\r\n>>>>>>>>>>>>>>>rarp example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
	vTaskDelay(10000);
	uint8_t *mac, *ip, *netmask, *gw;
	err_t result;
	
	while(wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS){ 
		printf("\r\n\r\n\r\n>>>>>>>>>>>>>>Wifi is disconnected!!Please connect!!<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
		vTaskDelay(10000);
	}
		
	rarp_retrieve_hook_callback(rarp_retrieve);
	
	for(int i = 0; i < 3; i ++){
		printf("\n\r\n\retharp_raw: sending raw RARP packet.\n\r\n\r");
		etharp_raw(&xnetif[0], (struct eth_addr *)xnetif[0].hwaddr, &macbroadcast,
			(struct eth_addr *)xnetif[0].hwaddr, IP_ADDR_ANY, 
			(struct eth_addr *)xnetif[0].hwaddr, IP_ADDR_ANY, RARP_REQUEST);
		vTaskDelay(1000);
	}

    vTaskDelete(NULL);
}

void example_rarp(void){
    if(xTaskCreate(rarp_thread, ((const char*)"rarp_thread"), RARP_THREAD_STACK_SIZE, NULL, 1 , NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(rarp_thread) failed\n\r", __FUNCTION__);
    return;
}

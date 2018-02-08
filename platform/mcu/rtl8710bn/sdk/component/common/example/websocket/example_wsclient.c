#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "websocket/wsclient_api.h"
#include <stdio.h>
#include "example_wsclient.h"

#ifdef USING_SSL
#if CONFIG_USE_POLARSSL 
#include <polarssl/memory.h>
#include <polarssl/ssl.h>

void set_ssl_func(wsclient_context *wsclient){
	wsclient->ssl = ws_malloc(sizeof(ssl_context));
	memset(wsclient->ssl, 0, sizeof(ssl_context));
}
#elif CONFIG_USE_MBEDTLS 
#include <mbedtls/net_sockets.h>
#include <mbedtlS/ssl.h>

void set_ssl_func(wsclient_context *wsclient){        
	wsclient->ssl = ws_malloc(sizeof(mbedtls_ssl_context));
	wsclient->conf = ws_malloc(sizeof(mbedtls_ssl_config));  
	wsclient->ssl_fd = ws_malloc(sizeof(mbedtls_net_context));         
	memset(wsclient->ssl, 0, sizeof(mbedtls_ssl_context));        
	memset(wsclient->conf, 0, sizeof(mbedtls_ssl_config));
	memset(wsclient->ssl_fd, 0, sizeof(mbedtls_net_context));   
}        
#endif
#endif

void handle_message(wsclient_context *wsclient, int data_len)
{
        printf("\r\n>>>>>> Receiving: %s with length: %d\n", wsclient->receivedData, data_len);

	if(strcmp(wsclient->receivedData, "hello") == 0)
		ws_send("world", strlen("world"), 1, wsclient);
	else if (strcmp(wsclient->receivedData, "world") == 0){
		ws_close(wsclient);
		wsclient = NULL;
    }
}

static void example_wsclient_thread(void *param)
{
  	printf("\r\n\r\n\r\n>>>>>>>>>>>>>>>wsclient example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
	vTaskDelay(10000);
	while(wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS){ 
		printf("\r\n\r\n\r\n>>>>>>>>>>>>>>Wifi is disconnected!!Please connect!!<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
		vTaskDelay(10000);
	}

	int ret;
	//wsclient_context *wsclient = create_wsclient("wss://echo.websocket.org", 0, NULL, NULL);
	wsclient_context *wsclient = create_wsclient("wss://sandbox.kaazing.net", 0, "echo", NULL);
	if(wsclient != NULL){

		if(wsclient->use_ssl == 1){
#ifdef USING_SSL                
			set_ssl_func(wsclient);
#else
			printf("\r\nNot Support the wss server!\r\n");
			vTaskDelete(NULL);       
#endif
          }
		ret = ws_connect_url(wsclient);
		if(ret >= 0){
			ws_send("hello", strlen("hello"), 1, wsclient);
			while (ws_getReadyState(wsclient) != CLOSED) {
				ws_dispatch(handle_message);
				ws_poll(0, wsclient);
			}
		}
		else
			printf("\r\nConnect to websocket server failed!\r\n");
	}
	else
		printf("\r\nCreat websocket context failed!\r\n");
	
	vTaskDelete(NULL);
}

void example_wsclient(void)
{
	if(xTaskCreate(example_wsclient_thread, ((const char*)"example_wsclient_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#include "platform_opts.h"
#include "google_nest.h"

static int _random_func(void *p_rng, unsigned char *output, size_t output_len)                                               
{                                                                                                                            
	rtw_get_random_bytes(output, output_len);                                                                                  
	return 0;                                                                                                                  
}   

static char *gn_itoa(int value){
	char *val_str;
	int tmp = value, len = 1;

	while((tmp /= 10) > 0)
		len ++;

	val_str = (char *) pvPortMalloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

#if CONFIG_USE_POLARSSL
#include <polarssl/net.h>
#include <polarssl/ssl.h>
#include <polarssl/error.h>
#include <polarssl/memory.h>

int gn_tls_connect(googlenest_context *googlenest, int port){
	int ret;        
	ssl_context *ssl = &googlenest->ssl;
        
        memory_set_own(pvPortMalloc, vPortFree);
	googlenest->socket = -1;
        
//SSL connect
	if((ret = net_connect(&googlenest->socket, googlenest->host, port)) != 0) {
		printf("net_connect ret(%d)\r\n", ret);
		goto exit;
	}

//SSL configuration
	if((ret = ssl_init(ssl)) != 0) {
		printf("ssl_init ret(%d)\r\n", ret);
		goto exit;
	}

	ssl_set_endpoint(ssl, SSL_IS_CLIENT);
	ssl_set_authmode(ssl, SSL_VERIFY_NONE);
	ssl_set_rng(ssl, _random_func, NULL);
	ssl_set_bio(ssl, net_recv, &googlenest->socket, net_send, &googlenest->socket);
	
//SSL handshake
	if((ret = ssl_handshake(ssl)) != 0) {
		printf("ssl_handshake ret(-0x%x)\r\n", -ret);
		goto exit;
	}
	return 0;

exit:
#ifdef POLARSSL_ERROR_C
	if(ret != 0) {
		char error_buf[100];
		polarssl_strerror(ret, error_buf, 100);
		printf("\n\rLast error was: %d - %s\n", ret, error_buf);
	}
#endif
	if(googlenest->socket != -1) {
		net_close(googlenest->socket);
		googlenest->socket = -1;
        ret = -1 ;
	}
	ssl_free(ssl);
	return ret;	
}

void gn_tls_close(googlenest_context *googlenest){
	if(googlenest->socket != -1) {
		net_close(googlenest->socket);
		googlenest->socket = -1;
	}
	ssl_free(&googlenest->ssl);	
}

int gn_tls_write(googlenest_context *googlenest, char *request, int request_len){
	ssl_context *ssl = &googlenest->ssl;	
	return ssl_write(ssl, request, request_len);	
}

int gn_tls_read(googlenest_context *googlenest, char *buffer, int buf_len){
	ssl_context *ssl = &googlenest->ssl;
	return ssl_read(ssl, buffer, buf_len);
}

#elif CONFIG_USE_MBEDTLS
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"

static void* _calloc_func(size_t nmemb, size_t size)                                                                         
{                                                                                                                            
	size_t mem_size;                                                                                                           
	void *ptr = NULL;                                                                                                          
                                                                                                                             
	mem_size = nmemb * size;                                                                                                   
	ptr = pvPortMalloc(mem_size);                                                                                              
                                                                                                                             
	if(ptr)                                                                                                                    
		memset(ptr, 0, mem_size);                                                                                                
                                                                                                                             
	return ptr;                                                                                                                
} 

int gn_tls_connect(googlenest_context *googlenest, int port){
	int ret;
	mbedtls_ssl_context *ssl = &googlenest->ssl;
	mbedtls_ssl_config *conf = &googlenest->conf;
 
	mbedtls_platform_set_calloc_free(_calloc_func, vPortFree);
	googlenest->socket.fd = -1;
	char *port_str = gn_itoa(port);
        
//SSL connect
	if((ret = mbedtls_net_connect(&googlenest->socket, googlenest->host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
		printf("net_connect ret(%d)\r\n", ret);
		goto exit;
	}

//SSL configuration
        mbedtls_ssl_init(ssl);
	mbedtls_ssl_config_init(conf);
	mbedtls_ssl_set_bio(ssl, &googlenest->socket,mbedtls_net_send, mbedtls_net_recv, NULL);
	
        if((ret = mbedtls_ssl_config_defaults(conf,
			MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		printf("ssl_config ret(%d)\r\n", ret);
		goto exit;
	}
        
 	mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(conf, _random_func, NULL);       
        
	if((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
		printf("ssl_setup ret(%d)\r\n", ret);
		goto exit;
	}
        
//SSL handshake
	if((ret = mbedtls_ssl_handshake(ssl)) != 0) {
		printf("ssl_handshake ret(-0x%x)\r\n", -ret);
		goto exit;
	}
	return 0;

exit:
#ifdef MBEDTLS_ERROR_C
	if(ret != 0) {
		char error_buf[100];
		mbedtls_strerror(ret, error_buf, 100);
		printf("\n\rLast error was: %d - %s\n", ret, error_buf);
	}
#endif
	if(googlenest->socket.fd != -1) {
		mbedtls_net_free(&googlenest->socket);
		googlenest->socket.fd = -1;
        ret = -1 ;
	}
	mbedtls_ssl_free(ssl);
	mbedtls_ssl_config_free(conf);
	return ret;
}

void gn_tls_close(googlenest_context *googlenest){
	if(googlenest->socket.fd != -1) {
		mbedtls_net_free(&googlenest->socket);
                mbedtls_ssl_config_free(&googlenest->conf);
		googlenest->socket.fd = -1;
	}
	mbedtls_ssl_free(&googlenest->ssl);
        mbedtls_ssl_config_free(&googlenest->conf);
}

int gn_tls_write(googlenest_context *googlenest, char *request, int request_len){
        mbedtls_ssl_context *ssl = &googlenest->ssl;
        return mbedtls_ssl_write(ssl, request, request_len);
}

int gn_tls_read(googlenest_context *googlenest, char *buffer, int buf_len){
        mbedtls_ssl_context *ssl = &googlenest->ssl;
        return mbedtls_ssl_read(ssl, buffer, buf_len);
}
#endif /*CONFIG_USE_POLARSSL*/
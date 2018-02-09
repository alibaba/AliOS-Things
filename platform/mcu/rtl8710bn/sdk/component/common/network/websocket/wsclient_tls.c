#include "platform_opts.h"
#include <websocket/libwsclient.h>

#if CONFIG_USE_POLARSSL
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include <polarssl/memory.h>

int ws_random(void *p_rng, unsigned char *output, size_t output_len);

int wss_tls_connect(wsclient_context *wsclient){
	int ret;
        
	memory_set_own(pvPortMalloc, vPortFree);
	if((ret = net_connect(&wsclient->sockfd, wsclient->host, wsclient->port)) != 0)
          return ret;

        return 0;
}

int wss_tls_handshake(wsclient_context *wsclient){
	int ret;
        
	if((ret = ssl_init(wsclient->ssl)) != 0)
           return ret;
		
	ssl_set_endpoint(wsclient->ssl, 0);
	ssl_set_authmode(wsclient->ssl, 0);
	ssl_set_rng(wsclient->ssl, ws_random, NULL);
	ssl_set_bio(wsclient->ssl, net_recv, &wsclient->sockfd, net_send, &wsclient->sockfd);
        
	if((ret = ssl_handshake(wsclient->ssl)) != 0)
          return ret;
        
	return 0;
}


void wss_tls_close(wsclient_context *wsclient){
	if(wsclient->sockfd != -1) {
		net_close(wsclient->sockfd);
		wsclient->sockfd = -1;
	}
	if(wsclient->ssl)
		ssl_free(wsclient->ssl);
}

int wss_tls_write(ssl_context *ssl, char *request, int request_len){
        return ssl_write(ssl, request, request_len);
}

int wss_tls_read(ssl_context *ssl, char *buffer, int buf_len){
        return ssl_read(ssl, buffer, buf_len);
}

#elif CONFIG_USE_MBEDTLS /* CONFIG_USE_POLARSSL */

#include <mbedTLS/ssl.h>
#include <mbedtls/net_sockets.h>

int ws_random(void *p_rng, unsigned char *output, size_t output_len);

static void* my_calloc(size_t nelements, size_t elementSize){
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}

static char *ws_itoa(int value){
	char *val_str;
	int tmp = value, len = 1;

	while((tmp /= 10) > 0)
		len ++;

	val_str = (char *) pvPortMalloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

int wss_tls_connect(wsclient_context *wsclient){
        int ret;
        mbedtls_net_context *ws_fd = wsclient->ssl_fd;

        mbedtls_net_init(ws_fd);        
	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);
 
	char *ws_port_str = ws_itoa (wsclient->port);	
	if((ret = mbedtls_net_connect(ws_fd, wsclient->host, ws_port_str, MBEDTLS_NET_PROTO_TCP)) != 0)
          return ret;
             
        wsclient->sockfd = ws_fd->fd;
        return 0;
}

int wss_tls_handshake(wsclient_context *wsclient){
	int ret;
        mbedtls_net_context *ws_fd = wsclient->ssl_fd;

        mbedtls_ssl_init(wsclient->ssl);
	mbedtls_ssl_config_init(wsclient->conf);
        
 	mbedtls_ssl_set_bio(wsclient->ssl, ws_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
        
 	if((ret = mbedtls_ssl_config_defaults(wsclient->conf,
			MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
            return ret;
	
	mbedtls_ssl_conf_authmode(wsclient->conf, MBEDTLS_SSL_VERIFY_NONE);	
	mbedtls_ssl_conf_rng(wsclient->conf, ws_random, NULL);
  
	if((ret = mbedtls_ssl_setup(wsclient->ssl, wsclient->conf)) != 0) 
            return ret;         
        
	if((ret = mbedtls_ssl_handshake(wsclient->ssl)) != 0)
            return ret;
        
        wsclient->sockfd = ws_fd->fd;
        return 0;
}

void wss_tls_close(wsclient_context *wsclient){
        mbedtls_net_context ws_sockfd;
        ws_sockfd.fd = wsclient->sockfd;
        
        if(ws_sockfd.fd != -1) {
          mbedtls_net_free(&ws_sockfd);
          (*wsclient).sockfd= -1;
				}
        mbedtls_net_free(&ws_sockfd);
        mbedtls_ssl_free(wsclient->ssl);
        mbedtls_ssl_config_free(wsclient->conf);       
}

int wss_tls_write(mbedtls_ssl_context *ssl, char *request, int request_len){
        return mbedtls_ssl_write(ssl, request, request_len);
}

int wss_tls_read(mbedtls_ssl_context *ssl, char *buffer, int buf_len){
        return mbedtls_ssl_read(ssl, buffer, buf_len);
}

#endif /* CONFIG_USE_POLARSSL */
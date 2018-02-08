#ifndef _GOOGLENEST_H_
#define _GOOGLENEST_H_

#include "platform_opts.h"

#define BUFFER_SIZE       512

#if CONFIG_USE_POLARSSL
#include <polarssl/ssl.h>
typedef struct {
	int socket;
	char *host;
	ssl_context ssl;      
} googlenest_context;

#elif CONFIG_USE_MBEDTLS
#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
typedef struct {
	mbedtls_net_context socket;
	char *host;
	mbedtls_ssl_context ssl;     
        mbedtls_ssl_config conf;
} googlenest_context;
#endif

int gn_connect(googlenest_context *googlenest, char *host, int port);
void gn_close(googlenest_context *googlenest);
int gn_put(googlenest_context *googlenest, char *uri, char *content);
int gn_patch(googlenest_context *googlenest, char *uri, char *content);
int gn_post(googlenest_context *googlenest, char *uri, char *content, unsigned char *out_buffer, size_t out_len);
int gn_get(googlenest_context *googlenest, char *uri, unsigned char *out_buffer, size_t out_len);
int gn_delete(googlenest_context *googlenest, char *uri);
int gn_stream(googlenest_context *googlenest, char *uri);
void google_retrieve_data_hook_callback(void (*callback)(char *));

#endif
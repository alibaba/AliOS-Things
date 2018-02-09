#ifndef EASYWSCLIENT_H
#define EASYWSCLIENT_H
#include <platform/platform_stdlib.h>

/****************Define the debug message level*********************/
#define DEBUG_WSCLIENT    1

#define WSCLIENT_LOG(level, fmt, ...) printf("\n\r[WSCLIENT %s] %s: " fmt "\n", level, __FUNCTION__, ##__VA_ARGS__)
#if DEBUG_WSCLIENT == 2
#define WSCLIENT_DEBUG(fmt, ...) WSCLIENT_LOG("DEBUG", fmt, ##__VA_ARGS__)
#else
#define WSCLIENT_DEBUG(fmt, ...)
#endif
#if DEBUG_WSCLIENT
#define WSCLIENT_ERROR(fmt, ...) WSCLIENT_LOG("ERROR", fmt, ##__VA_ARGS__)
#else
#define WSCLIENT_ERROR(fmt, ...)
#endif
/*******************************************************************/

/****************Define the structures used*************************/
typedef enum{ 
	CLOSING, 
	CLOSED, 
	CONNECTING, 
	OPEN 
} readyStateValues;

struct wsheader_type{
	unsigned header_size;
	int fin;
	int mask;
	enum opcode_type {
		CONTINUATION = 0x0,
		TEXT_FRAME = 0x1,
		BINARY_FRAME = 0x2,
		CLOSE = 8,
		PING = 9,
		PONG = 0xa,
	} opcode;
	int N0;
	uint64_t N;
	uint8_t masking_key[4];
};

struct _wsclient_context;

struct ws_fun_ops{
	int (*hostname_connect)(struct _wsclient_context *wsclient);
	void (*client_close)(struct _wsclient_context *wsclient);
	int (*client_send)(struct _wsclient_context *wsclient, unsigned char *data, size_t data_len);
	int (*client_read)(struct _wsclient_context *wsclient, unsigned char *data, size_t data_len);
};

typedef struct _wsclient_context{
	char host[128];
	char path[128];
	char origin[200];
	int port;
	uint8_t use_ssl;
	int sockfd;
	readyStateValues readyState;
	int tx_len;
	void *ssl;
	void *conf;
	void *ssl_fd; 
	uint8_t *txbuf;
	uint8_t *rxbuf;
	uint8_t *receivedData;
	struct ws_fun_ops fun_ops;
}wsclient_context;
/*******************************************************************/

/****************General functions used by wsclient*****************/
void ws_get_random_bytes(void *buf, size_t len);
void* ws_malloc(unsigned int size);
void ws_free(void *buf);
int ws_client_handshake(wsclient_context *wsclient);
int ws_check_handshake(wsclient_context *wsclient);
void ws_sendData(uint8_t type, size_t message_size, uint8_t* message, int useMask, wsclient_context *wsclient);
/*******************************************************************/

/*************Functions used by wsclient without SSL****************/

int ws_hostname_connect(wsclient_context *wsclient);
int ws_client_read(wsclient_context *wsclient, unsigned char *data, size_t data_len);
int ws_client_send(wsclient_context *wsclient, unsigned char *data, size_t data_len);
void ws_client_close(wsclient_context *wsclient);
/*******************************************************************/

/***************Functions used by wsclient with SSL*****************/
int wss_hostname_connect(wsclient_context *wsclient);
int wss_client_read(wsclient_context *wsclient, unsigned char *data, size_t data_len);
int wss_client_send(wsclient_context *wsclient, unsigned char *data, size_t data_len);
void wss_client_close(wsclient_context *wsclient);
/*******************************************************************/

#endif

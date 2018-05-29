/*
 * ssl_platom.h
 *
 *  Created on: Sep 7, 2015
 *      Author: liuhan
 */

#ifndef SSL_PLATOM_H_
#define SSL_PLATOM_H_

#include "ssl/ssl_os_port.h"
#include "ssl/ssl_ssl.h"
#include "ssl/ssl_tls1.h"

typedef void * (*ssl_func_type_t)(void);
typedef void * (*bio_func_type_t)(void);

typedef struct
{
    ssl_func_type_t ssl_func_type;
} PLATOM_CTX;

#define PLATOM_CTX_ATTR  ((PLATOM_CTX *)ssl_ctx->bonus_attr)

/*encapsulation the structure based on the espressif platform*/
struct	_MD_CTX
{
    unsigned char cksum[16];    /* checksum of the data block */
    unsigned char state[48];    /* intermediate digest state */
    unsigned char buffer[16];   /* data block being processed */
    int left;                   /* amount of data in buffer */
};

typedef struct _MD_CTX 			EVP_MD_CTX;
typedef unsigned char			EVP_MD;
typedef struct _x509_ctx 		X509;
typedef struct _x509_ctx 		X509_STORE_CTX;
//typedef struct _SSL 			SSL;
//typedef struct _SSL_CTX 		SSL_CTX;

#define ESP_SSL_ERROR_NONE					0
#define ESP_SSL_ERROR_WANT_WRITE			1
#define ESP_SSL_ERROR_WANT_READ				2
#define ESP_SSL_ERROR_WANT_X509_LOOKUP		3
#define ESP_SSL_ERROR_SYSCALL				4
#define ESP_SSL_ERROR_ZERO_RETURN			5
#define ESP_SSL_ERROR_SSL					6
#define ESP_SSL_FILETYPE_PEM				10
#define ESP_SSL_VERIFY_PEER					11
#define ESP_EVP_MAX_MD_SIZE					6
#define ESP_SSL_VERIFY_FAIL_IF_NO_PEER_CERT	4

#endif /* SSL_PLATOM_H_ */

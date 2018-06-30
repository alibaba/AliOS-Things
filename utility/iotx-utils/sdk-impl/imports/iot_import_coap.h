/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_H__
#define __COAP_PLATFORM_H__

#define NETWORK_ADDR_LEN      (16)

typedef struct
{
    char                 addr[NETWORK_ADDR_LEN];
    unsigned short       port;
} coap_address_t;

#ifdef __OS_LINUX_PLATOFRM__
void *os_wrapper_malloc(const char * f, const int l, int size);
void  os_wrapper_free(void * ptr);
void  os_dump_malloc_free_stats();



#define coap_malloc(size) os_wrapper_malloc(__func__, __LINE__, size)
#define coap_free(ptr)   os_wrapper_free(ptr)
#define coap_calloc calloc
#else

#define coap_malloc(size) malloc(size)
#define coap_free(ptr)    free(ptr)
#define coap_calloc       calloc

#endif

int HAL_UDP_create(void *p_socket);
void HAL_UDP_close(void *p_socket);
int HAL_UDP_write(void *p_socket, const coap_address_t *p_remote,
                  const unsigned char *p_data, unsigned int datalen);
int HAL_UDP_read(void         *p_socket, coap_address_t   *p_remote,
                    unsigned char   *p_data, unsigned int     datalen);
int HAL_UDP_readTimeout( void *p_socket,
                coap_address_t *p_remote, unsigned char  *p_data,
                unsigned int datalen,     unsigned int timeout );

int HAL_UDP_resolveAddress(const char *p_host,  char addr[NETWORK_ADDR_LEN]);


#endif

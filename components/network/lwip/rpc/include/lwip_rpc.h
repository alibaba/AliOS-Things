/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef LWIP_RPC_H
#define LWIP_RPC_H

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <k_api.h>
#include <parcel.h>
#include "lwip/opt.h"
#include "lwip_rpc_socket.h"

#define AOS_SUCCESS (0)
#define AOS_FAILED (-1)

#define ULWIP_RPC_BUFFER_SIZE (4096)      //!< for big data transfer
#define ULWIP_RPC_MAIN_NAME "rpc-ulwip" //!< the main service name of lwip RPC

#define USELECT_RPC_BUFFER_SIZE (4096)     //!< for big data transfer
#define USELECT_RPC_MAIN_NAME     "rpc-usel" //!< the main service name of lwip RPC
#define USELECT_RPC_NAME_FORMAT "rpc-sel-%d" //!< rpc name format for select service
#define NUM_SELECTS 1024
#define USELECT_MULTITASK_SUPPORT

#define SOCKET_RPC_NAME_MAX_SIZE 16 //!< the max length of service name

#if defined(AOS_RPC_NAME_MAX_SIZE) && (AOS_RPC_NAME_MAX_SIZE < SOCKET_RPC_NAME_MAX_SIZE)
#error "The rpc name of lwip service is out range of max size of aos rpc name!!! "
#else
#define SOCKET_RPC_NAME_SIZE SOCKET_RPC_NAME_MAX_SIZE
#endif

void ulwip_request_proccess(struct aos_parcel_t *p_pkt);
void ulwip_request_callback(int eid, void *val);

void ulwip_client_attach_vfs(void);
void ulwip_server_attach_vfs(void);

#endif /* LWIP_RPC_H */

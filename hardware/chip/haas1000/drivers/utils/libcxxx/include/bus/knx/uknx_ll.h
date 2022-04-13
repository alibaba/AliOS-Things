/*
 * @file uknx_ll.h
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef __UKNX_LL_H__
#define __UKNX_LL_H__

/** @addtogroup uknx_stack uknx
 *  Optional policy of uknx.
 *
 *  @{
 */

#include "uknx_def.h"
#include "uknx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UKNX_LL_NCN5120 = 0x1,
    UKNX_LL_IP_TUNNEL = 0x2,
    UKNX_LL_IP_ROUTER = 0x4,
    UKNX_LL_E981 = 0x8,
} UKNX_LL_TYPE;

/* Layer2 Interal API. */
typedef UKNX_RET( *UKNX_LL_IND )( UKNX_BUF *, int len );
UKNX_RET uknx_ll_ind( UKNX_LL_IND cb );
UKNX_RET uknx_ll_req( UKNX_BUF *, int len );
UKNX_RET uknx_ll_proc();

UKNX_RET uknx_ll_start();
UKNX_RET uknx_ll_stop();

UKNX_RET uknx_ll_frame( UKNX_BUF pb, UKNX_SIZE len );

UKNX_RET uknx_ll_init( UKNX_LL_TYPE layer );

/* Layer2 module API. */
typedef enum {
    UKNX_CONF_IPT_SERVER,
    UKNX_CONF_IPT_SERVER_PORT,
    UKNX_CONF_IPT_KEEP_ALIVE,
    UKNX_CONF_IPR_ADDR_H,
    UKNX_CONF_IPR_ADDR_L,
    UKNX_CONF_NCN_ADDR_H,
    UKNX_CONF_NCN_ADDR_L,
    UKNX_CONF_TTY_DEV,
    UKNX_CONF_IPT_LOCAL_ADDR,
    UKNX_CONF_IPT_LOCAL_PORT,
} UKNX_LL_CONFIG;

/**
 * API link layer configuation.
 *
 * @param[in]   conf   conf item.
 * @param[in]   data   conf data pointer
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET uknx_ll_config( UKNX_LL_CONFIG conf, void *data );

UKNX_RET uknx_tpdu_ind( UKNX_DATA_EVENT *pe, int group );

#ifdef __cplusplus
}
#endif

/** @} */

#endif

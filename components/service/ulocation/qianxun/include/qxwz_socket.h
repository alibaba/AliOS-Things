/*------------------------------------------------------------------------------
* qxwz_socket.h : socket interface that clients should realize
*          Copyright (C) 2015-2017 by QXSI, All rights reserved.
*/
#ifndef __QXWZ_SOC_H_
#define __QXWZ_SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qxwz_types.h"

typedef qxwz_s32_t qxwz_soc;

typedef struct _host_info_{
    qxwz_s8_t *hostName;
    qxwz_u16_t port;
}host_info_t;

typedef struct _soc_cb_{
    qxwz_s32_t ( *cb_connect )   (qxwz_s32_t soc);
    qxwz_s32_t ( *cb_send    )   (qxwz_s32_t soc);
    qxwz_s32_t ( *cb_recv    )   (qxwz_s32_t soc, const qxwz_s8_t *buf, qxwz_u32_t len);
    qxwz_s32_t ( *cb_close   )   (qxwz_s32_t soc);
    qxwz_s32_t ( *cb_status  )   (qxwz_s32_t soc, qxwz_s32_t status);/*status: <0 means error occurs in low-level tcp/ip stack*/
}soc_cb_t;

/**
 * @param[in]  nwk_id: network identifier.
 *                     0, By default, then depends on user to choose.
 *                     1, China telecom cellular system
 *                     2, China mobile cellular system
 *                     3, China unicom cellular system
 *                     4, Wi-Fi
 *                     5, Wired
 *                     6, Others
 *
 * @return:
 *    -1 if fail
 *    socket number if okay
 */
qxwz_s32_t qxwz_soc_create(qxwz_u32_t nwk_id);


/**
 * @param[in]  soc: socket number.
 * @param[in]  host: the server info @see host_info_t.
 * @param[in]  cbs: the callbacks for async @see soc_cb_t.
 *
 * @return:
 *   0 if success
 *  -1 if fail
 *  -2 if async occurs
 */
qxwz_s32_t qxwz_soc_connect(qxwz_s32_t soc, host_info_t *host, soc_cb_t *cbs);


/**
 * @param[in]  soc: socket number.
 * @param[in]  buf: the data buffer which will be sent.
 * @param[in]  len: the data buffer length.
 *
 * @return:
 *  >=0 if success
 *   -1 if fail for any reason
 *   -2 if buffer not available, async occurs
 */
qxwz_s32_t qxwz_soc_send(qxwz_s32_t soc, const qxwz_s8_t *buf, qxwz_s32_t len);

/**
 * @param[in]  soc: socket number.
 * @param[out] buf: the data buffer which will be filled by received data.
 * @param[in]  len: the maxium lenght of data buffer.
 *
 * @return:
 * >=0 if success
 *  -1 if fail for any reason, or receive the FIN from the server
 *  -2 if no data available, async occurs
 */
qxwz_s32_t qxwz_soc_recv(qxwz_s32_t soc, qxwz_s8_t *buf, qxwz_s32_t len);

/**
 * @param[in]  soc: socket number.
 *
 * @return:
 *   0 if success
 *  -1 if fail
 *  -2 if async occurs
 */
qxwz_s32_t qxwz_soc_close(qxwz_s32_t soc);

qxwz_s32_t qxwz_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif /*__QXWZ_SOC_H_*/


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __SE_DRIVER_H__
#define __SE_DRIVER_H__

#include "irot_hal.h"

#ifdef __cplusplus
extern "C"
#endif

/**
 * @brief open session and connect to SE.
 *
 * @param handle
 *
 * @return @see error code definitions.
 */
irot_result_t se_open_session(void **handle);

/**
 * @brief transmit APDU to SE.
 *
 * @param handle
 * @param cmd_apdu Command APDU(ISO7816-4).
 * @param cmd_len  Command APDU length
 * @param rsp_buf  response APDU buffer.
 * @param rsp_len  input with response APDU buffer length, output with real response APDU length (SW in last two bytes).
 *
 * @return @see error code definitions.
 */
irot_result_t se_transmit(void *handle, const uint8_t *cmd_apdu, uint32_t cmd_len, uint8_t *rsp_buf, uint32_t *rsp_len);

/**
 * @brief close session and disconnect to SE.
 *
 * @param handle
 *
 * @return @see error code definitions.
 */
irot_result_t se_close_session(void *handle);

#ifdef __cplusplus
}
#endif

#endif  /* __SE_DRIVER_H__ */


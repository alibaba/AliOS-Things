/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATHOST_IMPORT_H_
#define _ATHOST_IMPORT_H_

/**
 * The prototype of the callback function which is to be
 * called to handle the AT commands reaceived.
 *
 * @param[in]  arg     The argument which will be passed to the callback function.
 * @param[in]  buf     The buffer which can be used by the callback function.
 * @param[in]  buflen  The length, in bytes, of the buffer.
 *
 * @return  none.
 */
typedef void (*athost_atcmd_handle_cb_t)(void *arg, char *buf, int buflen);

/**
 * Read maximun 'len' bytes data from athost's AT receiving buffer.
 * The read data is to be stored in outbuf.
 *
 * @param[out]  outbuf  The pointer of the memory to store the read data.
 * @param[in]   len     The expected length of the data to read.
 *
 * @return  a positive number to indicate the bytes read, otherwise failure.
 */
int HAL_Athost_Read(char *outbuf, uint32_t len);

/**
 * Send the data with header and tailer.
 *
 * @param[in]  header  The header string to be sent proi to the data sending.
 * @param[in]  data    The pointer of the data to be sent.
 * @param[in]  len     The lenght of the data.
 * @param[in]  tailer  The tailer string to be sent after the data sending.
 *
 * @return  0 on success, otherwise failure.
 */
int HAL_Athost_Write(const char *header, const uint8_t *data, uint32_t len,
                     const char *tailer);

/**
 * Register a callback to handle the AT commands received with 'prefix' prefix.
 *
 * @param[in]  prefix  The prefix string of the AT commands.
 *             Usually it's something like "AT+".
 * @param[in]  fn      The callback function to handle the AT commands.
 *
 * @return  0 on success, otherwise failure.
 */
int HAL_Athost_HandleRegisterCb(const char *prefix, athost_atcmd_handle_cb_t fn);

#endif

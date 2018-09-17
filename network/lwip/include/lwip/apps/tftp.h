/****************************************************************//**
 *
 * @file tftp.h
 *
 * @author   Logan Gunthorpe <logang@deltatee.com>
 *
 * @brief    Trivial File Transfer Protocol (RFC 1350)
 *
 * Copyright (c) Deltatee Enterprises Ltd. 2013
 * All rights reserved.
 *
 ********************************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification,are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Logan Gunthorpe <logang@deltatee.com>
 *
 */

#ifndef LWIP_HDR_APPS_TFTP_H
#define LWIP_HDR_APPS_TFTP_H

#include "lwip/apps/tftp_opts.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup tftp
 * TFTP context containing callback functions for TFTP transfers
 */

#define TFTP_MAX_PAYLOAD_SIZE 512
#define TFTP_HEADER_LENGTH    4

#define TFTP_RRQ   1
#define TFTP_WRQ   2
#define TFTP_DATA  3
#define TFTP_ACK   4
#define TFTP_ERROR 5

typedef enum tftp_error_s {
  TFTP_ERROR_FILE_NOT_FOUND    = 1,
  TFTP_ERROR_ACCESS_VIOLATION  = 2,
  TFTP_ERROR_DISK_FULL         = 3,
  TFTP_ERROR_ILLEGAL_OPERATION = 4,
  TFTP_ERROR_UNKNOWN_TRFR_ID   = 5,
  TFTP_ERROR_FILE_EXISTS       = 6,
  TFTP_ERROR_NO_SUCH_USER      = 7
} tftp_error_t;

typedef struct tftp_context_s {
  /**
   * Open file for read/write.
   * @param fname Filename
   * @param mode Mode string from TFTP RFC 1350 (netascii, octet, mail)
   * @param write Flag indicating read (0) or write (!= 0) access
   * @returns File handle supplied to other functions
   */
  void* (*open)(const char* fname, const char* mode, u8_t write);
  /**
   * Close file handle
   * @param handle File handle returned by open()
   */
  void (*close)(void* handle);
  /**
   * Read from file
   * @param handle File handle returned by open()
   * @param buf Target buffer to copy read data to
   * @param bytes Number of bytes to copy to buf
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
  int (*read)(void* handle, void* buf, int bytes);
  /**
   * Write to file
   * @param handle File handle returned by open()
   * @param pbuf PBUF adjusted such that payload pointer points
   *             to the beginning of write data. In other words,
   *             TFTP headers are stripped off.
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
  int (*write)(void* handle, struct pbuf* p);
} tftp_context_t;

typedef void (*tftp_done_cb)(int err, int length);

err_t tftp_server_start(void);
void  tftp_server_stop(void);
int   tftp_client_get(const ip_addr_t *paddr, const char *fname,
                      tftp_context_t *ctx, tftp_done_cb cb);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_TFTP_H */

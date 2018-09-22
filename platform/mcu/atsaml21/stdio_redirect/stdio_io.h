/**
 * \file
 *
 * \brief STDIO redirection terminal
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _STDIO_IO_H_INCLUDED
#define _STDIO_IO_H_INCLUDED

#include <hal_io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 *  \brief Initialize STDIO access
 *  \param[in] io Pointer to IO descriptor,
 *                NULL to discard R/W without any error.
 */
void stdio_io_init(struct io_descriptor *io);

/**
 *  \brief Change IO descriptor for terminal to R/W data
 *  \param[in] io Pointer to IO descriptor,
 *                NULL to discard R/W without any error.
 */
void stdio_io_set_io(struct io_descriptor *io);

/**
 *  \brief Read through specified terminal
 *  \param[out] buf Pointer to buffer to place read data
 *  \param[in] len Data length in number of bytes
 *  \return status
 *  \retval >=0 number of bytes read
 *  \retval <0 error
 */
int32_t stdio_io_read(uint8_t *buf, const int32_t len);

/**
 *  \brief Write through specified terminal
 *  \param[in] buf Pointer to buffer to place data to write
 *  \param[in] len Data length in number of bytes
 *  \return status
 *  \retval >=0 number of bytes read
 *  \retval <0 error
 */
int32_t stdio_io_write(const uint8_t *buf, const int32_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _STDIO_IO_H_INCLUDED */

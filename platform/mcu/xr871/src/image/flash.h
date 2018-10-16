/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _IMAGE_FLASH_H_
#define _IMAGE_FLASH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t flash_rw(uint32_t flash, uint32_t addr,
                  void *buf, uint32_t size, int do_write);

/**
 * @brief Read an amount of data from flash
 * @param[in] flash Flash device number
 * @param[in] addr flash address to be read
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read
 * @return Number of bytes read
 */
static __inline uint32_t flash_read(uint32_t flash, uint32_t addr,
                                    void *buf, uint32_t size)
{
	return flash_rw(flash, addr, buf, size, 0);
}

/**
 * @brief Write an amount of data to flash
 * @param[in] flash Flash device number
 * @param[in] addr flash address to be written
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be written
 * @return Number of bytes written
 */
static __inline uint32_t flash_write(uint32_t flash, uint32_t addr,
                                     const void *buf, uint32_t size)
{
	return flash_rw(flash, addr, (void *)buf, size, 1);
}

int32_t flash_get_erase_block(uint32_t flash, uint32_t addr, uint32_t size);
int flash_erase(uint32_t flash, uint32_t addr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _IMAGE_FLASH_H_ */

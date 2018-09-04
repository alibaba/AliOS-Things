/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CHIP_MX6SL

#include "obds.h"

const char g_cs42888_i2c_device_id_test_name[] = "CS42888 I2C Device ID Test";
extern audio_card_p snd_card;
extern audio_card_t snd_card_esai;
extern int32_t cs42888_dev_info(void *, uint8_t *);
extern int32_t cs42888_init(void *);
extern int32_t cs42888_deinit(void *);
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

//! @todo Implement cs42888 device check using audio driver, if possible.
//!
//! The original test attempted to read each of the device's registers, from
//! register 0x01 through 0x19.
test_return_t i2c_device_check_cs42888(void)
{
		int nRet1 = TEST_FAILED;
		snd_card = &snd_card_esai;
		cs42888_init(snd_card->codec);

		
#if defined (BOARD_SABRE_AI)
		uint8_t dev_id = 0;
		int32_t ret;

		if((ret = cs42888_dev_info(snd_card, &dev_id)) == 0) {
				nRet1 = TEST_PASSED;
				printf("CS42888 device ID is 0x%04x , revision level is 0x%04x.\n\n", (dev_id & 0xf0) >> 4, (dev_id & 0x0f));
			}
			else {
				nRet1 = TEST_FAILED;
				printf("CS42888 I2C device check failed.\n\n");
			}
#endif

		cs42888_deinit(snd_card);

//     unsigned int i, ret;
//     unsigned char data;
// 
//     cs42888_i2c_init();
//     for (i = 1; i < 0x1A; i++) {
//         if ((ret = cs42888_reg_read(i, &data)) != 0) {
//             printf("CS42888 I2C device check failed.\n\n");
//             return ret;
//         }
//     }
// 
//     printf("CS42888 I2C device check passed. \n\n");

    return nRet1;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

#endif

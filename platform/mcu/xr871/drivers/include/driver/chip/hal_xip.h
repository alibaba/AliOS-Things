/**
  * @file  hal_xip.h
  * @author  XRADIO IOT WLAN Team
  */

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

#ifndef HAL_XIP_H_
#define HAL_XIP_H_

#include "driver/chip/hal_flashctrl.h"
#include "driver/chip/flashchip/flash_chip.h"

//typedef struct XipDriverBase XipDriverBase;

struct XipDriverBase
{
	int (*setCmd)(struct XipDriverBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);
	int (*setDelay)(struct XipDriverBase *base, Flashc_Delay *delay);
	int (*setAddr)(struct XipDriverBase *base, uint32_t addr);
	int (*setContinue)(struct XipDriverBase *base, uint32_t continueMode, void *arg);

	XIP_Config mCfg;
	uint32_t flash;
	FlashDev *dev;
};


HAL_Status HAL_Xip_Init(uint32_t flash, uint32_t xaddr);

HAL_Status HAL_Xip_Deinit(void);

#endif /* HAL_XIP_H_ */

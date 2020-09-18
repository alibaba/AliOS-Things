/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_audiosystem.h
 * @brief:   vendor audiosystem apis defination
 * @author:  
 * @date:    2019/12/09
 * @version: 1.0
 */
#ifndef _TG_VENDOR_AUDIOSYSTEM_H_
#define _TG_VENDOR_AUDIOSYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set Master Volume
 * @param[in] volume: master volume with value from 0 to 100, 0 means mute.
 *                    volume steps should implement in this function.
 */
void tg_volume_set(int volume);

int tg_gain_set(int mic_id, int gain);

int tg_gain_get(int mic_id);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_AUDIOSYSTEM_H_ */

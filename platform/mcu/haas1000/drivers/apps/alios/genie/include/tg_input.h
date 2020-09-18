/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts_light.h
 * @brief:
 * @author:  
 * @date:    2019/12/15
 * @version: 1.0
 */

#ifndef _TG_VENDOR_LIGHT_H_
#define _TG_VENDOR_LIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif


enum tg_key_code {
    KEYBOARD_VOL_INC_SHORT = 110,
    KEYBOARD_VOL_DEC_SHORT,
    KEYBOARD_MIC_SHORT,
    KEYBOARD_VOL_INC_LONG,
    KEYBOARD_VOL_DEC_LONG,
    KEYBOARD_MIC_LONG,
    KEYBOARD_MIC_VOL_INC,
    KEYBOARD_MIC_VOL_DEC,
    KEYBOARD_RST_SHORT,
    KEYBOARD_RST_LONG,
    KEYBOARD_RST_VOL_INC,
    KEYBOARD_RST_VOL_DEC,
    KEYBOARD_MIC_RST,
    KEYBOARD_PHONE_HANGUP,
    KEYBOARD_GENIE_KEY_SHORT,
    KEYBOARD_GENIE_KEY_DOUBLE,
    KEYBOARD_GENIE_KEY_LONG,
    KEYBOARD_MAX
};

typedef void(*keycode_report_cb)(int keycode);


/**
 * @brief register keyboard event callback
 * @param[in] keycode_report_cb: keyboard event callback
 * @return Zero on success or error code otherwise
 */
int tg_register_keyboard_event_callback(keycode_report_cb cb);



#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_LIGHT_H_ */

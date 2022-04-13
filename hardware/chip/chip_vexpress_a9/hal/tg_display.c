/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "tg_syscall_arg.h"
#include "tg_audio.h"


int tg_display_show_fb(unsigned char *buf, unsigned int x, unsigned int y,unsigned int w,unsigned int h)
{
    return 0;
}

tg_pcm_handle_t tg_pcm_open(enum tg_pcm_mode mode, int samplerate, int channels, enum tg_pcm_format format)
{
    return NULL;
}

int tg_pcm_start(tg_pcm_handle_t pcm)
{
    return 0;
}

int tg_pcm_write(tg_pcm_handle_t pcm, void *buf, unsigned int size)
{
    return 0;
}

int tg_pcm_read(tg_pcm_handle_t pcm, void *buf, unsigned int size)
{
    return 0;
}

int tg_pcm_stop(tg_pcm_handle_t pcm)
{
    return 0;
}

void tg_pcm_close(tg_pcm_handle_t pcm)
{

}
void tg_volume_set(int volume)
{

}
void tg_pcm_set_state_listener(tg_pcm_handle_t pcm, tg_pcm_state_callback_t cbf, void *user)
{

}

int tg_get_chipid(char *buffer, int buf_len)
{
    return 0;
}
void tg_reboot(void)
{

}

const char* tg_ota_get_current_ab(void)
{
    return NULL;
}

void tg_ota_start(void)
{

}

int tg_ota_upgrade_slice(const char *image_name, const char *file_name,
        uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, char *buffer)
{
    return 0;
}

int tg_ota_upgrade_end(void)
{
    return 0;
}

void tg_ota_finish(int ab_fallback)
{
    
}

int drv_tg_speech_msg_read(void *buffer, int size)
{
    return 0;
}
int drv_tg_speech_msg_send(void *buffer, int size, bool sync)
{
    return 0;
}
int drv_tg_speech_msg_register(void)
{
    return 0;
}
typedef void (*TG_BT_HAL_HCIT_CB)(void);
int32_t tg_bt_hal_hcit_rx(uint8_t *p_buf, uint16_t len)
{
    return 0;
}
int32_t tg_bt_hal_hcit_tx(uint8_t *p_buf, uint16_t len, TG_BT_HAL_HCIT_CB tx_cb)
{
    return 0;
}
void tg_bt_hal_hcit_set_rx_ind_callback(TG_BT_HAL_HCIT_CB ind)
{

}
int32_t tg_bt_hal_vendor_bringup(void)
{
    return 0;
}
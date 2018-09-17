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

#include <stdio.h>
#include "obds.h"
#include "sdk.h"
#include "audio/test/wav_data.data"
#include "audio/imx_audmux.h"
#include "audio/audio.h"

#define SSI_WRITE_TIMEOUT	0x400000

const char g_audio_ssi_test_name[] = "I2S Audio Test";

//extern audio_pcm_para_t pcm_para;
audio_pcm_para_t pcm_para = {
    .sample_rate = SAMPLERATE_44_1KHz,
    .channel_number = 2,
    .word_length = WL_16,
};

//extern audio_pcm_t pcm_music;
audio_pcm_t pcm_music = {
    .name = "some noise",
    .para = &pcm_para,
    .buf = (uint8_t *) wav_data,
    .size = sizeof(wav_data),
};

uint8_t audio_record_buf[AUDIO_RECORD_BUF_LEN] __attribute__ ((aligned(4)));
audio_pcm_t pcm_record = {
    .name = "Record",
    .buf = audio_record_buf,
    .size = sizeof(audio_record_buf),
    .para = &pcm_para
};

extern void audio_codec_power_on(void);
extern void audio_clock_config(void);

audio_card_p snd_card;

extern audio_card_t snd_card_ssi_sgtl5000;
extern audio_card_t snd_card_ssi_wm8958;
extern audio_card_t snd_card_ssi_wm8962;

/* This function initializes AUDMUX for SSI2, and Audio Codec */
int audio_test_init(void)
{
    audio_dev_para_t dev_para;

#if defined(BOARD_SMART_DEVICE)
#if defined(BOARD_REV_A)
    snd_card = &snd_card_ssi_wm8958;
#else
    snd_card = &snd_card_ssi_wm8962;
#endif
#elif (defined(CHIP_MX6SL) && defined(BOARD_EVK))
    snd_card = &snd_card_ssi_wm8962;
#else
    //Default
    snd_card = &snd_card_ssi_sgtl5000;
#endif

#if defined(BOARD_SMART_DEVICE) || (defined(CHIP_MX6SL) && defined(BOARD_EVK))
    audmux_route(AUDMUX_PORT_2, AUDMUX_PORT_3, AUDMUX_SSI_MASTER);
    dev_para.bus_mode = AUDIO_BUS_MODE_MASTER;
#endif

    //Config SSI 
    dev_para.bus_protocol = AUDIO_BUS_PROTOCOL_I2S;
    dev_para.sample_rate = SAMPLERATE_48KHz;
    dev_para.word_length = WL_16;
    dev_para.trans_dir = AUDIO_TRANS_DIR_TX;

    if (0 != snd_card->ops->init(snd_card)) {
        printf("Init %s failed.\n", snd_card->name);
        return -1;
    }
    printf("SSI port2 initialized\n");

    if (0 != snd_card->ops->config(snd_card, &dev_para)) {
        printf("The codec %s failed to be configured.\n", snd_card->name);
        return -1;
    }

    return 0;
}

/*!
 * This function sends square wave data to I2S data in of SGTL5000 and
 * output through headphone. make sure SSI port has been initialized.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
test_return_t i2s_audio_test(void)
{
    char recvCh;
    uint32_t bytes_written = 0;
    test_return_t result = TEST_FAILED;
	const char* indent = menu_get_indent();

    printf("\n%sPlease plug in headphones to the HEADPHONE OUT jack.\n", indent);

    if (!is_input_char('y', indent))
    	return TEST_BYPASSED;

    //printf("To power code on\n");
    audio_codec_power_on();     // in hardware.c

    audio_clock_config();
    //printf("Audio clock configed.\n");

    if (0 != audio_test_init()) {
        printf("%sInitialization of audio subsystem failed.\n", indent);
        return TEST_FAILED;
    }

    printf("%sAudio output: please ensure headphones are plugged in to hear.\n", indent);
    printf("%sIf you hear the sound, enter Y to exit. Or press any other key to try it again.\n", indent);

    while (1) {
        if (snd_card->ops->write(snd_card, pcm_music.buf, pcm_music.size, &bytes_written) < 0) 
        {
            printf("%sWrite to SSI timeout.\n", indent);

            return TEST_FAILED;
        }

        printf("\n%sDo you need to re-hear it? (y/n)\n", indent);

        do {
            recvCh = getchar();
        }
        while (recvCh == 0xFF);

        if ((recvCh == 'Y') || (recvCh == 'y'))
            continue;           /* hear again */
        else
            break;              /* by pass */
    }

    printf("%sDo you hear audio from headphone? (y/n)\n", indent);

    do {
        recvCh = getchar();
    }
    while (recvCh == 0xFF);

    if ((recvCh == 'y') || (recvCh == 'Y')) 
    {
        printf("%s SSI playback test passed.\n", indent);
        result = TEST_PASSED;
    } else 
    {
        printf("%s SSI playback test failed.\n", indent);

        return TEST_FAILED;
    }

#if (defined(CHIP_MX6SL) && defined(BOARD_EVK)) || (defined(BOARD_SMART_DEVICE) && (defined(BOARD_REV_B) || defined(BOARD_REV_C)))
    printf("%s Audio input: please ensure microphone is plugged in. Press 'y/Y' to confirm.\n", indent);
    do {
        recvCh = getchar();
    }
    while (recvCh == 0xFF);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("%s Start recording...\n", indent);
        if (snd_card->ops->read(snd_card, pcm_record.buf, pcm_record.size, &bytes_written) > 0) 
        {
            printf("%sRead SSI fifo timeout.\n", indent);

            return TEST_FAILED;
        }

        printf("%s Start playing back the voice just recorded...\n", indent);
        if (snd_card->ops->write(snd_card, pcm_record.buf, pcm_record.size, &bytes_written) > 0) 
        {
            printf("%sWrite SSI fifo timeout.\n", indent);

            return TEST_FAILED;
        }
        printf("%s Do you hear voice from headphone? (y/n)\n", indent);

        do {
            recvCh = getchar();
        }
        while (recvCh == 0xFF);

        if ((recvCh == 'y') || (recvCh == 'Y')) 
        {
            printf("%s SSI record test passed.\n", indent);

            result = TEST_PASSED;
        } 
        else 
        {
            printf(" SSI record test failed.\n");

            return TEST_FAILED;
        }
    }
#endif

    return result;
}

#if 0
int i2s_audio_test_enable;

/* 	This function sends square wave data to I2S data in of SGTL5000 and output through headphone.
	make sure SSI port has been initialized
*/
int i2s_audio_test(void)
{
    char recvCh;
    uint32_t bytes_written = 0;
//    int time_out = SSI_WRITE_TIMEOUT;

    if (!i2s_audio_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("I2S AUDIO", NULL);

    printf("Please plug in headphones to the HEADPHONE OUT jack\n");

    if (!is_input_char('y', NULL)) {
        printf(" Skip AUDIO test \n");
        return TEST_BYPASSED;
    }
    //printf("To power code on\n");
    audio_codec_power_on();     // in hardware.c
    //hal_delay_us(1000*1000);
    //printf("Audio codec powered.\n");
    audio_clock_config();
    //printf("Audio clock configed.\n");

    if (0 != audio_test_init()) {
        printf("Initialization of audio subsystem failed\n");
        return TEST_FAILED;
    }

    printf("Audio output: please ensure headphones are plugged in to hear \n");
    printf("If you hear the sound, enter Y to exit. Or press any other key to try it again.\n");

    while (1) {
        if (snd_card->ops->write(snd_card, pcm_music.buf, pcm_music.size, &bytes_written) < 0) {
            printf("Write to SSI timeout\n");
            return TEST_FAILED;
        }

        printf(" Do you need to re-hear it? (y/n)\n");

        do {
            recvCh = getchar();
        }
        while (recvCh == 0xFF);

        if ((recvCh == 'Y') || (recvCh == 'y'))
            continue;           /* hear again */
        else
            break;              /* by pass */
    }

    printf(" Do you hear audio from headphone? (y/n)\n");

    do {
        recvCh = getchar();
    }
    while (recvCh == 0xFF);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf(" SSI playback test passed.\n");
    } else {
        printf(" SSI playback test failed.\n");
        return TEST_FAILED;
    }

#if (defined(CHIP_MX6SL) && defined(BOARD_EVK)) || (defined(BOARD_SMART_DEVICE) && (defined(BOARD_REV_B) || defined(BOARD_REV_C)))
    printf(" Audio input: please ensure micphone is plugged in. Press 'y/Y' to confirm.\n");
    do {
        recvCh = getchar();
    }
    while (recvCh == 0xFF);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf(" Start recording...\n");
        if (snd_card->ops->read(snd_card, pcm_record.buf, pcm_record.size, &bytes_written) > 0) {
            printf("Read SSI fifo timeout.\n");
            return TEST_FAILED;
        }

        printf(" Start playbacking the voice just recorded...\n");
        if (snd_card->ops->write(snd_card, pcm_record.buf, pcm_record.size, &bytes_written) > 0) {
            printf("Write SSI fifo timeout.\n");
            return TEST_FAILED;
        }
        printf(" Do you hear voice from headphone? (y/n)\n");

        do {
            recvCh = getchar();
        }
        while (recvCh == 0xFF);

        if ((recvCh == 'y') || (recvCh == 'Y')) {
            printf(" SSI record test passed.\n");
        } else {
            printf(" SSI record test failed.\n");
            return TEST_FAILED;
        }
    }
#endif

    return TEST_PASSED;
}
#endif

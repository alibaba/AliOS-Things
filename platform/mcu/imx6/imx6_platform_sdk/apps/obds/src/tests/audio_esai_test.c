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

#include <stdio.h>
#include "obds.h"
#include "io.h"
#include "soc_memory_map.h"
#include "audio/audio.h"

const char g_audio_esai_test_name[] = "Audio ESAI Test";

extern audio_pcm_t pcm_music;
extern audio_pcm_para_t pcm_para;
extern audio_pcm_t pcm_record;
extern audio_card_t snd_card_esai;

extern audio_card_p snd_card;

test_return_t esai_test(void)
{
//    int nRet1 = TEST_FAILED, nRet2 = TEST_FAILED, nRet3 = TEST_FAILED;
    int nRet1 = TEST_FAILED;
#if !defined(BOARD_SABRE_AI) // per Babin 5/8/12
    int nRet2 = TEST_FAILED, nRet3 = TEST_FAILED;
#endif
    uint8_t recvCh = 0;
    audio_dev_para_t dev_para;
    uint32_t bytes_written = 0;
    const char* indent = menu_get_indent();

    dev_para.bus_mode = AUDIO_BUS_MODE_MASTER;
    dev_para.trans_dir = AUDIO_TRANS_DIR_TX;
    dev_para.sample_rate = (pcm_music.para)->sample_rate;
    dev_para.word_length = (pcm_music.para)->word_length;
    dev_para.channel_number = (pcm_music.para)->channel_number;

#if defined(BOARD_SABRE_AI)
    printf
        ("%sPlease make sure the SABRE AI board is mounted on the main board and the headphone is inserted. Type 'y' to confirm.\n", indent);
#else
    printf
        ("%sPlease make sure the \"Automotive Port card \" is mounted and the headphone is inserted. Type 'y' to confirm.\n", indent);
#endif
    do {
        recvCh = getchar();
    } while (0xFF == recvCh);
    if ((recvCh != 'y') && (recvCh != 'Y')) {
        return TEST_BYPASSED;
    }
    
    snd_card = &snd_card_esai;
 
    if (0 != snd_card->ops->init(snd_card)) {
        nRet1 = TEST_FAILED;
        goto CleanUP2;
    }
#if defined(BOARD_SABRE_AI)
    dev_para.bus_mode = AUDIO_BUS_MODE_SLAVE;
    snd_card->ops->config(snd_card, &dev_para);
//    esai_config(0, TX_CH_SEL_CH0 | TX_CH_SEL_CH1 | TX_CH_SEL_CH2 | TX_CH_SEL_CH3, RX_CH_SEL_CH0);   //Config as slave
#else
    dev_para.bus_mode = AUDIO_BUS_MODE_MASTER;
    snd_card->ops->config(snd_card, &dev_para);
//    esai_config(1, TX_CH_SEL_CH0 | TX_CH_SEL_CH1 | TX_CH_SEL_CH2 | TX_CH_SEL_CH3, RX_CH_SEL_CH0);   //Config as master
#endif

    while (1) {
        if (0 != snd_card->ops->write(snd_card, pcm_music.buf, pcm_music.size, &bytes_written)) {
            printf("%sesai_output_data timeout.\n", indent);
            nRet1 = TEST_FAILED;
            goto CleanUP2;
        }

        printf("%sDo you need to replay it? Type 'y' or 'n'.\n", indent);
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);
        if ((recvCh == 'y') || (recvCh == 'Y'))
            continue;
        else
            break;
    }

    printf("%sDo you hear audio? Type 'y' or 'n'.\n", indent);
    do {
        recvCh = getchar();
    } while (0xFF == recvCh);
    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("%s ESAI playback test passed.\n", indent);
        nRet1 = TEST_PASSED;
    } else {
        printf("%s ESAI playback test failed.\n", indent);
        nRet1 = TEST_FAILED;
        goto CleanUP1;
    }

    /***************************LINI-IN recording test*******************/
#if !defined(BOARD_SABRE_AI)          // per Babin 5/9/12

//    audio_pcm_t pcm_record;

    printf("%sDo you want to run the LINE-IN recording test? Type 'y' or 'n'.\n", indent);
    do {
        recvCh = getchar();
    } while (0xFF == recvCh);
    if ((recvCh != 'y') && (recvCh != 'Y')) {
        printf("%sESAI recording test bypassed.\n", indent);
        nRet2 = TEST_BYPASSED;
    }
    if (nRet2 != TEST_BYPASSED) {
#if (!defined(CHIP_MX6SL) && defined(BOARD_EVB))
        printf("%sFor MX6DQ/MX6SDL EVB board, only Rev X3 supported, and R394 should be mounted.\n", indent);
#endif
        printf("%sPlease connect LINE_OUT signal in your PC ", indent);
#if defined(BOARD_SABRE_AI)
        printf("to \"LINE-IN\" on the main board.");
#else
        printf("to AIN1 and AIN2 on \"Automotive Port card \".");
#endif

        printf(" Type 'y' or 'Y' to confirm.\n");
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);
        if ((recvCh != 'y') && (recvCh != 'Y')) {
            printf("%sESAI recording test bypassed.\n", indent);
            nRet2 = TEST_BYPASSED;
            goto CleanUP1;
        }

        printf("%sRecording...\n", indent);
        if (snd_card->ops->read(snd_card, pcm_record.buf, pcm_record.size, &bytes_written) <= 0) {
//        if (0 != esai_input_data(audio_record_buf, AUDIO_RECORD_BUF_LEN)) {
            printf("%sesai_input_data timeout.\n", indent);
            nRet2 = TEST_FAILED;
            goto CleanUP1;
        }

        printf("%sPress any key to hear the audio recorded? \n", indent);
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);

        printf("%sPlayback the music recorded...\n", indent);

        memcpy(&pcm_record, &pcm_music, sizeof(audio_pcm_t));

        while (1) {
            snd_card->ops->write(snd_card, pcm_record.buf, pcm_record.size, &bytes_written);
//            esai_output_data((audio_pcm_p) & pcm_record);

            printf("%sDo you need to replay it? Type 'y' or 'n'.\n", indent);
            do {
                recvCh = getchar();
            } while (0xFF == recvCh);
            if ((recvCh == 'y') || (recvCh == 'Y'))
                continue;
            else
                break;
        }

        printf("%sDo you hear audio? Type 'y' or 'n'.\n", indent);
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);
        if ((recvCh == 'y') || (recvCh == 'Y')) {
            printf("%s ESAI record test passed.\n", indent);
            nRet2 = TEST_PASSED;
        } else {
            nRet2 = TEST_FAILED;;
            printf("%s ESAI record test failed.\n", indent);
            goto CleanUP1;
        }
    }
#endif // !defined(BOARD_SABRE_AI) - per Babin 5/8/12

    /***************************** MIC-IN recording test ***********************************/
#if !defined(BOARD_SABRE_AI)          // per Babin 5/8/12
    printf("%sDo you want to run the MIC-IN recording test? Type 'y' or 'n'.\n", indent);
    do {
        recvCh = getchar();
    } while (0xFF == recvCh);
    if ((recvCh != 'y') && (recvCh != 'Y')) {
        printf("%sESAI recording test bypassed.\n", indent);
        nRet3 = TEST_BYPASSED;
    }
    if (nRet3 != TEST_BYPASSED) {
        printf("%sPlease connect headphone", indent);
        printf(" to \"MICS1+2\" on the main board.");

        printf("Type 'y' or 'Y' to confirm.\n");
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);
        if ((recvCh != 'y') && (recvCh != 'Y')) {
            printf("%sESAI MIC-IN recording test bypassed.\n", indent);
            nRet3 = TEST_BYPASSED;
            goto CleanUP1;
        }

        printf("%sRecording...\n", indent);
        
        if (0 != snd_card->ops->read(snd_card, pcm_record.buf, pcm_record.size, &bytes_written)) {
//        if (0 != esai_input_data(audio_record_buf, AUDIO_RECORD_BUF_LEN)) {
            printf("%sesai_input_data timeout.\n", indent);
            nRet3 = TEST_FAILED;
            goto CleanUP1;
        }

        printf("%sPress any key to hear the audio recorded? \n", indent);
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);

        printf("%sPlayback the music recorded...\n", indent);

        memcpy(&pcm_record, &pcm_music, sizeof(audio_pcm_t));

        while (1) {
            snd_card->ops->write(snd_card, pcm_record.buf, pcm_record.size, &bytes_written);
//            esai_output_data((audio_pcm_p) & pcm_record);

            printf("%sDo you need to replay it? Type 'y' or 'n'.\n", indent);
            do {
                recvCh = getchar();
            } while (0xFF == recvCh);
            if ((recvCh == 'y') || (recvCh == 'Y'))
                continue;
            else
                break;
        }

        printf("%sDo you hear audio? Type 'y' or 'n'.\n", indent);
        do {
            recvCh = getchar();
        } while (0xFF == recvCh);
        if ((recvCh == 'y') || (recvCh == 'Y')) {
            printf("%s ESAI record test passed.\n", indent);
            nRet3 = TEST_PASSED;
        } else {
            nRet3 = TEST_FAILED;;
            printf("%s ESAI record test failed.\n", indent);
            goto CleanUP1;
            printf("%sDo you want to run the LINE-IN recording test? Type 'y' or 'n'.\n", indent);
            do {
                recvCh = getchar();
            } while (0xFF == recvCh);
            if ((recvCh != 'y') && (recvCh != 'Y')) {
                printf("%sESAI recording test bypassed.\n", indent);
                nRet3 = TEST_BYPASSED;
                goto CleanUP1;
            }
        }
    }
#endif // !defined(BOARD_SABRE_AI) - per Babin 5/8/12

  CleanUP1:
    snd_card->ops->deinit(snd_card);
//   esai_deinit();

  CleanUP2:
#if !defined(BOARD_SABRE_AI) // per Babin 5/8/12
    if ((nRet1 == TEST_FAILED) || (nRet2 == TEST_FAILED) || (nRet3 == TEST_FAILED))
        return TEST_FAILED;
    else
        return TEST_PASSED;
#else
    return nRet1;
#endif
}

#endif // #ifndef CHIP_MX6SL

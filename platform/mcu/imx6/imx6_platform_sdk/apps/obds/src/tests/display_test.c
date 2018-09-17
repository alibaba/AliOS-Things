/*
 * Copyright (c) 2009-2012, Freescale Semiconductor, Inc.
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

/*!
 * @defgroup diag_ipu IPU Test
 */

/*!
 * @file ipu_test.c
 * @brief ipu test case on different display devices, including TFTLCD, HDMI, LVDS and VGA panels.
 * @ingroup diag_ipu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "obds.h"
#include "gpio/gpio.h"
#include "logo.h"
#include "ldb/ldb_def.h"
#include "ipu/ipu_common.h"
#include "registers/regsipu.h"
#include "hdmi/src/hdmi_tx.h"

const char g_ipu_display_tests_name[] = "IPU Display Tests";

char panel_name[32];

/*!
 * configure the hdmi module for 720p output
 */
void hdmi_720p_init(int ipu_index, int ipu_di)
{
    int hdmi_src = ((ipu_index - 1) << 1) | ipu_di;

    hdmi_data_info_s myHDMI_info = { 0 };   // declare new hdmi module object instance
    hdmi_vmode_s myHDMI_vmode_info = { 0 }; // declare new hdmi module object instance
    myHDMI_info.video_mode = &myHDMI_vmode_info;

    myHDMI_info.enc_in_format = eRGB;
    myHDMI_info.enc_out_format = eRGB;
    myHDMI_info.enc_color_depth = 8;
    myHDMI_info.colorimetry = eITU601;
    myHDMI_info.pix_repet_factor = 0;
    myHDMI_info.hdcp_enable = 0;
    myHDMI_info.video_mode->mCode = 4;  //1280x720p @ 59.94/60Hz 16:9
    myHDMI_info.video_mode->mHdmiDviSel = TRUE;
    myHDMI_info.video_mode->mRVBlankInOSC = FALSE;
    myHDMI_info.video_mode->mRefreshRate = 60000;
    myHDMI_info.video_mode->mDataEnablePolarity = TRUE;

    hdmi_av_frame_composer(&myHDMI_info);
    hdmi_video_packetize(myHDMI_info);
    hdmi_video_csc(myHDMI_info);
    hdmi_video_sample(myHDMI_info);

    hdmi_audio_mute(TRUE);
    hdmi_tx_hdcp_config(myHDMI_info.video_mode->mDataEnablePolarity);

    hdmi_phy_init(TRUE, myHDMI_info.video_mode->mPixelClock);

    hdmi_config_input_source(hdmi_src); // configure input source to HDMI block
}

static uint32_t panel_select(void)
{
    uint32_t index = 0;
    char str[2];
    char recvCh = NONE_CHAR;
    do {
        recvCh = getchar();
        
        if((recvCh >= '0') && (recvCh <= '9'))
        {
            str[index++] = recvCh;
        } else if(recvCh == 0x0D) {
            break;
        }
    } while (index < 2);
   
    if(index == 1)
        index = (uint32_t)(str[0] - '0');
    else 
        index = (uint32_t)(str[0] - '0') * 10 + (uint32_t) (str[1] - '0');

    return index;
}

/*!
 * IPU display test case
 *
 *@param  disp_dev_type  display panel type
 *
 */
static test_return_t ipu_display_test(int disp_dev_type)
{
    uint32_t disp_mem = CH23_EBA0;
    char recvCh = NONE_CHAR;
    ips_dev_panel_t *panel = NULL;
    int32_t ipu_index = HW_IPU1;
    int index = 0;

    ipu_iomux_config(ipu_index);
    ipu_sw_reset(ipu_index, 1000);

    switch (disp_dev_type) {
    case DISP_DEV_HDMI:
        hdmi_720p_init(ipu_index, 0);
        panel = search_panel("HDMI 720P 60Hz"); //supported
        break;
    case DISP_DEV_TFTLCD:
        /*TFT LCD test */
        printf("please select tft lcd type:(enter to confirm)\n");
        list_panel(disp_dev_type);
        index = panel_select();
        panel = get_panel_by_id(index);
        break;
    case DISP_DEV_LVDS:
        /*LVDS panel test */
        printf("please select lvds panel type:(enter to confirm)\n");
        list_panel(disp_dev_type);
        index = panel_select();
        panel = get_panel_by_id(index);
        break;
    case DISP_DEV_MIPI:
        panel = search_panel("TRULY MIPI TFT480800"); //supported
        break;
    default:
        printf("This type (%d) of display device can not be supported yet.\n", disp_dev_type);
        return TEST_FAILED;
    }

    if(panel == NULL)
        return TEST_FAILED;

    printf("****** testing display %s ******\n", panel->panel_name);
    panel->panel_init(&ipu_index);
    ipu_display_setup(ipu_index, CH23_EBA0, (uint32_t) NULL, INTERLEAVED_RGB565, panel);

    load_centralized_image(disp_mem, panel);
    ipu_enable_display(ipu_index);

    printf("Do you see the image (y or n) ?\n");
    do {
        recvCh = getchar();
    } while (recvCh == (uint8_t)0xFF);
    if(!(recvCh == 'Y' || recvCh == 'y'))
        return TEST_FAILED;
    else
        return TEST_PASSED;
}

static const char *display_type_name[] = {
    "NULL",
    "TFTLCD",
    "LVDS",
    "VGA",
    "HDMI",
    "TV",
    "MIPI"
};

int32_t ipu_display_panel[20];

/*!
 * IPU display test main entry
 *
 * @return  TEST_NOT_PRESENT if no IPU tests are enabled for the platform;
 *          TEST_BYPASSED if no IPU test enabled on the platform is chosen to run,
 *                        or no display at all on that platform;
 *          TEST_FAILED if any of the chosen tests fail;
 *          TEST_PASSED if all the chosen tests pass.
 */
test_return_t ipu_display_test_main(void)
{
    int i, fail = 0, pass = 0, panel_index;

    for (i = 0;; i++) {
        panel_index = ipu_display_panel[i];
        if (panel_index == DISP_DEV_NULL) {
            break;
        }
        printf("\n---- Running < %s > test\n", display_type_name[panel_index]);
        if (is_input_char('y', NULL)) {
            if (ipu_display_test(panel_index) != TEST_PASSED) {
                fail++;
                printf("\t %s test FAILED\n", display_type_name[panel_index]);
            } else {
                pass++;
                printf("\t %s test PASSED\n", display_type_name[panel_index]);
            }
        }
    }
    if (i == 0)
        return TEST_NOT_PRESENT;

    if (fail == 0 && pass == 0) {
        printf("WARNING: No display type selected\n");
        return TEST_BYPASSED;
    }

    if (fail != 0) {
        printf("\nFAILED: IPU display %d out of total %d selected tests\n", fail, fail + pass);
        return TEST_FAILED;
    }

    printf("\nPASSED: IPU display %d out of total %d selected tests\n", pass, fail + pass);
    return TEST_PASSED;
}

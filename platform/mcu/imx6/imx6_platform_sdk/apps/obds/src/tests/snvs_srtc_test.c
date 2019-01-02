/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
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
 * @file snvs_srtc_test.c
 * @brief Basic security RTC test of the SNVS module.
 *
 */

#include "obds.h"
#include "sdk.h"
#include "registers/regsocotp.h"
#include "registers/regssnvs.h"

const char g_snvs_srtc_test_name[] = "SNVS - SRTC Test";

/*!
 * The SRTC test enables the SRTC of the SNVS and check if the SRTC LP counter toggles.
 */
test_return_t snvs_srtc_test(void)
{
	const char* indent = menu_get_indent();

    // Check to see if Secure Clock can run
    // SEC_CONFIG[0] ( 0x440[7:0] bit 1 )
	//
	//    Combined with SEC_CONFIG[1], provide FAB/Open/Close security states:
	//    00 - FAB (Open)
	//    01 - Open - allows any code to be flashed and executed, even if it has no valid signature.
	//    1x -  Closed (Security On)
    unsigned int reg;
    reg = HW_OCOTP_CFG3_RD(); // readl(OCOTP_BASE_ADDR + 0x440);
    if ( (reg & 0x2) == 0 )
    {
    	printf("%s%s%sWARNING!! SNVS SRTC TEST MAY FAIL BECAUSE DEVICE IS IN AN INVALID\n", indent, NULL, g_TextColorYellow);
    	printf(    "%sSECURITY MODE. FREESCALE DOES NOT DISTRIBUTE BOARDS IN THIS MODE.\n", indent);
    	printf(    "%sPLEASE CONTACT YOUR FREESCALE REPRESENTATIVE FOR INSTRUCTIONS TO\n", indent);
    	printf(    "%sRECONFIGURE THIS DEVICE TO A VALID SECURITY MODE.%s\n", indent, g_TextAttributeDefault);

    	return TEST_BYPASSED;
    }
    else
    {
    	printf("%sSECURITY CONFIGURATION is OPEN.\n", indent);
    }

    // enable SNVS HP
    BW_SNVS_HPCR_RTC_EN(1); // writel(0x1, SNVS_HPCR);

    reg32_t c1 = HW_SNVS_HPRTCMR_RD(); // readl(SNVS_HPCMR);
    reg32_t c2 = HW_SNVS_HPRTCLR_RD(); // readl(SNVS_HPCLR);

    hal_delay_us(10000);

    if (c1 == HW_SNVS_HPRTCMR_RD() && c2 == HW_SNVS_HPRTCLR_RD())
    {
        printf("%sSNVS SRTC secure counter failed to run.\n", indent);

        return TEST_FAILED;
    }
    else
        printf("%sSNVS SRTC HP counter is running.\n", indent);

    // enable SNVS LP
    BW_SNVS_LPCR_SRTC_ENV(1); // writel(0x1, SNVS_LPCR);

    c1 = HW_SNVS_LPSRTCMR_RD(); // readl(SNVS_LPSCMR);
    c2 = HW_SNVS_LPSRTCLR_RD(); // readl(SNVS_LPSCLR);
    hal_delay_us(10000);

    if (c1 == HW_SNVS_LPSRTCMR_RD() && c2 == HW_SNVS_LPSRTCLR_RD())
    {
        printf("%sSNVS SRTC secure counter failed to run.\n", indent);

        return TEST_FAILED;
    }
    else
        printf("%sSNVS SRTC LP counter is running.\n", indent);


    return TEST_PASSED;
}

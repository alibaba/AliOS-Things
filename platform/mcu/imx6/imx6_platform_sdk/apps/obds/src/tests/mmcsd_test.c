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
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "obds.h"
#include "registers/regsusdhc.h"

int SDHC_UHSI_mode;
extern int card_detect_test_en;
extern int write_protect_test_en;
const char g_mmcsd_test_name[] = "MMC/SD Test";

extern test_return_t mmc_sd_test(unsigned int bus_width, uint32_t instance);
extern bool usdhc_card_detected(uint32_t instance);
extern bool usdhc_write_protected(uint32_t instance);

uint32_t mmcsd_bus_width, instance;
#if defined(CHIP_MX6SL) && defined(BOARD_EVK)
test_return_t mmcsd_test(void)
{
    int ret_val = TEST_NOT_STARTED;
 	const char* indent = menu_get_indent();

    /* Always try maximum bus width */
    mmcsd_bus_width = 8;

	/* USDHC2 test - Test only card detect (CD_B) and write_protect (WP)
	 * pins since we booted from this SD port.
	*/

	printf("\n%sTesting MMC/SD card on SD2\n", indent);

    instance = HW_USDHC2;

    // card detect
	if(usdhc_card_detected(instance) == false) {
		printf("%s*Card on SD2 is not inserted.\n", indent);
		ret_val = TEST_FAILED;
	} else {
		printf("%sCard on SD2 is inserted.\n", indent);
	}

    // write protect
	if(usdhc_write_protected(instance) == true) {
		printf("%s*Card on SD2 is write protected.\n", indent);
		ret_val = TEST_FAILED;
	} else {
		printf("%sCard on SD2 is not write protected.\n", indent);
	}

	if (ret_val == TEST_FAILED) {
		printf("\n%sTest for MMC/SD card on SD2 FAILED.\n", indent);
		ret_val = TEST_FAILED;
	}

	/* USDHC1 test */
	printf("\n%sPlease insert MMC/SD card into SD1\n", indent);

	if (!is_input_char('y', indent)) {
		printf("%sSkip MMC SD test on SD1\n", indent);
		ret_val = ret_val == TEST_FAILED ? TEST_FAILED : TEST_BYPASSED;
	} else {

		instance = HW_USDHC1;
		card_detect_test_en = true;
		write_protect_test_en = true;

		printf("%sU-HSI(make sure voltage switch circuit on board) mode test?\n", indent);
		if (is_input_char('y', indent)) {
			SDHC_UHSI_mode = 1;
		} else {
			SDHC_UHSI_mode = 0;
		}

		if (mmc_sd_test(mmcsd_bus_width, instance) == TEST_FAILED) {
			printf("%sTest for MMC/SD card on SD1 FAILED.\n", indent);

			ret_val = TEST_FAILED;
		}
	}

	/* USDHC3 test */
	printf("\n%sPlease insert MMC/SD card into SD3\n", indent);

	if (!is_input_char('y', indent)) {
		printf("%sSkip MMC SD test on SD3\n", indent);
		return ret_val == TEST_FAILED ? TEST_FAILED : TEST_BYPASSED;
	}

	instance = HW_USDHC3;
	card_detect_test_en = true;
	write_protect_test_en = false;

	printf("%sU-HSI(make sure voltage switch circuit on board) mode test?\n", indent);
	if (is_input_char('y', indent)) {
		SDHC_UHSI_mode = 1;
	} else {
		SDHC_UHSI_mode = 0;
	}

	if (mmc_sd_test(mmcsd_bus_width, instance) == TEST_FAILED) {
		printf("%sTest for MMC/SD card on SD3 FAILED.\n", indent);

		ret_val = TEST_FAILED;
	}

    if (ret_val == TEST_NOT_STARTED)
        printf("\n%sAll MMC/SD Tests PASSED.\n", indent);
    else
        printf("\n%s*Overall MMC/SD Test FAILED.\n", indent);

    return ret_val == TEST_NOT_STARTED ? TEST_PASSED : ret_val;

}
#else // #if defined(MX6SL) && defined(EVB)
test_return_t mmcsd_test(void)
{
    int ret_val = TEST_NOT_STARTED;
 	const char* indent = menu_get_indent();

    /* Always try maximum bus width */
    mmcsd_bus_width = 8;
#if ((!defined(CHIP_MX6SL) && defined(BOARD_EVB)) || defined(BOARD_SMART_DEVICE) )

    /* USDHC3 test - Test only card detect (CD_B) and write_protect (WP)
     * pins since we booted from this SD port.
    */
    printf("\n%sTesting MMC/SD boot card on SD3\n", indent);

    instance = HW_USDHC3;

    // card detect
    if(usdhc_card_detected(instance) == false) {
        printf("%s%s*Card on SD3 is not inserted.\n", indent, indent);
        ret_val = TEST_FAILED;
    } else {
        printf("%s%sCard on SD3 is inserted.\n", indent, indent);
    }

    // write protect
    if(usdhc_write_protected(instance) == true) {
        printf("%s%s*Card on SD3 is write protected.\n", indent, indent);
        ret_val = TEST_FAILED;
    } else {
        printf("%s%sCard on SD3 is not write protected.\n", indent, indent);
    }

    if (ret_val == TEST_FAILED) {
        printf("\n%s*Test for MMC/SD card on SD3 FAILED.\n", indent);
    }

    /* USDHC2 test */
    printf("\n%sTesting MMC/SD boot card on SD2\n", indent);
    printf("\n%sPlease insert an MMC/SD card into the SD2 slot.\n", indent);
    if (!is_input_char('y', indent)) {
        printf("%sSkip MMC SD test on SD2\n", indent);
        return ret_val == TEST_FAILED ? TEST_FAILED : TEST_BYPASSED;
    }
    instance = HW_USDHC2;
    card_detect_test_en = true;
    write_protect_test_en = true;

#elif defined(BOARD_SABRE_AI)

    /* USDHC3 test - Test only card detect (CD_B) and write_protect (WP)
	 * pins since we booted from this SD port.
	*/
	printf("\n%sTesting MMC/SD boot card on SD3\n", indent);

	instance = HW_USDHC3;

    // card detect
	if(usdhc_card_detected(instance) == false) {
		printf("%s%s*Card on SD3 is not inserted.\n", indent, indent);
		ret_val = TEST_FAILED;
	} else {
		printf("%s%sCard on SD3 is inserted.\n", indent, indent);
	}

    // write protect
	if(usdhc_write_protected(instance) == true) {
		printf("%s%s*Card on SD3 is write protected.\n", indent, indent);
		ret_val = TEST_FAILED;
	} else {
		printf("%s%sCard on SD3 is not write protected.\n", indent, indent);
	}

	if (ret_val == TEST_FAILED) {
		printf("\n%s*Test for MMC/SD card on SD3 FAILED.\n", indent);
	}

    printf("\n%sPlease insert an MMC/SD card into the SD slot on the bottom of the main board.\n", indent);
    if (!is_input_char('y', indent)) {
        printf("%sSkip MMC SD test on SD1\n", indent);
		return ret_val == TEST_FAILED ? TEST_FAILED : TEST_BYPASSED;
    }
    instance = HW_USDHC1;
	card_detect_test_en = true;
	write_protect_test_en = true;

#else
    printf("\n%sPlease make sure to insert an MMC/SD card into SD slot 2 \n", indent);
    if (!is_input_char('y', indent)) {
        printf("%sSkip MMC SD test \n", indent);
    	
        return TEST_BYPASSED;
    }

    instance = HW_USDHC2;
#endif

#if (!defined(CHIP_MX6SL) && defined(BOARD_EVB))
    /* Test for voltage switch & reset */
    printf("%sU-HSI(make sure voltage switch circuit on board) mode test?\n", indent);
    if (is_input_char('y', indent)) {
        SDHC_UHSI_mode = 1;
    } else {
        SDHC_UHSI_mode = 0;
    }
#endif

    if (mmc_sd_test(mmcsd_bus_width, instance) == TEST_FAILED) {
		printf("\n%s*Test for MMC/SD card on SD%d FAILED.\n", indent, instance);
		ret_val = TEST_FAILED;
	}

	if (ret_val == TEST_NOT_STARTED)
		printf("\n%sAll MMC/SD Tests PASSED.\n", indent);
	else
		printf("\n%s*Overall MMC/SD Test FAILED.\n", indent);

	return ret_val == TEST_NOT_STARTED ? TEST_PASSED : ret_val;
}
#endif


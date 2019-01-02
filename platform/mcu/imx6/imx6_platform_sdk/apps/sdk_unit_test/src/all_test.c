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
#include "sdk.h"
#include "sdk_version.h"
#include "menu.h"

#ifdef ALL_TEST_ENABLE

////////////////////////////////////////////////////////////////////////////////
// Externs
////////////////////////////////////////////////////////////////////////////////

extern int camera_test(void);
extern int eim_test();
extern int gpmi_test(void);
extern int mipi_test();
extern int pwm_test();
extern int spi_test(void);
extern int vpu_test(void);
extern int wdog_test(void);
extern void audio_test(void);
extern void enet_test(void);
extern void epit_test(void);
extern void flexcan_test(void);
extern void gic_test(void);
extern void gpt_test(void);
extern void hdmi_test(void);
extern void i2c_test(void);
extern void ipu_test(void);
extern void microseconds_test(void);
extern void ocotp_test(void);
extern void sdma_test(void);
extern void snvs_rtc_test(void);
extern void snvs_srtc_test(void);
extern void tempmon_test(void);
extern void uart_test(void);
extern void usb_test(void);
extern void usdhc_test(void);
extern void keypad_test(void);
extern void accelerometer_test(void);
extern void multicore_test(void);
extern int cpu_wp_test(void);

#ifdef CHIP_MX6DQ
extern int sata_test(void);
#endif
#if defined(BOARD_EVB) || defined(BOARD_SMART_DEVICE) || defined(BOARD_SABRE_AI) 
extern int pcie_test(void);
#endif

#if defined(CHIP_MX6SL)
extern int camera_test(void);
extern int fec_test(void);
extern int lcdif_test(void);
extern void epdc_test(void);
extern void spdc_test(void);
#endif

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Macro to help create test menu items.
#define DEFINE_TEST_MENU_ITEM(k, m, t) MENU_MAKE_MENUITEM(k, m, run_test, t)

//! @brief Typedef for one of the test functions.
typedef void (*test_function_t)(void);

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

menu_action_t run_test(void* param);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//! @brief Menu items for the test selection menu.
const menuitem_t k_menuItems[] = {
        // Common tests for all chips and boards
        DEFINE_TEST_MENU_ITEM("e",  "epit test",        epit_test),
        DEFINE_TEST_MENU_ITEM("g",  "gpt test",         gpt_test),
        DEFINE_TEST_MENU_ITEM("i",  "i2c test",         i2c_test),
        DEFINE_TEST_MENU_ITEM("w",  "pwm test",         pwm_test),
        DEFINE_TEST_MENU_ITEM("s",  "sdma test",        sdma_test),
        DEFINE_TEST_MENU_ITEM("r",  "snvs rtc test",    snvs_rtc_test),
        DEFINE_TEST_MENU_ITEM("v",  "snvs srtc test",   snvs_srtc_test),
        DEFINE_TEST_MENU_ITEM("t",  "tempmon test",     tempmon_test),
        DEFINE_TEST_MENU_ITEM("u",  "uart test",        uart_test),
        DEFINE_TEST_MENU_ITEM("d",  "usdhc test",       usdhc_test),
        DEFINE_TEST_MENU_ITEM("c",  "gic test",         gic_test),
        DEFINE_TEST_MENU_ITEM("m",  "microseconds timer test", microseconds_test),
        DEFINE_TEST_MENU_ITEM("wa", "watchdog test",    wdog_test),
        DEFINE_TEST_MENU_ITEM("o",  "ocotp test",       ocotp_test),
        DEFINE_TEST_MENU_ITEM("wp", "cpu workpoint test", cpu_wp_test),

        // mx6dq and mx6sdl are grouped together because they share the same boards.
#if defined(CHIP_MX6DQ) || defined(CHIP_MX6SDL)
        // Tests for all boards of mx6dq and mx6sdl.
        DEFINE_TEST_MENU_ITEM("h",  "hdmi test",        hdmi_test),
        DEFINE_TEST_MENU_ITEM("ip", "ipu test",         ipu_test),
        DEFINE_TEST_MENU_ITEM("mc", "multicore test",   multicore_test),
        
#if defined(CHIP_MX6DQ)
        // The sata test only applies to the mx6dq.
        DEFINE_TEST_MENU_ITEM("sa", "sata test",        sata_test),
#endif // defined(CHIP_MX6DQ)
        
        // Tests that apply only to particular boards for either the mx6dq or mx6sdl.
#if defined(BOARD_EVB)
        DEFINE_TEST_MENU_ITEM("a",  "audio test",       audio_test),
        DEFINE_TEST_MENU_ITEM("mi", "mipi test",        mipi_test),
        DEFINE_TEST_MENU_ITEM("pc", "pcie test",        pcie_test),
        DEFINE_TEST_MENU_ITEM("gp", "gpmi test",        gpmi_test),
        DEFINE_TEST_MENU_ITEM("n",  "spi nor test",     spi_test),
        DEFINE_TEST_MENU_ITEM("en", "enet test",    	enet_test),
        DEFINE_TEST_MENU_ITEM("us", "usb test",     	usb_test),
        DEFINE_TEST_MENU_ITEM("ca", "camera test",      camera_test),
#elif defined(BOARD_SABRE_AI)
        DEFINE_TEST_MENU_ITEM("ei",  "eim test",        eim_test),
        DEFINE_TEST_MENU_ITEM("mi", "mipi test",        mipi_test),
        DEFINE_TEST_MENU_ITEM("f",  "flexcan test",     flexcan_test),
        DEFINE_TEST_MENU_ITEM("us", "usb test",         usb_test),
        DEFINE_TEST_MENU_ITEM("n",  "spi nor test",     spi_test),
        DEFINE_TEST_MENU_ITEM("pc", "pcie test",        pcie_test),
        DEFINE_TEST_MENU_ITEM("ca", "camera test",      camera_test),
        DEFINE_TEST_MENU_ITEM("x",  "accelerometer test", accelerometer_test),
#elif defined(BOARD_SMART_DEVICE)
        DEFINE_TEST_MENU_ITEM("a",  "audio test",       audio_test),
        DEFINE_TEST_MENU_ITEM("ca", "camera test",      camera_test),
        DEFINE_TEST_MENU_ITEM("us", "usb test",     	usb_test),
        DEFINE_TEST_MENU_ITEM("n",  "spi nor test",     spi_test),
        DEFINE_TEST_MENU_ITEM("pc", "pcie test",        pcie_test),
		DEFINE_TEST_MENU_ITEM("en", "enet test",		enet_test),
		DEFINE_TEST_MENU_ITEM("k", "keypad test",      keypad_test),
        DEFINE_TEST_MENU_ITEM("x",  "accelerometer test", accelerometer_test),
#endif // defined(BOARD_SMART_DEVICE)

#elif defined(CHIP_MX6SL)
        // Tests for mx6sl evk.
        DEFINE_TEST_MENU_ITEM("ca", "camera test",      camera_test),
        DEFINE_TEST_MENU_ITEM("l",  "lcdif test",       lcdif_test),
        DEFINE_TEST_MENU_ITEM("ep",  "epdc test",        epdc_test),
        DEFINE_TEST_MENU_ITEM("sp", "spdc test",        spdc_test),
        DEFINE_TEST_MENU_ITEM("us", "usb test",     	usb_test),
        DEFINE_TEST_MENU_ITEM("n",  "spi nor test",     spi_test),
        DEFINE_TEST_MENU_ITEM("a",  "audio test",       audio_test),
        DEFINE_TEST_MENU_ITEM("f",	"fec test",			fec_test),
#endif // defined(CHIP_MX6SL)
        
        // Quit menu item
        MENU_MAKE_MENUITEM_EXIT(),
        
        // Menu terminator
        MENU_MAKE_MENUITEM_END()
    };

//! @brief The test selection menu.
const menu_t k_mainMenu = {
        .header = "SDK Unit Tests\n"
                  "  --------------",
        .menuitems = k_menuItems,
        .footer = "Select test to run:"
    };

//! @brief Action function to call the test function passed in as a parameter.
menu_action_t run_test(void* param)
{
    test_function_t testFunction = (test_function_t)param;
    testFunction();
    
    return MenuAction_Show;
}

void ALL_test(void)
{
    // Just run the test selection menu.
    menu_present(&k_mainMenu);
}

#endif // ALL_TEST_ENABLE

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

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
 * @defgroup diag_init Base OBDS files
 */

/*!
 * @file  main.c
 * @brief main() function
 *
 * @ingroup diag_init
 */

//#include <stdio.h>
#include "sdk.h"
#include "board_id/board_id.h"
#include "obds.h"
#include "obds_utils.h"
#include "platform_init.h"
#include "text_color.h"
#include "sdk_version.h"
#include "print_clock_info.h"
#include "print_version.h"


int total_tests = 0;
int auto_run_enable = 1; // global flag to indicate auto-run feature enabled or not
menuitem_t main_menuitems[MAX_TESTS];
obds_test_t obds_tests[MAX_TESTS];

/*!
 * main function that decides which tests to run and prompts the user before
 * running test.
 * @return      should never return because ARM is at WFI (low power mode in the end)
 */
int main(void)
{
    auto_run_enable = 1;

    platform_init();
    printf(g_TextAttributeDefault); // reset terminal colors to default
    print_version();

    show_freq();
    show_ddr_config();
    show_boot_switch_info();

    //
    // Create the test list (menuitems) for the board.
    //
    memset(obds_tests, 0, MAX_TESTS * sizeof(obds_test_t));
    select_tests(main_menuitems, SelectTests_Menu);

    //
    // Initialize the test results list.
    //
    memset(obds_tests, 0, MAX_TESTS * sizeof(obds_test_t));
    int mi = 0, ti = 0;
    for(mi=0; main_menuitems[mi].type != MenuItem_Null; ++mi)
    {
        if (main_menuitems[mi].func_ptr != (menu_function_t)run_test)
            continue;

        obds_tests[ti].key = main_menuitems[mi].key;
        obds_tests[ti].description = main_menuitems[mi].description;
        obds_tests[ti].test_function = (obds_test_function_t)main_menuitems[mi].func_param;
        obds_tests[ti].result_node = (test_result_node_t*)malloc(sizeof(test_result_node_t));
        obds_tests[ti].result_node->result = TEST_NOT_STARTED;
        obds_tests[ti].result_node->next = NULL;
        ++ti;
    }

    //
    // Create the main menu header
    //
    char chip_str[64];
    char chip_rev_str[64];
    char board_str[64];
    char board_rev_str[64];

    fsl_board_id_t fsl_board_id = get_board_id();

    chip_name(chip_str, fsl_board_id.B.CHIP_TYPE_ID, false);
    chip_revision(chip_rev_str, fsl_board_id.B.CHIP_REV_MAJOR, fsl_board_id.B.CHIP_REV_MINOR);
    board_name(board_str, BOARD_TYPE);
    board_revision(board_rev_str, BOARD_REVISION);

    char main_menu_desc[1024];
    sprintf(main_menu_desc, "This is an interactive test menu for %s %s %s %s.",
            chip_str, chip_rev_str, board_str, board_rev_str);

    //
    // Create the main menu
    //
    const menu_t main_menu = {
            .header = main_menu_desc,
            .menuitems = main_menuitems,
            .footer = "Enter test number followed by the enter key, 'm' for menu, or 'q' to exit."
    };

    //
    // Show the main menu
    //
    menu_present(&main_menu);

    //
    // Report the results of the tests.
    //
    report_test_results();


    free_test_results();

    return 0;
}

void free_test_results(void)
{
    for (int i=0; obds_tests[i].key == NULL; ++i)
    {
    test_result_node_t* result = obds_tests[i].result_node;
        while(result)
        {
            test_result_node_t* temp = result;
            result = temp->next;
            free(temp);
        }
    }
}


int printf_hook(const char *format, ...)
{
     int n = 0;
     va_list args;
     va_start(args, format);

     char param_format[8192];
     char obds_format[8192] = {'\0'};
     char token_buffer[8192] = {'\0'};
     const char* indent = menu_get_indent();
     strcpy(param_format, format);


     if ( format[0] == '\n' || format[0] == '\r')
         sprintf(obds_format, "%s\n", indent);

     const char separators[]   = "\r\n";

     char *token;
     token = strtok(param_format, separators);
     while( token != NULL )
    {
       sprintf(token_buffer, "%s%s\n", indent, token);
       strcat(obds_format, token_buffer);

       token = strtok( NULL, separators);
    }

     /* If needed, put your change to arguments in here,
      * char *argv = NULL;
      * argv = va_arg(args, char *);
      * argv ?
      */

     n = vprintf("inside obds_printf\n", args);

     n = vprintf(format, args);
     va_end(args);
     return n;
}

test_return_t prompt_run_test(const char * const test_name, const char* const indent)
{
    printf_color(g_TextAttributeBold, NULL, "\n%s---- Running < %s >\n", indent, test_name);
    if (!auto_run_enable)
    {
        if (!is_input_char('y', indent))
        {
            print_test_skipped(test_name, indent);
            return TEST_BYPASSED;
        }
    }
    else
        printf("\n");

    return TEST_CONTINUE;
}

void print_test_passed(const char* const test_name, const char* const indent)
{
    printf_color(NULL, g_TextColorGreen, "\n%s%s PASSED.\n", indent, test_name);
}

void print_test_skipped(const char* const test_name, const char* const indent)
{
     printf_color(NULL, g_TextColorYellow, "\n%s%s SKIPPED.\n", indent, test_name);
}

void print_test_failed(const char* const test_name, const char* const indent)
{
     printf_color(g_TextAttributeBold, g_TextColorRed, "\n%s%s FAILED.\n", indent, test_name);
}

void print_test_not_implemented(const char* const test_name, const char* const indent)
{
     printf_color(g_TextAttributeBold, g_TextColorRed, "\n%s%s is NOT IMPLEMENTED.\n", indent, test_name);
}

//
// PRIVATE
//

//! @brief Action function to call the test function passed in as a parameter.
menu_action_t run_test(void* param)
{
    test_return_t testResult = TEST_NOT_STARTED;

    const char* const indent = menu_get_indent();
    obds_test_function_t testFunction = (obds_test_function_t)param;

    int i = 0;
    while (obds_tests[i].test_function != testFunction) ++i;
    test_result_node_t* result_node = obds_tests[i].result_node;

    if ( prompt_run_test(obds_tests[i].description, indent) != TEST_CONTINUE )
        testResult = TEST_BYPASSED;
    else
        testResult = testFunction();

    while (result_node->next != NULL)
        result_node = result_node->next;
    result_node->result = testResult;
    result_node->next = (test_result_node_t*) malloc(sizeof(test_result_node_t));
    result_node->next->result = TEST_NOT_STARTED;
    result_node->next->next = NULL;

    switch(testResult)
    {
        case TEST_BYPASSED:
            print_test_skipped(obds_tests[i].description, indent);
            break;
        case TEST_FAILED:
            print_test_failed(obds_tests[i].description, indent);
            break;
        case TEST_PASSED:
            print_test_passed(obds_tests[i].description, indent);
            break;
        case TEST_NOT_IMPLEMENTED:
            print_test_not_implemented(obds_tests[i].description, indent);
            break;
        case TEST_NOT_PRESENT:
        case TEST_CONTINUE:
        case TEST_NOT_STARTED:
            break;
    }

    return MenuAction_Continue;
}

//
// Report the results of the tests.
// NOTE: Only the last test result for a given test is reflected
//       in the summary if a test was run more than once from the menu.
//
void report_test_results(void)
{
    int passed = 0, failed = 0, not_implemented = 0, skipped = 0, not_run = 0, not_present = 0, unknown = 0;
    int i;

    printf("\n----------------- TEST RESULTS -----------------\n");
    for (i = 0; obds_tests[i].key != NULL; i++)
    {
        test_result_node_t* test_result = obds_tests[i].result_node;
        int test_pass = 1;
        while ( test_result != NULL && test_pass > 1 )
        {
            if (test_result->result == TEST_NOT_PRESENT)
            {
                ++not_present;
            }
            else if (test_result->result == TEST_PASSED)
            {
                printf_color(NULL/*g_TextAttributeBold*/, g_TextColorGreen, "\t<%s> pass %d PASSED\n", obds_tests[i].description, test_pass);
                ++passed;
            }
            else if (test_result->result == TEST_FAILED)
            {
                printf_color(g_TextAttributeBold, g_TextColorRed, "\t<%s> pass %d FAILED\n", obds_tests[i].description, test_pass);
                ++failed;
            }
            else if (test_result->result == TEST_NOT_IMPLEMENTED)
            {
                printf_color(g_TextAttributeBold, g_TextColorRed, "\t<%s> is NOT IMPLEMENTED\n", obds_tests[i].description);
                ++not_implemented;
            }
            else if (test_result->result == TEST_BYPASSED)
            {
                printf_color(NULL/*g_TextAttributeBold*/, g_TextColorYellow, "\t<%s> SKIPPED\n", obds_tests[i].description);
                ++skipped;
            }
            else if (test_result->result == TEST_NOT_STARTED)
            {
                printf_color(g_TextAttributeDim, g_TextColorGray, "\t<%s> NOT RUN\n", obds_tests[i].description);
                ++not_run;
            }
            else
            {
                ++unknown;
            }

            test_result = test_result->next;
            ++test_pass;
        }
    }

    printf("\n=================== SUMMARY ====================\n");
    printf("PASSED:          \t%d\n", passed);
    printf("FAILED:          \t%d\n", failed);
    printf("NOT IMPLEMENTED: \t%d\n", not_implemented);
    printf("SKIPPED:         \t%d\n", skipped);
    printf("NOT RUN:         \t%d\n", not_run);
    printf("TOTAL:           \t%d\n", MAX_TESTS - not_present - unknown);
    printf("=================================================\n");
}

menu_action_t run_all_tests(void* const param)
{
    select_tests_t tests = SelectTests_CpuAndMainBoard;
    const char* indent = menu_get_indent();

    if (BOARD_TYPE == BOARD_TYPE_SABRE_AI || BOARD_TYPE == BOARD_TYPE_EVB)
    {
        printf("%sDo you only wish to run the CPU Board set of tests?\n", indent);
        if (is_input_char('y', indent)) {
            printf("%s  Run CPU Board ONLY tests.\n\n", indent);
            tests = SelectTests_CpuBoardOnly;
        }
        else
        {
            printf("%sDo you only wish to run the Main Board set of tests?\n", indent);
            if (is_input_char('y', indent)) {
                printf("%s  Run MAIN Board ONLY tests.\n\n", indent);
                tests = SelectTests_MainBoardOnly;
            }
            else
            {
                printf("%s  Run CPU & MAIN Board tests.\n\n", indent);
                tests = SelectTests_CpuAndMainBoard;
            }
        }
    }
    
    select_tests(main_menuitems, tests);

    printf("%sEnable auto-run feature?\n", indent);
    if (is_input_char('y', indent)) {
        printf("%s  Auto-run enabled.\n\n", indent);
        auto_run_enable = 1;
    } else {
        auto_run_enable = 0;
    }

    // run through the tests starting at 1 since run_all_tests() is at 0.
    int test_idx;
    for (test_idx = 1; test_idx < MAX_TESTS; test_idx++)
    {
        if ( main_menuitems[test_idx].func_ptr &&
             strcmp(main_menuitems[test_idx].key, "m") != 0 &&
             strcmp(main_menuitems[test_idx].key, "q") != 0 )
        {
            // This calls run_test() which handles all the prompting and return results.
            main_menuitems[test_idx].func_ptr(main_menuitems[test_idx].func_param);
        }
    }

    return MenuAction_Exit;
}

void select_tests(menuitem_t menuitems[], const select_tests_t select_tests)
{
    memset(menuitems, 0, sizeof(menuitem_t) * MAX_TESTS);
    menuitems[0].type = MenuItem_Null;

    menu_append_menuitems(menuitems, MAX_TESTS, MenuList_All_Tests);

#if defined(BOARD_SABRE_AI)

    int display = 0;

    if ( select_tests != SelectTests_MainBoardOnly )
    {
        menu_append_menuitems(menuitems, MAX_TESTS, CPU_Tests);
        ipu_display_panel[display++] = DISP_DEV_TFTLCD;
        ipu_display_panel[display++] = DISP_DEV_LVDS;
        ipu_display_panel[display++] = DISP_DEV_HDMI;
    }
    if ( select_tests != SelectTests_CpuBoardOnly )
    {
        menu_append_menuitems(menuitems, MAX_TESTS, MainBoard_Tests);
        if ( select_tests == SelectTests_MainBoardOnly )
        {
            menu_append_menuitems(menuitems, MAX_TESTS, Display_Test);
            ipu_display_panel[display++] = DISP_DEV_LVDS;
        }
    }

    ipu_display_panel[display++] = DISP_DEV_NULL;

#endif // defined(BOARD_SABRE_AI)

#if defined(BOARD_SMART_DEVICE)

    int display = 0;

    menu_append_menuitems(menuitems, MAX_TESTS, CPU_Tests);
    ipu_display_panel[display++] = DISP_DEV_TFTLCD;
    ipu_display_panel[display++] = DISP_DEV_LVDS;
    ipu_display_panel[display++] = DISP_DEV_HDMI;
    ipu_display_panel[display++] = DISP_DEV_NULL;

#endif // defined(BOARD_SMART_DEVICE)

#if defined(CHIP_MX6SL) && defined(BOARD_EVK)

    menu_append_menuitems(menuitems, MAX_TESTS, CPU_Tests);

#endif // defined(CHIP_MX6SL) && defined(BOARD_EVK)

    menu_append_menuitems(menuitems, MAX_TESTS, Menu_Commands);
    
}

const menuitem_t MenuList_All_Tests[] =
{
    MENU_MAKE_MENUITEM("00", "RUN ALL TESTS", run_all_tests, NULL),
    MENU_MAKE_MENUITEM_END()
};

const menuitem_t Menu_Commands[] =
{
    MENU_MAKE_MENUITEM_GROUP("Menu functions"),
    MENU_MAKE_MENUITEM_SHOW(),
    MENU_MAKE_MENUITEM_EXIT(),
    MENU_MAKE_MENUITEM_END()
};

#if defined (CHIP_MX6DQ) || defined(CHIP_MX6SDL)
#if defined(BOARD_SABRE_AI)

    const menuitem_t CPU_Tests[] =
    {
        MENU_MAKE_MENUITEM_GROUP("CPU Board Tests"),
        MENU_MAKE_MENUITEM("01", g_program_board_id_test_name, run_test, program_board_id),
        MENU_MAKE_MENUITEM("02", g_program_mac_address_test_name, run_test, program_mac_address),
        MENU_MAKE_MENUITEM("03", g_ddr_test_name, run_test, ddr_test),
        MENU_MAKE_MENUITEM("04", g_snvs_srtc_test_name, run_test, snvs_srtc_test),
#if defined(BOARD_REV_A)
        // use micrel ethernet for rev A board
        MENU_MAKE_MENUITEM("05", g_KSZ9021RN_test_name, run_test, KSZ9021RN_test_main),
#else
        // use Atheros ethernet for all other revs (rev B)
        MENU_MAKE_MENUITEM("05", g_ar8031_test_name, run_test, ar8031_test_main),
#endif
        MENU_MAKE_MENUITEM("06", g_eim_NOR_flash_test_name, run_test, eim_nor_flash_test),
        MENU_MAKE_MENUITEM("07", g_max7310_i2c_device_id_test_name, run_test, max7310_i2c_device_id_check),
        MENU_MAKE_MENUITEM("08", g_hs_tsc_p1003_i2c_device_id_test_name, run_test, i2c_device_id_check_p1003),
        MENU_MAKE_MENUITEM("09", g_pmic_pf0100_i2c_device_id_test_name, run_test, pf0100_i2c_device_id_check),
        MENU_MAKE_MENUITEM("10", g_spi_nor_test_name, run_test, spi_nor_test),
        MENU_MAKE_MENUITEM("11", g_usb_otg_dev_enum_test_name, run_test, usbo_dev_enum_test),
        MENU_MAKE_MENUITEM("12", g_uart_test_name, run_test, uart_test),
        MENU_MAKE_MENUITEM("13", g_gpio_led_test_name, run_test, gpio_led_test),
        MENU_MAKE_MENUITEM("14", g_ipu_display_tests_name, run_test, ipu_display_test_main),
#if defined(CHIP_MX6DQ)
        MENU_MAKE_MENUITEM("15", g_sata_test_name, run_test, sata_test),
#endif
        MENU_MAKE_MENUITEM_END()
    };


    const menuitem_t MainBoard_Tests[] =
    {
        MENU_MAKE_MENUITEM_GROUP("Main Board Tests"),
        MENU_MAKE_MENUITEM("20", g_video_dec_test_name, run_test, adv7180_test_main),
        MENU_MAKE_MENUITEM("21", g_audio_esai_test_name, run_test, esai_test),
        MENU_MAKE_MENUITEM("22", g_usb_host1_dev_enum_test_name, run_test, usbh_dev_enum_test),
        MENU_MAKE_MENUITEM("23", g_cs42888_i2c_device_id_test_name, run_test, i2c_device_check_cs42888),
        MENU_MAKE_MENUITEM("24", g_isl29023_i2c_device_id_test_name, run_test, i2c_device_id_check_isl29023),
        MENU_MAKE_MENUITEM("25", g_mag3110_i2c_device_id_test_name, run_test, i2c_device_id_check_mag3110),
        MENU_MAKE_MENUITEM("26", g_mma8451_i2c_device_id_test_name, run_test, i2c_device_id_check_MMA8451),
        MENU_MAKE_MENUITEM("27", g_android_buttons_test_name, run_test, android_buttons_test),
        MENU_MAKE_MENUITEM("28", g_flexcan_test_name, run_test, flexcan_test),
        MENU_MAKE_MENUITEM("29", g_ard_main_board_reset_button_test, run_test, ard_mb_reset_test),
        MENU_MAKE_MENUITEM("30", g_ard_mb_expander_reset_test_name, run_test, ard_mb_expander_reset_test),
        MENU_MAKE_MENUITEM("31", g_mmcsd_test_name, run_test, mmcsd_test),
        MENU_MAKE_MENUITEM_END()
    };

    const menuitem_t Display_Test[] =
    {
        MENU_MAKE_MENUITEM("49", g_ipu_display_tests_name, run_test, ipu_display_test_main),
        MENU_MAKE_MENUITEM_END()
    };

#elif defined(BOARD_SMART_DEVICE)

    const menuitem_t CPU_Tests[] =
    {
        MENU_MAKE_MENUITEM_GROUP("CPU Board Tests"),
        MENU_MAKE_MENUITEM("01", g_program_board_id_test_name, run_test, program_board_id),
        MENU_MAKE_MENUITEM("02", g_program_mac_address_test_name, run_test, program_mac_address),
        MENU_MAKE_MENUITEM("03", g_ddr_test_name, run_test, ddr_test),
        MENU_MAKE_MENUITEM("04", g_snvs_srtc_test_name, run_test, snvs_srtc_test),
        MENU_MAKE_MENUITEM("05", g_ar8031_test_name, run_test, ar8031_test_main),
        MENU_MAKE_MENUITEM("06", g_hs_tsc_p1003_i2c_device_id_test_name, run_test, i2c_device_id_check_p1003),
        MENU_MAKE_MENUITEM("07", g_pmic_pf0100_i2c_device_id_test_name, run_test, pf0100_i2c_device_id_check),
        MENU_MAKE_MENUITEM("08", g_spi_nor_test_name, run_test, spi_nor_test),
        MENU_MAKE_MENUITEM("09", g_usb_otg_dev_enum_test_name, run_test, usbo_dev_enum_test),
        MENU_MAKE_MENUITEM("10", g_uart_test_name, run_test, uart_test),
        MENU_MAKE_MENUITEM("11", g_gpio_led_test_name, run_test, gpio_led_test),
        MENU_MAKE_MENUITEM("12", g_ipu_display_tests_name, run_test, ipu_display_test_main),
        MENU_MAKE_MENUITEM("13", g_audio_ssi_test_name, run_test, i2s_audio_test),
        MENU_MAKE_MENUITEM("14", g_isl29023_i2c_device_id_test_name, run_test, i2c_device_id_check_isl29023),
        MENU_MAKE_MENUITEM("15", g_mag3110_i2c_device_id_test_name, run_test, i2c_device_id_check_mag3110),
        MENU_MAKE_MENUITEM("16", g_mma8451_i2c_device_id_test_name, run_test, i2c_device_id_check_MMA8451),
        MENU_MAKE_MENUITEM("17", g_mmcsd_test_name, run_test, mmcsd_test),
        MENU_MAKE_MENUITEM("18", g_camera_test_name, run_test, camera_test),
#if defined(CHIP_MX6DQ)
        MENU_MAKE_MENUITEM("19", g_sata_test_name, run_test, sata_test),
#endif
#if defined(CHIP_MX6SDL)
        MENU_MAKE_MENUITEM("19", g_epdc_test_name, run_test, epdc_test),
#endif
        
        MENU_MAKE_MENUITEM_END()
    };

#elif defined(BOARD_EVB)
#if defined(CHIP_MX6DQ)
//    MENU_MAKE_MENUITEM("12", g_sata_test_name, run_test, sata_test),
#endif
#endif
#elif defined(CHIP_MX6SL)
#if defined(BOARD_EVK)

    const menuitem_t CPU_Tests[] =
    {
        MENU_MAKE_MENUITEM_GROUP("CPU Board Tests"),
        MENU_MAKE_MENUITEM("01", g_program_board_id_test_name, run_test, program_board_id),
        MENU_MAKE_MENUITEM("02", g_program_mac_address_test_name, run_test, program_mac_address),
        MENU_MAKE_MENUITEM("03", g_ddr_test_name, run_test, ddr_test),
        MENU_MAKE_MENUITEM("04", g_snvs_srtc_test_name, run_test, snvs_srtc_test),
        MENU_MAKE_MENUITEM("05", g_fec_enet_test_name, run_test, fec_test),
        MENU_MAKE_MENUITEM("06", g_mmcsd_test_name, run_test, mmcsd_test),
        MENU_MAKE_MENUITEM("07", g_max17135_i2c_device_id_test_name, run_test, i2c_device_id_check_MAX17135),
        MENU_MAKE_MENUITEM("08", g_mma8450_i2c_device_id_test_name, run_test, i2c_device_id_check_MMA8450),
        MENU_MAKE_MENUITEM("09", g_pmic_pf0100_i2c_device_id_test_name, run_test, pf0100_i2c_device_id_check),
        MENU_MAKE_MENUITEM("10", g_spi_nor_test_name, run_test, spi_nor_test),
        MENU_MAKE_MENUITEM("11", g_usb_otg_dev_enum_test_name, run_test, usbo_dev_enum_test),
        MENU_MAKE_MENUITEM("12", g_uart_test_name, run_test, uart_test),
        MENU_MAKE_MENUITEM("13", g_audio_ssi_test_name, run_test, i2s_audio_test),
        MENU_MAKE_MENUITEM("14", g_keypad_port_test_name, run_test, test_gpio_keyboard),
        MENU_MAKE_MENUITEM("15", g_usb_host1_dev_enum_test_name, run_test, usbh_dev_enum_test),
        MENU_MAKE_MENUITEM("16", g_camera_test_name, run_test, camera_test),
        MENU_MAKE_MENUITEM("17", g_epdc_test_name, run_test, epdc_test),
        MENU_MAKE_MENUITEM("18", g_lcd_display_test_name, run_test, lcdif_display_test),
        MENU_MAKE_MENUITEM_END()
    };


    const menuitem_t MainBoard_Tests[] =
    {
        MENU_MAKE_MENUITEM_GROUP("Main Board Tests"),
        MENU_MAKE_MENUITEM_END()
    };

#endif
#endif

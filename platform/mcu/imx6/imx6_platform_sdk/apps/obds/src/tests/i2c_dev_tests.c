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

#include "obds.h"

//extern int i2c_device_id_check_MMA8451(unsigned int i2c_base_addr);
//extern int i2c_device_id_check_isl29023(unsigned int i2c_base_addr);
//extern int i2c_device_id_check_mag3112(unsigned int i2c_base_addr);
//extern int i2c_device_id_check_mag3110(unsigned int i2c_base_addr);
extern int i2c_device_check_max11801(unsigned int i2c_base_addr);
//extern int i2c_device_check_cs42888(void);
//extern int i2c_device_id_check_p1003(unsigned int i2c_base_addr);
extern int i2c_device_id_check_emc1046(unsigned int i2c_base_addr);

extern int i2c_device_id_check_MMA8450(unsigned int i2c_base_addr);
extern int i2c_device_id_check_ppl3115(unsigned int i2c_base_addr);
extern int i2c_device_id_check_mc1323(unsigned int i2c_base_addr);

int i2c_device_id_check_mag3112_test_enable = 1;
int i2c_device_id_check_mag3110_test_enable = 1;
int i2c_device_id_check_isl29023_test_enable = 1;
int i2c_device_id_check_mma8451_test_enable = 1;
int i2c_device_id_check_p1003_test_enable = 1;
int i2c_device_id_check_cs42888_test_enable = 1;

static const char * const i2c_device_test_name = "I2C Device ID Test";

/*!
 * This test performs i2c device id check 
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
test_return_t i2c_device_id_check(void)
{
	int rc = 0;
    int test_count = 0;
    const char* indent = menu_get_indent();

    if ( prompt_run_test(i2c_device_test_name, indent) != TEST_CONTINUE )
    	return TEST_BYPASSED;
    
#if (BOARD_TYPE == BOARD_TYPE_SMART_DEVICE)
        //  USB_OTG_PWR_EN (EIM_D22)
        writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
        gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
        gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
        hal_delay_us(1000);
        gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);

        //rc |= i2c_device_id_check_mc1323(I2C2_BASE_ADDR);
        if (i2c_device_id_check_mma8451_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_MMA8451();  // accelerometer
        }
        if (i2c_device_id_check_isl29023_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_isl29023(); // light sensor
        }
        if (i2c_device_id_check_mag3110_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_mag3110();  // eCompass mag3110
        }
        if (i2c_device_id_check_p1003_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_p1003(g_p1003_tsc_i2c_device.port);    // hannstar display TSC via lvds
        }
#elif (BOARD_TYPE == BOARD_TYPE_SABRE_AI) 
        if (i2c_device_id_check_p1003_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_p1003(g_p1003_tsc_i2c_device.port);    // hannstar display TSC via lvds
        }
        if (i2c_device_id_check_cs42888_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_check_cs42888();   // audio codec on main board
        }
        if (i2c_device_id_check_isl29023_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_isl29023(); // light sensor on main board
        }
        if (i2c_device_id_check_mag3110_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_mag3110();  // eCompass on main board
        }
        if (i2c_device_id_check_mma8451_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_MMA8451();  // accelerometer on main board
        }       
#endif    

    if (test_count == 0) 
    {
        printf("%sNo I2C Device tests performed.\n", indent);

        return TEST_NOT_IMPLEMENTED;
    }
    if (rc == 0) 
    {
        printf("\n%sI2C Device test failed.\n", indent);

        return TEST_FAILED;
    }
    else
    {
    	printf("\n%sI2C Device test passed.\n", indent);

    	return TEST_PASSED;
    }

    return TEST_PASSED;
}
#if 0  //clw
extern menu_action_t i2c_device_isl29023_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_mag3110_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_mag3112_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_ppl3115_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_MMA8450_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_MMA8451_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_emc1046_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_P1003_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_max11801_test(const menu_context_t* context, void* param);
extern menu_action_t i2c_device_mc1323_test(const menu_context_t* context, void* param);

//extern int i2c_device_check_cs42888(void);
//int i2c_device_id_check_cs42888_test_enable = 1;

//PMIC device tests
menu_action_t pmic_mc13892_test(const menu_context_t* context, void* param);
menu_action_t pmic_MAX17135_test(const menu_context_t* context, void* param);
menu_action_t pmic_lt3589_test(const menu_context_t* context, void* param);
menu_action_t pmic_da9053_test(const menu_context_t* context, void* param);
menu_action_t pf0100_i2c_device_id_check(const menu_context_t* context, void* param);

static const char * const i2c_device_test_name = "I2C Device ID Test";
static const char * const pmic_test_name = "PMIC Test";

/*!
 * This test performs PMIC device test on i2c 
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
menu_action_t pmic_test(const menu_context_t* const context, void* const param) 
{
	if ( prompt_run_test(pmic_test_name, NULL) != TEST_CONTINUE )
    {
    	*(test_return_t*)param = TEST_BYPASSED;
    	return MENU_CONTINUE;
    }  
	pmic_mc13892_test(context, param);
    pmic_MAX17135_test(context, param);
    pmic_lt3589_test(context, param);
    pmic_da9053_test(context, param);
    pf0100_i2c_device_id_check(context, param);
    
    return MENU_CONTINUE;  
}

/*!
 * This test performs i2c device id check 
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
menu_action_t i2c_device_id_check(const menu_context_t* const context, void* const param) 
{
    int test_count = 0;

    if ( prompt_run_test(i2c_device_test_name, NULL) != TEST_CONTINUE )
    {
    	*(test_return_t*)param = TEST_BYPASSED;
    	return MENU_CONTINUE;
    }  
    
#if defined(CHIP_MX6DQ)   
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
    gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
    hal_delay_us(1000);
    gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);
#if defined(BOARD_SMART_DEVICE) 
    test_count = 5;
    i2c_device_MMA8451_test(context, param);
    i2c_device_isl29023_test(context, param);
    i2c_device_mag3110_test(context, param);
    i2c_device_P1003_test(context, param);
    i2c_device_emc1046_test(context, param);
    //rc |= i2c_device_id_check_mc1323(I2C2_BASE_ADDR);
    
#elif defined(BOARD_SABRE_LITE) 
    test_count = 1;
    i2c_device_P1003_test(context, param);
#elif defined(BOARD_SABRE_AI)      
    test_count = 6;   
    i2c_device_P1003_test(context, param);
    i2c_device_isl29023_test(context, param);
    i2c_device_mag3110_test(context, param);
    i2c_device_mag3112_test(context, param);
    i2c_device_MMA8451_test(context, param);
    i2c_device_emc1046_test(context, param);
    
/*        if (i2c_device_id_check_cs42888_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_check_cs42888();   // audio codec on main board
        } */
#endif    

#endif //Chip MX6DQ 

#if defined(CHIP_MX6SDL)
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
    gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
    hal_delay_us(1000);
    gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);

#if defined(BOARD_SMART_DEVICE) 
    test_count = 5;
    i2c_device_MMA8451_test(context, param);
    i2c_device_isl29023_test(context, param);
    i2c_device_mag3110_test(context, param);
    i2c_device_P1003_test(context, param);
    i2c_device_emc1046_test(context, param);
#elif defined(BOARD_SABRE_LITE)
    test_count = 1;
    i2c_device_P1003_test(context, param);    
#elif defined(BOARD_SABRE_AI)
    test_count = 5;
    i2c_device_P1003_test(context, param);
    i2c_device_isl29023_test(context, param);
    i2c_device_mag3110_test(context, param);
    i2c_device_mag3112_test(context, param);
    i2c_device_MMA8451_test(context, param);
/*        if (i2c_device_id_check_cs42888_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_check_cs42888();   // audio codec on main board
        } */
#endif

#endif //Chip MX6SDL

#if defined(CHIP_MX6SL)
    test_count = 2;
    i2c_device_MMA8450_test(context, param);
    i2c_device_isl29023_test(context, param);
#endif //Chip MX6SL

    if (test_count == 0) 
    {
        printf("No I2C Device tests performed.\n");
        print_test_failed(i2c_device_test_name, NULL);

        *(test_return_t*)param = TEST_FAILED;
        return MENU_CONTINUE;        
        
    }

    return MENU_CONTINUE;          
}
#endif // clw
#if 0
int i2c_device_id_check(void)
{
    int rc = 0;
    int test_count = 0;

    PROMPT_RUN_TEST("I2C Device ID", NULL);

    if ((BOARD_TYPE_ID == BOARD_TYPE_DEFAULT) || (BOARD_TYPE_ID == BOARD_TYPE_EVB)) 
    {

    } 
    else if (BOARD_TYPE_ID == BOARD_TYPE_SMART_DEVICE) 
    {
        //  USB_OTG_PWR_EN (EIM_D22)
        writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
        gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
        gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
        hal_delay_us(1000);
        gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);

        //rc |= i2c_device_id_check_mc1323(I2C2_BASE_ADDR);
        if (i2c_device_id_check_mma8451_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_MMA8451();  // accelerometer
        }
        if (i2c_device_id_check_isl29023_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_isl29023(); // light sensor
        }
        if (i2c_device_id_check_mag3110_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_mag3110();  // eCompass mag3110
        }
//c        if (i2c_device_id_check_p1003_test_enable == 1) {
//c            ++test_count;
//c            rc |= i2c_device_id_check_p1003(g_p1003_tsc_i2c_device.port);    // hannstar display TSC via lvds
//c        }
        //rc |= i2c_device_id_check_emc1046(I2C3_BASE_ADDR);
    } 
    else if (BOARD_TYPE_ID == BOARD_TYPE_SABRE_AI) 
    {
//c        if (i2c_device_id_check_p1003_test_enable == 1) {
//c            ++test_count;
//c            rc |= i2c_device_id_check_p1003(g_p1003_tsc_i2c_device.port);    // hannstar display TSC via lvds
//c        }
        if (i2c_device_id_check_cs42888_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_check_cs42888();   // audio codec on main board
        }
        if (i2c_device_id_check_isl29023_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_isl29023(); // light sensor on main board
        }
        if (i2c_device_id_check_mag3112_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_mag3112(I2C3_BASE_ADDR);  // eCompass on main board
        }
        if (i2c_device_id_check_mma8451_test_enable == 1) {
            ++test_count;
            rc |= i2c_device_id_check_MMA8451();  // accelerometer on main board
        }
    }

    if (test_count == 0) {
        printf("No I2C Device tests performed.\n");
        return 0;
    }
    printf("I2C Device test %s.\n", (rc == 0) ? "passed" : "failed");
    return rc;
}
#endif


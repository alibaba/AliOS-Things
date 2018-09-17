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
 * @file can_common.c
 * @brief CAN driver's common operation functions
 *
 * @ingroup diag_can
 */

#include "obds.h"
//#include "functions.h"
//#include "can_common.h"
//extern int can_tx0rx1(void);
//extern int can_tx1rx0(void);

const char g_flexcan_test_name[] = "FlexCan1 <-> FlexCan2 Loopback Test";

/*!
 * Test can0 and can1 tx/rx.
 *
 * @return      TEST_PASSED or TEST_FAILED
 */
test_return_t flexcan_test(void)
{
	const char* indent = menu_get_indent();

    test_return_t result1 = TEST_NOT_IMPLEMENTED, result2 = TEST_NOT_IMPLEMENTED;

#if defined(BOARD_SABRE_AI)
    printf("%s(Please note that in order to run the test, you need to first\n", indent);
    printf("%s(on SABRE AI main board): connect J35.6 to J34.7 and connect J35.7 to J34.2\n\n", indent);
    printf("%sWould you like to run the FLEXCAN1/FLEXCAN2 loopback tests?\n", indent);
    if (!is_input_char('y', indent))
        return TEST_BYPASSED;

/* TODO: Call new functions
    can_io_cfg(CAN0_BASE_ADDR);
    can_io_cfg(CAN1_BASE_ADDR);
*/
#endif

    printf("%sFLEXCAN test1: FLEXCAN2-TX and FLEXCAN1-RX\n", indent);
/* TODO: Implemnent CAN2 TX -> CAN1 RX
    result1 = can_tx2rx1();

    // Disable the module
    reg32setbit(CAN1_BASE_ADDR + CAN_MCR, 31);
    reg32setbit(CAN0_BASE_ADDR + CAN_MCR, 31);
    //reset can0 and can1
    reg32setbit(CAN0_BASE_ADDR + CAN_MCR, 25);
    reg32setbit(CAN1_BASE_ADDR + CAN_MCR, 25);
    hal_delay_us(5000);
*/
    printf("%sFLEXCAN test2: FLEXCAN1-TX and FLEXCAN2-RX\n", indent);
/* TODO: Implemnent CAN1 TX -> CAN2 RX
    result2 = can_tx0rx1();
*/
    return result1 = result2;

/* TODO: Reinstate this code and remove the above code
 * after implementing testsImplemnent CAN1 TX -> CAN2 RX

    if (result1 == TEST_PASSED && result2 == TEST_PASSED)
    {
        print_test_passed(test_name, indent);

        *(test_return_t*)param = TEST_PASSED;
        return MENU_CONTINUE;
    }

    print_test_failed(test_name, indent);

    *(test_return_t*)param = TEST_FAILED;
    return MENU_CONTINUE;
*/
}

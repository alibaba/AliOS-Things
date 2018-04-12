/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_init.c
* Description  : Performs initialization common to all MCUs in this Group
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.10.2016 1.00     First Release
*         : 15.05.2017 2.00     Added port setting of 177 pins and 176 pins.
*                               Added setting of PORTG.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Get specifics on this MCU. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* RX MCUs come in different packages and different pin counts. For MCUs that do not have the maximum number of pins
 * for their group (e.g. MCU with 100 pins when maximum is 177 pins) these 'non-existent' pins that are not bonded out
 * need to be initialized to save power. The macros below define the non-existent pins on each port for smaller
 * pin count MCUs. If a pin is non-existent then its value is set to a 1. These values are then ORed into the
 * direction registers to set non-existent pins as outputs which can help save power.
 */
#if   ((BSP_PACKAGE_PINS == 177) || (BSP_PACKAGE_PINS == 176))
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x50)    /* Missing pins: P04 P06 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT6_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT7_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT8_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT9_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTF_NE_PIN_MASK     (0xC0)    /* Missing pins: PF6 PF7 */
    #define BSP_PRV_PORTG_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0xD0)    /* Missing pins: PJ4 PJ6 PJ7 */
#elif ((BSP_PACKAGE_PINS == 145) || (BSP_PACKAGE_PINS == 144))
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x50)    /* Missing pins: P04 P06 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x03)    /* Missing pins: P10 P11 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0x80)    /* Missing pins: P57 */
    #define BSP_PRV_PORT6_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT7_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT8_NE_PIN_MASK     (0x30)    /* Missing pins: P84 P85 */
    #define BSP_PRV_PORT9_NE_PIN_MASK     (0xF0)    /* Missing pins: P94 P95 P96 P97 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTF_NE_PIN_MASK     (0xDF)    /* Missing pins: PF0 PF1 PF2 PF3 PF4 PF6 PF7 */
#if defined(BSP_MCU_RX65N_2MB)
    #define BSP_PRV_PORTG_NE_PIN_MASK     (0xFF)    /* Missing pins: PG0 PG1 PG2 PG3 PG4 PG5 PG6 PG7 */
#endif
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0xD7)    /* Missing pins: PJ0 PJ1 PJ2 PJ4 PJ6 PJ7 */
#elif (BSP_PACKAGE_PINS == 100)
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x5F)    /* Missing pins: P00 P01 P02 P03 P04 P06 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x03)    /* Missing pins: P10 P11 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0xC0)    /* Missing pins: P56 P57 */
    #define BSP_PRV_PORT6_NE_PIN_MASK     (0xFF)    /* Missing pins: P60 P61 P62 P63 P64 P65 P66 P67 */
    #define BSP_PRV_PORT7_NE_PIN_MASK     (0xFF)    /* Missing pins: P70 P71 P72 P73 P74 P75 P76 P77 */
    #define BSP_PRV_PORT8_NE_PIN_MASK     (0xFF)    /* Missing pins: P80 P81 P82 P83 P84 P85 P86 P87 */
    #define BSP_PRV_PORT9_NE_PIN_MASK     (0xFF)    /* Missing pins: P90 P91 P92 P93 P94 P95 P96 P97 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTF_NE_PIN_MASK     (0xFF)    /* Missing pins: PF0 PF1 PF2 PF3 PF4 PF5 PF6 PF7 */
#if defined(BSP_MCU_RX65N_2MB)
    #define BSP_PRV_PORTG_NE_PIN_MASK     (0xFF)    /* Missing pins: PG0 PG1 PG2 PG3 PG4 PG5 PG6 PG7 */
#endif
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0xF7)    /* Missing pins: PJ0 PJ1 PJ2 PJ4 PJ5 PJ6 PJ7 */
#else
    #error "ERROR - This package is not defined in mcu_init.c"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: bsp_non_existent_port_init
* Description  : For MCUs that do not have the maximum number of pins for their group (e.g. MCU with 100 pins when
*                maximum is 177 pins) these 'non-existent' pins that are not bonded out need to be initialized to save
*                power.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void bsp_non_existent_port_init (void)
{
    /* OR in missing pin masks from above. */
    PORT0.PDR.BYTE |= BSP_PRV_PORT0_NE_PIN_MASK;
    PORT1.PDR.BYTE |= BSP_PRV_PORT1_NE_PIN_MASK;
    PORT2.PDR.BYTE |= BSP_PRV_PORT2_NE_PIN_MASK;
    PORT3.PDR.BYTE |= BSP_PRV_PORT3_NE_PIN_MASK;
    PORT4.PDR.BYTE |= BSP_PRV_PORT4_NE_PIN_MASK;
    PORT5.PDR.BYTE |= BSP_PRV_PORT5_NE_PIN_MASK;
    PORT6.PDR.BYTE |= BSP_PRV_PORT6_NE_PIN_MASK;
    PORT7.PDR.BYTE |= BSP_PRV_PORT7_NE_PIN_MASK;
    PORT8.PDR.BYTE |= BSP_PRV_PORT8_NE_PIN_MASK;
    PORT9.PDR.BYTE |= BSP_PRV_PORT9_NE_PIN_MASK;
    PORTA.PDR.BYTE |= BSP_PRV_PORTA_NE_PIN_MASK;
    PORTB.PDR.BYTE |= BSP_PRV_PORTB_NE_PIN_MASK;
    PORTC.PDR.BYTE |= BSP_PRV_PORTC_NE_PIN_MASK;
    PORTD.PDR.BYTE |= BSP_PRV_PORTD_NE_PIN_MASK;
    PORTE.PDR.BYTE |= BSP_PRV_PORTE_NE_PIN_MASK;
    PORTF.PDR.BYTE |= BSP_PRV_PORTF_NE_PIN_MASK;
#if defined(BSP_MCU_RX65N_2MB)
    PORTG.PDR.BYTE |= BSP_PRV_PORTG_NE_PIN_MASK;
#endif
    PORTJ.PDR.BYTE |= BSP_PRV_PORTJ_NE_PIN_MASK;
}


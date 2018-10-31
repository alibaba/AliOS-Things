<#--
/*******************************************************************************
  Clock System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk.ftl

  Summary:
    Clock System Service Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

-->
<#if CONFIG_USE_SYS_CLK == true>

// *****************************************************************************
/* Clock System Service Configuration Options
*/
<#if CONFIG_PIC32MX == true>
#define SYS_CLK_FREQ                        ${CONFIG_SYS_CLK_FREQ}ul
#define SYS_CLK_BUS_PERIPHERAL_1            ${CONFIG_SYS_CLK_PBCLK_FREQ}ul
<#if CONFIG_DS60001143 == false && CONFIG_DS60001156 == false>
<#if CONFIG_SYS_CLK_REFCLK_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_1             ${CONFIG_SYS_CLK_REFCLK_FREQ}ul
</#if>
</#if>
#define SYS_CLK_UPLL_BEFORE_DIV2_FREQ       ${CONFIG_SYS_CLK_UPLL_BEFORE_DIV2_FREQ}ul
</#if>
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true)>
#define SYS_CLK_FREQ                        ${CONFIG_SYS_CLK_PBCLK6_FREQ}ul
<#if CONFIG_SYS_CLK_PBCLK0_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_1            ${CONFIG_SYS_CLK_PBCLK0_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK1_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_2            ${CONFIG_SYS_CLK_PBCLK1_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK2_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_3            ${CONFIG_SYS_CLK_PBCLK2_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK3_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_4            ${CONFIG_SYS_CLK_PBCLK3_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK4_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_5            ${CONFIG_SYS_CLK_PBCLK4_FREQ}ul
</#if>
<#if CONFIG_DS60001361 == true || CONFIG_DS60001402 == true>
<#if CONFIG_SYS_CLK_PBCLK5_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_6            ${CONFIG_SYS_CLK_PBCLK5_FREQ}ul
</#if></#if>
<#if CONFIG_SYS_CLK_PBCLK6_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_7            ${CONFIG_SYS_CLK_PBCLK6_FREQ}ul
</#if>
<#if CONFIG_DS60001361 == false && CONFIG_DS60001402 == false>
<#if CONFIG_SYS_CLK_PBCLK7_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_8            ${CONFIG_SYS_CLK_PBCLK7_FREQ}ul
</#if></#if>
<#if CONFIG_SYS_CLK_REFCLK0_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_1             ${CONFIG_SYS_CLK_REFCLK0_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK1_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_2             ${CONFIG_SYS_CLK_REFCLK1_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK2_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_3             ${CONFIG_SYS_CLK_REFCLK2_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK3_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_4             ${CONFIG_SYS_CLK_REFCLK3_FREQ}ul
</#if>
<#if CONFIG_HAVE_REFCLOCK5 == true>
<#if CONFIG_SYS_CLK_REFCLK4_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_5             ${CONFIG_SYS_CLK_REFCLK4_FREQ}ul
</#if></#if></#if>
<#if CONFIG_PIC32WK == true>
#define SYS_CLK_FREQ                        ${CONFIG_SYS_CLK_FREQ}ul
<#if CONFIG_SYS_CLK_PBCLK0_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_1            ${CONFIG_SYS_CLK_PBCLK0_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK1_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_2            ${CONFIG_SYS_CLK_PBCLK1_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK2_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_3            ${CONFIG_SYS_CLK_PBCLK2_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_PBCLK3_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_4            ${CONFIG_SYS_CLK_PBCLK3_FREQ}ul
</#if>
<#if CONFIG_PIC32WK == false>
<#if CONFIG_SYS_CLK_PBCLK4_ENABLE == true>
#define SYS_CLK_BUS_PERIPHERAL_5            ${CONFIG_SYS_CLK_PBCLK4_FREQ}ul
</#if>
</#if>
<#if CONFIG_SYS_CLK_REFCLK0_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_1             ${CONFIG_SYS_CLK_REFCLK0_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK1_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_2             ${CONFIG_SYS_CLK_REFCLK1_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK2_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_3             ${CONFIG_SYS_CLK_REFCLK2_FREQ}ul
</#if>
<#if CONFIG_SYS_CLK_REFCLK3_ENABLE == true>
#define SYS_CLK_BUS_REFERENCE_4             ${CONFIG_SYS_CLK_REFCLK3_FREQ}ul
</#if>
</#if>
<#if CONFIG_SYS_CLK_CONFIG_PRIMARY_XTAL == "">
#define SYS_CLK_CONFIG_PRIMARY_XTAL         0ul
<#else>
#define SYS_CLK_CONFIG_PRIMARY_XTAL         ${CONFIG_SYS_CLK_CONFIG_PRIMARY_XTAL}ul
</#if>
<#if CONFIG_SYS_CLK_CONFIG_SECONDARY_XTAL == "">
#define SYS_CLK_CONFIG_SECONDARY_XTAL       0ul
<#else>
#define SYS_CLK_CONFIG_SECONDARY_XTAL       ${CONFIG_SYS_CLK_CONFIG_SECONDARY_XTAL}ul
</#if>
<#if CONFIG_SYS_CLK_MODE == "DYNAMIC">
#define SYS_CLK_CONFIG_FREQ_ERROR_LIMIT     ${CONFIG_SYS_CLK_CONFIG_FREQ_ERROR_LIMIT}
<#if CONFIG_SYS_CLK_WAIT_FOR_SWITCH == true>
#define SYS_CLK_WAIT_FOR_SWITCH             true
<#else>
#define SYS_CLK_WAIT_FOR_SWITCH             false
</#if>
#define SYS_CLK_ON_WAIT                     ${CONFIG_SYS_CLK_ON_WAIT} 
</#if> <#-- CONFIG_SYS_CLK_MODE == "DYNAMIC" --> </#if> <#-- CONFIG_USE_SYS_CLK -->
<#--

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
*/
-->

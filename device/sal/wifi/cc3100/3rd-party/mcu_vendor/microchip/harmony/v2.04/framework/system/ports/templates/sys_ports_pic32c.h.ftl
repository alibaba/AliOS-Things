<#--
/*******************************************************************************
  Ports System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_pic32c.h.ftl

  Summary:
    Ports System Service Freemarker Template File

  Description:
    This file contains the PORTS system service configuration template.
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

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
-->
<#if CONFIG_USE_SYS_PORTS == true>
    <#assign PORT_A_NUM_INT_PINS = 0>
    <#assign PORT_B_NUM_INT_PINS = 0>
    <#assign PORT_C_NUM_INT_PINS = 0>
    <#assign PORT_D_NUM_INT_PINS = 0>
    <#assign PORT_E_NUM_INT_PINS = 0>
    <#list 1..350 as i>
    <#assign intConfig = "CONFIG_BSP_PIN_" + i + "_PIO_INTERRUPT">
    <#assign portChannel = "CONFIG_BSP_PIN_" + i + "_PORT_CHANNEL">
        <#if .vars[intConfig]?has_content>
            <#if (.vars[intConfig] != "Disabled")>
                <#if (.vars[portChannel] == "A")>
                    <#assign PORT_A_NUM_INT_PINS = PORT_A_NUM_INT_PINS + 1>
                </#if>
                <#if (.vars[portChannel] == "B")>
                    <#assign PORT_B_NUM_INT_PINS = PORT_B_NUM_INT_PINS + 1>
                </#if>
                <#if (.vars[portChannel] == "C")>
                    <#assign PORT_C_NUM_INT_PINS = PORT_C_NUM_INT_PINS + 1>
                </#if>
                <#if (.vars[portChannel] == "D")>
                    <#assign PORT_D_NUM_INT_PINS = PORT_D_NUM_INT_PINS + 1>
                </#if>
                <#if (.vars[portChannel] == "E")>
                    <#assign PORT_E_NUM_INT_PINS = PORT_E_NUM_INT_PINS + 1>
                </#if>
            </#if>
        </#if>
    </#list>
/*** Ports System Service Configuration ***/

<#if CONFIG_SYS_PORTS_INST_IDX0 == true>

#define SYS_PORT_A_DIR          ${CONFIG_PIOA_OER_VALUE}
#define SYS_PORT_A_LAT          ${CONFIG_PIOA_SODR_VALUE}
#define SYS_PORT_A_OD           ${CONFIG_PIOA_MDER_VALUE}
#define SYS_PORT_A_PU           ${CONFIG_PIOA_PUER_VALUE}
#define SYS_PORT_A_PD           ${CONFIG_PIOA_PPDEN_VALUE}
#define SYS_PORT_A_PER          ${CONFIG_PIOA_PER_VALUE}
#define SYS_PORT_A_ABCD1        ${CONFIG_PIOA_ABCDSR1_VALUE}
#define SYS_PORT_A_ABCD2        ${CONFIG_PIOA_ABCDSR2_VALUE}
#define SYS_PORT_A_INT          ${CONFIG_PIOA_IER_VALUE}
#define SYS_PORT_A_INT_TYPE     ${CONFIG_PIOA_AIMER_VALUE}
#define SYS_PORT_A_INT_EDGE     ${CONFIG_PIOA_ESR_VALUE}
#define SYS_PORT_A_INT_RE_HL    ${CONFIG_PIOA_REHLSR_VALUE}
#define SYS_PORT_A_NUM_INT_PINS ${PORT_A_NUM_INT_PINS}
</#if>
<#if CONFIG_SYS_PORTS_INST_IDX1 == true>

#define SYS_PORT_B_DIR          ${CONFIG_PIOB_OER_VALUE}
#define SYS_PORT_B_LAT          ${CONFIG_PIOB_SODR_VALUE}
#define SYS_PORT_B_OD           ${CONFIG_PIOB_MDER_VALUE}
#define SYS_PORT_B_PU           ${CONFIG_PIOB_PUER_VALUE}
#define SYS_PORT_B_PD           ${CONFIG_PIOB_PPDEN_VALUE}
#define SYS_PORT_B_PER          ${CONFIG_PIOB_PER_VALUE}
#define SYS_PORT_B_ABCD1        ${CONFIG_PIOB_ABCDSR1_VALUE}
#define SYS_PORT_B_ABCD2        ${CONFIG_PIOB_ABCDSR2_VALUE}
#define SYS_PORT_B_INT          ${CONFIG_PIOB_IER_VALUE}
#define SYS_PORT_B_INT_TYPE     ${CONFIG_PIOB_AIMER_VALUE}
#define SYS_PORT_B_INT_EDGE     ${CONFIG_PIOB_ESR_VALUE}
#define SYS_PORT_B_INT_RE_HL    ${CONFIG_PIOB_REHLSR_VALUE}
#define SYS_PORT_B_NUM_INT_PINS ${PORT_B_NUM_INT_PINS}
</#if>
<#if CONFIG_SYS_PORTS_INST_IDX2 == true>

#define SYS_PORT_C_DIR          ${CONFIG_PIOC_OER_VALUE}
#define SYS_PORT_C_LAT          ${CONFIG_PIOC_SODR_VALUE}
#define SYS_PORT_C_OD           ${CONFIG_PIOC_MDER_VALUE}
#define SYS_PORT_C_PU           ${CONFIG_PIOC_PUER_VALUE}
#define SYS_PORT_C_PD           ${CONFIG_PIOC_PPDEN_VALUE}
#define SYS_PORT_C_PER          ${CONFIG_PIOC_PER_VALUE}
#define SYS_PORT_C_ABCD1        ${CONFIG_PIOC_ABCDSR1_VALUE}
#define SYS_PORT_C_ABCD2        ${CONFIG_PIOC_ABCDSR2_VALUE}
#define SYS_PORT_C_INT          ${CONFIG_PIOC_IER_VALUE}
#define SYS_PORT_C_INT_TYPE     ${CONFIG_PIOC_AIMER_VALUE}
#define SYS_PORT_C_INT_EDGE     ${CONFIG_PIOC_ESR_VALUE}
#define SYS_PORT_C_INT_RE_HL    ${CONFIG_PIOC_REHLSR_VALUE}
#define SYS_PORT_C_NUM_INT_PINS ${PORT_C_NUM_INT_PINS}
</#if>
<#if CONFIG_SYS_PORTS_INST_IDX3 == true>

#define SYS_PORT_D_DIR          ${CONFIG_PIOD_OER_VALUE}
#define SYS_PORT_D_LAT          ${CONFIG_PIOD_SODR_VALUE}
#define SYS_PORT_D_OD           ${CONFIG_PIOD_MDER_VALUE}
#define SYS_PORT_D_PU           ${CONFIG_PIOD_PUER_VALUE}
#define SYS_PORT_D_PD           ${CONFIG_PIOD_PPDEN_VALUE}
#define SYS_PORT_D_PER          ${CONFIG_PIOD_PER_VALUE}
#define SYS_PORT_D_ABCD1        ${CONFIG_PIOD_ABCDSR1_VALUE}
#define SYS_PORT_D_ABCD2        ${CONFIG_PIOD_ABCDSR2_VALUE}
#define SYS_PORT_D_INT          ${CONFIG_PIOD_IER_VALUE}
#define SYS_PORT_D_INT_TYPE     ${CONFIG_PIOD_AIMER_VALUE}
#define SYS_PORT_D_INT_EDGE     ${CONFIG_PIOD_ESR_VALUE}
#define SYS_PORT_D_INT_RE_HL    ${CONFIG_PIOD_REHLSR_VALUE}
#define SYS_PORT_D_NUM_INT_PINS ${PORT_D_NUM_INT_PINS}
</#if>
<#if CONFIG_SYS_PORTS_INST_IDX4 == true>

#define SYS_PORT_E_DIR          ${CONFIG_PIOE_OER_VALUE}
#define SYS_PORT_E_LAT          ${CONFIG_PIOE_SODR_VALUE}
#define SYS_PORT_E_OD           ${CONFIG_PIOE_MDER_VALUE}
#define SYS_PORT_E_PU           ${CONFIG_PIOE_PUER_VALUE}
#define SYS_PORT_E_PD           ${CONFIG_PIOE_PPDEN_VALUE}
#define SYS_PORT_E_PER          ${CONFIG_PIOE_PER_VALUE}
#define SYS_PORT_E_ABCD1        ${CONFIG_PIOE_ABCDSR1_VALUE}
#define SYS_PORT_E_ABCD2        ${CONFIG_PIOE_ABCDSR2_VALUE}
#define SYS_PORT_E_INT          ${CONFIG_PIOE_IER_VALUE}
#define SYS_PORT_E_INT_TYPE     ${CONFIG_PIOE_AIMER_VALUE}
#define SYS_PORT_E_INT_EDGE     ${CONFIG_PIOE_ESR_VALUE}
#define SYS_PORT_E_INT_RE_HL    ${CONFIG_PIOE_REHLSR_VALUE}
#define SYS_PORT_E_NUM_INT_PINS ${PORT_E_NUM_INT_PINS}
</#if>

#define SYS_PORT_ANALOG0        ${CONFIG_AFEC0_CHER_VALUE}
#define SYS_PORT_ANALOG1        ${CONFIG_AFEC1_CHER_VALUE}
#define SYS_PORT_DACC           ${CONFIG_DACC_CHER_VALUE}
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->

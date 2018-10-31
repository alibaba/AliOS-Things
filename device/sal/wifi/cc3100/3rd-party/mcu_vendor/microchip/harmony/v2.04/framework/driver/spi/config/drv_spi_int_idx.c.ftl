<#--
/*******************************************************************************
  SPI Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_int_idx.c.ftl

  Summary:
    SPI Driver Freemarker Template File

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
-->
<#macro DRV_SPI_INT_C INST_INDEX>
    <#if CONFIG_PIC32MX == true>
        <#if CONFIG_USE_3RDPARTY_RTOS>
        <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${.vars["CONFIG_DRV_SPI_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_INT_PRIO_NUM_IDX${INST_INDEX}"]}SOFT) _IntHandlerSPIInstance${INST_INDEX}(void)
        <#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${.vars["CONFIG_DRV_SPI_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO), vector(${.vars["CONFIG_DRV_SPI_ISR_VECTOR_IDX${INST_INDEX}"]}))) IntHandlerSPIInstance${INST_INDEX}_ISR( void );
</#if>
void IntHandlerSPIInstance${INST_INDEX}(void)
        </#if>
        <#else>
void __ISR(${.vars["CONFIG_DRV_SPI_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO) _IntHandlerSPIInstance${INST_INDEX}(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    DRV_SPI_Tasks(sysObj.spiObjectIdx${INST_INDEX});
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if>
    <#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true>
        <#if CONFIG_USE_3RDPARTY_RTOS>
        <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${.vars["CONFIG_DRV_SPI_RX_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_RX_INT_PRIO_NUM_IDX${INST_INDEX}"]}SOFT) _IntHandlerSPIRxInstance${INST_INDEX}(void)
        <#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${.vars["CONFIG_DRV_SPI_RX_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO), vector(${.vars["CONFIG_DRV_SPI_RX_ISR_VECTOR_IDX${INST_INDEX}"]}))) IntHandlerSPIRxInstance${INST_INDEX}_ISR( void );
</#if>
void IntHandlerSPIRxInstance${INST_INDEX}(void)
        </#if>
        <#else>
void __ISR(${.vars["CONFIG_DRV_SPI_RX_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_RX_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO) _IntHandlerSPIRxInstance${INST_INDEX}(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    DRV_SPI_Tasks(sysObj.spiObjectIdx${INST_INDEX});
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
        <#if CONFIG_USE_3RDPARTY_RTOS>
        <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${.vars["CONFIG_DRV_SPI_TX_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_TX_INT_PRIO_NUM_IDX${INST_INDEX}"]}SOFT) _IntHandlerSPITxInstance${INST_INDEX}(void)
        <#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${.vars["CONFIG_DRV_SPI_TX_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO), vector(${.vars["CONFIG_DRV_SPI_TX_ISR_VECTOR_IDX${INST_INDEX}"]}))) IntHandlerSPITxInstance${INST_INDEX}_ISR( void );
</#if>
void IntHandlerSPITxInstance${INST_INDEX}(void)
        </#if>
        <#else>
void __ISR(${.vars["CONFIG_DRV_SPI_TX_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_TX_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO) _IntHandlerSPITxInstance${INST_INDEX}(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    DRV_SPI_Tasks(sysObj.spiObjectIdx${INST_INDEX});
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
        <#if CONFIG_USE_3RDPARTY_RTOS>
        <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${.vars["CONFIG_DRV_SPI_ERROR_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_ERROR_INT_PRIO_NUM_IDX${INST_INDEX}"]}SOFT) _IntHandlerSPIFaultInstance${INST_INDEX}(void)
        <#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${.vars["CONFIG_DRV_SPI_ERROR_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO), vector(${.vars["CONFIG_DRV_SPI_ERROR_ISR_VECTOR_IDX${INST_INDEX}"]}))) IntHandlerSPIFaultInstance${INST_INDEX}_ISR( void );
</#if>
void IntHandlerSPIFaultInstance${INST_INDEX}(void)
        </#if>
        <#else>
void __ISR(${.vars["CONFIG_DRV_SPI_ERROR_ISR_VECTOR_IDX${INST_INDEX}"]}, ipl${.vars["CONFIG_DRV_SPI_ERROR_INT_PRIO_NUM_IDX${INST_INDEX}"]}AUTO) _IntHandlerSPIFaultInstance${INST_INDEX}(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    DRV_SPI_Tasks(sysObj.spiObjectIdx${INST_INDEX});
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if>
</#macro>

<#if CONFIG_DRV_SPI_IDX0 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX0 == true>
    <@DRV_SPI_INT_C "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX1 == true>
    <@DRV_SPI_INT_C "1"/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX2 == true>
    <@DRV_SPI_INT_C "2"/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX3 == true>
    <@DRV_SPI_INT_C "3"/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX4 == true>
    <@DRV_SPI_INT_C "4"/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true && CONFIG_DRV_SPI_TASK_MODE_ISR_IDX5 == true>
    <@DRV_SPI_INT_C "5"/>
</#if>

<#--
/*******************************************************************************
  SPI Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_init.c.ftl

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
<#macro DRV_SPI_INIT_CODE INST_INDEX>

    /*** SPI Driver Index ${INST_INDEX} initialization***/

<#if .vars["CONFIG_DRV_SPI_TASK_MODE_ISR_IDX${INST_INDEX}"] == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
    SYS_INT_VectorPrioritySet(DRV_SPI_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_INT_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorSubprioritySet(DRV_SPI_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_INT_SUB_PRIORITY_IDX${INST_INDEX});
<#else>
    SYS_INT_VectorPrioritySet(${.vars["CONFIG_DRV_SPI_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_INT_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorSubprioritySet(${.vars["CONFIG_DRV_SPI_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_INT_SUB_PRIORITY_IDX${INST_INDEX}"]});
</#if>
<#else>
<#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
    SYS_INT_VectorPrioritySet(DRV_SPI_TX_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_TX_INT_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorSubprioritySet(DRV_SPI_TX_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_TX_INT_SUB_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorPrioritySet(DRV_SPI_RX_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_RX_INT_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorSubprioritySet(DRV_SPI_RX_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_RX_INT_SUB_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorPrioritySet(DRV_DRV_SPI_ERROR_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_ERROR_INT_PRIORITY_IDX${INST_INDEX});
    SYS_INT_VectorSubprioritySet(DRV_DRV_SPI_ERROR_INT_VECTOR_IDX${INST_INDEX}, DRV_SPI_ERROR_INT_SUB_PRIORITY_IDX${INST_INDEX});
<#else>
    SYS_INT_VectorPrioritySet(${.vars["CONFIG_DRV_SPI_TX_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_TX_INT_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorSubprioritySet(${.vars["CONFIG_DRV_SPI_TX_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_TX_INT_SUB_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorPrioritySet(${.vars["CONFIG_DRV_SPI_RX_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_RX_INT_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorSubprioritySet(${.vars["CONFIG_DRV_SPI_RX_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_RX_INT_SUB_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorPrioritySet(${.vars["CONFIG_DRV_SPI_ERROR_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_ERROR_INT_PRIORITY_IDX${INST_INDEX}"]});
    SYS_INT_VectorSubprioritySet(${.vars["CONFIG_DRV_SPI_ERROR_INT_VECTOR_IDX${INST_INDEX}"]}, ${.vars["CONFIG_DRV_SPI_ERROR_INT_SUB_PRIORITY_IDX${INST_INDEX}"]});
</#if>
</#if>
</#if>
<#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
    sysObj.spiObjectIdx${INST_INDEX} = DRV_SPI_Initialize(DRV_SPI_INDEX_${INST_INDEX}, (const SYS_MODULE_INIT  * const)&drvSpi${INST_INDEX}InitData);
<#else>
    sysObj.spiObjectIdx${INST_INDEX} = DRV_SPI_Initialize(DRV_SPI_INDEX_${INST_INDEX}, (const SYS_MODULE_INIT  * const)NULL);
</#if>	
</#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_INIT_CODE "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_INIT_CODE "1"/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
    <@DRV_SPI_INIT_CODE "2"/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
    <@DRV_SPI_INIT_CODE "3"/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
    <@DRV_SPI_INIT_CODE "4"/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
    <@DRV_SPI_INIT_CODE "5"/>
</#if>

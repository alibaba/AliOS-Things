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
    /* SPI Driver Initialization */
<#if CONFIG_DRV_SPI_IDX0 == true>
  <#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
    sysObj.spiObjectIdx0 = DRV_SPI_Initialize(DRV_SPI_INDEX_0, (const SYS_MODULE_INIT  * const)&drvSpi0InitData);
  <#else>
    sysObj.spiObjectIdx0 = DRV_SPI_Initialize(DRV_SPI_INDEX_0, (const SYS_MODULE_INIT  * const)NULL);
  </#if>	
  <#if .vars["CONFIG_DRV_SPI_INTERRUPT_MODE"] == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_SPI_INTERRUPT_VECTOR_IDX0}, ${CONFIG_DRV_SPI_INT_PRIORITY_IDX0});
  </#if>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
  <#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
    sysObj.spiObjectIdx1 = DRV_SPI_Initialize(DRV_SPI_INDEX_1, (const SYS_MODULE_INIT  * const)&drvSpi1InitData);
  <#else>
    sysObj.spiObjectIdx1 = DRV_SPI_Initialize(DRV_SPI_INDEX_1, (const SYS_MODULE_INIT  * const)NULL);
  </#if>	
  <#if .vars["CONFIG_DRV_SPI_INTERRUPT_MODE"] == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_SPI_INTERRUPT_VECTOR_IDX1}, ${CONFIG_DRV_SPI_INT_PRIORITY_IDX1});
  </#if>
</#if>

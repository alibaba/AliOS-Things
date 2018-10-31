<#--
/*******************************************************************************
  SPI Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi.ftl

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

<#if CONFIG_DRV_SPI_USE_DRIVER == true>
/*** SPI Driver Configuration ***/
#define DRV_SPI_NUMBER_OF_MODULES		${CONFIG_SPI_NUMBER_OF_MODULES}
/*** Driver Compilation and static configuration options. ***/
/*** Select SPI compilation units.***/
<#if CONFIG_DRV_SPI_USE_POLLED_MODE>
#define DRV_SPI_POLLED 				1
<#else>
#define DRV_SPI_POLLED 				0
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE>
#define DRV_SPI_ISR 				1
<#else>
#define DRV_SPI_ISR 				0
</#if>
<#if CONFIG_DRV_SPI_USE_MASTER_MODE>
#define DRV_SPI_MASTER 				1
<#else>
#define DRV_SPI_MASTER 				0
</#if>
<#if CONFIG_DRV_SPI_USE_SLAVE_MODE>
#define DRV_SPI_SLAVE 				1
<#else>
#define DRV_SPI_SLAVE 				0
</#if>
<#if CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE>
#define DRV_SPI_RM 					1
<#else>
#define DRV_SPI_RM 					0
</#if>
<#if CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE>
#define DRV_SPI_EBM 				1
<#else>
#define DRV_SPI_EBM 				0
</#if>
<#if CONFIG_DRV_SPI_USE_8BIT_MODE>
#define DRV_SPI_8BIT 				1
<#else>
#define DRV_SPI_8BIT 				0
</#if>
<#if CONFIG_DRV_SPI_USE_16BIT_MODE>
#define DRV_SPI_16BIT 				1
<#else>
#define DRV_SPI_16BIT 				0
</#if>
<#if CONFIG_DRV_SPI_USE_32BIT_MODE>
#define DRV_SPI_32BIT 				1
<#else>
#define DRV_SPI_32BIT 				0
</#if>
<#if CONFIG_DRV_SPI_USE_DMA == true>
#define DRV_SPI_DMA 				1
<#else>
#define DRV_SPI_DMA 				0
</#if>

/*** SPI Driver Static Allocation Options ***/
#define DRV_SPI_INSTANCES_NUMBER 		${CONFIG_DRV_SPI_INSTANCES_NUMBER}
#define DRV_SPI_CLIENTS_NUMBER 			${CONFIG_DRV_SPI_CLIENT_NUMBER}
#define DRV_SPI_ELEMENTS_PER_QUEUE 		${CONFIG_DRV_SPI_NUM_ELEMENTS_PER_INSTANCE}
<#if CONFIG_DRV_SPI_USE_DMA == true>
/*** SPI Driver DMA Options ***/
#define DRV_SPI_DMA_TXFER_SIZE 			${CONFIG_DRV_SPI_DMA_TXFER_SIZE}
#define DRV_SPI_DMA_DUMMY_BUFFER_SIZE 	${CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE}
</#if>
<#if CONFIG_DRV_SPI_DYNAMIC_MODE == true>
<#include "/framework/driver/spi/config/drv_spi_idx.h.ftl">
</#if>
</#if>

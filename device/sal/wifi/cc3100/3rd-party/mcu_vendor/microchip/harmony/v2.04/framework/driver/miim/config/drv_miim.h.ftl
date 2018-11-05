<#--
/*******************************************************************************
  MIIM Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim.h.ftl

  Summary:
    MIIM Driver Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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

<#if CONFIG_DRV_MIIM_USE_DRIVER>
/*** MIIM Driver Configuration ***/
#define DRV_MIIM_ETH_MODULE_ID              ${CONFIG_DRV_MIIM_ETH_MODULE_ID}
#define DRV_MIIM_INSTANCES_NUMBER           ${CONFIG_DRV_MIIM_INSTANCES_NUMBER}
#define DRV_MIIM_INSTANCE_OPERATIONS        ${CONFIG_DRV_MIIM_INSTANCE_OPERATIONS}
#define DRV_MIIM_INSTANCE_CLIENTS           ${CONFIG_DRV_MIIM_INSTANCE_CLIENTS}
<#if CONFIG_DRV_MIIM_CLIENT_OP_PROTECTION == true>
#define DRV_MIIM_CLIENT_OP_PROTECTION   true
<#else>
#define DRV_MIIM_CLIENT_OP_PROTECTION   false
</#if>
<#if CONFIG_DRV_MIIM_COMMANDS == true>
#define DRV_MIIM_COMMANDS   true
<#else>
#define DRV_MIIM_COMMANDS   false
</#if>
#define DRV_MIIM_DRIVER_OBJECT              ${CONFIG_DRV_MIIM_DRIVER_OBJECT}
#define DRV_MIIM_DRIVER_INDEX               DRV_MIIM_INDEX_${CONFIG_DRV_MIIM_DRIVER_INDEX}              
</#if>

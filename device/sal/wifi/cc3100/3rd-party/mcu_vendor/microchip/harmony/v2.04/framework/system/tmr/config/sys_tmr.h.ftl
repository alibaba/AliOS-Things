<#--
/*******************************************************************************
  Timer System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tmr.ftl

  Summary:
    Timer System Service Freemarker Template File

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
/*** Timer System Service Configuration ***/
<#if CONFIG_USE_SYS_TMR == true>
#define SYS_TMR_POWER_STATE             ${CONFIG_SYS_TMR_POWER_STATE}
#define SYS_TMR_DRIVER_INDEX            ${CONFIG_SYS_TMR_DRIVER_INDEX}
#define SYS_TMR_MAX_CLIENT_OBJECTS      ${CONFIG_SYS_TMR_MAX_CLIENT_OBJECTS}
#define SYS_TMR_FREQUENCY               ${CONFIG_SYS_TMR_FREQUENCY}
#define SYS_TMR_FREQUENCY_TOLERANCE     ${CONFIG_SYS_TMR_FREQUENCY_TOLERANCE}
#define SYS_TMR_UNIT_RESOLUTION         ${CONFIG_SYS_TMR_UNIT_RESOLUTION}
#define SYS_TMR_CLIENT_TOLERANCE        ${CONFIG_SYS_TMR_CLIENT_TOLERANCE}
<#if CONFIG_SYS_TMR_INTERRUPT_NOTIFICATION == true>
#define SYS_TMR_INTERRUPT_NOTIFICATION  true
<#else>
#define SYS_TMR_INTERRUPT_NOTIFICATION  false
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->

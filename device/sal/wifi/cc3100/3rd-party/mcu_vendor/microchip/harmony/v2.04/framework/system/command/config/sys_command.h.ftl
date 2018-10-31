<#--
/*******************************************************************************
  Clock System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_command.h.ftl

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
-->
<#if CONFIG_USE_SYS_COMMAND == true>

/*** Command Processor System Service Configuration ***/
#define SYS_CMD_ENABLE
#define SYS_CMD_DEVICE_MAX_INSTANCES    SYS_CONSOLE_DEVICE_MAX_INSTANCES
#define SYS_CMD_PRINT_BUFFER_SIZE       ${CONFIG_COMMAND_PRINT_BUFFER_SIZE}
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true>
#define SYS_CMD_BUFFER_DMA_READY        __attribute__((coherent)) __attribute__((aligned(16)))
<#else>
#define SYS_CMD_BUFFER_DMA_READY
</#if>
<#if CONFIG_REMAP_SYS_CONSOLE_MESSAGE == true>
#define SYS_CMD_REMAP_SYS_CONSOLE_MESSAGE
</#if>
<#if CONFIG_REMAP_SYS_DEBUG_MESSAGE == true>
#define SYS_CMD_REMAP_SYS_DEBUG_MESSAGE
</#if>
<#else>
<#if CONFIG_TCPIP_STACK_USE_COMMANDS == true >
#define SYS_COMMAND_INCLUDE_APIS
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->

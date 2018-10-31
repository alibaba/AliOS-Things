<#--
/*******************************************************************************
  Ports System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports.h.ftl

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

/*** Ports System Service Configuration ***/
<#if CONFIG_HAVE_PPS == false>
#define SYS_PORT_AD1PCFG        ~${CONFIG_SYS_PORT_ADPCFG}
#define SYS_PORT_CNPUE          ${CONFIG_SYS_PORT_CNPU}
#define SYS_PORT_CNEN           ${CONFIG_SYS_PORT_CNEN}
</#if>
<@mhc_process_port port="A"/>
<@mhc_process_port port="B"/>
<@mhc_process_port port="C"/>
<@mhc_process_port port="D"/>
<@mhc_process_port port="E"/>
<@mhc_process_port port="F"/>
<@mhc_process_port port="G"/>
<@mhc_process_port port="H"/>
<@mhc_process_port port="J"/>
<@mhc_process_port port="K"/>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->

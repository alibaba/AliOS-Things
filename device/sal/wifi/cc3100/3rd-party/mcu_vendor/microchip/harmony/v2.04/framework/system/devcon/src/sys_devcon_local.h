/*******************************************************************************
  Device Control System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_devcon_local.h

  Summary:
    Device Control System Service local declarations and definitions.

  Description:
    This file contains the Device Control System Service local declarations 
    and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
//DOM-IGNORE-END


#ifndef _SYS_DEVCON_LOCAL_H
#define _SYS_DEVCON_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system_config.h"
#include "system/devcon/sys_devcon.h"


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* SYS DEVCON State Machine States

   Summary
    Defines the various states that can be achieved by the module operation.

   Description
    This enumeration defines the various states that can be achieved by the 
    module operation.

   Remarks:
    None.
*/

typedef enum
{
    /* SYS DEVCON state ready */
    SYS_DEVCON_STATE_READY,

    /* SYS DEVCON state busy */
    SYS_DEVCON_STATE_BUSY,

    /* SYS TMR state init */
    SYS_DEVCON_STATE_INIT,

} SYS_DEVCON_STATES;


// *****************************************************************************
/* Device Constrol System Service Hardware Instance Object

  Summary:
    Defines the object required for the maintainence of the hardware.

  Description:
    This defines the object required for the maintainence of the hardware.

  Remarks:
    None.
*/

typedef struct
{
    /* Current state of module */
    SYS_DEVCON_STATES state;

    /* Status of SYS DEVCON module */
    SYS_STATUS status;

} SYS_DEVCON_OBJECT;


// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes of local functions
// *****************************************************************************
// *****************************************************************************

extern SYS_DEVCON_OBJECT devconObject;

void  __attribute__((far)) _pic32_init_cache (SYS_CACHE_COHERENCY cacheCoherency);
void  __attribute__((far)) _pic32_size_cache (void);

void  __attribute__((far)) _pic32_flush_cache (void);
void  __attribute__((far)) _pic32_flush_dcache (void);
void  __attribute__((far)) _pic32_flush_icache (void);

void  __attribute__((far)) _pic32_sync_icache (uint32_t, size_t);

void  __attribute__((far)) _pic32_clean_cache (uint32_t, size_t);
void  __attribute__((far)) _pic32_clean_dcache (uint32_t, size_t);
void  __attribute__((far)) _pic32_clean_dcache_nowrite (uint32_t, size_t);
void  __attribute__((far)) _pic32_clean_icache (uint32_t, size_t);

void  __attribute__((far)) _pic32_lock_dcache (uint32_t, size_t);
void  __attribute__((far)) _pic32_lock_icache (uint32_t, size_t);

size_t  __attribute__((far)) _pic32_get_dcache_associativity (void);
size_t  __attribute__((far)) _pic32_get_icache_associativity (void);
size_t  __attribute__((far)) _pic32_get_dcache_linesize (void);
size_t  __attribute__((far)) _pic32_get_icache_linesize (void);
size_t  __attribute__((far)) _pic32_get_dcache_lines_per_way (void);
size_t  __attribute__((far)) _pic32_get_icache_lines_per_way (void);
size_t  __attribute__((far)) _pic32_get_dcache_size (void);
size_t  __attribute__((far)) _pic32_get_icache_size (void);


#endif //#ifndef _SYS_DEVCON_LOCAL_H

/*******************************************************************************
 End of File
*/


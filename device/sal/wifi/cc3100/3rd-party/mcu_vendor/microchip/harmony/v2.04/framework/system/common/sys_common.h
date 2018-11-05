/*******************************************************************************
  System Services Common Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sys_common.h

  Summary:
    Common System Services definitions and declarations.

  Description:
    This file provides commonsystem services definitions and declarations.

  Remarks:
    This file is included by "system.h" and does not need to be included 
    directly.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _SYS_COMMON_H_
#define _SYS_COMMON_H_


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "assert.h"

/* Compiler Header. */
#if defined(__XC32)
    #include <xc.h>
    #include <sys/attribs.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif


//**************************************************************************
/*
  Function:
     void SYS_ASSERT ( bool test, char *message )

  Summary:
    Implements default system assert routine, asserts that "test" is true.

  Description:
    This macro implements the default system assert routine that asserts
    that the provided boolean test is true.

  Preconditions:
    None, depending on the implementation to which this macro is mapped.

  Parameters:
    test    -  This is an expression that resolves to a boolean value
               (zero=false, non-zero=true)
               
    message -  This is a NULL-terminated ASCII character string that can be
               displayed on a debug output terminal if "test" is false (if
               supported).

  Return:
    None. Normally hangs in a loop, depending on the implementation to which
    it is mapped.

  Example:
    <code>
    void MyFunc ( int *pointer )
    {
        SYS_ASSERT(NULL != pointer, "NULL Pointer passed to MyFunc");

        Do something with pointer.
    }
    </code>

  Remarks:
    Can be overridden as desired by defining your own SYS_ASSERT macro
    before including system.h.

    The default definition removes this macro from all code because it adds
    significant size to all projects. The most efficient use is to enable
    it in individual libraries and build them separately for debugging.
*/

#ifndef SYS_ASSERT

    #define SYS_ASSERT(test,message)

#endif


// *****************************************************************************
/* Main Function Return Type

  Summary:
    Defines the correct return type for the "main" routine.

  Description:
    This macro defines the correct return type for the "main" routine for the
    selected Microchip microcontroller family.

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        // Initialize the system
        SYS_Initialize(...);

        // Main Loop
        while(true)
        {
            SYS_Tasks();
        }

        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>

  Remarks:
    The main function return type may change, depending upon which family of 
    Microchip microcontrollers is chosen. Refer to the user documentation for 
    the C-language compiler in use for more information.
*/

#if defined(__PIC32MX__) || defined(__C30__)||defined(__XC32) || defined(__PIC32C__)

    #define MAIN_RETURN int

#else

    #define MAIN_RETURN void

#endif


// *****************************************************************************
/* Main Routine Code Macro

  Summary:
    Casts the given value to the correct type for the return code from "main".

  Description:
    This macro cases the given value to the correct type for the return code 
    from the main function.

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        // Initialize the system
        SYS_Initialize(...);

        // Main Loop
        while(true)
        {
            SYS_Tasks();
        }

        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>

  Remarks:
    The main function return type may change, depending upon which family of 
    Microchip microcontrollers is chosen. Refer to the user documentation for 
    the C-language compiler in use for more information.
*/

#if defined(__PIC32MX__) || defined(__C30__)||defined(__XC32)

    #define MAIN_RETURN_CODE(c)     ((MAIN_RETURN)(c))

#else

    #define MAIN_RETURN_CODE(c)

#endif


// *****************************************************************************
/* Main Routine Codes Enumeration

  Summary:
    Defines return codes for "main".

  Description:
    This enumeration provides a predefined list of return codes for the main
    function.  These codes can be passed into the MAIN_RETURN_CODE macro to 
    convert them to the appropriate type (or discard them if not needed) for 
    the Microchip C-language compiler in use.

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        SYS_Initialize(...);

        while(true)
        {
            SYS_Tasks();
        }

        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>

  Remarks:
    The main function return type may change, depending upon which family of 
    Microchip microcontrollers is chosen. Refer to the user documentation for 
    the C-language compiler in use for more information.
*/

typedef enum
{
    MAIN_RETURN_FAILURE     = -1,

    MAIN_RETURN_SUCCESS     = 0

} MAIN_RETURN_CODES;


//******************************************************************************
/* Function:
    SYS_VersionGet( void )

  Summary:
    Gets SYS_COMMON version in numerical format.

  Description:
    This routine gets the SYS_COMMON version. The version is encoded as
    major * 10000 + minor * 100 + patch. The string version can be obtained
    using SYS_VersionStrGet()

  Parameters:
    None.

  Returns:
    Current driver version in numerical format.
*/

#define SYS_VersionGet( void ) SYS_VERSION


// *****************************************************************************
/* Macro:
    char * SYS_VersionStrGet ( void )

  Summary:
    Gets SYS_COMMON version in string format.

  Description:
    This routine gets the SYS_COMMON version in string format. The version is
    returned as major.minor.path[type], where type is optional. The numerical version can
    be obtained using SYS_VersionGet()

  Parameters:
    None.

  Returns:
    Current SYS_COMMON version in the string format.

  Remarks:
    None.
*/

#define SYS_VersionStrGet( void )   SYS_VERSION_STR


#ifdef __cplusplus
    }
#endif

#endif // _SYS_COMMON_H_

/*******************************************************************************
 End of File
*/

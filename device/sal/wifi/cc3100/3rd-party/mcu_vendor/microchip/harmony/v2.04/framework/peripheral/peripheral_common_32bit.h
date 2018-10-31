/*******************************************************************************
  Peripheral Library specific for 32 bit parts

  Company:
    Microchip Technology Inc.

  File Name:
    peripheral_common_32bit.h

  Summary:
    This file defines the 32-bit specific macros and definitions used by the peripheral
    library implementation headers.

  Description:
    This file defines the 32-bit specific macros and definitions used by the peripheral
    library implementation headers.  All the definitions in this file are
    private to the peripheral library implementation.

  Remarks:
    None
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _PLIB_COMMON_32BIT_H
#define _PLIB_COMMON_32BIT_H

#include <assert.h>
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/* These macros are used to access bits in registers in the peripheral libraries
*/

// *****************************************************************************
/* SFR_TYPE datatype

  Summary:
    Defines the type for SFR on the 32 bit parts

  Description:
    This type is specific to the SFR type in the 32 bit parts.

*/

typedef volatile uint32_t SFR_TYPE;


// *****************************************************************************
/* SFR_DATA datatype

  Summary:
    Defines the type for SFR data on the 32 bit parts

  Description:
    This type is specific to the SFR data in the 32 bit parts.

*/

typedef uint32_t SFR_DATA;


// *****************************************************************************
/* PIC32MZ kseg2/3 to physical address conversion macros

  Summary:
    Macros to convert between kseg2/3 virtual and physical addresses

  Description:
    Macros to convert between kseg2/3 virtual and physical addresses. Needed for
    DMA transfer to/from external memory on EBI or SQI with PIC32MZ.

*/

#define KVA2_TO_PA(v) (((v) & 0x3FFFFFFF) | 0x20000000)
#define KVA3_TO_PA(v) ((v) & 0x3FFFFFFF)
#define PA_TO_KVA2(v) (((v) & 0xDFFFFFFF) | 0xC0000000) 
#define PA_TO_KVA3(v) ((v) | 0xC0000000) 


// *****************************************************************************
/* Function:
    unsigned uintptr_t _PLIB_VirtToPhys ( const void* p )

  Summary:
    Translates a virtual pointer to a physical address

  Description:
    This function translates a virtual pointer to a physical address suitable
    for use with DMA and other hardware operations that require a physical
    address.

  Precondition:
    None.

  Parameters:
    p       - Pointer to be converted

  Returns:
    The physical address corresponding to the virtual input pointer.

  Example:
    <code>
    int         myData;
    uintptr_t   physAddr;
    physAddr = _PLIB_VirtToPhys(&myData);  // Safe to write to DMA register
    </code>

  Remarks:
    On parts that do not require this translation, this function returns
    the given address unchanged (other than by the data type to which
    it is cast).
*/

extern __inline__ uintptr_t __attribute__ ( ( always_inline ) ) _PLIB_VirtToPhys ( const void* p )
{
     return ( (((uintptr_t)(p))&0x80000000L) ? (((uintptr_t)(p))&0x1fffffffL) : (((uintptr_t)(p))|0x40000000L) );
}


// *****************************************************************************
/* Function:
   void* _PLIB_PhysToVirtK0 ( uintptr_t a )

  Summary:
    Translates a PIC32 K0 segment physical address to a virtual pointer

  Description:
    This function translates a PIC32 K0 segment physical address to a virtual
    pointer.

  Precondition:
    None.

  Parameters:
    a       - Physical address to be converted

  Returns:
    A virtual pointer corresponding to the given physical address.

  Example:
    <code>
    char *      pMyData;    // Virtual pointer to DMA data
    pMyData = _PLIB_PhysToVirtK0(PHY_ADDR_MY_DMA_BUFFER);
    </code>

  Remarks:
    On parts that do not require this translation, this function returns
    the given address unchanged (other than by the data type to which
    it is cast).
*/

extern __inline__ void* __attribute__ ( ( always_inline ) ) _PLIB_PhysToVirtK0 ( uintptr_t a )
{
	return ( ( a<0x40000000L )?( void* )( a|0x80000000L ):( void* )( a-0x40000000L ) );
}


// *****************************************************************************
/* Function:
   void* _PLIB_PhysToVirtK1 ( uintptr_t a )

  Summary:
    Translates a K1 segment physical address to a virtual pointer

  Description:
    This function translates a K1 segment physical address to a virtual
    pointer.

  Precondition:
    None.

  Parameters:
    a       - Physical address to be converted

  Returns:
    A virtual pointer corresponding to the given physical address.

  Example:
    <code>
    char *      pMyData;    // Virtual pointer to DMA data
    pMyData = _PLIB_PhysToVirtK1(PHY_ADDR_MY_DMA_BUFFER);
    </code>

  Remarks:
    On parts that do not require this translation, this function returns
    the given address unchanged (other than by the data type to which
    it is cast).
*/

extern __inline__ void* __attribute__((always_inline)) _PLIB_PhysToVirtK1( uintptr_t a )
{
	return (a<0x40000000L?(void*)(a|0xa0000000L):(void*)(a-0x40000000L));
}

// *****************************************************************************
// *****************************************************************************
// Section: 32-bit specific macros
// *****************************************************************************
// *****************************************************************************
/*  This section contains macros that utilize the CLR/SET/INV shadow registers
 *  in PIC32 devices.
*/

// *****************************************************************************
/* Register Operation:  Bit Set

  Summary:
    Sets the bit in the specified register.

  Description:
    This macro sets the specified bit in the specified register.
    The bit set is selected by the given bit position.

  Parameters:
    reg - SFR register name.
    pos - bit position within register.

  Returns:
    New value of the SFR register.

  Example:
  <code>
    PLIB_TEMPLATE void PLIB_USB_OTG_VBusChargeEnable ( USB_MODULE_ID index )
    {
        _SFR_BIT_SET(_USB_VBUS_CHARGE_PULLUP_VREG(index),
                     _USB_VBUS_CHARGE_PULLUP_POS(index) );
    }
  </code>

  Remarks:
    The compiler should optimize the macro to a single bit-set instruction on
    processors that support bit-set instructions.  The specific instruction will
    be dependent on the architecture on which the macro is being used.

    This operation is not guaranteed to access the register atomically.
    It may perform a read-modify-write operation.
*/

#define _SFR_BIT_SET(reg,pos)        ( *((SFR_TYPE *)((reg)+(2))) = 1<<(pos) )


// *****************************************************************************
/* Register Operation:  Bit Clear

  Summary:
    Clears the bit in the specified register.

  Description:
    This macro clear the specified bit in the specified register.
    The bit cleared is selected by the given bit position.

  Parameters:
    reg - SFR register name.
    pos - bit position within register.

  Returns:
    New value of the SFR register.

  Example:
  <code>
    PLIB_TEMPLATE void PLIB_USB_OTG_VBusChargeDisable ( USB_MODULE_ID index )
    {
        _SFR_BIT_CLEAR(_USB_VBUS_CHARGE_PULLUP_VREG(index),
                       _USB_VBUS_CHARGE_PULLUP_POS(index) );
    }
  </code>

  Remarks:
	This operation adds an offset of '1' to access the 'CLR' register
	corresponding to the register specified.
*/

#define _SFR_BIT_CLEAR(reg,pos)      ( *((SFR_TYPE *)((reg)+(1))) = 1<<(pos) )


// *****************************************************************************
/* Register Operation:  Bit Invert

  Summary:
    Inverts the bit in the specified register.

  Description:
    This macro invert the specified bit in the specified register.
    The bit inverted is selected by the given bit position.

  Parameters:
    reg - SFR register name.
    pos - bit position within register.

  Returns:
    New value of the SFR register.

  Example:
  <code>
  </code>

  Remarks:

	This operation adds an offset of '3' to access the 'INV' register
	corresponding to the register specified.
*/

#define _SFR_BIT_INVERT(reg,pos)     ( *((SFR_TYPE *)((reg)+(3))) = 1<<(pos) )


// *****************************************************************************
/* Register Operation:  Field Set

  Summary:
    Sets bits in the field of a specified register.

  Description:
    This macro sets bits in the specified field of the specified register.
    The field modified is selected by the given field position and field mask.
    The bits set within the field are specified by the value.

  Parameters:
    reg - SFR register name.
    mask - field bit mask within the register.
    pos - bit position within register.
    val - Each "1" bit in val field indicates the corresponding bit in
          the field is set to "1", with the LSb of val justified to be at bit zero.

  Returns:
    New value of the SFR register.

  Example:
  <code>
    PLIB_TEMPLATE void PLIB_USB_OTG_VBusChargeEnable ( USB_MODULE_ID index )
    {
        _SFR_FIELD_SET(_USB_VBUS_CHARGE_PULLUP_VREG(index),
                       _USB_VBUS_CHARGE_PULLUP_MASK(index),
                       _USB_VBUS_CHARGE_PULLUP_POS(index) ,
                       0x1                                );
    }
  </code>

  Remarks:

	This operation adds an offset of '2' to access the 'SET' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_FIELD_SET(reg,mask,pos,val)    (  *((SFR_TYPE *)((reg)+(2))) = ( (mask)&((val)<<(pos)) )  )


// *****************************************************************************
/* Register Operation:  Field Clear

  Summary:
    Clears bits in the field of a specified register

  Description:
    This macro clears bits in the specified field of the specified register.
    The field modified is selected by the given field position and field mask.
    The bits cleared within the field are specified by the value

  Parameters:
    reg - SFR register name.
    mask - field bit mask within the register.
    pos - bit position within register.
    val - Each "1" bit in val field indicates the corresponding bit in
          the field is cleared to "0", with the LSb of val justified to be at bit zero.

  Returns:
    New value of the SFR register.

  Example:
  <code>
    PLIB_TEMPLATE void PLIB_USB_OTG_VBusChargeDisable ( USB_MODULE_ID index )
    {
        _SFR_FIELD_CLEAR(_USB_VBUS_CHARGE_PULLUP_VREG(index),
                         _USB_VBUS_CHARGE_PULLUP_MASK(index),
                         _USB_VBUS_CHARGE_PULLUP_POS(index) ,
                         0x1                                );
    }
  </code>

  Remarks:
	This operation adds an offset of '1' to access the 'CLR' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_FIELD_CLEAR(reg,mask,pos,val)  ( *((SFR_TYPE *)((reg)+(1))) = ( (mask)&((val)<<(pos)) ) )


// *****************************************************************************
/* Register Operation:  Field Invert

  Summary:
    Inverts bits in the field of a specified register

  Description:
    This macro inverts bits in the specified field of the specified register.
    The field modified is selected by the given field position and field mask.
    The bits inverted within the field are specified by the value

  Parameters:
    reg - SFR register name.
    mask - field bit mask within the register.
    pos - bit position within register.
    val - Each "1" bit in val field indicates the corresponding bit in
          the field is inverted (0->1,1->0), with the LSb of val justified
          to be at bit zero

  Returns:
    New value of the SFR register.

  Example:
  <code>
  </code>

  Remarks:
	This operation adds an offset of '3' to access the 'INV' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_FIELD_INVERT(reg,mask,pos,val) (  *((SFR_TYPE *)((reg)+(3))) = ( (mask)&((val)<<(pos)) )  )


//******************************************************************************
/* Register Operation:  SFR Set

  Summary:
    Sets the bits of the specified register using the bit map provided.

  Description:
    Sets the bits of the specified register using the bit map provided.
    The bits set are specified in the value provided.

  Parameters:
    reg - SFR register name.
    val - Each "1" bit in val indicates the corresponding bit in
          the register is set to "1".

  Returns:
    New value of the SFR register.

  Example:
  <code>
  </code>

  Remarks:
	This operation adds an offset of '2' to access the 'SET' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_SET(reg,val)    ( *((SFR_TYPE *)((reg)+(2))) = (val) )


//******************************************************************************
/* Register Operation:  SFR Clear

  Summary:
    Clears the bits of the specified register using the bit map provided.

  Description:
    Clears the bits of the specified register using the bit map provided.
    The bits cleared are specified in the value provided.

  Parameters:
    reg - SFR register name.
    val - Each "1" bit in val indicates the corresponding bit in
          the register is cleared to zero.

  Returns:
    New value of the SFR register.

  Example:
  <code>
  </code>

  Remarks:
	This operation adds an offset of '1' to access the 'CLR' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_CLEAR(reg,val)  ( *((SFR_TYPE *)((reg)+(1))) = (val) )


//******************************************************************************
/* Register Operation:  SFR Invert

  Summary:
    Inverts the bits of the specified register using the bit map provided.

  Description:
    Inverts the bits of the specified register using the bit map provided.
    The bits inverted are specified in the value provided.

  Parameters:
    reg - SFR register name.
    val - Each "1" bit in val indicates the corresponding bit in
          the register is inverted (0->1,1->0).

  Returns:
    New value of the SFR register.

  Example:
  <code>
  </code>

  Remarks:
    This operation adds an offset of '3' to access the 'INV' register
	corresponding to the register specified.

    Positions are measured from the least significant bit (LSb) of the register.
    Masks are position aligned.
    Values are zero aligned.
*/

#define _SFR_INVERT(reg,val) ( *((SFR_TYPE *)((reg)+(3))) = (val) )

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include "peripheral/peripheral_common.h"


#endif /* End of File _PLIB_COMMON_32BIT_H */


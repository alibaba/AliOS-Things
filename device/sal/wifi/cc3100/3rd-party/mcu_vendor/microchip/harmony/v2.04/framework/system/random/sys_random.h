/*******************************************************************************
  Random Number Generator (RNG) System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_random.h

  Summary:
    Random Number Generator System Service interface definition.

  Description:
    This file contains the interface definition for the Random Number Generator
    System Service.
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

#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "system/system.h"


// *****************************************************************************
// *****************************************************************************
// Section: SYS RANDOM Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Random Number Generator Service Initialization Data

*/

typedef struct
{
    uint32_t    seedPseudo;
    void        *seedCrypto;
    size_t      seedCryptoSize;


} SYS_RANDOM_INIT;


// *****************************************************************************
// *****************************************************************************
// Section: SYS RANDOM Module Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_RANDOM_Initialize( const SYS_RANDOM_INIT* const randomInit );

  Summary:
    Initializes the Random Number Generator system service.

  Description:
	This function initializes the Random Number System service, including seeding
	the Pseudo Random Number Generator, the Crypto RNG library, and setting the
	seed size.

  Precondition:
    None.

  Parameters:
    index - Module instance index.
    
    init  - initialization data for the random system service (cast of a pointer 
            to a SYS_RANDOM_INIT structure to a SYS_MODULE_INDEX structure 
            pointer).

  Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID.
    
  Example:
  <code>
    SYS_MODULE_OBJ rngObject;
    
    rngObject = SYS_RANDOM_Initialize();
    SYS_RANDOM_Deinitialize(rngObject);
  </code>

  Remarks:
    None.
*/

SYS_MODULE_OBJ SYS_RANDOM_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void SYS_RANDOM_Deinitialize( SYS_MODULE_OBJ object );

  Summary:
    Deinitializes the Random Number Generator system service.

  Description:
	This function deinitializes the Random Number Generator system service.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    object          - SYS RANDOM object handle, returned from SYS_RANDOM_Initialize

  Returns:
    None.
    
  Example:
    <code>
    SYS_MODULE_OBJ rngObject;
    
    rngObject = SYS_RANDOM_Initialize();
	</code>

  Remarks:
    None.
*/

void SYS_RANDOM_Deinitialize( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section: SYS RANDOM Pseudo Random Number Generator Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_RANDOM_PseudoSeedSet( uint32_t seed );

  Summary:
    Reseeds the Pseudo-random Number Generator.

  Description:
	This function inserts a seed value into the Pseudo-Random Number Generator.
	The value will be used to generate the next random number.

  Precondition:
    None.

  Parameters:
    seed - 32-bit value to use as the seed for the Pseudo RNG.

  Returns:
    None.
    
  Example:
    <code>
    uint32_t rngSeed = 0x12345678;

    SYS_RANDOM_PseudoSeedSet(rngSeed);
	<code>

  Remarks:
    None.
*/

void SYS_RANDOM_PseudoSeedSet( uint32_t seed );


// *****************************************************************************
/* Function:
    uint32_t SYS_RANDOM_PseudoGet( void );

  Summary:
    Returns a random value from the Pseudo-random Number Generator.

  Description:
	This function returns one 32-bit Pseudo-random Number.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    A 32-bit pseudo-random number.
    
  Example:
    <code>
    uint32_t rngNumber;

    rngNumber = SYS_RANDOM_PseudoGet();
	</code>

  Remarks:
    None.
*/

uint32_t SYS_RANDOM_PseudoGet( void );


// *****************************************************************************
// *****************************************************************************
// Section: SYS RANDOM Cryptographic Random Number Generator Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_RANDOM_CryptoSeedSet( void *seed, size_t size);

  Summary:
    Reseeds the cryptographic quality Random Number Generator. 

  Description:
	This function inserts a seed value into the Cryptographic-quality Random
	Number Generator. The value will be used to generate the next random number.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    seed - Pointer to the memory location containing the new seed.
    size - Must be less or equal to SYS_RANDOM_CRYPTO_SEED_SIZE

  Returns:
    None.
    
  Example:
    <code>
    SYS_MODULE_OBJ rngObject;
    uint8_t rngSeed[SYS_RANDOM_CRYPTO_SEED_SIZE];
    
    rngObject = SYS_RANDOM_Initialize();
    // Fill the rngSeed
    SYS_RANDOM_CryptoSeedSet(rngSeed, sizeof(rngSeed));
	</code>

  Remarks:
    None.
*/

void SYS_RANDOM_CryptoSeedSet( void *seed, size_t size);


// *****************************************************************************
/* Function:
    size_t SYS_RANDOM_CryptoSeedSizeGet( void );

  Summary:
    Reseeds the cryptographic quality Random Number Generator. 

  Description:
	This function returns the current size of the seed used in the Cryptographic-
	quality Random Number Generator.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    None.
    
  Returns:
    Returns the size (in bytes) of the most recently set seed value for the 
    cryptographic strength random number generator.
    
  Example:
    <code>
    SYS_MODULE_OBJ rngObject;
    size_t seedSize;
    
    rngObject = SYS_RANDOM_Initialize();
    ...
    seedSize = SYS_RANDOM_CryptoSeedSizeGet();
	</code>

  Remarks:
    None.
*/

size_t SYS_RANDOM_CryptoSeedSizeGet( void );


// *****************************************************************************
/* Function:
    uint32_t SYS_RANDOM_CryptoGet( void );

  Summary:
    Returns a random 32 bit value from the cryptographic quality Random Number 
	Generator. 

  Description:
	This function returns a 32-bit random number from the Cryptographic-
	quality Random Number Generator.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    None.

  Returns:
    32-bit random number.
    
  Example:
    <code>
    SYS_MODULE_OBJ rngObject;
    uint32_t rngData;
    
    rngObject = SYS_RANDOM_Initialize();
    ...
    rngData = SYS_RANDOM_CryptoGet();
	</code>

  Remarks:
    On PIC32MZ devices with a hardware Random Number Generator, this function 
    returns a value from that generator. Other devices will use a software 
    library to generate the random number.
*/

uint32_t SYS_RANDOM_CryptoGet( void );


// *****************************************************************************
/* Function:
    void SYS_RANDOM_CryptoBlockGet( uint8_t buffer, size_t bufferSize );

  Summary:
    Generates a sequence of random bytes using the cryptographic quality Random
	Number Generator. 

  Description:
	This function uses the Cryptographic-quality Random Number Generator to fill
	a block of data with random numbers.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    buffer - Pointer to the memory location to fill with random data.
    size - The amount of random data, in bytes, to put in memory.

  Returns:
    None.
    
  Example:
    <code>
    #define	RNG_DATA_SIZE	32

    uint8_t	randomData[RNG_DATA_SIZE];
    SYS_MODULE_OBJ rngObject;

    rngObject = SYS_RANDOM_Initialize();
    ...
    SYS_RANDOM_CryptoBlockGet((void *)randomData, RNG_DATA_SIZE);
	</code>

  Remarks:
    When run, it will fill the memory starting at buffer with size bytes of 
    random numbers.
*/

void SYS_RANDOM_CryptoBlockGet( void *buffer, size_t size );


// *****************************************************************************
/* Function:
    uint8_t SYS_RANDOM_CryptoByteGet( void );

  Summary:
    Returns a random byte from the cryptographic quality Random Number Generator. 

  Description:
	This function returns a single byte from the Cryptographic-quality Random
	Number Generator.

  Precondition:
    System service must be initialized by calling SYS_RANDOM_Initialize before 
    this function is called.

  Parameters:
    None.

  Returns:
    Returns one byte of cryptographic-quality random data.
    
  Example:
    <code>
    uint8_t	randomData;
    SYS_MODULE_OBJ rngObject;

    rngObject = SYS_RANDOM_Initialize();
    randomData = SYS_RANDOM_CryptoByteGet();
	</code>

  Remarks:
    None.
*/

uint8_t SYS_RANDOM_CryptoByteGet( void );


// *****************************************************************************
/* Function:
    void SYS_RANDOM_CryptoEntropyAdd( uint8_t data );

  Summary:
    Adds randomness to the cryptographic quality Random Number Generator. 

  Description:
	This function adds entropy to the Cryptographic-quality Random Number
	Generator in order to adjust the randomness of the generated numbers.

  Precondition:
    None.

  Parameters:
    data - 8-bit value to add to the entropy calculation.

  Returns:
    None.
    
  Example:
    <code>
    SYS_MODULE_OBJ rngObject;

    rngObject = SYS_RANDOM_Initialize();
    SYS_RANDOM_CryptoEntropyAdd(23);
    </code>
	
  Remarks:
    This function currently does not function.
*/

void SYS_RANDOM_CryptoEntropyAdd( uint8_t data );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_SYS_RANDOM_H

/*******************************************************************************
 End of File
*/

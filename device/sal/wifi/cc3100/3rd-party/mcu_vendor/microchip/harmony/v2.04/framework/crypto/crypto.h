/**************************************************************************
  Crypto Framework Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    crypto.h
  
  Summary:
    Crypto Framework Library header for cryptographic functions.

  Description:
    This header file contains function prototypes and definitions of
    the data types and constants that make up the Cryptographic Framework
    Library for PIC32 families of Microchip microcontrollers.
**************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright © 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
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

/* Defines Microchip CRYPTO API layer */


#ifndef MC_CRYPTO_API_H
#define MC_CRYPTO_API_H


#ifdef __cplusplus
    extern "C" {
#endif

/* MD5 */
typedef struct CRYPT_MD5_CTX {
    int holder[110];   /* big enough to hold internal, but check on init */
} CRYPT_MD5_CTX;

//******************************************************************************
/* Function:
    int CRYPT_MD5_Initialize(CRYPT_MD5_CTX* md5)

  Summary:
    Initializes the internal structures necessary for MD5 hash calculations.

  Description:
    This function initializes the internal structures necessary for MD5 hash calculations.

  Precondition:
	None.
	
  Parameters:
    md5             - Pointer to CRYPT_MD5_CTX structure which holds the hash values.

  Returns:
    - BAD_FUNC_ARG	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_MD5_CTX md5;
	uint8_t buffer[1024];
	uint8_t md5sum[MD5_DIGEST_SIZE];
	
    CRYPT_MD5_Initialize(&md5);
	CRYPT_MD5_DataAdd(&md5, buffer, sizeof(buffer));
	CRYPT_MD5_Finalize(&md5, md5sum);
    </code>

  Remarks:
	All MD5 hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_MD5_Initialize(CRYPT_MD5_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_MD5_DataSizeSet(CRYPT_MD5_CTX* md5, unsigned int msgSize)

  Summary:
    For PIC32MZ hardware encryption, sets the size of the input data.

  Description:
    The PIC32MZ hardware encryption module needs to know the size of the data
    before it starts processing. This function sets that value.

  Precondition:
	None.

  Parameters:
    md5             - Pointer to CRYPT_MD5_CTX structure which holds the hash values.
    msgSize         - Size of the data (in bytes) that will be processed.

  Returns:
    - BAD_FUNC_ARG	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_MD5_CTX md5;
	uint8_t buffer[1024];
	uint8_t md5sum[MD5_DIGEST_SIZE];

        CRYPT_MD5_Initialize(&md5);
        CRYPT_MD5DataSizeSet(&md5, sizeof(buffer));
	CRYPT_MD5_DataAdd(&md5, buffer, sizeof(buffer));
	CRYPT_MD5_Finalize(&md5, md5sum);
    </code>

  Remarks:
	All MD5 hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_MD5_DataSizeSet(CRYPT_MD5_CTX*, unsigned int);

//********************************************************************************************
/*
  Function:
    int CRYPT_MD5_DataAdd(CRYPT_MD5_CTX* md5, const unsigned char* input, unsigned int sz)
    
  Summary:
    Updates the hash with the data provided.
	
  Description:
    This function updates the hash with the data provided.
	
  Preconditions:
    The MD5 context must be initialized prior to the first call of this
    function. The context must not be modified by code outside of this
    function.
	
  Parameters:
    md5   -  Pointer to CRYPT_MD5_CTX structure which holds the hash values.
    input -  Pointer to the data to use to update the hash.
    sz    -  Size of the data (in bytes) of the data to use to update the hash.
  Returns:
      * BAD_FUNC_ARG - An invalid pointer was passed to the function, either in md5 or input
      * 0 - An invalid pointer was not passed to the function
	  
  Example:
    <code>
    CRYPT_MD5_CTX md5;
    uint8_t buffer[1024];
    uint8_t md5sum[MD5_DIGEST_SIZE];
    
    CRYPT_MD5_Initialize(&md5);
    CRYPT_MD5_DataAdd(&md5, buffer, sizeof(buffer));
    CRYPT_MD5_Finalize(&md5, md5sum);
    </code>
	
  Remarks:
    To preserve the validity of the MD5 hash, nothing must modify
    the context holding variable between calls to CRYPT_MD5_DataAdd.                         
*/

int CRYPT_MD5_DataAdd(CRYPT_MD5_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_MD5_Finalize(CRYPT_MD5_CTX* md5, unsigned char* digest)

  Summary:
    Finalizes the hash and puts the result into digest.

  Description:
    This function finalizes the hash and puts the result into digest.

  Precondition:
	The MD5 context must be initialized prior to calling this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    md5             - Pointer to CRYPT_MD5_CTX structure which holds the hash values.
	digest			- Pointer to byte array to store hash result.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in md5 or digest.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_MD5_CTX md5;
	uint8_t buffer[1024];
	uint8_t md5sum[MD5_DIGEST_SIZE];
	
    CRYPT_MD5_Initialize(&md5);
	CRYPT_MD5_DataAdd(&md5, buffer, sizeof(buffer));
	CRYPT_MD5_Finalize(&md5, md5sum);
    </code>

  Remarks:
	In order to preserve the validity of the MD5 hash, nothing must modify the
	context holding variable between calls to CRYPT_MD5_DataAdd and CRYPT_MD5_Finalize.
*/

int CRYPT_MD5_Finalize(CRYPT_MD5_CTX*, unsigned char*);

enum {
    CRYPT_MD5_DIGEST_SIZE = 16 
};


/* SHA */
typedef struct CRYPT_SHA_CTX 
{
    /* big enough to hold internal, but check on init */
    int holder[110] __attribute__((aligned (128)));
} CRYPT_SHA_CTX;

//******************************************************************************
/* Function:
    int CRYPT_SHA_Initialize(CRYPT_SHA_CTX* sha)

  Summary:
    Initializes the internal structures necessary for SHA hash calculations.

  Description:
    This function initializes the internal structures necessary for SHA hash calculations.

  Precondition:
	None.
	
  Parameters:
    sha             - Pointer to CRYPT_SHA_CTX structure which holds the hash values.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA_CTX sha;
	uint8_t shaSum[SHA_DIGEST_SIZE];
	
    CRYPT_SHA_Initialize(&sha);
	CRYPT_SHA_DataAdd(&sha, buffer, sizeof(buffer));
	CRYPT_SHA_Finalize(&sha, shaSum);
    </code>

  Remarks:
	All SHA hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA_Initialize(CRYPT_SHA_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_SHA_DataSizeSet(CRYPT_SHA_CTX* sha, unsigned int msgSize)

  Summary:
    For PIC32MZ hardware encryption, sets the size of the input data.

  Description:
    The PIC32MZ hardware encryption module needs to know the size of the data
    before it starts processing. This function sets that value.

  Precondition:
	None.

  Parameters:
    sha             - Pointer to CRYPT_SHA_CTX structure which holds the hash values.
    msgSize         - Size of the data (in bytes) that will be processed.

  Returns:
    - BAD_FUNC_ARG	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA_CTX sha;
	uint8_t buffer[1024];
	uint8_t shasum[SHA_DIGEST_SIZE];

        CRYPT_SHA_Initialize(&sha);
        CRYPT_SHADataSizeSet(&sha, sizeof(buffer));
	CRYPT_SHA_DataAdd(&sha, buffer, sizeof(buffer));
	CRYPT_SHA_Finalize(&sha, shasum);
    </code>

  Remarks:
	All SHA hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA_DataSizeSet(CRYPT_SHA_CTX*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA_DataAdd(CRYPT_SHA_CTX* sha, const unsigned char* input, unsigned int sz)

  Summary:
    Updates the hash with the data provided. 

  Description:
    This function updates the hash with the data provided.

  Precondition:
	The SHA context must be initialized prior to the first call of this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha             - Pointer to CRYPT_SHA_CTX structure which holds the hash values.
    input			- Pointer to the data to use to update the hash.
    sz				- Size of the data (in bytes) of the data to use to update the hash.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha or input.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA_CTX sha;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA_DIGEST_SIZE];
	
    CRYPT_SHA_Initialize(&sha);
	CRYPT_SHA_DataAdd(&sha, buffer, sizeof(buffer));
	CRYPT_SHA_Finalize(&sha, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA_DataAdd.
*/

int CRYPT_SHA_DataAdd(CRYPT_SHA_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA_Finalize(CRYPT_SHA_CTX* sha, unsigned char* digest)

  Summary:
    Finalizes the hash and puts the result into digest.

  Description:
    This function finalizes the hash and puts the result into digest.

  Precondition:
	The SHA context must be initialized prior to calling this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha             - Pointer to CRYPT_SHA_CTX structure which holds the hash values.
	digest			- Pointer to byte array to store hash result.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha or digest.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA_CTX sha;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA_DIGEST_SIZE];
	
    CRYPT_SHA_Initialize(&sha);
	CRYPT_SHA_DataAdd(&sha, buffer, sizeof(buffer));
	CRYPT_SHA_Finalize(&sha, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA_DataAdd and CRYPT_SHA_Finalize.
*/

int CRYPT_SHA_Finalize(CRYPT_SHA_CTX*, unsigned char*);

enum {
    CRYPT_SHA_DIGEST_SIZE = 20
};


/* SHA-256 */
typedef struct CRYPT_SHA256_CTX {
    /* big enough to hold internal, but check on init */
    int holder[110] __attribute__((aligned (128)));
} CRYPT_SHA256_CTX;

//******************************************************************************
/* Function:
    int CRYPT_SHA256_Initialize(CRYPT_SHA256_CTX* sha256)

  Summary:
    Initializes the internal structures necessary for SHA256 hash calculations.

  Description:
    This function initializes the internal structures necessary for SHA256 hash calculations.

  Precondition:
	None.
	
  Parameters:
    sha256			- Pointer to context which saves state between calls.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA256_CTX sha;
	uint8_t shaSum[SHA256_DIGEST_SIZE];
	
    CRYPT_SHA256_Initialize(&sha);
	CRYPT_SHA256_DataAdd(&sha, buffer, sizeof(buffer));
	CRYPT_SHA256_Finalize(&sha, shaSum);
    </code>

  Remarks:
	All SHA hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA256_Initialize(CRYPT_SHA256_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_SHA256_DataSizeSet(CRYPT_SHA256_CTX* sha256, unsigned int msgSize)

  Summary:
    For PIC32MZ hardware encryption, sets the size of the input data.

  Description:
    The PIC32MZ hardware encryption module needs to know the size of the data
    before it starts processing. This function sets that value.

  Precondition:
	None.

  Parameters:
    sha256             - Pointer to CRYPT_SHA256_CTX structure which holds the hash values.
    msgSize         - Size of the data (in bytes) that will be processed.

  Returns:
    - BAD_FUNC_ARG	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA256_CTX sha256;
	uint8_t buffer[1024];
	uint8_t sha256sum[SHA256_DIGEST_SIZE];

        CRYPT_SHA256_Initialize(&sha256);
        CRYPT_SHA256DataSizeSet(&sha256, sizeof(buffer));
	CRYPT_SHA256_DataAdd(&sha256, buffer, sizeof(buffer));
	CRYPT_SHA256_Finalize(&sha256, sha256sum);
    </code>

  Remarks:
	All SHA256 hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA256_DataSizeSet(CRYPT_SHA256_CTX*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA256_DataAdd(CRYPT_SHA256_CTX* sha256, const unsigned char* input, unsigned int sz)

  Summary:
    Updates the hash with the data provided. 

  Description:
    This function updates the hash with the data provided.

  Precondition:
	The SHA256 context must be initialized prior to the first call of this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha256          - Pointer to CRYPT_SHA256_CTX structure which holds the hash values.
    input			- Pointer to the data to use to update the hash.
    sz				- Size of the data (in bytes) of the data to use to update the hash.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha256 or input.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA256_CTX sha256;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA256_DIGEST_SIZE];
	
    CRYPT_SHA256_Initialize(&sha256);
	CRYPT_SHA256_DataAdd(&sha256, buffer, sizeof(buffer));
	CRYPT_SHA256_Finalize(&sha256, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA256 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA256_DataAdd.
*/

int CRYPT_SHA256_DataAdd(CRYPT_SHA256_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA256_Finalize(CRYPT_SHA256_CTX* sha256, unsigned char* digest)

  Summary:
    Finalizes the hash and puts the result into digest.

  Description:
    This function finalizes the hash and puts the result into digest.

  Precondition:
	The SHA256 context must be initialized prior to calling this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha256          - Pointer to CRYPT_SHA256_CTX structure which holds the hash values.
	digest			- Pointer to byte array to store hash result.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha or digest.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA256_CTX sha256;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA256_DIGEST_SIZE];
	
    CRYPT_SHA256_Initialize(&sha256);
	CRYPT_SHA256_DataAdd(&sha256, buffer, sizeof(buffer));
	CRYPT_SHA256_Finalize(&sha256, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA256 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA256_DataAdd and CRYPT_SHA256_Finalize.
*/

int CRYPT_SHA256_Finalize(CRYPT_SHA256_CTX*, unsigned char*);

enum {
    CRYPT_SHA256_DIGEST_SIZE = 32 
};


/* SHA-384 */
typedef struct CRYPT_SHA384_CTX {
    long long holder[32];   /* big enough to hold internal, but check on init */
} CRYPT_SHA384_CTX;

//******************************************************************************
/* Function:
    int CRYPT_SHA384_Initialize(CRYPT_SHA384_CTX* sha384)

  Summary:
    Initializes the internal structures necessary for SHA384 hash calculations.

  Description:
    This function initializes the internal structures necessary for SHA384 hash calculations.

  Precondition:
	None.
	
  Parameters:
    sha384          - Pointer to CRYPT_SHA384_CTX structure which holds the hash values.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function
	- 0 			- An invalid pointer was not passed to the function

  Example:
    <code>
	CRYPT_SHA384_CTX sha384;
	uint8_t shaSum[SHA384_DIGEST_SIZE];
	
    CRYPT_SHA384_Initialize(&sha384);
	CRYPT_SHA384_DataAdd(&sha384, buffer, sizeof(buffer));
	CRYPT_SHA384_Finalize(&sha384, shaSum);
    </code>

  Remarks:
	All SHA384 hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA384_Initialize(CRYPT_SHA384_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_SHA384_DataAdd(CRYPT_SHA384_CTX* sha384, const unsigned char* input, unsigned int sz)

  Summary:
    Updates the hash with the data provided. 

  Description:
    This function updates the hash with the data provided.

  Precondition:
	The SHA384 context must be initialized prior to the first call of this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha384          - Pointer to CRYPT_SHA384_CTX structure which holds the hash values.
    input			- Pointer to the data to use to update the hash.
    sz				- Size of the data (in bytes) of the data to use to update the hash.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha384 or input.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA384_CTX sha384;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA384_DIGEST_SIZE];
	
    CRYPT_SHA384_Initialize(&sha384);
	CRYPT_SHA384_DataAdd(&sha384, buffer, sizeof(buffer));
	CRYPT_SHA384_Finalize(&sha384, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA384 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA384_DataAdd.
*/

int CRYPT_SHA384_DataAdd(CRYPT_SHA384_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA384_Finalize(CRYPT_SHA384_CTX* sha384, unsigned char* digest)

  Summary:
    Finalizes the hash and puts the result into digest.

  Description:
    This function finalizes the hash and puts the result into digest.

  Precondition:
	The SHA384 context must be initialized prior to calling this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha384          - Pointer to CRYPT_SHA384_CTX structure which holds the hash values.
	digest			- Pointer to byte array to store hash result.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha384 or digest.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA384_CTX sha384;
	uint8_t buffer[1024];
	uint8_t shaSum[SHA384_DIGEST_SIZE];
	
    CRYPT_SHA384_Initialize(&sha384);
	CRYPT_SHA384_DataAdd(&sha384, buffer, sizeof(buffer));
	CRYPT_SHA384_Finalize(&sha384, shaSum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA384 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA384_DataAdd and CRYPT_SHA384_Finalize.
*/

int CRYPT_SHA384_Finalize(CRYPT_SHA384_CTX*, unsigned char*);

enum {
    CRYPT_SHA384_DIGEST_SIZE = 48
};


/* SHA-512 */
typedef struct CRYPT_SHA512_CTX {
    long long holder[36];   /* big enough to hold internal, but check on init */
} CRYPT_SHA512_CTX;

//******************************************************************************
/* Function:
    int CRYPT_SHA512_Initialize(CRYPT_SHA512_CTX* sha512)

  Summary:
    Initializes the internal structures necessary for SHA512 hash calculations.

  Description:
    This function initializes the internal structures necessary for SHA512 hash calculations.

  Precondition:
	None.
	
  Parameters:
    sha512          - Pointer to CRYPT_SHA512_CTX structure which holds the hash values.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA512_CTX sha512;
	uint8_t sha512Sum[SHA512_DIGEST_SIZE];
	
    CRYPT_SHA512_Initialize(&sha512);
	CRYPT_SHA512_DataAdd(&sha512, buffer, sizeof(buffer));
	CRYPT_SHA512_Finalize(&sha512, sha512Sum);
    </code>

  Remarks:
	All SHA512 hashes have to start at a particular value before adding new data
	to it. This function sets the necessary values for the structure.
*/

int CRYPT_SHA512_Initialize(CRYPT_SHA512_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_SHA512_DataAdd(CRYPT_SHA512_CTX* sha512, const unsigned char* input, unsigned int sz)

  Summary:
    Updates the hash with the data provided. 

  Description:
    This function updates the hash with the data provided.

  Precondition:
	The SHA512 context must be initialized prior to the first call of this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha512          - Pointer to CRYPT_SHA512_CTX structure which holds the hash values.
    input			- Pointer to the data to use to update the hash.
    sz				- Size of the data (in bytes) of the data to use to update the hash.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha512 or input.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA512_CTX sha512;
	uint8_t buffer[1024];
	uint8_t sha512Sum[SHA512_DIGEST_SIZE];
	
    CRYPT_SHA512_Initialize(&sha512);
	CRYPT_SHA512_DataAdd(&sha512, buffer, sizeof(buffer));
	CRYPT_SHA512_Finalize(&sha512, sha512Sum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA512 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA512_DataAdd.
*/

int CRYPT_SHA512_DataAdd(CRYPT_SHA512_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_SHA512_Finalize(CRYPT_SHA512_CTX* sha512, unsigned char* digest)

  Summary:
    Finalizes the hash and puts the result into digest.

  Description:
    This function finalizes the hash and puts the result into digest.

  Precondition:
	The SHA512 context must be initialized prior to calling this function.
	The context must not be modified by code outside of this function.
	
  Parameters:
    sha512          - Pointer to CRYPT_SHA512_CTX structure which holds the hash values.
	digest			- Pointer to byte array to store hash result.

  Returns:
    - BAD_FUNC_ARG 	- An invalid pointer was passed to the function, either in sha512 or digest.
	- 0 			- An invalid pointer was not passed to the function.

  Example:
    <code>
	CRYPT_SHA512_CTX sha512;
	uint8_t buffer[1024];
	uint8_t sha512Sum[SHA512_DIGEST_SIZE];
	
    CRYPT_SHA512_Initialize(&sha512);
	CRYPT_SHA512_DataAdd(&sha512, buffer, sizeof(buffer));
	CRYPT_SHA512_Finalize(&sha512, sha512Sum);
    </code>

  Remarks:
	In order to preserve the validity of the SHA512 hash, nothing must modify the
	context holding variable between calls to CRYPT_SHA512_DataAdd and CRYPT_SHA512_Finalize.
*/

int CRYPT_SHA512_Finalize(CRYPT_SHA512_CTX*, unsigned char*);

enum {
    CRYPT_SHA512_DIGEST_SIZE = 64 
};


/* HMAC */
typedef struct CRYPT_HMAC_CTX {
    long long holder[80];   /* big enough to hold internal, but check on init */
} CRYPT_HMAC_CTX;

//*****************************************************************************
/*
  Function:
    int CRYPT_HMAC_SetKey(CRYPT_HMAC_CTX* hmac, int type, const unsigned char* key, unsigned int sz)
    
  Summary:
	Initializes the HMAC context and set the key for the hash.
	
  Description:
	This function initializes the HMAC context and set the key for the hash.
  
  Preconditions:
	None.
	
  Parameters:
    hmac -  Pointer to context which saves state between calls.
    type -  Type of SHA operation to use with HMAC. Must be one of the
            following:
            * CRYPT_HMAC_SHA
            * CRYPT_HMAC_SHA256
            * CRYPT_HMAC_SHA384
            * CRYPT_HMAC_SHA512
    key -   Secret key used for the hash.
	sz	-	Size of the input data in bytes.
	
  Returns:
    * BAD_FUNC_ARG 	- An invalid pointer was passed to the function. 
	* 0				- An invalid pointer was not passed to the function.
	
  Example:
    <code>
    CRYPT_HMAC_CTX mcHmac;
    byte           mcDigest[CRYPT_SHA512_DIGEST_SIZE];
    
    CRYPT_HMAC_SetKey(&mcHmac, CRYPT_HMAC_SHA, key, 4);
    
    CRYPT_HMAC_DataAdd(&mcHmac, ourData, OUR_DATA_SIZE);
    
    CRYPT_HMAC_Finalize(&mcHmac, mcDigest);
    </code>
	
  Remarks:
    None.  
*/

int CRYPT_HMAC_SetKey(CRYPT_HMAC_CTX*, int, const unsigned char*, unsigned int);

//*********************************************************************************
/*
  Function:
    int CRYPT_HMAC_DataAdd(CRYPT_HMAC_CTX*, const unsigned char*, unsigned int)
    
  Summary:
	Adds data to the HMAC calculation.
	
  Description:
	This function adds data to the HMAC so that multiple blocks of data can be processed.
	
  Preconditions:
    The CRYPT_HMAC_CTX context must be initialized using the
    CRYPT_HMAC_SetKey function prior to any call to this function.
	
  Parameters:
    hmac  -  Pointer to context that saves state between calls.
    input -  Pointer to the data to use to update the hash.
    sz    -  Size of the input data in bytes.
	
  Returns:
    * BAD_FUNC_ARG - An invalid pointer was passed to the function.
	* 0 - An invalid pointer was not passed to the function.
	
  Example:
    <code>
    CRYPT_HMAC_CTX mcHmac;
    byte           mcDigest[CRYPT_SHA512_DIGEST_SIZE];
    
    CRYPT_HMAC_SetKey(&mcHmac, CRYPT_HMAC_SHA, key, 4);
    
    CRYPT_HMAC_DataAdd(&mcHmac, ourData, OUR_DATA_SIZE);
    
    CRYPT_HMAC_Finalize(&mcHmac, mcDigest);
    </code>
	
  Remarks:
    None.
  
*/

int CRYPT_HMAC_DataAdd(CRYPT_HMAC_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_HMAC_Finalize(CRYPT_HMAC_CTX* hmac, unsigned char* digest)

  Summary:
	Completes the HMAC calculation and get the results.
	
  Description:
	This function completes the HMAC calculations. The results are placed in 
	the location pointed to by digest.

  Precondition:
    The CRYPT_HMAC_CTX context must be initialized using the
    CRYPT_HMAC_SetKey function prior to any call to this function.
	
  Parameters:
    hmac		- Pointer to context which saves state between calls.
    digest		- Pointer to place to put the final HMAC digest results.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_HMAC_CTX mcHmac;
    byte           mcDigest[CRYPT_SHA512_DIGEST_SIZE];

    CRYPT_HMAC_SetKey(&mcHmac, CRYPT_HMAC_SHA, key, 4);

    CRYPT_HMAC_DataAdd(&mcHmac, ourData, OUR_DATA_SIZE);

    CRYPT_HMAC_Finalize(&mcHmac, mcDigest);
    </code>

  Remarks:
	The area pointed to by digest must be large enough to hold the results.
*/

int CRYPT_HMAC_Finalize(CRYPT_HMAC_CTX*, unsigned char*);

/* HMAC types */
enum {
    CRYPT_HMAC_SHA    = 1, 
    CRYPT_HMAC_SHA256 = 2, 
    CRYPT_HMAC_SHA384 = 5, 
    CRYPT_HMAC_SHA512 = 4 
};


/* Huffman */

//******************************************************************************
/* Function:
    int CRYPT_HUFFMAN_Compress(unsigned char* out, unsigned int outSz, const unsigned char* in,
                               unsigned int inSz, unsigned int flags)

  Summary:
	Compresses a block of data.

  Description:
	This function compresses a block of data using Huffman encoding.

  Precondition:
	None.
	
  Parameters:
    out		- Pointer to location to store the compressed data.
    outSz	- Maximum size of the output data in bytes.
    in		- Point to location of source data.
    inSz	- Size of the input data in bytes.
    flags	- Flags to control how compress operates

  Returns:
	- negative	- error code
	- positive	- bytes stored in out buffer

  Example:
    <code>
	const unsigned char text[] = "...";
	unsigned int inSz = sizeof(text);
	unsigned int outSz;
	unsigned char cBuffer[1024];
	
	int ret;
	
	ret = CRYPT_HUFFMAN_COMPRESS(cBuffer, sizeof(cBuffer), text, inSz, 0);
    </code>

  Remarks:
	Output buffer must be large enough to hold the contents of the operation.
*/

int CRYPT_HUFFMAN_Compress(unsigned char*, unsigned int, const unsigned char*,
                           unsigned int, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_HUFFMAN_DeCompress(unsigned char* out, unsigned int outSz, const unsigned char* in, unsigned int inSz)

  Summary:
	Decompresses a block of data.

  Description:
	This function decompresses a block of data using Huffman encoding.

  Precondition:
	None.
	
  Parameters:
    out		- Pointer to destination buffer
    outSz	- Size of destination buffer
    in		- Pointer to source buffer to decompress
    inSz	- Size of source buffer to decompress

  Returns:
	- negative 	- Error code
	- positive	- Bytes stored in out buffer

  Example:
    <code>
    unsigned char cBuffer[1024];
    unsigned char dBuffer[1024];

	int ret
	
	ret = CRYPT_HUFFMAN_DeCompress(dBuffer, sizeof(dBuffer), cBuffer, msglen);
    </code>

  Remarks:
	Output buffer must be large enough to hold the contents of the operation.
*/

int CRYPT_HUFFMAN_DeCompress(unsigned char*, unsigned int, const unsigned char*,
                             unsigned int);

/* flag to use static huffman */
enum {
    CRYPT_HUFFMAN_COMPRESS_STATIC = 1 
};


/* RNG */
typedef struct CRYPT_RNG_CTX {
    int holder[66];   /* big enough to hold internal, but check on init */
} CRYPT_RNG_CTX;

//******************************************************************************
/* Function:
    int CRYPT_RNG_Initialize(CRYPT_RNG_CTX* rng)

  Summary:
	Initializes the random number generator.

  Description:
	This function initializes the context that stores information relative to
	random number generation.

  Precondition:
	None.
	
  Parameters:
    rng		- Pointer to random number generator context.

  Returns:
	- negative 	- An error occurred setting up the random number generator.
	- 0			- An invalid pointer was not passed to the function.

  Example:
    <code>
	#define RANDOM_BYTE_SZ 32

	int           ret;
	CRYPT_RNG_CTX mcRng;
	byte          out[RANDOM_BYTE_SZ];
	
	ret = CRYPT_RNG_Initialize(&mcRng);
	
	ret = CRYPT_RNG_Get(&mcRng, &out[0]);

    ret = CRYPT_RNG_BlockGenerate(&mcRng, out, RANDOM_BYTE_SZ);
    </code>

  Remarks:
*/

#define CRYPT_RNG_Initialize CRYPT_RNG_Initialize
int CRYPT_RNG_Initialize(CRYPT_RNG_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_RNG_Get(CRYPT_RNG_CTX* rng, unsigned char* b)

  Summary:
	Gets one random number.

  Description:
	This function gets one random number from the random number generator.

  Precondition:
	RNG context was initialized using the CRYPT_RNG_Initialize function.
	
  Parameters:
    rng		- Pointer to context which saves state between calls.
    b		- Pointer to 8-bit location to store the result.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- Less than 0	- An error occurred.
	- 0 or greater	- Success.

  Example:
    <code>
	#define RANDOM_BYTE_SZ 32

	int           ret;
	CRYPT_RNG_CTX mcRng;
	byte          out[RANDOM_BYTE_SZ];
	
	ret = CRYPT_RNG_Initialize(&mcRng);
	
	ret = CRYPT_RNG_Get(&mcRng, &out[0]);

    ret = CRYPT_RNG_BlockGenerate(&mcRng, out, RANDOM_BYTE_SZ);
    </code>

  Remarks:
*/

int CRYPT_RNG_Get(CRYPT_RNG_CTX*, unsigned char*);

//******************************************************************************
/* Function:
    int CRYPT_RNG_BlockGenerate(CRYPT_RNG_CTX* rng, unsigned char* b, unsigned int sz)

  Summary:
	Createa several random numbers.

  Description:
	This function generates several random numbers and places them in the space allocated.

  Precondition:
	RNG context was initialized using the CRYPT_RNG_Initialize function.
	
  Parameters:
    rng		- Pointer to context which saves state between calls.
    b		- Pointer to buffer to store the random numbers.
    sz		- Number of 8-bit random numbers to generate.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
	#define RANDOM_BYTE_SZ 32

	int           ret;
	CRYPT_RNG_CTX mcRng;
	byte          out[RANDOM_BYTE_SZ];
	
	ret = CRYPT_RNG_Initialize(&mcRng);
	
	ret = CRYPT_RNG_Get(&mcRng, &out[0]);

    ret = CRYPT_RNG_BlockGenerate(&mcRng, out, RANDOM_BYTE_SZ);
    </code>

  Remarks:
*/

int CRYPT_RNG_BlockGenerate(CRYPT_RNG_CTX*, unsigned char*, unsigned int);


/* TDES */
typedef struct CRYPT_TDES_CTX {
    int holder[104];   /* big enough to hold internal, but check on init */
} CRYPT_TDES_CTX;

//******************************************************************************
/* Function:
    int CRYPT_TDES_KeySet(CRYPT_TDES_CTX* tdes, const unsigned char* key, const unsigned char* iv, int dir)

  Summary:
	Initialization of Triple DES context.

  Description:
	This function sets the key and initialization vector (IV) for a set of Triple-DES operations.

  Precondition:
	None.
	
  Parameters:
    tdes	- Pointer to context which saves state between calls.
    key		- Pointer to buffer holding the key. Must be 24 bytes in size.
    iv		- Pointer to buffer holding the initialization vector. Must be 8 bytes in size.
    dir		- Indicates whether encryption or decryption will be done. Can be set to:
		* CRYPT_TDES_ENCRYPTION	- For Encryption operations
		* CRYPT_TDES_DECRYPTION	- Fro Decryption operations

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_TDES_CTX mcDes3;
    int            ret;
    byte           out1[TDES_SIZE];
    byte           out2[TDES_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmn", 24);
    strncpy((char*)iv,  "12345678", 8);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_ENCRYPTION);

    ret = CRYPT_TDES_CBC_Encrypt(&mcDes3, out1, ourData, TDES_SIZE);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_DECRYPTION);
	
    ret = CRYPT_TDES_CBC_Decrypt(&mcDes3, out2, out1, TDES_TEST_SIZE);
    </code>

  Remarks:
	The input data must be a multiple of 8 bytes, and must be padded at the end with zeros to meet the length.
*/

int CRYPT_TDES_KeySet(CRYPT_TDES_CTX*, const unsigned char*,
                      const unsigned char*, int);

//******************************************************************************
/* Function:
    int CRYPT_TDES_IvSet(CRYPT_TDES_CTX* tdes, const unsigned char* iv)

  Summary:
	Sets the Initialization Vector (IV) for a Triple DES operation.

  Description:
	This function changes the IV of a TDES context, but leaves the Key alone.

  Precondition:
	None.
	
  Parameters:
    tdes	- Pointer to context which saves state between calls.
    iv		- Pointer to buffer holding the initialization vector. Must be 8 bytes in size.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_TDES_CTX mcDes3;
    int            ret;
    byte           out1[TDES_SIZE];
    byte           out2[TDES_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmn", 24);
    strncpy((char*)iv,  "12345678", 8);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_ENCRYPTION);

	ret = CRYPT_TDES_IvSet(&mcDes3, iv);

    ret = CRYPT_TDES_CBC_Encrypt(&mcDes3, out1, ourData, TDES_SIZE);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_DECRYPTION);
	
    ret = CRYPT_TDES_CBC_Decrypt(&mcDes3, out2, out1, TDES_TEST_SIZE);
    </code>

  Remarks:
	The IV must be 8 bytes long.
*/

int CRYPT_TDES_IvSet(CRYPT_TDES_CTX*, const unsigned char*);

//******************************************************************************
/* Function:
    int CRYPT_TDES_CBC_Encrypt(CRYPT_TDES_CTX* tdes, unsigned char* out, const unsigned char* in, unsigned int inSz)

  Summary:
	Encrypts a data block using Triple DES.

  Description:
	This function encrypts a block of data using a Triple DES algorithm.

  Precondition:
	The context tdes must be set earlier using CRYPT_TDES_KeySet. The input block must be a multiple of 8 bytes long.
	
  Parameters:
    tdes	- Pointer to context which saves state between calls.
    out		- Pointer to output buffer to store the results.
    in		- Pointer to input buffer for the source of the data.
    inSz	- Size of the input data buffer.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_TDES_CTX mcDes3;
    int            ret;
    byte           out1[TDES_SIZE];
    byte           out2[TDES_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmn", 24);
    strncpy((char*)iv,  "12345678", 8);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_ENCRYPTION);

    ret = CRYPT_TDES_CBC_Encrypt(&mcDes3, out1, ourData, TDES_SIZE);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_DECRYPTION);
	
    ret = CRYPT_TDES_CBC_Decrypt(&mcDes3, out2, out1, TDES_TEST_SIZE);
    </code>

  Remarks:
	The input data must be padded at the end with zeros to make the length a multiple of 8.
*/

int CRYPT_TDES_CBC_Encrypt(CRYPT_TDES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_TDES_CBC_Decrypt(CRYPT_TDES_CTX* tdes, unsigned char* out, const unsigned char* in, unsigned int inSz)

  Summary:
	Decrypts a data block using Triple DES.

  Description:
	This function decrypts a block of data using a Triple DES algorithm.

  Precondition:
	The context tdes must be set earlier using CRYPT_TDES_KeySet. The input block must be a multiple of 8 bytes long.
	
  Parameters:
    tdes	- Pointer to context which saves state between calls.
    out		- Pointer to output buffer to store the results.
    in		- Pointer to input buffer for the source of the data.
    inSz	- Size of the input data buffer.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_TDES_CTX mcDes3;
    int            ret;
    byte           out1[TDES_SIZE];
    byte           out2[TDES_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmn", 24);
    strncpy((char*)iv,  "12345678", 8);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_ENCRYPTION);

    ret = CRYPT_TDES_CBC_Encrypt(&mcDes3, out1, ourData, TDES_SIZE);

    ret = CRYPT_TDES_KeySet(&mcDes3, key, iv, CRYPT_TDES_DECRYPTION);
	
    ret = CRYPT_TDES_CBC_Decrypt(&mcDes3, out2, out1, TDES_TEST_SIZE);
    </code>

  Remarks:
	Input data must have a length a multiple of 8 bytes. Output data will be
	zero-padded at the end if the original data was not a multiple of 8 bytes long.
*/

int CRYPT_TDES_CBC_Decrypt(CRYPT_TDES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);

/* key direction flags for setup */
enum {
    CRYPT_TDES_ENCRYPTION = 0,
    CRYPT_TDES_DECRYPTION = 1 
};


/* AES */
typedef struct CRYPT_AES_CTX {
    int holder[90];   /* big enough to hold internal, but check on init */
} CRYPT_AES_CTX;

//******************************************************************************
/* Function:
    int CRYPT_AES_KeySet(CRYPT_AES_CTX* aes, const unsigned char* key, 
	                  unsigned int keylen, const unsigned char* iv, int dir)

  Summary:
    Sets the key and initialization vector (IV) for AES processing.

  Description:
    This function sets the key and IV, and the direction (encryption or
	decryption) that AES will later perform.

  Precondition:
	None.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    key		- Pointer to buffer holding the key itself.
    keyLen	- Length of key in bytes.
    iv		- Pointer to buffer holding the initialization vector.
    dir		- Which operation (CRYPT_AES_ENCRYPTION or CRYPT_AES_DECRYPTION).

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    </code>

  Remarks:
*/

int CRYPT_AES_KeySet(CRYPT_AES_CTX*, const unsigned char*, unsigned int,
                      const unsigned char*, int);

//******************************************************************************
/* Function:
    int CRYPT_AES_IvSet(CRYPT_AES_CTX* aes, const unsigned char* iv)

  Summary:
    Sets the initialization vector (IV) for AES processing.

  Description:
    This function sets the IV that AES will use for later processing.

  Precondition:
	The key must be set previously with CRYPT_AES_KeySet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    iv		- Pointer to buffer holding the initialization vector.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;

    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_IvSet(&mcAes, iv);
    </code>

  Remarks:
	Direction, as set previously in CRYPT_AES_KeySet, is preserved.
*/

int CRYPT_AES_IvSet(CRYPT_AES_CTX*, const unsigned char*);

//******************************************************************************
/* Function:
    int CRYPT_AES_CBC_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)

  Summary:
	Performs AES encryption using Cipher-Block-Chaining (CBC).

  Description:
	This function encrypts a block of data using the AES algorithm in Cipher-
	Block-Chaining (CBC) mode.

  Precondition:
	Key and Initialization Vector (IV) must be set earlier with a call to
	CRYPT_AES_KeySet and CRYPT_AES_IvSet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    out		- Pointer to buffer to store the results of the encryption pass.
    in		- Pointer to buffer holding the data to be encrypted.
    inSz	- Size of the input data, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;
    byte          out1[AES_TEST_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    ret = CRYPT_AES_CBC_Encrypt(&mcAes, out1, ourData, AES_TEST_SIZE);
    </code>

  Remarks:
	The output buffer must be equal in size to the input buffer.
*/

int CRYPT_AES_CBC_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_AES_CBC_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)

  Summary:
	Performs AES decryption using Cipher-Block-Chaining (CBC).

  Description:
	This function decrypts a block of data using the AES algorithm in Cipher-
	Block-Chaining (CBC) mode.

  Precondition:
	Key and Initialization Vector (IV) must be set earlier with a call to
	CRYPT_AES_KeySet and CRYPT_AES_IvSet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    out		- Pointer to buffer to store the results of the decryption pass.
    in		- Pointer to buffer holding the data to be decrypted.
    inSz	- Size of the input data, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;
    byte          out1[AES_TEST_SIZE];
    byte          out2[AES_TEST_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    ret = CRYPT_AES_CBC_Decrypt(&mcAes, out2, out1, AES_TEST_SIZE);
    </code>

  Remarks:
	The output buffer must be equal in size to the input buffer.
*/

int CRYPT_AES_CBC_Decrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);


//******************************************************************************
/* Function:
    int CRYPT_AES_CTR_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)

  Summary:
	Performs AES encryption using Counter (CTR).

  Description:
	This function encrypts a block of data using the AES algorithm in Counter (CTR) mode.

  Precondition:
	Key and Initialization Vector (IV) must be set earlier with a call to
	CRYPT_AES_KeySet and CRYPT_AES_IvSet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    out		- Pointer to buffer to store the results of the encryption pass.
    in		- Pointer to buffer holding the data to be encrypted.
    inSz	- Size of the input data, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;
    byte          out1[AES_TEST_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    ret = CRYPT_AES_CTR_Encrypt(&mcAes, out1, ourData, AES_TEST_SIZE);
    </code>

  Remarks:
	The output buffer must be equal in size to the input buffer.
*/

int CRYPT_AES_CTR_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                          const unsigned char*, unsigned int);


//******************************************************************************
/* Function:
    int CRYPT_AES_DIRECT_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                           const unsigned char* in)

  Summary:
	Directs encryption of one block of data.

  Description:
	This function encrypts one block of data, equal to the AES block size.

  Precondition:
	Key and Initialization Vector (IV) must be set earlier with a call to
	CRYPT_AES_KeySet and CRYPT_AES_IvSet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    out		- Pointer to buffer to store the results of the encryption.
    in		- Pointer to buffer where the data to encrypt is located.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;
    byte          out1[CRYPT_AES_BLOCK_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    ret = CRYPT_AES_DIRECT_Encrypt(&mcAes, out1, ourData);
    </code>

  Remarks:
	Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

int CRYPT_AES_DIRECT_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*);

//******************************************************************************
/* Function:
    int CRYPT_AES_DIRECT_Decrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*)

  Summary:
	Directs decryption of one block of data.

  Description:
	This function decrypts one block of data, equal to the AES block size.

  Precondition:
	Key and Initialization Vector (IV) must be set earlier with a call to
	CRYPT_AES_KeySet and CRYPT_AES_IvSet.
	
  Parameters:
    aes		- Pointer to context which saves state between calls.
    out		- Pointer to buffer to store the results of the decryption.
    in		- Pointer to buffer where the data to decrypt is located.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_AES_CTX mcAes;
    int           ret;
    byte          out1[CRYPT_AES_BLOCK_SIZE];
    byte          out2[CRYPT_AES_BLOCK_SIZE];

    strncpy((char*)key, "1234567890abcdefghijklmnopqrstuv", 32);
    strncpy((char*)iv,  "1234567890abcdef", 16);

    ret = CRYPT_AES_KeySet(&mcAes, key, 16, iv, CRYPT_AES_ENCRYPTION);
    ret = CRYPT_AES_DIRECT_Decrypt(&mcAes, out2, out1);
    </code>

  Remarks:
	Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

int CRYPT_AES_DIRECT_Decrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*);

/* key direction flags for setup, ctr always uses ENCRYPT flag */
enum {
    CRYPT_AES_ENCRYPTION = 0,
    CRYPT_AES_DECRYPTION = 1,
    CRYPT_AES_BLOCK_SIZE = 16
};



/* RSA */
typedef struct CRYPT_RSA_CTX {
    void* holder;
} CRYPT_RSA_CTX;


//******************************************************************************
/* Function:
    int CRYPT_RSA_Initialize(CRYPT_RSA_CTX* rsa)

  Summary:
	Initializes the internal structures necessary for RSA processing.

  Description:
	This function initializes the context used during public-key RSA encryption
	and decryption.

  Precondition:
	None.
	
  Parameters:
    rsa		- Pointer to RSA context which saves state between calls.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- -1			- Unable to allocate the memory necessary for the key.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;

    ret = CRYPT_RSA_Initialize(&mcRsa);
    </code>

  Remarks:
*/

int CRYPT_RSA_Initialize(CRYPT_RSA_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_RSA_Free(CRYPT_RSA_CTX* rsa)

  Summary:
	Releases the memory used for the key and clean up the context.

  Description:
	This function releases the memory used during RSA processing for storing the
	public/private key.

  Precondition:
	The context must have been set up previously with a call to CRYPT_RSA_Initialize.
	
  Parameters:
    rsa		- Pointer to context which saves state between calls.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;

    ret = CRYPT_RSA_Initialize(&mcRsa);
	ret = CRYPT_RSA_Free(&mcRsa);
    </code>

  Remarks:
*/

int CRYPT_RSA_Free(CRYPT_RSA_CTX*);


//******************************************************************************
/* Function:
    int CRYPT_RSA_PublicKeyDecode(CRYPT_RSA_CTX* rsa, const unsigned char* in,
                              unsigned int inSz)

  Summary:
	Constructs the Public Key from a DER certificate.

  Description:
	This function builds a public key from a DER-formated certificate. DER stands for
	Distinguished Encoding Rules.

  Precondition:
	The context must have been initialized with a call to CRYPT_RSA_Initialize.
	
  Parameters:
    rsa		- Pointer to context which saves state between calls.
    in		- Pointer to buffer containing the certificate to process to extract the public key.
    inSz	- Size of the input data in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;
    unsigned int  keySz = (unsigned int)sizeof(client_key_der_1024);

    ret = CRYPT_RSA_Initialize(&mcRsa);

    ret = CRYPT_RSA_PublicKeyDecode(&mcRsa, client_key_der_1024, keySz);
    </code>

  Remarks:
*/

int CRYPT_RSA_PublicKeyDecode(CRYPT_RSA_CTX*, const unsigned char*,
                              unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_RSA_PrivateKeyDecode(CRYPT_RSA_CTX*, const unsigned char*,
                               unsigned int)

  Summary:
	Constructs the Private Key from a DER certificate.

  Description:
	This function builds a private key from a DER-formated certificate. DER stands for
	Distinguished Encoding Rules.

  Precondition:
	The context must have been initialized with a call to CRYPT_RSA_Initialize.
	
  Parameters:
    rsa		- Pointer to context which saves state between calls.
    in		- Pointer to buffer containing the certificate to process to extract the private key.
    inSz	- Size of the input data in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;
    unsigned int  keySz = (unsigned int)sizeof(client_key_der_1024);

    ret = CRYPT_RSA_Initialize(&mcRsa);

    ret = CRYPT_RSA_PrivateKeyDecode(&mcRsa, client_key_der_1024, keySz);
    </code>

  Remarks:
*/

int CRYPT_RSA_PrivateKeyDecode(CRYPT_RSA_CTX*, const unsigned char*,
                               unsigned int);


//******************************************************************************
/* Function:
    int CRYPT_RSA_PublicEncrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                            unsigned int outSz, const unsigned char* in, unsigned int inSz,
                            CRYPT_RNG_CTX* rng)

  Summary:
	Encrypts data using a public key.

  Description:
	This function encrypts a data block using a public key.

  Precondition:
	The context must be initialized using CRYPT_RSA_Initialized and the Public Key
	Decoded using CRYPT_RSA_PublicKeyDecode prior to calling this function.
	The random number generator must be initialized with a call to CRYPT_RNG_Initialize.
	
  Parameters:
    rsa		- Pointer to context which saves state between calls.
    out		- Pointer to output buffer to store results.
    outSz	- Size of output buffer.
    in		- Pointer to source buffer to be encrypted.
    inSz	- Size of input buffer.
	rng		- Pointer to Random Number Generator (RNG) context.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- int			- Size of the actual output.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
	CRYPT_RNG_CTX mcRng;
    int           ret;
    unsigned int  keySz = (unsigned int)sizeof(client_key_der_1024);
    byte          out1[256];

    ret = CRYPT_RSA_Initialize(&mcRsa);
	ret = CRYPT_RNG_Initialize(&mcRng);

    ret = CRYPT_RSA_PublicKeyDecode(&mcRsa, client_key_der_1024, keySz);

    ret = CRYPT_RSA_PublicEncrypt(&mcRsa, out1, sizeof(out1), ourData,
                                  RSA_TEST_SIZE, &mcRng);
    </code>

  Remarks:
*/

int CRYPT_RSA_PublicEncrypt(CRYPT_RSA_CTX*, unsigned char*,
                            unsigned int, const unsigned char*, unsigned int,
                            CRYPT_RNG_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_RSA_PrivateDecrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                             unsigned int outSz, const unsigned char* in, unsigned int inSz)

  Summary:
	Decrypts data using a private key.

  Description:
	This function decrypts a data block using a private key.

  Precondition:
	The context must be initialized using CRYPT_RSA_Initialized and the Private Key
	Decoded using CRYPT_RSA_PrivateKeyDecode prior to calling this function.
	
  Parameters:
    rsa		- Pointer to context which saves state between calls.
    out		- Pointer to output buffer to store results.
    outSz	- Size of output buffer.
    in		- Pointer to source buffer to be decrypted.
    inSz	- Size of input buffer.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- int			- Size of the actual output.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;
    unsigned int  keySz = (unsigned int)sizeof(client_key_der_1024);
    byte          out1[256];

    ret = CRYPT_RSA_Initialize(&mcRsa);

    ret = CRYPT_RSA_PrivateKeyDecode(&mcRsa, client_key_der_1024, keySz);

    ret = CRYPT_RSA_PrivateDecrypt(&mcRsa, out2, sizeof(out2), out1,
                                  RSA_TEST_SIZE);
    </code>

  Remarks:
*/

int CRYPT_RSA_PrivateDecrypt(CRYPT_RSA_CTX*, unsigned char*,
                             unsigned int, const unsigned char*, unsigned int);


//******************************************************************************
/* Function:
    int CRYPT_RSA_EncryptSizeGet(CRYPT_RSA_CTX* rsa)

  Summary:
	Gets the size of the RSA Key.

  Description:
	This function retrieves the size of the RSA Key in use for the context.

  Precondition:
	The context must be initialized with a call to CRYPT_RSA_Initialize and the keys
	decoded either with CRYPT_RSA_PrivateKeyDecode or CRYPT_RSA_PublicKeyDecode.
	
  Parameters:
    rsa 	- Pointer to context which saves state between calls.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- int			- Size of the key.

  Example:
    <code>
    CRYPT_RSA_CTX mcRsa;
    int           ret;
    unsigned int  keySz = (unsigned int)sizeof(client_key_der_1024);
    byte          out1[256];

    ret = CRYPT_RSA_Initialize(&mcRsa);

    ret = CRYPT_RSA_PrivateKeyDecode(&mcRsa, client_key_der_1024, keySz);
	
	ret = CRYPT_RSA_EncryptSizeGet(&mcRsa);
    </code>

  Remarks:
*/

int CRYPT_RSA_EncryptSizeGet(CRYPT_RSA_CTX*);                             



/* ECC */
typedef struct CRYPT_ECC_CTX {
    void* holder;
} CRYPT_ECC_CTX;


//******************************************************************************
/* Function:
    int CRYPT_ECC_Initialize(CRYPT_ECC_CTX* ecc)

  Summary:
	Initializes the context for Elliptic Curve Cryptography (ECC).

  Description:
	This function initializes the context used for Elliptic Curve Cryptography (ECC).
	The context is then passed to calls to perform key building, encryption,
	decryption, etc.

  Precondition:
	None.
	
  Parameters:
    ecc		- Pointer to context to initialize.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- -1			- Unable to allocate memory for the keys.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;

    ret = CRYPT_ECC_Initialize(&userA);
    </code>

  Remarks:
*/

int CRYPT_ECC_Initialize(CRYPT_ECC_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_ECC_Free(CRYPT_ECC_CTX* ecc)

  Summary:
	Cleans up an Elliptic Curve Cryptography (ECC) Context.

  Description:
	This function cleans up memory allocated for an ECC Context.

  Precondition:
	The context must have been initialized previously with a call to
	CRYPT_ECC_Initialize.
	
  Parameters:
    ecc		- Pointer to context to clean up.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;

    ret = CRYPT_ECC_Initialize(&userA);
	...
	ret = CRYPT_ECC_Free(&userA);
    </code>

  Remarks:
*/

int CRYPT_ECC_Free(CRYPT_ECC_CTX*);


//******************************************************************************
/* Function:
    int CRYPT_ECC_PublicExport(CRYPT_ECC_CTX* ecc, unsigned char* out, 
								unsigned int outSz, unsigned int* usedSz)

  Summary:
	Exports public ECC key in ANSI X9.63 format.

  Description:
	This function takes an ECC public key and exports it in ANSI X9.63 format.

  Precondition:
	The context must be initialized previously with a call to CRYPT_ECC_Initialize.
	The key must also have been constructed with a call to CRYPT_ECC_DHE_KeyMake.
	A random number generator must all have been initialized with a call to CRYPT_RNG_Initialize.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls.
    out		- Buffer in which to store the public key.
    outSz	- The available size of the buffer, in bytes.
    usedSz	- Return value indicating how many bytes were used.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- BUFFER_E	    - The output buffer was too small to store the key.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
	byte          sharedA[100];
	unsigned int  aSz = (unsigned int)sizeof(sharedA);
	unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	ret = CRYPT_ECC_DHE_KeyMake(&userA, &mcRng, 32);
	ret = CRYPT_ECC_PublicExport(&userA, sharedA, aSz, &usedA);
    </code>

  Remarks:
*/

int CRYPT_ECC_PublicExport(CRYPT_ECC_CTX*, unsigned char*, unsigned int,
                           unsigned int*);

//******************************************************************************
/* Function:
    int CRYPT_ECC_PublicImport(CRYPT_ECC_CTX* ecc, const unsigned char* in, 
	                           unsigned int inSz)

  Summary:
	Imports public key in ANSI X9.63 format.

  Description:
	This function imports a public key in ANSI X9.63 format into the ECC context.

  Precondition:
	The ECC context must have previously been initialized with a call to
	CRYPT_ECC_Initialize.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls.
    in		- Input buffer the holds the public key.
    inSz	- Size of the input buffer, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- MEMORY_E		- Memory could not be allocated for the key.
	- ASN_PARSE_E	- There was a parse error while going through the key.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userB; 
    int           ret;
	byte          sharedA[100];
	unsigned int  aSz = (unsigned int)sizeof(sharedA);
	unsigned int  usedA;

    ret = CRYPT_ECC_Initialize(&userB);
	...
	ret = CRYPT_ECC_PublicImport(&userB, sharedA, usedA);
    </code>

  Remarks:
*/

int CRYPT_ECC_PublicImport(CRYPT_ECC_CTX*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_ECC_PrivateImport(CRYPT_ECC_CTX* ecc, const unsigned char* priv, 
	                            unsigned int privSz, const unsigned char* pub, 
								unsigned int pubSz)

  Summary:
	Imports private key pair in X9.63 format.

  Description:
	This function imports a public/private key pair in X9.63 format.

  Precondition:
	The context must have been initialized with a call to CRYPT_ECC_Initialize.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls.
    priv	- Pointer to the private key.
    privSz	- Size of the private key, in bytes.
    pub 	- Pointer to the public key.
    pubSz	- Size of the public key, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX ecc;
	
	CRYPT_ECC_Initialize(&ecc);
	...
	CRYPT_ECC_PrivateImport(&ecc, priv_key, sizeof(priv_key), pub_key, sizeof(pub_key));
    </code>

  Remarks:
*/

int CRYPT_ECC_PrivateImport(CRYPT_ECC_CTX*, const unsigned char*, unsigned int,
                            const unsigned char*, unsigned int);


//******************************************************************************
/* Function:
    int CRYPT_ECC_DHE_KeyMake(CRYPT_ECC_CTX*, CRYPT_RNG_CTX*, int)

  Summary:
	Creates a new ECC key.

  Description:
	This function creates a new ECC key.

  Precondition:
	The context must have been initialized with a call to CRYPT_ECC_Initialize.
	The random number generator context must have been initialized with a call to
	CRYPT_RNG_Initialize.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls.
    rng		- Pointer to the context for the random number generator.
    keySz	- The size of the key desired.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- MEMORY_E		- Could not create the memory buffer for the key.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
	byte          sharedA[100];
	unsigned int  aSz = (unsigned int)sizeof(sharedA);
	unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	ret = CRYPT_ECC_DHE_KeyMake(&userA, &mcRng, 32);
    </code>

  Remarks:
*/
int CRYPT_ECC_DHE_KeyMake(CRYPT_ECC_CTX*, CRYPT_RNG_CTX*, int);

//******************************************************************************
/* Function:
    int CRYPT_ECC_DHE_SharedSecretMake(CRYPT_ECC_CTX* priv, CRYPT_ECC_CTX* pub,
                                   unsigned char* out, unsigned int outSz, unsigned int* usedSz)

  Summary:
	Creates an ECC shared secret between two keys.

  Description:
	This function takes two ECC contexts (one public, one private) and creates 
	a shared secret between the two. The secret conforms to EC-DH from ANSI X9.63.

  Precondition:
	Both contexts must have been initialized with a call to CRYPT_ECC_Initialize.
	Both contexts have had their respective keys imported or created.
	
  Parameters:
    priv		- Pointer to the private ECC context (with the private key).
    pub 		- Pointer to the public ECC context (with the public key).
    out 		- Destination of the shared secret.
    outSz		- The max size of the shared secret.
    usedSz		- Resulting size of the shared secret.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- MEMORY_E		- Could not create the memory buffer for the shared secret.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    CRYPT_ECC_CTX userB;
    int           ret;
    byte          sharedA[100];
    unsigned int  aSz   = (unsigned int)sizeof(sharedA);
    unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
    ret = CRYPT_ECC_Initialize(&userB);
	...
	// Make or import the appropriate keys
	...
    ret = CRYPT_ECC_DHE_SharedSecretMake(&userA, &userB, sharedA, aSz, &usedA);
    </code>

  Remarks:
*/

int CRYPT_ECC_DHE_SharedSecretMake(CRYPT_ECC_CTX*, CRYPT_ECC_CTX*,
                                   unsigned char*, unsigned int, unsigned int*);


//******************************************************************************
/* Function:
    int CRYPT_ECC_DSA_HashSign(CRYPT_ECC_CTX* ecc, CRYPT_RNG_CTX* rng, unsigned char* sig,
        unsigned int sigSz, unsigned int* usedSz, const unsigned char* in, unsigned int inSz)

  Summary:
	Signs a message digest.

  Description:
	This function takes a message digest and signs it using a private ECC key.

  Precondition:
	The ECC context must have been initialized with a call to CRYPT_ECC_Initialize.
	The RNG context must have been initialized with a call to CRYPT_RNG_Initialize.
	The private key used for the signature must have been imported or created prior to
	calling this function.
	
  Parameters:
    ecc		- Pointer to ECC context which saves state between calls and holds keys.
    rng		- Pointer to Random Number Generator context.
    sig		- Destination for the signature.
    sigSz	- The max size of the signature, in bytes.
    usedSz	- The resulting size of the signature, in bytes.
	in      - Pointer to the message digest to sign.
	inSz    - The length of the digest, in bytes.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
    byte          sig[100];
    unsigned int  sigSz = (unsigned int)sizeof(sig);
    unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	...
	// Import or generate private key
	...
    ret = CRYPT_ECC_DSA_HashSign(&userA, &mcRng, sig, sigSz, &usedA, ourData,
                                 CRYPT_SHA_DIGEST_SIZE);
    </code>

  Remarks:
*/

int CRYPT_ECC_DSA_HashSign(CRYPT_ECC_CTX*, CRYPT_RNG_CTX*, unsigned char*,
        unsigned int, unsigned int*, const unsigned char*, unsigned int);

//******************************************************************************
/* Function:
    int CRYPT_ECC_DSA_HashVerify(CRYPT_ECC_CTX* ecc, const unsigned char* sig,
                     unsigned int sigSz, unsigned char* hash, unsigned int hashSz, int* status)

  Summary:
	Verifies an ECC signature.

  Description:
	This function verifies that an ECC signature is valid.

  Precondition:
	The ECC context must have been initialized with a call to CRYPT_ECC_Initialize.
	The key used for the signature must have been imported or created prior to
	calling this function.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls.
    sig		- The signature to verify.
    sigSz	- The length of the signature (octets).
    hash	- The hash (message digest) that was signed.
    hashSz	- The length of the hash (octets).
	status	- Result of signature (1 == valid, 0 == invalid).

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- MEMORY_E		- Memory could not be allocated for the operation.
	- 0				- An invalid pointer was not passed to the function.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
    byte          sig[100];
    unsigned int  sigSz = (unsigned int)sizeof(sig);
    unsigned int  usedA = 0;
	int verifyStatus = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	...
	// Import or generate private key
	...
    ret = CRYPT_ECC_DSA_HashVerify(&userA, sig, sigSz, ourData,
                                   CRYPT_SHA_DIGEST_SIZE, &verifyStatus);
    </code>

  Remarks:
*/

int CRYPT_ECC_DSA_HashVerify(CRYPT_ECC_CTX*, const unsigned char*,
                     unsigned int, unsigned char*, unsigned int, int*);


//******************************************************************************
/* Function:
    int CRYPT_ECC_KeySizeGet(CRYPT_ECC_CTX* ecc)

  Summary:
	Returns the key size in octets.

  Description:
	This function returns the size of the ECC key, in octets.

  Precondition:
	The ECC context must have been initialized with a call to CRYPT_ECC_Initialize.
	The key must have been imported or created prior to	calling this function.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls and contains the key.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- int			- The size of the key, in octets.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
    byte          sig[100];
    unsigned int  sigSz = (unsigned int)sizeof(sig);
    unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	...
	// Import or generate private key
	...
    ret = CRYPT_ECC_KeySizeGet(&userA);
    </code>

  Remarks:
*/

int CRYPT_ECC_KeySizeGet(CRYPT_ECC_CTX*);

//******************************************************************************
/* Function:
    int CRYPT_ECC_SignatureSizeGet(CRYPT_ECC_CTX* ecc)

  Summary:
	Returns the signature size in octets.

  Description:
	This function returns the size of the signature in a given context, in octets.

  Precondition:
	The ECC context must have been initialized with a call to CRYPT_ECC_Initialize.
	The keys must have been imported or created prior to calling this function.
	
  Parameters:
    ecc		- Pointer to context which saves state between calls, and contains the signature.

  Returns:
	- BAD_FUNC_ARG 	- An invalid pointer was passed to the function.
	- int			- The size of the signature.

  Example:
    <code>
    CRYPT_ECC_CTX userA; 
    int           ret;
    byte          sig[100];
    unsigned int  sigSz = (unsigned int)sizeof(sig);
    unsigned int  usedA = 0;

    ret = CRYPT_ECC_Initialize(&userA);
	...
	// Import or generate private key
	...
    ret = CRYPT_ECC_SignatureSizeGet(&userA);
    </code>

  Remarks:
*/

int CRYPT_ECC_SignatureSizeGet(CRYPT_ECC_CTX*);



//******************************************************************************
/* Function:
    int CRYPT_ERROR_StringGet(int err, char* str)

  Summary:
	Reports the nature of an error.

  Description:
	This function takes an error and converts it into human-readable text.

  Precondition:
	None.
	
  Parameters:
    int		- Error code to convert.
    str		- Pointer to buffer to store the message. Must hold at least 80 characters.

  Returns:
	- BAD_FUNC_ARG 	- A null string was passed for the return message.
	- 0				- A null string was not passed for the return message.

  Example:
    <code>
	char msg[80];
	
	CRYPT_ERR_StringGet(ret, msg);
    </code>

  Remarks:
	String needs to be >= 80 chars.
*/

int CRYPT_ERROR_StringGet(int, char*);


#ifdef __cplusplus
    }  /* extern "C" */ 
#endif


#endif /* MC_CRYPTO_API_H */

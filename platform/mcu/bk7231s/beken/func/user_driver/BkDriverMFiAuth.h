/**
 ******************************************************************************
 * @file    BkDriverMFiAuth.h

 * @version V1.0.0
 * @date    7-Nov-2014
 * @brief   This file provides all the headers of Apple Authentication Coprocessor
 *          operations, Apple Authentication Coprocessor should be connected by
 *          BK_I2C_CP defined in platform.h, and based on I2C driver defined in
 *          BkDriverI2c.h
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#ifndef __BEKENDRIVERMFIAUTH_H__
#define __BEKENDRIVERMFIAUTH_H__

#pragma once
#include "include.h"

/** @addtogroup BK_PLATFORM
* @{
*/


/** @defgroup BK_MFIAUTH _BK_ MFiAuth
  * @brief  Provide APIs for Apple Authentication Coprocessor operations
  * @{
  */

/******************************************************
 *                   Macros
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/



/******************************************************
 *                 Function Declarations
 ******************************************************/

/** @brief     PlatformMFiAuthInitialize
 *
 *  @abstract Performs any platform-specific initialization needed.
 *            Example: Bring up I2C interface for communication with
 *            the Apple Authentication Coprocessor.
 *
 *  @param   i2c            :  bk_i2c_t context
 *
 *  @return    kNoErr        : on success.
 *  @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus BkMFiAuthInitialize( bk_i2c_t i2c );


/** @brief    PlatformMFiAuthFinalize
 *
 *  @abstract Performs any platform-specific cleanup needed.
 *            Example: Bringing down the I2C interface for communication with
 *            the Apple Authentication Coprocessor.
 *
 *  @param    i2c            :  bk_i2c_t context
 *
 *  @return   none
 */
void BkMFiAuthFinalize( void );



/** @brief    PlatformMFiAuthCreateSignature
 *
 *  @abstract Create an RSA signature from the specified SHA-1 digest
 *            using the Apple Authentication Coprocessor.
 *
 *  @param    inDigestPtr     :    Pointer to 20-byte SHA-1 digest.
 *  @param    inDigestLen     :    Number of bytes in the digest. Must be 20.
 *  @param    outSignaturePtr :    Receives malloc()'d ptr to RSA signature. Caller must free() on success.
 *  @param    outSignatureLen :    Receives number of bytes in RSA signature.
 *
 *  @return    kNoErr         :    on success.
 *  @return    kGeneralErr    :    if an error occurred with any step
 */
OSStatus BkMFiAuthCreateSignature( const  void      *inDigestPtr,
                                   size_t     inDigestLen,
                                   uint8_t  **outSignaturePtr,
                                   size_t    *outSignatureLen );



/** @brief    PlatformMFiAuthCopyCertificate
 *
 *  @abstract Copy the certificate from the Apple Authentication Coprocessor.
 *
 *  @param    outCertificatePtr:   Receives malloc()'d ptr to a DER-encoded PKCS#7 message containing the certificate.
                                    Caller must free() on success.
 *  @param    outCertificateLen:   Number of bytes in the DER-encoded certificate.
 *
 *  @return    kNoErr         :    on success.
 *  @return    kGeneralErr    :    if an error occurred with any step
 */
OSStatus BkMFiAuthCopyCertificate( uint8_t **outCertificatePtr, size_t *outCertificateLen );

/** @} */
/** @} */
#endif // __BEKENDRIVERMFIAUTH_H__



/**
 * \file
 * \brief function declarations required to work with DER encoded data related to X.509 certificates.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */


#ifndef ATCACERT_DER_H
#define ATCACERT_DER_H

#include <stddef.h>
#include <stdint.h>
#include "atcacert.h"

// Inform function naming when compiling in C++
#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup atcacert_ Certificate manipulation methods (atcacert_)
 *
 * \brief
 * These methods provide convenient ways to perform certification I/O with
 * CryptoAuth chips and perform certificate manipulation in memory
 *
   @{ */

/**
 * \brief Encode a length in DER format.
 *
 * X.690 (http://www.itu.int/rec/T-REC-X.690/en) section 8.1.3, for encoding
 *
 * \param[in]    length           Length to be encoded.
 * \param[out]   der_length       DER encoded length will returned in this buffer.
 * \param[inout] der_length_size  As input, size of der_length buffer in bytes.
 *                                As output, the size of the DER length encoding in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_enc_length(uint32_t length, uint8_t* der_length, size_t* der_length_size);

/**
 * \brief Decode a DER format length.
 *
 * X.690 (http://www.itu.int/rec/T-REC-X.690/en) section 8.1.3, for encoding
 *
 * \param[in]    der_length       DER encoded length.
 * \param[inout] der_length_size  As input, the size of the der_length buffer in bytes.
 *                                As output, the size of the DER encoded length that was decoded.
 * \param[out]   length           Decoded length is returned here.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_dec_length(const uint8_t* der_length, size_t* der_length_size, uint32_t* length);

int atcacert_der_adjust_length(uint8_t* der_length, size_t* der_length_size, int delta_length, uint32_t* new_length);

/**
 * \brief Encode an ASN.1 integer in DER format, including tag and length fields.
 *
 * X.680 (http://www.itu.int/rec/T-REC-X.680/en) section 19.8, for tag value
 * X.690 (http://www.itu.int/rec/T-REC-X.690/en) section 8.3, for encoding
 *
 * \param[in]    int_data       Raw integer in big-endian format.
 * \param[in]    int_data_size  Size of the raw integer in bytes.
 * \param[in]    is_unsigned    Indicate whether the input integer should be treated as unsigned.
 * \param[out]   der_int        DER encoded integer is returned in this buffer.
 * \param[inout] der_int_size   As input, the size of the der_int buffer in bytes.
 *                              As output, the size of the DER integer returned in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_enc_integer(const uint8_t* int_data,
                             size_t         int_data_size,
                             uint8_t        is_unsigned,
                             uint8_t*       der_int,
                             size_t*        der_int_size);

/**
 * \brief Decode an ASN.1 DER encoded integer.
 *
 * X.680 (http://www.itu.int/rec/T-REC-X.680/en) section 19.8, for tag value
 * X.690 (http://www.itu.int/rec/T-REC-X.690/en) section 8.3, for encoding
 *
 * \param[in]     der_int        DER encoded ASN.1 integer, including the tag and length fields.
 * \param[inout]  der_int_size   As input, the size of the der_int buffer in bytes.
 *                               As output, the size of the DER integer decoded in bytes.
 * \param[out]    int_data       Decode integer is returned in this buffer in a signed big-endian
 *                               format.
 * \param[inout]  int_data_size  As input, the size of int_data in bytes.
 *                               As output, the size of the decoded integer in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_dec_integer(const uint8_t* der_int,
                             size_t*        der_int_size,
                             uint8_t*       int_data,
                             size_t*        int_data_size);

/**
 * \brief Formats a raw ECDSA P256 signature in the DER encoding found in X.509 certificates.
 *
 * This will return the DER encoding of the signatureValue field as found in an X.509 certificate
 * (RFC 5280). This include the tag, length, and value.  The value of the signatureValue is the DER
 * encoding of the ECDSA-Sig-Value as specified by RFC 5480 and SECG SEC1.
 *
 * \param[in]  raw_sig          P256 ECDSA signature to be formatted. Input format is R and S
 *                              integers concatenated together. 64 bytes.
 * \param[out] der_sig          X.509 format signature (TLV of signatureValue) will be returned in
 *                              this buffer.
 * \param[inout] der_sig_size   As input, the size of the x509_sig buffer in bytes.
 *                              As output, the size of the returned X.509 signature in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_enc_ecdsa_sig_value(const uint8_t raw_sig[64],
                                     uint8_t*      der_sig,
                                     size_t*       der_sig_size);

/**
 * \brief Parses an ECDSA P256 signature in the DER encoding as found in X.509 certificates.
 *
 * This will parse the DER encoding of the signatureValue field as found in an X.509 certificate
 * (RFC 5280). x509_sig should include the tag, length, and value.  The value of the signatureValue
 * is the DER encoding of the ECDSA-Sig-Value as specified by RFC 5480 and SECG SEC1.
 *
 * \param[in]    der_sig        X.509 format signature (TLV of signatureValue) to be parsed.
 * \param[inout] der_sig_size   As input, size of the der_sig buffer in bytes.
 *                              As output, size of the DER x.509 signature parsed from the buffer.
 * \param[out]   raw_sig        Parsed P256 ECDSA signature will be returned in this buffer.
 *                              Formatted as R and S integers concatenated together. 64 bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_der_dec_ecdsa_sig_value(const uint8_t * der_sig,
                                     size_t *        der_sig_size,
                                     uint8_t         raw_sig[64]);

/** @} */
#ifdef __cplusplus
}
#endif

#endif
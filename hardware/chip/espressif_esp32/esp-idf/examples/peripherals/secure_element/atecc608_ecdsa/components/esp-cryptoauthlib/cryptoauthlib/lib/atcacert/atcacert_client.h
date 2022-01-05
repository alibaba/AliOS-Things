/**
 * \file
 * \brief Client side cert i/o methods. These declarations deal with the client-side, the node being authenticated,
 *        of the authentication process. It is assumed the client has an ECC CryptoAuthentication device
 *        (e.g. ATECC508A) and the certificates are stored on that device.
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


#ifndef ATCACERT_CLIENT_H
#define ATCACERT_CLIENT_H

#include <stddef.h>
#include <stdint.h>
#include "atcacert_def.h"

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

/** \brief Read the data from a device location.
 *
 * \param[in]  device_loc  Device location to read data from.
 * \param[out] data        Data read is returned here.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_read_device_loc(const atcacert_device_loc_t* device_loc,
                             uint8_t*                     data);

/**
 * \brief Reads the certificate specified by the certificate definition from the
 *        ATECC508A device.
 *
 * This process involves reading the dynamic cert data from the device and combining it
 * with the template found in the certificate definition.
 *
 * \param[in]    cert_def       Certificate definition describing where to find the dynamic
 *                              certificate information on the device and how to incorporate it
 *                              into the template.
 * \param[in]    ca_public_key  The ECC P256 public key of the certificate authority that signed
 *                              this certificate. Formatted as the 32 byte X and Y integers
 *                              concatenated together (64 bytes total). Set to NULL if the
 *                              authority key id is not needed, set properly in the cert_def
 *                              template, or stored on the device as specifed in the
 *                              cert_def cert_elements.
 * \param[out]   cert           Buffer to received the certificate.
 * \param[inout] cert_size      As input, the size of the cert buffer in bytes.
 *                              As output, the size of the certificate returned in cert in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_read_cert(const atcacert_def_t* cert_def,
                       const uint8_t         ca_public_key[64],
                       uint8_t*              cert,
                       size_t*               cert_size);

/**
 * \brief Take a full certificate and write it to the ATECC508A device according to the
 *        certificate definition.
 *
 * \param[in] cert_def   Certificate definition describing where the dynamic certificate
 *                       information is and how to store it on the device.
 * \param[in] cert       Full certificate to be stored.
 * \param[in] cert_size  Size of the full certificate in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int atcacert_write_cert(const atcacert_def_t* cert_def,
                        const uint8_t*        cert,
                        size_t                cert_size);

/**
 * \brief Creates a CSR specified by the CSR definition from the ATECC508A device.
 *        This process involves reading the dynamic CSR data from the device and combining it
 *        with the template found in the CSR definition, then signing it. Return the CSR int der format
 * \param[in]    csr_def   CSR definition describing where to find the dynamic CSR information
 *                         on the device and how to incorporate it into the template.
 * \param[out]   csr       Buffer to receive the CSR.
 * \param[inout] csr_size  As input, the size of the CSR buffer in bytes.
 *                         As output, the size of the CSR returned in cert in bytes.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_create_csr(const atcacert_def_t* csr_def, uint8_t* csr, size_t* csr_size);

/**
 * \brief Creates a CSR specified by the CSR definition from the ATECC508A device.
 *        This process involves reading the dynamic CSR data from the device and combining it
 *        with the template found in the CSR definition, then signing it. Return the CSR int der format
 * \param[in]    csr_def   CSR definition describing where to find the dynamic CSR information
 *                         on the device and how to incorporate it into the template.
 * \param[out]   csr       Buffer to received the CSR formatted as PEM.
 * \param[inout] csr_size  As input, the size of the CSR buffer in bytes.
 *                         As output, the size of the CSR as PEM returned in cert in bytes.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_create_csr_pem(const atcacert_def_t* csr_def, char* csr, size_t* csr_size);

/**
 * \brief Calculates the response to a challenge sent from the host.
 *
 * The challenge-response protocol is an ECDSA Sign and Verify. This performs the ECDSA Sign on the
 * challenge and returns the signature as the response.
 *
 * \param[in]  device_private_key_slot  Slot number for the device's private key. This must be the
 *                                      same slot used to generate the public key included in the
 *                                      device's certificate.
 * \param[in]  challenge                Challenge to generate the response for. Must be 32 bytes.
 * \param[out] response                 Response will be returned in this buffer. 64 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_get_response(uint8_t       device_private_key_slot,
                          const uint8_t challenge[32],
                          uint8_t       response[64]);

/** @} */
#ifdef __cplusplus
}
#endif

#endif
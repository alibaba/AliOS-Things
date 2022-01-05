/**
 * \file
 * \brief Client side certificate I/O functions for TNG devices.
 *
 * \copyright (c) 2015-2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef TNG_ATCACERT_CLIENT_H
#define TNG_ATCACERT_CLIENT_H

#include <stdint.h>
#include "atcacert/atcacert.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \ingroup tng_
 * @{
 */

/** \brief Return the maximum possible certificate size in bytes for a TNG
 *         device certificate. Certificate can be variable size, so this
 *         gives an appropriate buffer size when reading the certificate.
 *
 * \param[out] max_cert_size  Maximum certificate size will be returned here
 *                            in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_max_device_cert_size(size_t* max_cert_size);

/**
 * \brief Reads the device certificate for a TNG device.
 *
 * \param[out]   cert         Buffer to received the certificate (DER format).
 * \param[inout] cert_size    As input, the size of the cert buffer in bytes.
 *                            As output, the size of the certificate returned
 *                            in cert in bytes.
 * \param[in]    signer_cert  If supplied, the signer public key is used from
 *                            this certificate. If set to NULL, the signer
 *                            public key is read from the device.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_read_device_cert(uint8_t* cert, size_t* cert_size, const uint8_t* signer_cert);

/**
 * \brief Reads the device public key.
 *
 * \param[out] public_key   Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve.
 * \param[in]  cert         If supplied, the device public key is used from
 *                          this certificate. If set to NULL, the device
 *                          public key is read from the device.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_device_public_key(uint8_t* public_key, uint8_t* cert);

/** \brief Return the maximum possible certificate size in bytes for a TNG
 *         signer certificate. Certificate can be variable size, so this
 *         gives an appropriate buffer size when reading the certificate.
 *
 * \param[out] max_cert_size  Maximum certificate size will be returned here
 *                            in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_max_signer_cert_size(size_t* max_cert_size);

/**
 * \brief Reads the signer certificate for a TNG device.
 *
 * \param[out]   cert       Buffer to received the certificate (DER format).
 * \param[inout] cert_size  As input, the size of the cert buffer in bytes.
 *                          As output, the size of the certificate returned
 *                          in cert in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_read_signer_cert(uint8_t* cert, size_t* cert_size);

/**
 * \brief Reads the signer public key.
 *
 * \param[out] public_key   Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve.
 * \param[in]  cert         If supplied, the signer public key is used from
 *                          this certificate. If set to NULL, the signer
 *                          public key is read from the device.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_signer_public_key(uint8_t* public_key, uint8_t* cert);

/** \brief Get the size of the TNG root cert.
 *
 * \param[out] cert_size  Certificate size will be returned here in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_root_cert_size(size_t* cert_size);

/** \brief Get the TNG root cert.
 *
 * \param[out]   cert       Buffer to received the certificate (DER format).
 * \param[inout] cert_size  As input, the size of the cert buffer in bytes.
 *                          As output, the size of the certificate returned
 *                          in cert in bytes.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_root_cert(uint8_t* cert, size_t* cert_size);

/**
 * \brief Gets the root public key.
 *
 * \param[out] public_key  Public key will be returned here. Format will be
 *                         the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve.
 *
 * \return ATCACERT_E_SUCCESS on success, otherwise an error code.
 */
int tng_atcacert_root_public_key(uint8_t* public_key);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/**
 * \file
 * \brief Functions for converting between DER and PEM formats.
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

#ifndef ATCACERT_PEM_H
#define ATCACERT_PEM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEM_CERT_BEGIN "-----BEGIN CERTIFICATE-----"
#define PEM_CERT_END   "-----END CERTIFICATE-----"
#define PEM_CSR_BEGIN  "-----BEGIN CERTIFICATE REQUEST-----"
#define PEM_CSR_END    "-----END CERTIFICATE REQUEST-----"

/**
 * \brief Encode a DER data in PEM format.
 * \param[in]    der       DER data to be encoded as PEM.
 * \param[out]   der_size  DER data size in bytes.
 * \param[out]   pem       PEM encoded data is returned here.
 * \param[inout] pem_size  As input, the size of the pem buffer.
 *                         As output, the size of the PEM data.
 * \param[in]    header    Header to place at the beginning of the PEM data.
 * \param[in]    footer    Footer to place at the end of the PEM data.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_encode_pem(const uint8_t* der,
                        size_t         der_size,
                        char*          pem,
                        size_t*        pem_size,
                        const char*    header,
                        const char*    footer);

/**
 * \brief Decode PEM data into DER format.
 * \param[in]    pem       PEM data to decode to DER.
 * \param[in]    pem_size  PEM data size in bytes.
 * \param[out]   der       DER data is returned here.
 * \param[inout] der_size  As input, the size of the der buffer.
 *                         As output, the size of the DER data.
 * \param[in]    header    Header to find the beginning of the PEM data.
 * \param[in]    footer    Footer to find the end of the PEM data.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_decode_pem(const char* pem,
                        size_t      pem_size,
                        uint8_t*    der,
                        size_t*     der_size,
                        const char* header,
                        const char* footer);

/**
 * \brief Encode a DER certificate in PEM format.
 * \param[in]    der_cert       DER certificate to be encoded as PEM.
 * \param[out]   der_cert_size  DER certificate size in bytes.
 * \param[out]   pem_cert       PEM encoded certificate is returned here.
 * \param[inout] pem_cert_size  As input, the size of the pem_cert buffer.
 *                              As output, the size of the PEM certificate.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_encode_pem_cert(const uint8_t* der_cert, size_t der_cert_size, char* pem_cert, size_t* pem_cert_size);

/**
 * \brief Decode a PEM certificate into DER format.
 * \param[in]    pem_cert       PEM certificate to decode to DER.
 * \param[in]    pem_cert_size  PEM certificate size in bytes.
 * \param[out]   der_cert       DER certificate is returned here.
 * \param[inout] der_cert_size  As input, the size of the der_cert buffer.
 *                              As output, the size of the DER certificate.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_decode_pem_cert(const char* pem_cert, size_t pem_cert_size, uint8_t* der_cert, size_t* der_cert_size);

/**
 * \brief Encode a DER CSR in PEM format.
 * \param[in]    der_csr       DER CSR to be encoded as PEM.
 * \param[out]   der_csr_size  DER CSR size in bytes.
 * \param[out]   pem_csr       PEM encoded CSR is returned here.
 * \param[inout] pem_csr_size  As input, the size of the pem_csr buffer.
 *                             As output, the size of the PEM CSR.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_encode_pem_csr(const uint8_t* der_csr, size_t der_csr_size, char* pem_csr, size_t* pem_csr_size);

/**
 * \brief Extract the CSR certificate bytes from a PEM encoded CSR certificate
 * \param[in]    pem_csr       PEM CSR to decode to DER.
 * \param[in]    pem_csr_size  PEM CSR size in bytes.
 * \param[out]   der_csr       DER CSR is returned here.
 * \param[inout] der_csr_size  As input, the size of the der_csr buffer.
 *                             As output, the size of the DER CSR.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcacert_decode_pem_csr(const char* pem_csr, size_t pem_csr_size, uint8_t* der_csr, size_t* der_csr_size);

#ifdef __cplusplus
}
#endif

#endif
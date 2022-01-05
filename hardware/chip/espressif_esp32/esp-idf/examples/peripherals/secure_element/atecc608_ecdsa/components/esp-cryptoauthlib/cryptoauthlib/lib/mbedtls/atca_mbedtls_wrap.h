/**
 * \brief mbedTLS Interface Functions that enable mbedtls objects to use
 * cryptoauthlib functions
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

#ifndef _ATCA_MBEDTLS_WRAP_H_
#define _ATCA_MBEDTLS_WRAP_H_

/** \defgroup atca_mbedtls_ mbedTLS Wrapper methods (atca_mbedtls_)
 *
 * \brief
 * These methods are for interfacing cryptoauthlib to mbedtls
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

struct mbedtls_pk_context;
struct mbedtls_x509_crt;
struct atcacert_def_s;

/* Wrapper Functions */
int atca_mbedtls_pk_init(struct mbedtls_pk_context * pkey, const uint16_t slotid);
int atca_mbedtls_cert_add(struct mbedtls_x509_crt * cert, const struct atcacert_def_s * cert_def);

/* Application Callback definitions */

/** \brief ECDH Callback to obtain the "slot" used in ECDH operations from the
 * application
 * \return Slot Number
 */
int atca_mbedtls_ecdh_slot_cb(void);

/** \brief ECDH Callback to obtain the IO Protection secret from the application
 * \param[out] secret 32 byte array used to store the secret
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atca_mbedtls_ecdh_ioprot_cb(uint8_t secret[32]);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _ATCA_MBEDTLS_WRAP_H_ */

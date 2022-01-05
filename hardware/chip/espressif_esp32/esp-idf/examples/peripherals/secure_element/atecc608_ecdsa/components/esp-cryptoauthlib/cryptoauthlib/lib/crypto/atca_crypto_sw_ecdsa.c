/**
 * \file
 * \brief API wrapper for software ECDSA verify.  Currently unimplemented but could be
 *        implemented via a 3rd party library such as MicroECC.
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


#include "atca_crypto_sw_ecdsa.h"

/** \brief return software generated ECDSA verification result and the function is currently not implemented
 * \param[in] msg         ptr to message or challenge
 * \param[in] signature   ptr to the signature to verify
 * \param[in] public_key  ptr to public key of device which signed the challenge
 * return ATCA_UNIMPLEMENTED , as the function is currently not implemented
 */

int atcac_sw_ecdsa_verify_p256(const uint8_t msg[ATCA_ECC_P256_FIELD_SIZE],
                               const uint8_t signature[ATCA_ECC_P256_SIGNATURE_SIZE],
                               const uint8_t public_key[ATCA_ECC_P256_PUBLIC_KEY_SIZE])
{
    return ATCA_UNIMPLEMENTED;
}
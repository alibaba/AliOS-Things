/**
 * \file
 * \brief Utilities to create and verify a JSON Web Token (JWT)
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

#ifndef ATCA_JWT_H_
#define ATCA_JWT_H_

/** \defgroup jwt JSON Web Token (JWT) methods (atca_jwt_)
 * \brief Methods for signing and verifying JSON Web Token (JWT) tokens.
   @{ */

#include "cryptoauthlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure to hold metadata information about the jwt being built */
typedef struct
{
    char*    buf;          /* Input buffer */
    uint16_t buflen;       /* Total buffer size */
    uint16_t cur;          /* Current location in the buffer */
} atca_jwt_t;

ATCA_STATUS atca_jwt_init(atca_jwt_t* jwt, char* buf, uint16_t buflen);
ATCA_STATUS atca_jwt_add_claim_string(atca_jwt_t* jwt, const char* claim, const char* value);
ATCA_STATUS atca_jwt_add_claim_numeric(atca_jwt_t* jwt, const char* claim, int32_t value);
ATCA_STATUS atca_jwt_finalize(atca_jwt_t* jwt, uint16_t key_id);
void atca_jwt_check_payload_start(atca_jwt_t* jwt);
ATCA_STATUS atca_jwt_verify(const char* buf, uint16_t buflen, const uint8_t* pubkey);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* ATCA_JWT_H_ */

/**
 * \file
 * \brief Wrapper API for SHA 1 routines
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

#ifndef ATCA_CRYPTO_SW_SHA1_H
#define ATCA_CRYPTO_SW_SHA1_H

#include "atca_crypto_sw.h"
#include <stddef.h>
#include <stdint.h>

/** \defgroup atcac_ Software crypto methods (atcac_)
 *
 * \brief
 * These methods provide a software implementation of various crypto
 * algorithms
 *
   @{ */

#define ATCA_SHA1_DIGEST_SIZE (20)

typedef struct
{
    uint32_t pad[32]; //!< Filler value to make sure the actual implementation has enough room to store its context. uint32_t is used to remove some alignment warnings.
} atcac_sha1_ctx;

#ifdef __cplusplus
extern "C" {
#endif

int atcac_sw_sha1_init(atcac_sha1_ctx* ctx);
int atcac_sw_sha1_update(atcac_sha1_ctx* ctx, const uint8_t* data, size_t data_size);
int atcac_sw_sha1_finish(atcac_sha1_ctx * ctx, uint8_t digest[ATCA_SHA1_DIGEST_SIZE]);
int atcac_sw_sha1(const uint8_t * data, size_t data_size, uint8_t digest[ATCA_SHA1_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
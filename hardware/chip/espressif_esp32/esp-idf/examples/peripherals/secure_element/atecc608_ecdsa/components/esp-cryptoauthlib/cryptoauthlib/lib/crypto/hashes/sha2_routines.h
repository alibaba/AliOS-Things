/**
 * \file
 * \brief Software implementation of the SHA256 algorithm.
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

#ifndef SHA2_ROUTINES_H
#define SHA2_ROUTINES_H

#include <stdint.h>

#define SHA256_DIGEST_SIZE (32)
#define SHA256_BLOCK_SIZE  (64)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t total_msg_size;                //!< Total number of message bytes processed
    uint32_t block_size;                    //!< Number of bytes in current block
    uint8_t  block[SHA256_BLOCK_SIZE * 2];  //!< Unprocessed message storage
    uint32_t hash[8];                       //!< Hash state
} sw_sha256_ctx;

void sw_sha256_init(sw_sha256_ctx* ctx);

void sw_sha256_update(sw_sha256_ctx* ctx, const uint8_t* message, uint32_t len);

void sw_sha256_final(sw_sha256_ctx * ctx, uint8_t digest[SHA256_DIGEST_SIZE]);

void sw_sha256(const uint8_t * message, unsigned int len, uint8_t digest[SHA256_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif

#endif // SHA2_ROUTINES_H


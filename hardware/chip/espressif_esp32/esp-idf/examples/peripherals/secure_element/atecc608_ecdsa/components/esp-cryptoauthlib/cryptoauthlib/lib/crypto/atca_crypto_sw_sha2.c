/**
 * \file
 * \brief Wrapper API for software SHA 256 routines
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

#include "atca_crypto_sw_sha2.h"
#include "hashes/sha2_routines.h"

/** \brief initializes the SHA256 software
 * \param[in] ctx  ptr to context data structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

int atcac_sw_sha2_256_init(atcac_sha2_256_ctx* ctx)
{
    if (sizeof(sw_sha256_ctx) > sizeof(atcac_sha2_256_ctx))
    {
        return ATCA_ASSERT_FAILURE;  // atcac_sha1_ctx isn't large enough for this implementation
    }
    sw_sha256_init((sw_sha256_ctx*)ctx);

    return ATCA_SUCCESS;
}

/** \brief updates the running hash with the next block of data, called iteratively for the entire
    stream of data to be hashed using the SHA256 software
    \param[in] ctx        ptr to SHA context data structure
    \param[in] data       ptr to next block of data to hash
    \param[in] data_size  size amount of data to hash in the given block, in bytes
    \return ATCA_SUCCESS
 */

int atcac_sw_sha2_256_update(atcac_sha2_256_ctx* ctx, const uint8_t* data, size_t data_size)
{
    sw_sha256_update((sw_sha256_ctx*)ctx, data, (uint32_t)data_size);

    return ATCA_SUCCESS;
}

/** \brief completes the final SHA256 calculation and returns the final digest/hash
 * \param[in]  ctx     ptr to context data structure
 * \param[out] digest  receives the computed digest of the SHA 256
 * \return ATCA_SUCCESS
 */

int atcac_sw_sha2_256_finish(atcac_sha2_256_ctx* ctx, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE])
{
    sw_sha256_final((sw_sha256_ctx*)ctx, digest);

    return ATCA_SUCCESS;
}


/** \brief single call convenience function which computes Hash of given data using SHA256 software
 * \param[in]  data       pointer to stream of data to hash
 * \param[in]  data_size  size of data stream to hash
 * \param[out] digest     result
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

int atcac_sw_sha2_256(const uint8_t* data, size_t data_size, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE])
{
    int ret;
    atcac_sha2_256_ctx ctx;

    ret = atcac_sw_sha2_256_init(&ctx);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    ret = atcac_sw_sha2_256_update(&ctx, data, data_size);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    ret = atcac_sw_sha2_256_finish(&ctx, digest);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    return ATCA_SUCCESS;
}
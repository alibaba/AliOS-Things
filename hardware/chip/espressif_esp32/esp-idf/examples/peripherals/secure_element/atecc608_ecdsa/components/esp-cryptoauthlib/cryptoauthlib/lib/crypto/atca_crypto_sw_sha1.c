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


#include "atca_crypto_sw_sha1.h"
#include "hashes/sha1_routines.h"


/** \brief Initialize context for performing SHA1 hash in software.
 * \param[in] ctx  Hash context
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

int atcac_sw_sha1_init(atcac_sha1_ctx* ctx)
{
    if (sizeof(CL_HashContext) > sizeof(atcac_sha1_ctx))
    {
        return ATCA_ASSERT_FAILURE;  // atcac_sha1_ctx isn't large enough for this implementation
    }
    CL_hashInit((CL_HashContext*)ctx);

    return ATCA_SUCCESS;
}


/** \brief Add arbitrary data to a SHA1 hash.
    \param[in] ctx        Hash context
    \param[in] data       Data to be added to the hash
    \param[in] data_size  Data size in bytes
    \return ATCA_SUCCESS
 */
int atcac_sw_sha1_update(atcac_sha1_ctx* ctx, const uint8_t* data, size_t data_size)
{
    CL_hashUpdate((CL_HashContext*)ctx, data, (int)data_size);

    return ATCA_SUCCESS;
}

/** \brief Complete the SHA1 hash in software and return the digest.
 * \param[in]  ctx     Hash context
 * \param[out] digest  Digest is returned here (20 bytes)
 * \return ATCA_SUCCESS
 */
int atcac_sw_sha1_finish(atcac_sha1_ctx* ctx, uint8_t digest[ATCA_SHA1_DIGEST_SIZE])
{
    CL_hashFinal((CL_HashContext*)ctx, digest);

    return ATCA_SUCCESS;
}


/** \brief Perform SHA1 hash of data in software.
 * \param[in]  data       Data to be hashed
 * \param[in]  data_size  Data size in bytes
 * \param[out] digest     Digest is returned here (20 bytes)
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha1(const uint8_t* data, size_t data_size, uint8_t digest[ATCA_SHA1_DIGEST_SIZE])
{
    int ret;
    atcac_sha1_ctx ctx;

    ret = atcac_sw_sha1_init(&ctx);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    ret = atcac_sw_sha1_update(&ctx, data, data_size);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    ret = atcac_sw_sha1_finish(&ctx, digest);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    return ATCA_SUCCESS;
}
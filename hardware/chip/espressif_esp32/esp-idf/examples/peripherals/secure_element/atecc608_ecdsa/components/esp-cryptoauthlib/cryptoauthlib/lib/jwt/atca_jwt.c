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

#include "cryptoauthlib.h"
#include "basic/atca_helpers.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "jwt/atca_jwt.h"
#include <stdio.h>

/** \brief The only supported JWT format for this library */
static const char g_jwt_header[] = "{\"alg\":\"ES256\",\"typ\":\"JWT\"}";

/**
 * \brief Check the provided context to see what character needs to be added in
 * order to append a claim
 */
void atca_jwt_check_payload_start(
    atca_jwt_t* jwt     /**< [in] JWT Context to use */
    )
{
    /* Rationality checks: a) must be valid, b) buf must be valid, c) must not be at the start, d) must have room */
    if (jwt && jwt->buf && jwt->cur && (jwt->cur < jwt->buflen - 1))
    {
        /* Check the previous */
        char c = jwt->buf[jwt->cur - 1];
        if ('.' == c)
        {
            jwt->buf[jwt->cur++] = '{';
        }
        else if ('{' != c)
        {
            jwt->buf[jwt->cur++] = ',';
        }
    }
}

/**
 * \brief Initialize a JWT structure
 */
ATCA_STATUS atca_jwt_init(
    atca_jwt_t* jwt,    /**< [in] JWT Context to initialize */
    char*       buf,    /**< [inout] Pointer to a buffer to store the token */
    uint16_t    buflen  /**< [in] Length of the buffer */
    )
{
    ATCA_STATUS ret = ATCA_BAD_PARAM;
    size_t tSize;

    if (jwt && buf && buflen)
    {
        jwt->buf = buf;
        jwt->buflen = buflen;
        jwt->cur = 0;

        /* Encode the header into the buffer */
        tSize = jwt->buflen;
        ret = atcab_base64encode_((const uint8_t*)g_jwt_header, strlen(g_jwt_header), jwt->buf,
                                  &tSize, atcab_b64rules_urlsafe);
        if (ATCA_SUCCESS == ret)
        {
            jwt->cur += (uint16_t)tSize;

            /* Check length */
            if (jwt->cur < jwt->buflen - 1)
            {
                /* Add the separator */
                jwt->buf[jwt->cur++] = '.';
            }
            else
            {
                ret = ATCA_INVALID_SIZE;
            }
        }
    }
    return ret;
}

/**
 * \brief Close the claims of a token, encode them, then sign the result
 */
ATCA_STATUS atca_jwt_finalize(
    atca_jwt_t* jwt,    /**< [in] JWT Context to use */
    uint16_t    key_id  /**< [in] Key Id (Slot number) used to sign */
    )
{
    ATCA_STATUS status;
    uint16_t i;
    size_t rem;
    size_t tSize;

    if (!jwt || !jwt->buf || !jwt->buflen || !jwt->cur)
    {
        return ATCA_BAD_PARAM;
    }

    /* Verify the payload is closed */
    if ('}' != jwt->buf[jwt->cur - 1])
    {
        jwt->buf[jwt->cur++] = '}';
    }

    /* Find the start of the "claims" portion of the token - header should
       already be encoded */
    for (i = 0; i < jwt->cur; i++)
    {
        if ('.' == jwt->buf[i])
        {
            i++;
            break;
        }
    }

    /* Make sure there is enough remaining buffer given base64 4/3 expansion */
    rem = (jwt->cur - i + ATCA_SIG_SIZE) * 4;
    rem /= 3;

    /* Increase Count to accomodate: 1 for the '.', 1 for the null terminator,
        and 1 for padding */
    rem += 3;

    if (rem > (size_t)(jwt->buflen - jwt->cur))
    {
        return ATCA_INVALID_SIZE;
    }

    /* Calculate the payload length */
    rem = jwt->cur - i;
    /* Move the payload to make room for the encoding */
    memmove(jwt->buf + jwt->buflen - jwt->cur, &jwt->buf[i], rem);

    /* Encode the payload into the buffer */
    tSize = jwt->buflen;
    status = atcab_base64encode_((uint8_t*)(jwt->buf + jwt->buflen - jwt->cur), rem,
                                 &jwt->buf[i], &tSize, atcab_b64rules_urlsafe);
    if (ATCA_SUCCESS != status)
    {
        return status;
    }

    jwt->cur = (uint16_t)(i + tSize);

    /* Make sure there room to add the signature
        ECDSA(P256) -> 64 bytes -> base64 -> 86.3 (87) -> 88 including null */
    if (jwt->cur >= jwt->buflen - 88)
    {
        /* Something broke */
        return ATCA_INVALID_SIZE;
    }

    /* Create digest of the message store and store in the buffer */
    status = atcac_sw_sha2_256((const uint8_t*)jwt->buf, jwt->cur, (uint8_t*)(jwt->buf + jwt->buflen - 32));
    if (ATCA_SUCCESS != status)
    {
        return status;
    }

    /* Create ECSDA signature of the digest and store it back in the buffer */
    status = atcab_sign(key_id, (const uint8_t*)(jwt->buf + jwt->buflen - ATCA_SHA_DIGEST_SIZE),
                        (uint8_t*)(jwt->buf + jwt->buflen - 64));
    if (ATCA_SUCCESS != status)
    {
        return status;
    }

    /* Add the separator */
    jwt->buf[jwt->cur++] = '.';

    /* Encode the signature and store it in the buffer */
    tSize = jwt->buflen - jwt->cur;
    atcab_base64encode_((const uint8_t*)(jwt->buf + jwt->buflen - ATCA_SIG_SIZE), ATCA_SIG_SIZE,
                        &jwt->buf[jwt->cur], &tSize, atcab_b64rules_urlsafe);
    jwt->cur += (uint16_t)tSize;

    if (jwt->cur >= jwt->buflen)
    {
        /* Something broke */
        return ATCA_INVALID_SIZE;
    }

    /* Make sure resulting buffer is null terminated */
    jwt->buf[jwt->cur] = 0;

    return status;
}

/**
 * \brief Add a string claim to a token
 * \note This function does not escape strings so the user has to ensure they
 *       are valid for use in a JSON string first
 */
ATCA_STATUS atca_jwt_add_claim_string(
    atca_jwt_t* jwt,    /**< [in] JWT Context to use */
    const char* claim,  /**< [in] Name of the claim to be inserted */
    const char* value   /**< [in] Null terminated string to be insterted */
    )
{
    int32_t written;
    int32_t remaining;

    if (jwt && jwt->buf && jwt->buflen && claim && value)
    {
        atca_jwt_check_payload_start(jwt);

        remaining = jwt->buflen - jwt->cur;
        written = snprintf(&jwt->buf[jwt->cur], remaining, "\"%s\":\"%s\"", claim, value);
        if (0 < written && written < remaining)
        {
            jwt->cur += written;
            return ATCA_SUCCESS;
        }
        else
        {
            return ATCA_GEN_FAIL;
        }
    }
    else
    {
        return ATCA_BAD_PARAM;
    }
}

/**
 * \brief Add a numeric claim to a token
 * \note This function does not escape strings so the user has to ensure the
 *       claim is valid first
 */
ATCA_STATUS atca_jwt_add_claim_numeric(
    atca_jwt_t* jwt,    /**< [in] JWT Context to use */
    const char* claim,  /**< [in] Name of the claim to be inserted */
    int32_t     value   /**< [in] integer value to be inserted */
    )
{
    int32_t written;
    int32_t remaining;

    if (jwt && jwt->buf && jwt->buflen && claim)
    {
        atca_jwt_check_payload_start(jwt);

        remaining = jwt->buflen - jwt->cur;
        written = snprintf(&jwt->buf[jwt->cur], remaining, "\"%s\":%ld", claim, (long)value);
        if (0 < written && written < remaining)
        {
            jwt->cur += written;
            return 0;
        }
        else
        {
            return ATCA_GEN_FAIL;
        }
    }
    else
    {
        return ATCA_BAD_PARAM;
    }
}

/**
 * \brief Verifies the signature of a jwt using the provided public key
 */
ATCA_STATUS atca_jwt_verify(
    const char*    buf,      /**< [in] Buffer holding an encoded jwt */
    uint16_t       buflen,   /**< [in] Length of the buffer/jwt */
    const uint8_t* pubkey    /**< [in] Public key (raw byte format) */
    )
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t digest[ATCA_KEY_SIZE];
    uint8_t signature[ATCA_SIG_SIZE];
    size_t sig_len = sizeof(signature);
    const char* pStr = buf;

    bool verified = false;

    if (!buf || !buflen || !pubkey)
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        /* Payload */
        pStr = strchr(pStr, '.') + 1;
        if (!pStr)
        {
            break;
        }

        /* Signature */
        pStr = strchr(pStr, '.') + 1;
        if (!pStr)
        {
            break;
        }

        /* Extract the signature */
        if (ATCA_SUCCESS != (status = atcab_base64decode_(pStr, strlen(pStr),
                                                          signature, &sig_len, atcab_b64rules_urlsafe)))
        {
            break;
        }

        /* Digest the token */
        if (ATCA_SUCCESS != (status = atcac_sw_sha2_256((const uint8_t*)buf, pStr - buf - 1, digest)))
        {
            break;
        }

        /* Do a signature verification using the device */
        if (ATCA_SUCCESS != (status = atcab_verify_extern(digest, signature,
                                                          pubkey, &verified)))
        {
            break;
        }

        if (!verified)
        {
            status = ATCA_CHECKMAC_VERIFY_FAILED;
        }
    }
    while (0);

    return status;
}

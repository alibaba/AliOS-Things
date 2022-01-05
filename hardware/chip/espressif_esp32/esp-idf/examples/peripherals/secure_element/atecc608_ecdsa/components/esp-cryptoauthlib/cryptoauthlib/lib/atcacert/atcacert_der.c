/**
 * \file
 * \brief functions required to work with DER encoded data related to X.509 certificates.
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


#include "atcacert_der.h"
#include <string.h>

int atcacert_der_enc_length(uint32_t length, uint8_t* der_length, size_t* der_length_size)
{
    size_t der_length_size_calc = 0;
    int exp = sizeof(length) - 1;

    if (der_length_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (length < 0x80)
    {
        // The length can take the short form with only one byte
        der_length_size_calc = 1;
        exp = 0;
    }
    else
    {
        // Length is long-form, encoded as a multi-byte big-endian unsigned integer

        // Find first non-zero octet
        while (length / ((uint32_t)1 << (8 * exp)) == 0)
        {
            exp--;
        }

        der_length_size_calc = 2 + exp;
    }

    if (der_length != NULL && *der_length_size < der_length_size_calc)
    {
        *der_length_size = der_length_size_calc;
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }

    *der_length_size = der_length_size_calc;

    if (der_length == NULL)
    {
        return ATCACERT_E_SUCCESS;  // Caller is only requesting the size

    }
    // Encode length in big-endian format
    for (; exp >= 0; exp--)
    {
        der_length[der_length_size_calc - 1 - exp] = (uint8_t)((length >> (exp * 8)) & 0xFF);
    }

    if (der_length_size_calc > 1)
    {
        der_length[0] = 0x80 | (uint8_t)(der_length_size_calc - 1);  // Set number of bytes octet with long-form flag

    }
    return ATCACERT_E_SUCCESS;
}

int atcacert_der_dec_length(const uint8_t* der_length, size_t* der_length_size, uint32_t* length)
{
    if (der_length == NULL || der_length_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (*der_length_size < 1)
    {
        return ATCACERT_E_DECODING_ERROR;
    }

    if (der_length[0] & 0x80)
    {
        // Long form
        size_t num_bytes = der_length[0] & 0x7F;
        size_t i;
        if (*der_length_size < num_bytes + 1)
        {
            return ATCACERT_E_DECODING_ERROR;   //  Invalid DER length format, not enough data.
        }
        if (num_bytes == 0)
        {
            return ATCACERT_E_DECODING_ERROR;   //  Invalid DER length format, indefinite length not supported.
        }
        if (num_bytes > sizeof(*length))
        {
            return ATCACERT_E_DECODING_ERROR;   //  Can't parse DER length format, larger than length.

        }
        if (length != NULL)
        {
            // Decode integer in big-endian format
            *length = 0;
            for (i = 1; i <= num_bytes; i++)
            {
                *length += der_length[i] * ((uint32_t)1 << (8 * (num_bytes - i)));
            }
        }
        *der_length_size = num_bytes + 1; // Return the actual number of bytes the DER length encoding used.
    }
    else
    {
        if (length != NULL)
        {
            *length = der_length[0];
        }
        *der_length_size = 1; // Return the actual number of bytes the DER length encoding used.
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_der_adjust_length(uint8_t* der_length, size_t* der_length_size, int delta_length, uint32_t* new_length)
{
    int ret = 0;
    size_t new_der_len_size = 0;
    uint32_t old_len = 0;
    uint32_t new_len = 0;
    uint8_t new_der_length[5];

    ret = atcacert_der_dec_length(der_length, der_length_size, &old_len);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    if (delta_length < 0 && (size_t)(-delta_length) > old_len)
    {
        return ATCACERT_E_ERROR;
    }
    new_len = old_len + delta_length;

    if (new_length != NULL)
    {
        *new_length = new_len;
    }

    new_der_len_size = sizeof(new_der_length);
    ret = atcacert_der_enc_length(new_len, new_der_length, &new_der_len_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    if (*der_length_size != new_der_len_size)
    {
        return ATCACERT_E_BAD_CERT;
    }

    memcpy(der_length, new_der_length, new_der_len_size);

    return 0;
}

int atcacert_der_enc_integer(const uint8_t* int_data,
                             size_t         int_data_size,
                             uint8_t        is_unsigned,
                             uint8_t*       der_int,
                             size_t*        der_int_size)
{
    uint8_t der_length[5];
    size_t der_length_size = sizeof(der_length);
    size_t der_int_size_calc = 0;
    size_t trim = 0;
    size_t pad = 0;
    int ret;

    if (int_data == NULL || der_int_size == NULL || int_data_size <= 0)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (!(is_unsigned && (int_data[0] & 0x80)))
    {
        // This is not an unsigned value that needs a padding byte, trim any unnecessary bytes.
        // Trim a byte when the upper 9 bits are all 0s or all 1s.
        while (
            (int_data_size - trim >= 2) && (
                ((int_data[trim] == 0x00) && ((int_data[trim + 1] & 0x80) == 0)) ||
                ((int_data[trim] == 0xFF) && ((int_data[trim + 1] & 0x80) != 0))))
        {
            trim++;
        }
    }
    else
    {
        // Will be adding extra byte for unsigned padding so it's not interpreted as negative
        pad = 1;
    }

    ret = atcacert_der_enc_length((uint32_t)(int_data_size + pad - trim), der_length, &der_length_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    der_int_size_calc = 1 + der_length_size + int_data_size + pad - trim;

    if (der_int != NULL && der_int_size_calc > *der_int_size)
    {
        *der_int_size = der_int_size_calc;
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }

    *der_int_size = der_int_size_calc;

    if (der_int == NULL)
    {
        return ATCACERT_E_SUCCESS;                                                      // Caller just wanted the size of the encoded integer

    }
    der_int[0] = 0x02;                                                                  // Integer tag
    memcpy(&der_int[1], der_length, der_length_size);                                   // Integer length
    if (pad)
    {
        der_int[der_length_size + 1] = 0;                                               // Unsigned integer value requires padding byte so it's not interpreted as negative
    }
    memcpy(&der_int[der_length_size + 1 + pad], &int_data[trim], int_data_size - trim); // Integer value

    return ATCACERT_E_SUCCESS;
}

int atcacert_der_dec_integer(const uint8_t* der_int,
                             size_t*        der_int_size,
                             uint8_t*       int_data,
                             size_t*        int_data_size)
{
    int ret = 0;
    size_t der_length_size = 0;
    uint32_t int_data_size_calc = 0;

    if (der_int == NULL || der_int_size == NULL || (int_data != NULL && int_data_size == NULL))
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (*der_int_size < 1)
    {
        return ATCACERT_E_DECODING_ERROR;  // No data to decode

    }
    if (der_int[0] != 0x02)
    {
        return ATCACERT_E_DECODING_ERROR;  // Not an integer tag

    }
    der_length_size = *der_int_size - 1;
    ret = atcacert_der_dec_length(&der_int[1], &der_length_size, &int_data_size_calc);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    if (*der_int_size < (1 + der_length_size + int_data_size_calc))
    {
        return ATCACERT_E_DECODING_ERROR;  // Invalid DER integer, not enough data.

    }
    *der_int_size = (1 + der_length_size + int_data_size_calc);

    if (int_data == NULL && int_data_size == NULL)
    {
        return ATCACERT_E_SUCCESS;  // Caller doesn't want the actual data, just the der_int_size

    }
    if (int_data != NULL && *int_data_size < int_data_size_calc)
    {
        *int_data_size = int_data_size_calc;
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }

    *int_data_size = int_data_size_calc;

    if (int_data == NULL)
    {
        return ATCACERT_E_SUCCESS;  // Caller doesn't want the actual data, just the int_data_size

    }
    memcpy(int_data, &der_int[1 + der_length_size], int_data_size_calc);

    return ATCACERT_E_SUCCESS;
}

int atcacert_der_enc_ecdsa_sig_value(const uint8_t raw_sig[64],
                                     uint8_t*      der_sig,
                                     size_t*       der_sig_size)
{
    int ret = 0;
    size_t r_size = 0;
    size_t s_size = 0;
    size_t der_sig_size_calc = 0;

    if (raw_sig == NULL || der_sig_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    // Find size of the DER encoded R integer
    ret = atcacert_der_enc_integer(&raw_sig[0], 32, TRUE, NULL, &r_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    // Find size of the DER encoded S integer
    ret = atcacert_der_enc_integer(&raw_sig[32], 32, TRUE, NULL, &s_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    // This calculation assumes all DER lengths are a single byte, which is fine for 32 byte
    // R and S integers.
    der_sig_size_calc = 5 + r_size + s_size;

    if (der_sig != NULL && *der_sig_size < der_sig_size_calc)
    {
        *der_sig_size = der_sig_size_calc;
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }

    *der_sig_size = der_sig_size_calc;

    if (der_sig == NULL)
    {
        return ATCACERT_E_SUCCESS;                  // Caller just wanted the encoded size

    }
    der_sig[0] = 0x03;                              // signatureValue bit string tag
    der_sig[1] = (uint8_t)(der_sig_size_calc - 2);  // signatureValue bit string length
    der_sig[2] = 0x00;                              // signatureValue bit string spare bits

    // signatureValue bit string value is the DER encoding of ECDSA-Sig-Value
    der_sig[3] = 0x30;                              // sequence tag
    der_sig[4] = (uint8_t)(der_sig_size_calc - 5);  // sequence length

    // Add R integer
    ret = atcacert_der_enc_integer(&raw_sig[0], 32, TRUE, &der_sig[5], &r_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    // Add S integer
    ret = atcacert_der_enc_integer(&raw_sig[32], 32, TRUE, &der_sig[5 + r_size], &s_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_der_dec_ecdsa_sig_value(const uint8_t* der_sig,
                                     size_t*        der_sig_size,
                                     uint8_t        raw_sig[64])
{
    int ret = 0;
    size_t curr_idx = 0;
    size_t dec_size = 0;
    uint32_t bs_length = 0;
    uint32_t seq_length = 0;
    size_t r_size = 0;
    size_t s_size = 0;
    uint8_t int_data[33];
    size_t int_data_size = 0;

    if (der_sig == NULL || der_sig_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (*der_sig_size < 1)
    {
        return ATCACERT_E_DECODING_ERROR;  // No data to decode

    }
    // signatureValue bit string tag
    curr_idx = 0;
    if (der_sig[curr_idx] != 0x03)
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected tag value
    }
    curr_idx++;

    // signatureValue bit string length
    dec_size = *der_sig_size - curr_idx;
    ret = atcacert_der_dec_length(&der_sig[curr_idx], &dec_size, &bs_length);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;  // Failed to decode length
    }
    curr_idx += dec_size;
    if (curr_idx + bs_length > *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // Not enough data in buffer to decode the rest

    }
    // signatureValue bit string spare bits
    if (curr_idx >= *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;   // No data left
    }
    if (der_sig[curr_idx] != 0x00)
    {
        return ATCACERT_E_DECODING_ERROR;   // Unexpected spare bits value
    }
    curr_idx++;

    // signatureValue bit string value is the DER encoding of ECDSA-Sig-Value

    // sequence tag
    if (curr_idx >= *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;   // No data left
    }
    if (der_sig[curr_idx] != 0x30)
    {
        return ATCACERT_E_DECODING_ERROR;   // Unexpected tag value
    }
    curr_idx++;

    // sequence length
    if (curr_idx >= *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // No data left
    }
    dec_size = *der_sig_size - curr_idx;
    ret = atcacert_der_dec_length(&der_sig[curr_idx], &dec_size, &seq_length);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;  // Failed to decode length
    }
    curr_idx += dec_size;
    if (curr_idx + seq_length > *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // Not enough data in buffer to decode the rest

    }
    // R integer
    if (curr_idx >= *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // No data left
    }
    r_size = *der_sig_size - curr_idx;
    int_data_size = sizeof(int_data);
    ret = atcacert_der_dec_integer(&der_sig[curr_idx], &r_size, int_data, &int_data_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;  // Failed to decode length
    }
    curr_idx += r_size;

    if (raw_sig != NULL)
    {
        memset(raw_sig, 0, 64);  // Zero out the raw sig as the decoded integers may not touch all bytes

    }
    if (int_data_size <= 32)
    {
        if (raw_sig != NULL)
        {
            memcpy(&raw_sig[32 - int_data_size], &int_data[0], int_data_size);
        }
    }
    else if (int_data_size == 33)
    {
        if (int_data[0] != 0x00)
        {
            return ATCACERT_E_DECODING_ERROR;  // R integer is too large
        }
        // DER integer was 0-padded to keep it positive
        if (raw_sig != NULL)
        {
            memcpy(&raw_sig[0], &int_data[1], 32);
        }
    }
    else
    {
        return ATCACERT_E_DECODING_ERROR; // R integer is too large

    }
    // S integer
    if (curr_idx >= *der_sig_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // No data left
    }
    s_size = *der_sig_size - curr_idx;
    int_data_size = sizeof(int_data);
    ret = atcacert_der_dec_integer(&der_sig[curr_idx], &s_size, int_data, &int_data_size);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;  // Failed to decode length
    }
    curr_idx += s_size;

    if (int_data_size <= 32)
    {
        if (raw_sig != NULL)
        {
            memcpy(&raw_sig[64 - int_data_size], &int_data[0], int_data_size);
        }
    }
    else if (int_data_size == 33)
    {
        if (int_data[0] != 0x00)
        {
            return ATCACERT_E_DECODING_ERROR;  // S integer is too large
        }
        // DER integer was 0-padded to keep it positive
        if (raw_sig != NULL)
        {
            memcpy(&raw_sig[32], &int_data[1], 32);
        }
    }
    else
    {
        return ATCACERT_E_DECODING_ERROR; // S integer is too large

    }
    if (seq_length != r_size + s_size)
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected extra data in sequence

    }
    if (bs_length != r_size + s_size + 3)
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected extra data in bit string

    }
    *der_sig_size = curr_idx;

    return ATCACERT_E_SUCCESS;
}

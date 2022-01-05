/**
 * \file
 * \brief Helpers to support the CryptoAuthLib Basic API methods
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

#ifndef ATCA_HELPERS_H_
#define ATCA_HELPERS_H_

#include "cryptoauthlib.h"

/** \ingroup atcab_
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

ATCA_STATUS atcab_printbin(uint8_t* binary, size_t bin_len, bool add_space);
ATCA_STATUS atcab_bin2hex(const uint8_t* bin, size_t bin_size, char* hex, size_t* hex_size);
ATCA_STATUS atcab_bin2hex_(const uint8_t* bin, size_t bin_size, char* hex, size_t* hex_size, bool is_pretty, bool is_space, bool is_upper);
ATCA_STATUS atcab_hex2bin(const char* ascii_hex, size_t ascii_hex_len, uint8_t* binary, size_t* bin_len);
ATCA_STATUS atcab_hex2bin_(const char* hex, size_t hex_size, uint8_t* bin, size_t* bin_size, bool is_space);
ATCA_STATUS atcab_printbin_sp(uint8_t* binary, size_t bin_len);
ATCA_STATUS atcab_printbin_label(const char* label, uint8_t* binary, size_t bin_len);


ATCA_STATUS packHex(const char* ascii_hex, size_t ascii_hex_len, char* packed_hex, size_t* packed_len);
bool isDigit(char c);
bool isWhiteSpace(char c);
bool isAlpha(char c);
bool isHexAlpha(char c);
bool isHex(char c);
bool isHexDigit(char c);

bool isBase64(char c, const uint8_t * rules);
bool isBase64Digit(char c, const uint8_t * rules);
uint8_t base64Index(char c, const uint8_t * rules);
char base64Char(uint8_t id, const uint8_t * rules);

extern uint8_t atcab_b64rules_default[4];
extern uint8_t atcab_b64rules_mime[4];
extern uint8_t atcab_b64rules_urlsafe[4];

ATCA_STATUS atcab_base64decode_(const char* encoded, size_t encoded_size, uint8_t* data, size_t* data_size, const uint8_t * rules);
ATCA_STATUS atcab_base64decode(const char* encoded, size_t encoded_size, uint8_t* data, size_t* data_size);

ATCA_STATUS atcab_base64encode_(const uint8_t* data, size_t data_size, char* encoded, size_t* encoded_size, const uint8_t * rules);
ATCA_STATUS atcab_base64encode(const uint8_t* data, size_t data_size, char* encoded, size_t* encoded_size);


ATCA_STATUS atcab_reversal(const uint8_t* bin, size_t bin_size, uint8_t* dest, size_t* dest_size);


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ATCA_HELPERS_H_ */

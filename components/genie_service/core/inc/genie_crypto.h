#ifndef __GENIE_CRYPTO_H__
#define __GENIE_CRYPTO_H__

#define GENIE_CRYPTO_UNIT_SIZE (16)

#define STATIC_OOB_LENGTH 16

/**
 * @brief get the authentication info
 * @param[in] random: the random sequence used for calculating.
 * @return the authenticated sequence
 */
uint8_t *genie_crypto_get_auth(const uint8_t random[16]);

/**
 * @brief encrypt ais data.
 * @param[in] raw data
 * @param[out] encryped data
 */
int genie_crypto_encrypt(const uint8_t data_in[16], uint8_t data_out[16]);

/**
 * @brief decrypt ais data.
 * @param[in] encryped data
 * @param[out] raw data
 */
int genie_crypto_decrypt(const uint8_t data_in[16], uint8_t data_out[16]);

/**
 * @brief get the cipher for ble auth.
 * @param[in] random, 16 Byte
 * @param[out] cipher, 1 6Byte
 */
void genie_ais_get_cipher(const uint8_t random[16], uint8_t *cipher);

/**
 * @brief clear the keys of ais service.
 */
void genie_ais_reset(void);

/**
 * @brief init the ais advertising.
 * @param[inout] the phoint of advertising data
 * @param[in] is silent adv or not
 */
void genie_crypto_adv_create(uint8_t ad_structure[14], uint8_t is_silent);

#endif

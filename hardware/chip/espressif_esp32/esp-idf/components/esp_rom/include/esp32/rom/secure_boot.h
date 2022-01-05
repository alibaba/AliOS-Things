// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"

#ifndef _ROM_SECURE_BOOT_H_
#define _ROM_SECURE_BOOT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ets_secure_boot_start(void);

void ets_secure_boot_finish(void);

void ets_secure_boot_hash(const uint32_t *buf);

void ets_secure_boot_obtain(void);

int ets_secure_boot_check(uint32_t *buf);

void ets_secure_boot_rd_iv(uint32_t *buf);

void ets_secure_boot_rd_abstract(uint32_t *buf);

bool ets_secure_boot_check_start(uint8_t abs_index, uint32_t iv_addr);

int ets_secure_boot_check_finish(uint32_t *abstract);

#ifdef CONFIG_ESP32_REV_MIN_3
#define SECURE_BOOT_NUM_BLOCKS 1

typedef enum {
    SBV2_SUCCESS = 0x3A5A5AA5,
    SBV2_FAILED = 0xA533885A,
} secure_boot_v2_status_t;

/* Secure Boot Version 2 - Public Key format */
typedef struct {
    uint8_t n[384]; /* Public key modulus */
    uint32_t e;     /* Public key exponent */
    uint8_t rinv[384];
    uint32_t mdash;
} ets_rsa_pubkey_t;

/* Secure Boot Version 2 signature format for ESP32 ECO3 */
typedef struct {
    uint8_t magic_byte;
    uint8_t version;
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t image_digest[32];
    ets_rsa_pubkey_t key;
    uint8_t signature[384];
    uint32_t block_crc;
    uint8_t _padding[16];
} ets_secure_boot_sig_block_t;

/* Multiple key block support */
struct ets_secure_boot_signature {
    ets_secure_boot_sig_block_t block[SECURE_BOOT_NUM_BLOCKS];
    uint8_t _padding[4096 - (sizeof(ets_secure_boot_sig_block_t) * SECURE_BOOT_NUM_BLOCKS)];
};
typedef struct ets_secure_boot_signature ets_secure_boot_signature_t;

/** @brief Verifies the signature block appended to a firmware image. Implemented in the ROM.
 *
 * This function is used to verify the bootloader before burning its public key hash into Efuse.
 * Also, it is used to verify the app on loading the image on boot and on OTA.
 *
 * @param sig The signature block flashed aligned 4096 bytes from the firmware.
 * @param image_digest The SHA-256 Digest of the firmware to be verified
 * @param trusted_key_digest The SHA-256 Digest of the public key (ets_rsa_pubkey_t) of a single signature block.
 * @param verified_digest RSA-PSS signature of image_digest. Pass an uninitialised array.
 *
 * @return SBV2_SUCCESS if signature is valid
 * SBV2_FAILED for failures.
 */
secure_boot_v2_status_t ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig, const uint8_t *image_digest, const uint8_t *trusted_key_digest, uint8_t *verified_digest);

/** @brief This function verifies the 1st stage bootloader. Implemented in the ROM.
 *  Reboots post verification. It reads the Efuse key for verification of the public key.
 *
 * This function is not used in the current workflow.
 *
 */
void ets_secure_boot_verify_boot_bootloader(void);

/** @brief Confirms if the secure boot V2 has been enabled. Implemented in the ROM.
 *
 * In ESP32-ECO3 - It checks the value of ABS_DONE_1 in EFuse.
 *
 * @return true if is Secure boot v2 has been enabled
 * False if Secure boot v2 has not been enabled.
 */
bool ets_use_secure_boot_v2();

#endif /* CONFIG_ESP32_REV_MIN_3 */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SECURE_BOOT_H_ */
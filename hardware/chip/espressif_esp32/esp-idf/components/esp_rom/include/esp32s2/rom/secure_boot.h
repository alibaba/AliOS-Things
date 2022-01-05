// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ROM_SECURE_BOOT_H_
#define _ROM_SECURE_BOOT_H_

#include <stdint.h>
#include "rsa_pss.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ets_secure_boot_sig_block;
struct ets_secure_boot_signature_t;

typedef struct ets_secure_boot_sig_block ets_secure_boot_sig_block_t;
typedef struct ets_secure_boot_signature ets_secure_boot_signature_t;
typedef struct ets_secure_boot_key_digests ets_secure_boot_key_digests_t;

/* 64KB 'staging buffer' for loading the verified bootloader

   Comes from the "shared buffers" region (see shared_buffers.h)

   The bootloader can't be safely linked into this address range
   (may be possible with some cleverness.)
*/
#define SECURE_BOOT_STAGING_BUFFER_START ((uint32_t)(g_shared_buffers.secure_boot_staging_buf))
#define SECURE_BOOT_STAGING_BUFFER_SZ    sizeof(g_shared_buffers.secure_boot_staging_buf)
#define SECURE_BOOT_STAGING_BUFFER_END   (SECURE_BOOT_STAGING_BUFFER_START + SECURE_BOOT_STAGING_BUFFER_SZ)

/* Anti-FI measure: use full words for success/fail, instead of
   0/non-zero
*/
typedef enum {
    SB_SUCCESS = 0x3A5A5AA5,
    SB_FAILED = 0x7533885E,
} ets_secure_boot_status_t;


/* Verify and stage-load the bootloader image
   (reconfigures cache to map, loads trusted key digests from efuse,
   copies the bootloader into the staging buffer.)

   If allow_key_revoke is true and aggressive revoke efuse is set,
   any failed signature has its associated key revoked in efuse.

   If result is SB_SUCCESS, the "simple hash" of the bootloader
   is copied into verified_hash.
*/
ets_secure_boot_status_t ets_secure_boot_verify_stage_bootloader(uint8_t *verified_hash, bool allow_key_revoke);

/* Verify bootloader image (reconfigures cache to map),
   with key digests provided as parameters.)

   Can be used to verify secure boot status before enabling
   secure boot permanently.

   If stage_load parameter is true, bootloader is copied into staging
   buffer in RAM at the same time.

   If result is SB_SUCCESS, the "simple hash" of the bootloader is
   copied into verified_hash.
*/
ets_secure_boot_status_t ets_secure_boot_verify_bootloader_with_keys(uint8_t *verified_hash, const ets_secure_boot_key_digests_t *trusted_keys, bool stage_load);

/* Read key digests from efuse. Any revoked/missing digests will be
   marked as NULL
*/
ETS_STATUS ets_secure_boot_read_key_digests(ets_secure_boot_key_digests_t *trusted_keys);

/* Verify supplied signature against supplied digest, using
   supplied trusted key digests.

   Doesn't reconfigure cache or any other hardware access except for RSA peripheral.

   If result is SB_SUCCESS, the image_digest value is copied into verified_digest.
*/
ets_secure_boot_status_t ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig, const uint8_t *image_digest, const ets_secure_boot_key_digests_t *trusted_keys, uint8_t *verified_digest);

/* Revoke a public key digest in efuse.
   @param index Digest to revoke. Must be 0, 1 or 2.
 */
void ets_secure_boot_revoke_public_key_digest(int index);

#define ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC 0xE7

/* Secure Boot V2 signature block

   (Up to 3 in a signature sector are appended to the image)
 */
struct ets_secure_boot_sig_block {
    uint8_t magic_byte;
    uint8_t version;
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t image_digest[32];
    ets_rsa_pubkey_t key;
    uint8_t signature[384];
    uint32_t block_crc;
    uint8_t _padding[16];
};

_Static_assert(sizeof(ets_secure_boot_sig_block_t) == 1216, "invalid sig block size");

#define SECURE_BOOT_NUM_BLOCKS 3

/* V2 Secure boot signature sector (up to 3 blocks) */
struct ets_secure_boot_signature {
    ets_secure_boot_sig_block_t block[SECURE_BOOT_NUM_BLOCKS];
    uint8_t _padding[4096 - (sizeof(ets_secure_boot_sig_block_t) * SECURE_BOOT_NUM_BLOCKS)];
};

_Static_assert(sizeof(ets_secure_boot_signature_t) == 4096, "invalid sig sector size");

#define MAX_KEY_DIGESTS 3

struct ets_secure_boot_key_digests {
    const void *key_digests[MAX_KEY_DIGESTS];
    bool allow_key_revoke;
};

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SECURE_BOOT_H_ */

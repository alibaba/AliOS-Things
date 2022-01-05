/**
 * \file
 *
 * \brief Provides required APIs to manage secure boot under various scenarios.
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

#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "atca_status.h"
#include "secure_boot_memory.h"
#include "atca_command.h"
#include "crypto/atca_crypto_sw_sha2.h"

#define SECURE_BOOT_CONFIG_DISABLE              0
#define SECURE_BOOT_CONFIG_FULL_BOTH            1
#define SECURE_BOOT_CONFIG_FULL_SIGN            2
#define SECURE_BOOT_CONFIG_FULL_DIG             3

#ifndef SECURE_BOOT_CONFIGURATION
#define SECURE_BOOT_CONFIGURATION               SECURE_BOOT_CONFIG_FULL_DIG
#endif

#ifndef SECURE_BOOT_DIGEST_ENCRYPT_ENABLED
#define SECURE_BOOT_DIGEST_ENCRYPT_ENABLED      true
#endif

#ifndef SECURE_BOOT_UPGRADE_SUPPORT
#define SECURE_BOOT_UPGRADE_SUPPORT             true
#endif

typedef struct
{
    uint16_t secure_boot_mode : 2;
    uint16_t secure_boot_reserved1 : 1;
    uint16_t secure_boot_persistent_enable : 1;
    uint16_t secure_boot_rand_nonce : 1;
    uint16_t secure_boot_reserved2 : 3;
    uint16_t secure_boot_sig_dig : 4;
    uint16_t secure_boot_pub_key : 4;
}secure_boot_config_bits;


typedef struct
{
    memory_parameters  memory_params;
    atcac_sha2_256_ctx s_sha_context;
    uint8_t            app_digest[ATCA_SHA_DIGEST_SIZE];
    #if SECURE_BOOT_DIGEST_ENCRYPT_ENABLED
    uint8_t randomnum[RANDOM_RSP_SIZE];
    uint8_t io_protection_key[ATCA_KEY_SIZE];
    #endif
}secure_boot_parameters;

ATCA_STATUS secure_boot_process(void);
ATCA_STATUS bind_host_and_secure_element_with_io_protection(uint16_t slot);
extern ATCA_STATUS host_generate_random_number(uint8_t *rand);

#ifdef __cplusplus
}
#endif

#endif




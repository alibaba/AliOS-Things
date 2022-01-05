/**
 * \file
 * \brief TNG LORA signer certificate definition
 *
 * \copyright (c) 2015-2019 Microchip Technology Inc. and its subsidiaries.
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

#include "atcacert/atcacert_def.h"
#include "tngtls_cert_def_1_signer.h"

extern const uint8_t g_tngtls_cert_template_1_signer[];

const atcacert_def_t g_tnglora_cert_def_1_signer = {
    .type                = CERTTYPE_X509,
    .template_id         = 1,
    .chain_id            = 0,
    .private_key_slot    = 0,
    .sn_source           = SNSRC_PUB_KEY_HASH,
    .cert_sn_dev_loc     = {
        .zone            = DEVZONE_NONE,
        .slot            = 0,
        .is_genkey       = 0,
        .offset          = 0,
        .count           = 0
    },
    .issue_date_format   = DATEFMT_RFC5280_UTC,
    .expire_date_format  = DATEFMT_RFC5280_GEN,
    .tbs_cert_loc        = {
        .offset          = 4,
        .count           = 430
    },
    .expire_years        = 31,
    .public_key_dev_loc  = {
        .zone            = DEVZONE_DATA,
        .slot            = 8,
        .is_genkey       = 0,
        .offset          = 0,
        .count           = 72
    },
    .comp_cert_dev_loc   = {
        .zone            = DEVZONE_DATA,
        .slot            = 8,
        .is_genkey       = 0,
        .offset          = 144,
        .count           = 72
    },
    .std_cert_elements   = {
        {   // STDCERT_PUBLIC_KEY
            .offset      = 266,
            .count       = 64
        },
        {   // STDCERT_SIGNATURE
            .offset      = 446,
            .count       = 64
        },
        {   // STDCERT_ISSUE_DATE
            .offset      = 128,
            .count       = 13
        },
        {   // STDCERT_EXPIRE_DATE
            .offset      = 143,
            .count       = 15
        },
        {   // STDCERT_SIGNER_ID
            .offset      = 235,
            .count       = 4
        },
        {   // STDCERT_CERT_SN
            .offset      = 15,
            .count       = 16
        },
        {   // STDCERT_AUTH_KEY_ID
            .offset      = 414,
            .count       = 20
        },
        {   // STDCERT_SUBJ_KEY_ID
            .offset      = 381,
            .count       = 20
        }
    },
    .cert_elements       = NULL,
    .cert_elements_count = 0,
    .cert_template       = g_tngtls_cert_template_1_signer,
    .cert_template_size  = TNGTLS_CERT_TEMPLATE_1_SIGNER_SIZE,
    .ca_cert_def         = NULL
};

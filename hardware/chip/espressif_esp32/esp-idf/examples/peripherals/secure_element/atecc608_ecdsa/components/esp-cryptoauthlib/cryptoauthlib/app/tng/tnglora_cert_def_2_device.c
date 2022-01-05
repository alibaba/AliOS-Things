/**
 * \file
 * \brief TNG LORA device certificate definition
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
#include "tngtls_cert_def_2_device.h"
#include "tnglora_cert_def_1_signer.h"

extern const uint8_t g_tngtls_cert_template_2_device[];
extern const atcacert_cert_element_t g_tngtls_cert_elements_2_device[];

const atcacert_def_t g_tnglora_cert_def_2_device = {
    .type                = CERTTYPE_X509,
    .template_id         = 2,
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
        .count           = 415
    },
    .expire_years        = 28,
    .public_key_dev_loc  = {
        .zone            = DEVZONE_DATA,
        .slot            = 1,
        .is_genkey       = 1,
        .offset          = 0,
        .count           = 64
    },
    .comp_cert_dev_loc   = {
        .zone            = DEVZONE_DATA,
        .slot            = 8,
        .is_genkey       = 0,
        .offset          = 72,
        .count           = 72
    },
    .std_cert_elements   = {
        {   // STDCERT_PUBLIC_KEY
            .offset      = 257,
            .count       = 64
        },
        {   // STDCERT_SIGNATURE
            .offset      = 431,
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
            .offset      = 120,
            .count       = 4
        },
        {   // STDCERT_CERT_SN
            .offset      = 15,
            .count       = 16
        },
        {   // STDCERT_AUTH_KEY_ID
            .offset      = 399,
            .count       = 20
        },
        {   // STDCERT_SUBJ_KEY_ID
            .offset      = 366,
            .count       = 20
        }
    },
    .cert_elements       = g_tngtls_cert_elements_2_device,
    .cert_elements_count = TNGTLS_CERT_ELEMENTS_2_DEVICE_COUNT,
    .cert_template       = g_tngtls_cert_template_2_device,
    .cert_template_size  = TNGTLS_CERT_TEMPLATE_2_DEVICE_SIZE,
    .ca_cert_def         = &g_tnglora_cert_def_1_signer
};

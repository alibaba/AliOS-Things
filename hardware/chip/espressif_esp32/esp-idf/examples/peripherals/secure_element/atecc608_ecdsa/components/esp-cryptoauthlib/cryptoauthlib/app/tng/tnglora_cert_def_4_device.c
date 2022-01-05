#include "atcacert/atcacert_def.h"
#include "tnglora_cert_def_4_device.h"
#include "tnglora_cert_def_1_signer.h"

const uint8_t g_tnglora_cert_template_4_device[TNGLORA_CERT_TEMPLATE_4_DEVICE_SIZE] = {
    0x30, 0x82, 0x02, 0x24, 0x30, 0x82, 0x01, 0xc9, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x55,
    0xce, 0x2e, 0x8f, 0xf6, 0x1c, 0x62, 0x50, 0xb7, 0xe1, 0x68, 0x03, 0x54, 0x14, 0x1c, 0x94, 0x30,
    0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x4f, 0x31, 0x21, 0x30,
    0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x63, 0x68, 0x69,
    0x70, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f, 0x6c, 0x6f, 0x67, 0x79, 0x20, 0x49, 0x6e, 0x63,
    0x31, 0x2a, 0x30, 0x28, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x21, 0x43, 0x72, 0x79, 0x70, 0x74,
    0x6f, 0x20, 0x41, 0x75, 0x74, 0x68, 0x65, 0x6e, 0x74, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e,
    0x20, 0x53, 0x69, 0x67, 0x6e, 0x65, 0x72, 0x20, 0x46, 0x46, 0x46, 0x46, 0x30, 0x20, 0x17, 0x0d,
    0x31, 0x38, 0x31, 0x31, 0x30, 0x38, 0x30, 0x35, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x18, 0x0f, 0x32,
    0x30, 0x34, 0x36, 0x31, 0x31, 0x30, 0x38, 0x30, 0x35, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x30, 0x42,
    0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x4d, 0x69, 0x63, 0x72, 0x6f,
    0x63, 0x68, 0x69, 0x70, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f, 0x6c, 0x6f, 0x67, 0x79, 0x20,
    0x49, 0x6e, 0x63, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x14, 0x73, 0x6e,
    0x30, 0x31, 0x32, 0x33, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36,
    0x32, 0x37, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06,
    0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x71, 0xf1, 0xa7,
    0x0d, 0xa3, 0x79, 0xa3, 0xfd, 0xed, 0x6b, 0x50, 0x10, 0xbd, 0xad, 0x6e, 0x1f, 0xb9, 0xe8, 0xeb,
    0xa7, 0xdf, 0x2c, 0x4b, 0x5c, 0x67, 0xd3, 0x5e, 0xba, 0x84, 0xda, 0x09, 0xe7, 0x7a, 0xe8, 0xdb,
    0x2c, 0xcb, 0x96, 0x28, 0xee, 0xeb, 0x85, 0xcd, 0xaa, 0xb3, 0x5c, 0x92, 0xe5, 0x3e, 0x1c, 0x44,
    0xd5, 0x5a, 0x2b, 0xa7, 0xa0, 0x24, 0xaa, 0x92, 0x60, 0x3b, 0x68, 0x94, 0x8a, 0xa3, 0x81, 0x91,
    0x30, 0x81, 0x8e, 0x30, 0x2e, 0x06, 0x03, 0x55, 0x1d, 0x11, 0x04, 0x27, 0x30, 0x25, 0xa4, 0x23,
    0x30, 0x21, 0x31, 0x1f, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x04, 0x05, 0x13, 0x16, 0x65, 0x75, 0x69,
    0x36, 0x34, 0x5f, 0x30, 0x30, 0x30, 0x34, 0x41, 0x33, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30,
    0x00, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x03,
    0x88, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x1a, 0x90, 0xb2, 0x22,
    0x37, 0xa4, 0x51, 0xb7, 0x57, 0xdd, 0x36, 0xd1, 0x3a, 0x85, 0x2b, 0xe1, 0x3d, 0x2e, 0xf2, 0xca,
    0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0xbc, 0xd4, 0xfd,
    0xe8, 0x80, 0x8a, 0x2d, 0xc9, 0x0b, 0x6d, 0x01, 0xa8, 0xc5, 0xb9, 0xb2, 0x47, 0x33, 0x7e, 0xbd,
    0xda, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x49, 0x00,
    0x30, 0x46, 0x02, 0x21, 0x00, 0xa8, 0x34, 0x1a, 0xf3, 0x21, 0xbc, 0xb1, 0x9c, 0xcc, 0x8f, 0x0f,
    0x1c, 0xe8, 0x90, 0x7c, 0xf8, 0xed, 0x4c, 0x24, 0x5f, 0x93, 0x4c, 0x50, 0xbd, 0x83, 0x30, 0xb6,
    0x1c, 0x87, 0x27, 0xd2, 0xc8, 0x02, 0x21, 0x00, 0xfc, 0x1e, 0x80, 0xca, 0x01, 0xe9, 0xd9, 0xd9,
    0x6b, 0x3e, 0xcb, 0x41, 0x79, 0x7b, 0x95, 0x99, 0x10, 0x21, 0x6e, 0xf2, 0x90, 0x2c, 0x0b, 0x13,
    0x22, 0x7f, 0x91, 0x9c, 0xd2, 0x63, 0x34, 0x82
};

const atcacert_cert_element_t g_tnglora_cert_elements_4_device[] = {
    {
        .id = "SN03",
        .device_loc ={
            .zone      = DEVZONE_CONFIG,
            .slot      = 0,
            .is_genkey = 0,
            .offset    = 0,
            .count     = 4
        },
        .cert_loc ={
            .offset = 208,
            .count  = 8
        },
        .transforms ={
            TF_BIN2HEX_UC,
            TF_NONE
        }
    },
    {
        .id = "SN48",
        .device_loc ={
            .zone      = DEVZONE_CONFIG,
            .slot      = 0,
            .is_genkey = 0,
            .offset    = 8,
            .count     = 5
        },
        .cert_loc ={
            .offset = 216,
            .count  = 10
        },
        .transforms ={
            TF_BIN2HEX_UC,
            TF_NONE
        }
    },
    {
        .id = "EUI-64",
        .device_loc ={
            .zone      = DEVZONE_DATA,
            .slot      = 10,
            .is_genkey = 0,
            .offset    = 0,
            .count     = 16
        },
        .cert_loc ={
            .offset = 355,
            .count  = 16
        },
        .transforms ={
            TF_NONE,
            TF_NONE
        }
    }
};

const atcacert_def_t g_tnglora_cert_def_4_device = {
    .type                = CERTTYPE_X509,
    .template_id         = 4,
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
        .count           = 461
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
            .offset      = 253,
            .count       = 64
        },
        {   // STDCERT_SIGNATURE
            .offset      = 477,
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
            .offset      = 445,
            .count       = 20
        },
        {   // STDCERT_SUBJ_KEY_ID
            .offset      = 412,
            .count       = 20
        }
    },
    .cert_elements       = g_tnglora_cert_elements_4_device,
    .cert_elements_count = sizeof(g_tnglora_cert_elements_4_device) / sizeof(g_tnglora_cert_elements_4_device[0]),
    .cert_template       = g_tnglora_cert_template_4_device,
    .cert_template_size  = sizeof(g_tnglora_cert_template_4_device),
    .ca_cert_def         = &g_tnglora_cert_def_1_signer
};
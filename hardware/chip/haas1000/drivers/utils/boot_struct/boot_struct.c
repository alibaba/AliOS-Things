/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "tool_msg.h"
#include "reboot_param.h"
#include "norflash_cfg.h"
#include "hal_cmu.h"

extern const char sys_build_info[];

#ifdef USER_SECURE_BOOT
extern const unsigned int system_info;
#endif

// -----------------------------------------------------------
// Boot struct and code sig struct
// -----------------------------------------------------------
#if defined(PROGRAMMER_INFLASH)
#undef PROGRAMMER
#endif

#define BOOT_STRUCT_LOC                 __attribute((section(".boot_struct")))

#ifdef USER_SECURE_BOOT
#define DEFAULT_BUILD_INFO              ((uint32_t)&system_info)
#else
#define DEFAULT_BUILD_INFO              ((uint32_t)sys_build_info)
#endif

#define DEFAULT_CODE_SIG                \
    {                                   \
        .code_size = 0,                 \
        .sig_len = SIG_LEN,             \
    }

#ifdef SIMU
#define DEFAULT_NORFLASH_SAMDLY         0x1
#define DEFAULT_NORFLASH_MOD_CLK        HAL_CMU_FREQ_26M
#define DEFAULT_NORFLASH_RDCMD          0x03
#else
#define DEFAULT_NORFLASH_SAMDLY         0x2
// Select 26M for 40M crystal case
#define DEFAULT_NORFLASH_MOD_CLK        HAL_CMU_FREQ_104M
// Below 50M: 0x03, Above 50M: 0x0B
#define DEFAULT_NORFLASH_RDCMD          0x0B
#endif

#define DEFAULT_NORFLASH_CFG            \
    {                                   \
        .neg_phase = 0,                 \
        .pos_neg   = 0,                 \
        .cmdquad   = 0,                 \
        .div       = 0x2,               \
        .samdly    = DEFAULT_NORFLASH_SAMDLY, \
        .dualmode  = 1,                 \
        .holdpin   = 0,                 \
        .wprpin    = 0,                 \
        .quadmode  = 0,                 \
        .mod_clk   = DEFAULT_NORFLASH_MOD_CLK, \
        .spiruen   = 0,                 \
        .spirden   = 0,                 \
        .dualiocmd = 0xBB,              \
        .rdcmd     = DEFAULT_NORFLASH_RDCMD, \
        .frdcmd    = 0x0B,              \
        .qrdcmd    = 0xEB,              \
    }

#ifdef SECURE_BOOT_V1

#ifdef PROGRAMMER

struct programmer_boot_struct_t {
    struct boot_struct_v1_t boot_struct;
    struct code_sig_struct_t code_sig_struct;
};

const struct programmer_boot_struct_t BOOT_STRUCT_LOC programmer_boot_struct = {
    .boot_struct = {
        .hdr = {
            .magic = BOOT_MAGIC_NUMBER,
            .security = 1,
            .hash_type = BOOT_HASH_TYPE_SHA256,
            .key_type = BOOT_KEY_TYPE_RSA2048,
            .key_len = KEY_LEN,
            .sig_len = SIG_LEN,
            .build_info_start = ((uint32_t)sys_build_info),
        },
    },
    .code_sig_struct = DEFAULT_CODE_SIG,
};

#elif defined(SECURE_BOOT)

struct secure_boot_struct_t {
    struct boot_struct_v1_t boot_struct;
    struct code_sig_struct_t code_sig_struct;
    struct norflash_cfg_struct_t norflash_cfg;
};

const struct secure_boot_struct_t BOOT_STRUCT_LOC secure_boot_struct = {
    .boot_struct = {
        .hdr = {
            .magic = ~0UL,
            .security = 1,
            .hash_type = BOOT_HASH_TYPE_SHA256,
            .key_type = BOOT_KEY_TYPE_RSA2048,
            .key_len = KEY_LEN,
            .sig_len = SIG_LEN,
            .build_info_start = DEFAULT_BUILD_INFO,
        },
    },
    .code_sig_struct = DEFAULT_CODE_SIG,
    .norflash_cfg = DEFAULT_NORFLASH_CFG,
};

#else

const struct boot_hdr_v1_t BOOT_STRUCT_LOC boot_struct = {
    .magic = ~0UL,
    .security = 0,
    .build_info_start = DEFAULT_BUILD_INFO,
};

#endif

#else // !SECURE_BOOT_V1

#ifdef PROGRAMMER

union programmer_boot_struct_t {
    // To keep compatible with the old download tools when downloadig non-secure images
    struct {
        struct boot_struct_v1_t dummy;
        struct code_sig_struct_t code_sig_struct;
    } s_v1;
    struct {
        struct boot_struct_t boot_struct;
        struct code_sig_struct_t code_sig_struct;
    } s;
};

const union programmer_boot_struct_t BOOT_STRUCT_LOC programmer_boot_struct = {
    .s = {
        .boot_struct = {
            .hdr = {
                .magic = BOOT_MAGIC_NUMBER,
                .security = 1,
                .version = BOOT_STRUCT_VERSION,
                .build_info_start = ((uint32_t)sys_build_info),
            },
        },
        .code_sig_struct = DEFAULT_CODE_SIG,
    },
};

#elif defined(SECURE_BOOT)

struct secure_boot_struct_t {
    struct boot_struct_t boot_struct;
    struct code_sig_struct_t code_sig_struct;
    struct norflash_cfg_struct_t norflash_cfg;
};

const struct secure_boot_struct_t BOOT_STRUCT_LOC secure_boot_struct = {
    .boot_struct = {
        .hdr = {
            .magic = ~0UL,
            .security = 1,
            .version = BOOT_STRUCT_VERSION,
            .build_info_start = DEFAULT_BUILD_INFO,
        },
#if HAAS_OTA_ENABLED
        .ver = HAAS_OTA_BIN_VER,
#endif
    },
    .code_sig_struct = DEFAULT_CODE_SIG,
    .norflash_cfg = DEFAULT_NORFLASH_CFG,
};

#else

const struct boot_hdr_t BOOT_STRUCT_LOC boot_struct = {
    .magic = ~0UL,
    .security = 0,
    .version = BOOT_STRUCT_VERSION,
    .build_info_start = DEFAULT_BUILD_INFO,
};

#endif

#endif // !SECURE_BOOT_V1

// -----------------------------------------------------------
// Reboot param
// -----------------------------------------------------------

#ifdef __ARMCC_VERSION
#define REBOOT_PARAM_LOC __attribute((section(".bss.reboot_param")))
#else
#define REBOOT_PARAM_LOC __attribute((section(".reboot_param")))
#endif

struct REBOOT_PARAM_T REBOOT_PARAM_LOC reboot_param;


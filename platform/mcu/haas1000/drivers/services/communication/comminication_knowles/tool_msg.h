#ifndef __TOOL_MSG_H__
#define __TOOL_MSG_H__

#include "communication_cmd_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_MAGIC_NUMBER           0xBE57EC1C
#define BOOT_HASH_TYPE_MD5          1
#define BOOT_HASH_TYPE_SHA256       2
#define BOOT_KEY_TYPE_RSA2048       1
#define BOOT_KEY_TYPE_ECDSA192      2
#define BOOT_KEY_TYPE_ECDSA256      3

#ifdef __PC_CMD_UART__
#define PREFIX_CHAR                 0x7b//{
#else
#define PREFIX_CHAR                 0xBE
#endif

#define KEY_LEN                     (4 + 256 + 256)
#define SIG_LEN                     256

#define BOOT_STRUCT_OFFSET_TO_SIGN(b)  \
    ((unsigned char *)&((struct boot_struct_t *)(b))->hdr.security)
#define BOOT_STRUCT_LEN_TO_SIGN     \
    ((unsigned int)&((struct boot_struct_t *)0)->sig[0] - \
    (unsigned int)&((struct boot_struct_t *)0)->hdr.security)

#define SIG_MSG_OVERHEAD            8
#define SIG_MSG_EXTRA_DATA_LEN      (sizeof(struct boot_struct_t) + sizeof(struct code_sig_struct_t))
#define SIG_MSG_TOTAL_LEN           (SIG_MSG_OVERHEAD + SIG_MSG_EXTRA_DATA_LEN)

#define CODE_MSG_OVERHEAD           8
#define BURN_DATA_MSG_OVERHEAD      16

#define SECTOR_SIZE_64K             (1 << 16)
#define SECTOR_SIZE_32K             (1 << 15)
#define SECTOR_SIZE_16K             (1 << 14)
#define SECTOR_SIZE_4K              (1 << 12)

#define MSG_TOTAL_LEN(msg)          (sizeof((msg)->hdr) + (msg)->hdr.len + 1)

enum MSG_TYPE {
    TYPE_SYS = 0x00,
    TYPE_READ = 0x01,
    TYPE_WRITE = 0x02,
    TYPE_BULK_READ = 0x03,
    TYPE_SYNC = 0x50,
    TYPE_SIG_INFO = 0x51,
    TYPE_SIG = 0x52,
    TYPE_CODE_INFO = 0x53,
    TYPE_CODE = 0x54,
    TYPE_RUN = 0x55,
    TYPE_SECTOR_SIZE = 0x60,
    TYPE_ERASE_BURN_START = 0x61,
    TYPE_ERASE_BURN_DATA = 0x62,
    TYPE_BURN_START = 0x63,
    TYPE_BURN_DATA = 0x64,
    TYPE_BURN_CMD = 0x65,
    TYPE_GET_SECTOR_INFO = 0x66,
#if defined(__EXT_CMD_SUPPORT__)
    TYPE_EXTEND_CMD = 0x67,
#endif
    TYPE_COMMUNICATION_CMD = 0x68,
};

enum SYS_CMD_TYPE {
    SYS_CMD_REBOOT = 0xF1,
    SYS_CMD_SHUTDOWN = 0xF2,
    SYS_CMD_FLASH_BOOT = 0xF3,
    SYS_CMD_SET_BOOTMODE = 0xE1,
    SYS_CMD_CLR_BOOTMODE = 0xE2,
    SYS_CMD_GET_BOOTMODE = 0xE3,
};

enum ERR_CODE {
    ERR_NONE = 0x00,
    ERR_LEN = 0x01,
    ERR_CHECKSUM = 0x02,
    ERR_NOT_SYNC = 0x03,
    ERR_NOT_SEC = 0x04,
    ERR_SYNC_WORD = 0x05,
    ERR_SYS_CMD = 0x06,
    ERR_DATA_ADDR = 0x07,
    ERR_DATA_LEN = 0x08,
    ERR_ACCESS_RIGHT = 0x09,

    ERR_TYPE_INVALID = 0x0F,

    //ERR_BOOT_OK = 0x10,
    ERR_BOOT_MAGIC = 0x11,
    ERR_BOOT_SEC = 0x12,
    ERR_BOOT_HASH_TYPE = 0x13,
    ERR_BOOT_KEY_TYPE = 0x14,
    ERR_BOOT_KEY_LEN = 0x15,
    ERR_BOOT_SIG_LEN = 0x16,
    ERR_BOOT_SIG = 0x17,
    ERR_BOOT_CRC = 0x18,
    ERR_BOOT_LEN = 0x19,
    ERR_SIG_CODE_SIZE = 0x1A,
    ERR_SIG_SIG_LEN = 0x1B,
    ERR_SIG_INFO_MISSING = 0x1C,

    ERR_CODE_OK = 0x20,
    ERR_BOOT_MISSING = 0x21,
    ERR_CODE_SIZE_SIG = 0x22,
    ERR_CODE_ADDR_SIZE = 0x23,
    ERR_CODE_INFO_MISSING = 0x24,
    ERR_CODE_CRC = 0x25,
    ERR_CODE_SIG = 0x26,

    ERR_CODE_MISSING = 0x31,

    ERR_BURN_OK = 0x60,
    ERR_SECTOR_SIZE = 0x61,
    ERR_SECTOR_SEQ_OVERFLOW = 0x62,
    ERR_BURN_INFO_MISSING = 0x63,
    ERR_SECTOR_DATA_LEN = 0x64,
    ERR_SECTOR_DATA_CRC = 0x65,
    ERR_SECTOR_SEQ = 0x66,
    ERR_ERASE_FLSH = 0x67,
    ERR_BURN_FLSH = 0x68,
    ERR_VERIFY_FLSH = 0x69,
    ERR_BURN_CMD = 0x6A,

    ERR_TYPE_MISMATCHED = 0xE1,
    ERR_SEQ_MISMATCHED = 0xE2,
    ERR_BUF_TOO_SMALL = 0xE3,

    ERR_INTERNAL = 0xFF,
};

enum PARSE_STATE {
    PARSE_HEADER,
    PARSE_DATA,
    PARSE_EXTRA,
};

struct message_t {
    struct msg_hdr_t {
        unsigned char prefix;
        unsigned char type;
        unsigned char seq;
        unsigned char len;
    } hdr;
    unsigned char data[255];
};

struct boot_struct_t {
    struct boot_hdr_t {
        unsigned int magic;
        unsigned short security;
        unsigned char hash_type;
        unsigned char key_type;
        unsigned short key_len;
        unsigned short sig_len;
        unsigned int build_info_start;
    } hdr;
    unsigned char key[KEY_LEN];
    unsigned char sig[SIG_LEN];
};

struct code_sig_struct_t {
    unsigned int code_size;
    unsigned short sig_len;
    unsigned short reserved;
    unsigned char sig[SIG_LEN];
};

struct exec_struct_t {
    unsigned int entry;
    unsigned int param;
    unsigned int sp;
    unsigned int reserved;
};

#ifdef __cplusplus
}
#endif

#endif


// Copyright (2018) Baidu Inc. All rights reserved.
/**
 * File: lightduer_flash.h
 * Auth: Sijun Li(lisijun@baidu.com)
 * Desc: Common defines for flash strings module.
 */
#ifndef BAIDU_DUER_LIGHTDUER_FLASH_H
#define BAIDU_DUER_LIGHTDUER_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"

#define FLASH_MAGIC                     0x56BD50C4
#define FLASH_MAGIC_BITMASK             0xffffffff
#define FLASH_LEN_BITMASK               0xffffffff
#define FLASH_INVALID_ADDR              0xffffffff

typedef enum {
    DUER_ERR_FLASH_CORRUPT = -2,
    DUER_ERR_FLASH_FULL = -3,
}duer_flash_errcode;

/*
 * Configuration of flash operations.
 * xxx_align_bits: Number of last bits of address to be aligned.
 *          For example, Bits=5 indicates header's last 5 bits are '0'.
 */
typedef struct {
    // For erase.
    int sector_align_bits;
    // For write.
    int page_align_bits;
    // For read.
    int word_align_bits;
} duer_flash_config_t;

typedef struct {
    void *handle;
    unsigned int len;
    void *object;
} duer_flash_context_t;

typedef struct {
    unsigned int magic;         // Magic number to check data valid;
    unsigned int len;           // Length of payload string;
    // Used to know the sequence of the data in the flash, it's used in flash ring buf
    unsigned int sequence_num;
} duer_flash_data_header;

inline unsigned int flash_addr_align_floor(unsigned int addr, int bits) {
    addr >>= bits;
    addr <<= bits;
    return addr;
}

inline unsigned int flash_addr_align_ceil(unsigned int addr, int bits) {
    unsigned int bitmask = (1 << bits) - 1;
    if (addr & bitmask) {
        addr >>= bits;
        addr += 1;
        addr <<= bits;
    }
    return addr;
}

inline unsigned int flash_addr_cycle(unsigned int addr, int size) {
    if (addr >= size) {
        return (addr - size);
    } else {
        return addr;
    }
}

/**
 * DESC:
 * Developer needs to implement this interface to read flash.
 *
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] addr: the address offset of flash to read.
 * @PARAM[out] buf: the buffer to store read data.
 * @PARAM[in] len: length of byte to read.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_flash_read(
        duer_flash_context_t *ctx,
        unsigned int addr,
        void *buf,
        unsigned int len);

/**
 * DESC:
 * Developer needs to implement this interface to write flash.
 *
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] addr: the address offset of flash to write.
 * @PARAM[in] buf: the buffer stored writing data.
 * @PARAM[in] len: length of byte to write.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_flash_write(
        duer_flash_context_t *ctx,
        unsigned int addr,
        void *buf,
        unsigned int len);

/**
 * DESC:
 * Developer needs to implement this interface to erase flash.
 *
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] addr: the address offset start of flash to erase.
 * @PARAM[in] len: length of byte to erase.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_flash_erase(
        duer_flash_context_t *ctx,
        unsigned int addr,
        unsigned int len);

int duer_payload_write_flash(
        duer_flash_context_t *ctx,
        unsigned int addr_start,
        duer_flash_data_header *p_flash_header,
        const char *payload_string,
        int payload_len,
        int page_size);

#ifdef __cplusplus
}
#endif

#endif//BAIDU_DUER_LIGHTDUER_FLASH_H


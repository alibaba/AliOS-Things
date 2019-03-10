/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This module provides functionality that is common to all Sensirion drivers
 */

#include "sensirion_i2c.h"
#include "sensirion_common.h"

/* #define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0) */



u8 sensirion_common_generate_crc(u8 *data, u16 count)
{
    u16 current_byte;
    u8 crc = CRC8_INIT;
    u8 crc_bit;

    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

s8 sensirion_common_check_crc(u8 *data, u16 count, u8 checksum)
{
    u8 res = sensirion_common_generate_crc(data, count);
    if (res != checksum) {
        //EXAMPLE_TRACE("sensirion_common_generate_crc(%02x %02x,%d) ==  0x%02x != 0x%02x", *(data+1), *(data+0), count, res, checksum);
        return STATUS_FAIL;
    }
        
    return STATUS_OK;
}

u16 sensirion_fill_cmd_send_buf(u8 *buf, u16 cmd, const u16 *args, u8 num_args)
{
    u8 crc;
    u8 i;
    u16 idx = 0;

    buf[idx++] = (u8)((cmd & 0xFF00) >> 8);
    buf[idx++] = (u8)((cmd & 0x00FF) >> 0);

    for (i = 0; i < num_args; ++i) {
        buf[idx++] = (u8)((args[i] & 0xFF00) >> 8);
        buf[idx++] = (u8)((args[i] & 0x00FF) >> 0);

        crc = sensirion_common_generate_crc((u8 *)&buf[idx - 2],
                                            SENSIRION_WORD_SIZE);
        buf[idx++] = crc;
    }
    return idx;
}

s16 sensirion_i2c_read_bytes(u8 address, u8 *data, u16 num_words) {
    s16 ret;
    u16 i, j;
    u16 size = num_words * (SENSIRION_WORD_SIZE + CRC8_LEN);
    u16 word_buf[SENSIRION_MAX_BUFFER_WORDS];
    u8 * const buf8 = (u8 *)word_buf;

    ret = sensirion_i2c_read(address, buf8, size);
    if (ret != STATUS_OK) {
        //EXAMPLE_TRACE("sensirion_i2c_read(%d, 0x%x, %d) returned %d", address, buf8, size, ret);
        return ret;
    }
    /* check the CRC for each word */
    for (i = 0, j = 0; i < size; i += SENSIRION_WORD_SIZE + CRC8_LEN) {

        ret = sensirion_common_check_crc(&buf8[i], SENSIRION_WORD_SIZE,
                                         buf8[i + SENSIRION_WORD_SIZE]);
        if (ret != STATUS_OK) {
            //EXAMPLE_TRACE("sensirion_common_check_crc returned %d (byte %d)",ret,i);
            return ret;
        }

        data[j++] = buf8[i];
        data[j++] = buf8[i + 1];
    }

    return STATUS_OK;
}

s16 sensirion_i2c_read_words(u8 address, u16 *data_words, u16 num_words) {
    s16 ret;
    u8 i;

    ret = sensirion_i2c_read_bytes(address, (u8 *)data_words, num_words);
    if (ret != STATUS_OK) {
        //EXAMPLE_TRACE("sensirion_i2c_read_bytes returned %d", ret);
        return ret;
    }

    for (i = 0; i < num_words; ++i)
        data_words[i] = be16_to_cpu(data_words[i]);

    return STATUS_OK;
}

s16 sensirion_i2c_write_cmd(u8 address, u16 command) {
    u8 buf[SENSIRION_COMMAND_SIZE];

    sensirion_fill_cmd_send_buf(buf, command, NULL, 0);
    return sensirion_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
}

s16 sensirion_i2c_write_cmd_with_args(u8 address, u16 command,
                                      const u16 *data_words, u16 num_words) {
    u8 buf[SENSIRION_MAX_BUFFER_WORDS];
    u16 buf_size;

    buf_size = sensirion_fill_cmd_send_buf(buf, command, data_words, num_words);
    return sensirion_i2c_write(address, buf, buf_size);
}

s16 sensirion_i2c_delayed_read_cmd(u8 address, u16 cmd, u32 delay_us,
                                   u16 *data_words, u16 num_words) {
    s16 ret;
    u8 buf[SENSIRION_COMMAND_SIZE];

    sensirion_fill_cmd_send_buf(buf, cmd, NULL, 0);
    ret = sensirion_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
    if (ret != STATUS_OK) {
        //EXAMPLE_TRACE("sensirion_i2c_write returned %d", ret);
        return ret;
    }

    if (delay_us)
        sensirion_sleep_usec(delay_us);

    return sensirion_i2c_read_words(address, data_words, num_words);
}

s16 sensirion_i2c_read_cmd(u8 address, u16 cmd, u16 *data_words, u16 num_words)
{
    return sensirion_i2c_delayed_read_cmd(address, cmd, 0, data_words,
                                          num_words);
}

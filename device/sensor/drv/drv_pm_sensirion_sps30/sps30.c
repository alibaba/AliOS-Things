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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sensirion_arch_config.h"
#include "sensirion_i2c.h"
#include "sensirion_common.h"
#include "sps_git_version.h"
#include "sps30.h"

const char *sps_get_driver_version()
{
    return SPS_DRV_VERSION_STR;
}

s16 sps30_probe() {
    char serial[SPS_MAX_SERIAL_LEN];

    sensirion_i2c_init();

    return sps30_get_serial(serial);
}

s16 sps30_get_serial(char *serial) {
    u16 i;
    s16 ret;
    union {
        char serial[SPS_MAX_SERIAL_LEN];
        u16 __enforce_alignment;
    } buffer;

    ret = sensirion_i2c_read_cmd(SPS_I2C_ADDRESS, SPS_CMD_GET_SERIAL,
                                 (u16 *)buffer.serial,
                                 SENSIRION_NUM_WORDS(buffer.serial));
    if (ret != STATUS_OK)
        return ret;

    SENSIRION_WORDS_TO_BYTES(buffer.serial, SENSIRION_NUM_WORDS(buffer.serial));
    for (i = 0; i < SPS_MAX_SERIAL_LEN; ++i) {
        serial[i] = buffer.serial[i];
        if (serial[i] == '\0')
            return 0;
    }

    return 0;
}

s16 sps30_start_measurement() {
    const u16 arg = SPS_CMD_START_MEASUREMENT_ARG;

    return sensirion_i2c_write_cmd_with_args(SPS_I2C_ADDRESS,
                                             SPS_CMD_START_MEASUREMENT,
                                             &arg,
                                             SENSIRION_NUM_WORDS(arg));
}

s16 sps30_stop_measurement() {
    return sensirion_i2c_write_cmd(SPS_I2C_ADDRESS, SPS_CMD_STOP_MEASUREMENT);
}

s16 sps30_read_data_ready(u16 *data_ready) {
    return sensirion_i2c_read_cmd(SPS_I2C_ADDRESS, SPS_CMD_GET_DATA_READY,
                                  data_ready, SENSIRION_NUM_WORDS(*data_ready));
}

s16 sps30_read_measurement(struct sps30_measurement *measurement) {
    s16 ret;
    u16 idx;
    union {
        u16 u16[2];
        u32 u;
        f32 f;
    } val, data[10];

    ret = sensirion_i2c_read_cmd(SPS_I2C_ADDRESS, SPS_CMD_READ_MEASUREMENT,
                                 data->u16, SENSIRION_NUM_WORDS(data));
    if (ret != STATUS_OK)
        return ret;

    SENSIRION_WORDS_TO_BYTES(data->u16, SENSIRION_NUM_WORDS(data));

    idx = 0;
    val.u = be32_to_cpu(data[idx].u);
    measurement->mc_1p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->mc_2p5 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->mc_4p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->mc_10p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->nc_0p5 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->nc_1p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->nc_2p5 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->nc_4p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->nc_10p0 = val.f;
    ++idx;
    val.u = be32_to_cpu(data[idx].u);
    measurement->typical_particle_size = val.f;
    ++idx;

    return 0;
}

s16 sps30_get_fan_auto_cleaning_interval(u32 *interval_seconds) {
    union {
        u16 u16[2];
        u32 u32;
    } data;
    s16 ret = sensirion_i2c_read_cmd(SPS_I2C_ADDRESS,
                                     SPS_CMD_AUTOCLEAN_INTERVAL,
                                     data.u16, SENSIRION_NUM_WORDS(data.u16));
    if (ret != STATUS_OK)
        return ret;

    SENSIRION_WORDS_TO_BYTES(data.u16, SENSIRION_NUM_WORDS(data.u16));
    *interval_seconds = be32_to_cpu(data.u32);

    return 0;
}

s16 sps30_set_fan_auto_cleaning_interval(u32 interval_seconds) {
    s16 ret;
    const u16 data[] = {(interval_seconds & 0xFFFF0000) >> 16,
                        (interval_seconds & 0x0000FFFF) >> 0};

    ret = sensirion_i2c_write_cmd_with_args(SPS_I2C_ADDRESS,
                                            SPS_CMD_AUTOCLEAN_INTERVAL, data,
                                            SENSIRION_NUM_WORDS(data));
    sensirion_sleep_usec(SPS_WRITE_DELAY_US);
    return ret;
}

s16 sps30_get_fan_auto_cleaning_interval_days(u8 *interval_days) {
    s16 ret;
    u32 interval_seconds;

    ret = sps30_get_fan_auto_cleaning_interval(&interval_seconds);
    if (ret < 0)
        return ret;

    *interval_days = interval_seconds / (24 * 60 * 60);
    return ret;
}

s16 sps30_set_fan_auto_cleaning_interval_days(u8 interval_days) {
    return sps30_set_fan_auto_cleaning_interval((u32)interval_days *
                                                24 * 60 * 60);
}

s16 sps30_reset() {
    return sensirion_i2c_write_cmd(SPS_I2C_ADDRESS, SPS_CMD_RESET);
}

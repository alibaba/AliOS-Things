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

#ifndef SPS30_H
#define SPS30_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"

#define SPS_MAX_SERIAL_LEN 32

struct sps30_measurement {
    f32 mc_1p0;
    f32 mc_2p5;
    f32 mc_4p0;
    f32 mc_10p0;
    f32 nc_0p5;
    f32 nc_1p0;
    f32 nc_2p5;
    f32 nc_4p0;
    f32 nc_10p0;
    f32 typical_particle_size;
};

//static const u8 SPS_I2C_ADDRESS = 0x69;
#define SPS_I2C_ADDRESS 0x69


#define SPS_CMD_START_MEASUREMENT       0x0010
#define SPS_CMD_START_MEASUREMENT_ARG   0x0300
#define SPS_CMD_STOP_MEASUREMENT        0x0104
#define SPS_CMD_READ_MEASUREMENT        0x0300
#define SPS_CMD_GET_DATA_READY          0x0202
#define SPS_CMD_AUTOCLEAN_INTERVAL      0x8004
#define SPS_CMD_GET_SERIAL              0xd033
#define SPS_CMD_RESET                   0xd304
#define SPS_WRITE_DELAY_US              20000


/**
 * sps_get_driver_version() - Return the driver version
 * Return:  Driver version string
 */
const char *sps_get_driver_version(void);

/**
 * sps30_probe() - check if SPS sensor is available and initialize it
 *
 * Note that Pin 4 must be pulled to ground for the sensor to operate in i2c
 * mode (this driver). When left floating, the sensor operates in UART mode
 * which is not compatible with this i2c driver.
 *
 * Return:  0 on success, an error code otherwise
 */
s16 sps30_probe();

/**
 * sps30_get_serial() - retrieve the serial number
 *
 * Note that serial must be discarded when the return code is non-zero.
 *
 * @serial: Memory where the serial number is written into as hex string (zero
 *          terminated). Must be at least SPS_MAX_SERIAL_LEN long.
 * Return:  0 on success, an error code otherwise
 */
s16 sps30_get_serial(char *serial);

/**
 * sps30_start_measurement() - start measuring
 *
 * Once the measurement is started, measurements are retrievable once per second
 * with sps30_read_measurement.
 *
 * Return:  0 on success, an error code otherwise
 */
s16 sps30_start_measurement();

/**
 * sps30_stop_measurement() - stop measuring
 *
 * Return:  0 on success, an error code otherwise
 */
s16 sps30_stop_measurement();

/**
 * sps30_read_datda_ready() - reads the current data-ready flag
 *
 * The data-ready flag indicates that new (not yet retrieved) measurements are
 * available
 *
 * @data_ready: Memory where the data-ready flag (0|1) is stored.
 * Return:      0 on success, an error code otherwise
 */
s16 sps30_read_data_ready(u16 *data_ready);

/**
 * sps30_read_measurement() - read a measurement
 *
 * Read the last measurement.
 *
 * Return:  0 on success, an error code otherwise
 */
s16 sps30_read_measurement(struct sps30_measurement *measurement);

/**
 * sps30_get_fan_auto_cleaning_interval() - read the current(*) auto-cleaning
 * interval
 *
 * Note that interval_seconds must be discarded when the return code is
 * non-zero.
 *
 * (*) Note that due to a firmware bug, the reported interval is only updated on
 * sensor restart/reset. If the interval was thus updated after the last reset,
 * the old value is still reported. Power-cycle the sensor or call sps30_reset()
 * first if you need the latest value.
 *
 * @interval_seconds:   Memory where the interval in seconds is stored
 * Return:              0 on success, an error code otherwise
 */
s16 sps30_get_fan_auto_cleaning_interval(u32 *interval_seconds);

/**
 * sps30_set_fan_auto_cleaning_interval() - set the current auto-cleaning
 * interval
 *
 * @interval_seconds:   Value in seconds used to sets the auto-cleaning
 *                      interval, 0 to disable auto cleaning
 * Return:              0 on success, an error code otherwise
 */
s16 sps30_set_fan_auto_cleaning_interval(u32 interval_seconds);

/**
 * sps30_get_fan_auto_cleaning_interval_days() - convenience function to read
 * the current(*) auto-cleaning interval in days
 *
 * note that the value is simply cut, not rounded or calculated nicely, thus
 * using this method is not precise when used in conjunction with
 * sps30_set_fan_auto_cleaning_interval instead of
 * sps30_set_fan_auto_cleaning_interval_days
 *
 * Note that interval_days must be discarded when the return code is non-zero.
 *
 * (*) Note that due to a firmware bug, the reported interval is only updated on
 * sensor restart/reset. If the interval was thus updated after the last reset,
 * the old value is still reported. Power-cycle the sensor or call sps30_reset()
 * first if you need the latest value.
 *
 * @interval_days:  Memory where the interval in days is stored
 * Return:          0 on success, an error code otherwise
 */
s16 sps30_get_fan_auto_cleaning_interval_days(u8 *interval_days);

/**
 * sps30_set_fan_auto_cleaning_interval_days() - convenience function to set the
 * current auto-cleaning interval in days
 *
 * @interval_days:  Value in days used to sets the auto-cleaning interval, 0 to
 *                  disable auto cleaning
 * Return:          0 on success, an error code otherwise
 */
s16 sps30_set_fan_auto_cleaning_interval_days(u8 interval_days);

/**
 * sps30_reset() - reset the SGP30
 *
 * The sensor reboots to the same state as before the reset but takes a few
 * seconds to resume measurements.
 *
 * Note that the interface-select configuration is reinterpreted, thus Pin 4
 * must be pulled to ground during the reset period for the sensor to remain in
 * i2c mode.
 *
 * Return:          0 on success, an error code otherwise
 */
s16 sps30_reset();

#ifdef __cplusplus
}
#endif

#endif /* SPS30_H */

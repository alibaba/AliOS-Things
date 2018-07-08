/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_SYSTEM_H__
#define __ESP_SYSTEM_H__

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup System_APIs System APIs
  * @brief System APIs
  */

/** @addtogroup System_APIs
  * @{
  */

typedef enum {
    REASON_DEFAULT_RST = 0,         /**< normal startup by power on */
        REASON_WDT_RST,             /**< hardware watch dog reset */
        REASON_EXCEPTION_RST,       /**< exception reset, GPIO status won't change */
        REASON_SOFT_WDT_RST,        /**< software watch dog reset, GPIO status won't change */
        REASON_SOFT_RESTART,        /**< software restart ,system_restart , GPIO status won't change */
        REASON_DEEP_SLEEP_AWAKE,    /**< wake up from deep-sleep */
        REASON_EXT_SYS_RST          /**< external system reset */
} rst_reason;

struct rst_info {
    rst_reason reason;  /**< enum rst_reason */
    uint32 exccause;
    uint32 epc1;
    uint32 epc2;
    uint32 epc3;
    uint32 excvaddr;
    uint32 depc;
    uint32 rtn_addr;
};

/**
  * @brief  Get the reason of restart.
  *
  * @param  null
  *
  * @return struct rst_info* : information of the system restart
  */
struct rst_info *system_get_rst_info(void);

/**
  * @brief  Get information of the SDK version.
  *
  * @param  null
  *
  * @return Information of the SDK version.
  */
const char *system_get_sdk_version(void);

/**
  * @brief  Reset to default settings.
  *
  *         Reset to default settings of the following APIs : wifi_station_set_auto_connect,
  *         wifi_set_phy_mode, wifi_softap_set_config related, wifi_station_set_config
  *         related, and wifi_set_opmode.
  *
  * @param  null
  *
  * @return null
  */
void system_restore(void);

/**
  * @brief  Restart system.
  *
  * @param  null
  *
  * @return null
  */
void system_restart(void);

/**
  * @brief     Set the chip to deep-sleep mode.
  *
  *            The device will automatically wake up after the deep-sleep time set
  *            by the users. Upon waking up, the device boots up from user_init.
  *
  * @attention 1. XPD_DCDC should be connected to EXT_RSTB through 0 ohm resistor
  *               in order to support deep-sleep wakeup.
  * @attention 2. system_deep_sleep(0): there is no wake up timer; in order to wake
  *               up, connect a GPIO to pin RST, the chip will wake up by a falling-edge
  *               on pin RST
  *
  * @param     uint32 time_in_us : deep-sleep time, unit: microsecond
  *
  * @return    null
  */
void system_deep_sleep(uint32 time_in_us);

/**
  * @brief  Call this API before system_deep_sleep to set the activity after the
  *         next deep-sleep wakeup.
  *
  *         If this API is not called, default to be system_deep_sleep_set_option(1).
  *
  * @param  uint8 option :
  * @param  0 : Radio calibration after the deep-sleep wakeup is decided by byte
  *             108 of esp_init_data_default.bin (0~127byte).
  * @param  1 : Radio calibration will be done after the deep-sleep wakeup. This
  *             will lead to stronger current.
  * @param  2 : Radio calibration will not be done after the deep-sleep wakeup.
  *             This will lead to weaker current.
  * @param  4 : Disable radio calibration after the deep-sleep wakeup (the same
  *             as modem-sleep). This will lead to the weakest current, but the device
  *             can't receive or transmit data after waking up.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_deep_sleep_set_option(uint8 option);

/**
  * @brief  Get system time, unit: microsecond.
  *
  * @param  null
  *
  * @return System time, unit: microsecond.
  */
uint32 system_get_time(void);

/**
  * @brief  Print the system memory distribution, including data/rodata/bss/heap.
  *
  * @param  null
  *
  * @return null
  */
void system_print_meminfo(void);

/**
  * @brief  Get the size of available heap.
  *
  * @param  null
  *
  * @return Available heap size.
  */
uint32 system_get_free_heap_size(void);

/**
  * @brief  Get the chip ID.
  *
  * @param  null
  *
  * @return The chip ID.
  */
uint32 system_get_chip_id(void);

/**
  * @brief     Get the RTC clock cycle.
  *
  * @attention 1. The RTC clock cycle has decimal part.
  * @attention 2. The RTC clock cycle will change according to the temperature,
  *               so RTC timer is not very precise.
  *
  * @param     null
  *
  * @return    RTC clock period (unit: microsecond), bit11~ bit0 are decimal.
  */
uint32 system_rtc_clock_cali_proc(void);

/**
  * @brief     Get RTC time, unit: RTC clock cycle.
  *
  * Example:
  *            If system_get_rtc_time returns 10 (it means 10 RTC cycles), and
  *            system_rtc_clock_cali_proc returns 5.75 (it means 5.75 microseconds per RTC clock cycle),
  *            (then the actual time is 10 x 5.75 = 57.5 microseconds.
  *
  * @attention System time will return to zero because of system_restart, but the
  *            RTC time still goes on. If the chip is reset by pin EXT_RST or pin
  *            CHIP_EN (including the deep-sleep wakeup), situations are shown as below:
  * @attention 1. reset by pin EXT_RST : RTC memory won't change, RTC timer returns to zero
  * @attention 2. watchdog reset : RTC memory won't change, RTC timer won't change
  * @attention 3. system_restart : RTC memory won't change, RTC timer won't change
  * @attention 4. power on : RTC memory is random value, RTC timer starts from zero
  * @attention 5. reset by pin CHIP_EN : RTC memory is random value, RTC timer starts from zero
  *
  * @param     null
  *
  * @return    RTC time.
  */
uint32 system_get_rtc_time(void);

/**
  * @brief     Read user data from the RTC memory.
  *
  *            The user data segment (512 bytes, as shown below) is used to store user data.
  *
  *             |<---- system data(256 bytes) ---->|<----------- user data(512 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when reading data
  *            at the beginning of the user data segment, src_addr will be 256/4 = 64,
  *            n will be data length.
  *
  * @param     uint8 src : source address of rtc memory, src_addr >= 64
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_read(uint8 src, void *dst, uint16 n);

/**
  * @brief     Write user data to  the RTC memory.
  *
  *            During deep-sleep, only RTC is working. So users can store their data
  *            in RTC memory if it is needed. The user data segment below (512 bytes)
  *            is used to store the user data.
  *
  *            |<---- system data(256 bytes) ---->|<----------- user data(512 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when storing data
  *            at the beginning of the user data segment, src_addr will be 256/4 = 64,
  *            n will be data length.
  *
  * @param     uint8 src : source address of rtc memory, src_addr >= 64
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_write(uint8 dst, const void *src, uint16 n);

/**
  * @brief  UART0 swap.
  *
  *         Use MTCK as UART0 RX, MTDO as UART0 TX, so ROM log will not output from
  *         this new UART0. We also need to use MTDO (U0RTS) and MTCK (U0CTS) as UART0 in hardware.
  *
  * @param  null
  *
  * @return null
  */
void system_uart_swap(void);

/**
  * @brief  Disable UART0 swap.
  *
  *         Use the original UART0, not MTCK and MTDO.
  *
  * @param  null
  *
  * @return null
  */
void system_uart_de_swap(void);

/**
  * @brief     Measure the input voltage of TOUT pin 6, unit : 1/1024 V.
  *
  * @attention 1. system_adc_read can only be called when the TOUT pin is connected
  *               to the external circuitry, and the TOUT pin input voltage should
  *               be limited to 0~1.0V.
  * @attention 2. When the TOUT pin is connected to the external circuitry, the 107th
  *               byte (vdd33_const) of esp_init_data_default.bin(0~127byte) should be
  *               set as the real power voltage of VDD3P3 pin 3 and 4.
  * @attention 3. The unit of vdd33_const is 0.1V, the effective value range is [18, 36];
  *               if vdd33_const is in [0, 18) or (36, 255), 3.3V is used to optimize RF by default.
  *
  * @param     null
  *
  * @return    Input voltage of TOUT pin 6, unit : 1/1024 V
  */
uint16 system_adc_read(void);

/**
  * @brief     Measure the power voltage of VDD3P3 pin 3 and 4, unit : 1/1024 V.
  *
  * @attention 1. system_get_vdd33 depends on RF, please do not use it if RF is disabled.
  * @attention 2. system_get_vdd33 can only be called when TOUT pin is suspended.
  * @attention 3. The 107th byte in esp_init_data_default.bin (0~127byte) is named
  *               as "vdd33_const", when TOUT pin is suspended vdd33_const must be
  *               set as 0xFF, that is 255.
  *
  * @param     null
  *
  * @return    Power voltage of VDD33, unit : 1/1024 V
  */
uint16 system_get_vdd33(void);

/**
  * @brief  Write data into flash with protection.
  *
  *         Flash read/write has to be 4-bytes aligned.
  *
  *         Protection of flash read/write :
  *             use 3 sectors (4KBytes per sector) to save  4KB data with protect,
  *             sector 0 and sector 1 are data sectors, back up each other,
  *             save data alternately, sector 2 is flag sector, point out which sector
  *             is keeping the latest data, sector 0 or sector 1.
  *
  * @param  uint16 start_sec : start sector (sector 0) of the 3 sectors which are
  *                            used for flash read/write protection.
  *    - For example, in IOT_Demo we can use the 3 sectors (3 * 4KB) starting from flash
  *      0x3D000 for flash read/write protection, so the parameter start_sec should be 0x3D
  * @param  void *param : pointer of the data to be written
  * @param  uint16 len : data length, should be less than a sector, which is 4 * 1024
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_param_save_with_protect(uint16 start_sec, void *param, uint16 len);

/**
  * @brief  Read the data saved into flash with the read/write protection.
  *
  *         Flash read/write has to be 4-bytes aligned.
  *
  *         Read/write protection of flash:
  *             use 3 sectors (4KB per sector) to save  4KB data with protect, sector
  *             0 and sector 1 are data sectors, back up each other, save data alternately,
  *             sector 2 is flag sector, point out which sector is keeping the latest data,
  *             sector 0 or sector 1.
  *
  * @param  uint16 start_sec : start sector (sector 0) of the 3 sectors used for
  *                            flash read/write protection. It cannot be sector 1 or sector 2.
  *    - For example, in IOT_Demo, the 3 sectors (3 * 4KB) starting from flash 0x3D000
  *      can be used for flash read/write protection.
  *      The parameter start_sec is 0x3D, and it cannot be 0x3E or 0x3F.
  * @param  uint16 offset : offset of data saved in sector
  * @param  void *param : data pointer
  * @param  uint16 len : data length, offset + len =< 4 * 1024
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_param_load(uint16 start_sec, uint16 offset, void *param, uint16 len);

/**
  * @brief  Set the maximum value of RF TX Power, unit : 0.25dBm.
  *
  * @param  uint8 max_tpw : the maximum value of RF Tx Power, unit : 0.25dBm, range [0, 82].
  *                         It can be set refer to the 34th byte (target_power_qdb_0)
  *                         of esp_init_data_default.bin(0~127byte)
  *
  * @return null
  */
void system_phy_set_max_tpw(uint8 max_tpw);

/**
  * @brief     Adjust the RF TX Power according to VDD33, unit : 1/1024 V.
  *
  * @attention 1. When TOUT pin is suspended, VDD33 can be measured by system_get_vdd33.
  * @attention 2. When TOUT pin is connected to the external circuitry, system_get_vdd33
  *               can not be used to measure VDD33.
  *
  * @param     uint16 vdd33 : VDD33, unit : 1/1024V, range [1900, 3300]
  *
  * @return    null
  */
void system_phy_set_tpw_via_vdd33(uint16 vdd33);

/**
  * @brief     Enable RF or not when wakeup from deep-sleep.
  *
  * @attention 1. This API can only be called in user_rf_pre_init.
  * @attention 2. Function of this API is similar to system_deep_sleep_set_option,
  *               if they are both called, it will disregard system_deep_sleep_set_option
  *               which is called before deep-sleep, and refer to system_phy_set_rfoption
  *               which is called when deep-sleep wake up.
  * @attention 3. Before calling this API, system_deep_sleep_set_option should be called
  *               once at least.
  *
  * @param     uint8 option :
  *    - 0 : Radio calibration after deep-sleep wake up depends on esp_init_data_default.bin (0~127byte) byte 108.
  *    - 1 : Radio calibration is done after deep-sleep wake up; this increases the
  *          current consumption.
  *    - 2 : No radio calibration after deep-sleep wake up; this reduces the current consumption.
  *    - 4 : Disable RF after deep-sleep wake up, just like modem sleep; this has the
  *          least current consumption; the device is not able to transmit or receive
  *          data after wake up.
  *
  * @return    null
  */
void system_phy_set_rfoption(uint8 option);

/** @addtogroup Upgrade_APIs
  * @{
  */

/**
  * @brief  Check the user bin.
  *
  * @param  null
  *
  * @return 0x00 : UPGRADE_FW_BIN1, i.e. user1.bin
  * @return 0x01 : UPGRADE_FW_BIN2, i.e. user2.bin
  */
uint8 system_upgrade_userbin_check(void);

/**
  * @brief  Reboot system to use the new software.
  *
  * @param  null
  *
  * @return null
  */
void system_upgrade_reboot(void);

/**
  * @brief  Check the upgrade status flag.
  *
  * @param  null
  *
  * @return #define UPGRADE_FLAG_IDLE      0x00
  * @return #define UPGRADE_FLAG_START     0x01
  * @return #define UPGRADE_FLAG_FINISH    0x02
  */
uint8 system_upgrade_flag_check();

/**
  * @brief     Set the upgrade status flag.
  *
  * @attention After downloading new softwares, set the flag to UPGRADE_FLAG_FINISH
  *            and call system_upgrade_reboot to reboot the system in order to run
  *            the new software.
  *
  * @param     uint8 flag:
  *    - UPGRADE_FLAG_IDLE      0x00
  *    - UPGRADE_FLAG_START     0x01
  *    - UPGRADE_FLAG_FINISH    0x02
  *
  * @return    null
  */
void system_upgrade_flag_set(uint8 flag);

/**
  * @}
  */

/** \defgroup System_boot_APIs Boot APIs
  * @brief boot APIs
  */

/** @addtogroup System_boot_APIs
  * @{
  */

#define SYS_BOOT_ENHANCE_MODE   0   /**<  It can load and run firmware at any address, for Espressif factory test bin*/
#define SYS_BOOT_NORMAL_MODE    1   /**<  It can only load and run at some addresses of user1.bin (or user2.bin)*/

#define SYS_BOOT_NORMAL_BIN     0   /**<  user1.bin or user2.bin*/
#define SYS_BOOT_TEST_BIN       1   /**<  can only be Espressif test bin*/

/**
  * @brief     Get information of the boot version.
  *
  * @attention If boot version >= 1.3 , users can enable the enhanced boot mode
  *            (refer to system_restart_enhance).
  *
  * @param     null
  *
  * @return    Information of the boot version.
  */
uint8 system_get_boot_version(void);

/**
  * @brief  Get the address of the current running user bin (user1.bin or user2.bin).
  *
  * @param  null
  *
  * @return The address of the current running user bin.
  */
uint32 system_get_userbin_addr(void);

/**
  * @brief  Get the boot mode.
  *
  * @param  null
  *
  * @return #define SYS_BOOT_ENHANCE_MODE   0
  * @return #define SYS_BOOT_NORMAL_MODE    1
  */
uint8 system_get_boot_mode(void);

/**
  * @brief     Restarts the system, and enters the enhanced boot mode.
  *
  * @attention SYS_BOOT_TEST_BIN is used for factory test during production; users
  *            can apply for the test bin from Espressif Systems.
  *
  * @param     uint8 bin_type : type of bin
  *    - #define SYS_BOOT_NORMAL_BIN      0   // user1.bin or user2.bin
  *    - #define SYS_BOOT_TEST_BIN        1   // can only be Espressif test bin
  * @param     uint32 bin_addr : starting address of the bin file
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_restart_enhance(uint8 bin_type, uint32 bin_addr);

typedef enum {
    FLASH_SIZE_4M_MAP_256_256 = 0,  /**<  Flash size : 4Mbits. Map : 256KBytes + 256KBytes */
    FLASH_SIZE_2M,                  /**<  Flash size : 2Mbits. Map : 256KBytes */
    FLASH_SIZE_8M_MAP_512_512,      /**<  Flash size : 8Mbits. Map : 512KBytes + 512KBytes */
    FLASH_SIZE_16M_MAP_512_512,     /**<  Flash size : 16Mbits. Map : 512KBytes + 512KBytes */
    FLASH_SIZE_32M_MAP_512_512,     /**<  Flash size : 32Mbits. Map : 512KBytes + 512KBytes */
    FLASH_SIZE_16M_MAP_1024_1024,   /**<  Flash size : 16Mbits. Map : 1024KBytes + 1024KBytes */
    FLASH_SIZE_32M_MAP_1024_1024,    /**<  Flash size : 32Mbits. Map : 1024KBytes + 1024KBytes */
    FLASH_SIZE_32M_MAP_2048_2048,    /**<  attention: don't support now ,just compatible for nodemcu;
                                           Flash size : 32Mbits. Map : 2048KBytes + 2048KBytes */
    FLASH_SIZE_64M_MAP_1024_1024,     /**<  Flash size : 64Mbits. Map : 1024KBytes + 1024KBytes */
    FLASH_SIZE_128M_MAP_1024_1024     /**<  Flash size : 128Mbits. Map : 1024KBytes + 1024KBytes */

} flash_size_map;

/**
  * @brief  Get the current Flash size and Flash map.
  *
  *         Flash map depends on the selection when compiling, more details in document
  *         "2A-ESP8266__IOT_SDK_User_Manual"
  *
  * @param  null
  *
  * @return enum flash_size_map
  */
flash_size_map system_get_flash_size_map(void);

#define SYS_CPU_80MHZ   80
#define SYS_CPU_160MHZ  160

/**
  * @brief  Set CPU frequency. Default is 80MHz.
  *
  *         System bus frequency is 80MHz, will not be affected by CPU frequency.
  *         The frequency of UART, SPI, or other peripheral devices, are divided
  *         from system bus frequency, so they will not be affected by CPU frequency either.
  *
  * @param  uint8 freq : CPU frequency, 80 or 160.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_update_cpu_freq(uint8 freq);

/**
  * @brief  Get CPU frequency.
  *
  * @param  null
  *
  * @return CPU frequency, unit : MHz.
  */
uint8 system_get_cpu_freq(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

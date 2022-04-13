#ifndef __CLI_CMD_LIST_H__
#define __CLI_CMD_LIST_H__

#include "cli_cmd_functions.h"

typedef int (*syscall_func)(int argc, char **argv);
/* system call table */
struct finsh_syscall
{
    const char*     name;       /* the name of system call */
    const char*     desc;       /* description of system call */
    syscall_func func;      /* the function address of system call */
};

#define RT_USED     __attribute__((used))
#define SECTION(x)  __attribute__((section(x)))


#define AOS_VENDOR_CLI_CMD(name, cmd, desc)                      \
                const char __fsym_##cmd##_name[] SECTION(".rodata") = #cmd;    \
                const char __fsym_##cmd##_desc[] SECTION(".rodata") = #desc;   \
                static void name##_stub(char *buf, int len, int argc, char **argv) { \
                    name(argc, argv); \
				} \
                RT_USED const struct finsh_syscall __fsym_##cmd SECTION("UsrCmdSymTab") = \
                {                           \
                    __fsym_##cmd##_name,    \
                    __fsym_##cmd##_desc,    \
                    (syscall_func)&name##_stub \
                };


AOS_VENDOR_CLI_CMD(ali_factory_mode, factory, ali-factory test)

AOS_VENDOR_CLI_CMD(ali_bt_mp_stop, bt_mp_stop, Console bt stop mp mode Command)

AOS_VENDOR_CLI_CMD(ali_bt_mp_start, bt_mp_start,  Console bt start mp mode Command)

AOS_VENDOR_CLI_CMD(ali_wifi_iwpriv, iwpriv, Console Wifi iwpriv Command)

AOS_VENDOR_CLI_CMD(ali_wifi_besphy, besphy, Console Wifi RF test Command)

AOS_VENDOR_CLI_CMD(ali_wifi_bt_freq_offset,besfreq, Console debug freq offset)

AOS_VENDOR_CLI_CMD(ali_wifi_get_power_list, power_list, get power list)

AOS_VENDOR_CLI_CMD(ali_wifi_power_config, power, set/get power level)

//AOS_VENDOR_CLI_CMD(ali_wifi_tx_stat_config, tx_stat, enable/disable wifi tx statistics)

AOS_VENDOR_CLI_CMD(ali_wifi_rw_data, reg, read and write reg for debug)

AOS_VENDOR_CLI_CMD(ali_wdt_disable, wdt_disable, disable wdt)

AOS_VENDOR_CLI_CMD(ali_thermal_get_temp, ths_gt, thermal get temp)

#ifdef __A7_DSP_ENABLE__
//AOS_VENDOR_CLI_CMD(ali_a7_dsp_reboot, a7_dsp_reboot, reboot a7 dsp)

//AOS_VENDOR_CLI_CMD(ali_dsp_cmd, dsp_cmd, dsp cmd functions)

#ifdef CONFIG_GENIE_DEBUG
//AOS_VENDOR_CLI_CMD(ali_mic, mic, wakeup feed test)
#endif
#endif

//AOS_VENDOR_CLI_CMD(ali_mixer, mixer, mixer test)

//AOS_VENDOR_CLI_CMD(ali_uart2, uart2, uart2: tgdb swd eq_pc_cmd a7_trace ble_nosignal);

AOS_VENDOR_CLI_CMD(ali_stat_print, statprint, print key statistics)

//AOS_VENDOR_CLI_CMD(ali_netmgr, netmgr, netmgr [start|clear|stats|connect ssid password])

//AOS_VENDOR_CLI_CMD(ali_autotest_client, autotest_client, autotest-client cmd run)

AOS_VENDOR_CLI_CMD(ali_gpio_set, gpio_set, gpio set <gpio_index> <0..1>)

AOS_VENDOR_CLI_CMD(ali_gpio_get, gpio_get, gpio get <gpio_index> <0..1>)

//AOS_VENDOR_CLI_CMD(ali_secboot_hash_set, secboot_hash_set, secboot hash set)

AOS_VENDOR_CLI_CMD(ali_bt_bdr_tx_signal, bt_bdr_tx_signal, bt BDR Tx signal)

AOS_VENDOR_CLI_CMD(ali_bt_edr_tx_signal, bt_edr_tx_signal, bt EDR Tx signal)

AOS_VENDOR_CLI_CMD(ali_bt_bdr_rx_signal, bt_bdr_rx_signal, bt BDR Rx signal)

AOS_VENDOR_CLI_CMD(ali_bt_edr_rx_signal, bt_edr_rx_signal, bt EDR Rx signal)

AOS_VENDOR_CLI_CMD(ali_bt_stop_tx_signal, bt_stop_tx_signal, stop Tx signal)

AOS_VENDOR_CLI_CMD(ali_bt_stop_rx_signal, bt_stop_rx_signal, stop Rx signal)

AOS_VENDOR_CLI_CMD(ali_bt_le_transmitter_test, bt_le_transmitter_test, le transmitter test)

AOS_VENDOR_CLI_CMD(ali_bt_le_receiver_test, bt_le_receiver_test, le receiver test)

AOS_VENDOR_CLI_CMD(ali_bt_le_test_end, bt_le_test_end, le test end)

AOS_VENDOR_CLI_CMD(ali_bt_set_bdr_power, bt_set_bdr_power, set bdr power)

AOS_VENDOR_CLI_CMD(ali_bt_set_edr_power, bt_set_edr_power, set edr power)

AOS_VENDOR_CLI_CMD(ali_bt_set_ble_power, bt_set_ble_power, set ble power)

AOS_VENDOR_CLI_CMD(ali_bt_set_mac, bt_set_mac, set bt mac)

AOS_VENDOR_CLI_CMD(ali_bt_read_mac, bt_read_mac, read bt mac)

AOS_VENDOR_CLI_CMD(ali_bt_hci_reset, bt_hci_reset, hci reset)

AOS_VENDOR_CLI_CMD(ali_bt_carrier_start, bt_carrier_start, bt carrier start)

AOS_VENDOR_CLI_CMD(ali_bt_carrier_stop, bt_carrier_stop, bt carrier stop)

AOS_VENDOR_CLI_CMD(ali_bt_write_reg32, bt_write_reg32, write bt reg)

AOS_VENDOR_CLI_CMD(ali_bt_read_reg32, bt_read_reg32, read bt reg)

AOS_VENDOR_CLI_CMD(ali_i2c_debug, i2c_debug, uart change to i2c)

AOS_VENDOR_CLI_CMD(ali_bt_enter_signal_mode, bt_enter_signal_mode, enter signal mode)

AOS_VENDOR_CLI_CMD(ali_set_tport, set_tport, set toprt)

AOS_VENDOR_CLI_CMD(ali_bt_write_reg16, bt_write_reg16, write bt reg)

AOS_VENDOR_CLI_CMD(ali_bt_read_reg16, bt_read_reg16, read bt reg)

AOS_VENDOR_CLI_CMD(ali_wifi_write_reg16, wifi_write_reg16, write wifi reg)

AOS_VENDOR_CLI_CMD(ali_wifi_read_reg16, wifi_read_reg16, read wifi reg)

AOS_VENDOR_CLI_CMD(ali_ble_nosignal_start, ble_nosignal_start, ble nosignal test)

AOS_VENDOR_CLI_CMD(ali_ble_nosignal_stop, ble_nosignal_stop, ble nosignal test)

AOS_VENDOR_CLI_CMD(ali_set_epta_para, set_epta_para, set epta parameter)

AOS_VENDOR_CLI_CMD(ali_read_bdr_power, read_bdr_power, read bdr power)

AOS_VENDOR_CLI_CMD(ali_read_edr_power, read_edr_power, read edr power)

AOS_VENDOR_CLI_CMD(ali_read_ble_power, read_ble_power, read ble power)

//AOS_VENDOR_CLI_CMD(ali_cli_mode, cli_mode, cli mode set)

AOS_VENDOR_CLI_CMD(ali_cli_reg_read, reg_read, read 32bit reg)

AOS_VENDOR_CLI_CMD(ali_cli_reg_write, reg_write, write 32bit reg)

AOS_VENDOR_CLI_CMD(ali_ota_cmd, ota, operate ota section )

//AOS_VENDOR_CLI_CMD(ali_flash_rw_check, flash_rw_check, check flash read/write data )

AOS_VENDOR_CLI_CMD(ali_flash_read_loop, read_loop, read_loop data )

AOS_VENDOR_CLI_CMD(ali_bt_scan_control, bt_scan,control bt scan )

//AOS_VENDOR_CLI_CMD(ali_wakeup_response, wakeup_response,control wakeup response )

//AOS_VENDOR_CLI_CMD(ali_audio_tuning_cfg, audio_tuning_cfg, show audio tuning cfg )

AOS_VENDOR_CLI_CMD(ali_wifi_test_tx, wifi_tx, set wifi continue tx test)

//AOS_VENDOR_CLI_CMD(ali_audio_process_bypass_eq, audio_process_bypass_EQ, audio_process_bypass_EQ <on/off> bypass EQ filter and DRC process but limiter process still valid )

//AOS_VENDOR_CLI_CMD(ali_ultrasound_test, ultrasound_test, ultrasound_test <start/stop> will play 20khz music meanwhile play music )

//AOS_VENDOR_CLI_CMD(ali_pwm_test, pwm_breath, pwm_breath <channel> <breath_times> )

//AOS_VENDOR_CLI_CMD(ali_pwm_led, pwm_led, pwm_led <channel> <lightness> )

//AOS_VENDOR_CLI_CMD(ali_console_trace, console_trace, console_trace <open/close>)

//AOS_VENDOR_CLI_CMD(ali_cpuusage_show, cpuusage_show, cpuusage_show <open/close>)

#endif /*__CLI_CMD_LIST_H__*/
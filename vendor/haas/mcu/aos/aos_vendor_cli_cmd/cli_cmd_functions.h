#ifndef __CLI_CMD_FUNCTIONS_H__
#define __CLI_CMD_FUNCTIONS_H__

void ali_factory_mode(int argc, char **argv);

void ali_volume(int argc, char **argv);

void ali_tmall_play(int argc, char **argv);

void ali_rsa_verify(int argc, char **argv);

void ali_start_remote_debugger(int argc, char **argv);

void ali_ulog_encode_fs(int argc, char **argv);

void ali_watchdog_debug(int argc, char **argv);

void ali_spinor_test(int argc, char **argv);

void ali_audiocodec_dump_reg(int argc, char **argv);

void ali_firmware_set_env(int argc, char **argv);

void ali_firmware_print_env(int argc, char **argv);

void ali_dump_nor_part(int argc, char **argv);

void ali_cat_file(int argc, char **argv);

void ali_copy_file(int argc, char **argv);

void ali_df_cmd(int argc, char **argv);

void ali_hexdump_c(int argc, char **argv);

void ali_hexdump(int argc, char **argv);

void ali_list_all(int argc, char **argv);

void ali_list(int argc, char **argv);

void ali_make_directory(int argc, char **argv);

void ali_move_file(int argc, char **argv);

void ali_remove_file(int argc, char **argv);

void ali_rwcheck(int argc, char **argv);

void ali_rw_dev(int argc, char **argv);

void ali_rw_speed(int argc, char **argv);

void ali_touch(int argc, char **argv);

void ali_sona_checksum(int argc, char **argv);

void ali_check_toc1(int argc, char **argv);

void ali_upgrade_app(int argc, char **argv);

void ali_bt_mp_stop(int argc, char **argv);

void ali_bt_mp_start(int argc, char **argv);

void ali_bt_fifo_all(int argc, char **argv);

void ali_bt_fifo(int argc, char **argv);

void ali_bt_bqb(int argc, char **argv);

void ali_bt_avrcp(int argc, char **argv);

void ali_bt_test(int argc, char **argv);

void ali_bt_mp_test(int argc, char **argv);

void ali_bt_debug(int argc, char **argv);

void ali_bt_off(int argc, char **argv);

void ali_bt_on(int argc, char **argv);

void ali_uart3(int argc, char **argv);

void ali_close_test_cmw500(int argc, char **argv);

void ali_open_test_cmw500(int argc, char **argv);

void ali_bt_colog(int argc, char **argv);

void ali_wifi_debug(int argc, char **argv);

void ali_wifi_iwpriv(int argc, char **argv);

void ali_wifi_besphy(int argc, char **argv);

void ali_wifi_bt_freq_offset(int argc, char **argv);

void ali_wifi_get_power_list(int argc, char** argv);

void ali_wifi_rw_data(int argc, char** argv);

void ali_wifi_reoder_scan(int argc, char **argv);

void ali_wifi_scan(int argc, char **argv);

void ali_wifi_scan_with_ssid(int argc, char **argv);

void ali_wifi_long_time(int argc, char **argv);

void ali_wifi_off(int argc, char **argv);

void ali_wifi_on(int argc, char **argv);

void ali_wifi_info(int argc, char **argv);

void ali_wifi_promisc(int argc, char **argv);

void ali_dhcp_start(int argc, char **argv);

void ali_wifi_connect(int argc, char **argv);

void ali_wifi_io_debug(int argc, char **argv);

void ali_wifi_test(int argc, char **argv);

void ali_tcpip(int argc, char **argv);

void ali_ifconfig(int argc, char **argv);

void ali_bt_reset(int argc, char **argv);

void ali_wifi_reset(int argc, char **argv);

void ali_thermal_get_temp(int argc, char **argv);

void ali_efuse_write(int argc, char **argv);

void ali_efuse_read(int argc, char **argv);

void ali_adbd_service(int argc, char **argv);

void ali_adbd_forward(int argc, char **argv);

void ali_snd_adb(int argc, char **argv);

void ali_iperf(int argc, char **argv);

void ali_ping(int argc, char **argv);

void ali_sendfile(int argc, char **argv);

void ali_dns(int argc, char **argv);

void ali_arp(int argc, char **argv);

void ali_lsfd(int argc, char **argv);

void ali_pktprint(int argc, char **argv);

void ali_network_config(int argc, char **argv);

void ali_play(int argc, char **argv);

void ali_record(int argc, char **argv);

void ali_mixer(int argc, char **argv);

void ali_uart2(int argc, char **argv);

void ali_dsp_cmd(int argc, char **argv);

void ali_stat_print(int argc, char** argv);

void ali_audiodump(int argc, char **argv);

void ali_agrecord(int argc, char **argv);

void ali_mic(int argc, char **argv);

void ali_netmgr(int argc, char **argv);

void ali_autotest_client(int argc, char **argv);

void ali_gpio_set(int argc, char **argv);

void ali_gpio_get(int argc, char **argv);

void ali_secboot_hash_set(int argc, char **argv);

void ali_bt_bdr_tx_signal(int argc, char **argv);

void ali_bt_edr_tx_signal(int argc, char **argv);

void ali_bt_bdr_rx_signal(int argc, char **argv);

void ali_bt_edr_rx_signal(int argc, char **argv);

void ali_bt_stop_tx_signal(int argc, char **argv);

void ali_bt_stop_rx_signal(int argc, char **argv);

void ali_bt_le_transmitter_test(int argc, char **argv);

void ali_bt_le_receiver_test(int argc, char **argv);

void ali_bt_le_test_end(int argc, char **argv);

void ali_bt_set_bdr_power(int argc, char **argv);

void ali_bt_set_edr_power(int argc, char **argv);

void ali_bt_set_ble_power(int argc, char **argv);

void ali_bt_set_mac(int argc, char **argv);

void ali_bt_read_mac(int argc, char **argv);

void ali_bt_hci_reset(int argc, char **argv);

void ali_bt_carrier_start(int argc, char **argv);

void ali_bt_carrier_stop(int argc, char **argv);

void ali_bt_write_reg32(int argc, char **argv);

void ali_bt_read_reg32(int argc, char **argv);

void ali_i2c_debug(int argc, char **argv);

void ali_bt_enter_signal_mode(int argc, char **argv);

void ali_set_tport(int argc, char **argv);

void ali_bt_write_reg16(int argc, char **argv);

void ali_bt_read_reg16(int argc, char **argv);

void ali_wifi_write_reg16(int argc, char **argv);

void ali_wifi_read_reg16(int argc, char **argv);

void ali_ble_nosignal_start(int argc, char **argv);

void ali_ble_nosignal_stop(int argc, char **argv);

void ali_set_epta_para(int argc, char **argv);

void ali_read_bdr_power(int argc, char **argv);

void ali_read_edr_power(int argc, char **argv);

void ali_read_ble_power(int argc, char **argv);

void ali_cli_mode(int argc, char **argv);

void ali_ota_cmd(int argc, char **argv);

void ali_flash_rw_check(int argc, char **argv);

void ali_flash_read_loop(int argc, char **argv);

void ali_bt_scan_control(int argc, char **argv);

void ali_wakeup_response(int argc, char **argv);

void ali_audio_tuning_cfg(int argc, char **argv);

void ali_wifi_test_tx(int argc, char **argv);

void ali_audio_process_bypass_eq(int argc, char **argv);

void ali_ultrasound_test(int argc, char ** argv);

void ali_pwm_test(int argc, char **argv);

void ali_console_trace(int argc, char **argv);
#endif /*__CLI_CMD_FUNCTIONS_H__*/
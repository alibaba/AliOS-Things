#ifndef __BESPHY_FUNCTIONS_H__
#define __BESPHY_FUNCTIONS_H__

void besphy_rf_usage();

int besphy_rf_dpd_rw_mem_loop();

int besphy_rf_tx_dc_tone();

int besphy_rf_dpd_check();

int besphy_rf_dpd_resume();

int besphy_rf_start();

int besphy_rf_tx_start();

int besphy_rf_channel(int channel);

int besphy_rf_get_channel();

int besphy_rf_set_band(char* band);

int besphy_rf_txpower(int power);

int besphy_rf_get_txpower();

int besphy_rf_rate(int rate);

int besphy_rf_get_rate();

int besphy_rf_tx_stop();

int besphy_rf_freq_offset(int xcap);

int besphy_rf_get_freq_offset();

int besphy_rf_reset_stats();

int besphy_rf_rx_start();

int besphy_rf_rx_ackset();

int besphy_rf_rx_ackread();

int besphy_rf_rx_stop();

int besphy_rf_data_save_to_flash();

int besphy_rf_get_power_from_flash();

int besphy_rf_get_temp();

int besphy_rf_save_temp();

int besphy_rf_get_save_temp();

int besphy_rf_get_save_temp();

//bool besphy_rf_status();

int besphy_rf_save_freq_offset();

int besphy_rf_get_save_freq_offset();

int besphy_rf_save_all_ch_power();

int besphy_rf_get_all_save_ch_power();

int besphy_rf_tx_tone();

int besphy_rf_get_power_list_from_reg();

int besphy_rf_mem_read(char** argv);

int besphy_rf_mem_write(char** argv);

int besphy_rf_set_default_power();

int besphy_rf_set_tx_interval(int value);

int besphy_rf_get_tx_interval(void);

int besphy_rf_open_temp_calibration();

int besphy_rf_wifi_reset();

int besphy_rf_debug_thoughtput_status();

int besphy_rf_set_tx_power_config(int* tx_power);

int besphy_rf_agc_select(unsigned char id, unsigned char bandWidth_limit);

int besphy_rf_green_mix_switch(unsigned char mode);

int system_temp_log_disable();

int system_temp_log_enable();
//int besphy_adjust_power_by_rssi();
#endif /*__BESPHY_FUNCTIONS_H__*/

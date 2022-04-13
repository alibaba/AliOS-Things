#include "cli_api.h"
#include "besphy_functions.h"
#include "apps.h"
#include "hal_cmu.h"

extern int wifi_rf_test_enable(char* band);
extern int wifi_rf_test_mode(int testType);
extern void wifi_rf_set_tx_stop(void);
extern int wifi_rf_test_set_channel(int channel);
extern int wifi_rf_test_get_channel( void );
extern int wifi_rf_test_set_tx_interval(int interval);
extern int wifi_rf_test_get_tx_interval();
extern int wifi_rf_test_set_frame_length(int frameLength);
extern int wifi_rf_test_get_frame_length( );
extern int wifi_rf_test_set_rate(int rateIndx);
extern int wifi_rf_test_get_rate( void );
extern int wifi_rf_test_calibrate_txPower(int powerlevel);
extern int wifi_rf_test_get_txPower( void );
extern int wifi_rf_test_calibrate_ClockfreqOffset(  unsigned short freqOffset);
extern int wifi_rf_test_get_ClockfreqOffset( void );
extern int wifi_rf_test_power_save();
extern int wifi_rf_test_get_power_table_from_flash();

extern int wifi_rf_test_get_temp();

extern int wifi_rf_test_get_save_temp();
extern int wifi_rf_test_save_temp();

extern void wifi_rf_rx_ack_statistic(void);
extern void wifi_rf_set_rx_ack_statistic(void);

extern void app_factory_enter_wifi_nosignaltest_mode(void);
extern void app_factory_exit_wifi_nosignaltest_mode(void);

extern int wifi_rf_test_save_clock_freq();
extern int wifi_rf_test_get_save_clock_freq();
extern int wifi_rf_test_get_all_ch_power_table_from_flash();
extern int wifi_rf_test_power_all_ch_save();
extern int wifi_rf_set_default_power();
extern int wifi_rf_test_set_tx_interval(int interval);
extern int wifi_rf_test_get_tx_interval();
extern int wifi_rf_temp_calibration();
extern int wifi_rf_reg_dump();
extern void wifi_rf_green_field_mode(unsigned char mode);
extern void wifi_rf_to_bt();
extern void wifi_rf_temp_calibration_open();
extern void wifi_rf_temp_calibration_close();
extern int wifi_rf_rx_stop();
extern int wifi_rf_set_tx_power_config(int* tx_power);
extern int wifi_rf_rssi_task(void);
extern int wifi_rf_agc_select(unsigned char id,unsigned char bandWidth_limit);

extern int config_dpd_just_check_mem(uint32_t block);
extern int config_dpd_check_mem(uint32_t block);
extern int config_dpd_write_read_mem_loop();

extern int wifi_temp_log_disable();
extern int wifi_temp_log_enable();

static bool enter_rf_flags = 0;

int system_temp_log_disable()
{
    cli_printf("%s",__func__);
    wifi_temp_log_disable();
    return 0;
}

int system_temp_log_enable()
{
    cli_printf("%s",__func__);
    wifi_temp_log_enable();
    return 0;
}

int besphy_rf_dpd_rw_mem_loop()
{
    //config_dpd_write_read_mem_loop();
    return 0;
}

int besphy_rf_dpd_resume()
{
    config_dpd_check_mem(0);
    return 0;
}

int besphy_rf_dpd_check()
{
    config_dpd_just_check_mem(0);
    return 0;
}

int besphy_rf_agc_select(unsigned char id, unsigned char bandWidth_limit)
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_agc_select(id , bandWidth_limit);
    return 0;
}

int besphy_rf_debug_thoughtput_status()
{
    cli_printf("%s ...\n",__func__);
    //wifi_rf_reg_status_debug_throughput();
    wifi_rf_test_mode(0xf6);
    return 0;
}

int besphy_rf_set_tx_power_config(int* tx_power)
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_set_tx_power_config(tx_power);
    return 0;
}

#if 0
int besphy_adjust_power_by_rssi()
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_rssi_task();
    return 0;
}
#endif

void besphy_rf_usage()
{
    cli_printf("%s ...\n",__func__);
}

int besphy_rf_start()
{
    cli_printf("%s ...\n",__func__);
    app_factory_enter_wifi_nosignaltest_mode();
    hal_cmu_sys_reboot();
    return 0;
}

int besphy_rf_tx_start()
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_test_mode(0x02);
    wifi_rf_test_set_tx_interval(1);
    return 0;
}

int besphy_rf_channel(int channel)
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_test_set_channel(channel);
    return 0;
}

int besphy_rf_get_channel()
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_test_get_channel();
    return 0;
}

int besphy_rf_set_band(char* band)
{
    cli_printf("%s ...\n",__func__);
    wifi_rf_test_enable(band);
    return 0;
}

int besphy_rf_txpower(int power)
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_calibrate_txPower(power);
   return 0;
}

int besphy_rf_get_txpower()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_get_txPower();
   return 0;
}

int besphy_rf_rate(int rate)
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_set_rate(rate);
   return 0;
}

int besphy_rf_get_rate()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_get_rate();
   return 0;
}

int besphy_rf_tx_stop()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_set_tx_stop();
   return 0;
}

int besphy_rf_freq_offset(int xcap)
{
   cli_printf("%s ... freqOffset = %d \n",__func__,xcap);
   wifi_rf_test_calibrate_ClockfreqOffset((unsigned short)xcap);
   return 0;
}

int besphy_rf_get_freq_offset()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_get_ClockfreqOffset();
   return 0;
}

int besphy_rf_reset_stats()
{
   cli_printf("%s ...\n",__func__);
   return 0;
}

int besphy_rf_rx_start()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_mode(0x03);
   return 0;
}

int besphy_rf_tx_dc_tone()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_mode(0xF7);
   return 0;
}

int besphy_rf_tx_tone()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_mode(0xF2);
   return 0;
}

int besphy_rf_tx_ctn()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_mode(0xF3);
   return 0;
}


int besphy_rf_rx_ackset()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_set_rx_ack_statistic();
   return 0;
}

int besphy_rf_rx_ackread()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_rx_ack_statistic();
   return 0;
}

int besphy_rf_rx_stop()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_rx_stop();
   return 0;
}

int besphy_rf_data_save_to_flash()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_power_save();
   return 0;
}

int besphy_rf_get_power_from_flash()
{
	cli_printf("%s ...\n",__func__);
	wifi_rf_test_get_power_table_from_flash();
	return 0;
}

int besphy_rf_get_temp()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_get_temp();
   return 0;
}

int besphy_rf_save_temp()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_save_temp();
   return 0;
}

int besphy_rf_get_save_temp()
{
   cli_printf("%s ...\n",__func__);
   wifi_rf_test_get_save_temp();
   return 0;
}

bool besphy_rf_status()
{
   cli_printf("%s ...\n",__func__);
   enter_rf_flags =  app_enter_factory_wifi_test();
   return enter_rf_flags;
}

int besphy_rf_save_freq_offset()
{
	cli_printf("%s ...\n",__func__);
	wifi_rf_test_save_clock_freq();
	return 0;
}

int besphy_rf_get_save_freq_offset()
{
	cli_printf("%s ...\n",__func__);
	wifi_rf_test_get_save_clock_freq();
	return 0;
}


int besphy_rf_save_all_ch_power()
{
	cli_printf("%s ...\n",__func__);
	wifi_rf_test_power_all_ch_save();
	return 0;
}

int besphy_rf_get_all_save_ch_power()
{
	cli_printf("%s ...\n",__func__);
	wifi_rf_test_get_all_ch_power_table_from_flash();
	return 0;
}

int besphy_rf_get_power_list_from_reg()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_power_list();
    return 0;
}

int besphy_rf_mem_read(char** argv)
{
    cli_printf("%s ... \n", __func__);
    wifi_rf_mem_read(argv);
    return 0;
}

int besphy_rf_mem_write(char** argv)
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_mem_write(argv);
    return 0;
}

int besphy_rf_set_default_power()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_set_default_power();
    return 0;
}

int besphy_rf_set_tx_interval(int value)
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_test_set_tx_interval(value);
    return 0;
}

int besphy_rf_get_tx_interval()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_test_get_tx_interval();
    return 0;
}

int besphy_rf_open_temp_calibration()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_temp_calibration();
    return 0;
}

int besphy_rf_dump()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_reg_dump();
    return 0;
}
int besphy_rf_switch_to_bt()
{
    cli_printf("%s ... \n",__func__);
    besphy_rf_rx_stop();
    wifi_rf_to_bt();
    return 0;
}
int besphy_rf_green_mix_switch(unsigned char mode)
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_green_field_mode(mode);
    return 0;
}

int besphy_rf_temp_calibration_open()
{
    cli_printf("%s ... \n",__func__);
    wifi_rf_temp_calibration_open();
    return 0;
}

int besphy_rf_temp_calibration_close()
{
    cli_printf("%s ... \n");
    wifi_rf_temp_calibration_close();
    return 0;
}
int besphy_rf_wifi_reset()
{
    cli_printf("%s ...\n");
    wifi_rf_test_mode(0xff);
    return 0;
}

#ifndef __FACTORY_FUNCTIONS_H__
#define __FACTORY_FUNCTIONS_H__

int ate_enter_factory();

int ate_get_version();

int ate_set_pcb_sn(unsigned char* sn);

int ate_get_pcb_sn();

int ate_set_wmac(unsigned char* wmac);

int ate_get_wmac();

int ate_set_btmac(unsigned char *btmac);

int ate_get_btmac();

int ate_set_bootkey(unsigned char * bootKey);

int ate_get_bootkey();

int ate_set_secretKey_md5(unsigned char *secretKey,unsigned char *md5);

int ate_get_secretKey();

int ate_set_odm_name(unsigned char *name);

int ate_get_rom();

int ate_get_ram();

int ate_get_nand_model();

int ate_get_record_start(unsigned long seconds);

int ate_led_all_on();

int ate_led_all_off();

int ate_set_vol(long vol);

int ate_mic_quality_test();

int ate_play_file(unsigned char *path);

int ate_get_als_value();

int ate_set_light_calibration(int on_off,int value);

int ate_get_light_calibration(int on_off);

int ate_get_hw_id();

int ate_get_station_test_result();

int ate_get_max_temp();

int ate_set_sn(unsigned char *sn);

int ate_get_sn();

int ate_enter_user();

int ate_get_flash_uid();

void factory_usage();
#endif /*__FACTORY_FUNCTIONS_H__*/

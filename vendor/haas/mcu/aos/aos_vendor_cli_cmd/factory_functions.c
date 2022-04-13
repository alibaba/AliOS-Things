#include "factory_functions.h"
#include "tg_light.h"
#include "tg_factory.h"
#include "tg_audiosystem.h"
#include "cli_api.h"
#include "net_debug.h"


int ate_enter_factory()
{
    int ret = 0;
    //todo:stop smartbox service
    //ret = stop_smartbox_service();
    if(ret >= 0)
        cli_printf("stop smartbox service\n");
	//todo:post_stop smartbox service
    //ret = post_stop_smartbox_service();
    if(ret >= 0)
        cli_printf("post_stop smartbox service\n");
    //sh: you need to specify whom to kill
    return ret;
}

int ate_set_pcb_sn(unsigned char* sn)
{
    int ret = 0;
    //todo:secure storage read private_message
    //ret = sec_storage_read_priv_msg();
    if(ret >= 0)
        cli_printf("secure storage read private_message done\n");
	//todo:secure object name[private_message] already in device
    //ret = write_secure_storage();
    if(ret >= 0)
        cli_printf("write secure storage ok\n");

    return ret;
}

int ate_get_version()
{
    int ret = 0;


    return ret;
}

int ate_get_pcb_sn()
{
    int ret = 0;
	uint8_t sn[20] = {0};
    //todo:get serial number
    //ret = get_pcb_sn(sn);
    if(ret >= 0)
        cli_printf("PCB SN:%s\n",sn);

    return ret;
}

int ate_set_wmac(unsigned char* wmac)
{
    int ret = 0;
    cli_printf("set wifi mac %s\n", wmac);
    ret = tg_wifi_set_mac_address(wmac);
    return ret;
}

int ate_get_wmac()
{
    int ret = 0;
    uint8_t wmac[20] = {0};
    ret = tg_wifi_get_mac_address(wmac, 20);
    if (ret == 0)
        cli_printf("get wifi mac %s\n", wmac);
    else
        ret = -1;

    return ret;
}

int ate_get_btmac()
{
    int ret = 0;
    uint8_t btmac[20] = {0};
    ret = tg_bt_get_mac_address(btmac, 20);
    if (ret == 0)
        cli_printf("get bt mac %s\n", btmac);
    else
        ret = -1;

    return ret;
}

int ate_set_btmac(unsigned char *btmac)
{
    int ret = 0;
    cli_printf("set bt mac %s\n", btmac);
    ret = tg_bt_set_mac_address(btmac);
    return ret;
}

int ate_set_bootkey(unsigned char * bootKey)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_bootkey()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_set_secretKey_md5(unsigned char *secretKey,unsigned char *md5)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_secretKey()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_set_odm_name(unsigned char *name)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_rom()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_ram()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
	int mm_size = tg_get_mm_free();
    if (mm_size < 0)
        ret = -1;
	cli_printf("ram free size %d\n",mm_size);
    return ret;
}

int ate_get_nand_model()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_record_start(unsigned long seconds)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

static int led_all_on_off(int val)
{
	int ret = -1;
	int on = 0;
	if (val > 0)
		on = 255;

	for (int channel = TG_LIGHT_PWM_CHANNEL_0; channel < TG_LIGHT_PWM_CHANNEL_NUM; ++channel)
    {
        ret = tg_light_pwm_led_init(channel);
		if (ret)
		{
            cli_printf("channel %d init failed\n",channel);
			break;
        }

        ret = tg_light_pwm_led_set_brightness(channel, on);
        {
            cli_printf("channel %d set bright %d failed\n",channel,on);
            break;
        }
    }
	return ret;
}

int ate_led_all_on()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    ret = led_all_on_off(1);
    return ret;
}

int ate_led_all_off()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    ret = led_all_on_off(0);
    return ret;
}

int ate_set_vol(long vol)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    tg_volume_set(vol);
    return ret;
}

int ate_mic_quality_test()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_play_file(unsigned char *path)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_als_value()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_set_light_calibration(int on_off,int value)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_light_calibration(int on_off)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_hw_id(s)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_station_test_result()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_max_temp()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_set_sn(unsigned char *sn)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_sn()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_enter_user()
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    return ret;
}

int ate_get_flash_uid()
{
    unsigned char chipId[16];
    tg_get_chipid(chipId, 16);
    int i = 0;
    cli_printf("chipId = ");
    for(i= 0 ; i< 16; i++)
        cli_printf("0x%x ," ,chipId[i]);
    cli_printf("\n");
    return 0;
}

void factory_usage()
{
	cli_printf("%s ...\n",__func__);
}

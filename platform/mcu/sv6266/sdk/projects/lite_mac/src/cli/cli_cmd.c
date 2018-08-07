//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include <string.h>
#include "soc_types.h"
#include "soc_defs.h"
#include "osal.h"
#include "cli.h"
#include "sw_version.h"
#include "error.h"
#include "rf/rf_api.h"
#include "wificonf.h"
#include "wifi_api.h"
#ifdef SMARTCONFIG_EN
#include "joylink/joylink_main.h"
#endif
#ifdef OTA_EN
#include "ota_api.h"
#endif
#include "lowpower.h"
#include "sys_boot.h"

extern struct st_rf_table ssv_rf_table;
int bCliEfuseEnableWrite = 0;
/*---------------------------------- CMDs -----------------------------------*/
int Cmd_meminfo(s32 argc, char *argv[])
{
    printf("\n");
    OS_MemInfo();
    return ERROR_SUCCESS;
}

int Cmd_sysinfo(s32 argc, char *argv[])
{
    printf("\n");
    OS_SysInfo();
    return ERROR_SUCCESS;
}

int Cmd_WriteReg32 (s32 argc, char *argv[])
{
    char *pkey = 0, *pvalue = 0;

    if (argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    pkey = argv[0];
    pvalue = argv[1];
    
    if( pkey == 0 || pvalue == 0)
        return ERROR_INVALID_PARAMETER;

    unsigned int address = strtoul(pkey, NULL, 16);
    unsigned int data = strtoul(pvalue, NULL, 16);

    printf("reg : write 0x%x  0x%x \n", address, data);

    REG32(address)  = data;
    
    return ERROR_SUCCESS;
}


int Cmd_ReadReg32 (s32 argc, char *argv[])
{
    char *pkey = 0;
    unsigned int address; 
    if (argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    pkey = argv[0];
    
    address = strtoul(pkey, NULL, 16);
    
    printf("reg : read 0x%08x  0x%08x \n", address, (unsigned int)REG32(address));
    return ERROR_SUCCESS;
}

#ifdef SMARTCONFIG_EN 
int Cmd_smartconfig(s32 argc, char *argv[])
{
    u8 enable, sc_len, key[16];
    u8 *sc = NULL;
    if (argc < 2)  {
        printf("\nAT+SC [joylink/ smartlink/ airkiss],[xxx]\n");
        printf("ex: \n");
        printf("\tAT+SC=joylink,0123456789123456 => start joylink with key_code\n");
        printf("\tAT+SC=smartlink 1              => start smartlink\n");
        return ERROR_INVALID_PARAMETER;
    }

    printf("\n");
    sc_len = sizeof(u8)*strlen(argv[0]);
    sc = (u8 *) malloc(sc_len);
    memcpy(sc, argv[0], sc_len);
    sc[sc_len] = '\0';

    if (!memcmp(sc, "joylink", sc_len)) {
        memcpy(key, argv[1], sizeof(key));
        if (strlen(argv[1]) == 16) {
            joylink_init(key);    
        } 
        else {
            joylink_stop();    
        }
    }
#if 0
    else if (!memcmp(sc, "smartlink", sc_len)) {
        enable = atoi(param->argv[1]);
        if (enable) {
            smartlink_init();    
        }
        else {
            smartlink_stop();    
        } 
    }
#endif
    if (sc) {
        free(sc);
        sc = NULL;    
    }

    
    return ERROR_SUCCESS;

}
#endif //SMARTCONFIG_EN

static int Cmd_GetVersion (s32 argc, char *argv[])
{
    printf("\r\n");
    printf("+VERSION:%s_%s\r\n", version, date);
    return ERROR_SUCCESS;
}

int At_RadioWriteMAC1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    if( strlen(argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", "AT+RF_WRITE_MAC2");
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }
   
    wifi_cfg_replace_mem_addr1(cfg_handle, argv[0]);
    wifi_cfg_write_cfg(cfg_handle);
    wifi_cfg_deinit(cfg_handle);

    return ret;
}
int At_RadioReadMAC1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    void *cfg_handle = wifi_cfg_init();
    unsigned char mac_addr[6] = {0};
    wifi_cfg_get_addr1(cfg_handle, (char *)mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", __func__, 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return ret;
}
int At_RadioWriteMAC2(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    if( strlen(argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", "AT+RF_WRITE_MAC2");
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }
   
    wifi_cfg_replace_mem_addr2(cfg_handle, argv[0]);
    wifi_cfg_write_cfg(cfg_handle);
    wifi_cfg_deinit(cfg_handle);

    return ret;
}
int At_RadioReadMAC2(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    void *cfg_handle = wifi_cfg_init();
    unsigned char mac_addr[6] = {0};
    wifi_cfg_get_addr2(cfg_handle, mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", __func__, 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return ret; 
}
//efuse command
int At_ShowRet(int ret)
{
    if(ret == ERROR_SUCCESS)
        printf("+ok\n");
    else
        printf("+error:%d\n",ret);
    return ret;
}
int At_CliEfuseShowFreeBits(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    printf("Efuse Free bits = %d\n", efuse_get_free_bits());

    At_ShowRet(ret);
    return ret; 
}
int At_CliEfuseEnableWrite(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    bCliEfuseEnableWrite = 1;
    printf("Now you can write efuse\n");
    
    return ret; 
}
int At_CliEfuseWriteMAC(s32 argc, char *argv[])
{
    int ret = ERROR_SUCCESS;
    int i = 0;
    const char delimiters[] = ":";
    char *pMac = 0, *ptmp;
    unsigned long int toul_val;
    uint8_t efuse_mac[6] = {0};

    printf("\n");
    
    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if( strlen(argv[0]) != 17 )
    {
        printf("EX:AT+EFUSE_WRITE_MAC=00:23:45:67:89:AB\n");
        return ERROR_INVALID_PARAMETER;
    }

    pMac = argv[0];

    if(pMac != NULL) 
    {
        ptmp = strtok (pMac, delimiters);
        for(i=0; i<=5; i++) 
        {
            toul_val =  strtoul(ptmp, NULL, 16);
            efuse_mac[i] = toul_val;
            ptmp = strtok (NULL, delimiters);
        }
    }

    printf("Parse mac -> %02X:%02X:%02X:%02X:%02X:%02X\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);

    if(bCliEfuseEnableWrite == 0)
    {
        printf("Please run AT+EFUSE_ENABLE_MAC first\n");
        return ERROR_NOT_IMPLEMENT;
    }
    bCliEfuseEnableWrite = 0;
    
    ret = efuse_write_mac(efuse_mac);

    At_ShowRet(ret);
    return ret;
}
int At_CliEfuseReadMAC(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    uint8_t efuse_mac[6] = {0};

    ret = efuse_read_mac(efuse_mac);
    
    if(ret == 0)
    {
        printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", "Efuse MAC", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
    }
    else
    {
        printf("No MAC in Efuse\n");
    }
    
    return ret; 
}   
int At_CliEfuseDump(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    ret = efuse_dump_data();
    
    return ret; 
}   
//rf test command
int At_CliReboot(s32 argc, char *argv[])
{

    drv_wdt_init();
    drv_wdt_enable(1, 100);
    
    return ERROR_SUCCESS;
}
int At_CliRFStart(s32 argc, char *argv[])
{	
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    ret = rf_tx_start(atoi(argv[0]), atoi(argv[1])); 	

    At_ShowRet(ret);
    return ret;
}
int At_CliChannel(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int ch = atoi(argv[0]);
    int ch_type = 1;
    
    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    if(strcmp("+", argv[1]) == 0)
    {
        ch_type = 3;    //NL80211_CHAN_HT40PLUS
    }
    else if(strcmp("-", argv[1]) == 0)
    {
        ch_type= 2; //NL80211_CHAN_HT40MINUS
    }

    ret = rf_set_ch(ch, 0, ch_type);

    At_ShowRet(ret);
    return ret;
}
int At_CliRFRate(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int rate_id = 0;
    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    rate_id = atoi(argv[0]);
    ret = rf_set_rate(rate_id);

    At_ShowRet(ret);
	return ret;
}
int At_CliRFBandGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }

    ret = write_reg_band_gain(atoi(argv[0]));

    At_ShowRet(ret);
    return ret;
}
int At_CliRFRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    int gain_idx = 0;
    struct st_rate_gain rate_config = {0};
        
    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    gain_idx = atoi(argv[1]); 

    printf("argv0 = %s, \n", argv[0]);
    if( strncmp(argv[0], "b", 1) == 0 )
    {
        printf("set b mode\n");
        ret = write_reg_rate_gain_b(gain_idx);
    }
    else if( strncmp(argv[0], "g", 1) == 0 )
    {
        printf("set g mode\n");
        if(strcmp(argv[0], "g6") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x8); //
        }
        else if(strcmp(argv[0], "g12") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x4);
        }
        else if(strcmp(argv[0], "g24") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x2);
        }
        else if(strcmp(argv[0], "g48") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }    
    else if( strncmp(argv[0], "20n", 3) == 0 )
    {

        printf("set 20n mode\n");
        if(strcmp(argv[0], "20n0") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x8);
        }
        else if(strcmp(argv[0], "20n1") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x4);            
        }
        else if(strcmp(argv[0], "20n3") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x2);
        }
        else if(strcmp(argv[0], "20n5") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }
    else if( strncmp(argv[0], "40n", 3) == 0 )
    {

        printf("set 40n mode\n");

        if(strcmp(argv[0], "40n0") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x8);
        }
        else if(strcmp(argv[0], "40n1") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x4);
        }
        else if(strcmp(argv[0], "40n3") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x2);
        }
        else if(strcmp(argv[0], "40n5") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }
    else
    {
        return ERROR_NOT_IMPLEMENT;
    }

    At_ShowRet(ret);
    return ret;
}
int At_CliRFReadTempcs(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    int value = 0;
    get_current_temperature(&value);

    printf("temperature = %d\n", value);
    At_ShowRet(ret);
    return ret;
}
int At_CliRFTempBoundary(s32 argc, char *argv[])
{

    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }

    set_temper_value(atoi(argv[0]), atoi(argv[1]));
    
    save_rf_table_to_flash();

    At_ShowRet(ret);
    return ret;
}
int At_CliRFStop(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    
    ret = rf_tx_stop();

    At_ShowRet(ret);
    return ret;
}
int At_CliRFCount(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    uint32_t err_rxpkt = 0;
    uint32_t total_rxpkt = 0;

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }

    if( atoi(argv[0]) == 1 )
    {
        printf("B Mode Count:\n");
        rf_bmode_count(&total_rxpkt, &err_rxpkt);
    }
    else
    {
        printf("G/N Mode Count:\n");
        rf_gmode_count(&total_rxpkt, &err_rxpkt);
    }
        
    printf("crc count = '%d'\n", err_rxpkt);
    printf("total count = '%d'\n", total_rxpkt);	

    At_ShowRet(ret);
    return ret;
}

int At_CliRFReset(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    ret = rf_rx_reset();

    At_ShowRet(ret);
    return ret;
}
int At_CliRFFreqoffset(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }

    ret = write_reg_freq_xi_xo(atoi(argv[0]), atoi(argv[0]), 0x3);

    At_ShowRet(ret);
    return ret;
}
int At_CliRFFreqoffset_readxo(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xo_value = 0;
    ret = rf_freqoffset_readxo(&xo_value);
    
    printf("Freq Offset XO=%d\n", xo_value);

    At_ShowRet(ret);
    return ret;
}  
int At_CliRFFreqoffset_readxi(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xi_value = 0;
    ret = rf_freqoffset_readxi(&xi_value);

    printf("Freq Offset XI=%d\n", xi_value);

    At_ShowRet(ret);
    return ret;
}    
int At_CliRFDump(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    dump_rf_value();

    At_ShowRet(ret); 
    return ret;
}
int At_CliRFTableRT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", argv[0], argv[1], argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("RT band_gain_ch1_ch2 = %d\n", ssv_rf_table.rt_config.band_gain[0] );
        printf("RT band_gain_ch3_ch4 = %d\n", ssv_rf_table.rt_config.band_gain[1] );
        printf("RT band_gain_ch5_ch6 = %d\n", ssv_rf_table.rt_config.band_gain[2] );
        printf("RT band_gain_ch7_ch8 = %d\n", ssv_rf_table.rt_config.band_gain[3] );
        printf("RT band_gain_ch9_ch10 = %d\n", ssv_rf_table.rt_config.band_gain[4] );
        printf("RT band_gain_ch11_ch12 = %d\n", ssv_rf_table.rt_config.band_gain[5] );
        printf("RT band_gain_ch13_ch14 = %d\n", ssv_rf_table.rt_config.band_gain[6] );

        printf("RT freq xi = %d\n", ssv_rf_table.rt_config.band_gain[7] );
        printf("RT freq xo = %d\n", ssv_rf_table.rt_config.band_gain[8] );
        printf("RT ldo rxafe = %d\n", ssv_rf_table.rt_config.band_gain[9] );
        printf("RT ldo dcdcv = %d\n", ssv_rf_table.rt_config.band_gain[10] );
        printf("RT ldo dldov = %d\n", ssv_rf_table.rt_config.band_gain[11] );

        printf("RT 2gpa1 = %d\n", ssv_rf_table.rt_config.band_gain[12] );
        printf("RT 2gpa2 = %d\n", ssv_rf_table.rt_config.band_gain[13] );
        printf("RT 2gpa3 = %d\n", ssv_rf_table.rt_config.band_gain[14] );
        printf("RT 2gpa bias= %d\n", ssv_rf_table.rt_config.band_gain[15] );
        printf("RT 2gpa cap = %d\n", ssv_rf_table.rt_config.band_gain[16] );
        printf("RT padpd cali = %d\n", ssv_rf_table.rt_config.band_gain[17] );        
    }
    else
    {
        struct st_tempe_table tempe_config = {0};
        tempe_config.band_gain[0] = atoi(argv[0]);
        tempe_config.band_gain[1] = atoi(argv[1]);
        tempe_config.band_gain[2] = atoi(argv[2]);
        tempe_config.band_gain[3] = atoi(argv[3]);
        tempe_config.band_gain[4] = atoi(argv[4]);
        tempe_config.band_gain[5] = atoi(argv[5]);
        tempe_config.band_gain[6] = atoi(argv[6]);
        
        tempe_config.freq_xi = atoi(argv[7]);
        tempe_config.freq_xo = atoi(argv[8]);
        tempe_config.ldo_rxafe = atoi(argv[9]);
        tempe_config.ldo_dcdcv = atoi(argv[10]);
        tempe_config.ldo_dldov = atoi(argv[11]);

        tempe_config.pa_vcas1 = atoi(argv[12]);
        tempe_config.pa_vcas2 = atoi(argv[13]);
        tempe_config.pa_vcas3 = atoi(argv[14]);
        tempe_config.pa_bias = atoi(argv[15]);
        tempe_config.pa_cap = atoi(argv[16]);
        tempe_config.padpd_cali = atoi(argv[17]);

        ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
        {
            At_ShowRet(ret);
            return ret;
        }
        memcpy(&ssv_rf_table.rt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }

    At_ShowRet(ret);
    return ret;
}  
int At_CliRfTableHT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", param->argv[0], param->argv[1], param->argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("HT band_gain_ch1_ch2 = %d\n", ssv_rf_table.ht_config.band_gain[0] );
        printf("HT band_gain_ch3_ch4 = %d\n", ssv_rf_table.ht_config.band_gain[1] );
        printf("HT band_gain_ch5_ch6 = %d\n", ssv_rf_table.ht_config.band_gain[2] );
        printf("HT band_gain_ch7_ch8 = %d\n", ssv_rf_table.ht_config.band_gain[3] );
        printf("HT band_gain_ch9_ch10 = %d\n", ssv_rf_table.ht_config.band_gain[4] );
        printf("HT band_gain_ch11_ch12 = %d\n", ssv_rf_table.ht_config.band_gain[5] );
        printf("HT band_gain_ch13_ch14 = %d\n", ssv_rf_table.ht_config.band_gain[6] );

        printf("HT freq xi = %d\n", ssv_rf_table.ht_config.band_gain[7] );
        printf("HT freq xo = %d\n", ssv_rf_table.ht_config.band_gain[8] );
        printf("HT ldo rxafe = %d\n", ssv_rf_table.ht_config.band_gain[9] );
        printf("HT ldo dcdcv = %d\n", ssv_rf_table.ht_config.band_gain[10] );
        printf("HT ldo dldov = %d\n", ssv_rf_table.ht_config.band_gain[11] );

        printf("HT 2gpa1 = %d\n", ssv_rf_table.ht_config.band_gain[12] );
        printf("HT 2gpa2 = %d\n", ssv_rf_table.ht_config.band_gain[13] );
        printf("HT 2gpa3 = %d\n", ssv_rf_table.ht_config.band_gain[14] );
        printf("HT 2gpa bias= %d\n", ssv_rf_table.ht_config.band_gain[15] );
        printf("HT 2gpa cap = %d\n", ssv_rf_table.ht_config.band_gain[16] );
        printf("HT padpd cali = %d\n", ssv_rf_table.ht_config.band_gain[17] );
    }
    else
    {
        struct st_tempe_table tempe_config = {0};
        tempe_config.band_gain[0] = atoi(argv[0]);
        tempe_config.band_gain[1] = atoi(argv[1]);
        tempe_config.band_gain[2] = atoi(argv[2]);
        tempe_config.band_gain[3] = atoi(argv[3]);
        tempe_config.band_gain[4] = atoi(argv[4]);
        tempe_config.band_gain[5] = atoi(argv[5]);
        tempe_config.band_gain[6] = atoi(argv[6]);
        
        tempe_config.freq_xi = atoi(argv[7]);
        tempe_config.freq_xo = atoi(argv[8]);
        tempe_config.ldo_rxafe = atoi(argv[9]);
        tempe_config.ldo_dcdcv = atoi(argv[10]);
        tempe_config.ldo_dldov = atoi(argv[11]);

        tempe_config.pa_vcas1 = atoi(argv[12]);
        tempe_config.pa_vcas2 = atoi(argv[13]);
        tempe_config.pa_vcas3 = atoi(argv[14]);
        tempe_config.pa_bias = atoi(argv[15]);
        tempe_config.pa_cap = atoi(argv[16]);
        tempe_config.padpd_cali = atoi(argv[17]);

        ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
        {
            At_ShowRet(ret);
            return ret;
        }

        memcpy(&ssv_rf_table.ht_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    At_ShowRet(ret);
    return ret;
}      
int At_CliRfTableLT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", param->argv[0], param->argv[1], param->argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("LT band_gain_ch1_ch2 = %d\n", ssv_rf_table.lt_config.band_gain[0] );
        printf("LT band_gain_ch3_ch4 = %d\n", ssv_rf_table.lt_config.band_gain[1] );
        printf("LT band_gain_ch5_ch6 = %d\n", ssv_rf_table.lt_config.band_gain[2] );
        printf("LT band_gain_ch7_ch8 = %d\n", ssv_rf_table.lt_config.band_gain[3] );
        printf("LT band_gain_ch9_ch10 = %d\n", ssv_rf_table.lt_config.band_gain[4] );
        printf("LT band_gain_ch11_ch12 = %d\n", ssv_rf_table.lt_config.band_gain[5] );
        printf("LT band_gain_ch13_ch14 = %d\n", ssv_rf_table.lt_config.band_gain[6] );

        printf("LT freq xi = %d\n", ssv_rf_table.lt_config.band_gain[7] );
        printf("LT freq xo = %d\n", ssv_rf_table.lt_config.band_gain[8] );
        printf("LT ldo rxafe = %d\n", ssv_rf_table.lt_config.band_gain[9] );
        printf("LT ldo dcdcv = %d\n", ssv_rf_table.lt_config.band_gain[10] );
        printf("LT ldo dldov = %d\n", ssv_rf_table.lt_config.band_gain[11] );

        printf("LT 2gpa1 = %d\n", ssv_rf_table.lt_config.band_gain[12] );
        printf("LT 2gpa2 = %d\n", ssv_rf_table.lt_config.band_gain[13] );
        printf("LT 2gpa3 = %d\n", ssv_rf_table.lt_config.band_gain[14] );
        printf("LT 2gpa bias= %d\n", ssv_rf_table.lt_config.band_gain[15] );
        printf("LT 2gpa cap = %d\n", ssv_rf_table.lt_config.band_gain[16] );
        printf("LT padpd cali = %d\n", ssv_rf_table.lt_config.band_gain[17] );        
    }
    else
    {
        struct st_tempe_table tempe_config = {0};
        tempe_config.band_gain[0] = atoi(argv[0]);
        tempe_config.band_gain[1] = atoi(argv[1]);
        tempe_config.band_gain[2] = atoi(argv[2]);
        tempe_config.band_gain[3] = atoi(argv[3]);
        tempe_config.band_gain[4] = atoi(argv[4]);
        tempe_config.band_gain[5] = atoi(argv[5]);
        tempe_config.band_gain[6] = atoi(argv[6]);
        
        tempe_config.freq_xi = atoi(argv[7]);
        tempe_config.freq_xo = atoi(argv[8]);
        tempe_config.ldo_rxafe = atoi(argv[9]);
        tempe_config.ldo_dcdcv = atoi(argv[10]);
        tempe_config.ldo_dldov = atoi(argv[11]);

        tempe_config.pa_vcas1 = atoi(argv[12]);
        tempe_config.pa_vcas2 = atoi(argv[13]);
        tempe_config.pa_vcas3 = atoi(argv[14]);
        tempe_config.pa_bias = atoi(argv[15]);
        tempe_config.pa_cap = atoi(argv[16]);
        tempe_config.padpd_cali = atoi(argv[17]);

        ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
        {
            At_ShowRet(ret);
            return ret;
        }

        memcpy(&ssv_rf_table.lt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    
    At_ShowRet(ret);
    return ret;
}      
int At_CliRfTableGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("RF Gain = %d\n", ssv_rf_table.rf_gain );
    }
    else
    {
        uint8_t rf_gain = atoi(argv[0]);
        ret = write_reg_rf_gain(rf_gain);
        if(ret != 0)
            return ret;
        ssv_rf_table.rf_gain = rf_gain;
        save_rf_table_to_flash(); 
    }
    At_ShowRet(ret);
    return ret;
}
int At_CliRfTableBRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("B Mode Rate Gain = %d\n", ssv_rf_table.rate_gain_b);
    }
    else
    {
        uint8_t rate_gain = atoi(argv[0]);
        ret = write_reg_rate_gain_b(rate_gain);
        if(ret != 0)
            return ret;
        ssv_rf_table.rate_gain_b = rate_gain;
        save_rf_table_to_flash();
    }
    At_ShowRet(ret);
    return ret;
}   
int At_CliRfTableGRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("G Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_g.rate1, 
                                                                              ssv_rf_table.rate_config_g.rate2, 
                                                                              ssv_rf_table.rate_config_g.rate3, 
                                                                              ssv_rf_table.rate_config_g.rate4 );
    }
    else
    {
        struct st_rate_gain g_rate_gain = {0};
        g_rate_gain.rate1 = atoi(argv[0]);
        g_rate_gain.rate2 = atoi(argv[1]);
        g_rate_gain.rate3 = atoi(argv[2]);
        g_rate_gain.rate4 = atoi(argv[3]);
        
        ret = write_reg_rate_gain_g(g_rate_gain, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_g, &g_rate_gain, sizeof(g_rate_gain));
        save_rf_table_to_flash(); 
    }
    At_ShowRet(ret);
    return ret;
}   
int At_CliRfTable20NRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("20N Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_20n.rate1, 
                                                                              ssv_rf_table.rate_config_20n.rate2, 
                                                                              ssv_rf_table.rate_config_20n.rate3, 
                                                                              ssv_rf_table.rate_config_20n.rate4 );
    }
    else
    {
        struct st_rate_gain rate_gain_20n = {0};
        rate_gain_20n.rate1 = atoi(argv[0]);
        rate_gain_20n.rate2 = atoi(argv[1]);
        rate_gain_20n.rate3 = atoi(argv[2]);
        rate_gain_20n.rate4 = atoi(argv[3]);
        
        ret = write_reg_rate_gain_20n(rate_gain_20n, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_20n, &rate_gain_20n, sizeof(rate_gain_20n));
        save_rf_table_to_flash(); 
    }
    At_ShowRet(ret);
    return ret;
}   
int At_CliRfTable40NRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
    {
        ret = ERROR_INVALID_PARAMETER;
        At_ShowRet(ret);
        return ret;
    }
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("40N Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_40n.rate1, 
                                                                              ssv_rf_table.rate_config_40n.rate2, 
                                                                              ssv_rf_table.rate_config_40n.rate3, 
                                                                              ssv_rf_table.rate_config_40n.rate4 );
    }
    else
    {
        struct st_rate_gain rate_gain_40n = {0};
        rate_gain_40n.rate1 = atoi(argv[0]);
        rate_gain_40n.rate2 = atoi(argv[1]);
        rate_gain_40n.rate3 = atoi(argv[2]);
        rate_gain_40n.rate4 = atoi(argv[3]);
        
        ret = write_reg_rate_gain_40n(rate_gain_40n, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_40n, &rate_gain_40n, sizeof(rate_gain_40n));
        save_rf_table_to_flash(); 
    }

    At_ShowRet(ret);
    return ret;
} 

int At_PowerSave(s32 argc, char *argv[])
{
    set_power_mode(1, 0);
    return ERROR_SUCCESS;
}

static void lp_idle_usage() {
    printf("Usage:");
    printf("AT+IDLE_POWER <ctrl>\n");
    printf("<ctrl> :1 --> to idle power\n");
    printf("<ctrl> :0 --> leave idle power\n");
}

void scan_cbfunc()
{
    u8 i;

    printf("\nCount:%d\n", getAvailableIndex());
    for(i = 0; i < getAvailableIndex(); i++)
    {
        printf("%2d - name:%32s, rssi:-%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n", i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel
                , ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2], ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
    printf("end\n");
}

void At_Connect_usage() {
}

#define CNT_SSID        "CHT2095"
#define CNT_PSK         "**********"

int At_Scan(s32 argc, char *argv[])
{
    DUT_wifi_start(1);
    if(scan_AP(scan_cbfunc))
        return ERROR_INVALID_PARAMETER;
    return ERROR_SUCCESS;
}

void atwificbfunc(WIFI_RSP *msg)
{
    uint8_t dhcpen;
    u8 mac[6];
    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;

    if(msg->wifistatus == 1)
    {
        printf("connect OK\n");
        if(msg->id == 0)
            get_if_config_2("wlan0", &dhcpen, &ipaddr, &submask, &gateway, &dnsserver, mac, 6);
        else
            get_if_config_2("wlan1", &dhcpen, &ipaddr, &submask, &gateway, &dnsserver, mac, 6);
        printf("STA%d:\n", msg->id);
        printf("mac addr        - %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("ip addr         - %d.%d.%d.%d\n", ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3]);
        printf("netmask         - %d.%d.%d.%d\n", submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3]);
        printf("default gateway - %d.%d.%d.%d\n", gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);
        printf("DNS server      - %d.%d.%d.%d\n", dnsserver.u8[0], dnsserver.u8[1], dnsserver.u8[2], dnsserver.u8[3]);
    }
    else
    {
        printf("disconnect OK\n");
    }
}

int At_Connect(s32 argc, char *argv[])
{
    int ret;
    if (argc < 1)  {
        At_Connect_usage();
        return ERROR_INVALID_PARAMETER;
    }

    printf("ssid = %s\n", argv[0]);
    if (argc > 1) {
        printf("password = %s\n", argv[1]);
        ret = set_wifi_config(argv[0], strlen(argv[0]), argv[1], strlen(argv[1]), NULL, 0);
    } else {
        ret = set_wifi_config(argv[0], strlen(argv[0]), NULL, 0, NULL, 0);
    }
    //ret = set_wifi_config(CNT_SSID, strlen(CNT_SSID), CNT_PSK, strlen(CNT_PSK), NULL, 0);
    wifi_connect(atwificbfunc);
    return ERROR_SUCCESS;
}
#ifdef OTA_EN
int At_OTASetServer (s32 argc, char *argv[])
{
    char *pIp = 0, *pport = 0;

    if (get_wifi_status() != 1)
        return ERROR_WIFI_CONNECTION;

    if (argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

    pIp = argv[0];
    pport = argv[1];

    if (atoi(pport) == 0) 
    {
        return ERROR_INVALID_PARAMETER;
    }

    ota_set_server(atoi(argv[2]), pIp, atoi(pport));

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_OTASetParam (s32 argc, char *argv[])
{
    char *pFilename = 0, *pPathname = 0;

    pFilename = argv[0];
    pPathname = argv[1];

    if(pFilename == 0)
        return 1;
    if(pPathname == NULL)
        ota_set_parameter(pFilename, "");
    else
        ota_set_parameter(pFilename, pPathname);
    return ERROR_SUCCESS;	
}
/*---------------------------------------------------------------------------*/
int At_OTAStart (s32 argc, char *argv[])
{
    ota_update();
    return ERROR_SUCCESS;
}
#endif

/*---------------------------------------------------------------------------*/
int At_UartFWUpgrade (s32 argc, char *argv[])
{
    sys_uart_fw_upgrade();
    return ERROR_SUCCESS;
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] =
{
    { "meminfo",            Cmd_meminfo,        "meminfo"},
    { "sysinfo",            Cmd_sysinfo,        "sysinfo"},
    { "regw",               Cmd_WriteReg32,     "regw"},
    { "regr",               Cmd_ReadReg32,      "regr"},

    { "AT+VERSION",         Cmd_GetVersion,     "SW Version"},
    { "AT+RF_WRITE_MAC1",   At_RadioWriteMAC1,  "write mac 1"},
    { "AT+RF_READ_MAC1",    At_RadioReadMAC1,   "read mac 1"},
    { "AT+RF_WRITE_MAC2",   At_RadioWriteMAC2,  "write mac 2"},
    { "AT+RF_READ_MAC2",    At_RadioReadMAC2,   "read mac 2"},
    //efuse
    { "AT+EFUSE_FREE_BITS",    At_CliEfuseShowFreeBits,   "show efuse free bits"},
    { "AT+EFUSE_ENABLE_MAC",    At_CliEfuseEnableWrite,   "enable efuse write"},
    { "AT+EFUSE_WRITE_MAC",   At_CliEfuseWriteMAC,  "write mac to efuse"},
    { "AT+EFUSE_READ_MAC",    At_CliEfuseReadMAC,   "read mac from efuse"},
    { "AT+EFUSE_DUMP",    At_CliEfuseDump,   "dump efuse data"},
    //rf test command
    { "AT+REBOOT",   At_CliReboot,  "system reboot"},
    { "AT+RF_START",   At_CliRFStart,  "rf tx start"},
    { "AT+RF_CHANNEL",    At_CliChannel,   "rf set channel"},
    { "AT+RF_RATE",   At_CliRFRate,  "rf set rate"},
    { "AT+RF_2GBAND_GAIN",   At_CliRFBandGain,  "rf set 2.4G digital gain"},
    { "AT+RF_RATE_GAIN",   At_CliRFRateGain,  "rf set rate gain"},
    { "AT+RF_READ_TEMPCS",   At_CliRFReadTempcs,  "read chip temperature"},
    { "AT+RF_TEMP_BOUNDARY",   At_CliRFTempBoundary,  "set temperature boundary"},
    { "AT+RF_STOP",   At_CliRFStop,  "rf tx stop"},
    { "AT+RF_COUNT",   At_CliRFCount,  "rf get b/g count"},
    { "AT+RF_RESET",   At_CliRFReset,  "rf rx reset count"},
    //rf tune command
    { "AT+RF_FREQOFFSET",   At_CliRFFreqoffset,  "rf set frequency"},
    { "AT+RF_FREQOFFSETREAD_XO",   At_CliRFFreqoffset_readxo,  "rf read freq XO"},
    { "AT+RF_FREQOFFSETREAD_XI",   At_CliRFFreqoffset_readxi,  "rf read freq XI"},
    { "AT+RF_DUMP",   At_CliRFDump,  "dump rf setting"},
    //rf table command
    { "AT+RF_TABLE_RT",   At_CliRFTableRT,  "write rt table to flash"},
    { "AT+RF_TABLE_HT",   At_CliRfTableHT,  "write ht table to flash"},
    { "AT+RF_TABLE_LT",   At_CliRfTableLT,  "write lt table to flash"},
    { "AT+RF_TABLE_GAIN",   At_CliRfTableGain,  "write rf gain to flash"},
    { "AT+RF_TABLE_BRATE_GAIN",   At_CliRfTableBRateGain,  "write B mod rate gain to flash"},
    { "AT+RF_TABLE_GRATE_GAIN",   At_CliRfTableGRateGain,  "write G mod rate gain to flash"},
    { "AT+RF_TABLE_20NRATE_GAIN",   At_CliRfTable20NRateGain,  "write 20N mod rate gain to flash"},
    { "AT+RF_TABLE_40NRATE_GAIN",   At_CliRfTable40NRateGain,  "write 40N mod rate gain to flash"},
    
#ifdef SMARTCONFIG_EN
    { "AT+SC",              Cmd_smartconfig,    "smart config, airkiss, joylink"},
#endif //SMARTCONFIG_EN
    {"AT+SCAN", At_Scan, "do scan"},
    {"AT+CONNECT", At_Connect, "test connect to ssid password"},
    {"AT+POWERSAVE",			At_PowerSave,			"start power saving mode"},
#ifdef OTA_EN	
    {"AT+OTASETSERVER",       At_OTASetServer,       "set ota server"},
    {"AT+OTASETPARAM",        At_OTASetParam,        "set ota parameter"},
    {"AT+OTASTART",           At_OTAStart,           "start ota"},
#endif
#if (SETTING_UART_FW_UPGRADE == 0)
    {"AT+UART_FW_UPGRADE", At_UartFWUpgrade, "start uart upgrade"},
#endif
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};



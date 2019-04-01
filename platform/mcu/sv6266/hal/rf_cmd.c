#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <aos/network.h>
//#include "common.h"
#include "wifi_api.h"
#include "rf_api.h"
#include "mac_cfg.h"
#include "atcmd_rf_def.h"
#include "atcmd_general_def.h"
#include "error.h"

extern struct st_rf_table ssv_rf_table;

////RF AT Command
int At_RadioRFStart(s32 argc, char *argv[])
{	
    printf("\n");
    int ret = ERROR_SUCCESS;

    ret = rf_tx_start(atoi(argv[0]), atoi(argv[1])); 	
    return ret;
}
int At_RadioChannel(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int ch = atoi(argv[0]);
    int ch_type = NL80211_CHAN_HT20;
    
    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("+", argv[1]) == 0)
    {
        ch_type = NL80211_CHAN_HT40PLUS;
    }
    else if(strcmp("-", argv[1]) == 0)
    {
        ch_type= NL80211_CHAN_HT40MINUS;
    }

    ret = rf_set_ch(ch, 0, ch_type);

    return ret;
}
int At_RadioRFRate(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int rate_id = 0;
    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    rate_id = atoi(argv[0]);
    ret = rf_set_rate(rate_id);

	return ret;
}
int At_RadioRFBandGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_band_gain(atoi(argv[0]));
    
    return ret;
}
int At_RadioRFGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    ret = write_reg_rf_gain(atoi(argv[0]));
        
    return ret;
}
int At_RadioRFRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    int gain_idx = 0;
    struct st_rate_gain rate_config = {0};
        
    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    
    return ret;
}
int At_RadioRFReadTempcs(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    int value = 0;
    get_current_temperature(&value);

    printf("temperature = %d\n", value);
    return ret;
}
int At_RadioRFStop(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    
    ret = rf_tx_stop();
    
    return ret;
}
int At_RadioRFReset(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    ret = rf_rx_reset();

    return ret;
}
int At_RadioRFCount(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    uint32_t err_rxpkt = 0;
    uint32_t total_rxpkt = 0;

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

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
        
    printf("crc count = '%ld'\n", err_rxpkt);
    printf("total count = '%ld'\n", total_rxpkt);	

    return ret;
}
int At_RadioEnableTCSR(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    ret = rf_enable_tcsr(atoi(argv[0]));
    return ret;
}
int At_RadioRFDump(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    dump_rf_table();
    return ret;
}
int At_RfCaliDPD(s32 argc, char *argv[])    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
   ret = write_reg_padpd_cali(atoi(argv[0]));
     
    return ret;    

}
int At_RfManuDPD(s32 argc, char *argv[])    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    ret = rf_manu_padpd_cali();
     
    return ret;    

}
int At_RadioRFFreqoffset(s32 argc, char *argv[])    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_freq_xi_xo(atoi(argv[0]), atoi(argv[0]), 0x3);
        
    return ret;
}
int At_RadioRFFreqoffset_readxi(s32 argc, char *argv[])    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xi_value = 0;
    ret = rf_freqoffset_readxi(&xi_value);

    printf("%s=%ld\n", ATCMD_RF_FREQOFFSET_READXI, xi_value);
         
    return ret;
}    
int At_RadioRFFreqoffset_readxo(s32 argc, char *argv[])    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xo_value = 0;
    ret = rf_freqoffset_readxo(&xo_value);
    
    printf("%s=%ld\n", ATCMD_RF_FREQOFFSET_READXO, xo_value);
         
    return ret;
}    

int At_RadioRF2Gpa1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(atoi(argv[0]), 0, 0, 0, 0x8);
        
    return ret;
}    
int At_RadioRF2Gpa2(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, atoi(argv[0]), 0, 0, 0x4);
        
    return ret;
}   
int At_RadioRF2Gpa3(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, 0, atoi(argv[0]), 0, 0x2);
        
    return ret;
}   
int At_RadioRF2GpaBias(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, 0, 0, atoi(argv[0]), 0x1);
        
    return ret;
}   
int At_RadioRF2GpaCell(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa_cell(atoi(argv[0]));
        
    return ret;
}   
int At_RadioRF2GpaCap(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa_cap(atoi(argv[0]));
        
    return ret;
}   

int At_RadioRFLdo(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_rxfe(atoi(argv[0]));
        
    return ret;
}     
int At_RadioRFDcdc(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_dcdc(atoi(argv[0]));
        
    return ret;
} 
int At_RadioRFDcdcv(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_ldo(atoi(argv[0]), 0, 0x2);
        
    return ret;
} 
int At_RadioRFDldov(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_ldo(0, atoi(argv[0]), 0x1);
        
    return ret;
}     
int At_RadioRFXoldo(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_xoldo(atoi(argv[0]));
        
    return ret;
}     
int At_RadioRFRxPadCh13(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_rxpad_ch13(atoi(argv[0]));
        
    return ret;
}      
int At_RadioRFTempBoundary(s32 argc, char *argv[])
{

    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    set_temper_value(atoi(argv[0]), atoi(argv[1]));
    
    save_rf_table_to_flash();
    
    return ret;
}
int At_RfTableRT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", argv[0], argv[1], argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_RT
		 , ssv_rf_table.rt_config.band_gain[0]
		 , ssv_rf_table.rt_config.band_gain[1]
		 , ssv_rf_table.rt_config.band_gain[2]
		 , ssv_rf_table.rt_config.band_gain[3]
		 , ssv_rf_table.rt_config.band_gain[4]
		 , ssv_rf_table.rt_config.band_gain[5]
		 , ssv_rf_table.rt_config.band_gain[6]
		 , ssv_rf_table.rt_config.freq_xi
		 , ssv_rf_table.rt_config.freq_xo
		 , ssv_rf_table.rt_config.ldo_rxafe
		 , ssv_rf_table.rt_config.ldo_dcdcv
		 , ssv_rf_table.rt_config.ldo_dldov
		 , ssv_rf_table.rt_config.pa_vcas1
		 , ssv_rf_table.rt_config.pa_vcas2
		 , ssv_rf_table.rt_config.pa_vcas3
		 , ssv_rf_table.rt_config.pa_bias
		 , ssv_rf_table.rt_config.pa_cap
		 , ssv_rf_table.rt_config.padpd_cali
		 );
    }
    else
    {
        struct st_tempe_table tempe_config;
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

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 0 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
        
    return ret;
}      
int At_RfTableHT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", argv[0], argv[1], argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_HT
		 , ssv_rf_table.ht_config.band_gain[0]
		 , ssv_rf_table.ht_config.band_gain[1]
		 , ssv_rf_table.ht_config.band_gain[2]
		 , ssv_rf_table.ht_config.band_gain[3]
		 , ssv_rf_table.ht_config.band_gain[4]
		 , ssv_rf_table.ht_config.band_gain[5]
		 , ssv_rf_table.ht_config.band_gain[6]
		 , ssv_rf_table.ht_config.freq_xi
		 , ssv_rf_table.ht_config.freq_xo
		 , ssv_rf_table.ht_config.ldo_rxafe
		 , ssv_rf_table.ht_config.ldo_dcdcv
		 , ssv_rf_table.ht_config.ldo_dldov
		 , ssv_rf_table.ht_config.pa_vcas1
		 , ssv_rf_table.ht_config.pa_vcas2
		 , ssv_rf_table.ht_config.pa_vcas3
		 , ssv_rf_table.ht_config.pa_bias
		 , ssv_rf_table.ht_config.pa_cap
		 , ssv_rf_table.ht_config.padpd_cali
		 );
    }
    else
    {
        struct st_tempe_table tempe_config;
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

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 1 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.ht_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    return ret;
}      
int At_RfTableLT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", argv[0], argv[1], argv[17]);
    
    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_LT
		 , ssv_rf_table.lt_config.band_gain[0]
		 , ssv_rf_table.lt_config.band_gain[1]
		 , ssv_rf_table.lt_config.band_gain[2]
		 , ssv_rf_table.lt_config.band_gain[3]
		 , ssv_rf_table.lt_config.band_gain[4]
		 , ssv_rf_table.lt_config.band_gain[5]
		 , ssv_rf_table.lt_config.band_gain[6]
		 , ssv_rf_table.lt_config.freq_xi
		 , ssv_rf_table.lt_config.freq_xo
		 , ssv_rf_table.lt_config.ldo_rxafe
		 , ssv_rf_table.lt_config.ldo_dcdcv
		 , ssv_rf_table.lt_config.ldo_dldov
		 , ssv_rf_table.lt_config.pa_vcas1
		 , ssv_rf_table.lt_config.pa_vcas2
		 , ssv_rf_table.lt_config.pa_vcas3
		 , ssv_rf_table.lt_config.pa_bias
		 , ssv_rf_table.lt_config.pa_cap
		 , ssv_rf_table.lt_config.padpd_cali
		 );
    }
    else
    {
        struct st_tempe_table tempe_config;
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

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 2 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.lt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    return ret;
}      
int At_RfTableGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    return ret;
}
int At_RfTableBRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    return ret;
}   
int At_RfTableGRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    return ret;
}   
int At_RfTable20NRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    return ret;
}   
int At_RfTable40NRateGain(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(argc <1)
        return ERROR_INVALID_PARAMETER;
    
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
    return ret;
}   
int At_RadioWriteMAC1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(argc <1)
        return ERROR_INVALID_PARAMETER;

    if( strlen(argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_RADIO_WRITE_MAC1);
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_MEMORY_FAILED;
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
    char mac_addr[6] = {0};
    wifi_cfg_get_addr1(cfg_handle, mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_RADIO_READ_MAC1, (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2], (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5]);
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
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_RADIO_WRITE_MAC2);
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_MEMORY_FAILED;
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
    char mac_addr[6] = {0};
    wifi_cfg_get_addr2(cfg_handle, mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_RADIO_READ_MAC2, (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2], (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5]);
    return ret; 
}

int bEfuseEnableWrite = 0;

int At_EfuseEnableWrite(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    bEfuseEnableWrite = 1;
    printf("Now you can write efuse\n");
    
    return ret; 
}
int At_EfuseWriteMAC(s32 argc, char *argv[])
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
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_EFUSE_WRITE_MAC);
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

    if(bEfuseEnableWrite == 0)
    {
        printf("Please run %s first\n", ATCMD_EFUSE_ENABLE_WRITE);
        return ERROR_NOT_IMPLEMENT;
    }
    bEfuseEnableWrite = 0;

    ret = efuse_write_mac(efuse_mac);

    return ret;
}
int At_EfuseReadMAC(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    uint8_t efuse_mac[6] = {0};

    ret = efuse_read_mac(efuse_mac);
    
    if(ret == 0)
    {
        printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_EFUSE_READ_MAC, efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
     
        //efuse_read_mac_increase(efuse_mac);
        //printf("MAC INCREASE=%02X:%02X:%02X:%02X:%02X:%02X\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
    }
    else
    {
        printf("No MAC in Efuse\n");
    }
    
    return ret; 
}    
int At_EfuseDump(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    int len = 0;
    uint8_t efuse_mac[6] = {0};
    uint16_t usb_vid = 0;
    uint16_t usb_pid = 0;
    uint8_t power_tx1 = 0;
    uint8_t power_tx2 = 0;
    uint8_t xtal = 0;

    len = efuse_dump_data();

    printf("efuse free bits = %d\n", efuse_get_free_bits());
    
    if( efuse_read_mac(efuse_mac) == 0 )
    {
        printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
    }
    else
    {
        printf("NO MAC\n");
    }

    if( efuse_read_usb_vid(&usb_vid) == 0 )
    {
        printf("VID = 0x%x\n", usb_vid);
    }
    else
    {
        printf("NO VID\n");
    }

    if( efuse_read_usb_pid(&usb_pid) == 0 )
    {
        printf("PID = 0x%x\n", usb_pid);
    }
    else
    {
        printf("NO PID\n");
    }
    
    if( efuse_read_tx_power1(&power_tx1) == 0 )
    {
        printf("TX Power1 = 0x%x\n", power_tx1);
    }
    else
    {
        printf("NO TX Power1\n");
    }
    
    if( efuse_read_tx_power2(&power_tx2) == 0 )
    {
        printf("TX Power1 = 0x%x\n", power_tx2);
    }
    else
    {
        printf("NO TX Power2\n");
    }

     if( efuse_read_xtal(&xtal) == 0 )
    {
        printf("XTAL = 0x%x\n", xtal);
    }
    else
    {
        printf("NO XTAL\n");
    }   
   
    return ret; 
}
//5G
int At_Rf5GBBScale(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    
    if(argc < 4)
        return ERROR_INVALID_PARAMETER;
  
    if( write_reg_5g_bbscale_5100(atoi(argv[0])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5500(atoi(argv[1])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5700(atoi(argv[2])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5900(atoi(argv[3])) != 0)
        return -1;

    return ret;
}
int At_Rf5GBias1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_5g_bias1(strtoul(argv[0], NULL, 16));
        
    return ret;
}
int At_Rf5GBias2(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_5g_bias2(strtoul(argv[0], NULL, 16));
        
    return ret;
}
int At_Rf5GTableRT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_RT, ssv_rf_table.rt_5g_config.bbscale_band0
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band1
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band2
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band3);
        return ret;
    }
    
    if(argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(argv[0]);
    tempe_config.bbscale_band1 = atoi(argv[1]);
    tempe_config.bbscale_band2 = atoi(argv[2]);
    tempe_config.bbscale_band3 = atoi(argv[3]);
    tempe_config.bias1 = ssv_rf_table.rt_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.rt_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 0 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.rt_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableHT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_HT, ssv_rf_table.ht_5g_config.bbscale_band0
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band1
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band2
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band3);
        return ret;
    }
    
    if(argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(argv[0]);
    tempe_config.bbscale_band1 = atoi(argv[1]);
    tempe_config.bbscale_band2 = atoi(argv[2]);
    tempe_config.bbscale_band3 = atoi(argv[3]);
    tempe_config.bias1 = ssv_rf_table.ht_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.ht_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 1 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.ht_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableLT(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_LT, ssv_rf_table.lt_5g_config.bbscale_band0
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band1
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band2
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band3);
        return ret;
    }
    
    if(argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(argv[0]);
    tempe_config.bbscale_band1 = atoi(argv[1]);
    tempe_config.bbscale_band2 = atoi(argv[2]);
    tempe_config.bbscale_band3 = atoi(argv[3]);
    tempe_config.bias1 = ssv_rf_table.lt_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.lt_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 2 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.lt_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret; 
}
int At_Rf5GTableBIAS1(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    uint32_t rt_bias1 = 0, ht_bias1 = 0, lt_bias1 = 0;

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=0x%x,0x%x,0x%x\n", ATCMD_RF_5GTABLE_BIAS1, ssv_rf_table.rt_5g_config.bias1
                                                                                           , ssv_rf_table.ht_5g_config.bias1
                                                                                           , ssv_rf_table.lt_5g_config.bias1);
        return ret;
    }
    
    if(argc < 3)
        return ERROR_INVALID_PARAMETER;

    rt_bias1 = strtoul(argv[0], NULL, 16);
    ht_bias1 = strtoul(argv[1], NULL, 16);
    lt_bias1 = strtoul(argv[2], NULL, 16);
    
    ret = write_reg_5g_three_bias(rt_bias1, ht_bias1, lt_bias1);

    if(ret != 0)
        return ret;

    ssv_rf_table.rt_5g_config.bias1 = rt_bias1;
    ssv_rf_table.ht_5g_config.bias1 = ht_bias1;
    ssv_rf_table.lt_5g_config.bias1 = lt_bias1;
    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableBIAS2(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    uint32_t rt_bias2 = 0, ht_bias2 = 0, lt_bias2 = 0;

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=0x%x,0x%x,0x%x\n", ATCMD_RF_5GTABLE_BIAS2, ssv_rf_table.rt_5g_config.bias2
                                                                                           , ssv_rf_table.ht_5g_config.bias2
                                                                                           , ssv_rf_table.lt_5g_config.bias2);
        return ret;
    }
    
    if(argc < 3)
        return ERROR_INVALID_PARAMETER;

    rt_bias2 = strtoul(argv[0], NULL, 16);
    ht_bias2 = strtoul(argv[1], NULL, 16);
    lt_bias2 = strtoul(argv[2], NULL, 16);
    
    ret = write_reg_5g_three_bias(rt_bias2, ht_bias2, lt_bias2);

    if(ret != 0)
        return ret;

    ssv_rf_table.rt_5g_config.bias2 = rt_bias2;
    ssv_rf_table.ht_5g_config.bias2 = ht_bias2;
    ssv_rf_table.lt_5g_config.bias2 = lt_bias2;
    
    save_rf_table_to_flash(); 

    return ret; 
}
int At_Rf5GBandThreshold(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    uint16_t thr_f0, thr_f1, thr_f2;

    if(strcmp("?" , argv[0])==0)
    {
        printf("%s=%d,%d,%d\n", ATCMD_RF_5GTABLE_BAND_THRESHOLD, ssv_rf_table.band_f0_threshold, ssv_rf_table.band_f1_threshold, ssv_rf_table.band_f2_threshold);
        return ret;
    }
    
    if(argc < 3)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    thr_f0 = strtoul(argv[0], NULL, 10);
    thr_f1 = strtoul(argv[1], NULL, 10);
    thr_f2 = strtoul(argv[2], NULL, 10);
    
    ret = write_reg_5g_band_threshold(thr_f0, thr_f1, thr_f2);

    if(ret != 0)
        return ret;
        
    ssv_rf_table.band_f0_threshold = thr_f0;
    ssv_rf_table.band_f1_threshold = thr_f1;
    ssv_rf_table.band_f2_threshold = thr_f2;
    
    save_rf_table_to_flash(); 

    return ret; 
}

void At_RfSingleTone(s32 argc, char *argv[])
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    
    if(argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = rf_tone_gen(atoi(argv[0]));

    return ret; 
}

int At_ShowRfCommand (s32 argc, char *argv[]);


typedef int (*PF_handle) (s32, char  ** );
typedef struct _at_cmd_info
{
	char*		atCmd;
	PF_handle	pfHandle;
    uint8_t maxargu;
} at_cmd_info;


const at_cmd_info atcmdicomm_info_tbl[] =
{
    {ATCMD_RF,            At_ShowRfCommand,            0},
    {ATCMD_RADIO_RF_START,            At_RadioRFStart,            2},
    {ATCMD_RADIO_CHANNEL,            At_RadioChannel,            0},
    {ATCMD_RADIO_RF_RATE,            At_RadioRFRate,            1},
    {ATCMD_RADIO_RF_BANDGAIN,            At_RadioRFBandGain,            1},
    {ATCMD_RADIO_RF_GAIN,            At_RadioRFGain,            1},
    {ATCMD_RADIO_RF_RATEGAIN,            At_RadioRFRateGain,            2},
    {ATCMD_RADIO_RF_READ_TEMPCS,            At_RadioRFReadTempcs,            0},
    {ATCMD_RADIO_RF_STOP,            At_RadioRFStop,            0},
    {ATCMD_RADIO_RF_RESET,            At_RadioRFReset,            0},
    {ATCMD_RADIO_RF_COUNT,            At_RadioRFCount,            1},
    {ATCMD_RADIO_RF_ENABLE_TCSR,   At_RadioEnableTCSR,     1},
    {ATCMD_RADIO_RF_DUMP,            At_RadioRFDump,            0},
    
    {ATCMD_RF_CALI_DPD,            At_RfCaliDPD,            1},
    {ATCMD_RF_MANU_DPD,            At_RfManuDPD,            0},        
    {ATCMD_RF_FREQOFFSET,            At_RadioRFFreqoffset,            2},
    {ATCMD_RF_FREQOFFSET_READXO,            At_RadioRFFreqoffset_readxo,            0},
    {ATCMD_RF_FREQOFFSET_READXI,            At_RadioRFFreqoffset_readxi,            0},

    {ATCMD_RADIO_RF_2GPA1,            At_RadioRF2Gpa1,            1},
    {ATCMD_RADIO_RF_2GPA2,            At_RadioRF2Gpa2,            1},
    {ATCMD_RADIO_RF_2GPA3,            At_RadioRF2Gpa3,            1},
    {ATCMD_RADIO_RF_2GPABIAS,            At_RadioRF2GpaBias,            1},
    {ATCMD_RADIO_RF_2GPACELL,            At_RadioRF2GpaCell,            1},
    {ATCMD_RADIO_RF_2GPACAP,            At_RadioRF2GpaCap,            1},    

    {ATCMD_RADIO_RF_LDO,            At_RadioRFLdo,            1},
    {ATCMD_RADIO_RF_DCDC,            At_RadioRFDcdc,            1},
    {ATCMD_RADIO_RF_DCDCV,            At_RadioRFDcdcv,            1},
    {ATCMD_RADIO_RF_DLDOV,            At_RadioRFDldov,            1},
    {ATCMD_RADIO_RF_XOLDO,            At_RadioRFXoldo,            1},    
    {ATCMD_RADIO_RF_RXPAD_CH13,            At_RadioRFRxPadCh13,            1},          

    {ATCMD_RADIO_TEMP_BOUNDARY,            At_RadioRFTempBoundary,            2},
    {ATCMD_RF_TABLE_RT,            At_RfTableRT,            18},
    {ATCMD_RF_TABLE_HT,            At_RfTableHT,            18},
    {ATCMD_RF_TABLE_LT,            At_RfTableLT,            18},
    {ATCMD_RF_TABLE_GAIN,            At_RfTableGain,            18},
    {ATCMD_RF_TABLE_BRATE_GAIN,            At_RfTableBRateGain,            1},
    {ATCMD_RF_TABLE_GRATE_GAIN,            At_RfTableGRateGain,            4},
    {ATCMD_RF_TABLE_20NRATE_GAIN,            At_RfTable20NRateGain,            4},
    {ATCMD_RF_TABLE_40NRATE_GAIN,            At_RfTable40NRateGain,            4},        

    {ATCMD_RADIO_WRITE_MAC1,            At_RadioWriteMAC1,            1},
    {ATCMD_RADIO_READ_MAC1,            At_RadioReadMAC1,            1},
    {ATCMD_RADIO_WRITE_MAC2,            At_RadioWriteMAC2,            1},
    {ATCMD_RADIO_READ_MAC2,            At_RadioReadMAC2,            1},    
    
    {ATCMD_EFUSE_ENABLE_WRITE,            At_EfuseEnableWrite,            0},
    {ATCMD_EFUSE_WRITE_MAC,            At_EfuseWriteMAC,            1},
    {ATCMD_EFUSE_READ_MAC,            At_EfuseReadMAC,            1},        
    {ATCMD_EFUSE_DUMP,            At_EfuseDump,            0},       

    {ATCMD_RF_5G_BB_SCALE,            At_Rf5GBBScale,            0},   
    {ATCMD_RF_5G_BIAS1,            At_Rf5GBias1,            0},   
    {ATCMD_RF_5G_BIAS2,            At_Rf5GBias2,            0},   
    
    {ATCMD_RF_5GTABLE_RT,            At_Rf5GTableRT,            0},   
    {ATCMD_RF_5GTABLE_HT,            At_Rf5GTableHT,            0},   
    {ATCMD_RF_5GTABLE_LT,            At_Rf5GTableLT,            0},    
    {ATCMD_RF_5GTABLE_BIAS1,            At_Rf5GTableBIAS1,            0},      
    {ATCMD_RF_5GTABLE_BIAS2,            At_Rf5GTableBIAS2,            0},          
    {ATCMD_RF_5GTABLE_BAND_THRESHOLD,            At_Rf5GBandThreshold,            0},
    
    {ATCMD_RADIO_RF_SINGLE_TONE,            At_RfSingleTone,            0},


    { (const char *)NULL, (PF_handle)NULL,   0 },
};

int At_ShowRfCommand (s32 argc, char *argv[])
{
    int i = 0;
    printf("\r\n");
    int num = sizeof(atcmdicomm_info_tbl)/sizeof(at_cmd_info);
    for(i = 0;i<num-1;i++)
    {
        if( strncmp(atcmdicomm_info_tbl[i].atCmd, "AT+RF", strlen("AT+RF")) == 0 )
            printf("%s\n", atcmdicomm_info_tbl[i].atCmd);
    }

    return ERROR_SUCCESS;
}

void At_RespOK (char* pStr)
{
	char szResp[64] = {0};
	int len = strlen(pStr);
	if( *(pStr+len-1) == '?' && *(pStr+len-2) == '=' )
		len-=2;
	else if( *(pStr+len-1) == '=' )
		len-=1;
	memcpy(szResp, pStr, len);
	//printf("%s=OK\n", szResp);
    printf("%s=", szResp);
    printf("OK\n");	
}


static char *sgArgV[20];
static u32 sgArgC; 

void ssv_dbg_cmd_parse(char *pwbuf, int blen, int argc, char **argv)
{
	uint8_t ch;
    uint8_t *pch;
	const at_cmd_info *CmdPtr;
	int	nRet = ERROR_UNKNOWN_COMMAND;

	for( sgArgC=0,ch=0, pch=argv[1]; (*pch!=0x00)&&(sgArgC<20); pch++ )
	{
		if ( (ch==0) && (*pch!=' ') )
		{
			ch = 1;
			sgArgV[sgArgC] = pch;
		}
	
		if ( (ch==1) && (*pch==' ') )
		{
			*pch = 0x00;
			ch = 0;
			sgArgC ++;
		}
	
		if ( (ch==1) && (*pch=='=') && (sgArgC == 0))
		{
			*pch = 0x00;
			ch = 0;
			sgArgC ++;
		}
	
		if ( (ch==1) && (*pch==',') && (sgArgC != 0))
		{
			*pch = 0x00;
			ch = 0;
			sgArgC ++;
		}
	}
	if ( ch == 1)
	{
		sgArgC ++;
	}
	else if ( sgArgC > 0 )
	{
		*(pch-1) = ' ';
	}
	
	if ( sgArgC > 0 )
	{
		/* Dispatch command */
		for( CmdPtr=atcmdicomm_info_tbl; CmdPtr->atCmd; CmdPtr ++ )
		{
			if ( !strcmp(sgArgV[0], CmdPtr->atCmd) )
			{
				//printf("\n");
				fflush(stdout);
				nRet = CmdPtr->pfHandle( sgArgC-1, &sgArgV[1] );
				break;
			}
		}
		if (ERROR_SUCCESS > nRet){
			printf ( "+ERROR:%d\r\n", nRet);
   		 }else if(ERROR_SUCCESS == nRet)
        	At_RespOK(CmdPtr->atCmd);
	}

}


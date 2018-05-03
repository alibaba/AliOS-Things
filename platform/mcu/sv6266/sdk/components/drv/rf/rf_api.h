#ifndef _RF_API_H_
#define _RF_API_H_

struct st_tempe_table 
{
    uint8_t band_gain[7];   //0:ch1~ch2, 1:ch2~ch3, ..., 6:ch13~ch14
   
    uint8_t freq_xi;
    uint8_t freq_xo;
    
    uint8_t ldo_rxafe;
    uint8_t ldo_dcdcv;
    uint8_t ldo_dldov;
    
    uint8_t pa_vcas1;
    uint8_t pa_vcas2;
    uint8_t pa_vcas3;
    uint8_t pa_bias;
    uint8_t pa_cap;
    
    uint8_t padpd_cali;
};

struct st_rate_gain 
{
    uint8_t rate1;
    uint8_t rate2;
    uint8_t rate3;
    uint8_t rate4;
};

struct st_rf_table 
{
    struct st_tempe_table rt_config;
    struct st_tempe_table ht_config;
    struct st_tempe_table lt_config; 
  
    uint8_t rf_gain;
    uint8_t rate_gain_b;

    struct st_rate_gain rate_config_g;

    struct st_rate_gain rate_config_20n;

    struct st_rate_gain rate_config_40n;
    int8_t low_boundary;
    int8_t high_boundary;
};
int load_phy_table();
int dump_rf_value();
int dump_rf_table();
int load_rf_table_from_flash();
int save_rf_table_to_flash();
int get_current_temperature(int *pvalue);
int get_current_tempe_state(int8_t low_boundary, int8_t high_boundary);
int do_temerature_compensation();
int check_rate_gain_value(struct st_rate_gain rate_gain, uint8_t flag);
uint32_t flag_to_mask(uint8_t flag);
int write_reg_band_gain(uint8_t gain_index);
int write_reg_freq_xi_xo(uint8_t xi, uint8_t xo, uint8_t mask);
int write_reg_rxafe(uint8_t rxafe);
int write_reg_dcdc(uint8_t dcdc);
int write_reg_xoldo(uint8_t xoldo);
int write_reg_rxpad_ch13(uint8_t bflag);
int write_reg_ldo(uint8_t dcdcv, uint8_t dldov, uint32_t mask);
int write_reg_2gpa(uint8_t vcas1, uint8_t vcas2, uint8_t vcas3, uint8_t bias, uint32_t mask);
int write_reg_2gpa_cap(uint8_t pa_cap);
int write_reg_2gpa_cell(uint8_t pa_cell);
int write_reg_padpd_cali(uint8_t padpd_cali);
int write_reg_tempe_table(struct st_tempe_table config);
int write_reg_rf_gain(uint8_t gain_index);
int write_reg_rate_gain_b(uint8_t gain_b);
int write_reg_rate_gain_g(struct st_rate_gain rate_g, uint8_t wrk_flag);
int write_reg_rate_gain_20n(struct st_rate_gain rate_20n, uint8_t wrk_flag);
int write_reg_rate_gain_40n(struct st_rate_gain rate_40n, uint8_t wrk_flag);
int write_reg_rf_table();
int set_temper_value(uint8_t low, uint8_t high);

int rf_tx_start(int count, int interval);
int rf_set_ch(int32_t ch, uint8_t withRFDisable , uint32_t ch_type);
int rf_set_rate(int rate_id);
int rf_tx_stop();
int rf_rx_reset();
int rf_bmode_count(uint32_t *total_count, uint32_t *err_count);
int rf_gmode_count(uint32_t *total_count, uint32_t *err_count);
int rf_freqoffset_readxo(uint32_t *xo_value);
int rf_freqoffset_readxi(uint32_t *xi_value);
int rf_manu_padpd_cali();
int rf_debug_message(int level);

#endif

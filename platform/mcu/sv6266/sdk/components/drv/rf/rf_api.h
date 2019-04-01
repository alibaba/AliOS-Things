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

struct st_tempe_5g_table 
{
    uint8_t bbscale_band0;   //Band0 in 0xccb0ada8(31-24)
    uint8_t bbscale_band1;   //Band1 in 0xccb0ada8(23-16)
    uint8_t bbscale_band2;   //Band2 in 0xccb0ada8(15- 8)
    uint8_t bbscale_band3;   //Band3 in 0xccb0ada8( 7- 0)
    uint32_t bias1;                  // Band0 in 0xccb0a62c(15-0), Band1 in 0xccb0a62c(31-16)
    uint32_t bias2;                  // Band2 in 0xccb0a630(15-0), Band3 in 0xccb0a630(31-16)   
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
    /*0xFF: EN_FIRST_BOOT, 0:EN_NOT_FIST_BOOT*/
    uint8_t boot_flag;
    /*0:EN_WORK_NOMAL, 1:EN_WORK_ENGINEER*/
    uint8_t work_mode; 
    struct st_tempe_5g_table rt_5g_config;
    struct st_tempe_5g_table ht_5g_config;
    struct st_tempe_5g_table lt_5g_config;
    uint16_t band_f0_threshold;  
    uint16_t band_f1_threshold;
    uint16_t band_f2_threshold; 
};

enum{
    EN_FIRST_BOOT=0xFF,
    EN_NOT_FIRST_BOOT=0,
};

enum{
    EN_WORK_NOMAL=0,
    EN_WORK_ENGINEER
};

int dump_rf_value();
int dump_rf_table();
int build_default_rf_table(struct st_rf_table *pTable);
int load_rf_table_to_mac(struct st_rf_table *pTable);
int get_current_temperature(int *pvalue);
int get_current_tempe_state(int8_t low_boundary, int8_t high_boundary);
int do_temerature_compensation();
int check_rate_gain_value(struct st_rate_gain rate_gain, uint8_t flag);
uint32_t flag_to_mask(uint8_t flag);
int write_reg_band_gain(uint8_t gain_index);
int write_reg_freq_xi_xo(uint8_t xi, uint8_t xo, uint8_t mask);
int write_reg_rxfe(uint8_t rxafe);
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
int write_reg_rf_table_ex(struct st_rf_table *p_table);
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
int rf_enable_tcsr(uint8_t enable);
int rf_set_ch_ht40(int32_t ch_ht40);
#if 1
int write_reg_5g_bbscale_5100(uint8_t scale);
int write_reg_5g_bbscale_5500(uint8_t scale);
int write_reg_5g_bbscale_5700(uint8_t scale);
int write_reg_5g_bbscale_5900(uint8_t scale);
int write_reg_5g_bias1(uint32_t reg_value);
int write_reg_5g_bias2(uint32_t reg_value);
int write_reg_5g_band_threshold(uint16_t thr_f0, uint16_t thr_f1, uint16_t thr_f2);
int write_reg_tempe_5g_table(struct st_tempe_5g_table config);
#endif

int rf_tone_gen(uint8_t bStart);

int read_reg_band_gain_config(u8 *value);
int read_reg_5g_bbscale_5100_config(u8 *value);
int read_reg_5g_bbscale_5500_config(u8 *value);
int read_reg_5g_bbscale_5700_config(u8 *value);
int read_reg_5g_bbscale_5900_config(u8 *value);
int read_reg_freq_offset_config(u8 *freq_xi, u8 *freq_xo);
#endif

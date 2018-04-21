#ifndef RDA_CCFG_API_H
#define RDA_CCFG_API_H

#ifdef __cplusplus
extern "C" {
#endif

void rda_ccfg_aontmr(void);
void rda_ccfg_gp6(unsigned short cfg);
void rda_ccfg_gp7(unsigned short cfg);
void rda_ccfg_gp(unsigned char gp, unsigned short cfg);
void rda_ccfg_ckrst(void);
void rda_ccfg_perrst(void);
void rda_ccfg_adc_init(void);
void rda_ccfg_adc_gp(unsigned char gp, unsigned short cfg);
unsigned short rda_ccfg_adc_read(unsigned char ch);
void rda_ccfg_adc_free(void);
int rda_ccfg_abort_flag(void);
#if RDA5991H_HW_VER >= 2
void rda_ccfg_wdt_en(void);
#endif /* RDA5991H_HW_VER */
int rda_ccfg_hwver(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif


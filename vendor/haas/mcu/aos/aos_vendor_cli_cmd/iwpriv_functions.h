#ifndef __IWPRIV_FUNCTIONS_H__
#define __IWPRIV_FUNCTIONS_H__

void iwpriv_usage();

int mp_start();

int mp_channel(int channel);

int mp_bandwidth(int bw_40M,int shortGI);

int mp_ant_tx(char *ant_id);

int mp_ant_rx(char *ant_id);

int mp_txpower(int patha,int pathb);

int mp_rate(int rate);

int mp_ctx_pkt(int percent);

int mp_ctx_stop();

int mp_phypara(int xcap);

int mp_ther();

int mp_reset_stats();

int mp_arx_start();

int mp_arx_stop();

int mp_arx_phy();

int mp_stop();

int efuse_set(char *efuse_para);

int efuse_masks(char *efuse_para);

int efuse_get(char *efuse_para);

#endif /*__IWPRIV_FUNCTIONS_H__*/

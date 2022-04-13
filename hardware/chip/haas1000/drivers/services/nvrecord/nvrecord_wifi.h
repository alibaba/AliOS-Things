#ifndef _NVRECORD_WIFI_H
#define _NVRECORD_WIFI_H
#ifdef __cplusplus
extern "C" {
#endif

#include "../../net/bwifi_sta.h"

#define WIFI_RECORD_TYPE_SIZE    (3+1)

typedef  struct wifi_volume{
    int8_t a2dp_vol;
    int8_t hfp_vol;
}wifi_volume;

typedef struct _wifi_cal{
	uint16 freq_cal;
    uint16 freq_cal_5G;
	uint32 iQ_cal;
	uint32 iQ_offset;
	uint32 iQ_cal_5G;
	uint32 iQ_offset_5G;
	uint16 tx_power[14];
	uint16 tx_power_5G[5];
}wifi_cal;

#ifndef PMK_LEN
#define PMK_LEN 32
#endif
struct nvrec_wifi_station_data {
    struct bwifi_station_config config;
    u8 pmk[PMK_LEN];
    int pmk_set;
};
typedef struct _nvrec_wifidevicerecord
{
    char type[WIFI_RECORD_TYPE_SIZE];//"cf0","bnd","cny"
    union {
        //u32 conf_bit_map;
        struct nvrec_wifi_station_data sta_data;
        struct bwifi_quick_connect_config quick_config;
        wifi_volume volume;
        wifi_cal cal;
	    char band; //0:2.4 1:5 2:dual band
	    char country[3];
    }wifi_record;
} nvrec_wifidevicerecord;
#define BWIFI_RECORD_TYPE_VOLUME "vol"
#define BWIFI_RECORD_TYPE_CFG(n) ("cf"#n)
#define BWIFI_RECORD_QUICK_CONNECT "qcn"
#define BWIFI_RECORD_TYPE_CAL "cal"
#define BWIFI_RECORD_TYPE_BAND "bnd"
#define BWIFI_RECORD_TYPE_COUNTRY "cny"

#define WIFI_CONF_RECORD_NUM  4
static inline const char *_conf_id_2_type(u32 conf_id)
{
	switch (conf_id) {
	case 0:
		return BWIFI_RECORD_TYPE_CFG(0);
	case 1:
		return BWIFI_RECORD_TYPE_CFG(1);
	case 2:
		return BWIFI_RECORD_TYPE_CFG(2);
	case 3:
		return BWIFI_RECORD_TYPE_CFG(3);
	case 4:
		return BWIFI_RECORD_TYPE_CFG(4);
	case 5:
		return BWIFI_RECORD_TYPE_CFG(5);
	case 6:
		return BWIFI_RECORD_TYPE_CFG(6);
	case 7:
		return BWIFI_RECORD_TYPE_CFG(7);
	default:
		return "err";
	}
}
static inline int  bwifi_unknown_record_type(const char *type)
{
	int i;
    if (strcmp(type, BWIFI_RECORD_TYPE_VOLUME) == 0)
		return 0;
	if (strcmp(type, BWIFI_RECORD_QUICK_CONNECT) == 0)
		return 0;
    if (strcmp(type, BWIFI_RECORD_TYPE_CAL) == 0)
		return 0;
    if (strcmp(type, BWIFI_RECORD_TYPE_BAND) == 0)
		return 0;
    if (strcmp(type, BWIFI_RECORD_TYPE_COUNTRY) == 0)
		return 0;
	for (i=0; i<WIFI_CONF_RECORD_NUM; ++i)
		if (strcmp(type, _conf_id_2_type(i)) == 0)
			return 0;
	return 1;
}

int nv_record_wifirec_find(const char *type, nvrec_wifidevicerecord *record);
int nv_record_wifirec_delete(const char *type);
int nv_record_wifirec_add(const nvrec_wifidevicerecord *record);

int config_entries_wifidev_delete_head_cfg(void);
void nv_record_flash_flush(void);

#ifdef __cplusplus
}
#endif
#endif /*_NVRECORD_WIFI_H*/

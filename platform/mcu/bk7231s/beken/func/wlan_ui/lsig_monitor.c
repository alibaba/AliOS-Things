/* use LSIG to do monitor */
#include "include.h"
#include "common.h"

#include "wlan_ui_pub.h"
#include "rw_pub.h"
#include "vif_mgmt.h"

#include "sa_station.h"
#include "param_config.h"
#include "common/ieee802_11_defs.h"
#include "driver_beken.h"
#include "mac_ie.h"
#include "sa_ap.h"
#include "main_none.h"
#include "sm.h"
#include "mac.h"
#include "scan_task.h"
#include "hal_machw.h"

#define MAX_RTS_TBL 32

/* Most ofr RTS is STA send to DS. */
struct rts_cts_st {
    uint8_t ra[6];    // the receive address
    uint8_t ta[6];    // the transmite address
    int8_t  rssi;
    uint8_t security; 
    uint16_t seq;
    uint32_t last_len;
    uint32_t last_time;
};

struct rts_cts_pkt_info {
    uint8_t  ra[6];    // the receive address
    uint8_t  ta[6];    // the transmite address
    int8_t   rssi;
    uint32_t time_ms;
};

static struct rts_cts_st rts_tbl[MAX_RTS_TBL];
static struct rts_cts_pkt_info last_rts;
static int rts_tbl_num = 0;

void lsig_init(void)
{
    rts_tbl_num = 0;
    os_memset(&last_rts, 0 , sizeof(last_rts));
}

void rts_update(uint8_t *data, int8_t rssi, uint32_t time_ms)
{
    os_memcpy(last_rts.ra, &data[4], 12);
    last_rts.rssi = -rssi;
    last_rts.time_ms = time_ms;
}

void beacon_update(uint8_t *bssid, uint8_t security)
{
    int i;

    for(i=0; i<rts_tbl_num; i++) {
        if (rts_tbl[i].security != 0xFF)
            continue;
        if (os_memcmp(rts_tbl[i].ra, bssid, 6) == 0) {
            rts_tbl[i].security = security;
            rts_tbl[i].seq = 1;
        }
    }
}

static void report_lsig(int len, int8_t rssi, int index)
{
    hal_wifi_link_info_t info;
	monitor_data_cb_t fn;
    uint8_t data[64]; // mac header & qos & vector
    uint16_t seq_frag;
    int i = 0;
    uint32_t time = rtos_get_time();
    
    if (rts_tbl[index].security == 0xFF) {
        return;
    }
    
    if ((len == rts_tbl[index].last_len) && (time < rts_tbl[index].last_time+2)) {
        rts_tbl[index].last_time = time;
        return;
    }

    if (len < 24)
        return;
    
    info.rssi = -rssi;

    data[i++] = 0x88; // data
    if (rts_tbl[index].security == WLAN_ENC_OPEN) {
        data[i++] = 0x1;
    } else {
        data[i++] = 0x41; // protect data; 
    }

    data[i++] = 0x30; // duration
    data[i++] = 0;
    
    os_memcpy(&data[i], rts_tbl[index].ra, 6);
    i+=6;
    os_memcpy(&data[i], rts_tbl[index].ta, 6);
    i+=6;
    os_memset(&data[i], 0xFF, 6);
    i+=6;
    
    seq_frag = rts_tbl[index].seq;
    seq_frag <<= 4;
    os_memcpy(&data[i], &seq_frag, 2);
    i+=2;

    data[i++] = 0;// QOS
    data[i++] = 0;

    os_memset(&data[i], 0, 4);
    if (rts_tbl[index].security == WLAN_ENC_CCMP) {
        data[i+3] = 0x20;
    } else if (rts_tbl[index].security == WLAN_ENC_TKIP) {
        data[i+3] = 0x20;
        data[i] = 1;
        data[i+1] = ((data[i] | 0x20) & 0x7F);
    }

    rts_tbl[index].seq++;
    rts_tbl[index].last_len = len;
    rts_tbl[index].last_time = time;

	fn = bk_wlan_get_monitor_cb();		
	(*fn)((uint8_t *)data, len, &info);
}

void lsig_input(int len, int8_t rssi, uint32_t time_ms)
{
    int rts_match = 0, reported = 0;
    int i, delta;
    uint32_t min_time = 0, rep_index = 0;

    len -= 8; // remove FCS 
    
    if (last_rts.time_ms + 1 > time_ms) {//  RTS match
        rts_match = 1;
    }
    for(i=0; i<rts_tbl_num; i++) {
        if (rts_match == 1) {
            if (os_memcmp(last_rts.ra, rts_tbl[i].ra, 12) == 0) {
                report_lsig(len, rssi, i);
                reported++;
                break;
            }
        } else {
            delta = rts_tbl[i].rssi - rssi;
            if ((delta>-5) && (delta<5)) { // DO report for each RSSI match. 
                report_lsig(len, rssi, i);
                reported++;
            }
        }
    }
    if ((rts_match != 1) || (reported != 0))
        return;
    
    // new entry
    if (rts_tbl_num < MAX_RTS_TBL) {
        os_memcpy(rts_tbl[rts_tbl_num].ra, last_rts.ra, 12);
        rts_tbl[rts_tbl_num].security = 0xFF;
        rts_tbl[rts_tbl_num].rssi = rssi;
        rts_tbl[rts_tbl_num].last_time = rtos_get_time();
        rts_tbl[rts_tbl_num].last_len = len;
        rts_tbl_num++;
        return;
    }

    // replace old entry.
    rep_index = 0;
    min_time = rts_tbl[0].last_time;
    for(i=1; i<MAX_RTS_TBL; i++) {
        if (min_time > rts_tbl[i].last_time) {
            min_time = rts_tbl[i].last_time;
            rep_index = i;
        }
    }

    os_memcpy(rts_tbl[rep_index].ra, last_rts.ra, 12);
    rts_tbl[rep_index].security = 0xFF;
    rts_tbl[rep_index].rssi = rssi;
    rts_tbl[rep_index].last_len = len;
    rts_tbl[rep_index].last_time = rtos_get_time();
}



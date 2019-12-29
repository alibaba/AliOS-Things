/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LoRaMac.h"
#include "Region.h"
#include "linkwan_ica_at.h"
#include "lorawan_port.h"
#include "linkwan.h"



#define ATCMD_SIZE 256
#define PORT_LEN   4


#define LORAWAN_APP_DATA_BUFF_SIZE 256
#define LINKWAN_APP_DATA_SIZE 51


uint8_t  atcmd[ATCMD_SIZE];
uint16_t atcmd_index = 0;


static uint8_t  lora_join_mode = 0;
static uint8_t  lora_work_mode;
static uint32_t lora_repeater_freq;

static uint8_t  lora_reportmode;
static uint32_t lora_report_cycle;

static uint8_t  lora_rx1DRoffset;
static uint8_t  lora_rx2DataRate;
static uint32_t lora_rx2Frequency;

static uint8_t  lora_freqlist_method;
static uint8_t  lora_ulfreq_num;
static uint8_t  lora_dlfreq_num;
static uint32_t lora_ulfreqlist[8];
static uint32_t lora_dlfreqlist[8];

static uint32_t lora_rx1delay;

// this can be in intrpt context
void linkwan_serial_input(uint8_t cmd)
{
    if ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'z') ||
        (cmd >= 'A' && cmd <= 'Z') || cmd == '?' || cmd == '+' || cmd == ':' ||
        cmd == '=' || cmd == ' ' || cmd == ',') {
        atcmd[atcmd_index++] = cmd;
    } else if (cmd == '\r' || cmd == '\n') {
        atcmd[atcmd_index] = '\0';
    }

    if (atcmd_index >= ATCMD_SIZE) {
        atcmd_index = 0;
    }
}

static uint8_t get_lora_join_mode(void)
{
    return lora_join_mode;
}

static void set_lora_join_mode(uint8_t join_mode)
{
    lora_join_mode = join_mode;
}

static void set_work_mode(uint8_t work_mode)
{
    lora_work_mode = work_mode;
}

static uint8_t get_work_mode(void)
{
    return lora_work_mode;
}

static void set_repeater_freq(uint32_t repeater_freq)
{
    lora_repeater_freq = repeater_freq;
}

static uint32_t get_repeater_freq(void)
{
    return lora_repeater_freq;
}

static uint8_t get_reportmode(void)
{
    return lora_reportmode;
}

static void set_reportmode(uint8_t report_mode)
{
    lora_reportmode = report_mode;
}

static uint32_t get_reportcycle(void)
{
    return lora_report_cycle;
}

static void set_reportcycle(uint32_t report_cycle)
{
    lora_report_cycle = report_cycle;
}

static void get_rxparam(uint8_t *rx1DRoffset, uint8_t *rx2DataRate, uint32_t *rx2Frequency)
{
    *rx1DRoffset = lora_rx1DRoffset;
    *rx2DataRate = lora_rx2DataRate;
    *rx2Frequency = lora_rx2Frequency;
}

static void set_rxparam(uint8_t rx1DRoffset, uint8_t rx2DataRate, uint32_t rx2Frequency)
{
    lora_rx1DRoffset = rx1DRoffset;
    lora_rx2DataRate = rx2DataRate;
    lora_rx2Frequency = rx2Frequency;
}

static bool set_freqlist(uint8_t method, uint8_t uldl, uint8_t number, uint32_t *freqlist)
{
    uint8_t         i;

    if (number > 8) {
        return false;
    }

    lora_freqlist_method = method;

    if (method == 1) {
        if (uldl == 1) {
            lora_ulfreq_num = number;
            for (i = 0; i < number; i++) {
                lora_ulfreqlist[i] = freqlist[0] + 200000 * i;
            }
        } else if (uldl == 2) {
            lora_dlfreq_num = number;
            for (i = 0; i < number; i++) {
                lora_dlfreqlist[i] = freqlist[0] + 200000 * i;
            }
        } else {
            return false;
        }
    } else if (method == 2) {
        if (uldl == 1) {
            lora_ulfreq_num = number;
            for (i = 0; i < number; i++) {
                lora_ulfreqlist[i] = freqlist[i];
            }
        } else if (uldl == 2) {
            lora_dlfreq_num = number;
            for (i = 0; i < number; i++) {
                lora_dlfreqlist[i] = freqlist[i];
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    if (get_lora_freq_mode() == 1) {
        for (i = 0; i < number; i++) {
            lora_dlfreqlist[i] = lora_ulfreqlist[i];
        }
    }

    return true;
}

void get_freqlist(uint8_t *method, uint8_t uldl, uint8_t *number, uint32_t *freqlist)
{
    uint8_t     i;

    *method = lora_freqlist_method;

    if (*method == 1) {
        *number = 1;
    } else {
        if (uldl == 1) {
            *number = lora_ulfreq_num;
        } else {
            *number = lora_dlfreq_num;
        }
    }

    if (uldl == 1) {
        for (i = 0; i < *number; i++) {
            freqlist[i] = lora_ulfreqlist[i];
        }
    } else if (uldl == 2) {
        for (i = 0; i < *number; i++) {
            freqlist[i] = lora_dlfreqlist[i];
        }
    }
}

static void set_rx1_delay(uint32_t rx1_delay)
{
    lora_rx1delay = rx1_delay * 1000;
}

static uint8_t get_rx1_delay(void)
{
    return lora_rx1delay / 1000;
}

bool ica_at_joinmode(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CJOINMODE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CJOINMODE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"mode\"\r\nOK\r\n",
                 LORA_AT_CJOINMODE);
    } else if (at_cmd_len == (strlen(LORA_AT_CJOINMODE) + 1) &&
               at_cmd[strlen(LORA_AT_CJOINMODE)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                 LORA_AT_CJOINMODE, get_lora_join_mode());
    } else if (at_cmd_len == (strlen(LORA_AT_CJOINMODE) + 2) &&
               at_cmd[strlen(LORA_AT_CJOINMODE)] == '=') {
        int mode = strtol(&at_cmd[strlen(LORA_AT_CJOINMODE) + 1], NULL, 0);
        if (mode == 0 || mode == 1) {
            set_lora_join_mode(mode);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_deveui(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     eui[16];
    uint32_t    length;

    if (at_cmd_len == (strlen(LORA_AT_CDEVEUI) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CDEVEUI)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"DevEUI:length is 16\"\r\n",
                 LORA_AT_CDEVEUI);
    } else if (at_cmd_len == (strlen(LORA_AT_CDEVEUI) + 1) &&
               at_cmd[strlen(LORA_AT_CDEVEUI)] == '?') {
        get_lora_dev_eui(eui);
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n",
                 LORA_AT_CDEVEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                 eui[5], eui[6], eui[7]);
    } else if (at_cmd_len == (strlen(LORA_AT_CDEVEUI) + 17) &&
               at_cmd[strlen(LORA_AT_CDEVEUI)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CDEVEUI) + 1], eui, 8);
        if (length == 8) {
            set_lora_dev_eui(eui);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_appeui(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     eui[16];
    uint32_t    length;

    if (at_cmd_len == (strlen(LORA_AT_CAPPEUI) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CAPPEUI)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppEUI:length is 16\"\r\n",
                 LORA_AT_CAPPEUI);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPEUI) + 1) &&
               at_cmd[strlen(LORA_AT_CAPPEUI)] == '?') {
        get_lora_app_eui(eui);
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n",
                 LORA_AT_CAPPEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                 eui[5], eui[6], eui[7]);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPEUI) + 17) &&
               at_cmd[strlen(LORA_AT_CAPPEUI)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CAPPEUI) + 1], eui, 8);
        if (length == 8) {
            set_lora_app_eui(eui);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_appkey(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     key[16];
    uint32_t    length;

    if (at_cmd_len == (strlen(LORA_AT_CAPPKEY) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CAPPKEY)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppKey:length is 32\"\r\n",
                 LORA_AT_CAPPKEY);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPKEY) + 1) &&
               at_cmd[strlen(LORA_AT_CAPPKEY)] == '?') {
        get_lora_app_key(key);
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%"
                 "02x%02x%02x%02x\r\nOK\r\n",
                 LORA_AT_CAPPKEY, key[0], key[1], key[2], key[3], key[4],
                 key[5], key[6], key[7], key[8], key[9], key[10], key[11],
                 key[12], key[13], key[14], key[15]);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPKEY) + 33) &&
               at_cmd[strlen(LORA_AT_CAPPEUI)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CAPPKEY) + 1], key, 16);
        if (length == 16) {
            set_lora_app_key(key);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_devaddr(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         devaddr[4];
    uint32_t        length;

    if (at_cmd_len == (strlen(LORA_AT_CDEVADDR) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CDEVADDR)], "=?") == 0) {
        snprintf(
          atcmd, ATCMD_SIZE,
          "\r\n%s=<DevAddr:length is 8, Device address of ABP mode>\r\n",
          LORA_AT_CDEVADDR);
    } else if (at_cmd_len == (strlen(LORA_AT_CDEVADDR) + 1) &&
               at_cmd[strlen(LORA_AT_CDEVADDR)] == '?') {
        get_lora_devaddr(devaddr);

        length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%02x%02x%02x%02x",
                          LORA_AT_CDEVADDR, devaddr[0], devaddr[1],
                          devaddr[2], devaddr[3]);
        snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
    } else if (at_cmd_len == (strlen(LORA_AT_CDEVADDR) + 9) &&
               at_cmd[strlen(LORA_AT_CDEVADDR)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CDEVADDR) + 1], devaddr, 4);
        if (length == 4) {
            set_lora_devaddr(devaddr);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_appskey(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         appskey[16];
    uint32_t        length;
    uint8_t         i;

    if (at_cmd_len == (strlen(LORA_AT_CAPPSKEY) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CAPPSKEY)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=<AppSKey: length is 32>\r\n",
                 LORA_AT_CAPPSKEY);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPSKEY) + 1) &&
               at_cmd[strlen(LORA_AT_CAPPSKEY)] == '?') {
        get_lora_appskey(appskey);
        length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CAPPSKEY);
        for (i = 0; i < 16; i++) {
            length +=
              snprintf(atcmd + length, ATCMD_SIZE, "%02x", appskey[i]);
        }
        snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPSKEY) + 33) &&
               at_cmd[strlen(LORA_AT_CAPPSKEY)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CAPPSKEY) + 1], appskey, 16);
        if (length == 16) {
            set_lora_appskey(appskey);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_nwkskey(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         nwkskey[16];
    uint32_t        length;
    uint8_t         i;

    if (at_cmd_len == (strlen(LORA_AT_CNWKSKEY) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CNWKSKEY)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=<NwkSKey:length is 32>\r\n",
                 LORA_AT_CNWKSKEY);
    } else if (at_cmd_len == (strlen(LORA_AT_CNWKSKEY) + 1) &&
               at_cmd[strlen(LORA_AT_CNWKSKEY)] == '?') {
        get_lora_nwkskey(nwkskey);
        length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CNWKSKEY);
        for (i = 0; i < 16; i++) {
            length +=
              snprintf(atcmd + length, ATCMD_SIZE, "%02x", nwkskey[i]);
        }
        snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
    } else if (at_cmd_len == (strlen(LORA_AT_CNWKSKEY) + 33) &&
               at_cmd[strlen(LORA_AT_CNWKSKEY)] == '=') {
        length = hex2bin(&at_cmd[strlen(LORA_AT_CNWKSKEY) + 1], nwkskey, 16);
        if (length == 16) {
            set_lora_nwkskey(nwkskey);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

uint8_t g_multicast_mckey[4][16];

void multicast_show(void)
{
    uint8_t             i;
    char                *str;
    uint8_t             mc_info[256];
    MulticastChannel_t  multicast_channel;

    str = (char *)mc_info;

    memset(mc_info, '\0', 256);

    for (i = 0; i < LORAMAC_MAX_MC_CTX; i++) {
        if (LORAMAC_STATUS_OK == LoRaMacMulticastChannelGet(i, &multicast_channel)) {
            snprintf(str, 256, "%08x,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%d\r\n",
                                multicast_channel.Address, g_multicast_mckey[i][0], g_multicast_mckey[i][1], g_multicast_mckey[i][2],
                                g_multicast_mckey[i][3], g_multicast_mckey[i][4], g_multicast_mckey[i][5],
                                g_multicast_mckey[i][6], g_multicast_mckey[i][7], g_multicast_mckey[i][8],
                                g_multicast_mckey[i][9], g_multicast_mckey[i][10], g_multicast_mckey[i][11],
                                g_multicast_mckey[i][12], g_multicast_mckey[i][13], g_multicast_mckey[i][14],
                                g_multicast_mckey[i][15], multicast_channel.Frequency,
                                multicast_channel.Datarate, multicast_channel.Periodicity);

            str += strlen(str);
        }
    }

    snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%sOK\r\n", LORA_AT_CADDMULTICAST, mc_info);
}

bool ica_at_addmulticast(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool       ret= true;
    uint32_t   mc_devaddr;
    char       *str;
    uint8_t    mc_key[16];
    uint32_t   frequency = 0;
    uint8_t    data_rate = 0;
    uint8_t    periodicity = 0;
    int8_t     mc_pos;

    if (at_cmd_len == (strlen(LORA_AT_CADDMULTICAST) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CADDMULTICAST)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:\"DevAddr\",\"McKey\",\"Frequency\",\"DataRate\",\"Periodicity\"\r\nOK\r\n",
                 LORA_AT_CADDMULTICAST);
    } else if (at_cmd_len == (strlen(LORA_AT_CADDMULTICAST) + 1) &&
        at_cmd[strlen(LORA_AT_CADDMULTICAST)] == '?') {

        if (multicast_get_num() > 0) {
            multicast_show();
        } else {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:NO MULTICAST INFO\r\n",
                     LORA_AT_CADDMULTICAST);
        }
    } else if (at_cmd_len > (strlen(LORA_AT_CADDMULTICAST) + 1) &&
               at_cmd[strlen(LORA_AT_CADDMULTICAST)] == '=') {
        str = strtok(&at_cmd[strlen(LORA_AT_CADDMULTICAST) + 1], ",");
        hex2bin(str, (uint8_t *)&mc_devaddr, 8);
        mc_devaddr = (mc_devaddr << 24) & (0xFF000000) |
                     (mc_devaddr << 8) & (0x00FF0000) |
                     (mc_devaddr >> 8) & (0x0000FF00) |
                     (mc_devaddr >> 24) & (0x000000FF);

        str = strtok(NULL, "\,");
        hex2bin(str, mc_key, 16);

        str = strtok(NULL, "\,");

        if (str) {
            frequency = strtol(str, NULL, 0);
        } else {
            str = strtok(NULL, "\r\n");
            frequency = strtol(str, NULL, 0);
        }

        str = strtok(NULL, "\,");
        data_rate = strtol(str, NULL, 0);

        str = strtok(NULL, "\r\n");
        periodicity = strtol(str, NULL, 0);

        mc_pos = multicast_add(mc_devaddr, frequency, data_rate, periodicity, mc_key);

        if (-1 == mc_pos) {
            ret = false;
        } else {
            memcpy(g_multicast_mckey[mc_pos], mc_key, 16);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_delmulticast(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint32_t            devAddr;
    char                *str;
    bool                ret = true;
    uint8_t             i;

    if (at_cmd_len == (strlen(LORA_AT_CDELMULTICAST) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CDELMULTICAST)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"DevAddr\"\r\nOK\r\n", LORA_AT_CDELMULTICAST);
    } else if (at_cmd_len == (strlen(LORA_AT_CDELMULTICAST) + 1) &&
               at_cmd[strlen(LORA_AT_CDELMULTICAST)] == '?') {

        if (multicast_get_num() > 0) {
            multicast_show();
        } else {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:NO MULTICAST INFO\r\n", LORA_AT_CADDMULTICAST);
        }
    } else if (at_cmd_len > (strlen(LORA_AT_CDELMULTICAST) + 1) &&
        at_cmd[strlen(LORA_AT_CDELMULTICAST)] == '=') {

        str = strtok(&at_cmd[strlen(LORA_AT_CDELMULTICAST) + 1], ",");
        hex2bin(str, &devAddr, 8);
        devAddr = (devAddr << 24) & (0xFF000000) | (devAddr << 8) & (0x00FF0000) |
                  (devAddr >> 8) & (0x0000FF00) | (devAddr >> 24) & (0x000000FF);

        if (0 == multicast_delete(devAddr)) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");

            ret = true;
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_nummulticast(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool     ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CNUMMULTICAST) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CNUMMULTICAST)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"number\"\r\nOK\r\n", LORA_AT_CNUMMULTICAST);
    } else if (at_cmd_len == (strlen(LORA_AT_CDELMULTICAST) + 1) &&
               at_cmd[strlen(LORA_AT_CDELMULTICAST)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                 LORA_AT_CNUMMULTICAST, multicast_get_num());
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_freqbandmask(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint32_t    length;
    uint8_t     mask[2];

    if (at_cmd_len == (strlen(LORA_AT_CFREQBANDMASK) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CFREQBANDMASK)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mask\"\r\n",
                 LORA_AT_CFREQBANDMASK);
    } else if (at_cmd_len == (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
               at_cmd[strlen(LORA_AT_CFREQBANDMASK)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%04x\r\n",
                 LORA_AT_CFREQBANDMASK, get_lora_freqband_mask());
    } else if (at_cmd_len > (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
               at_cmd[strlen(LORA_AT_CFREQBANDMASK)] == '=') {
        hex2bin(&at_cmd[strlen(LORA_AT_CFREQBANDMASK) + 1],
                         (uint8_t *)mask, 4);

        set_lora_freqband_mask(mask[1] | (mask[0] << 8));
        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_uldlmode(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t     mode;
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CULDLMODE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CULDLMODE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n",
                 LORA_AT_CULDLMODE);
    } else if (at_cmd_len == (strlen(LORA_AT_CULDLMODE) + 1) &&
               at_cmd[strlen(LORA_AT_CULDLMODE)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CULDLMODE,
                 get_lora_freq_mode());
    } else if (at_cmd_len = (strlen(LORA_AT_CULDLMODE) + 2) &&
                at_cmd[strlen(LORA_AT_CULDLMODE)] == '=') {
        mode = strtol(&at_cmd[strlen(LORA_AT_CULDLMODE) + 1], NULL, 0);
        set_lora_freq_mode(mode);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_workmode(uint8_t *at_cmd, int16_t at_cmd_len)
{
    int         mode;
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CWORKMODE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CWORKMODE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n",
                 LORA_AT_CWORKMODE);
    } else if (at_cmd_len == (strlen(LORA_AT_CWORKMODE) + 1) &&
               at_cmd[strlen(LORA_AT_CWORKMODE)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CWORKMODE, get_work_mode());
    } else if (at_cmd_len = (strlen(LORA_AT_CWORKMODE) + 2) &&
               at_cmd[strlen(LORA_AT_CWORKMODE)] == '=') {
        mode = strtol(&at_cmd[strlen(LORA_AT_CWORKMODE) + 1], NULL, 0);

        set_work_mode(mode);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_repeaterfreq(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint32_t    repeater_freq;
    bool        ret = true;

    // not support for repeater mode
    return false;

    if (at_cmd_len == (strlen(LORA_AT_CREPEATERFREQ) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CREPEATERFREQ)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"freq\"\r\n",
                 LORA_AT_CREPEATERFREQ);
    } else if (at_cmd_len == (strlen(LORA_AT_CREPEATERFREQ) + 1) &&
               at_cmd[strlen(LORA_AT_CREPEATERFREQ)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CREPEATERFREQ, get_repeater_freq());
    } else if (at_cmd_len = (strlen(LORA_AT_CREPEATERFREQ) + 2) &&
               at_cmd[strlen(LORA_AT_CREPEATERFREQ)] == '=') {
        repeater_freq = strtol(&at_cmd[strlen(LORA_AT_CREPEATERFREQ) + 1], NULL, 0);

        set_repeater_freq(repeater_freq);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_class(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         class;
    uint8_t         branch;
    uint8_t         ping_period;

    if (at_cmd_len == (strlen(LORA_AT_CCLASS) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CCLASS)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:\"class\",\"branch\",\"para1\",\"para2\","
                 "\"para3\",\"para4\"\r\n",
                 LORA_AT_CCLASS);
    } else if (at_cmd_len >= (strlen(LORA_AT_CCLASS) + 1) &&
               at_cmd[strlen(LORA_AT_CCLASS)] == '?') {
        class = get_lora_class();
        if (class == CLASS_A || class == CLASS_B || class == CLASS_C) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                     LORA_AT_CCLASS, class);
        } else {
            ret = false;
        }
    } else if (at_cmd_len >= (strlen(LORA_AT_CCLASS) + 2) &&
               at_cmd[strlen(LORA_AT_CCLASS)] == '=') {
        class = strtol(&at_cmd[strlen(LORA_AT_CCLASS) + 1], NULL, 0);
        ret = set_lora_class(class);

        if (class == CLASS_B) {
            if ((at_cmd[strlen(LORA_AT_CCLASS)+2] == ',')
                 &&(at_cmd[strlen(LORA_AT_CCLASS)+4] == ',')) {
                branch = strtol(&at_cmd[strlen(LORA_AT_CCLASS) + 3], NULL, 0);
                ping_period = strtol(&at_cmd[strlen(LORA_AT_CCLASS) + 5], NULL, 0);

                if (branch == 0) {
                    set_classb_ping_period(ping_period);
                } else {
                    ret = false;
                }
            }
        }

        if (true == ret) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_batterylevel(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CBL) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CBL)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                 LORA_AT_CBL);
    } else if (at_cmd_len == (strlen(LORA_AT_CBL) + 1) &&
               at_cmd[strlen(LORA_AT_CBL)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "%s:%d\r\nOK\r\n", LORA_AT_CBL, get_device_battery());
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_status(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CSTATUS) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CSTATUS)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"status\"\r\nOK\r\n",
                 LORA_AT_CSTATUS);
    } else if (at_cmd_len == (strlen(LORA_AT_CSTATUS) + 1) &&
               at_cmd[strlen(LORA_AT_CSTATUS)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "%s:%d\r\nOK\r\n", LORA_AT_CSTATUS,
                 get_device_status());
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_join(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         bJoin;
    uint8_t         bAutoJoin;
    uint16_t        joinInterval;
    uint16_t        joinRetryCnt;

    if (at_cmd_len == (strlen(LORA_AT_CJOIN) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CJOIN)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:<ParaTag1>,[ParaTag2],[ParaTag3],[ParaTag4]\r\n",
                 LORA_AT_CJOIN);
    } else if (at_cmd_len == (strlen(LORA_AT_CJOIN) + 1) &&
               at_cmd[strlen(LORA_AT_CJOIN)] == '?') {
        get_lora_join_params(&bJoin, &bAutoJoin, &joinInterval, &joinRetryCnt);

        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d,%d,%d\r\nOK\r\n",
                 LORA_AT_CJOIN, bJoin, bAutoJoin, joinInterval,
                 joinRetryCnt);
    } else if (at_cmd_len >= (strlen(LORA_AT_CJOIN) + 2) &&
               at_cmd[strlen(LORA_AT_CJOIN)] == '=') {
        char *str    = strtok(&at_cmd[strlen(LORA_AT_CJOIN) + 1], ",");
        bJoin        = strtol(str, NULL, 0);
        str          = strtok(NULL, ",");
        bAutoJoin    = strtol(str, NULL, 0);
        str          = strtok(NULL, ",");
        joinInterval = strtol(str, NULL, 0);
        str          = strtok(NULL, ",");
        joinRetryCnt = strtol(str, NULL, 0);
        if ((bJoin == 0) || (bJoin == 1)) {
            init_lora_join(bJoin, bAutoJoin, joinInterval, joinRetryCnt);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_dtx(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint8_t         len;
    uint8_t         payload[LORAWAN_APP_DATA_BUFF_SIZE];

    if (at_cmd_len == (strlen(LORA_AT_DTX) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_DTX)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:[confirm],[nbtrials],[length],<payload>\r\n",
                 LORA_AT_DTX);
    } else if (at_cmd_len >= (strlen(LORA_AT_DTX) + 2) &&
               at_cmd[strlen(LORA_AT_DTX)] == '=') {
        char   *str = strtok(&at_cmd[strlen(LORA_AT_DTX) + 1], ",");
        len         = strtol(str, NULL, 0);
        str         = strtok(NULL, ",");
        strcpy(payload, str);
        payload[len] = '\0';

        ret = lora_tx_data_payload(get_lora_tx_cfm_flag(), get_lora_tx_cfm_trials(), payload, len);
        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK+SEND:\r\nOK+SENT:\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_drx(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;

    if (at_cmd_len == (strlen(LORA_AT_DRX) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_DRX)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:<Length>,<Payload>\r\n",
                 LORA_AT_DRX);
    } else if (at_cmd_len == (strlen(LORA_AT_DRX) + 1) &&
               at_cmd[strlen(LORA_AT_DRX)] == '?') {
        lora_AppData_t *rx_data;
        rx_data = get_lora_rx_data();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d", LORA_AT_DRX,
                 rx_data->BuffSize);
        linkwan_serial_output(atcmd, strlen(atcmd));
        if (rx_data->BuffSize > 0) {
            snprintf(atcmd, ATCMD_SIZE, ",");
            linkwan_serial_output(atcmd, strlen(atcmd));

            int i;
            char str[3] = {'\0'};
            for (i = 0; i < rx_data->BuffSize; i++) {
                snprintf(str, sizeof(str), "%02x", rx_data->Buff[i]);
                linkwan_serial_output(str, 2);
            }
        }

        rx_data->BuffSize = 0;

        snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\n", "OK");
    }

    return ret;
}

bool ica_at_confirm(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t     confirm_flag;
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CCONFIRM) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CCONFIRM)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                 LORA_AT_CCONFIRM);
    } else if (at_cmd_len == (strlen(LORA_AT_CCONFIRM) + 1) &&
               at_cmd[strlen(LORA_AT_CCONFIRM)] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCONFIRM,
                 get_lora_tx_cfm_flag());
    } else if (at_cmd_len == (strlen(LORA_AT_CCONFIRM) + 2) &&
               at_cmd[strlen(LORA_AT_CCONFIRM)] == '=') {
        confirm_flag = strtol(&at_cmd[strlen(LORA_AT_CCONFIRM) + 1], NULL, 0);
        set_lora_tx_cfm_flag(confirm_flag);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

static bool ica_at_appport(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     port;

    if (at_cmd_len == (strlen(LORA_AT_CAPPPORT) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CAPPPORT)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                 LORA_AT_CAPPPORT);
    } else if (at_cmd_len == (strlen(LORA_AT_CAPPPORT) + 1) &&
               at_cmd[at_cmd_len] == '?') {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CAPPPORT,
                 get_lora_app_port());
    } else if (at_cmd_len > (at_cmd_len + 1) && at_cmd[at_cmd_len] == '=') {
        port = (uint8_t)strtol(&at_cmd[at_cmd_len + 1], NULL, 0);

        set_lora_app_port(port);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_datarate(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t         datarate;
    bool            ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CDATARATE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CDATARATE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                 LORA_AT_CDATARATE);
    } else if (at_cmd_len == (strlen(LORA_AT_CDATARATE) + 1) &&
               at_cmd[strlen(LORA_AT_CDATARATE)] == '?') {
        datarate = get_lora_tx_datarate();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CDATARATE,
                 datarate);
    } else if (at_cmd_len == (strlen(LORA_AT_CDATARATE) + 2) &&
               at_cmd[strlen(LORA_AT_CDATARATE)] == '=') {
        datarate = strtol(&at_cmd[strlen(LORA_AT_CDATARATE) + 1], NULL, 0);
        set_lora_tx_datarate(datarate);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_rssi(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t         freq_band;
    bool            ret = true;
    char            freq_str[4];
    int16_t         channel_rssi[8];
    int8_t          channel_snr[8];

    if (at_cmd_len == (strlen(LORA_AT_CRSSI) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CRSSI)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRSSI);
    } else if ((at_cmd_len >= (strlen(LORA_AT_CRSSI) + 2)) &&
                (at_cmd[at_cmd_len - 1] == '?')) {
        if (at_cmd_len - strlen(LORA_AT_CRSSI) - 2 == 0) {
            ret = false;
        } else {
            strncpy(&freq_str, &at_cmd[strlen(LORA_AT_CRSSI)],
                    at_cmd_len - strlen(LORA_AT_CRSSI) - 2);
            freq_str[at_cmd_len - strlen(LORA_AT_CRSSI) - 2] = '\0';

            freq_band = strtol(freq_str, NULL, 0);
            get_lora_rssi(freq_band, channel_rssi, channel_snr);
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\r\n0:%d\r\n1:%d\r\n2:%d\r\n3:%d\r\n4:%d\r\n5:"
                     "%d\r\n6:%d\r\n7:%d\r\n0:%d\r\n1:%d\r\n2:%d\r\n3:"
                     "%d\r\n4:%d\r\n5:%d\r\n6:%d\r\n7:%d\r\nOK\r\n",
                     LORA_AT_CRSSI, channel_rssi[0], channel_rssi[1],
                     channel_rssi[2], channel_rssi[3], channel_rssi[4],
                     channel_rssi[5], channel_rssi[6], channel_rssi[7],
                     channel_snr[0], channel_snr[1], channel_snr[2],
                     channel_snr[3], channel_snr[4], channel_snr[5],
                     channel_snr[6], channel_snr[7]);
            ret = true;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_nbtrials(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t         m_type;
    uint8_t         value;
    bool            ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CNBTRIALS) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CNBTRIALS)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"MTypes\",\"value\"\r\nOK\r\n",
                 LORA_AT_CNBTRIALS);
    } else if (at_cmd_len == (strlen(LORA_AT_CNBTRIALS) + 1) &&
               at_cmd[strlen(LORA_AT_CNBTRIALS)] == '?') {
        m_type = get_lora_tx_cfm_flag();
        value  = get_lora_tx_cfm_trials();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d\r\nOK\r\n",
                 LORA_AT_CNBTRIALS, m_type, value);
    } else if (at_cmd_len > (strlen(LORA_AT_CNBTRIALS) + 1) &&
               at_cmd[strlen(LORA_AT_CNBTRIALS)] == '=') {
        m_type = at_cmd[strlen(LORA_AT_CNBTRIALS) + 1] - '0';
        value  = strtol(&at_cmd[strlen(LORA_AT_CNBTRIALS) + 3], NULL, 0);

        if ((m_type == 0 || m_type == 1) && (value >= 1 && value <= 15)) {
            set_lora_tx_cfm_flag(m_type);
            set_lora_tx_cfm_trials(value);
        } else {
            ret = false;
        }

        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_reportmode(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    int8_t          reportMode;
    uint32_t        reportInterval;

    if (at_cmd_len == (strlen(LORA_AT_CRM) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CRM)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:\"reportMode\",\"reportInterval\"\r\nOK\r\n",
                 LORA_AT_CRM);
    } else if (at_cmd_len == (strlen(LORA_AT_CRM) + 1) &&
               at_cmd[strlen(LORA_AT_CRM)] == '?') {
        reportMode     = get_lora_report_mode();
        reportInterval = get_lora_tx_dutycycle() / 1000;

        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d\r\nOK\r\n",
                 LORA_AT_CRM, reportMode, reportInterval);
    } else if (at_cmd_len > (strlen(LORA_AT_CRM) + 2) &&
               at_cmd[strlen(LORA_AT_CRM)] == '=') {
        reportMode     = at_cmd[strlen(LORA_AT_CRM) + 1] - '0';
        reportInterval = strtol(&at_cmd[strlen(LORA_AT_CRM) + 3], NULL, 0);

        if ((reportMode == 0) || (reportMode == 1 && reportInterval >= 1)) {
            set_lora_report_mode(reportMode);
            set_lora_tx_dutycycle(reportInterval * 1000);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK%d%d\r\n", reportMode,
                     reportInterval);
            ret = true;
        } else {
            ret = false;
            snprintf(atcmd, ATCMD_SIZE, "\r\nCME ERROR:%d\r\n", ret);
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_txpower(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     txpwr;

    if (at_cmd_len == (strlen(LORA_AT_CTXP) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CTXP)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                 LORA_AT_CTXP);
    } else if (at_cmd_len == (strlen(LORA_AT_CTXP) + 1) &&
               at_cmd[strlen(LORA_AT_CTXP)] == '?') {
        txpwr = get_lora_tx_power();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CTXP, txpwr);
    } else if (at_cmd_len == (strlen(LORA_AT_CTXP) + 2) &&
               at_cmd[strlen(LORA_AT_CTXP)] == '=') {
        ret         = false;
        txpwr       = strtol(&at_cmd[strlen(LORA_AT_CTXP) + 1], NULL, 0);
        set_lora_tx_power(txpwr);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_linkcheck(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t     checkValue;
    bool        ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CLINKCHECK) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CLINKCHECK)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                 LORA_AT_CLINKCHECK);
    } else if (at_cmd_len > (strlen(LORA_AT_CLINKCHECK) + 1) &&
               at_cmd[strlen(LORA_AT_CLINKCHECK)] == '=') {

        checkValue =
          strtol(&at_cmd[strlen(LORA_AT_CLINKCHECK) + 1], NULL, 0);
        PRINTF("LINK CHECK: %d\r\n", checkValue);
        if (checkValue == 1) {
            ret = send_lora_link_check();
        } else {
            // TBD:
        }

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_adr(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t         adr;
    bool            ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CADR) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CADR)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CADR);
    } else if (at_cmd_len == (strlen(LORA_AT_CADR) + 1) &&
               at_cmd[strlen(LORA_AT_CADR)] == '?') {
        adr = get_lora_adr();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CADR, adr);
    } else if (at_cmd_len == (strlen(LORA_AT_CADR) + 2) &&
               at_cmd[strlen(LORA_AT_CADR)] == '=') {
        adr = strtol(&at_cmd[strlen(LORA_AT_CADR) + 1], NULL, 0);
        set_lora_adr(adr);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_rxparam(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t         RX1DRoffset;
    uint8_t         RX2DataRate;
    uint32_t        RX2Frequency;
    bool            ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CRXP) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CRXP)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:\"RX1DRoffset\",\"RX2DataRate\","
                 "\"RX2Frequency\"\r\nOK\r\n",
                 LORA_AT_CRXP);
    } else if (at_cmd_len == (strlen(LORA_AT_CRXP) + 1) &&
               at_cmd[strlen(LORA_AT_CADR)] == '?') {
        get_lora_rx_window_params(&RX1DRoffset, &RX2DataRate, &RX2Frequency);
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d,%d\r\nOK\r\n",
                 LORA_AT_CRXP, RX1DRoffset, RX2DataRate, RX2Frequency);
    } else if (at_cmd_len > (strlen(LORA_AT_CRXP) + 2) &&
               at_cmd[strlen(LORA_AT_CRXP)] == '=') {
        char *str    = strtok(&at_cmd[strlen(LORA_AT_CRXP) + 1], ",");
        RX1DRoffset  = strtol(str, NULL, 0);
        str          = strtok(NULL, ",");
        RX2DataRate  = strtol(str, NULL, 0);
        str          = strtok(NULL, ",");
        RX2Frequency = strtol(str, NULL, 0);

        set_lora_rx_window_params(RX1DRoffset, RX2DataRate, RX2Frequency);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK-%d,%d,%d\r\n", RX1DRoffset,
                 RX2DataRate, RX2Frequency);
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_freqlist(uint8_t *at_cmd, int16_t at_cmd_len)
{
    uint8_t     uldl_mode;
    uint8_t     method;
    uint8_t     number;
    uint32_t    freqlist[8];
    bool        ret = true;
    char        *str;
    uint8_t     i;

    if (at_cmd_len == (strlen(LORA_AT_CFREQLIST) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CFREQLIST)], "=?") == 0) {
        snprintf(
          atcmd, ATCMD_SIZE,
          "\r\n%s:\"ULDL\",\"method\",\"number\",\"freqlist\"\r\nOK\r\n",
          LORA_AT_CFREQLIST);
    } else if (at_cmd_len == (strlen(LORA_AT_CFREQLIST) + 1) &&
               at_cmd[strlen(LORA_AT_CFREQLIST)] == '?') {
        method = get_lora_freq_mode();
        if (FREQ_MODE_INTER == method) {
            get_freqlist(&method, 1, &number, freqlist);
            get_freqlist(&method, 2, &number, freqlist);
        } else if (FREQ_MODE_INTRA == method) {
            get_freqlist(&method, 1, &number, freqlist);
        }
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CFREQLIST);
    } else if (at_cmd_len > (strlen(LORA_AT_CFREQLIST) + 2) &&
               at_cmd[strlen(LORA_AT_CFREQLIST)] == '=') {

        str         = strtok(&at_cmd[strlen(LORA_AT_CFREQLIST) + 1], ",");
        uldl_mode   = strtol(str, NULL, 0);
        str         = strtok(NULL, ",");
        method      = strtol(str, NULL, 0);
        str         = strtok(NULL, ",");
        number      = strtol(str, NULL, 0);

        for (i = 0; i < number; i++) {
            str         = strtok(NULL, ",");

            freqlist[i] = strtol(str, NULL, 0);
        }

        set_freqlist(method, uldl_mode, number, freqlist);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_rx1delay(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool            ret = true;
    uint32_t        rx1delay;

    if (at_cmd_len == (strlen(LORA_AT_CRX1DELAY) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CRX1DELAY)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                 LORA_AT_CRX1DELAY);
    } else if (at_cmd_len == (strlen(LORA_AT_CRX1DELAY) + 1) &&
               at_cmd[strlen(LORA_AT_CRX1DELAY)] == '?') {
        rx1delay = get_lora_mac_rx1_delay();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CRX1DELAY,
                 rx1delay);
    } else if (at_cmd_len >= (strlen(LORA_AT_CRX1DELAY) + 2) &&
               at_cmd[strlen(LORA_AT_CRX1DELAY)] == '=') {
        ret      = false;
        rx1delay = strtol(&at_cmd[strlen(LORA_AT_CRX1DELAY) + 1], NULL, 0);
        set_lora_mac_rx1_delay(rx1delay);

        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_save(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool      ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CSAVE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CSAVE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSAVE);
    } else if (at_cmd_len == strlen(LORA_AT_CSAVE)) {
        PRINTF("SAVE config\r\n");
        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_restore(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool     ret = true;

    if (at_cmd_len == (strlen(LORA_AT_CRESTORE) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CRESTORE)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSAVE);
    } else if (at_cmd_len == strlen(LORA_AT_CRESTORE)) {
        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");

        PRINTF("RESTORE \r\n");
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_repeater_filter(uint8_t *at_cmd, int16_t at_cmd_len)
{
    return false;
}

bool ica_at_cgmi(uint8_t *at_cmd, int16_t at_cmd_len)
{
    snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMI,
             aos_mft_itf.get_mft_id());

    return true;
}

bool ica_at_cgmm(uint8_t *at_cmd, int16_t at_cmd_len)
{
    snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMM,
             aos_mft_itf.get_mft_model());

    return true;
}

bool ica_at_cgmr(uint8_t *at_cmd, int16_t at_cmd_len)
{
    snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMR,
                 aos_mft_itf.get_mft_rev());

    return true;
}

bool ica_at_cgsn(uint8_t *at_cmd, int16_t at_cmd_len)
{
    snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGSN,
             aos_mft_itf.get_mft_sn());

    return true;
}

bool ica_at_cgbr(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint32_t    baud;

    if (at_cmd_len == (strlen(LORA_AT_CGBR) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CGBR)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CGBR);
    } else if (at_cmd_len == (strlen(LORA_AT_CGBR) + 1) &&
               at_cmd[strlen(LORA_AT_CGBR)] == '?') {
        baud = aos_mft_itf.get_mft_baud();
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CGBR, baud);
    } else if (at_cmd_len > (strlen(LORA_AT_CGBR) + 1) && at_cmd[strlen(LORA_AT_CGBR)] == '=') {
        ret  = false;
        baud = strtol(&at_cmd[strlen(LORA_AT_CGBR) + 1], NULL, 0);
        ret  = aos_mft_itf.set_mft_baud(baud);
        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_iloglevel(uint8_t *at_cmd, int16_t at_cmd_len)
{
    return false;
}

bool ica_at_ireboot(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    int8_t      mode;

    if (at_cmd_len == (strlen(LORA_AT_IREBOOT) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_IREBOOT)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n", LORA_AT_IREBOOT);
    } else if (at_cmd_len == (strlen(LORA_AT_IREBOOT) + 2) &&
               at_cmd[strlen(LORA_AT_IREBOOT)] == '=') {
        ret         = false;
        int8_t mode = strtol(&at_cmd[strlen(LORA_AT_IREBOOT) + 1], NULL, 0);
        if (mode == 0 || mode == 1) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            //lora_reboot(mode);
            ret = true;
        } else {
            ret = false;
        }
    } else {
        ret = false;
    }

    return ret;
}

bool ica_at_idefault(uint8_t *at_cmd, int16_t at_cmd_len)
{
    return false;
}

bool ica_at_complianceTest(uint8_t *at_cmd, int16_t at_cmd_len)
{
    bool        ret = true;
    uint8_t     idx;

    if (at_cmd_len == (strlen(LORA_AT_CTEST) + 2) &&
        strcmp(&at_cmd[strlen(LORA_AT_CTEST)], "=?") == 0) {
        snprintf(atcmd, ATCMD_SIZE,
                 "\r\n%s:[idx]\r\n", LORA_AT_CTEST);
    } else if (at_cmd_len >= (strlen(LORA_AT_CTEST) + 2) &&
               at_cmd[strlen(LORA_AT_CTEST)] == '=') {
        idx = strtol(&at_cmd[strlen(LORA_AT_CTEST) + 1], NULL, 16);
        ret = lora_compliance_test(idx);
        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        }
    } else {
        ret = false;
    }

    return ret;
}



struct ica_at_handle
{
    uint8_t        *cmd;
    uint8_t        cmd_len;
    bool          (*function)(uint8_t *at_cmd, int16_t at_cmd_len);
};

struct ica_at_handle   at_handles[] = {
    { LORA_AT_CDEVEUI, strlen(LORA_AT_CDEVEUI), ica_at_deveui },
    { LORA_AT_CAPPEUI, strlen(LORA_AT_CAPPEUI), ica_at_appeui },
    { LORA_AT_CAPPKEY, strlen(LORA_AT_CAPPKEY), ica_at_appkey },
    { LORA_AT_CDEVADDR, strlen(LORA_AT_CDEVADDR), ica_at_devaddr },
    { LORA_AT_CAPPSKEY, strlen(LORA_AT_CAPPSKEY), ica_at_appskey },
    { LORA_AT_CNWKSKEY, strlen(LORA_AT_CNWKSKEY), ica_at_nwkskey },
    { LORA_AT_CADDMULTICAST, strlen(LORA_AT_CADDMULTICAST), ica_at_addmulticast },
    { LORA_AT_CDELMULTICAST, strlen(LORA_AT_CDELMULTICAST), ica_at_delmulticast },
    { LORA_AT_CNUMMULTICAST, strlen(LORA_AT_CNUMMULTICAST), ica_at_nummulticast },
    { LORA_AT_CFREQBANDMASK, strlen(LORA_AT_CFREQBANDMASK), ica_at_freqbandmask },
    { LORA_AT_CULDLMODE, strlen(LORA_AT_CULDLMODE), ica_at_uldlmode },
    { LORA_AT_CWORKMODE, strlen(LORA_AT_CWORKMODE), ica_at_workmode },
    { LORA_AT_CREPEATERFREQ, strlen(LORA_AT_CREPEATERFREQ), ica_at_repeaterfreq },
    { LORA_AT_CCLASS, strlen(LORA_AT_CCLASS), ica_at_class },
    { LORA_AT_CBL, strlen(LORA_AT_CBL), ica_at_batterylevel },
    { LORA_AT_CSTATUS, strlen(LORA_AT_CSTATUS), ica_at_status },
    { LORA_AT_CJOIN, strlen(LORA_AT_CJOIN), ica_at_join },
    { LORA_AT_DTX, strlen(LORA_AT_DTX), ica_at_dtx },
    { LORA_AT_DRX, strlen(LORA_AT_DRX), ica_at_drx },
    { LORA_AT_CCONFIRM, strlen(LORA_AT_CCONFIRM), ica_at_confirm },
    { LORA_AT_CAPPPORT, strlen(LORA_AT_CAPPPORT), ica_at_appport },
    { LORA_AT_CDATARATE, strlen(LORA_AT_CDATARATE), ica_at_datarate },
    { LORA_AT_CRSSI, strlen(LORA_AT_CRSSI), ica_at_rssi },
    { LORA_AT_CNBTRIALS, strlen(LORA_AT_CNBTRIALS), ica_at_nbtrials },
    { LORA_AT_CRM, strlen(LORA_AT_CRM), ica_at_reportmode },
    { LORA_AT_CTXP, strlen(LORA_AT_CTXP), ica_at_txpower },
    { LORA_AT_CLINKCHECK, strlen(LORA_AT_CLINKCHECK), ica_at_linkcheck },
    { LORA_AT_CADR, strlen(LORA_AT_CADR), ica_at_adr },
    { LORA_AT_CRXP, strlen(LORA_AT_CRXP), ica_at_rxparam },
    { LORA_AT_CFREQLIST, strlen(LORA_AT_CFREQLIST), ica_at_freqlist },
    { LORA_AT_CRX1DELAY, strlen(LORA_AT_CRX1DELAY), ica_at_rx1delay },
    { LORA_AT_CSAVE, strlen(LORA_AT_CSAVE), ica_at_save },
    { LORA_AT_CRESTORE, strlen(LORA_AT_CRESTORE), ica_at_restore },
    { LORA_AT_CREPEATERFILTER, strlen(LORA_AT_CREPEATERFILTER), ica_at_repeater_filter },
    { LORA_AT_CGMI, strlen(LORA_AT_CGMI), ica_at_cgmi },
    { LORA_AT_CGMM, strlen(LORA_AT_CGMM), ica_at_cgmm },
    { LORA_AT_CGMR, strlen(LORA_AT_CGMR), ica_at_cgmr },
    { LORA_AT_CGSN, strlen(LORA_AT_CGSN), ica_at_cgsn },
    { LORA_AT_CGBR, strlen(LORA_AT_CGBR), ica_at_cgbr },
    { LORA_AT_ILOGLVL, strlen(LORA_AT_ILOGLVL), ica_at_iloglevel },
    { LORA_AT_IREBOOT, strlen(LORA_AT_IREBOOT), ica_at_ireboot },
    { LORA_AT_CTEST, strlen(LORA_AT_CTEST), ica_at_complianceTest },
    { LORA_AT_IDEFAULT, strlen(LORA_AT_IDEFAULT), ica_at_idefault }
};

void process_linkwan_at(void)
{
    bool                ret = false;
    int                 value;
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t     status;
    uint8_t             length;
    uint8_t             buf[16];
    uint8_t            *rxcmd       = atcmd + 2;
    int16_t             rxcmd_index = atcmd_index - 2;
    static uint8_t      uldl_mode   = 1;
    char               *str;
    uint32_t            i;

    if (rxcmd_index <= 0 || rxcmd[rxcmd_index] != '\0') {
        return;
    }

    for (i = 0; i < sizeof(at_handles) / sizeof(struct ica_at_handle); i++){
        if (strncmp(at_handles[i].cmd, rxcmd, at_handles[i].cmd_len) == 0) {
            ret = at_handles[i].function(rxcmd, rxcmd_index);

            break;
        }
    }

    if (ret == false) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s%x\r\n", AT_ERROR, 1);
    }

exit:
    linkwan_serial_output(atcmd, strlen(atcmd));
    atcmd_index = 0;
    memset(atcmd, 0xff, ATCMD_SIZE);
}


void linkwan_at_init(void)
{
    atcmd_index = 0;
    memset(atcmd, 0xff, ATCMD_SIZE);
}

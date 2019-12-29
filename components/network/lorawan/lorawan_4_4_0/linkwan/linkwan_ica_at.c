/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LoRaMac.h"
#include "Region.h"
#include "linkwan.h"
#include "linkwan_ica_at.h"
#include "lorawan_port.h"


//#define ATCMD_SIZE 64
#define ATCMD_SIZE 100
#define PORT_LEN   4

uint8_t  atcmd[ATCMD_SIZE];
uint16_t atcmd_index = 0;

LoRaMacParams_t        gMacParams;
extern LoRaMacParams_t LoRaMacParamsDefaults;

static int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
    uint16_t    hex_length = strlen(hex);
    const char *hex_end    = hex + hex_length;
    uint8_t    *cur        = bin;
    uint8_t     num_chars  = hex_length & 1;
    uint8_t     byte       = 0;

    if ((hex_length + 1) / 2 > bin_length) {
        return -1;
    }

    while (hex < hex_end) {
        if ('A' <= *hex && *hex <= 'F') {
            byte |= 10 + (*hex - 'A');
        } else if ('a' <= *hex && *hex <= 'f') {
            byte |= 10 + (*hex - 'a');
        } else if ('0' <= *hex && *hex <= '9') {
            byte |= *hex - '0';
        } else {
            return -1;
        }
        hex++;
        num_chars++;

        if (num_chars >= 2) {
            num_chars = 0;
            *cur++    = byte;
            byte      = 0;
        } else {
            byte <<= 4;
        }
    }
    return cur - bin;
}

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

void process_linkwan_at(void)
{
    bool                ret = true;
    int                 value;
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t     status;
    uint8_t             length;
    uint8_t             buf[16];
    uint8_t            *rxcmd       = atcmd + 2;
    int16_t             rxcmd_index = atcmd_index - 2;
    static uint8_t      uldl_mode   = 1;
    char               *str;

    if (rxcmd_index <= 0 || rxcmd[rxcmd_index] != '\0') {
        return;
    }

    if (strncmp(rxcmd, LORA_AT_CJOINMODE, strlen(LORA_AT_CJOINMODE)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CJOINMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"mode\"\r\nOK\r\n",
                     LORA_AT_CJOINMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 1) &&
                   rxcmd[strlen(LORA_AT_CJOINMODE)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                     LORA_AT_CJOINMODE, get_lora_join_mode());
        } else if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 2) &&
                   rxcmd[strlen(LORA_AT_CJOINMODE)] == '=') {
            int mode = strtol(&rxcmd[strlen(LORA_AT_CJOINMODE) + 1], NULL, 0);
            if (mode == 0 || mode == 1) {
                ret = set_lora_join_mode(mode);
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CDEVEUI, strlen(LORA_AT_CDEVEUI)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CDEVEUI) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CDEVEUI)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"DevEUI:length is 16\"\r\n",
                     LORA_AT_CDEVEUI);
        } else if (rxcmd_index == (strlen(LORA_AT_CDEVEUI) + 1) &&
                   rxcmd[strlen(LORA_AT_CDEVEUI)] == '?') {
            uint8_t eui[8];
            get_lora_dev_eui(eui);
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n",
                     LORA_AT_CDEVEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                     eui[5], eui[6], eui[7]);
        } else if (rxcmd_index == (strlen(LORA_AT_CDEVEUI) + 17) &&
                   rxcmd[strlen(LORA_AT_CDEVEUI)] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[strlen(LORA_AT_CDEVEUI) + 1], buf, 8);
            if (length == 8) {
                ret = set_lora_dev_eui(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CAPPEUI, strlen(LORA_AT_CAPPEUI)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CAPPEUI) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CAPPEUI)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppEUI:length is 16\"\r\n",
                     LORA_AT_CAPPEUI);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPEUI) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '?') {
            uint8_t eui[8];
            get_lora_app_eui(eui);
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n",
                     LORA_AT_CAPPEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                     eui[5], eui[6], eui[7]);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPEUI) + 17) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[strlen(LORA_AT_CAPPEUI) + 1], buf, 8);
            if (length == 8) {
                ret = set_lora_app_eui(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CAPPKEY, strlen(LORA_AT_CAPPKEY)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CAPPKEY) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CAPPKEY)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppKey:length is 32\"\r\n",
                     LORA_AT_CAPPKEY);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPKEY) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPKEY)] == '?') {
            uint8_t key[16];
            get_lora_app_key(key);
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%"
                     "02x%02x%02x%02x\r\nOK\r\n",
                     LORA_AT_CAPPKEY, key[0], key[1], key[2], key[3], key[4],
                     key[5], key[6], key[7], key[8], key[9], key[10], key[11],
                     key[12], key[13], key[14], key[15]);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPKEY) + 33) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[strlen(LORA_AT_CAPPKEY) + 1], buf, 16);
            if (length == 16) {
                ret = set_lora_app_key(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CDEVADDR, strlen(LORA_AT_CDEVADDR)) ==
               0) {
        uint8_t at_str_len;
        at_str_len = strlen(LORA_AT_CDEVADDR);
        if (rxcmd_index == (at_str_len + 2) &&
            strcmp(&rxcmd[at_str_len], "=?") == 0) {
            snprintf(
              atcmd, ATCMD_SIZE,
              "\r\n%s=<DevAddr:length is 8, Device address of ABP mode>\r\n",
              LORA_AT_CDEVADDR);
        } else if (rxcmd_index == (at_str_len + 1) &&
                   rxcmd[at_str_len] == '?') {
            uint8_t *devaddr = get_lora_devaddr();
            uint8_t  i;
            length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%02x%02x%02x%02x",
                              LORA_AT_CDEVADDR, devaddr[0], devaddr[1],
                              devaddr[2], devaddr[3]);
            snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
        } else if (rxcmd_index == (at_str_len + 9) &&
                   rxcmd[at_str_len] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[at_str_len + 1], buf, 4);
            if (length == 4) {
                ret = set_lora_devaddr(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CAPPSKEY, strlen(LORA_AT_CAPPSKEY)) ==
               0) {
        uint8_t at_str_len;
        at_str_len = strlen(LORA_AT_CAPPSKEY);
        if (rxcmd_index == (at_str_len + 2) &&
            strcmp(&rxcmd[at_str_len], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=<AppSKey: length is 32>\r\n",
                     LORA_AT_CAPPSKEY);
        } else if (rxcmd_index == (at_str_len + 1) &&
                   rxcmd[at_str_len] == '?') {
            uint8_t *devaddr = get_lora_appskey();
            uint8_t  i;
            length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CAPPSKEY);
            for (i = 0; i < 16; i++) {
                length +=
                  snprintf(atcmd + length, ATCMD_SIZE, "%02x", devaddr[i]);
            }
            snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
        } else if (rxcmd_index == (at_str_len + 33) &&
                   rxcmd[at_str_len] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[at_str_len + 1], buf, 16);
            if (length == 16) {
                ret = set_lora_appskey(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CNWKSKEY, strlen(LORA_AT_CNWKSKEY)) ==
               0) {
        uint8_t at_str_len;
        at_str_len = strlen(LORA_AT_CNWKSKEY);
        if (rxcmd_index == (at_str_len + 2) &&
            strcmp(&rxcmd[at_str_len], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=<NwkSKey:length is 32>\r\n",
                     LORA_AT_CNWKSKEY);
        } else if (rxcmd_index == (at_str_len + 1) &&
                   rxcmd[at_str_len] == '?') {
            uint8_t *devaddr = get_lora_nwkskey();
            uint8_t  i;
            length = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CNWKSKEY);
            for (i = 0; i < 16; i++) {
                length +=
                  snprintf(atcmd + length, ATCMD_SIZE, "%02x", devaddr[i]);
            }
            snprintf(atcmd + length, ATCMD_SIZE, "\r\nOK\r\n");
        } else if (rxcmd_index == (at_str_len + 33) &&
                   rxcmd[at_str_len] == '=') {
            ret    = false;
            length = hex2bin(&rxcmd[at_str_len + 1], buf, 16);
            if (length == 16) {
                ret = set_lora_nwkskey(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CADDMUTICAST,
                       strlen(LORA_AT_CADDMUTICAST)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CADDMUTICAST) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CADDMUTICAST)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\"DevAddr\",\"AppSKey\",\"NwkSKey\"\r\nOK\r\n",
                     LORA_AT_CADDMUTICAST);
        } else if (rxcmd_index == (strlen(LORA_AT_CADDMUTICAST) + 1) &&
                   rxcmd[strlen(LORA_AT_CADDMUTICAST)] == '?') {
            MulticastParams_t *mCastInfo = get_lora_cur_multicast();

            if (mCastInfo != NULL) {
                while (mCastInfo) {
                    DBG_LINKWAN(
                      "%08x,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x,\
    				%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                      mCastInfo->Address, mCastInfo->AppSKey[0],
                      mCastInfo->AppSKey[1], mCastInfo->AppSKey[2],
                      mCastInfo->AppSKey[3], mCastInfo->AppSKey[4],
                      mCastInfo->AppSKey[5], mCastInfo->AppSKey[6],
                      mCastInfo->AppSKey[7], mCastInfo->AppSKey[8],
                      mCastInfo->AppSKey[9], mCastInfo->AppSKey[10],
                      mCastInfo->AppSKey[11], mCastInfo->AppSKey[12],
                      mCastInfo->AppSKey[13], mCastInfo->AppSKey[14],
                      mCastInfo->AppSKey[15], mCastInfo->NwkSKey[0],
                      mCastInfo->NwkSKey[1], mCastInfo->NwkSKey[2],
                      mCastInfo->NwkSKey[3], mCastInfo->NwkSKey[4],
                      mCastInfo->NwkSKey[5], mCastInfo->NwkSKey[6],
                      mCastInfo->NwkSKey[7], mCastInfo->NwkSKey[8],
                      mCastInfo->NwkSKey[9], mCastInfo->NwkSKey[10],
                      mCastInfo->NwkSKey[11], mCastInfo->NwkSKey[12],
                      mCastInfo->NwkSKey[13], mCastInfo->NwkSKey[14],
                      mCastInfo->NwkSKey[15]);

                    mCastInfo = mCastInfo->Next;
                }
            } else {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:NO MULTICAST INFO\r\n",
                         LORA_AT_CADDMUTICAST);
            }
        } else if (rxcmd_index > (strlen(LORA_AT_CADDMUTICAST) + 1) &&
                   rxcmd[strlen(LORA_AT_CADDMUTICAST)] == '=') {
            MulticastParams_t *pMutiCastInfo;

            pMutiCastInfo = malloc(sizeof(MulticastParams_t));

            if (pMutiCastInfo == NULL) {
                return false;
            }

            memset(pMutiCastInfo, 0, sizeof(MulticastParams_t));

            str = strtok(&rxcmd[strlen(LORA_AT_CADDMUTICAST) + 1], ",");
            hex2bin(str, &(pMutiCastInfo->Address), 4);
            pMutiCastInfo->Address =
              (pMutiCastInfo->Address << 24) & (0xFF000000) |
              (pMutiCastInfo->Address << 8) & (0x00FF0000) |
              (pMutiCastInfo->Address >> 8) & (0x0000FF00) |
              (pMutiCastInfo->Address >> 24) & (0x000000FF);

            str = strtok(NULL, ",");
            hex2bin(str, pMutiCastInfo->AppSKey, 16);

            str = strtok(NULL, ",");
            hex2bin(str, pMutiCastInfo->NwkSKey, 16);

            if (false == set_lora_multicast(pMutiCastInfo)) {
                free(pMutiCastInfo);
                pMutiCastInfo = NULL;
            }

            MulticastParams_t *mCastInfo = get_lora_cur_multicast();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%p-%p testRRRPPT\r\n",
                     pMutiCastInfo, mCastInfo);
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CDELMUTICAST,
                       strlen(LORA_AT_CDELMUTICAST)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CDELMUTICAST) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CDELMUTICAST)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"%d,DevAddr\"\r\nOK\r\n",
                     LORA_AT_CDELMUTICAST);
        } else if (rxcmd_index > (strlen(LORA_AT_CDELMUTICAST) + 1) &&
                   rxcmd[strlen(LORA_AT_CDELMUTICAST)] == '=') {
            uint32_t devAddr;

            str = strtok(&rxcmd[strlen(LORA_AT_CADDMUTICAST) + 1], ",");
            hex2bin(str, &devAddr, 4);
            devAddr =
              (devAddr << 24) & (0xFF000000) | (devAddr << 8) & (0x00FF0000) |
              (devAddr >> 8) & (0x0000FF00) | (devAddr >> 24) & (0x000000FF);

            ret = lora_del_multicast(devAddr);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CNUMMUTICAST,
                       strlen(LORA_AT_CNUMMUTICAST)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CNUMMUTICAST) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CNUMMUTICAST)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"number\"\r\nOK\r\n",
                     LORA_AT_CNUMMUTICAST);
        } else if (rxcmd_index == (strlen(LORA_AT_CDELMUTICAST) + 1) &&
                   rxcmd[strlen(LORA_AT_CDELMUTICAST)] == '?') {
            uint8_t multiNum = get_lora_mulitcast_num();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                     LORA_AT_CNUMMUTICAST, multiNum);
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CFREQBANDMASK,
                       strlen(LORA_AT_CFREQBANDMASK)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CFREQBANDMASK) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CFREQBANDMASK)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mask\"\r\n",
                     LORA_AT_CFREQBANDMASK);
        } else if (rxcmd_index == (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQBANDMASK)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%04x\r\n",
                     LORA_AT_CFREQBANDMASK, get_lora_freqband_mask());
        } else if (rxcmd_index > (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQBANDMASK)] == '=') {
            uint8_t mask[2];
            ret    = false;
            length = hex2bin(&rxcmd[strlen(LORA_AT_CFREQBANDMASK) + 1],
                             (uint8_t *)mask, 2);
            if (length == 2) {
                ret = set_lora_freqband_mask(mask[1] | (mask[0] << 8));
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CFREQBAND, strlen(LORA_AT_CFREQBAND)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CFREQBAND) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CFREQBAND)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"freq band\"\r\nOK\r\n",
                     LORA_AT_CFREQBAND);
        } else if (rxcmd_index == (strlen(LORA_AT_CFREQBAND) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQBAND)] == '?') {
            uint8_t freq_band;
            get_lora_freq_band(&freq_band);

            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                     LORA_AT_CFREQBAND, freq_band);
        } else if (rxcmd_index == (strlen(LORA_AT_CFREQBAND) + 2) &&
                   rxcmd[strlen(LORA_AT_CFREQBAND)] == '=') {
            int freq_band = strtol(&rxcmd[strlen(LORA_AT_CFREQBAND) + 1], NULL, 0);
            if (freq_band < 16) {
                ret = set_lora_freq_band(freq_band);
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CULDLMODE, strlen(LORA_AT_CULDLMODE)) ==
               0) {
        int mode;
        if (rxcmd_index == (strlen(LORA_AT_CULDLMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CULDLMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n",
                     LORA_AT_CULDLMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CULDLMODE) + 1) &&
                   rxcmd[strlen(LORA_AT_CULDLMODE)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CULDLMODE,
                     get_lora_freq_mode());
        } else if (rxcmd_index = (strlen(LORA_AT_CULDLMODE) + 2) &&
                                 rxcmd[strlen(LORA_AT_CULDLMODE)] == '=') {
            mode = strtol(&rxcmd[strlen(LORA_AT_CULDLMODE) + 1], NULL, 0);
            ret  = set_lora_freq_mode(mode);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CWORKMODE, strlen(LORA_AT_CWORKMODE)) ==
               0) {
        int mode;
        if (rxcmd_index == (strlen(LORA_AT_CWORKMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CWORKMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n",
                     LORA_AT_CWORKMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CWORKMODE) + 1) &&
                   rxcmd[strlen(LORA_AT_CWORKMODE)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:2\r\n", LORA_AT_CWORKMODE);
        } else if (rxcmd_index = (strlen(LORA_AT_CWORKMODE) + 2) &&
                                 rxcmd[strlen(LORA_AT_CWORKMODE)] == '=') {
            mode = strtol(&rxcmd[strlen(LORA_AT_CWORKMODE) + 1], NULL, 0);
            if (mode == 2) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CREPEATERFREQ,
                       strlen(LORA_AT_CREPEATERFREQ)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CCLASS, strlen(LORA_AT_CCLASS)) == 0) {
        int8_t class;
        if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CCLASS)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\"class\",\"branch\",\"para1\",\"para2\","
                     "\"para3\",\"para4\"\r\n",
                     LORA_AT_CCLASS);
        } else if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 1) &&
                   rxcmd[strlen(LORA_AT_CCLASS)] == '?') {
            class = get_lora_class();
            if (class == CLASS_A || class == CLASS_C) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                         LORA_AT_CCLASS, class);
            } else if (class == CLASS_B) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n",
                         LORA_AT_CCLASS, class);
            } else {
                ret = false;
            }
        } else if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 2) &&
                   rxcmd[strlen(LORA_AT_CCLASS)] == '=') {
            class = strtol(&rxcmd[strlen(LORA_AT_CCLASS) + 1], NULL, 0);
            ret   = set_lora_class(class);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CBL, strlen(LORA_AT_CBL)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CBL) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CBL)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                     LORA_AT_CBL);
        } else if (rxcmd_index == (strlen(LORA_AT_CBL) + 1) &&
                   rxcmd[strlen(LORA_AT_CBL)] == '?') {
            uint8_t batteryLevel = get_device_battery();
            snprintf(atcmd, ATCMD_SIZE, "%s:%d\r\nOK\r\n", LORA_AT_CBL, status);
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CSTATUS, strlen(LORA_AT_CSTATUS)) == 0) {
        int status;
        if (rxcmd_index == (strlen(LORA_AT_CSTATUS) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CSTATUS)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"status\"\r\nOK\r\n",
                     LORA_AT_CSTATUS);
        } else if (rxcmd_index == (strlen(LORA_AT_CSTATUS) + 1) &&
                   rxcmd[strlen(LORA_AT_CSTATUS)] == '?') {
            status = get_device_status();
            snprintf(atcmd, ATCMD_SIZE, "%s:%d\r\nOK\r\n", LORA_AT_CSTATUS,
                     status);
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CJOIN, strlen(LORA_AT_CJOIN)) == 0) {
        uint8_t  bJoin, bAutoJoin;
        uint16_t joinInterval, joinRetryCnt;
        if (rxcmd_index == (strlen(LORA_AT_CJOIN) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CJOIN)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:<ParaTag1>,[ParaTag2],[ParaTag3],[ParaTag4]\r\n",
                     LORA_AT_CJOIN);
        } else if (rxcmd_index == (strlen(LORA_AT_CJOIN) + 1) &&
                   rxcmd[strlen(LORA_AT_CJOIN)] == '?') {
            get_lora_join_params(&bJoin, &bAutoJoin, &joinInterval,
                                 &joinRetryCnt);
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d,%d,%d\r\nOK\r\n",
                     LORA_AT_CJOIN, bJoin, bAutoJoin, joinInterval,
                     joinRetryCnt);
        } else if (rxcmd_index >= (strlen(LORA_AT_CJOIN) + 2) &&
                   rxcmd[strlen(LORA_AT_CJOIN)] == '=') {
            char *str    = strtok(&rxcmd[strlen(LORA_AT_CJOIN) + 1], ",");
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
    } else if (strncmp(rxcmd, LORA_AT_DTRX, strlen(LORA_AT_DTRX)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_DTRX) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_DTRX)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:[confirm],[nbtrials],[length],<payload>\r\n",
                     LORA_AT_DTRX);
        } else if (rxcmd_index >= (strlen(LORA_AT_DTRX) + 2) &&
                   rxcmd[strlen(LORA_AT_DTRX)] == '=') {
            uint8_t confirm, Nbtrials, len;
            uint8_t payload[LORAWAN_APP_DATA_BUFF_SIZE];
            char   *str = strtok(&rxcmd[strlen(LORA_AT_DTRX) + 1], ",");
            confirm     = strtol(str, NULL, 0);
            str         = strtok(NULL, ",");
            Nbtrials    = strtol(str, NULL, 0);
            str         = strtok(NULL, ",");
            len         = strtol(str, NULL, 0);
            str         = strtok(NULL, ",");
            strcpy(payload, str);
            payload[len] = '\0';
            ret = lora_tx_data_payload(confirm, Nbtrials, payload, len);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK+SEND:\r\nOK+SENT:\r\n");
            }

        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_DRX, strlen(LORA_AT_DRX)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_DRX) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_DRX)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:<Length>,<Payload>\r\n",
                     LORA_AT_DRX);
        } else if (rxcmd_index == (strlen(LORA_AT_DRX) + 1) &&
                   rxcmd[strlen(LORA_AT_DRX)] == '?') {
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
    } else if (strncmp(rxcmd, LORA_AT_CCONFIRM, strlen(LORA_AT_CCONFIRM)) ==
               0) {
        int cfm;
        if (rxcmd_index == (strlen(LORA_AT_CCONFIRM) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CCONFIRM)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                     LORA_AT_CCONFIRM);
        } else if (rxcmd_index == (strlen(LORA_AT_CCONFIRM) + 1) &&
                   rxcmd[strlen(LORA_AT_CCONFIRM)] == '?') {
            cfm = get_lora_tx_cfm_flag();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCONFIRM,
                     cfm);
        } else if (rxcmd_index == (strlen(LORA_AT_CCONFIRM) + 2) &&
                   rxcmd[strlen(LORA_AT_CCONFIRM)] == '=') {
            ret = false;
            cfm = strtol(&rxcmd[strlen(LORA_AT_CCONFIRM) + 1], NULL, 0);
            ret = set_lora_tx_cfm_flag(cfm);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CAPPPORT, strlen(LORA_AT_CAPPPORT)) ==
               0) {
        uint8_t port;
        uint8_t at_cmd_len;
        at_cmd_len = strlen(LORA_AT_CAPPPORT);
        if (rxcmd_index == (at_cmd_len + 2) &&
            strcmp(&rxcmd[at_cmd_len], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                     LORA_AT_CAPPPORT);
        } else if (rxcmd_index == (at_cmd_len + 1) &&
                   rxcmd[at_cmd_len] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CAPPPORT,
                     get_lora_app_port());
        } else if (rxcmd_index > (at_cmd_len + 1) && rxcmd[at_cmd_len] == '=') {
            ret  = false;
            port = (uint8_t)strtol(&rxcmd[at_cmd_len + 1], NULL, 0);
            ret  = set_lora_app_port(port);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CDATARATE, strlen(LORA_AT_CDATARATE)) ==
               0) {
        int8_t datarate;
        if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CDATARATE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                     LORA_AT_CDATARATE);
        } else if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 1) &&
                   rxcmd[strlen(LORA_AT_CDATARATE)] == '?') {
            datarate = get_lora_tx_datarate();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CDATARATE,
                     datarate);
        } else if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 2) &&
                   rxcmd[strlen(LORA_AT_CDATARATE)] == '=') {
            ret      = false;
            datarate = strtol(&rxcmd[strlen(LORA_AT_CDATARATE) + 1], NULL, 0);
            ret      = set_lora_tx_datarate(datarate);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRSSI, strlen(LORA_AT_CRSSI)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CRSSI) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CRSSI)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRSSI);
        } else if ((rxcmd_index > strlen(LORA_AT_CRSSI) + 2) &&
                   (rxcmd[rxcmd_index - 1] == '?')) {
            if (rxcmd_index - strlen(LORA_AT_CRSSI) - 2 == 0) {
                ret = false;
            } else {
                char    freq_str[4];
                int16_t channel_rssi[8];
                strncpy(&freq_str, &rxcmd[strlen(LORA_AT_CRSSI)],
                        rxcmd_index - strlen(LORA_AT_CRSSI) - 2);
                freq_str[rxcmd_index - strlen(LORA_AT_CRSSI) - 2] = '\0';
                uint8_t freq_band = strtol(freq_str, NULL, 0);
                get_lora_rssi(freq_band, channel_rssi);
                snprintf(atcmd, ATCMD_SIZE,
                         "\r\n%s:\r\n0:%d\r\n1:%d\r\n2:%d\r\n3:%d\r\n4:%d\r\n5:"
                         "%d\r\n6:%d\r\n7:%d\r\nOK\r\n",
                         LORA_AT_CRSSI, channel_rssi[0], channel_rssi[1],
                         channel_rssi[2], channel_rssi[3], channel_rssi[4],
                         channel_rssi[5], channel_rssi[6], channel_rssi[7]);
                ret = true;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CNBTRIALS, strlen(LORA_AT_CNBTRIALS)) ==
               0) {
        int8_t m_type, value;
        if (rxcmd_index == (strlen(LORA_AT_CNBTRIALS) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CNBTRIALS)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"MTypes\",\"value\"\r\nOK\r\n",
                     LORA_AT_CNBTRIALS);
        } else if (rxcmd_index == (strlen(LORA_AT_CNBTRIALS) + 1) &&
                   rxcmd[strlen(LORA_AT_CNBTRIALS)] == '?') {
            m_type = get_lora_tx_cfm_flag();
            value  = get_lora_tx_cfm_trials();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d\r\nOK\r\n",
                     LORA_AT_CNBTRIALS, m_type, value);
        } else if (rxcmd_index > (strlen(LORA_AT_CNBTRIALS) + 1) &&
                   rxcmd[strlen(LORA_AT_CNBTRIALS)] == '=') {
            m_type = rxcmd[strlen(LORA_AT_CNBTRIALS) + 1] - '0';
            value  = strtol(&rxcmd[strlen(LORA_AT_CNBTRIALS) + 3], NULL, 0);
            if ((m_type == 0 || m_type == 1) && (value >= 1 && value <= 16)) {
                set_lora_tx_cfm_flag(m_type);
                set_lora_tx_cfm_trials(value);
                ret = true;
            } else {
                ret = false;
            }
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                snprintf(atcmd, ATCMD_SIZE, "\r\nCME ERROR:error%d\r\n", ret);
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRM, strlen(LORA_AT_CRM)) == 0) {
        int8_t   reportMode;
        uint32_t reportInterval;
        if (rxcmd_index == (strlen(LORA_AT_CRM) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CRM)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\"reportMode\",\"reportInterval\"\r\nOK\r\n",
                     LORA_AT_CRM);
        } else if (rxcmd_index == (strlen(LORA_AT_CRM) + 1) &&
                   rxcmd[strlen(LORA_AT_CRM)] == '?') {
            reportMode     = get_lora_report_mode();
            reportInterval = get_lora_tx_dutycycle() / 1000;
            ret            = true;
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d\r\nOK\r\n",
                         LORA_AT_CRM, reportMode, reportInterval);
            } else {
                ret = false;
            }
        } else if (rxcmd_index > (strlen(LORA_AT_CRM) + 2) &&
                   rxcmd[strlen(LORA_AT_CRM)] == '=') {
            reportMode     = rxcmd[strlen(LORA_AT_CRM) + 1] - '0';
            reportInterval = strtol(&rxcmd[strlen(LORA_AT_CRM) + 3], NULL, 0);
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
    } else if (strncmp(rxcmd, LORA_AT_CTXP, strlen(LORA_AT_CTXP)) == 0) {
        int8_t txpwr;
        if (rxcmd_index == (strlen(LORA_AT_CTXP) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CTXP)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                     LORA_AT_CTXP);
        } else if (rxcmd_index == (strlen(LORA_AT_CTXP) + 1) &&
                   rxcmd[strlen(LORA_AT_CTXP)] == '?') {
            txpwr = get_lora_tx_power();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CTXP, txpwr);
        } else if (rxcmd_index == (strlen(LORA_AT_CTXP) + 2) &&
                   rxcmd[strlen(LORA_AT_CTXP)] == '=') {
            ret             = false;
            int8_t txpwrIdx = strtol(&rxcmd[strlen(LORA_AT_CTXP) + 1], NULL, 0);
            ret             = set_lora_tx_power(txpwrIdx);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CLINKCHECK, strlen(LORA_AT_CLINKCHECK)) ==
               0) {
        uint8_t checkValue;
        if (rxcmd_index == (strlen(LORA_AT_CLINKCHECK) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CLINKCHECK)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n",
                     LORA_AT_CLINKCHECK);
        } else if (rxcmd_index > (strlen(LORA_AT_CLINKCHECK) + 1) &&
                   rxcmd[strlen(LORA_AT_CLINKCHECK)] == '=') {
            ret = false;
            checkValue =
              strtol(&rxcmd[strlen(LORA_AT_CLINKCHECK) + 1], NULL, 0);
            if (checkValue == 1) {
                ret = send_lora_link_check();
            } else {
                // TBD:
            }
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CADR, strlen(LORA_AT_CADR)) == 0) {
        int adr;
        if (rxcmd_index == (strlen(LORA_AT_CADR) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CADR)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CADR);
        } else if (rxcmd_index == (strlen(LORA_AT_CADR) + 1) &&
                   rxcmd[strlen(LORA_AT_CADR)] == '?') {
            adr = get_lora_adr();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CADR, adr);
        } else if (rxcmd_index == (strlen(LORA_AT_CADR) + 2) &&
                   rxcmd[strlen(LORA_AT_CADR)] == '=') {
            ret = false;
            adr = strtol(&rxcmd[strlen(LORA_AT_CADR) + 1], NULL, 0);
            ret = set_lora_adr(adr);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRXP, strlen(LORA_AT_CRXP)) == 0) {
        uint8_t  RX1DRoffset;
        uint8_t  RX2DataRate;
        uint32_t RX2Frequency;
        if (rxcmd_index == (strlen(LORA_AT_CRXP) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CRXP)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\"RX1DRoffset\",\"RX2DataRate\","
                     "\"RX2Frequency\"\r\nOK\r\n",
                     LORA_AT_CRXP);
        } else if (rxcmd_index == (strlen(LORA_AT_CRXP) + 1) &&
                   rxcmd[strlen(LORA_AT_CADR)] == '?') {
            get_lora_rx_window_params(&RX1DRoffset, &RX2DataRate,
                                      &RX2Frequency);
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d,%d,%d\r\nOK\r\n",
                     LORA_AT_CRXP, RX1DRoffset, RX2DataRate, RX2Frequency);
        } else if (rxcmd_index > (strlen(LORA_AT_CRXP) + 2) &&
                   rxcmd[strlen(LORA_AT_CRXP)] == '=') {
            char *str    = strtok(&rxcmd[strlen(LORA_AT_CRXP) + 1], ",");
            RX1DRoffset  = strtol(str, NULL, 0);
            str          = strtok(NULL, ",");
            RX2DataRate  = strtol(str, NULL, 0);
            str          = strtok(NULL, ",");
            RX2Frequency = strtol(str, NULL, 0);
            ret =
              set_lora_rx_window_params(RX1DRoffset, RX2DataRate, RX2Frequency);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK-%d,%d,%d\r\n", RX1DRoffset,
                         RX2DataRate, RX2Frequency);
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CFREQLIST, strlen(LORA_AT_CFREQLIST)) ==
               0) {
        uint8_t  method, number;
        uint32_t freqlist[8];
        if (rxcmd_index == (strlen(LORA_AT_CFREQLIST) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CFREQLIST)], "=?") == 0) {
            snprintf(
              atcmd, ATCMD_SIZE,
              "\r\n%s:\"ULDL\",\"method\",\"number\",\"freqlist\"\r\nOK\r\n",
              LORA_AT_CFREQLIST);
        } else if (rxcmd_index == (strlen(LORA_AT_CFREQLIST) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQLIST)] == '?') {
            method = get_lora_freq_mode();
            if (uldl_mode == 1) {
                if (FREQ_MODE_INTER == method) {
                    // TBD:
                } else if (FREQ_MODE_INTRA == method) {
                }
            } else if (uldl_mode == 2) {
                if (FREQ_MODE_INTER == method) {
                    // TBD:
                } else if (FREQ_MODE_INTRA == method) {
                }
            }
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CFREQLIST);
        } else if (rxcmd_index > (strlen(LORA_AT_CFREQLIST) + 2) &&
                   rxcmd[strlen(LORA_AT_CFREQLIST)] == '=') {

            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRX1DELAY, strlen(LORA_AT_CRX1DELAY)) ==
               0) {
        uint32_t rx1delay;
        if (rxcmd_index == (strlen(LORA_AT_CRX1DELAY) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CRX1DELAY)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n",
                     LORA_AT_CRX1DELAY);
        } else if (rxcmd_index == (strlen(LORA_AT_CRX1DELAY) + 1) &&
                   rxcmd[strlen(LORA_AT_CRX1DELAY)] == '?') {
            rx1delay = get_lora_mac_rx1_delay();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CRX1DELAY,
                     rx1delay);
        } else if (rxcmd_index >= (strlen(LORA_AT_CRX1DELAY) + 2) &&
                   rxcmd[strlen(LORA_AT_CRX1DELAY)] == '=') {
            ret      = false;
            rx1delay = strtol(&rxcmd[strlen(LORA_AT_CRX1DELAY) + 1], NULL, 0);
            ret      = set_lora_mac_rx1_delay(rx1delay);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CSAVE, strlen(LORA_AT_CSAVE)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CSAVE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CSAVE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSAVE);
        } else if (rxcmd_index == strlen(LORA_AT_CSAVE)) {
            LoRaMacParams_t *mMacP;
            mMacP = get_lora_mac_params();
            memcpy(&gMacParams, mMacP, sizeof(LoRaMacParams_t));
#ifdef AOS_COMP_KV
            aos_kv_set("gLoRaMacPara", &gMacParams, sizeof(LoRaMacParams_t), 1);
#endif
#if 0
            uint32_t len = sizeof(LoRaMacParams_t);
            aos_kv_get("gLoRaMacPara", &gMacParams, &len);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK%d-%d-%d-%d\r\n", gMacParams.ChannelsTxPower, gMacParams.ReceiveDelay1,
                     gMacParams.ReceiveDelay2, gMacParams.MaxRxWindow);
#endif
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRESTORE, strlen(LORA_AT_CRESTORE)) ==
               0) {
        if (rxcmd_index == (strlen(LORA_AT_CRESTORE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CRESTORE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSAVE);
        } else if (rxcmd_index == strlen(LORA_AT_CRESTORE)) {
#ifdef AOS_COMP_KV
            aos_kv_set("gLoRaMacPara", &LoRaMacParamsDefaults,
                       sizeof(LoRaMacParams_t), 1);
#endif
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
#if 0
            uint32_t len = sizeof(LoRaMacParams_t);
            aos_kv_get("gLoRaMacPara", &gMacParams, &len);
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK%d-%d-%d-%d\r\n", gMacParams.ChannelsTxPower, gMacParams.ReceiveDelay1,
                     gMacParams.ReceiveDelay2, gMacParams.MaxRxWindow);
#endif
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CREPEATERFILTER,
                       strlen(LORA_AT_CREPEATERFILTER)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGMI, strlen(LORA_AT_CGMI)) == 0) {
        ret = true;
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMI,
                 aos_mft_itf.get_mft_id());
    } else if (strncmp(rxcmd, LORA_AT_CGMM, strlen(LORA_AT_CGMM)) == 0) {
        ret = true;
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMM,
                 aos_mft_itf.get_mft_model());
    } else if (strncmp(rxcmd, LORA_AT_CGMR, strlen(LORA_AT_CGMR)) == 0) {
        ret = true;
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGMR,
                 aos_mft_itf.get_mft_rev());
    } else if (strncmp(rxcmd, LORA_AT_CGSN, strlen(LORA_AT_CGSN)) == 0) {
        ret = true;
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s=0x%04x\r\nOK\r\n", LORA_AT_CGSN,
                 aos_mft_itf.get_mft_sn());
    } else if (strncmp(rxcmd, LORA_AT_CGBR, strlen(LORA_AT_CGBR)) == 0) {
        uint32_t baud;
        uint8_t  at_cmd_len;
        at_cmd_len = strlen(LORA_AT_CGBR);
        if (rxcmd_index == (at_cmd_len + 2) &&
            strcmp(&rxcmd[at_cmd_len], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CGBR);
        } else if (rxcmd_index == (at_cmd_len + 1) &&
                   rxcmd[at_cmd_len] == '?') {
            baud = aos_mft_itf.get_mft_baud();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CGBR, baud);
        } else if (rxcmd_index > (at_cmd_len + 1) && rxcmd[at_cmd_len] == '=') {
            ret  = false;
            baud = strtol(&rxcmd[at_cmd_len + 1], NULL, 0);
            ret  = aos_mft_itf.set_mft_baud(baud);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }

    } else if (strncmp(rxcmd, LORA_AT_ILOGLVL, strlen(LORA_AT_ILOGLVL)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_IREBOOT, strlen(LORA_AT_IREBOOT)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_IREBOOT) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_IREBOOT)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n", LORA_AT_IREBOOT);
        } else if (rxcmd_index == (strlen(LORA_AT_IREBOOT) + 2) &&
                   rxcmd[strlen(LORA_AT_IREBOOT)] == '=') {
            ret         = false;
            int8_t mode = strtol(&rxcmd[strlen(LORA_AT_IREBOOT) + 1], NULL, 0);
            if (mode == 0 || mode == 1) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                lora_reboot(mode);
                ret = true;
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CTEST, strlen(LORA_AT_CTEST)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CTEST) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CTEST)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:[idx]\r\n", LORA_AT_CTEST);
        } else if (rxcmd_index >= (strlen(LORA_AT_CTEST) + 2) &&
                   rxcmd[strlen(LORA_AT_CTEST)] == '=') {
            uint8_t idx = strtol(&rxcmd[strlen(LORA_AT_CTEST) + 1], NULL, 16);
            ret = lora_compliance_test(idx);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_IDEFAULT, strlen(LORA_AT_IDEFAULT)) ==
               0) {
        ret = false;
    } else {
        ret = false;
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

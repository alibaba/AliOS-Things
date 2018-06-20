/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LoRaMac.h"
#include "Region.h"
#include "linkwan.h"
#include "linkwan_ica_at.h"

#define ATCMD_SIZE (LORAWAN_APP_DATA_BUFF_SIZE + 14)
#define PORT_LEN 4

uint8_t atcmd[ATCMD_SIZE];
uint16_t atcmd_index = 0;

static int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
    uint16_t hex_length = strlen(hex);
    const char *hex_end = hex + hex_length;
    uint8_t *cur = bin;
    uint8_t num_chars = hex_length & 1;
    uint8_t byte = 0;

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
            *cur++ = byte;
            byte = 0;
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
        (cmd >= 'A' && cmd <= 'Z') || cmd == '?' || cmd == '+' ||
        cmd == ':' || cmd == '=') {
        atcmd[atcmd_index++] = cmd;
    } else if (cmd == '\r' || cmd == '\n') {
        atcmd[atcmd_index] = '\0';
    }

    if (atcmd_index > ATCMD_SIZE) {
        atcmd_index = 0;
    }
}

void process_linkwan_at(void)
{
    bool ret = true;
    int value;
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    uint8_t length;
    uint8_t buf[16];
    uint8_t *rxcmd = atcmd + 2;
    int16_t rxcmd_index = atcmd_index - 2;

    if (rxcmd_index <= 0 || rxcmd[rxcmd_index] != '\0') {
        return;
    }

    if (strncmp(rxcmd, LORA_AT_CJOINMODE, strlen(LORA_AT_CJOINMODE)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CJOINMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"mode\"\r\nOK\r\n", LORA_AT_CJOINMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 1) &&
                   rxcmd[strlen(LORA_AT_CJOINMODE)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:0\r\nOK\r\n", LORA_AT_CJOINMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CJOINMODE) + 2) &&
                   rxcmd[strlen(LORA_AT_CJOINMODE)] == '=') {
            int mode = strtol(&rxcmd[strlen(LORA_AT_CJOINMODE) + 1], NULL, 0);
            if (mode == 0 || mode == 1) {
                // set join mode
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
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"DevEUI:length is 16\"\r\n", LORA_AT_CDEVEUI);
        } else if (rxcmd_index == (strlen(LORA_AT_CDEVEUI) + 1) &&
                   rxcmd[strlen(LORA_AT_CDEVEUI)] == '?') {
            uint8_t *eui = get_lora_dev_eui();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n", \
                     LORA_AT_CDEVEUI, eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);
        } else if (rxcmd_index == (strlen(LORA_AT_CDEVEUI) + 17) &&
                   rxcmd[strlen(LORA_AT_CDEVEUI)] == '=') {
            ret = false;
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
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppEUI:length is 16\"\r\n", LORA_AT_CAPPEUI);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPEUI) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '?') {
            uint8_t *eui = get_lora_app_eui();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n", \
                     LORA_AT_CAPPEUI, eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPEUI) + 17) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '=') {
            ret = false;
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
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"AppKey:length is 32\"\r\n", LORA_AT_CAPPKEY);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPKEY) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPKEY)] == '?') {
            uint8_t *key = get_lora_app_key();
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\nOK\r\n", \
                     LORA_AT_CAPPKEY, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
                     key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPKEY) + 33) &&
                   rxcmd[strlen(LORA_AT_CAPPEUI)] == '=') {
            ret = false;
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
    } else if (strncmp(rxcmd, LORA_AT_CDEVADDR, strlen(LORA_AT_CDEVADDR)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CAPPSKEY, strlen(LORA_AT_CAPPSKEY)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CNWKSKEY, strlen(LORA_AT_CNWKSKEY)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CADDMUTICAST, strlen(LORA_AT_CADDMUTICAST)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CDELMUTICAST, strlen(LORA_AT_CDELMUTICAST)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CNUMMUTICAST, strlen(LORA_AT_CNUMMUTICAST)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CFREQBANDMASK, strlen(LORA_AT_CFREQBANDMASK)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_CFREQBANDMASK) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CFREQBANDMASK)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mask\"\r\n", LORA_AT_CFREQBANDMASK);
        } else if (rxcmd_index == (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQBANDMASK)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%04x\r\n", LORA_AT_CFREQBANDMASK, get_lora_freqband_mask());
        } else if (rxcmd_index > (strlen(LORA_AT_CFREQBANDMASK) + 1) &&
                   rxcmd[strlen(LORA_AT_CFREQBANDMASK)] == '=') {
            uint8_t mask[2];
            ret = false;
            length = hex2bin(&rxcmd[strlen(LORA_AT_CFREQBANDMASK) + 1], (uint8_t *)mask, 2);
            if (length == 2) {
                ret = set_lora_freqband_mask(mask[1] | (mask[0] << 8));
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CULDLMODE, strlen(LORA_AT_CULDLMODE)) == 0) {
        int mode;
        if (rxcmd_index == (strlen(LORA_AT_CULDLMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CULDLMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n", LORA_AT_CULDLMODE);
        } else if (rxcmd_index == (strlen(LORA_AT_CULDLMODE) + 1) &&
                   rxcmd[strlen(LORA_AT_CULDLMODE)] == '?') {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CULDLMODE, get_lora_freq_mode());
        } else if (rxcmd_index = (strlen(LORA_AT_CULDLMODE) + 2) &&
                   rxcmd[strlen(LORA_AT_CULDLMODE)] == '=') {
            mode = strtol(&rxcmd[strlen(LORA_AT_CULDLMODE) + 1], NULL, 0);
            ret = set_lora_freq_mode(mode);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CWORKMODE, strlen(LORA_AT_CWORKMODE)) == 0) {
        int mode;
        if (rxcmd_index == (strlen(LORA_AT_CWORKMODE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CWORKMODE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\n", LORA_AT_CWORKMODE);
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
    } else if (strncmp(rxcmd, LORA_AT_CREPEATERFREQ, strlen(LORA_AT_CREPEATERFREQ)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CCLASS, strlen(LORA_AT_CCLASS)) == 0) {
        int8_t class;
        if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CCLASS)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s:\"class\",\"branch\",\"para1\",\"para2\",\"para3\",\"para4\"\r\n",
                     LORA_AT_CCLASS);
        } else if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 1) &&
                   rxcmd[strlen(LORA_AT_CCLASS)] == '?') {
            class = get_lora_class();
            if (class == CLASS_A || class == CLASS_C) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCLASS, class);
            } else if (class == CLASS_B) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCLASS, class);
            } else {
                ret = false;
            }
        } else if (rxcmd_index == (strlen(LORA_AT_CCLASS) + 2) &&
                   rxcmd[strlen(LORA_AT_CCLASS)] == '=') {
            class = strtol(&rxcmd[strlen(LORA_AT_CCLASS) + 1], NULL, 0);
            ret = set_lora_class(class);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CBL, strlen(LORA_AT_CBL)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CSTATUS, strlen(LORA_AT_CSTATUS)) == 0) {
        int status;
        if (rxcmd_index == (strlen(LORA_AT_CSTATUS) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CSTATUS)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"status\"\r\nOK\r\n", LORA_AT_CSTATUS);
        } else if (rxcmd_index == (strlen(LORA_AT_CSTATUS) + 1) &&
                   rxcmd[strlen(LORA_AT_CSTATUS)] == '?') {
            status = get_device_status();
            snprintf(atcmd, ATCMD_SIZE, "%s:%d\r\nOK\r\n", LORA_AT_CSTATUS, status);
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CJOIN, strlen(LORA_AT_CJOIN)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_DTRX, strlen(LORA_AT_DTRX)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_DRX, strlen(LORA_AT_DRX)) == 0) {
        if (rxcmd_index == (strlen(LORA_AT_DRX) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_DRX)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:<Length>,<Payload>\r\n", LORA_AT_DRX);
        } else if (rxcmd_index == (strlen(LORA_AT_DRX) + 1) &&
                   rxcmd[strlen(LORA_AT_DRX)] == '?') {
            lora_AppData_t *rx_data;
            int16_t len = 0;

            rx_data = get_lora_rx_data();
            len = snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d", LORA_AT_DRX, rx_data->BuffSize);
            if (rx_data->BuffSize > 0) {
                len += snprintf(atcmd, ATCMD_SIZE, ",");
                memcpy(atcmd + len, rx_data->Buff, rx_data->BuffSize);
                len += rx_data->BuffSize;
            }
            rx_data->BuffSize = 0;
            snprintf(atcmd + len, ATCMD_SIZE, "\r\n%s\r\n", "OK");
        }
    } else if (strncmp(rxcmd, LORA_AT_CCONFIRM, strlen(LORA_AT_CCONFIRM)) == 0) {
        int cfm;
        if (rxcmd_index == (strlen(LORA_AT_CCONFIRM) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CCONFIRM)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CCONFIRM);
        } else if (rxcmd_index == (strlen(LORA_AT_CCONFIRM) + 1) &&
                   rxcmd[strlen(LORA_AT_CCONFIRM)] == '?') {
            cfm = get_lora_tx_cfm_flag();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCONFIRM, cfm);
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
    } else if (strncmp(rxcmd, LORA_AT_CAPPPORT, strlen(LORA_AT_CAPPPORT)) == 0) {
        lora_AppData_t *tx_data;
        if (rxcmd_index == (strlen(LORA_AT_CAPPPORT) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CAPPPORT)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CAPPPORT);
        } else if (rxcmd_index == (strlen(LORA_AT_CAPPPORT) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPPORT)] == '?') {
            ret = false;
        } else if (rxcmd_index > (strlen(LORA_AT_CAPPPORT) + 1) &&
                   rxcmd[strlen(LORA_AT_CAPPPORT)] == '=') {
            ret = false;
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CDATARATE, strlen(LORA_AT_CDATARATE)) == 0) {
        int8_t datarate;
        if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 2) &&
            strcmp(&rxcmd[strlen(LORA_AT_CDATARATE)], "=?") == 0) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\n", LORA_AT_CDATARATE);
        } else if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 1) &&
                   rxcmd[strlen(LORA_AT_CDATARATE)] == '?') {
            datarate = get_lora_tx_datarate();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%d\r\n", LORA_AT_CDATARATE, datarate);
        } else if (rxcmd_index == (strlen(LORA_AT_CDATARATE) + 2) &&
                   rxcmd[strlen(LORA_AT_CDATARATE)] == '=') {
            ret = false;
            datarate = strtol(&rxcmd[strlen(LORA_AT_CDATARATE) + 1], NULL, 0);
            ret = set_lora_tx_datarate(datarate);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
        } else {
            ret = false;
        }
    } else if (strncmp(rxcmd, LORA_AT_CRSSI, strlen(LORA_AT_CRSSI)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CNBTRIALS, strlen(LORA_AT_CNBTRIALS)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CRM, strlen(LORA_AT_CRM)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CTXP, strlen(LORA_AT_CTXP)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CLINKCHECK, strlen(LORA_AT_CLINKCHECK)) == 0) {
        ret = false;
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
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CFREQLIST, strlen(LORA_AT_CFREQLIST)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CRX1DELAY, strlen(LORA_AT_CRX1DELAY)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CSAVE, strlen(LORA_AT_CSAVE)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CSAVE, strlen(LORA_AT_CRESTORE)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CSAVE, strlen(LORA_AT_CREPEATERFILTER)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGMI, strlen(LORA_AT_CGMI)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGMM, strlen(LORA_AT_CGMM)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGMR, strlen(LORA_AT_CGMR)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGSN, strlen(LORA_AT_CGSN)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_CGBR, strlen(LORA_AT_CGBR)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_ILOGLVL, strlen(LORA_AT_ILOGLVL)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_IREBOOT, strlen(LORA_AT_IREBOOT)) == 0) {
        ret = false;
    } else if (strncmp(rxcmd, LORA_AT_IDEFAULT, strlen(LORA_AT_IDEFAULT)) == 0) {
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

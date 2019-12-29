/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LoRaMac.h"
#include "Region.h"
#include "linkwan.h"
#include "linkwan_at.h"

#define ATCMD_SIZE (LORAWAN_APP_DATA_BUFF_SIZE + 14)
#define PORT_LEN 4

uint8_t  atcmd[ATCMD_SIZE];
uint16_t atcmd_index = 0;

static int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
    uint16_t    hex_length = strlen(hex);
    const char *hex_end    = hex + hex_length;
    uint8_t *   cur        = bin;
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
        (cmd >= 'A' && cmd <= 'Z') || cmd == '?' || cmd == '+' || cmd == ':') {
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
    bool                ret = false;
    int                 value;
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t     status;
    uint8_t             length;
    uint8_t             buf[16];

    if (atcmd_index == 0 || atcmd[atcmd_index] != '\0') {
        return;
    }
    if (strncmp(atcmd, LORA_AT_HELP, strlen(LORA_AT_HELP)) == 0) {
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s\r\n", LORA_AT_HELP);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_RM);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_APPEUI);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_APPKEY);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_DEUI);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_DR);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_ADR);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_CLASS);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_SCANMASK);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_CFM);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_CFMTRIALS);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_JOIN);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_TX);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_RX);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_DCS);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_TXSIZE);
        linkwan_serial_output(atcmd, strlen(atcmd));
        snprintf(atcmd, ATCMD_SIZE, "%s\r\n", LORA_AT_LINKCHK);
        linkwan_serial_output(atcmd, strlen(atcmd));
        return;
    } else if (strncmp(atcmd, LORA_AT_APPEUI, strlen(LORA_AT_APPEUI)) == 0) {
        if (atcmd_index == strlen(LORA_AT_APPEUI)) {
            uint8_t eui[8];
            get_lora_app_eui(eui);
            ret = true;
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s %02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                     LORA_AT_APPEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                     eui[5], eui[6], eui[7]);
        } else if (atcmd_index == (strlen(LORA_AT_APPEUI) + 16)) {
            length = hex2bin(&atcmd[strlen(LORA_AT_APPEUI)], buf, 8);
            if (length == 8) {
                ret = set_lora_app_eui(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE,
                             "\r\n%s %02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                             LORA_AT_APPEUI, buf[0], buf[1], buf[2], buf[3],
                             buf[4], buf[5], buf[6], buf[7]);
                }
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_APPEUI);
        }
    } else if (strncmp(atcmd, LORA_AT_APPKEY, strlen(LORA_AT_APPKEY)) == 0) {
        if (atcmd_index == strlen(LORA_AT_APPKEY)) {
            uint8_t key[16];
            get_lora_app_key(key);
            ret = true;
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s "
                     "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%"
                     "02x%02x\r\n",
                     LORA_AT_APPKEY, key[0], key[1], key[2], key[3], key[4],
                     key[5], key[6], key[7], key[8], key[9], key[10], key[11],
                     key[12], key[13], key[14], key[15]);
        } else if (atcmd_index == (strlen(LORA_AT_APPKEY) + 32)) {
            length = hex2bin(&atcmd[strlen(LORA_AT_APPKEY)], buf, 16);
            if (length == 16) {
                ret = set_lora_app_key(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE,
                             "\r\n%s "
                             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%"
                             "02x%02x%02x%02x\r\n",
                             LORA_AT_APPKEY, buf[0], buf[1], buf[2], buf[3],
                             buf[4], buf[5], buf[6], buf[7], buf[8], buf[9],
                             buf[10], buf[11], buf[12], buf[13], buf[14],
                             buf[15]);
                }
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_APPKEY);
        }
    } else if (strncmp(atcmd, LORA_AT_DEUI, strlen(LORA_AT_DEUI)) == 0) {
        if (atcmd_index == strlen(LORA_AT_DEUI)) {
            uint8_t eui[8];
            get_lora_dev_eui(eui);
            ret = true;
            snprintf(atcmd, ATCMD_SIZE,
                     "\r\n%s %02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                     LORA_AT_DEUI, eui[0], eui[1], eui[2], eui[3], eui[4],
                     eui[5], eui[6], eui[7]);
        } else if (atcmd_index == (strlen(LORA_AT_DEUI) + 16)) {
            length = hex2bin(&atcmd[strlen(LORA_AT_DEUI)], buf, 8);
            if (length == 8) {
                ret = set_lora_dev_eui(buf);
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE,
                             "\r\n%s %02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                             LORA_AT_DEUI, buf[0], buf[1], buf[2], buf[3],
                             buf[4], buf[5], buf[6], buf[7]);
                }
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_DEUI);
        }
    } else if (strncmp(atcmd, LORA_AT_RM, strlen(LORA_AT_RM)) == 0) {
        aos_kv_del("lora_dev");
        ret = true;
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s OK\r\n", LORA_AT_RM);
    } else if (strncmp(atcmd, LORA_AT_DR, strlen(LORA_AT_DR)) == 0) {
        int8_t datarate;
        if (atcmd_index == strlen(LORA_AT_DR)) {
            ret      = true;
            datarate = get_lora_tx_datarate();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_DR, datarate);
        } else if (atcmd_index == (strlen(LORA_AT_DR) + 1)) {
            datarate = strtol(atcmd + strlen(LORA_AT_DR), NULL, 0);
            ret      = set_lora_tx_datarate(datarate);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_DR,
                         datarate);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_DR);
        }
    } else if (strncmp(atcmd, LORA_AT_ADR, strlen(LORA_AT_ADR)) == 0) {
        int adr;
        if (atcmd_index == strlen(LORA_AT_ADR)) {
            ret = true;
            adr = get_lora_adr();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_ADR, adr);
        } else if (atcmd_index == (strlen(LORA_AT_ADR) + 1)) {
            adr = strtol(atcmd + strlen(LORA_AT_ADR), NULL, 0);
            ret = set_lora_adr(adr);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_ADR, adr);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_ADR);
        }
    } else if (strncmp(atcmd, LORA_AT_CLASS, strlen(LORA_AT_CLASS)) == 0) {
        int8_t class;
        if (atcmd_index == strlen(LORA_AT_CLASS)) {
            ret   = true;
            class = get_lora_class();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CLASS, class);
        } else if (atcmd_index == (strlen(LORA_AT_CLASS) + 1)) {
            class = strtol(atcmd + strlen(LORA_AT_CLASS), NULL, 0);
            if (class == 0) {
                ret = set_lora_class(CLASS_A);
            } else if (class == 2) {
                ret = set_lora_class(CLASS_C);
            }
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CLASS,
                         class);
                atcmd_index = strlen(atcmd);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_CLASS);
        }
    } else if (strncmp(atcmd, LORA_AT_CFM, strlen(LORA_AT_CFM)) == 0) {
        int cfm;
        if (atcmd_index == strlen(LORA_AT_CFM)) {
            ret = true;
            cfm = get_lora_tx_cfm_flag();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CFM, cfm);
        } else if (atcmd_index == (strlen(LORA_AT_CFM) + 1)) {
            cfm = strtol(atcmd + strlen(LORA_AT_CFM), NULL, 0);
            ret = set_lora_tx_cfm_flag(cfm);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CFM, cfm);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_CFM);
        }
    } else if (strncmp(atcmd, LORA_AT_CFMTRIALS, strlen(LORA_AT_CFMTRIALS)) ==
               0) {
        uint8_t trials;
        if (atcmd_index == strlen(LORA_AT_CFMTRIALS)) {
            ret    = true;
            trials = get_lora_tx_cfm_trials();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CFMTRIALS,
                     trials);
        } else if (atcmd_index > strlen(LORA_AT_CFMTRIALS)) {
            trials = strtol(atcmd + strlen(LORA_AT_CFMTRIALS), NULL, 0);
            ret    = set_lora_tx_cfm_trials(trials);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_CFMTRIALS,
                         trials);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_CFMTRIALS);
        }
    } else if (strncmp(atcmd, LORA_AT_JOIN, strlen(LORA_AT_JOIN)) == 0) {
        ret = set_device_state(DEVICE_STATE_JOIN);
        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s OK\r\n", LORA_AT_JOIN);
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_JOIN);
        }
    } else if (strncmp(atcmd, LORA_AT_DCS, strlen(LORA_AT_DCS)) == 0) {
        uint32_t dutycycle;
        if (atcmd_index == strlen(LORA_AT_DCS)) {
            ret       = true;
            dutycycle = get_lora_tx_dutycycle();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_DCS,
                     dutycycle);
        } else if (atcmd_index > strlen(LORA_AT_DCS)) {
            dutycycle = strtol(atcmd + strlen(LORA_AT_DCS), NULL, 0);
            ret       = set_lora_tx_dutycycle(dutycycle);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_DCS,
                         dutycycle);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_DCS);
        }
    } else if (strncmp(atcmd, LORA_AT_LINKCHK, strlen(LORA_AT_LINKCHK)) == 0) {
        ret = send_lora_link_check();
        if (ret == true) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s OK\r\n", LORA_AT_LINKCHK);
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_LINKCHK);
        }
    } else if (strncmp(atcmd, LORA_AT_SCANMASK, strlen(LORA_AT_SCANMASK)) ==
               0) {
        uint8_t mask[2];
        int     length;
        if (atcmd_index == strlen(LORA_AT_SCANMASK)) {
            ret = true;
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s 0x%04x\r\n", LORA_AT_SCANMASK,
                     get_lora_freqband_mask());
        } else if (atcmd_index > strlen(LORA_AT_SCANMASK)) {
            length =
              hex2bin(&atcmd[strlen(LORA_AT_SCANMASK)], (uint8_t *)mask, 2);
            if (length == 2) {
                ret = set_lora_freqband_mask(mask[1] | (mask[0] << 8));
                if (ret == true) {
                    snprintf(atcmd, ATCMD_SIZE, "\r\n%s 0x%04x\r\n",
                             LORA_AT_SCANMASK, get_lora_freqband_mask());
                }
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_SCANMASK);
        }
    } else if (strncmp(atcmd, LORA_AT_TX, strlen(LORA_AT_TX)) == 0) {
        uint8_t         port_num[PORT_LEN];
        uint8_t *       cmd   = &atcmd[strlen(LORA_AT_TX)];
        uint8_t         index = 0;
        lora_AppData_t *tx_data;

        tx_data = get_lora_tx_data();
        if (tx_data == NULL || atcmd_index == strlen(LORA_AT_TX) + 1) {
            ret = false;
        } else {
            for (index = 0; index < PORT_LEN && cmd[index] != ':'; index++) {
                port_num[index] = cmd[index];
            }
            if (cmd[index] != ':') {
                ret = false;
            } else {
                port_num[index] = '\0';
                ret             = true;
            }
            if (ret == true) {
                tx_data->Port = (uint8_t)atoi((char *)port_num);
                tx_data->BuffSize =
                  atcmd_index - strlen(LORA_AT_TX) - index - 1;
                memcpy(tx_data->Buff, &atcmd[strlen(LORA_AT_TX) + index + 1],
                       tx_data->BuffSize);
                ret = tx_lora_data();
            }
        }
        snprintf(atcmd, ATCMD_SIZE, "\r\n%s %s\r\n", LORA_AT_TX,
                 ret == true ? "OK" : "ERROR");
    } else if (strncmp(atcmd, LORA_AT_RX, strlen(LORA_AT_RX)) == 0) {
        lora_AppData_t *rx_data;
        int16_t         len = 0;

        len     = snprintf(atcmd, ATCMD_SIZE, "\r\n%s ", LORA_AT_RX);
        rx_data = get_lora_rx_data();

        if (rx_data->BuffSize > 0) {
            len += snprintf(atcmd + len, ATCMD_SIZE, "%02x:", rx_data->Port);
            memcpy(atcmd + len, rx_data->Buff, rx_data->BuffSize);
            len += rx_data->BuffSize;
            len += snprintf(atcmd + len, ATCMD_SIZE, " ");
        }
        snprintf(atcmd + len, ATCMD_SIZE, "%s\r\n", "OK");
    } else if (strncmp(atcmd, LORA_AT_TXSIZE, strlen(LORA_AT_TXSIZE)) == 0) {
        uint8_t len;
        if (atcmd_index == strlen(LORA_AT_TXSIZE)) {
            ret = true;
            len = get_lora_tx_len();
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_TXSIZE, len);
        } else if (atcmd_index > strlen(LORA_AT_TXSIZE)) {
            len = strtol(atcmd + strlen(LORA_AT_TXSIZE), NULL, 0);
            ret = set_lora_tx_len(len);
            if (ret == true) {
                snprintf(atcmd, ATCMD_SIZE, "\r\n%s %d\r\n", LORA_AT_TXSIZE,
                         len);
            }
        }
        if (ret == false) {
            snprintf(atcmd, ATCMD_SIZE, "\r\n%s ERROR\r\n", LORA_AT_TXSIZE);
        }
    } else {
        snprintf(atcmd, ATCMD_SIZE, "\r\nERROR\r\n");
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

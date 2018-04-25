/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LoRaMac.h"
#include "Region.h"
#include "linkwan.h"

#define RX_CMD_SIZE 35
uint8_t rx_cmd[RX_CMD_SIZE + 1];
uint8_t rx_cmd_index = 0;

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
        if ('a' <= *hex && *hex <= 'f') {
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

void linkwan_test_cli_cb(uint8_t cmd)
{
    bool ret = true;
    int value;
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    uint8_t length;
    uint8_t buf[16];

    if (rx_cmd_index < RX_CMD_SIZE) {
        if ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'f') || cmd == '?') {
            rx_cmd[rx_cmd_index++] = cmd;
            return;
        } else if (cmd == ' ') {
            return;
        } else if (cmd != '\r') {
            goto exit;
        }
    } else {
        goto exit;
    }
    rx_cmd[rx_cmd_index] = '\0';

    switch (rx_cmd[0]) {
        case '?':
            PRINTF("0: size\r\n");
            PRINTF("1: datarate\r\n");
            PRINTF("2: dutyclcle in secconds\r\n");
            PRINTF("3: cfmed (0 or 1)\r\n");
            PRINTF("4: cfmed trials\r\n");
            PRINTF("5: join\r\n");
            PRINTF("6: linkcheck\r\n");
            PRINTF("7: ADR (0 or 1) \r\n");
            PRINTF("8: sleep (0 or 1) \r\n");
            PRINTF("9: class (0, 1 or 2) \r\n");
            PRINTF("a: dev eui\r\n");
            PRINTF("b: app eui\r\n");
            PRINTF("c: app key\r\n");
            PRINTF("d: delete stored info\r\n");
            break;
        case '0':
            value = strtol(&rx_cmd[1], NULL, 0);
            ret = set_lora_tx_len(value);
            if (ret == true) {
                PRINTF("size %d\r\n", value);
            }
            break;
        case '1':
            value = strtol(&rx_cmd[1], NULL, 0);
            ret = set_lora_tx_datarate(value);
            if (ret == true) {
                PRINTF("datarate %d\r\n", value);
            }
            break;
        case '2':
            value = strtol(&rx_cmd[1], NULL, 0);
            ret = set_lora_tx_dutycycle(value * 1000);
            if (ret == true) {
                PRINTF("dutycycle %d\r\n", value);
            }
            break;
        case '3':
            value = strtol(&rx_cmd[1], NULL, 0);
            if (value == 0) {
                ret = set_lora_tx_confirmed_flag(DISABLE);
            } else {
                ret = set_lora_tx_confirmed_flag(ENABLE);
            }
            if (ret == true) {
                PRINTF("cfm %s\r\n", ((value == 0)? "off": "on"));
            }
            break;
        case '4':
            value = strtol(&rx_cmd[1], NULL, 0);
            ret = set_lora_tx_num_trials(value);
            if (ret == true) {
                PRINTF("retrials %d\r\n", value);
            }
            break;
        case '5':
            ret = set_lora_state(DEVICE_STATE_JOIN);
            break;
        case '6':
            ret = send_lora_link_check();
            if (ret == true) {
                PRINTF("send link check request\r\n");
            }
            break;
        case '7':
            value = strtol(&rx_cmd[1], NULL, 0);
            if (value == 0) {
                mibReq.Param.AdrEnable = LORAWAN_ADR_OFF;
            } else {
                mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
            }
            mibReq.Type = MIB_ADR;
            status = LoRaMacMibSetRequestConfirm(&mibReq);
            if (status == LORAMAC_STATUS_OK) {
                PRINTF("ADR %s\r\n", (value == 0? "off": "on"));
            } else {
                ret = false;
            }
            break;
        case '8':
            value = strtol(&rx_cmd[1], NULL, 0);
            if (value == 0) {
                ret = set_lora_state(DEVICE_STATE_SLEEP);
            } else {
                ret = set_lora_state(DEVICE_STATE_SEND);
            }
            if (ret == true) {
                PRINTF("device %s\r\n", ((value == 0)? "sleep": "wakeup"));
            }
            break;
        case '9':
            value = strtol(&rx_cmd[1], NULL, 0);
            if (value == 0) {
                ret = set_lora_class(CLASS_A);
            } else {
                ret = set_lora_class(CLASS_C);
            }
            if (ret == true) {
                PRINTF("class %s\r\n", ((value == 0)? "class_a": "class_c"));
            }
            break;
        case 'a':
            if (rx_cmd_index > 17) {
                ret = false;
                goto exit;
            }
            length = hex2bin(&rx_cmd[1], buf, 8);
            if (length != 8) {
                ret = false;
                goto exit;
            }
            ret = set_lora_dev_eui(buf);
            if (ret == true) {
                PRINTF("dev_eui %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
                       buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
            }
            break;
        case 'b':
            if (rx_cmd_index > 17) {
                ret = false;
                goto exit;
            }
            length = hex2bin(&rx_cmd[1], buf, 8);
            if (length != 8) {
                ret = false;
                goto exit;
            }
            ret = set_lora_app_eui(buf);
            if (ret == true) {
                PRINTF("app_eui %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
                       buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
            }
            break;
        case 'c':
            if (rx_cmd_index > 33) {
                ret = false;
                goto exit;
            }
            length = hex2bin(&rx_cmd[1], buf, 16);
            if (length != 16) {
                ret = false;
                goto exit;
            }
            ret = set_lora_app_key(buf);
            if (ret == true) {
                PRINTF("app_key %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
                       buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], \
                       buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
            }
            break;
        case 'd':
            aos_kv_del("lora");
            aos_kv_del("lora_dev");
            break;
        default:
            PRINTF("press ? for help\r\n");
            break;
    }

exit:
    rx_cmd_index = 0;
    if (ret != true) {
        LOG_PRINTF("error\r\n");
    }
}

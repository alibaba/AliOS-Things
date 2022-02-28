/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>

//#include <aos/aos.h>
#include <aos/kernel.h>
#include <port/mesh_hal_ble.h>
#include "mesh.h"
#include "common/log.h"
#include "flash.h"
#include "genie_service.h"
#include "genie_sal_uart.h"

#define at_debug printf

static const at_cmd_t genie_at_commands[] = {
    //AT+CMD      AT+CMD=?        AT+CMD?
    {BLE_MESH_ADV, genie_at_cmd_adv_start, "AT+" BLE_MESH_ADV "=<mode>", genie_at_cmd_adv_query},
    {BLE_MESH_RESET, genie_at_cmd_mesh_reset, "AT+" BLE_MESH_RESET, NULL},
    {BLE_MESH_REBOOT, genie_at_cmd_reboot, "AT+" BLE_MESH_REBOOT, NULL},
    {BLE_MESH_INFO, NULL, "AT+" BLE_MESH_INFO, genie_at_cmd_mesh_info},
    {BLE_MESH_VER, NULL, "AT+" BLE_MESH_VER, genie_at_cmd_mesh_ver},
    {BLE_MESH_MAC, NULL, "AT+" BLE_MESH_MAC, genie_at_cmd_mesh_mac},
    {BLE_MESH_FACTY_TEST, genie_at_cmd_facty_test, "AT+" BLE_MESH_FACTY_TEST, NULL},
    {BLE_MESH_LOG, genie_at_cmd_mesh_log, "AT+" BLE_MESH_LOG "=<mode>", genie_at_cmd_mesh_log_query},
    {BLE_MESH_TEST, genie_at_cmd_mesh_test, "AT+" BLE_MESH_TEST "=<opcode>,[param]", NULL},
    {BLE_MESH_GROUP, genie_at_cmd_mesh_group, "AT+" BLE_MESH_GROUP "=<addr1>,[addr2]...", genie_at_cmd_mesh_group_query},
    {BLE_MESH_MSG_TX, genie_at_cmd_mesh_trans_send, "AT+" BLE_MESH_MSG_TX "=<data>", NULL},
    {BLE_MESH_EVENT, NULL, "+" BLE_MESH_EVENT ":<state>", NULL},
    {BLE_MESH_MSG_RX, NULL, "+" BLE_MESH_MSG_RX ":<len>,<data>", NULL},
    {NULL, NULL, NULL, NULL},
};

static u8_t g_current_adv_mode = MESH_ADV_START;
static uint8_t test_dev_mac[6] = {0};

static char *str_chr(char *d, char *s, int c)
{
    int i = 0;
    char *q = d;
    char *p = s;

    for (i = 0; *(s + i) != (char)c; i++)
    {
        *(q++) = *(p++);

        if (*(s + i) == '\0')
        {
            return NULL;
        }
    }

    *(q++) = '\0';
    return d;
}

static char *char_cut(char *d, char *s, int b, int e)
{
    char *stc;

    if (b == '\0')
    {
        return NULL;
    }

    stc = strchr(s, b);

    if (stc == NULL)
    {
        at_debug("not execute\r\n");
        return NULL;
    }

    stc++;
    str_chr(d, stc, e);

    return d; //(char *)d;
}

static int char2_hex(const char *c, uint8_t *x)
{
    if (*c >= '0' && *c <= '9')
    {
        *x = *c - '0';
    }
    else if (*c >= 'a' && *c <= 'f')
    {
        *x = *c - 'a' + 10;
    }
    else if (*c >= 'A' && *c <= 'F')
    {
        *x = *c - 'A' + 10;
    }
    else
    {
        return -EINVAL;
    }

    return 0;
}

static int str2_char(const char *str, uint8_t *addr)
{
    int i, j;
    uint8_t tmp;

    if (strlen(str) != 17)
    {
        return -EINVAL;
    }

    for (i = 0, j = 1; *str != '\0'; str++, j++)
    {
        if (!(j % 3) && (*str != ':'))
        {
            return -EINVAL;
        }
        else if (*str == ':')
        {
            i++;
            continue;
        }

        addr[i] = addr[i] << 4;

        if (char2_hex(str, &tmp) < 0)
        {
            return -EINVAL;
        }

        addr[i] |= tmp;
    }

    return 0;
}

static int argv_type(char *s)
{
    int flag = 0;

    flag |= AT_CMD_NO_ADD;

    for (int i = 0; *(s + i) != '\0'; i++)
    {
        if (*(s + i) == '+')
        {
            flag = flag & (~0x1);
            flag |= AT_CMD_ADD; //+ no =
        }

        if (*(s + i) == '=')
        { //, or not ,
            if (*(s + i + 1) == '?')
            {
                return AT_CMD_FIND;
            }
            else if (*(s + i + 1) != '\0')
            {
                return AT_CMD_DOU;
            }
            else
            {
                return 0;
            }
        }

        if (*(s + i) == '?')
        {
            flag = flag & (~0x2); //no +
            flag |= AT_CMD_QUERY;
        }
    }

    return flag;
}

static int argc_len(char *s)
{
    int i = 0;
    int j = 0;

    for (i = 0; *(s + i) != '\0'; i++)
    {
        if ((*(s + i) == ',') || (*(s + i) == '?') || (*(s + i) == '='))
        {
            j++;
        }
    }

    return j;
}

static void cmd_cli_func(int type, int argc, char **argv)
{
    int i = 0;
    int err;

    if (argc < 2)
    {
        genie_sal_uart_send_str("AT support commands\r\n");
        for (i = 0; genie_at_commands[i].cmd_name != NULL; i++)
        {
            genie_sal_uart_send_str("%s %s\r\n", genie_at_commands[i].cmd_name, genie_at_commands[i].help);
        }
        return;
    }

    for (i = 0; genie_at_commands[i].cmd_name != NULL; i++)
    {
        if (strlen(genie_at_commands[i].cmd_name) == strlen(argv[1]) &&
            !strncmp(genie_at_commands[i].cmd_name, argv[1], strlen(genie_at_commands[i].cmd_name)))
        {
            if (type == AT_OP_EXECUTE)
            {
                if (genie_at_commands[i].cb)
                {
                    err = genie_at_commands[i].cb(argc - 1, &argv[1]);
                    if (err == 0)
                    {
                        genie_sal_uart_send_str(AT_OK_STR);
                    }
                    else
                    {
                        genie_sal_uart_send_str(AT_CME_ERROR_STR, err);
                    }
                }
                else
                {
                    genie_sal_uart_send_str(AT_CME_ERROR_STR, AT_ERR_NOT_FOUND);
                }
            }
            else if (type == AT_OP_FCURRENT)
            {
                if (genie_at_commands[i].fcb)
                {
                    err = genie_at_commands[i].fcb(argc - 1, &argv[1]);
                    if (err == 0)
                    {
                        genie_sal_uart_send_str(AT_OK_STR);
                    }
                    else
                    {
                        genie_sal_uart_send_str(AT_CME_ERROR_STR, err);
                    }
                }
                else
                {
                    genie_sal_uart_send_str(AT_CME_ERROR_STR, AT_ERR_NOT_FOUND);
                }
            }
            else
            { //HLOOKUP
                if (genie_at_commands[i].help)
                {
                    genie_sal_uart_send_str("%s\n", genie_at_commands[i].help);
                    genie_sal_uart_send_str(AT_OK_STR);
                }
            }

            break;
        }
    }

    if ((i + 1) == ARRAY_SIZE(genie_at_commands))
    {
        genie_sal_uart_send_str(AT_CME_ERROR_STR, AT_ERR_NOT_FOUND);
    }
}

static int is_at_cmd(char *data)
{
    if (strlen(data) < 2)
    {
        return 1;
    }

    if (((*data == 'A') || (*data == 'a')) && ((*(data + 1) == 'T') || (*(data + 1) == 't')))
    {
        return 0;
    }

    return 1;
}

int genie_at_cmd_parser(char data[])
{
    char argv[AT_MAX_ARGC][AT_MAX_ARGV_LEN];
    char *hcc;
    char *argqv[AT_MAX_ARGC];

    if (is_at_cmd(data))
    {
        return -1;
    }

    memset(argv, 0, sizeof(argv));
    int ustype = argv_type(data);
    argv[0][0] = 'A';
    argv[0][1] = 'T';
    argv[0][2] = '\0';

    //at_debug("ustype: %d, data: %s\n", ustype, data);
    switch (ustype)
    {
    case AT_CMD_NO_ADD:
        cmd_cli_func(AT_OP_HELP, 1, NULL);
        genie_sal_uart_send_str(AT_OK_STR);
        break;

    case AT_CMD_ADD: //execute
        if (char_cut(argv[1], data, '+', '\0') == NULL)
        {
            return -1;
        }

        argqv[0] = (char *)(&argv[0]);
        argqv[1] = (char *)(&argv[1]);
        cmd_cli_func(AT_OP_EXECUTE, 2, (char **)(&argqv));
        break;

    case AT_CMD_QUERY: //?
        if (char_cut((char *)argv[1], data, '+', '?') == NULL)
        {
            return -1;
        }

        argqv[0] = (char *)(&argv[0]);
        argqv[1] = (char *)(&argv[1]);
        cmd_cli_func(AT_OP_FCURRENT, 2, (char **)(&argqv));
        break;

    case AT_CMD_DOU:
        if (char_cut((char *)argv[1], data, '+', '=') == NULL)
        {
            return -1;
        }

        hcc = strchr(data, '=');
        int num = argc_len(hcc);
        //at_debug("num:%d, len:%d\n", num, strlen(hcc));
        if ((num > AT_MAX_ARGC - 2) || (strlen(hcc) > (AT_MAX_ARGC * AT_MAX_ARGV_LEN)))
        {
            at_debug("argv out of the max len\r\n");
            return -1;
        }

        if (num == 0)
        {
            at_debug("num:err 0\r\n");
            return -1;
        }

        if (num == 1)
        {
            char_cut((char *)argv[2], hcc, '=', '\0');
            argqv[2] = (char *)(&argv[2]);
        }
        else
        {
            char_cut((char *)argv[2], hcc, '=', ',');
            argqv[2] = (char *)(&argv[2]);

            for (int i = 1; i < num; i++)
            {
                hcc = strchr(hcc, ',');
                char_cut((char *)argv[2 + i], hcc, ',', ',');
                argqv[2 + i] = (char *)(&argv[2 + i]);
                hcc++;
            }
        }

        argqv[0] = (char *)(&argv[0]);
        argqv[1] = (char *)(&argv[1]);
        cmd_cli_func(AT_OP_EXECUTE, num + 2, (char **)(&argqv));
        break;

    case AT_CMD_FIND: //=?
        hcc = strrchr(data, '+');
        hcc++;
        str_chr(argv[1], hcc, '=');
        argqv[0] = (char *)(&argv[0]);
        argqv[1] = (char *)(&argv[1]);
        cmd_cli_func(AT_OP_HLOOKUP, 2, (char **)(&argqv));
        break;

    default:
        at_debug("unknow type:%d\r\n", ustype);
        return -1;
    }

    return 0;
}

int genie_at_cmd_reboot(int argc, char *argv[])
{
    /* AT+REBOOT */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    genie_sal_uart_send_str("OK\r\n");
    aos_msleep(30);
    aos_reboot();
    return 0;
}

int genie_at_cmd_mesh_reset(int argc, char *argv[])
{
    /* AT+MESHRST */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    genie_sal_uart_send_str("OK\r\n");
    genie_event(GENIE_EVT_HW_RESET_START, NULL);
    return 0;
}

int genie_at_cmd_adv_start(int argc, char *argv[])
{
    /* AT+MESHADV=<mode> */
    uint8_t mode = 0;

    if (argc != 2)
    {
        return AT_ERR_FORMAT;
    }

    mode = strtoul(argv[1], NULL, 0);
    if (mode == MESH_ADV_CLOSE)
    {
        /* close adv */
        bt_mesh_adv_stop();
        bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    }
    else if (mode == MESH_ADV_START)
    {
        /* start adv */
        bt_mesh_adv_stop();
        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
        genie_event(GENIE_EVT_SDK_MESH_PBADV_START, NULL);
    }
    else if (mode == MESH_ADV_SILENT)
    {
        /* start silent adv */
        genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
    }
    else
    {
        return AT_ERR_FORMAT;
    }

    g_current_adv_mode = mode;
    return 0;
}

int genie_at_cmd_adv_query(int argc, char *argv[])
{
    /* AT+MESHADV? */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    genie_sal_uart_send_str(AT_RESPONSE_STR "%d\r\n", BLE_MESH_ADV, g_current_adv_mode);
    return 0;
}

int genie_at_cmd_mesh_log(int argc, char *argv[])
{
    /* AT+MESHLOG=<mode> */
    uint8_t mode = 0;

    if (argc != 2)
    {
        return AT_ERR_FORMAT;
    }

    mode = strtoul(argv[1], NULL, 0);
    if ((mode == 1) || (mode == 0))
    {
        g_mesh_log_mode = mode;
        if (!g_mesh_log_mode)
        {
            genie_sal_uart_send_str("OK\r\n");
            aos_set_log_level(LOG_EMERG);
        }
        else
        {
            aos_set_log_level(GENIE_MESH_DEFAULT_LOG_LEVEL);
        }
        return 0;
    }
    else
    {
        return AT_ERR_FORMAT;
    }
}

int genie_at_cmd_mesh_log_query(int argc, char *argv[])
{
    /* AT+MESHLOG? */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    genie_sal_uart_send_str("+%s:%d\r\n", BLE_MESH_LOG, g_mesh_log_mode);
    return 0;
}

int genie_at_cmd_mesh_info(int argc, char *argv[])
{
    bool status;

    /* AT+MESHINF? */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    status = bt_mesh_is_provisioned();
    genie_sal_uart_send_str(AT_RESPONSE_STR "%d\r\n", BLE_MESH_INFO, status);
    return 0;
}

int genie_at_cmd_mesh_ver(int argc, char *argv[])
{
    /* AT+MESHVER? */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    genie_sal_uart_send_str(AT_RESPONSE_STR "%08x\r\n", BLE_MESH_VER, genie_version_appver_get());
    return 0;
}

int genie_at_cmd_mesh_mac(int argc, char *argv[])
{
    uint8_t addr[6] = {0};

    /* AT+MESHMAC? */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    hal_flash_read_mac_params(addr, sizeof(addr));
    genie_sal_uart_send_str(AT_RESPONSE_STR " %02x:%02x:%02x:%02x:%02x:%02x\r\n", BLE_MESH_MAC, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    return 0;
}

int genie_at_cmd_facty_test(int argc, char *argv[])
{
    int ret = -1;

    /* AT+FACTYTEST */
    if (argc > 1)
    {
        return AT_ERR_FORMAT;
    }

    ret = genie_sal_ble_set_factory_flag();
    if (ret != 0)
    {
        return AT_ERR_EXE;
    }

    genie_sal_uart_send_str("OK\r\n");
    aos_reboot();
    return 0;
}

static int8_t get_rssi_result = 0;
static void get_ble_rssi_cb(uint8_t mac[6], int16_t rssi)
{
    if (memcmp(test_dev_mac, mac, sizeof(test_dev_mac)) == 0)
    {
        get_rssi_result = 0;
        memset(test_dev_mac, 0, sizeof(test_dev_mac));
        genie_sal_uart_send_str(AT_RESPONSE_STR "%d,%d\r\n", BLE_MESH_TEST, MESH_TEST_RSSI, rssi);
    }
    else
    {
        get_rssi_result = -1;
        at_debug("cann't find mac: %s rssi\n", bt_hex(test_dev_mac, sizeof(test_dev_mac)));
    }
}

int genie_at_cmd_mesh_test(int argc, char *argv[])
{
    uint32_t opcode;
    int err;

    /* AT+MESHTEST=<opcode>,[param] */
    if (argc < 2)
    {
        return AT_ERR_FORMAT;
    }

    opcode = strtoul(argv[1], NULL, 0);
    switch (opcode)
    {
    case MESH_TEST_RSSI:
        if (argc < 3) {
            return AT_ERR_FORMAT;
        }
        err = str2_char(argv[2], test_dev_mac);
        //at_debug("test_dev_mac: %s\n", bt_hex(test_dev_mac, sizeof(test_dev_mac)));
        if (err != 0)
        {
            at_debug("invalid address\n");
            return AT_ERR_FORMAT;
        }

        /* start scan,find the dev and get rssi */
        err = genie_sal_ble_get_rssi(test_dev_mac, (genie_sal_ble_get_rssi_cb)get_ble_rssi_cb, 5000);
        if (err != 0)
        {
            at_debug("start scan error\n");
            return AT_ERR_EXE;
        }
        break;

    default:
        return AT_ERR_FORMAT;
    }

    return get_rssi_result;
}

int genie_at_cmd_mesh_group(int argc, char *argv[])
{
    u16_t group_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT] = {0};
    int group_num = argc - 1;

    /* AT+MESHGRP=<addr1>,[addr2]... */
    if (argc > (CONFIG_BT_MESH_MODEL_GROUP_COUNT + 1) || argc < 2)
    {
        return AT_ERR_FORMAT;
    }

    for (int i = 0; i < group_num; i++)
    {
        group_list[i] = strtoul(argv[i + 1], NULL, 0);
        if (group_list[i] < GENIE_ADDR_MIN || group_list[i] > GENIE_ADDR_MAX)
        {
            return AT_ERR_FORMAT;
        }
    }

    /* set group addr */
    if (genie_storage_write_sub(group_list) != GENIE_STORAGE_SUCCESS)
    {
        return AT_ERR_EXE;
    }

    /* get group addr */
    if (genie_storage_read_sub(g_sub_list) != GENIE_STORAGE_SUCCESS)
    {
        return AT_ERR_EXE;
    }

    return 0;
}

int genie_at_cmd_mesh_group_query(int argc, char *argv[])
{
    u16_t group_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT] = {0};
    char result[100] = {0};
    int len = 0;

    /* AT+MESHGRP? */
    if (argc > 2)
    {
        return AT_ERR_FORMAT;
    }

    /* get group addr */
    if (genie_storage_read_sub(group_list) != GENIE_STORAGE_SUCCESS)
    {
        return AT_ERR_EXE;
    }

    len = snprintf(result, sizeof(result), AT_RESPONSE_STR, BLE_MESH_GROUP);
    for (int i = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT; i++)
    {
        if (len < (sizeof(result) - 5) && group_list[i] != 0x0000)
        {
            len += snprintf(result + len, sizeof(result) - len, "0x%04X,", group_list[i]);
        }
    }

    result[len - 1] = '\0';
    genie_sal_uart_send_str("%s\r\n", result);

    return 0;
}

static int _send_trans_msg(char *data)
{
    uint8_t count = 0;
    uint8_t msg_b[GENIE_HAL_BLE_SEND_MAX_DATA_LEN];
    uint8_t ret = 0;

    count = strlen(data) >> 1;
    if (count > GENIE_HAL_BLE_SEND_MAX_DATA_LEN)
    {
        at_debug("data out of the max len\n");
        return AT_ERR_FORMAT;
    }
    memset(msg_b, 0x0, sizeof(msg_b));
    ret = stringtohex(data, msg_b, count);
    if (ret == 0)
    {
        at_debug("stringtohex error\n");
        return AT_ERR_FORMAT;
    }

    if (genie_sal_ble_send_msg(0, msg_b, count))
    {
        return AT_ERR_EXE;
    }

    return 0;
}

int genie_at_cmd_mesh_trans_send(int argc, char *argv[])
{
    /* AT+MESHMSGTX=<data> */
    if (argc < 2)
    {
        return AT_ERR_FORMAT;
    }

    return _send_trans_msg(argv[1]);
}

int genie_at_cmd_send_data_to_mcu(uint8_t *p_data, uint16_t data_len)
{
    int i = 0;
    int len = 0;
    char *p_send_buff = NULL;
    uint16_t send_buff_len = 0;

    send_buff_len = strlen(BLE_MESH_MSG_RX) + 2 * data_len + 10;

    p_send_buff = (char *)aos_malloc(send_buff_len);
    if (NULL == p_send_buff)
    {
        return -1;
    }
    memset(p_send_buff, '\0', send_buff_len);

    len = snprintf(p_send_buff, send_buff_len, "\r\n" AT_RESPONSE_STR "%d,", BLE_MESH_MSG_RX, data_len);
    for (i = 0; i < data_len; i++)
    {
        len += snprintf(p_send_buff + len, send_buff_len - len, "%02X", p_data[i]);
    }

    genie_sal_uart_send_str("%s\r\n", p_send_buff);
    aos_free(p_send_buff);

    return 0;
}

int genie_at_output_event(genie_event_e event, void *p_arg)
{
    char result[100] = {0};
    int len = 0;

    /* +MESHEVT:<state> */
    len = snprintf(result, sizeof(result), AT_RESPONSE_STR, BLE_MESH_EVENT);

    if (event == GENIE_EVT_MESH_READY)
    {
        len += snprintf(result + len, sizeof(result) - len, "0x%02X", MESH_EVENT_DEV_READY);
    }
    else if (event == GENIE_EVT_SDK_MESH_PROV_SUCCESS)
    {
        len += snprintf(result + len, sizeof(result) - len, "0x%02X", MESH_EVENT_PROV_SUCCESS);
        g_current_adv_mode = MESH_ADV_CLOSE;
    }
    else if (event == GENIE_EVT_SDK_MESH_PROV_FAIL)
    {
        len += snprintf(result + len, sizeof(result) - len, "0x%02X", MESH_EVENT_PROV_FAILED);
    }
    else
    {
        if (event == GENIE_EVT_SDK_MESH_SILENT_START)
        {
            g_current_adv_mode = MESH_ADV_SILENT;
        }
        return 0;
    }

    genie_sal_uart_send_str("%s\r\n", result);

    return 0;
}

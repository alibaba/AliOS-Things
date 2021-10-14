
#include <stdint.h>

#include "genie_service.h"
#include "genie_sal_ble.h"
#include "genie_sal_uart.h"

static uint8_t g_rssi_mac[GENIE_BLE_MAC_LEN];

static void genie_bin_cmds_send(uint16_t opcode, uint8_t *p_data, uint16_t len)
{
    uint8_t checksum = 0;

    genie_sal_uart_send_one_byte(opcode >> 8);
    genie_sal_uart_send_one_byte(opcode & 0xff);
    genie_sal_uart_send_one_byte(len >> 8);
    genie_sal_uart_send_one_byte(len & 0xff);

    checksum += len >> 8;
    checksum += (len & 0xff);

    while (len--)
    {
        checksum += *p_data;
        genie_sal_uart_send_one_byte(*p_data++);
    }

    genie_sal_uart_send_one_byte(checksum);
}

static void genie_bin_cmds_error(uint8_t err_code)
{
    uint8_t code = err_code;

    genie_bin_cmds_send(GENIE_BIN_OPCODE_ERR, &code, 1);
}

static uint16_t mesh_adv_ctl(uint8_t ctl, uint8_t *resp)
{
    resp[0] = 0x01;
    resp[1] = ctl;

    switch (ctl)
    {
    case 0x00:
    {
        bt_mesh_adv_stop();
        bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    }
    break;
    case 0x01:
    {
        bt_mesh_adv_stop();
        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
        genie_event(GENIE_EVT_SDK_MESH_PBADV_START, NULL);
    }
    break;
    case 0x02:
    {
        genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
    }
    break;
    default:
        break;
    }
    return 2;
}

static uint16_t reset_prov_info(uint8_t *resp)
{
    resp[0] = 0x03;
    resp[1] = 0x01;
    resp[2] = 0x01;

    genie_event(GENIE_EVT_HW_RESET_START, NULL);

    return 3;
}

static void _send_rssi_msg(int8_t rssi)
{
    uint8_t resp[9];

    resp[0] = 0x03;
    resp[1] = 0x02;
    //get mac
    memcpy(resp + 2, g_rssi_mac, GENIE_BLE_MAC_LEN);
    //get rssi
    resp[8] = rssi;

    genie_bin_cmds_send(GENIE_BIN_OPCODE_CMD, resp, 9);
}

static uint16_t get_device_info(uint8_t type, uint8_t *resp)
{
    switch (type)
    {
    case 0x00:
    { //Get provision info
        resp[0] = 0x03;
        resp[1] = 0x03;
        if (bt_mesh_is_provisioned())
        {
            resp[2] = 0x01;
        }
        else
        {
            resp[2] = 0x00;
        }
        return 3;
    }
    case 0x01:
    { //Get device version
        uint32_t app_version = genie_version_appver_get();

        resp[0] = GENIE_CTL_DEVIE_INFO_AND_EVENT;
        resp[1] = 0x06;
        resp[2] = (uint8_t)(app_version >> 24);
        resp[3] = (uint8_t)(app_version >> 16);
        resp[4] = (uint8_t)(app_version >> 8);
        resp[5] = (uint8_t)(app_version);

        return 6;
    }
    default:
        break;
    }
    return 0;
}

static void genie_bin_cmd_type_data(uint8_t *p_data)
{
    uint16_t len = p_data[3] + (p_data[2] << 8);

    if (genie_sal_ble_send_msg(0, &p_data[4], len))
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_TRANSPARENT_ERR);
    }
}

static void get_ble_rssi_cb(uint8_t mac[6], int8_t rssi)
{
    if (memcmp(g_rssi_mac, mac, GENIE_BLE_MAC_LEN) == 0)
    {
        _send_rssi_msg(rssi);
        memset(g_rssi_mac, 0, GENIE_BLE_MAC_LEN);
    }
    else
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_GET_RSSI_MAC_NOT_MATCH_ERR);
    }
}

static int genie_bin_cmd_update_group_addr(uint8_t *addr_list, int addr_len)
{
    int index = 0;
    uint8_t group_addr_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2] = {0};

    if ((addr_len > CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2) || ((addr_len % 2) != 0))
    {
        return -1;
    }

    memset(group_addr_list, 0, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2);
    memcpy(group_addr_list, addr_list, addr_len);

    for (index = 0; index < addr_len; index++)
    {
        if ((index % 2) == 0)
        {
            if ((group_addr_list[index + 1] & 0xC0) != 0xC0) //invalid group address
            {
                return -2;
            }
        }
    }

    /* set group addr */
    if (genie_storage_write_sub((uint16_t *)group_addr_list) != GENIE_STORAGE_SUCCESS)
    {
        return -3;
    }

    /* get group addr */
    if (genie_storage_read_sub((uint16_t *)g_sub_list) != GENIE_STORAGE_SUCCESS)
    {
        return -3;
    }

    return 0;
}

static int get_mesh_group_addr(uint8_t *response)
{
    int i = 0;
    int j = 0;
    uint8_t group_addr_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2] = {0};

    /* get group addr */
    if (genie_storage_read_sub((uint16_t *)group_addr_list) != GENIE_STORAGE_SUCCESS)
    {
        return -1;
    }

    for (i = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2; i++)
    {
        if (i % 2 == 0)
        {
            if ((group_addr_list[i + 1] & 0xC0) == 0xC0) //valid group address
            {
                response[j] = group_addr_list[i];
                response[j + 1] = group_addr_list[i + 1];
                j += 2;
            }
        }
    }

    return j;
}

static void genie_bin_cmd_type_command(uint8_t *p_data)
{
    uint8_t *p_u8 = p_data;
    uint16_t len = p_u8[3] + (p_u8[2] << 8);

    uint8_t resp[20];
    uint16_t resp_len = 0;

    switch (p_u8[4])
    {
    case GENIE_CTL_MESH_ADV:
        if (len == 2)
        {
            resp_len = mesh_adv_ctl(p_u8[5], resp);
        }
        break;

    case GENIE_CTL_CLEAR_PROV_INFO:
        if (len == 1)
        {
            resp_len = reset_prov_info(resp);
        }
        break;

    case GENIE_CTL_TEST_MODE:
        if (len == 8)
        {
            if (p_u8[5] == 0x00)
            {
                memcpy(g_rssi_mac, &p_u8[6], GENIE_BLE_MAC_LEN); //backup mac addr
                if (genie_sal_ble_get_rssi(g_rssi_mac, (genie_sal_ble_get_rssi_cb)get_ble_rssi_cb, 5000) < 0)
                {
                    return genie_bin_cmds_error(GENIE_BINARY_CMD_GET_RSSI_BLE_ERR);
                }
            }
            else
            {
                return genie_bin_cmds_error(GENIE_BINARY_CMD_GET_RSSI_INPUT_ERR);
            }
        }
        else
        {
            return genie_bin_cmds_error(GENIE_BINARY_CMD_GET_RSSI_INPUT_ERR);
        }
        break;
    case GENIE_CTL_DEVICE_INFO:
        if (len == 2)
        {
            resp_len = get_device_info(p_u8[5], resp);
        }
        break;
    case GENIE_CTL_REBOOT_DEVICE:
        if (len == 1)
        {
            aos_reboot();
        }
        break;
    case GENIE_CTL_UPDATE_GROUP_ADDR:
    {
        int ret = 0;

        resp[0] = GENIE_CTL_UPDATE_GROUP_ADDR_RESPONSE;
        ret = genie_bin_cmd_update_group_addr(&p_u8[5], len - 1);
        if (0 == ret)
        {
            resp[1] = 0x00; //successful
            resp_len = 2;
        }
        else if (-1 == ret)
        {
            return genie_bin_cmds_error(GENIE_BINARY_CMD_UPDATE_GROUP_ADDR_LEN_ERR);
        }
        else if (-2 == ret)
        {
            return genie_bin_cmds_error(GENIE_BINARY_CMD_UPDATE_GROUP_ADDR_INVALID_ERR);
        }
        else if (-3 == ret)
        {
            return genie_bin_cmds_error(GENIE_BINARY_CMD_UPDATE_GROUP_ADDR_FLASH_ERR);
        }
    }
    break;
    case GENIE_CTL_GET_GROUP_ADDR:
    {
        if (len == 1)
        {
            resp[0] = GENIE_CTL_GET_GOURP_ADDR_RESPONSE;
            resp_len = get_mesh_group_addr(&resp[2]);
            if (resp_len > 0) //Have valid group addr
            {
                resp[1] = 0x00;
                resp_len += 2;
            }
            else //Have no valid group addr
            {
                resp[1] = 0x01;
                resp_len = 2;
            }
        }
    }
    break;
    case GENIE_CTL_FACTORY_TEST:
    {
        if (genie_sal_ble_set_factory_flag() != 0)
        {
            return genie_bin_cmds_error(GENIE_BINARY_CMD_START_FACTORY_ERR);
        }
        else
        {
            resp[0] = GENIE_CTL_FACTORY_TEST;
            resp[1] = 0x00;
            resp_len = 2;
            genie_bin_cmds_send(GENIE_BIN_OPCODE_CMD, resp, resp_len);
            aos_msleep(1000);
            aos_reboot();
        }
    }
    break;
    case GENIE_CTL_SWITCH_DEBUG:
    {
        if (len == 2)
        {
            if (p_u8[5] > 0)
            {
                g_mesh_log_mode = 1;
                aos_set_log_level(GENIE_MESH_DEFAULT_LOG_LEVEL);
            }
            else
            {
                g_mesh_log_mode = 0;
                aos_set_log_level(LOG_EMERG);
            }
            resp[0] = GENIE_CTL_SWITCH_DEBUG;
            resp[1] = 0x00;
            resp_len = 2;
        }
        else
        {
            resp[0] = GENIE_CTL_SWITCH_DEBUG;
            resp[1] = 0x01;
            resp_len = 2;
        }
    }
    break;
    default:
        break;
    }

    //if resp_len = 0 do nothing
    if (resp_len > 0) //need response
    {
        genie_bin_cmds_send(GENIE_BIN_OPCODE_CMD, resp, resp_len);
    }
    else if (resp_len < 0)
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_UNKNOW_ERR);
    }
}

int genie_bin_cmds_handle(uint8_t *p_data, uint8_t data_len)
{
    uint16_t len = 0;
    uint16_t index = 0;
    uint8_t checksum = 0;

    if ((data_len > 0) && (p_data[0] != 0xFF) && (p_data[0] != 0xFE))
    {
        return 0; //Don't send any data by UART
    }

    if (data_len < GENIE_BIN_MIN_DATA_LEN)
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_TOTAL_LEN_ERR);
        return -1;
    }

    len = p_data[3] + (p_data[2] << 8);
    if (len != (data_len - 5))
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_DATA_LEN_ERR);
        return -1;
    }

    checksum += p_data[2];
    checksum += p_data[3];

    while (index < len)
    {
        checksum += p_data[4 + index];
        index++;
    }

    if (checksum != p_data[data_len - 1])
    {
        genie_bin_cmds_error(GENIE_BINARY_CMD_CRC_ERR);
        return -1;
    }

    if (((p_data[0] << 8) | p_data[1]) == GENIE_BIN_OPCODE_DATA)
    {
        genie_bin_cmd_type_data(p_data);
    }
    else if (((p_data[0] << 8) | p_data[1]) == GENIE_BIN_OPCODE_CMD)
    {
        genie_bin_cmd_type_command(p_data);
    }

    return 0;
}

int genie_bin_cmd_handle_event(genie_event_e event, void *p_arg)
{
    uint8_t report_data[2];
    uint8_t report_data_len = 0;

    if (event == GENIE_EVT_MESH_READY)
    {
        report_data[0] = 0x03;
        report_data[1] = 0x04;
        report_data_len = 2;
    }
    else if (event == GENIE_EVT_SDK_MESH_PROV_SUCCESS)
    {
        report_data[0] = 0x03;
        report_data[1] = 0x00;
        report_data_len = 2;
    }
    else if (event == GENIE_EVT_SDK_MESH_PROV_FAIL)
    {
        report_data[0] = 0x03;
        report_data[1] = 0x05;
        report_data_len = 2;
    }

    if (report_data_len > 0)
    {
        genie_bin_cmds_send(GENIE_BIN_OPCODE_CMD, report_data, report_data_len);
    }

    return 0;
}

void genie_bin_cmds_send_data_to_mcu(uint8_t *p_data, uint8_t len)
{
    genie_bin_cmds_send(GENIE_BIN_OPCODE_DATA, p_data, len);
}

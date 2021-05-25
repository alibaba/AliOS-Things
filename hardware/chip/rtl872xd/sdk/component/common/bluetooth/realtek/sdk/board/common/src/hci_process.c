/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os_mem.h"
#include "os_sync.h"
#include "os_sched.h"

#include "hci_process.h"
#include "hci_tp.h"
#include "bt_board.h"
#include "trace_app.h"
#include "bt_types.h"
#include "hci_board.h"
#include "hci_uart.h"

extern void hci_normal_start(void);
extern bool bt_check_iqk(void);
extern bool hci_start_iqk(void);
extern uint8_t  hci_tp_lgc_efuse[0x20];
extern uint8_t  hci_tp_phy_efuse[19];

static uint8_t vendor_flow;
static uint8_t iqk_type = 0xff;
static uint8_t orignal_thermal[3];

//====================hci_patch_util.c================

uint8_t hci_tp_read_local_ver(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    HCI_PRINT_TRACE0("hci_tp_read_local_ver");

    hci_normal_start();
    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 4);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_READ_LOCAL_VERSION_INFO);
        LE_UINT8_TO_STREAM(p, 0); /* length */

        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
        return HCI_TP_CHECK_OK;
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
}

uint8_t hci_read_local_version_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    uint16_t    lmp_subver;
    /* Skip status, hci version, hci revision, lmp version & manufacture name */
    STREAM_SKIP_LEN(p_buf, 6);
    LE_STREAM_TO_UINT16(lmp_subver, p_buf);
    HCI_PRINT_INFO1("hci_tp_config: lmp_subver 0x%04x", lmp_subver);
    if (lmp_subver != BT_DEFAUT_LMP_SUBVER)
    {
        HCI_PRINT_ERROR0("hci_tp_config: Patch already exists");
        hci_board_debug("%s: Patch already exists\n",__FUNCTION__);
        return HCI_TP_CONFIG_END;
    }
    else
    {
        //hci_board_debug("hci_tp_config: lmp_subver 0x%04x\n", lmp_subver);
        return HCI_TP_CHECK_OK;
    }
}
uint8_t hci_tp_read_rom_ver(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    HCI_PRINT_TRACE0("hci_tp_read_rom_ver");

    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 4);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_VSC_READ_ROM_VERSION);
        LE_UINT8_TO_STREAM(p, 0); /* length */

        hci_adapter_send(p_cmd, p - p_cmd);
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}

extern bool hci_rtk_find_patch(uint8_t bt_hci_chip_id);
uint8_t hci_read_rom_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    bool ret = false;
    uint8_t    rom_version;
    uint8_t    bt_hci_chip_id;
    LE_STREAM_TO_UINT8(rom_version, p_buf);
    HCI_PRINT_INFO1("hci_tp_config: rom_version 0x%02x", rom_version);
    bt_hci_chip_id = rom_version + 1;
    hci_board_debug("%s: rom_version 0x%04x, bt_hci_chip_id 0x%04x\n", __FUNCTION__, rom_version, bt_hci_chip_id);

    ret = hci_rtk_find_patch(bt_hci_chip_id);
    if(ret == false)
    {
        hci_board_debug("\r\n%s: error operate\r\n",__FUNCTION__);
        return HCI_TP_CHECK_ERROR;
    }

    return HCI_TP_CHECK_OK;
}

uint8_t hci_tp_read_thermal(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    HCI_PRINT_TRACE0("hci_tp_read_thermal");

    if (rltk_wlan_is_mp())
    {
        p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 4);
            if (p_cmd != NULL)
            {
                p = p_cmd;
                    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
                    LE_UINT16_TO_STREAM(p, HCI_VSC_READ_THERMAL);
                    LE_UINT8_TO_STREAM(p, 0); /* length */
                    
                    hci_adapter_send(p_cmd, p - p_cmd);
            }
            else
            {
                return HCI_TP_CONFIG_FAIL;
            }
        return HCI_TP_CHECK_OK;
    }
    else
    {
        return HCI_TP_NOT_SEND;
    }
}

static int freq_cmp(const void *a, const void *b)
{
	int _a = *(int *) a;
	int _b = *(int *) b;

	if (_a == 0)
		return 1;
	if (_b == 0)
		return -1;
	return _a - _b;
}

uint8_t hci_thermal_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    (void)p_buf;
    uint8_t    thermal;
    LE_STREAM_TO_UINT8(thermal, p_buf);

    if (vendor_flow > 2)   /* fortime */
    {
        if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
        {
            hci_board_debug("%s: thermal_check 0x%02x\n",__FUNCTION__,thermal);
        }
		qsort(orignal_thermal, 3, sizeof(uint8_t), freq_cmp);
        hci_board_debug("\n\rthermal_check OK orignal_thermal[1] = %x\n", orignal_thermal[1]);

		thermal = orignal_thermal[1]&0x3F;
		hci_board_debug("\n\rthermal_check OK thermal = %x\n", thermal);
		hci_tp_phy_efuse[7]=thermal;
        vendor_flow = 0;
        return HCI_TP_CHECK_OK;
    }
    else
    {
        if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
        {
            hci_board_debug("%s: thermal_check 0x%02x, time %x\n",__FUNCTION__,thermal, vendor_flow);
        }
        orignal_thermal[vendor_flow]=thermal;
        vendor_flow++;
        return HCI_TP_CHECK_AGAIN;
    }
}
//==========================patch==========================

typedef struct
{
    uint8_t            *fw_buf;
    uint8_t            *config_buf;
    uint8_t             patch_frag_cnt;
    uint8_t             patch_frag_idx;
    uint8_t             patch_frag_len;
    uint8_t             patch_frag_tail;
    uint16_t            fw_len;
    uint16_t            config_len;
    uint32_t            baudrate;
}T_HCI_PATCH;

T_HCI_PATCH hci_patch_info;

#define PATCH_FRAGMENT_MAX_SIZE         252
bool hci_set_patch(uint8_t *fw_addr,uint32_t fw_len, uint8_t *fw_config_addr, uint32_t
        fw_config_len, uint32_t baudrate)
{
    T_HCI_PATCH *p_hci_patch = &hci_patch_info;
    uint32_t svn_version, coex_version, lmp_subversion;

    p_hci_patch->fw_len = fw_len;
    p_hci_patch->fw_buf = fw_addr;
    p_hci_patch->config_buf = fw_config_addr;
    p_hci_patch->config_len = fw_config_len;
    p_hci_patch->patch_frag_idx = 0;
    p_hci_patch->baudrate = baudrate;
    //p_hci_patch->config_len = 0;
    LE_ARRAY_TO_UINT32(svn_version, (p_hci_patch->fw_buf+p_hci_patch->fw_len-8));
    LE_ARRAY_TO_UINT32(coex_version, (p_hci_patch->fw_buf+p_hci_patch->fw_len-12));
    LE_ARRAY_TO_UINT32(lmp_subversion, (p_hci_patch->fw_buf+p_hci_patch->fw_len-4));
    p_hci_patch->patch_frag_cnt  = (p_hci_patch->fw_len + p_hci_patch->config_len) / PATCH_FRAGMENT_MAX_SIZE;
    p_hci_patch->patch_frag_tail = (p_hci_patch->fw_len + p_hci_patch->config_len) % PATCH_FRAGMENT_MAX_SIZE;

    if (p_hci_patch->patch_frag_tail)
    {
        p_hci_patch->patch_frag_cnt += 1;
    }
    else
    {
        p_hci_patch->patch_frag_tail = PATCH_FRAGMENT_MAX_SIZE;
    }
    HCI_PRINT_INFO3("hci_rtk_load_patch:svn %d patch frag count %u, tail len %u",svn_version,
                    p_hci_patch->patch_frag_cnt, p_hci_patch->patch_frag_tail);


    HCI_PRINT_INFO3("BT patch:svn %d coex svn_version: %x LMP VERSION:%x\n",(int)svn_version, (unsigned int)coex_version,(unsigned int)lmp_subversion);
    if (rltk_wlan_is_mp())
    {
        hci_board_debug("BT patch:svn %d coex svn_version: %x LMP VERSION:%x\n",(int)svn_version, (unsigned int)coex_version,(unsigned int)lmp_subversion);
    }
    (void)coex_version;
    (void)lmp_subversion;
    return true;
}

uint8_t hci_tp_download_patch(void)
{
    uint8_t  *p_cmd;
    uint8_t  *p;
    uint8_t  *p_frag;
    uint16_t  sent_len;
    T_HCI_PATCH *p_hci_rtk = &hci_patch_info;

    if (p_hci_rtk->patch_frag_idx < p_hci_rtk->patch_frag_cnt)
    {
        if (p_hci_rtk->patch_frag_idx == p_hci_rtk->patch_frag_cnt - 1)
        {
            HCI_PRINT_TRACE0("hci_tp_download_patch: send last frag");
            //hci_board_debug("hci_tp_download_patch: send last frag\n");

            p_hci_rtk->patch_frag_idx |= 0x80;
            p_hci_rtk->patch_frag_len  = p_hci_rtk->patch_frag_tail;
        }
        else
        {
            p_hci_rtk->patch_frag_len = PATCH_FRAGMENT_MAX_SIZE;
        }
    }

    //hci_board_debug("hci_tp_download_patch: frag index %d, len %d\n",
     //         p_hci_rtk->patch_frag_idx, p_hci_rtk->patch_frag_len);
    HCI_PRINT_TRACE2("hci_tp_download_patch: frag index %d, len %d",
                     p_hci_rtk->patch_frag_idx, p_hci_rtk->patch_frag_len);

    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, HCI_CMD_HDR_LEN + 1 + p_hci_rtk->patch_frag_len);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_VSC_DOWNLOAD_PATCH);
        LE_UINT8_TO_STREAM(p, 1 + p_hci_rtk->patch_frag_len);  /* length */
        LE_UINT8_TO_STREAM(p, p_hci_rtk->patch_frag_idx);  /* frag index */

        sent_len = (p_hci_rtk->patch_frag_idx & 0x7F) * PATCH_FRAGMENT_MAX_SIZE;

        if (sent_len >= p_hci_rtk->fw_len)    /* config patch domain */
        {
            p_frag = p_hci_rtk->config_buf + sent_len -p_hci_rtk->fw_len;
            memcpy(p, p_frag, p_hci_rtk->patch_frag_len);
        }
        else if (sent_len + p_hci_rtk->patch_frag_len <= p_hci_rtk->fw_len) /* fw patch domain */
        {
            p_frag = p_hci_rtk->fw_buf + sent_len;
            memcpy(p, p_frag, p_hci_rtk->patch_frag_len);
        }
        else    /* accross fw and config patch domains */
        {
            p_frag = p_hci_rtk->fw_buf + sent_len;
            memcpy(p, p_frag, p_hci_rtk->fw_len - sent_len);
            p += p_hci_rtk->fw_len - sent_len;
            memcpy(p, p_hci_rtk->config_buf, p_hci_rtk->patch_frag_len + sent_len - p_hci_rtk->fw_len);
        }

        p_hci_rtk->patch_frag_idx++;
        hci_adapter_send(p_cmd, HCI_CMD_HDR_LEN + 1 + p_hci_rtk->patch_frag_len);
        //hci_tp_send(p_cmd, HCI_CMD_HDR_LEN + 1 + p_hci_rtk->patch_frag_len, hci_rtk_tx_cb);
    }
    else
    {
        hci_board_debug("%s:p_cmd is NULL \n", __FUNCTION__);
        return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}

uint8_t hci_download_patch_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    uint8_t index;
    LE_STREAM_TO_UINT8(index, p_buf);
    T_HCI_PATCH *p_hci_rtk = &hci_patch_info;
    if (index & 0x80)   /* receive the last fragment completed evt */
    {
        HCI_PRINT_INFO0("Download patch completely\n");
        //hci_board_debug("Download patch completely\n");
        os_mem_free(p_hci_rtk->fw_buf);
        os_mem_free(p_hci_rtk->config_buf);
        return HCI_TP_CHECK_OK;
    }
    else
    {
        return HCI_TP_CHECK_AGAIN;
    }
}

uint8_t hci_tp_set_controller_baudrate(void)
{
    uint8_t *p_cmd;
    uint8_t *p;
    T_HCI_PATCH *p_hci_rtk = &hci_patch_info;

    HCI_PRINT_TRACE1("hci_tp_set_controller_baudrate: baudrate 0x%08x", p_hci_rtk->baudrate);
    //hci_board_debug("hci_tp_set_controller_baudrate: baudrate 0x%08x\n", p_hci_rtk->baudrate);

    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, HCI_CMD_HDR_LEN + 4);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_VSC_UPDATE_BAUDRATE);
        LE_UINT8_TO_STREAM(p, 4);   /* length */
        LE_UINT32_TO_STREAM(p, p_hci_rtk->baudrate);
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
    }
    else
    {
        hci_board_debug("%s:p_cmd is NULL \n", __FUNCTION__);
        return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}

static void hci_rtk_convert_buadrate(uint32_t bt_baudrate, uint32_t *uart_baudrate)
{
    uint8_t i;

    *uart_baudrate = 115200;
extern const BAUDRATE_MAP baudrates[];
extern unsigned int baudrates_length;

    for (i = 0; i < baudrates_length; i++)
    {
        if (baudrates[i].bt_baudrate == bt_baudrate)
        {
            *uart_baudrate = baudrates[i].uart_baudrate;
            return;
        }
    }

    HCI_PRINT_TRACE0("hci_rtk_convert_buadrate: use default baudrate[115200]");
    return;
}

uint8_t hci_set_baudrate_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    (void)p_buf;
    uint32_t uart_baudrate;
    T_HCI_PATCH *p_hci_rtk = &hci_patch_info;
    hci_rtk_convert_buadrate(p_hci_rtk->baudrate, &uart_baudrate);
    //hci_board_debug("hci_tp_set_controller_baudrate: baudrate %d\n", uart_baudrate);
    hci_uart_set_baudrate(uart_baudrate);
    os_delay(50);
    return HCI_TP_CHECK_OK;
}
//==================================IQK=======LOK=======
uint8_t hci_tp_rf_radio_ver(uint8_t offset, uint16_t value)
{
    uint8_t *p_cmd;
    uint8_t *p;

    HCI_PRINT_TRACE0("hci_tp_rf_radio_ver");
    hci_board_debug("hci_tp_rf_radio_ver\n");

    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 8);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_VENDOR_RF_RADIO_REG_WRITE);
        LE_UINT8_TO_STREAM(p, 4); /* length */
        LE_UINT8_TO_STREAM(p, offset); /* offset		*/
        LE_UINT16_TO_STREAM(p, value); /* value */
        LE_UINT8_TO_STREAM(p, 0); /*  */
        
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}


// after IQK done, reset iqk_type to let next bt init do bt_check_iqk
void reset_iqk_type(void) {
	iqk_type = 0xff;
}

uint8_t hci_tp_rf_radio_ver_flow(void)
{
#define NONEEDSTARTIQK  0x02
    if(iqk_type != NONEEDSTARTIQK)
    {
        if(bt_check_iqk() == true)
        {
                //hci_board_debug("we have iqk value:  DUMP,\r\n");
                return HCI_TP_NOT_SEND;
        }
        hci_board_debug("we need start iqk\r\n");
        iqk_type = NONEEDSTARTIQK;
    }

    if(vendor_flow == 0)
    {
         hci_tp_rf_radio_ver(0x00, CALI_IQK_RF_STEP0);
    }
    else if(vendor_flow == 1)
    {
        hci_tp_rf_radio_ver(0x01, CALI_IQK_RF_STEP1);
    }
	else if(vendor_flow == 2)
    {
		hci_tp_rf_radio_ver(0x02, CALI_IQK_RF_STEP2);
	}
	else if(vendor_flow == 3)
	{
		hci_tp_rf_radio_ver(0x3f, CALI_IQK_RF_STEP3F);
	}
	else
    {
        hci_board_debug("IQK error\n");
    }
    return HCI_TP_CHECK_OK;
}
uint8_t hci_write_rf_check(uint8_t len, uint8_t *p_buf)
{
    (void)len;
    (void)p_buf;
    if (vendor_flow >= 3)   /* receive the last fragment completed evt */
    {
        if(hci_start_iqk()== false)
        {
                vendor_flow = 0;
                return HCI_TP_CONFIG_FAIL;
        }
        hci_board_debug("\n\rIQK OK\n");
        vendor_flow = 0;
        return HCI_TP_CHECK_OK;
    }
    else
    {
        hci_board_debug("continue add  %x\n", vendor_flow);
        vendor_flow++;
        return HCI_TP_CHECK_AGAIN;
    }
}

uint8_t hci_tp_write_efuse_iqk(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    HCI_PRINT_TRACE0("hci_tp_write_efuse_iqk");
    
    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 23);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_VSC_VENDOR_IQK);
        LE_UINT8_TO_STREAM(p, 19); /* length */

        memcpy(p, hci_tp_phy_efuse, 19);
        p+=19;
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
        return HCI_TP_CHECK_OK;
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
}
//=========================================================
uint8_t hci_tp_hci_reset(void)
{
    uint8_t *p_cmd;
    uint8_t *p;
    HCI_PRINT_TRACE0("hci_reset");
    //hci_board_debug("hci_reset\n");
    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 4);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_HCI_RESET);
        LE_UINT8_TO_STREAM(p, 0); /* length */
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}

HCI_PROCESS_TABLE hci_process_table[] ={
#ifdef HCI_START_IQK
    {HCI_VENDOR_RF_RADIO_REG_WRITE, hci_tp_rf_radio_ver_flow, hci_write_rf_check},
#endif
    //pre donload patch
    {HCI_READ_LOCAL_VERSION_INFO,hci_tp_read_local_ver, hci_read_local_version_check},
    {HCI_VSC_READ_ROM_VERSION,hci_tp_read_rom_ver, hci_read_rom_check},

    //download patch
    {HCI_VSC_UPDATE_BAUDRATE,hci_tp_set_controller_baudrate, hci_set_baudrate_check},
    {HCI_VSC_DOWNLOAD_PATCH,hci_tp_download_patch, hci_download_patch_check},
#ifdef FT_MODE
    {HCI_VSC_READ_THERMAL,hci_tp_read_thermal, hci_thermal_check},
#endif

#ifdef HCI_WRITE_IQK
    {HCI_VSC_VENDOR_IQK,hci_tp_write_efuse_iqk, NULL},
#endif
    {HCI_HCI_RESET,hci_tp_hci_reset, NULL},
};
uint8_t hci_total_step = sizeof(hci_process_table)/sizeof(HCI_PROCESS_TABLE);

uint8_t hci_tp_tx_test_cmd(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    //HCI_PRINT_TRACE0("hci_tp_read_local_ver");
    //
#define HCI_LE_TRANSMIT_TEST 0x201e
    hci_board_debug("hci_tp_tx_test_cmd 2402, 0xFF, 0x00\n");

    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 7);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_LE_TRANSMIT_TEST);
        LE_UINT8_TO_STREAM(p, 3);
        LE_UINT8_TO_STREAM(p, 0);//TX_CHANNEL
        LE_UINT8_TO_STREAM(p, 0xff);//length_of_test_data
        LE_UINT8_TO_STREAM(p, 0);//packet_payload
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
    }
    else
    {
         return HCI_TP_CONFIG_FAIL;
    }
    return HCI_TP_CHECK_OK;
}

uint8_t hci_tp_rx_test_cmd(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

    //HCI_PRINT_TRACE0("hci_tp_read_local_ver");
    //
#define HCI_LE_RECEIVE_TEST 0x201d
    hci_board_debug("hci_tp_rx_test_cmd 2402\n");
    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 6);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_LE_RECEIVE_TEST);
        LE_UINT8_TO_STREAM(p, 1);
        LE_UINT8_TO_STREAM(p, 0);//TX_CHANNEL
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
        
    }
    else
    {
        return HCI_TP_CONFIG_FAIL;
    }
     return HCI_TP_CHECK_OK;
}

uint8_t hci_tp_test_end_cmd(void)
{
    uint8_t *p_cmd;
    uint8_t *p;

#define HCI_LE_TEST_END 0x201f
    hci_board_debug("hci_tp_rx_test_cmd 2402\n");
    p_cmd = os_mem_zalloc(RAM_TYPE_DATA_ON, 6);
    if (p_cmd != NULL)
    {
        p = p_cmd;
        LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
        LE_UINT16_TO_STREAM(p, HCI_LE_TEST_END);
        LE_UINT8_TO_STREAM(p, 0);
        
        hci_adapter_send(p_cmd, p - p_cmd);
        //hci_tp_send(p_cmd, p - p_cmd, hci_rtk_tx_cb);
    }
    else
    {
       return HCI_TP_CONFIG_FAIL;
    }
     return HCI_TP_CHECK_OK;
}

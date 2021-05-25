/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "trace_app.h"
#include "vendor_cmd.h"
#include "rtk_coex.h"
#include "platform_stdlib.h"
#include <bt_intf.h>

struct rtl_btinfo
{
    uint8_t cmd;
    uint8_t len;
    uint8_t data[6];
};

unsigned int send_coex_mailbox_to_wifi_from_BtAPP(uint8_t state)
{
    uint8_t para[8];

    para[0] = 0x45;      //Mailbox ID
    para[1] = state;     //Data0
    para[2] = 0;     //Data1
    para[3] = 0;     //Data2
    para[4] = 0;     //Data3
    para[5] = 0;     //Data4
    para[6] = 0;     //Data5
    para[7] = 0;     //Data6
    rltk_coex_mailbox_to_wifi(para, 8);
    return 1;
}

static void rtk_notify_info_to_wifi(uint8_t length, uint8_t *report_info)
{
    uint8_t para_length = 1 + length;
    uint8_t *p = (uint8_t *)malloc(para_length * sizeof(uint8_t));
    struct rtl_btinfo *report = (struct rtl_btinfo *)(report_info);

    *(p++) = report->cmd;
    //*p++ = 0x07;
    *(p++) = report->len;
    if (length)
    {
        memcpy(p, report->data, report->len);
    }
    if (length)
    {
        HCI_PRINT_TRACE1("bt info: cmd %2.2X", report->cmd);
        HCI_PRINT_TRACE1("bt info: len %2.2X", report->len);
        HCI_PRINT_TRACE6("bt info: data %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X",
                         report->data[0], report->data[1], report->data[2],
                         report->data[3], report->data[4], report->data[5]);
    }

    

    	rltk_coex_mailbox_to_wifi(report_info, report->len + 2);
    free(p);
    //rtk_btcoex_send_msg_towifi(buf, para_length + HCI_CMD_PREAMBLE_SIZE);
   // mailbox_to_wifi(buf, para_length + HCI_CMD_PREAMBLE_SIZE);
    /* send BT INFO to Wi-Fi driver */
}

void bt_coex_handle_cmd_complete_evt(uint16_t opcode, uint16_t cause, uint8_t total_len, uint8_t *p)
{

    (void)cause;

    if (opcode == HCI_VENDOR_MAILBOX_CMD)
    {
        uint8_t status;
        status = *p++;//jump the double subcmd
        total_len--;
        if(total_len <=1)
        {
            HCI_PRINT_TRACE0("bt_coex_handle_cmd_complete_evt: not reprot to wifi");
            return ;
        }



        (void)status;
        rltk_coex_mailbox_to_wifi(p, total_len);
        //rtk_parse_vendor_mailbox_cmd_evt(p, total_len, status);
    }
}

void bt_coex_handle_specific_evt(uint8_t *p, uint8_t len)
{
	rltk_coex_mailbox_to_wifi(p, len);
}

void bt_coex_init(void)
{
	vendor_cmd_init(NULL);
}



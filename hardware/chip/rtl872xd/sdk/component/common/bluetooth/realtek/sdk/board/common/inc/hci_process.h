/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */
#ifndef _HCI_PROCESS_H_
#define _HCI_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct _HCI_PROCESS_TABLE_{
    uint16_t    opcode;
    uint8_t     (*start_pro)(void);
    uint8_t     (*check_func)(uint8_t len,uint8_t *p_buf);
}HCI_PROCESS_TABLE, *PHCI_PROCESS_TABLE;

typedef struct
{
    uint32_t    bt_baudrate;
    uint32_t    uart_baudrate;
} BAUDRATE_MAP;

#define HCI_TP_CHECK_ERROR        0xFF
#define HCI_TP_CONFIG_END         0x01
#define HCI_TP_CONFIG_FAIL        0x02
#define HCI_TP_CHECK_OK           0x00
#define HCI_TP_CHECK_AGAIN        0x03

#define HCI_TP_NOT_SEND        0x04

#define HCI_COMMAND_COMPLETE            0x0e
#define HCI_COMMAND_STATUS              0x0f
#define HCI_CMD_PKT             0x01
#define HCI_ACL_PKT             0x02
#define HCI_SCO_PKT             0x03
#define HCI_EVT_PKT             0x04
#define HCI_CMD_HDR_LEN         4   /* packet type (1), command (2), length (1) */

#define HCI_READ_LOCAL_VERSION_INFO     0x1001
#define HCI_VSC_READ_ROM_VERSION        0xFC6D
#define HCI_VSC_UPDATE_BAUDRATE         0xFC17
#define HCI_VSC_DOWNLOAD_PATCH          0xFC20
#define HCI_VSC_CHECK_32K                     0xFC02
#define HCI_HCI_RESET                             0x0C03
#define HCI_VENDOR_RF_RADIO_REG_WRITE     0xfd4a
#define HCI_VSC_VENDOR_IQK        0xFD91
#define HCI_VSC_READ_THERMAL       0xFC40


bool hci_rtk_tx_cb(void);
bool hci_adapter_send(uint8_t *p_buf, uint16_t len);

bool hci_set_patch(uint8_t *fw_addr,uint32_t fw_len, uint8_t *fw_config_addr, uint32_t
        fw_config_len, uint32_t baudrate);

void reset_iqk_type(void);

#ifdef __cplusplus
}
#endif

#endif /* _HCI_PROCESS_H_ */

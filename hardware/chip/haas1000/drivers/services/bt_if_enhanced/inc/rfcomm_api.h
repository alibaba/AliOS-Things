/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __RFCOMM_API_H__
#define __RFCOMM_API_H__
#include "bluetooth.h"
#include "me_api.h"
#include "bt_if.h"

typedef void *rf_channel_t;

typedef uint8_t rf_event_t;

#define BTIF_RFEVENT_OPEN_IND              1
#define BTIF_RFEVENT_OPEN                  2
#define BTIF_RFEVENT_CLOSE_IND             3
#define BTIF_RFEVENT_CLOSED                4
#define BTIF_RFEVENT_DATA_IND              5
#define BTIF_RFEVENT_PACKET_HANDLED        6
#define BTIF_RFEVENT_TEST_CNF              7
#define BTIF_RFEVENT_PORT_NEG_IND          8
#define BTIF_RFEVENT_PORT_NEG_CNF          9
#define BTIF_RFEVENT_PORT_STATUS_IND       10
#define BTIF_RFEVENT_PORT_STATUS_CNF       11
#define BTIF_RFEVENT_MODEM_STATUS_IND      12
#define BTIF_RFEVENT_MODEM_STATUS_CNF      13
#define BTIF_RFEVENT_LINE_STATUS_IND       14
#define BTIF_RFEVENT_LINE_STATUS_CNF       15
#define BTIF_RFEVENT_FLOW_OFF_IND          16
#define BTIF_RFEVENT_FLOW_ON_IND           17
#define BTIF_RFEVENT_RESOURCE_FREE         18
#define BTIF_RFEVENT_FRAME_SIZE_CHANGED    19

typedef uint8_t rf_signals;
#define RFCOMM_FLOW  0x02
#define RFCOMM_RTC   0x04
#define RFCOMM_RTR   0x08
#define RFCOMM_IC    0x40
#define RFCOMM_DV    0x80

#define RFCOMM_DSR   0x04
#define RFCOMM_CTS   0x08
#define RFCOMM_RI    0x40
#define RFCOMM_CD    0x80

#define RFCOMM_DTR   0x04
#define RFCOMM_RTS   0x08

typedef uint8_t rf_data_format;
#define RFCOMM_DATA_BITS_5       0x00
#define RFCOMM_DATA_BITS_6       0x02
#define RFCOMM_DATA_BITS_7       0x01
#define RFCOMM_DATA_BITS_8       0x03

#define RFCOMM_STOP_BITS_1       0x00
#define RFCOMM_STOP_BITS_1_5     0x04

#define RFCOMM_PARITY_NONE       0x00
#define RFCOMM_PARITY_ON         0x08

#define RFCOMM_PARITY_TYPE_ODD        0x00
#define RFCOMM_PARITY_TYPE_EVEN       0x20
#define RFCOMM_PARITY_TYPE_MARK       0x10
#define RFCOMM_PARITY_TYPE_SPACE      0x30

#define RFCOMM_DATA_BITS_MASK    0x03
#define RFCOMM_STOP_BITS_MASK    0x04
#define RFCOMM_PARITY_MASK       0x08
#define RFCOMM_PARITY_TYPE_MASK  0x30

typedef uint8_t rf_line_status;
#define RFCOMM_LINE_ERROR  0x01
#define RFCOMM_OVERRUN     0x02
#define RFCOMM_PARITY      0x04
#define RFCOMM_FRAMING     0x08

typedef uint8_t rf_signals;
#define RFCOMM_FLOW  0x02
#define RFCOMM_RTC   0x04       /* Set when sender is ready to communicate.     */
#define RFCOMM_RTR   0x08       /* Set when sender is ready to receive data.    */
#define RFCOMM_IC    0x40       /* Set when a call is incoming.        */
#define RFCOMM_DV    0x80       /* Set when valid data is being sent.  */

#define RFCOMM_DSR   0x04
#define RFCOMM_CTS   0x08
#define RFCOMM_RI    0x40
#define RFCOMM_CD    0x80

#define RFCOMM_DTR   0x04
#define RFCOMM_RTS   0x08

struct rf_modem_status
{
    rf_signals signals;
    uint8_t break_len;
};

typedef uint16_t rf_port_settings_mask;

#define RFCOMM_PARM_BAUDRATE     0x0001
#define RFCOMM_PARM_DATABITS     0x0002
#define RFCOMM_PARM_STOPBITS     0x0004
#define RFCOMM_PARM_PARITY       0x0008
#define RFCOMM_PARM_PARITY_TYPE  0x0010
#define RFCOMM_PARM_DATA_FORMAT  (RFCOMM_PARM_DATABITS | RFCOMM_PARM_STOPBITS | \
                              RFCOMM_PARM_PARITY | RFCOMM_PARM_PARITY_TYPE)
#define RFCOMM_PARM_XON_CHAR     0x0020
#define RFCOMM_PARM_XOFF_CHAR    0x0040

#define RFCOMM_PARM_XON_INPUT    0x0100
#define RFCOMM_PARM_XON_OUTPUT   0x0200
#define RFCOMM_PARM_RTR_INPUT    0x0400
#define RFCOMM_PARM_RTR_OUTPUT   0x0800
#define RFCOMM_PARM_RTC_INPUT    0x1000
#define RFCOMM_PARM_RTC_OUTPUT   0x2000

#define RFCOMM_PARM_FLOW_RTS_CTS  (RFCOMM_PARM_RTR_INPUT | RFCOMM_PARM_RTR_OUTPUT)
#define RFCOMM_PARM_FLOW_DTR_DSR  (RFCOMM_PARM_RTC_INPUT | RFCOMM_PARM_RTC_OUTPUT)
#define RFCOMM_PARM_FLOW_XON_XOFF (RFCOMM_PARM_XON_INPUT | RFCOMM_PARM_XON_OUTPUT)

typedef uint8_t rf_flow_control;

#define RFCOMM_FLOW_CTRL_NONE    0x00
#define RFCOMM_XON_ON_INPUT      0x01
#define RFCOMM_XON_ON_OUTPUT     0x02
#define RFCOMM_RTR_ON_INPUT      0x04
#define RFCOMM_RTR_ON_OUTPUT     0x08
#define RFCOMM_RTC_ON_INPUT      0x10
#define RFCOMM_RTC_ON_OUTPUT     0x20

#define RFCOMM_FLOW_RTS_CTS      (RFCOMM_RTR_ON_INPUT | RFCOMM_RTR_ON_OUTPUT)
#define RFCOMM_FLOW_DTR_DSR      (RFCOMM_RTC_ON_INPUT | RFCOMM_RTC_ON_OUTPUT)
#define RFCOMM_FLOW_XON_XOFF     (RFCOMM_XON_ON_INPUT | RFCOMM_XON_ON_OUTPUT)

typedef uint8_t rf_data_format;
#define RFCOMM_DATA_BITS_5       0x00
#define RFCOMM_DATA_BITS_6       0x02
#define RFCOMM_DATA_BITS_7       0x01
#define RFCOMM_DATA_BITS_8       0x03
#define RFCOMM_STOP_BITS_1       0x00
#define RFCOMM_STOP_BITS_1_5     0x04
#define RFCOMM_PARITY_NONE       0x00
#define RFCOMM_PARITY_ON         0x08
#define RFCOMM_PARITY_TYPE_ODD   0x00
#define RFCOMM_PARITY_TYPE_EVEN  0x20
#define RFCOMM_PARITY_TYPE_MARK  0x10
#define RFCOMM_PARITY_TYPE_SPACE 0x30
#define RFCOMM_DATA_BITS_MASK    0x03
#define RFCOMM_STOP_BITS_MASK    0x04
#define RFCOMM_PARITY_MASK       0x08
#define RFCOMM_PARITY_TYPE_MASK  0x30

typedef uint8_t rf_baud_rate;

#define RFCOMM_BAUD_2400         0x00
#define RFCOMM_BAUD_4800         0x01
#define RFCOMM_BAUD_7200         0x02
#define RFCOMM_BAUD_9600         0x03
#define RFCOMM_BAUD_19200        0x04
#define RFCOMM_BAUD_38400        0x05
#define RFCOMM_BAUD_57600        0x06
#define RFCOMM_BAUD_115200       0x07
#define RFCOMM_BAUD_230400       0x08

typedef struct _rf_port_settings
{
    rf_baud_rate baudRate;
    rf_data_format dataFormat;
    rf_flow_control flowControl;
    uint8_t xonChar;
    uint8_t xoffChar;
    rf_port_settings_mask parmMask;
} rf_port_settings;

struct rf_callback_prams
{
    rf_event_t event;
    int status;
    uint16_t data_len;
    union
    {
        btif_bt_packet_t *packet;
        U8 *data;
        struct rf_modem_status *modem_status;
        rf_line_status *line_status;
        rf_port_settings *port_settings;
        btif_remote_device_t *remDev;
    } ptrs;
};

typedef void (*rf_callback_func) (rf_channel_t chan, struct rf_callback_prams * parms);

struct rf_chan_info
{
    rf_callback_func callback;
    uint16_t max_frame_size;
    uint8_t priority;
    void *priv;
};

struct rf_service
{
    uint8_t serviceId;
};

struct _rf_channel
{
    rf_callback_func callback;
    int8_t server_channel;
    uint32_t rfcomm_handle;
    uint32_t app_id;
    void *priv;
    list_entry_t tx_queue;
};

#ifdef __cplusplus
extern "C" {
#endif                          /*  */
void btif_rfcomm_ctx_input_init(void *para);
rf_channel_t btif_rf_create_channel(void);

void btif_rf_free_channel(rf_channel_t chan_h);

bt_status_t btif_rf_register_server_channel(rf_channel_t chan_h,
        struct rf_service *service, uint8_t credit);
bt_status_t btif_rf_deregister_server_channel(rf_channel_t chan_h, struct rf_service *service);

bt_status_t btif_rf_deregister_service(struct rf_service *service);

uint16_t btif_rf_frame_size(rf_channel_t chan_h);

bt_status_t btif_rf_respond_channel(rf_channel_t chan_h, bool accept);

bt_status_t btif_rf_accept_channel(rf_channel_t chan_h);
#define btif_rf_accept_channel(c) btif_rf_respond_channel(c, true)

bt_status_t btif_rf_reject_channel(rf_channel_t chan_h);
#define btif_rf_reject_channel(c) btif_rf_respond_channel(c, false)

#if BTIF_RF_SEND_CONTROL == BTIF_ENABLED
bt_status_t btif_rf_send_port_settings(rf_channel_t chan_h, rf_port_settings * PortSettings);

bt_status_t btif_rf_request_port_settings(rf_channel_t chan_h, rf_port_settings * portSettings);
#define btif_rf_request_port_settings(c, p) btif_rf_send_port_settings(c, p)

bt_status_t btif_rf_request_port_status(rf_channel_t chan_h);
#define btif_rf_request_port_status(c) btif_rf_send_port_settings(c, NULL)

bt_status_t btif_rf_set_modem_status(rf_channel_t chan_h,
                                     struct rf_modem_status *modem_status);
#endif                          /*BTIF_RF_SEND_CONTROL == BTIF_ENABLED */

bt_status_t btif_rf_open_client_channel(btif_remote_device_t * RemDev,
                                        uint8_t server_id, rf_channel_t chan_h, uint8_t credit);

bt_status_t btif_rf_close_channel(rf_channel_t chan_h);

bt_status_t btif_rf_send_data(rf_channel_t chan_h, btif_bt_packet_t * Packet);

bt_status_t btif_rf_advance_credit(rf_channel_t chan_h, uint8_t credit);

bool btif_rf_credit_flow_enabled(rf_channel_t chan_h);

bt_status_t btif_rf_setup_channel(rf_channel_t chan_h, struct rf_chan_info *info);

void *btif_rf_get_channel_priv(rf_channel_t chan);
uint32_t btif_rf_get_app_id(rf_channel_t chan_h);

uint32_t btif_rf_get_rfcomm_handle(rf_channel_t chan_h);
bt_status_t btif_rf_set_rfcomm_handle(rf_channel_t chan_h, uint32_t rfcomm_handle);
uint8_t btif_rf_get_server_channel(rf_channel_t chan_h);

uint16_t btif_rf_max_frame_size_configure(void);
void btif_rf_set_app_id(rf_channel_t chan_h,uint32_t app_id);

#if defined(IBRT)
#if defined(ENHANCED_STACK)
uint32_t btif_save_rfc_ctx(uint8_t *ctx_buffer, uint8_t MuxId);
uint32_t btif_set_rfc_ctx(uint8_t* ctx_buffer, uint16_t MuxId, uint8_t rm_devtbl_idx);
uint32_t btif_rfc_get_session_l2c_handle(uint16_t hci_handle);
bool btif_rfc_is_dlci_channel_connected(uint32_t session_l2c_handle,uint32_t app_id);
#else
uint32_t btif_save_rfc_ctx(uint8_t *ctx_buffer, uint8_t MuxId);
uint32_t btif_set_rfc_ctx(uint8_t* ctx_buffer, uint16_t MuxId, uint8_t rm_devtbl_idx);
uint32_t btif_save_rfcomm_channel_ctx(uint8_t *ctx_buffer, uint8_t MuxId, uint32_t app_id);
uint32_t btif_set_rfcomm_channel_ctx(uint8_t *ctx_buffer, uint8_t MuxId, uint32_t app_id);
#endif
bool btif_rf_is_rfcomm_channel_connected(uint8_t mux_id,uint32_t app_id);
btif_remote_device_t*  btif_rfc_get_mux_remote_device(uint8_t mux_idx);
uint8_t btif_rfc_get_mux_index(uint16_t hci_handle);
uint8_t btif_rfc_get_mux_state(uint8_t mux_idx);
#endif

#ifdef __cplusplus
}
#endif
#endif /*__RFCOMM_API_H__*/

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __RFCOMM_API_H__
#define __RFCOMM_API_H__
#include "bluetooth.h"
#include "me_api.h"
#include "a2dp_api.h"
#include "bt_if.h"

typedef void *rf_channel_t;

typedef uint8_t rf_event_t;

/** A remote device has requested a connection to a local RFCOMM service.
 * Call RF_AcceptChannel to indicate whether the channel should be opened.
 * This call can be made during or after the callback.
 * The remote device associated with the channel is provided in "ptrs.remDev."
 */
#define BTIF_RFEVENT_OPEN_IND              1

/** A channel is now open and ready for data exchange. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define BTIF_RFEVENT_OPEN                  2

/** A request to close a channel was received. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define BTIF_RFEVENT_CLOSE_IND             3

/** The channel is closed. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define BTIF_RFEVENT_CLOSED                4

/** Data was received from the remote device. The "dataLen" and "ptrs.data"
 * fields describe the received data.
 */
#define BTIF_RFEVENT_DATA_IND              5

/** RFCOMM is finished with the data packet provided in "ptrs.packet".
 * The RFCOMM user may reuse the packet structure to send more data.
 *
 * The "status" field indicates whether the packet was sent successfully
 * (BT_STS_SUCCESS) or if an error occurred (BT_STS_NO_CONNECTION).
 */
#define BTIF_RFEVENT_PACKET_HANDLED        6

/** The remote device has responded to a test sent by RF_SendTest.
 * The "status" field contains BT_STS_SUCCESS if the test succeeded,
 * or BT_STS_FAILED if the response did not match.
 *
 * This event is only available when RF_SEND_TEST is enabled.
 */
#define BTIF_RFEVENT_TEST_CNF              7

/** The remote device has requested negotiation of port settings. The
 * requested settings are in "ptrs.portSettings". The relevant parameters
 * for this indication are identified in "ptrs.portSettings->parmMask".
 *
 * After reviewing all relevant parameters, the RFCOMM user must
 * call RF_AcceptPortSettings to indicate which parameters were accepted
 * or rejected.  Either RF_AcceptPortSettings or RF_DelayPortRsp must be
 * called during the callback.  If RF_DelayPortRsp is called, then the
 * processing of this event can be handled outside the context of the
 * callback.  If processing is delayed, it is important that the application
 * call RF_AcceptPortSettings within 10 seconds to prevent a link disconnect
 * by the remote device.
 *
 * If neither RF_AcceptPortSettings nor RF_DelayPortRsp are called during
 * the callback, the requested parameters are considered to be accepted
 * and a response will be generated automatically.  Port settings are only
 * used for RS232 emulation and have no real effect on the data format
 * or flow control at the RFCOMM layer.
 *
 * It is possible for a registered service to receive a port negotiation
 * before the channel is actually open (RFEVENT_OPEN). If a port
 * negotiation is never received by the application, default values
 * should be assumed: 9600 baud, 8 bits, 1 stop bit, no parity, and no
 * port flow control.
 */
#define BTIF_RFEVENT_PORT_NEG_IND          8

/** A port settings negotiation request (started with btif_rf_request_port_settings)
 * is now complete. During this event, the "ptrs.portSettings->parmMask"
 * field contains bits that indicate which parameters were accepted by the
 * remote device.
 *
 * This event is only available when BTIF_RF_SEND_CONTROL is enabled.
 */
#define BTIF_RFEVENT_PORT_NEG_CNF          9

/** The remote device has requested the status of the port settings.  The
 * application must respond with its current port settings by calling
 * RF_SendPortStatus.  Either RF_SendPortStatus or RF_DelayPortRsp must be
 * called during the callback.  If RF_DelayPortRsp is called, then the
 * processing of this event can be handled outside the context of the
 * callback.  If processing is delayed, it is important that the application
 * call RF_SendPortStatus within 10 seconds to prevent a link disconnect
 * by the remote device.
 *
 * If the application does not respond to this event during the callback,
 * the response will be generated  automatically with the default Serial Port
 * Profile settings of: 9600 baud, 8 data bits, 1 stop bit, and no parity.
 */
#define BTIF_RFEVENT_PORT_STATUS_IND       10

/** The remote device has responded to a request for its current port
 * status.  This happens in response to btif_rf_request_port_status.  The relevant
 * parameters for this indication are identified in "ptrs.portSettings".
 *
 * This event is only available when BTIF_RF_SEND_CONTROL is enabled.
 */
#define BTIF_RFEVENT_PORT_STATUS_CNF       11

/** The remote device has provided modem status. The new status settings
 * are in "ptrs.modemStatus".
 */
#define BTIF_RFEVENT_MODEM_STATUS_IND      12

/** The remote device has acknowledged new modem status settings. The RFCOMM
 * user sent these settings with the RF_SetModemStatus function.
 */
#define BTIF_RFEVENT_MODEM_STATUS_CNF      13

/** The remote device has provided line status information. The new
 * line status settings are in "ptrs.lineStatus".
 *
 * This event is only available when BTIF_RF_SEND_CONTROL is enabled.
 */
#define BTIF_RFEVENT_LINE_STATUS_IND       14

/** RFCOMM has finished sending line status to the remote device. The RFCOMM
 * user sent these settings with the RF_SetLineStatus function.
 *
 * This event is only available when BTIF_RF_SEND_CONTROL is enabled.
 */
#define BTIF_RFEVENT_LINE_STATUS_CNF       15

/** The remote device has indicated that no RFCOMM data can be processed.
 * This indication affects all RFCOMM channels.  This event will only
 * be received if credit-based flow control has not been negotiated.
 */
#define BTIF_RFEVENT_FLOW_OFF_IND          16

/** The remote device has indicated that RFCOMM data can be processed again.
 * This indication affects all RFCOMM channels.  This event will only
 * be received if credit-based flow control has not been negotiated.
 */
#define BTIF_RFEVENT_FLOW_ON_IND           17

/** A resource is available for use by RF_AcceptPortSettings or
 *  RF_SendPortStatus.  If either of these functions returns
 *  BT_STS_NO_RESOURCES, this event will be sent to indicate that a
 *  resource is now available.
 */
#define BTIF_RFEVENT_RESOURCE_FREE         18

/** The framesize for the specified channel has changed.  The new value can
 *  be found by calling RF_FrameSize().
 */
#define BTIF_RFEVENT_FRAME_SIZE_CHANGED    19

/* End of RfEvent */

/*---------------------------------------------------------------------------
 * rf_signals type
 *
 *     Specifies signal types in an RfModemStatus structure.
 */
typedef uint8_t rf_signals;

/* Group: Any of the following signals may be set or clear. They represent
 * standard TS 7.10 Terminology for expressing each possible signal.
 */
#define RFCOMM_FLOW  0x02       /* Set when sender is unable to receive frames.
                                 *  This is only used when credit-based flow control
                                 *  has not been negotiated.
                                 */
#define RFCOMM_RTC   0x04       /* Set when sender is ready to communicate.     */
#define RFCOMM_RTR   0x08       /* Set when sender is ready to receive data.    */
#define RFCOMM_IC    0x40       /* Set when a call is incoming.        */
#define RFCOMM_DV    0x80       /* Set when valid data is being sent.  */

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DCE would send to a DTE device.
 */
#define RFCOMM_DSR   0x04
#define RFCOMM_CTS   0x08
#define RFCOMM_RI    0x40
#define RFCOMM_CD    0x80

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DTE would send to a DCE device.
 */
#define RFCOMM_DTR   0x04
#define RFCOMM_RTS   0x08
/* End of rf_signals */

/*---------------------------------------------------------------------------
 * rf_data_format type
 *
 *     Specifies the data bits, stop bits, and parity in an RfPortSettings
 *     structure. The data bits, stop bits, and parity settings are OR'ed
 *     together in the "dataFormat" field.
 */
typedef uint8_t rf_data_format;

/* Group: These values identify the number of data bits. */
#define RFCOMM_DATA_BITS_5       0x00
#define RFCOMM_DATA_BITS_6       0x02
#define RFCOMM_DATA_BITS_7       0x01
#define RFCOMM_DATA_BITS_8       0x03

/* Group: These values identify the number of stop bits. */
#define RFCOMM_STOP_BITS_1       0x00
#define RFCOMM_STOP_BITS_1_5     0x04

/* Group: These values identify whether of parity is used. */
#define RFCOMM_PARITY_NONE       0x00
#define RFCOMM_PARITY_ON         0x08

/* Group: These values identify the type of parity in use. Note that
 * these values have no effect unless parity is enabled (RFCOMM_PARITY_ON). */
#define RFCOMM_PARITY_TYPE_ODD        0x00
#define RFCOMM_PARITY_TYPE_EVEN       0x20
#define RFCOMM_PARITY_TYPE_MARK       0x10
#define RFCOMM_PARITY_TYPE_SPACE      0x30

/* Group: These masks allow you to select only the relevant bits from
 * the "dataFormat" field.
 */

/* AND with "dataFormat" for the data bits setting. */
#define RFCOMM_DATA_BITS_MASK    0x03

/* AND with "dataFormat" for the stop bits setting. */
#define RFCOMM_STOP_BITS_MASK    0x04

/* AND with "dataFormat" to determine whether parity is on.  */
#define RFCOMM_PARITY_MASK       0x08

/* AND with "dataFormat" to determine the type of parity selected. */
#define RFCOMM_PARITY_TYPE_MASK  0x30

/* End of rf_data_format */

/*---------------------------------------------------------------------------
 * rf_line_status type
 *
 *     Represents the line status. Line status is set using the
 *     RF_SetLineStatus function. If the remote device changes the line
 *     status, an rf_line_status value will be received during an
 *     RFEVENT_LINE_STATUS_IND event.
 */
typedef uint8_t rf_line_status;

/* When set, one or more errors have occurred. Any of the following bits
 * may be set to indicate an error. If RF_LINE_ERROR is clear, no error
 * has occurred, regardless of the state of the other bits.
 */
#define RFCOMM_LINE_ERROR  0x01
#define RFCOMM_OVERRUN     0x02 /* Set to indicate an overrun error. */
#define RFCOMM_PARITY      0x04 /* Set to indicate a parity error. */
#define RFCOMM_FRAMING     0x08 /* Set to indicate a framing error. */

/* End of rf_line_status*/

/*---------------------------------------------------------------------------
 * rf_signals type
 *
 *     Specifies signal types in an RfModemStatus structure.
 */
typedef uint8_t rf_signals;

/* Group: Any of the following signals may be set or clear. They represent
 * standard TS 7.10 Terminology for expressing each possible signal.
 */
#define RFCOMM_FLOW  0x02       /* Set when sender is unable to receive frames.
                                 *  This is only used when credit-based flow control
                                 *  has not been negotiated.
                                 */
#define RFCOMM_RTC   0x04       /* Set when sender is ready to communicate.     */
#define RFCOMM_RTR   0x08       /* Set when sender is ready to receive data.    */
#define RFCOMM_IC    0x40       /* Set when a call is incoming.        */
#define RFCOMM_DV    0x80       /* Set when valid data is being sent.  */

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DCE would send to a DTE device.
 */
#define RFCOMM_DSR   0x04
#define RFCOMM_CTS   0x08
#define RFCOMM_RI    0x40
#define RFCOMM_CD    0x80

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DTE would send to a DCE device.
 */
#define RFCOMM_DTR   0x04
#define RFCOMM_RTS   0x08

/* End of rf_signals */

/*---------------------------------------------------------------------------
 * RfModemStatus structure
 *
 * Represents the status of V.24 signals. To send signals on a connection,
 * create and fill in this structure, then call RF_SetModemStatus.
 *
 * When the remote device sends these signals, RFCOMM generates an
 * RFEVENT_MODEM_STATUS_IND event. During this event, you can read the
 * status in the RfCallbackParms "ptrs.modemStatus" field.
 */
struct rf_modem_status {
    rf_signals signals;         /* Contains all signals that apply to this status
                                 * message.
                                 */
    uint8_t break_len;          /* Indicates the length of the break signal in 200 ms
                                 * units. If 0, no break signal was sent. Must be
                                 * between 0 and 15 (inclusive).
                                 */
};

/*---------------------------------------------------------------------------
 * rf_port_settings_mask type
 *
 *     Used to specify which settings in an RfPortSettings structure are
 *     valid.  Port settings are used for RS232 emulation.
 */
typedef uint16_t rf_port_settings_mask;

/* The "baudRate" field is valid. */
#define RFCOMM_PARM_BAUDRATE     0x0001

/* The RFCOMM_DATA_BITS_* part of the "dataFormat" field is valid. */
#define RFCOMM_PARM_DATABITS     0x0002

/* The RFCOMM_STOP_BITS_* part of the "dataFormat" field is valid. */
#define RFCOMM_PARM_STOPBITS     0x0004

/* The RFCOMM_PARITY_NONE or RFCOMM_PARITY_ON part of the "dataFormat" field is
 * valid.
 */
#define RFCOMM_PARM_PARITY       0x0008

/* The RFCOMM_PARITY_* part of the "dataFormat" field is valid. */
#define RFCOMM_PARM_PARITY_TYPE  0x0010

/* All elements in the "dataFormat" field are valid. */
#define RFCOMM_PARM_DATA_FORMAT  (RFCOMM_PARM_DATABITS | RFCOMM_PARM_STOPBITS | \
                              RFCOMM_PARM_PARITY | RFCOMM_PARM_PARITY_TYPE)

/* The "xonChar" field is valid. */
#define RFCOMM_PARM_XON_CHAR     0x0020

/* The "xoffChar" field is valid. */
#define RFCOMM_PARM_XOFF_CHAR    0x0040

/* Group: The following fields identify whether specific bits in the
 * "flowControl" field are valid. They are equivalent to the values
 * described in the rf_flow_control type.
 */
#define RFCOMM_PARM_XON_INPUT    0x0100
#define RFCOMM_PARM_XON_OUTPUT   0x0200
#define RFCOMM_PARM_RTR_INPUT    0x0400
#define RFCOMM_PARM_RTR_OUTPUT   0x0800
#define RFCOMM_PARM_RTC_INPUT    0x1000
#define RFCOMM_PARM_RTC_OUTPUT   0x2000

#define RFCOMM_PARM_FLOW_RTS_CTS  (RFCOMM_PARM_RTR_INPUT | RFCOMM_PARM_RTR_OUTPUT)
#define RFCOMM_PARM_FLOW_DTR_DSR  (RFCOMM_PARM_RTC_INPUT | RFCOMM_PARM_RTC_OUTPUT)
#define RFCOMM_PARM_FLOW_XON_XOFF (RFCOMM_PARM_XON_INPUT | RFCOMM_PARM_XON_OUTPUT)

/* End of rf_port_settings_mask */

/*---------------------------------------------------------------------------
 * rf_flow_control type
 *
 *     Specifies the port flow control type in an RfPortSettings structure.
 *     This refers to flow control for RS232 emulation and not to the actual
 *     flow control of RFCOMM data which is controlled by either credit-based
 *     flow control (if negotiated), aggregate flow control (FCON/FCOFF), or
 *     the FC bit in the Modem Status Command.
 */
typedef uint8_t rf_flow_control;

#define RFCOMM_FLOW_CTRL_NONE    0x00
#define RFCOMM_XON_ON_INPUT      0x01
#define RFCOMM_XON_ON_OUTPUT     0x02
#define RFCOMM_RTR_ON_INPUT      0x04
#define RFCOMM_RTR_ON_OUTPUT     0x08
#define RFCOMM_RTC_ON_INPUT      0x10
#define RFCOMM_RTC_ON_OUTPUT     0x20

/* Group: These symbols define common combinations of port flow control
 * settings used in RS232 emulation
 */
#define RFCOMM_FLOW_RTS_CTS      (RFCOMM_RTR_ON_INPUT | RFCOMM_RTR_ON_OUTPUT)
#define RFCOMM_FLOW_DTR_DSR      (RFCOMM_RTC_ON_INPUT | RFCOMM_RTC_ON_OUTPUT)
#define RFCOMM_FLOW_XON_XOFF     (RFCOMM_XON_ON_INPUT | RFCOMM_XON_ON_OUTPUT)

/* End of rf_flow_control */

/*---------------------------------------------------------------------------
 * rf_data_format type
 *
 *     Specifies the data bits, stop bits, and parity in an RfPortSettings
 *     structure. The data bits, stop bits, and parity settings are OR'ed
 *     together in the "dataFormat" field.
 */
typedef uint8_t rf_data_format;

/* Group: These values identify the number of data bits. */
#define RFCOMM_DATA_BITS_5       0x00
#define RFCOMM_DATA_BITS_6       0x02
#define RFCOMM_DATA_BITS_7       0x01
#define RFCOMM_DATA_BITS_8       0x03

/* Group: These values identify the number of stop bits. */
#define RFCOMM_STOP_BITS_1       0x00
#define RFCOMM_STOP_BITS_1_5     0x04

/* Group: These values identify whether of parity is used. */
#define RFCOMM_PARITY_NONE       0x00
#define RFCOMM_PARITY_ON         0x08

/* Group: These values identify the type of parity in use. Note that
 * these values have no effect unless parity is enabled (RFCOMM_PARITY_ON). */
#define RFCOMM_PARITY_TYPE_ODD        0x00
#define RFCOMM_PARITY_TYPE_EVEN       0x20
#define RFCOMM_PARITY_TYPE_MARK       0x10
#define RFCOMM_PARITY_TYPE_SPACE      0x30

/* Group: These masks allow you to select only the relevant bits from
 * the "dataFormat" field.
 */

/* AND with "dataFormat" for the data bits setting. */
#define RFCOMM_DATA_BITS_MASK    0x03

/* AND with "dataFormat" for the stop bits setting. */
#define RFCOMM_STOP_BITS_MASK    0x04

/* AND with "dataFormat" to determine whether parity is on.  */
#define RFCOMM_PARITY_MASK       0x08

/* AND with "dataFormat" to determine the type of parity selected. */
#define RFCOMM_PARITY_TYPE_MASK  0x30

/* End of rf_data_format */

/*---------------------------------------------------------------------------
 * rf_baud_rate type
 *
 *     Specifies the baud rate in an RfPortSettings structure. Note that
 *     the baud rate setting does not actually affect RFCOMM throughput.
 */
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

/* End of rf_baud_rate */

/*---------------------------------------------------------------------------
 * rf_port_settings structure
 *
 *     Represents port settings for an RFCOMM channel. Port settings
 *     are used for RS232 emulation.  They are informational and have
 *     no real effect on the data stream at the RFCOMM level.  To change
 *     port settings for a channel, this structure is filled and provided
 *     to RFCOMM using the RF_RequestPortSettings function.
 *
 *     When the remote device changes port settings, RFCOMM provides
 *     this structure along with the RFEVENT_PORT_NEG_IND event. The
 *     RFCOMM user can then review the settings, then accept or reject
 *     them with RF_AcceptPortSettings.
 *
 *     Fields in this structure may be valid or invalid, depending on
 *     the contents of the "parmMask" field.
 */
typedef struct _rf_port_settings {
    rf_baud_rate baudRate;      /* Indicates the baud rate */
    rf_data_format dataFormat;  /* Contains data bits, stop bits, and
                                 * parity settings.
                                 */
    rf_flow_control flowControl;    /* Indicates port flow control options */
    uint8_t xonChar;            /* Indicates the XON character */
    uint8_t xoffChar;           /* Indicates the XOFF character */

    /* Contains a bitmask of settings.
     *
     * When changing port settings with RF_RequestPortSettings, "parmMask"
     * indicates which port settings are being changed.
     *
     * During an RFEVENT_PORT_NEG_IND event, "parmMask" describes which
     * parameters the remote device sent.
     */
    rf_port_settings_mask parmMask;

} rf_port_settings;

struct rf_callback_prams {
    rf_event_t event;           /* Type of the RFCOMM event */
    int status;                 /* Status or error information */
    uint16_t data_len;          /* Length of data (provided during RFEVENT_DATA_IND
                                 * events).
                                 */

    /* Group: The event type determines whether a field in this
     * union is valid.
     */
    union {

        /* During an RFEVENT_PACKET_HANDLED event, contains the previously
         * transmitted packet.
         */
        btif_bt_packet_t *packet;

        /* During an RFEVENT_DATA_IND event, contains the data received from the
         * remote device.
         */
        U8 *data;

        /* During an RFEVENT_MODEM_STATUS_IND event, contains modem status settings.
         */
        struct rf_modem_status *modem_status;

        /* During an RFEVENT_LINE_STATUS_IND event, contains line status settings.
         */
        rf_line_status *line_status;

        /* During an RFEVENT_PORT_NEG_IND, RFEVENT_PORT_NEG_CNF, or
         * RFEVENT_PORT_STATUS_CNF event, contains port settings.
         */
        rf_port_settings *port_settings;

        /* During an RFEVENT_OPEN event, contains the remote device structure */
        btif_remote_device_t *remDev;

    } ptrs;
};

typedef void (*rf_callback_func) (rf_channel_t chan, struct rf_callback_prams * parms);

struct rf_chan_info {
    rf_callback_func callback;
    uint16_t max_frame_size;
    uint8_t priority;
    void *priv;
};

struct rf_service {
    /* Identifies the ID that corresponds to this service. This ID value
     * can be set to 0 before this structure is used if the desired RFCOMM
     * Server ID not known.  If it is set to 0, then, after the service has been
     * registered with RFCOMM, it will contain a valid  RFCOMM Server ID.  If
     * the desired RFCOMM Server ID is known, then this ID value can be set to
     * that value before registering with RFCOMM.  In either case, make sure to
     * register this ID with SDP so that remote devices can locate this RFCOMM
     * service. This ID can also be used as the channel in * BtSecurityRecords
     * (for incoming connections only).
     */
    uint8_t serviceId;
};

#ifdef __cplusplus
extern "C" {
#endif                          /*  */

    rf_channel_t btif_rf_create_channel(void);

    void btif_rf_free_channel(rf_channel_t chan_h);

    bt_status_t btif_rf_register_server_channel(rf_channel_t chan_h,
                                                struct rf_service *service, uint8_t credit);

/*---------------------------------------------------------------------------
 *            RF_DeregisterServerChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister a channel from a service.
 *
 */
    bt_status_t btif_rf_deregister_server_channel(rf_channel_t chan_h, struct rf_service *service);

/*---------------------------------------------------------------------------
 *            RF_DeregisterService()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister a service from RFCOMM.
 *
 * Return:    <See rfcomm.h>
 */
    bt_status_t btif_rf_deregister_service(struct rf_service *service);

/*---------------------------------------------------------------------------
 *        btif_rf_frame_size
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum size allowed for a transmit packet.
 *
 * Return:    Maxmum number of bytes for a transmit packet.
 */
    uint16_t btif_rf_frame_size(rf_channel_t chan_h);

/*---------------------------------------------------------------------------
 *           btif_rf_respond_channel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept or reject a Channel connect request between two devices.
 *
 * Return:    <See rfcomm_api.h>
 */
    bt_status_t btif_rf_respond_channel(rf_channel_t chan_h, bool accept);

/*---------------------------------------------------------------------------
 * btif_rf_accept_channel()
 *
 *     Accepts an incoming request in response to an RFEVENT_OPEN_IND event.
 *     This event occurs when a remote client attempts to connect to a
 *     local RFCOMM server channel. Either this function or btif_rf_respond_channel
 *     must be used to respond to the connection request.
 *
 * Parameters:
 *     channel - Identifies the channel to be accepted. This channel is
 *         provided to the callback function as a parameter during the
 *         RFEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STS_PENDING - The accept message will be sent. The application
 *         will receive a RFEVENT_OPEN when the accept message has been sent
 *         and the channel is open.
 *
 *     BT_STS_FAILED - The specified channel did not have a pending
 *         connection request or the stack has run out of resources.
 *
 *     BT_STS_NO_CONNECTION - No L2CAP connection exists.
 *
 *     BT_STS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
    bt_status_t btif_rf_accept_channel(rf_channel_t chan_h);
#define btif_rf_accept_channel(c) btif_rf_respond_channel(c, true)

/*---------------------------------------------------------------------------
 *  btif_rf_reject_channel()
 *
 *     Rejects an incoming request in response to an RFEVENT_OPEN_IND event.
 *     This event occurs when a remote client attempts to connect to a
 *     local RFCOMM server channel. Either this function or btif_rf_accept_channel
 *     must be used to respond to the connection request.
 *
 * Parameters:
 *     channel - Identifies the channel to be rejected. This channel is
 *         provided to the callback function as a parameter during the
 *         RFEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STS_PENDING - The rejection message has been sent. RFCOMM
 *         will provide an RFEVENT_CLOSED event when the rejection is complete.
 *
 *     BT_STS_FAILED - The specified channel did not have a pending
 *         connection request or the stack has run out of resources.
 *
 *     BT_STS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
    bt_status_t btif_rf_reject_channel(rf_channel_t chan_h);
#define btif_rf_reject_channel(c) btif_rf_respond_channel(c, false)

#if BTIF_RF_SEND_CONTROL == BTIF_ENABLED
    bt_status_t btif_rf_send_port_settings(rf_channel_t chan_h, rf_port_settings * PortSettings);

/*---------------------------------------------------------------------------
 * btif_rf_request_port_settings()
 *
 *     Requests a change to the current port settings. The caller may set
 *     only a subset of the fields in the "portSettings" parameter. The
 *     "parmMask" field of the structure identifies which fields are
 *     important.
 *
 *     After a successful call to this function, RFCOMM exchanges
 *     the parameters with the remote device on the channel. After the
 *     remote device responds, RFCOMM generates an RFEVENT_PORT_NEG_CNF event
 *     to indicate which settings were accepted or rejected. Bits in
 *     the "parmMask" field indicate whether the parameter was accepted
 *     (bit set) or rejected (bit clear).
 *
 *     If the remote device rejects some of the parameters, subsequent
 *     requests can be sent with modified parameters until a final
 *     agreement is reached.
 *
 * Requires:
 *     BTIF_RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 *     portSettings - A pointer to an RfPortSettings structure, initialized
 *         with the desired port settings. This structure is owned by
 *         RFCOMM until either the RFEVENT_PORT_NEG_CNF or RFEVENT_CLOSED event
 *         is received.
 *
 * Returns:
 *     BT_STS_PENDING - The port negotiation will be sent to the
 *         remote device.  If the remote device responds to the request,
 *         RFCOMM sends an RFEVENT_PORT_NEG_CNF event. If the operation times
 *         out, RFCOMM sends an RFEVENT_CLOSED event.
 *
 *     BT_STS_FAILED - The channel is not open.
 *
 *     BT_STS_IN_PROGRESS -  An RFCOMM control channel request is already
 *         outstanding for this channel.  Wait for the status event associated
 *         with the request before sending a new request. The functions that
 *         make control channel requests are:  RF_SetModemStatus,
 *         RF_SetLineStatus, btif_rf_request_port_settings, btif_rf_request_port_status,
 *         and RF_SendTest.
 *
 *     BT_STS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
    bt_status_t btif_rf_request_port_settings(rf_channel_t chan_h, rf_port_settings * portSettings);
#define btif_rf_request_port_settings(c, p) btif_rf_send_port_settings(c, p)

/*---------------------------------------------------------------------------
 * btif_rf_request_port_status()
 *
 *     Requests the status of the port settings for the remote device.
 *
 *     After a successful call to this function, the remote device responds
 *     with its current port settings.
 *
 * Requires:
 *     BTIF_RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 * Returns:
 *     BT_STS_PENDING - The request will be sent to the remote device.
 *         If the remote device responds to the request, RFCOMM sends an
 *         RFEVENT_PORT_STATUS_CNF event. If the operation times out, RFCOMM
 *         sends an RFEVENT_CLOSED event.
 *
 *     BT_STS_FAILED - The channel is not open.
 *
 *     BT_STS_IN_PROGRESS -  An RFCOMM control channel request is already
 *         outstanding for this channel.  Wait for the status event associated
 *         with the request before sending a new request. The functions that
 *         make control channel requests are:  RF_SetModemStatus,
 *         RF_SetLineStatus, btif_rf_request_port_settings, btif_rf_request_port_status,
 *         and RF_SendTest.
 *
 *     BT_STS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
    bt_status_t btif_rf_request_port_status(rf_channel_t chan_h);
#define btif_rf_request_port_status(c) btif_rf_send_port_settings(c, NULL)

    bt_status_t btif_rf_set_modem_status(rf_channel_t chan_h,
                                        struct rf_modem_status *modem_status);
#endif                          /*BTIF_RF_SEND_CONTROL == BTIF_ENABLED */

/*---------------------------------------------------------------------------
 * RF_OpenClientChannel()
 *
 *     Attempts to establish a channel with a remote service. The RFCOMM
 *     user must identify the remote service by its device and remote
 *     server ID.
 *
 * Parameters:
 *     remDev - Identifies the remote device.
 *
 *     serviceId - Identifies the RFCOMM server ID on the remote device.
 *         The server ID can be determined by querying the remote device's
 *         SDP server.
 *
 *     channel - Provides information about the client channel. The
 *         "callback", "maxFrameSize", and "priority" fields must
 *         be initialized prior to the call. This structure is owned by
 *         RFCOMM until the channel is closed.
 *
 *     credit - This parameter is used to specify an initial amount of credit
 *         that will be granted to a client upon connection.  The amount of
 *         credit that is advanced tells the client how many RFCOMM packets it
 *         can send before flow control is shut off from client to server.
 *         In order to allow the client to send more data, RF_AdvanceCredit must
 *         be called.  If the remote device does not support credit-based flow
 *         control, then the flow control is controlled automatically by the FC
 *         bit in the modem status command.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STS_PENDING - The request to open the channel was sent.
 *         If the remote device accepts the request, RFCOMM will
 *         generate an RFEVENT_OPEN event. If the channel is rejected, RFCOMM
 *         will generate an RFEVENT_CLOSED event.
 *
 *     BT_STS_FAILED - The L2CAP connection is not ready for RFCOMM
 *         data
 *
 *     BT_STS_NO_RESOURCES - The system has run out of resources
 *         and cannot open the channel.  A channel must be closed before a
 *         new channel can be opened.
 *
 *     BT_STS_RESTRICTED - The channel failed a security check.
 *
 *     BT_STS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
    bt_status_t btif_rf_open_client_channel(btif_remote_device_t * RemDev,
                                            uint8_t server_id, rf_channel_t chan_h, uint8_t credit);

    bt_status_t btif_rf_close_channel(rf_channel_t chan_h);

    bt_status_t btif_rf_send_data(rf_channel_t chan_h, btif_bt_packet_t * Packet);

    bt_status_t btif_rf_advance_credit(rf_channel_t chan_h, uint8_t credit);

    bool btif_rf_credit_flow_enabled(rf_channel_t chan_h);

    bt_status_t btif_rf_setup_channel(rf_channel_t chan_h, struct rf_chan_info *info);

    void *btif_rf_get_channel_priv(rf_channel_t chan);
    uint32_t btif_rf_get_app_id(rf_channel_t chan_h);

    /*---------------------------------------------------------------------------
     *        btif_rf_max_frame_size_configure
     *---------------------------------------------------------------------------
     *
     * Synopsis:  Returns the maximum frame size configed by system
     *
     * Return:
     */
    uint16_t btif_rf_max_frame_size_configure(void);
    void btif_rf_set_app_id(rf_channel_t chan_h,uint32_t app_id);

#if defined(IBRT)
#if defined(ENHANCED_STACK)
	uint32_t btif_save_rfc_ctx(uint8_t *ctx_buffer, uint8_t MuxId);
	uint32_t btif_set_rfc_ctx(uint8_t* ctx_buffer, uint16_t MuxId, uint8_t rm_devtbl_idx);
#else
	uint32_t btif_save_rfc_ctx(uint8_t *ctx_buffer, uint8_t MuxId);
	uint32_t btif_set_rfc_ctx(uint8_t* ctx_buffer, uint16_t MuxId, uint8_t rm_devtbl_idx);
    uint32_t btif_save_rfcomm_channel_ctx(uint8_t *ctx_buffer, uint8_t MuxId, uint32_t app_id);
    uint32_t btif_set_rfcomm_channel_ctx(uint8_t *ctx_buffer, uint8_t MuxId, uint32_t app_id);
    bool btif_rf_is_rfcomm_channel_connected(uint8_t mux_id,uint32_t app_id);	
#endif	
	btif_remote_device_t*  btif_rfc_get_mux_remote_device(uint8_t mux_idx);
    uint8_t btif_rfc_get_mux_index(uint16_t hci_handle);
    uint8_t btif_rfc_get_mux_state(uint8_t mux_idx);
#endif

#ifdef __cplusplus
}
#endif                          /*  */
#endif /*__RFCOMM_API_H__*/

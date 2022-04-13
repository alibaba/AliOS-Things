/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __SPP_API_H__
#define __SPP_API_H__

#include "sdp_api.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "rfcomm_api.h"

#define spp_malloc malloc
#define spp_free free

#define SPP_DEVICE_NUM      6
#define SPP_SERVICE_NUM     6

/*---------------------------------------------------------------------------
 * Serial Port Profile (SPP) layer
 *
 *     The Serial Port Profile (SPP) specification defines procedures
 *     and protocols for Bluetooth devices emulating RS232 or other serial
 *     connections.
 */


/****************************************************************************
 *
 * Section: Configuration Constants
 *
 * The following defines are configuration constants that allow
 * an implementer to include/exclude functionality from SPP.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BTIF_SPP_SERVER constant
 *      Configuration constant to enable code for Serial Port Profile
 *      server. If the device is client-only, this should be defined as
 *      BTIF_DISABLED in overide.h.
 */
#ifndef BTIF_SPP_SERVER
#define BTIF_SPP_SERVER      BTIF_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BTIF_SPP_CLIENT constant
 *      Configuration constant to enable code for Serial Port Profile
 *      client. If the device is server-only, this should be defined as
 *      BTIF_DISABLED in overide.h.
 */
#ifndef BTIF_SPP_CLIENT
#define BTIF_SPP_CLIENT      BTIF_DISABLED
#endif

#define SPP_RX_SIGNAL_ID         0x04

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * spp_event_t type
 *
 *     The application is notified of various indications and confirmations
 *     through a callback function.  Depending on the event, different
 *     elements of the spp_callback_parms "spp_callback_parms.p" union will be
 *     valid.
 */
typedef uint16_t spp_event_t;

/** A connection has been established with a remote device.
 *
 *  When this callback is received, the "spp_callback_parms.p.remDev" field
 *  contains a pointer to the remote device context.
 */
#define BTIF_SPP_EVENT_REMDEV_CONNECTED     0

/** A connection has been terminated for a remote device.
 *
 *  When this callback is received, the "spp_callback_parms.p.other" field
 *  contains a 0.
 */
#define BTIF_SPP_EVENT_REMDEV_DISCONNECTED  1

/** The data has been sent out. At this time the tx buffer can be released.
 *  When this callback is received, the "spp_callback_parms.p.other" field
 *  is from data structure SppTxDone_t.
 *
 */
#define BTIF_SPP_EVENT_DATA_SENT            2

/** A request to close a channel was received.
 *
 *  When this callback is received, the "spp_callback_parms.p.other" field
 *  contains a 0.
 */
#define BTIF_SPP_EVENT_REMDEV_DISCONNECTED_IND       3

/** A request to open a channel was received.
 *
 *  When this callback is received, the "spp_callback_parms.p.remDev" field
 *  contains a pointer to the remote device context.
 *
 *  This is an opportunitity for the server to reject the incoming request. To
 *  reject the request modify spp_callback_parms.status to be BT_STATUS_CANCELLED.
 */
#define BTIF_SPP_EVENT_REMDEV_CONNECTED_IND          4

/* End of spp_event_t */


/*---------------------------------------------------------------------------
 * spp_port_t type
 *
 *     Identifies the serial port as a client (outgoing) or server (incoming).
 */
typedef uint8_t spp_port_t;

#define BTIF_SPP_SERVER_PORT             0x01
#define BTIF_SPP_CLIENT_PORT             0x02

struct spp_device;

struct spp_tx_done {
    uint8_t         *tx_buf;
    uint16_t        tx_data_length;
};

/* End of spp_port_t */

#if BTIF_SPP_SERVER == BTIF_ENABLED
/*---------------------------------------------------------------------------
 * struct spp_service structure
 *
 *      Servers need this data type for registering with RFCOMM. A particular
 *      service may have one or more ports available. For example, a device
 *      might provide access to 2 modems. It is providing a single service
 *      (modem) via 2 serial ports.
 */
struct spp_service {
    struct rf_service   rf_service;     /* Required for registering service w/RFCOMM */
    const uint8_t       *name;          /* name of service */
    uint16_t            nameLen;        /* Length of name */
    uint16_t            numPorts;       /* Number of ports in this service */
    btif_sdp_record_t   *sdpRecord;     /* Description of the service */
    uint8_t             service_used_flag;
};

#endif /* BTIF_SPP_SERVER == BTIF_ENABLED */

#if BTIF_SPP_CLIENT == BTIF_ENABLED
/*---------------------------------------------------------------------------
 * spp_client structure
 *      Contains all fields unique to clients. spp_client is a data type in
 *      device structures (struct _spp_dev) for clients.
 */
struct spp_client {
    /* === Internal use only === */
    btif_remote_device_t    *remDev;
    uint8_t                 serverId;
    btif_sdp_query_token_t    *sdpQuery;
    uint8_t *rfcommServiceSearchRequestPtr;
    uint8_t rfcommServiceSearchRequestLen;
};
#endif


/*---------------------------------------------------------------------------
 * spp_callback_parms structure
 *
 * A pointer to this structure is sent to the application's callback function
 * notifying the application of state changes or important events.
 */
struct spp_callback_parms {
    spp_event_t            event;   /* Event associated with the callback */

    int status;  /* Status of the callback event       */

    /* For certain events, a single member of this union will be valid.
     * See spp_event_t documentation for more information.
     */
    union {
        void           *other;
        btif_remote_device_t *remDev;
    } p;
};

/*---------------------------------------------------------------------------
 * spp_callback_t type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*spp_callback_t)(struct spp_device *locDev,
                            struct spp_callback_parms *Info);

/* End of spp_callback_t */


/*---------------------------------------------------------------------------
 * _spp_dev structure
 *      This structure defines an SPP device.  Members should not be accessed
 *      directly. and this structure only used internally;
 */
struct _spp_dev {

    /* Fields specific to clients and servers */
    union {

#if BTIF_SPP_CLIENT == BTIF_ENABLED

        struct spp_client   client;

#endif

#if BTIF_SPP_SERVER == BTIF_ENABLED

        struct spp_service  *sppService;

#endif

    } type;

    /* === Internal use only === */
    spp_callback_t      callback;       /* application callback function */

    /* Server / Client elements */
    uint8_t             state;          /* device state */
    rf_channel_t        channel;
    struct rf_modem_status   rModemStatus;   /* remote modem status */
    struct rf_modem_status   lModemStatus;   /* local modem status */
    rf_line_status    lineStatus;
    rf_port_settings  portSettings;

    uint8_t             xonChar;
    uint8_t             xoffChar;
    int16_t             highWater;      /* when rRing.dataLen >= highWater,
                                       flow off rx */
    uint8_t             credit;         /* rx credits outstanding */
    uint8_t             maxCredits;     /* maximum rx credits to issue */
    uint8_t             msr;            /* Modem Status Register */
    int16_t             breakLen;

    /* Transmit packet */
    list_entry_t        freeTxPacketList;

    list_entry_t        pendingTxPacketList;
};

typedef int (*spp_handle_data_event_func_t)(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);

struct spp_device {
    spp_port_t      portType;       /* SPP_SERVER_PORT or SPP_CLIENT_PORT */
    uint32_t        app_id;         /* spp application id,used for identify spp application*/
    osThreadId      reader_thread_id;
    osMutexId       mutex_id;
    CQueue          rx_queue;
    uint8_t         *rx_buffer;
    uint32_t         rx_buffer_size;
    uint8_t         *tx_packets;
    struct _spp_dev sppDev;
    void            *priv;   /*used by the application code for context use*/
    spp_handle_data_event_func_t    spp_handle_data_event_func;
    uint8_t         serial_number;
    uint8_t         spp_connected_flag;
    uint8_t         spp_used_flag;
};

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 *
 * Section: Driver Initialization Functions
 *
 * These APIs are called typically when the driver is initialized or loaded.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * btif_spp_init_device()
 *
 *      Initializes a Serial Port Profile device structure. Call this API
 *      for each SPP device before using it for any other SPP API. In most
 *      cases this would be during driver initialization.
 *
 * Parameters:
 *      dev -   pointer to existing device structure
 *      numPackets - number of packets pointed to by 'packets'. This number
 *          can be 1 or more. If too few packets are provided, SPP may
 *          run out and be unable to queue more data to RFCOMM until packets
 *          in use are returned to SPP.
 *      mid-  the pointer of a mutex
 */
void btif_spp_init_device(struct spp_device *osDev, uint16_t numPackets, osMutexId mid);

/****************************************************************************
 *
 * Section: Serial Port Profile I/O Functions
 *
 * These APIs are provided to upper layers. Normally, they are called
 * as the result of a corresponding call to the system API. For example,
 * in a POSIX interface open() would call spp_open().
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * btif_spp_open()
 *
 *      Opens a serial device for reading and writing. Requires an
 *      existing ACL connection with the remote device for client devices.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      btDevice - for client port this is a pointer to an existing
 *               RemoteDevice structure describing the remote device to
 *               which a service connection will be made.  Server ports
 *               ignore this field.
 *      callback - The callback function that should be invoked to inform the
 *               application of events of interest.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - device opened successfully
 *      BT_STATUS_INVALID_PARM - invalid parameter; usually this is because
 *                               btDevice is not valid or no ACL exists to
 *                               the device
 *      BT_STATUS_FAILED - device is not in a closed state
bt_status_t btif_spp_open(SppDev         *dev,
                  btif_remote_device_t  *btDevice,
                  spp_callback_t     callback);
 */
bt_status_t btif_spp_open(struct spp_device *osDev,
                        btif_remote_device_t  *btDevice,
                        spp_callback_t callback);


bt_status_t btif_spp_disconnect(struct spp_device *osDev_t);

/*---------------------------------------------------------------------------
 * btif_spp_close()
 *
 *     Close the serial device. Requires the device to have been opened
 *     previously by btif_spp_open(). This function does not verify that the
 *     calling thread or process is the same one that opened it. The calling
 *     layer may optionally do any such checks.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - closed successfully
 *      BT_STATUS_FAILED - device was not opened
 */
bt_status_t btif_spp_close(struct spp_device *osDev);

/*---------------------------------------------------------------------------
 * btif_spp_write()
 *      Write to the serial device. Requires the device to have been
 *      successfully opened by btif_spp_open().
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      buffer - buffer containing characters to write
 *      nBytes - on input: pointer to number of bytes in buffer; actual
 *               bytes written are returned in nBytes if it returns success
 *
 * Returns:
 *      BT_STATUS_SUCCESS - write was successful
 *      BT_STATUS_FAILED - device is not open
 */
bt_status_t btif_spp_write(struct spp_device *osDev, char *buffer, uint16_t *nBytes);

/*---------------------------------------------------------------------------
 * btif_spp_ioctl()
 *      Perform an I/O control function on the serial device.
 *      Requires the device to have been successfully opened by the caller.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      function - I/O control function as allowed by OS
 *      arg - argument peculiar to function; many ioctl functions alter
 *            the contents of *arg upon success (this is totally platform
 *            dependent and under the control of SPPOS_Ioctl)
 *
 * Returns:
 *      This function returns the status of SPPOS_Ioctl.
 */
bt_status_t btif_spp_ioctl(struct spp_device *osDev, uint16_t function, void *arg);

/****************************************************************************
 * Section: SPP Lower-layer API's
 *
 * The following APIs are primarily for I/O control operations. That is,
 * the serial driver could call these functions for assistance in carrying
 * out a particular I/O request. For example, SPPOS_Ioctl() might use
 * these functions to obtain information about the device.
 *
 * Error checking (for example, making sure the SppDev is open and in the
 * right state, protecting from race conditions, etc.) is assumed handled
 * prior to making these calls.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * btif_spp_get_data_format()
 *      Retrieve the current data format for the specified device.
 *
 * Parameters:
 *      dev - pointer to existing device structure
 *
 * Returns:
 *      Current data format (data bits, stop bits and parity). The value
 *      returned is a superset of that for each individual setting (data bits,
 *      stop bits and parity - see APIs below).
 */
rf_data_format btif_spp_get_data_format(struct spp_device *dev);
#define btif_spp_get_data_format(dev)        ((dev->sppDev)->portSettings.dataFormat)

/*---------------------------------------------------------------------------
 * btif_spp_get_data_bits()
 *      Get the number of data bits in effect for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of data bits as defined by rf_data_format.
 */

rf_data_format btif_spp_get_data_bits(struct spp_device *dev);
#define btif_spp_get_data_bits(dev)        (rf_data_format)((dev->sppDev)->portSettings.dataFormat \
                                                    & RF_DATA_BITS_MASK)

/*---------------------------------------------------------------------------
 * btif_spp_get_parity()
 *      Get the current parity setting for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current parity setting (see rf_data_format).
 */
rf_data_format btif_spp_get_parity(struct spp_device *dev);
#define btif_spp_get_parity(dev)        (rf_data_format)((dev->sppDev)->portSettings.dataFormat \
                                   & (RF_PARITY_TYPE_MASK | RF_PARITY_MASK))

/*---------------------------------------------------------------------------
 * btif_spp_get_stop_bits()
 *      Retrieve the number of stop bits for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of stop bits as defined by rf_data_format.
 */
rf_data_format btif_spp_get_stop_bits(struct spp_device *dev);
#define btif_spp_get_stop_bits(dev)   (rf_data_format)((dev->sppDev)->portSettings.dataFormat \
                                     & RF_STOP_BITS_MASK)

/*---------------------------------------------------------------------------
 * btif_spp_get_flow_control()
 *      Retrieve the current flow control setting for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Flow control setting (see rf_flow_control).
 */
rf_flow_control btif_spp_get_flow_control(struct spp_device *dev);
#define btif_spp_get_flow_control(dev)   ((dev->sppDev)->portSettings.flowControl)

/*---------------------------------------------------------------------------
 * btif_spp_get_modem_status()
 *      Retrieve the current modem status signals for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Modem status signals (see rf_signals).
 */
rf_signals btif_spp_get_modem_status(struct spp_device *dev);
#define btif_spp_get_modem_status(dev)        ((dev->sppDev)->rModemStatus.signals)


/*---------------------------------------------------------------------------
 * btif_spp_get_baud()
 *      Retrieve the current baud rate setting for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Baud rate (see rf_baud_rate).
 */
rf_baud_rate btif_spp_get_baud(struct spp_device *dev);
#define btif_spp_get_baud(dev)        ((dev->sppDev).portSettings.baudRate)

/*---------------------------------------------------------------------------
 * btif_spp_send_port_settings()
 *      Transmit the port settings (RfPortSettings) to the remote device.
 *      When a port setting changes on the local system, that setting should
 *      be communicated to the remote device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The processing of sending to the remote device
 *               has been started or BTIF_RF_SEND_CONTROL is disabled
 *      BT_STATUS_FAILED
 */
bt_status_t btif_spp_send_port_settings(struct spp_device *osDev_t);

/*---------------------------------------------------------------------------
 * btif_spp_set_modem_control()
 *      Set the modem control signals for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      signals - modem control signals to be set
 *
 * Returns:
 *      The modem control signals are set in the device structure. If
 *      BTIF_RF_SEND_CONTROL is enabled, the settings are sent to the remote
 *      device.
 *
 *      BT_STATUS_SUCCESS - BTIF_RF_SEND_CONTROL is disabled or
 *               BTIF_RF_SEND_CONTROL is enabled and the transmission
 *               was begun.
 *
 *      Other returns - See RF_SetModemStatus().
 */
bt_status_t btif_spp_set_modem_control(struct spp_device *osDev, rf_signals signals);

/*---------------------------------------------------------------------------
 * btif_spp_set_baud()
 *      Set the baud rate for the specified device. Baud rate for the
 *      device is informational only.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      baud -   current baud rate
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'baud'. Note that btif_spp_send_port_settings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to
 *      be set.
 */
void btif_spp_set_baud(struct spp_device *osDev, uint8_t baud);

/*---------------------------------------------------------------------------
 * btif_spp_set_data_format()
 *      Set the data format values for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    data format (see rf_data_format)
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that btif_spp_send_port_settings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to
 *      be set.
 */
void btif_spp_set_data_format(struct spp_device *osDev, uint8_t val);

/*---------------------------------------------------------------------------
 * btif_spp_set_flow_control()
 *      Set the flow control value for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    Flow control value (see rf_flow_control).
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that btif_spp_send_port_settings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to
 *      be set.
 */
void btif_spp_set_flow_control(struct spp_device *osDev, uint8_t val);

void btif_spp_init_rx_buf(struct spp_device *osDev, uint8_t *rx_buf, uint32_t size);

bt_status_t btif_spp_service_setup(struct spp_device *osDev,
                                struct spp_service *service,
                                btif_sdp_record_t *record);

#if BTIF_SPP_CLIENT == BTIF_ENABLED
bt_status_t btif_ccmp_open(struct spp_device *osDev,
                        btif_remote_device_t  *btDevice,
                        spp_callback_t callback,
                        uint8_t port);
#endif

const char *btif_spp_event2str(spp_event_t event);

struct spp_device *btif_create_spp_device(void);
void btif_destroy_spp_device(struct spp_device *osDev_t);
struct spp_device *btif_spp_get_device(uint32_t app_id);
struct spp_service *btif_create_spp_service(void);
void btif_destroy_spp_service(struct spp_service *spp_service_p);
uint32_t btif_spp_get_app_id(struct spp_device *dev);

#if defined(IBRT)
typedef void (*btif_bt_spp_app_callback_func)(void* spp_devi, void* spp_para);
void btif_register_bt_spp_callback_func(btif_bt_spp_app_callback_func func);
#endif

/*---------------------------------------------------------------------------
 * rfcomm channel number
 *      should be from 1 to 30
 */
enum RFCOMM_CHANNEL_NUM {
    RFCOMM_CHANNEL_HFP= 1,
   // don't touch Bisto's rfcomm channel number!!!
    RFCOMM_CHANNEL_GS_CONTROL = 2,
    RFCOMM_CHANNEL_GS_AUDIO = 3,

    RFCOMM_CHANNEL_TOTA,

    RFCOMM_CHANNEL_SMARTVOICE,

    RFCOMM_CHANNEL_BES_OTA,

    RFCOMM_CHANNEL_TENCENT,

    RFCOMM_CHANNEL_AI_VOICE,

    RFCOMM_CHANNEL_DMA,

    RFCOMM_CHANNEL_AMA,

    RFCOMM_CHANNEL_RED,

    RFCOMM_CHANNEL_GREEN,

    RFCOMM_CHANNEL_FP
};

#ifdef __cplusplus
}
#endif

#endif /* __SPP_API_H__ */

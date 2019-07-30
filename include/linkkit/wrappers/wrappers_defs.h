#ifndef _WRAPPERS_DEFS_H_
#define _WRAPPERS_DEFS_H_

#include <stdio.h>
#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"

#define PLATFORM_WAIT_INFINITE (~0)

typedef struct {
    void *(*malloc)(uint32_t size);
    void (*free)(void *ptr);
} ssl_hooks_t;

typedef enum {
    os_thread_priority_idle = -3,        /* priority: idle (lowest) */
    os_thread_priority_low = -2,         /* priority: low */
    os_thread_priority_belowNormal = -1, /* priority: below normal */
    os_thread_priority_normal = 0,       /* priority: normal (default) */
    os_thread_priority_aboveNormal = 1,  /* priority: above normal */
    os_thread_priority_high = 2,         /* priority: high */
    os_thread_priority_realtime = 3,     /* priority: realtime (highest) */
    os_thread_priority_error = 0x84,     /* system cannot determine priority or thread has illegal priority */
} hal_os_thread_priority_t;

typedef struct _hal_os_thread {
    hal_os_thread_priority_t priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    int                   stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                      detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} hal_os_thread_param_t;

#define DTLS_ERROR_BASE                (1<<24)
#define DTLS_SUCCESS                   (0)
#define DTLS_INVALID_PARAM             (DTLS_ERROR_BASE | 1)
#define DTLS_INVALID_CA_CERTIFICATE    (DTLS_ERROR_BASE | 2)
#define DTLS_HANDSHAKE_IN_PROGRESS     (DTLS_ERROR_BASE | 3)
#define DTLS_HANDSHAKE_FAILED          (DTLS_ERROR_BASE | 4)
#define DTLS_FATAL_ALERT_MESSAGE       (DTLS_ERROR_BASE | 5)
#define DTLS_PEER_CLOSE_NOTIFY         (DTLS_ERROR_BASE | 6)
#define DTLS_SESSION_CREATE_FAILED     (DTLS_ERROR_BASE | 7)
#define DTLS_READ_DATA_FAILED          (DTLS_ERROR_BASE | 8)

typedef struct {
    unsigned char             *p_ca_cert_pem;
    char                      *p_host;
    unsigned short             port;
} coap_dtls_options_t;

typedef enum {
    HAL_SEEK_SET,
    HAL_SEEK_CUR,
    HAL_SEEK_END
} hal_fs_seek_type_t;

typedef void DTLSContext;

/* link type */
enum AWSS_LINK_TYPE {
    /* rtos HAL choose this type */
    AWSS_LINK_TYPE_NONE,

    /* linux HAL may choose the following type */
    AWSS_LINK_TYPE_PRISM,
    AWSS_LINK_TYPE_80211_RADIO,
    AWSS_LINK_TYPE_80211_RADIO_AVS,
    AWSS_LINK_TYPE_HT40_CTRL /* for espressif HAL, see struct ht40_ctrl */
};

typedef int (*awss_recv_80211_frame_cb_t)(char *buf, int length,
        enum AWSS_LINK_TYPE link_type, int with_fcs, signed char rssi);

typedef void (*awss_wifi_mgmt_frame_cb_t)(_IN_ uint8_t *buffer, _IN_ int len,
        _IN_ signed char rssi_dbm, _IN_ int buffer_type);

struct HAL_Ht40_Ctrl {
    uint16_t    length;
    uint8_t     filter;
    signed char rssi;
};

/* encryt type */
enum AWSS_ENC_TYPE {
    AWSS_ENC_TYPE_NONE,
    AWSS_ENC_TYPE_WEP,
    AWSS_ENC_TYPE_TKIP,
    AWSS_ENC_TYPE_AES,
    AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_MAX = AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_INVALID = 0xff,
};
/* auth type */
enum AWSS_AUTH_TYPE {
    AWSS_AUTH_TYPE_OPEN,
    AWSS_AUTH_TYPE_SHARED,
    AWSS_AUTH_TYPE_WPAPSK,
    AWSS_AUTH_TYPE_WPA8021X,
    AWSS_AUTH_TYPE_WPA2PSK,
    AWSS_AUTH_TYPE_WPA28021X,
    AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_MAX = AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_INVALID = 0xff,
};

/* 80211 frame type */
typedef enum HAL_Awss_Frame_Type {
    FRAME_ACTION,
    FRAME_BEACON,
    FRAME_PROBE_REQ,
    FRAME_PROBE_RESPONSE,
    FRAME_DATA
} HAL_Awss_Frame_Type_t;

#if defined(AT_PARSER_ENABLED)
/*
 * UART data width
 */
typedef enum {
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT,
    DATA_WIDTH_9BIT
} hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_BITS_1,
    STOP_BITS_2
} hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,
    FLOW_CONTROL_CTS,
    FLOW_CONTROL_RTS,
    FLOW_CONTROL_CTS_RTS
} hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    NO_PARITY,
    ODD_PARITY,
    EVEN_PARITY
} hal_uart_parity_t;

/*
 * UART mode
 */
typedef enum {
    MODE_TX,
    MODE_RX,
    MODE_TX_RX
} hal_uart_mode_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t                baud_rate;
    hal_uart_data_width_t   data_width;
    hal_uart_parity_t       parity;
    hal_uart_stop_bits_t    stop_bits;
    hal_uart_flow_control_t flow_control;
    hal_uart_mode_t         mode;
} at_uart_config_t;

typedef struct {
    uint8_t          port;   /* uart port */
    at_uart_config_t config; /* uart config */
    void             *priv;   /* priv data */
} uart_dev_t;
#endif

#if defined(AT_TCP_ENABLED)
typedef enum {
    /* WiFi */
    TCP_SERVER,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

#if defined(AT_SSL_ENABLED)
typedef enum {
    ROOT_CERT,
    CLIENT_CERT,
    /* add other type */
} CERT_TYPE;

typedef struct {
    int cert_len;
    char *cert_data;
    CERT_TYPE cert_type;
} cert_info_t;
#endif

/* Fill necessary fileds according to the socket type. */
typedef struct {
    int fd; /* fd that are used in socket level */
    CONN_TYPE type;
    char *addr; /* remote ip or domain */
    int32_t r_port; /* remote port (set to -1 if not used) */
    int32_t l_port; /* local port (set to -1 if not used) */
    uint32_t tcp_keep_alive; /* tcp keep alive value (set to 0 if not used) */
    void *param;
} at_conn_t;

struct at_conn_input {
    int        fd;
    void      *data;
    uint32_t   datalen;
    char      *remote_ip;
    uint16_t   remote_port;
};

#endif

#endif


#ifndef _ATHOST_H_
#define _ATHOST_H_

typedef enum {
    /* WiFi */
    TCP_SERVER = 0,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,

    CONN_TYPE_NUM
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

typedef int (*at_data_check_cb_t)(char data);
typedef void (*recv_task_t)(void *arg);

typedef struct link_s {
    int fd;
    int linkid;
    CONN_TYPE type;

    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;


/* Structure for registering at CLI commands */
struct at_cli_command {
    const char *name;
    const char *help;

    int (*function)(void);
};

enum {
    ATCMD_FAIL = 0,
    ATCMD_SUCCESS = 1,

    CIP_STATUS_CLOSED = 0,
    CIP_STATUS_CONNECTED = 1,

    WEVENT_STATUS_UP = 0,
    WEVENT_STATUS_DOWN = 1,
};

enum {
    AP = 0,
    STA = 1,
};

#endif

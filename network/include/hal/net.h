/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_NET_H
#define HAL_NET_H

#include <hal/base.h>

typedef struct hal_net_module_s hal_net_module_t;

typedef struct {
    uint8_t dhcp;     /* DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server. */
    char    ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    dns[16];  /* DNS server IP address. */
    char    mac[16];  /* MAC address, example: "C89346112233". */
    char    broadcastip[16];
} hal_ip_stat_t;

/*
 * The event call back function called at specific events occurred.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef struct {
    void (*connect_fail)(hal_net_module_t *m, int err, void *arg);
    void (*ip_got)(hal_net_module_t *m, hal_ip_stat_t *pnet, void *arg);
    void (*disconnected)(hal_net_module_t *m, void *arg);
    void (*fatal_err)(hal_net_module_t *m, void *arg);
} hal_net_event_cb_t;

struct hal_net_module_s {
    hal_module_base_t base;
    const hal_net_event_cb_t *ev_cb;

    int  (*init)(hal_net_module_t *m);
    void (*get_mac_addr)(hal_net_module_t *m, uint8_t *mac);
};

/**
 * Get the default net instance.
 * The system may have more than one net instance,
 * this API returns the default one.
 *
 * @return  Instance pointer, or NULL if no instance registered.
 */
hal_net_module_t *hal_net_get_default_module(void);

/**
 * Regster a net instance to the HAL framework.
 *
 * @param[in]  m  the net instance.
 */
void hal_net_register_module(hal_net_module_t *m);

/**
 * Initialize net instances.
 *
 * @note  This is supposed to be called during system boot,
 *        not supposed to be called by user module directly.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_net_init(void);

/**
 * Get the MAC address of the specified net instance.
 *
 * @param[in]   m    the net instance, NULL if default.
 * @param[out]  mac  the place to hold the result.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_net_get_mac_addr(hal_net_module_t *m, uint8_t *mac);

#endif  /* HAL_NET_H */

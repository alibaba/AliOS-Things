/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_CELLULAR_H
#define HAL_CELLULAR_H

#include "network/hal/base.h"

typedef struct hal_cellular_module_s hal_cellular_module_t;

typedef struct {
    char ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char dns[16];  /* DNS server IP address. */
} hal_cellular_ip_stat_t;

/*
 * The event call back function called at specific events occurred.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef struct {
    void (*connect_fail)(hal_cellular_module_t *m, int err, void *arg);
    void (*ip_got)(hal_cellular_module_t *m, hal_cellular_ip_stat_t *pnet, void *arg);
    void (*disconnected)(hal_cellular_module_t *m, void *arg);
    void (*fatal_err)(hal_cellular_module_t *m, void *arg);
} hal_cellular_event_cb_t;

struct hal_cellular_module_s {
    hal_module_base_t base;

    const hal_cellular_event_cb_t *ev_cb;

    int (*init)(hal_cellular_module_t *m);
};

/*  @addtogroup wds_qmi_enums
 *  @{
 */
typedef enum {
  PDP_TYPE_IPV4   = 0x00, /* *<  PDP-IP (IPv4) \n */
  PDP_TYPE_PPP    = 0x01, /* *<  PDP-PPP \n */
  PDP_TYPE_IPV6   = 0x02, /* *<  PDP-IPv6 \n */
  PDP_TYPE_IPV4V6 = 0x03, /* *<  PDP-IPv4 and IPv6 \n */
  PDP_TYPE_NONIP  = 0x04, /* *<  PDP-NON IP */
  PDP_TYPE_MAX
} pdp_type_t;

#define APN_NAME_MAX 150

/* details about PDP activate message */
typedef struct {
    char       apn[APN_NAME_MAX + 1]; /* APN name */
    pdp_type_t type;
} hal_cellular_init_type_t;

/**
 * Get the default cellular instance.
 * The system may have more than one cellular instance,
 * this API returns the default one.
 *
 * @return  Instance pointer, or NULL if no instance registered.
 */
hal_cellular_module_t *hal_cellular_get_default_module(void);

/**
 * Regster a cellular instance to the HAL framework.
 *
 * @param[in]  m  the cellular instance.
 */
void hal_cellular_register_module(hal_cellular_module_t *m);

/**
 * Initialize cellular instances.
 *
 * @note  This is supposed to be called during system boot,
 *        not supposed to be called by user module directly.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_cellular_init(void);

/**
 * Start the cellular instance.
 *
 * @param[in]  m          the cellular instance, NULL if default.
 * @param[in]  init_para  the config used to start the cellular.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_cellular_start(hal_cellular_module_t *m, hal_cellular_init_type_t *init_para);

/**
 * Get the status of the specified cellular instance, e.g. the IP, mask etc.
 *
 * @param[in]   m             the cellular instance, NULL if default.
 * @param[out]  out_net_para  the place to hold the results.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_cellular_get_ip_stat(hal_cellular_module_t *m, hal_cellular_ip_stat_t *out_net_para);

/**
 * Set the event callback function array for the cellular.
 * Please don't do time consuming work in these callbacks.
 *
 * @note Please don't do time consuming work in these callbacks.
 *
 * @param[in]  m   the cellular instance, NULL for default.
 * @param[in]  cb  the event callback function info.
 */
void hal_cellular_install_event(hal_cellular_module_t *m, const hal_cellular_event_cb_t *cb);

#endif /* HAL_CELLULAR_H */


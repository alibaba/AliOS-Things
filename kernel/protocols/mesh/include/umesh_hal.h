/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_HAL_H
#define UMESH_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "umesh_types.h"

struct umesh_hal_module_s;

/**
 * Callback function when ucast frame send done
 *
 * @param[out] context The context
 * @param[in]  frame   The frame that has been sent
 * @param[out] error   The error code for this sent frame
 */
typedef void (*umesh_handle_sent_ucast_t)(void *context,
                                          frame_t *frame, int error);

/**
 * Callback function when bcast frame send done
 *
 * @param[out] context The context
 * @param[in]  frame   The frame that has been sent
 * @param[out] error   The error code for this sent frame
 */
typedef void (*umesh_handle_sent_bcast_t)(void *context,
                                          frame_t *frame, int error);

/**
 * Callback function when frame received
 *
 * @param[out] context The context
 * @param[in]  frame      The received frame
 * @param[out] frame_info The frame info that needs by uppder layer
 * @param[out] error      The error code for this received data frame
 */
typedef void (*umesh_handle_received_frame_t)(void *context, frame_t *frame,
                                              frame_info_t *frame_info,
                                              int error);

/**
 * Callback function when received commands
 *
 * @param[in] buf    The received commands
 * @param[in] length The commands length
 */
typedef void (*umesh_cli_input_t)(const uint8_t *buf, uint16_t length);

typedef struct umesh_hal_module_s {
    struct {
        dlist_t list;
        int magic;
        const char *name;
        void *priv_dev; /* Driver may want to describe it */
    } base;
    media_type_t type;
    umesh_handle_received_frame_t receiver;

    int (*umesh_hal_init)(struct umesh_hal_module_s *module, void *config);
    int (*umesh_hal_enable)(struct umesh_hal_module_s *module);
    int (*umesh_hal_disable)(struct umesh_hal_module_s *module);

    /* send ucast frame */
    int (*umesh_hal_send_ucast_request)(struct umesh_hal_module_s *module,
                                        frame_t *frame, mac_address_t *dest,
                                        umesh_handle_sent_ucast_t sent,
                                        void *context);
    /* send bcast frame */
    int (*umesh_hal_send_bcast_request)(struct umesh_hal_module_s *module,
                                        frame_t *frame,
                                        umesh_handle_sent_bcast_t sent,
                                        void *context);

    /* register call back when received packet */
    int (*umesh_hal_register_receiver)(struct umesh_hal_module_s *module,
                                       umesh_handle_received_frame_t received, void *context);

    /* request low layer to transmit beacons intervally*/
    int (*umesh_hal_get_bcast_mtu)(struct umesh_hal_module_s *module);
    int (*umesh_hal_get_ucast_mtu)(struct umesh_hal_module_s *module);

    int (*umesh_hal_set_channel)(struct umesh_hal_module_s *module, uint8_t channel);
    int (*umesh_hal_get_channel)(struct umesh_hal_module_s *module);
    int (*umesh_hal_get_chnlist)(struct umesh_hal_module_s *module, const uint8_t **chnlist);

    int (*umesh_hal_set_txpower)(struct umesh_hal_module_s *module, int8_t txpower);
    int (*umesh_hal_get_txpower)(struct umesh_hal_module_s *module);

    int (*umesh_hal_set_extnetid)(struct umesh_hal_module_s *module,
                                  const umesh_extnetid_t *extnetid);
    void (*umesh_hal_get_extnetid)(struct umesh_hal_module_s *module,
                                   umesh_extnetid_t *extnetid);
    const mac_address_t *(*umesh_hal_get_mac_address)(
        struct umesh_hal_module_s *module);

    int (*umesh_hal_radio_wakeup)(struct umesh_hal_module_s *module);
    int (*umesh_hal_radio_sleep)(struct umesh_hal_module_s *module);

    const frame_stats_t *(*umesh_hal_get_stats)(struct umesh_hal_module_s *module);
} umesh_hal_module_t;

/**
 * Initialize all registed HAL modules.
 *
 * @return
 *     Initalization result, 0 if success, nonzero if fail
 */
int hal_umesh_init(void);

/**
 * Get the defaut umesh HAL
 *
 * The system may have more than 1 mesh HAL instances.
 *
 * @return
 *     Instance pointer or NULL
 */
umesh_hal_module_t *hal_umesh_get_default_module(void);

/**
 * Get the next umesh HAL
 *
 * The system may have more than 1 mesh HAL instances.
 *
 * @return
 *     Instance pointer or NULL
 */
umesh_hal_module_t *hal_umesh_get_next_module(umesh_hal_module_t *cur);

/**
 * Register one or more mesh instances to HAL.
 *
 * @param[in] module The HAL module to be registered
 */
void hal_umesh_register_module(umesh_hal_module_t *module);

/**
 * Enable a umesh HAL module, which usually powers on its hardware
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     Enable result, 0 if success, -1 if fail
 */
int hal_umesh_enable(umesh_hal_module_t *module);

/**
 * Disable a umesh HAL module, which usually power off its hardware
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     Disable result, 0 if success, -1 if fail
 */
int hal_umesh_disable(umesh_hal_module_t *module);

/**
 * Send HAL ucast frame request
 *
 * @param[in] module  The HAL module to send data to; if NULL, the default module will be used
 * @param[in] frame   The frame buffer that contains the data
 * @param[in] dest    The destination of this frame.
 * @param[in] sent    The callback function to be called after hardware send data finish
 * @param[in] context The context
 *
 * @return
 *     Send frame request result, 0 if success, -1 if fail, -2 if drop
 */
int hal_umesh_send_ucast_request(umesh_hal_module_t *module,
                                 frame_t *frame, mac_address_t *dest,
                                 umesh_handle_sent_ucast_t sent, void *context);

/**
 * Send HAL bcast frame request
 *
 * @param[in] module  The HAL module to send data to; if NULL, the default module will be used
 * @param[in] frame   The frame buffer that contains the data
 * @param[in] sent    The callback function to be called after hardware send data finish
 * @param[in] context The context
 *
 * @return
 *     Send frame request result, 0 if success, -1 if fail, -2 if drop
 */
int hal_umesh_send_bcast_request(umesh_hal_module_t *module,
                                 frame_t *frame,
                                 umesh_handle_sent_bcast_t sent, void *context);

/**
 * Register data frame receiver callback function
 *
 * @param[in] module   The HAL module to receive data from; if NULL, the default module will be used
 * @param[in] received The callback function to be called after hardware received data
 * @param[in] context  The context
 *
 * @return
 *     Register receiver result, 0 if success, -1 if fail
 */
int hal_umesh_register_receiver(umesh_hal_module_t *module,
                                umesh_handle_received_frame_t received, void *context);

/**
 * Request HAL to send beacons
 *
 * @param[in] module       The HAL module to send data to; if NULL, the default module will be used
 * @param[in] frame        The frame buffer that contains the data and config information
 * @param[in] dest         The destination of this frame.
 * @param[in] max_interval The max interval that low layer needs to transmit beacon
 *
 * @return
 *     The result, 0 if success, -1 if fail
 */
int hal_umesh_start_beacons(umesh_hal_module_t *module,
                            frame_t *frame, mac_address_t *dest,
                            uint16_t max_interval);

/**
 * Stop HAL to send beacons
 *
 * @param[in] module The HAL module to send data to; if NULL, the default module will be used
 *
 * @return
 *     The result, 0 if success, -1 if fail
 */
int hal_umesh_stop_beacons(umesh_hal_module_t *module);

/**
 * Set HAL broadcast MTU, MTU is normally decided by HAL.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] mtu    The MTU to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_bcast_mtu(umesh_hal_module_t *module, uint16_t mtu);

/**
 * Get HAL broadcast MTU
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The MTU, -1 if fail
 */
int hal_umesh_get_bcast_mtu(umesh_hal_module_t *module);

/**
 * Set HAL unicast MTU, MTU is normally decided by HAL.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] mtu    The MTU to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_ucast_mtu(umesh_hal_module_t *module, uint16_t mtu);

/**
 * Get HAL unicast MTU
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The MTU, -1 if fail
 */
int hal_umesh_get_ucast_mtu(umesh_hal_module_t *module);

/**
 * Set channel
 *
 * @param[in] module  The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] channel The channel to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_channel(umesh_hal_module_t *module, uint8_t channel);

/**
 * Get channel
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The channel, -1 if fail
 */
int hal_umesh_get_channel(umesh_hal_module_t *module);

/**
 * Get channel list
 *
 * @param[in] module  The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] chnlist Pointer to store the unicast channel list
 *
 * @return
 *     The number of unicast channels, -1 if fail
 */
int hal_umesh_get_chnlist(umesh_hal_module_t *module, const uint8_t **chnlist);

/**
 * Set transmit power
 *
 * @param[in] module  The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] txpower The transmit power to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_txpower(umesh_hal_module_t *module, int8_t txpower);

/**
 * Get transmit power
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The transmit power, -1 if fail
 */
int hal_umesh_get_txpower(umesh_hal_module_t *module);

/**
 * Set extension meshnetid
 *
 * @param[in] module   The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] extnetid The extension meshnetid to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_extnetid(umesh_hal_module_t *module,
                           const umesh_extnetid_t *extnetid);

/**
 * Get extension meshnetid
 *
 * @param[in] module   The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] extnetid The extension meshnetid to be get
 *
 */
void hal_umesh_get_extnetid(umesh_hal_module_t *module,
                            umesh_extnetid_t *extnetid);

/**
 * Set HAL mac address.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 * @param[in] addr   The mac address to be set
 *
 * @return
 *     Set media configuration result, 0 if success, -1 if fail
 */
int hal_umesh_set_mac_address(umesh_hal_module_t *module,
                              const mac_address_t *addr);

/**
 * Get HAL mac address.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The mac address, NULL if fail
 */
const mac_address_t *hal_umesh_get_mac_address(umesh_hal_module_t *module);

/**
 * Put umesh radio in wakeup state.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     Set result, 0 if success, -1 if fail
 */
int hal_umesh_radio_wakeup(umesh_hal_module_t *module);

/**
 * Sleep umesh radio sleep state.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     Set result, 0 if success, -1 if fail
 */
int hal_umesh_radio_sleep(umesh_hal_module_t *module);

/**
 * Read umesh HAL frame stats.
 *
 * @param[in] module The HAL module to be operated; if NULL, the default module will be operated
 *
 * @return
 *     The HAL frame stats, NULL if fail
 */
const frame_stats_t *hal_umesh_get_stats(umesh_hal_module_t *module);

#ifdef __cplusplus
}
#endif

#endif /* UMESH_HAL_H */

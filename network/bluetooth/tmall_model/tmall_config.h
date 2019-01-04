/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_PROFILE_CONFIG_H_
#define BT_MESH_PROFILE_CONFIG_H_

/**
 * @def CONFIG_MESH_ELEM_NUM
 *
 * The specific supported number of elements in product.
 *
 */
#define CONFIG_MESH_ELEM_NUM         1

/**
 * @def CONFIG_MESH_MODLE_NUM
 *
 * The specific supported number of models in product.
 *
 */
#define CONFIG_MESH_MODEL_NUM        6

/**
 * @def CONFIG_CID_TAOBAO
 *
 * The specific company id used for tmall mesh profile.
 *
 */
#define CONFIG_CID_TAOBAO            0x01A8

/**
 * @def CONFIG_BT_MESH_CPRL
 *
 * The specific cprl value in the Composition Data Page 0.
 *
 */
#define CONFIG_BT_MESH_CRPL          100

/**
 * @def CONFIG_BT_MESH_LED_PIN
 *
 * The specific LED1 pin number of nrf52832 dev board.
 * You can change this macro with the desired pin number.
 * e.g. LED1(P0.13) for nrf52840 pca10056 dev board.
 *
 */
#define CONFIG_BT_MESH_LED_PIN       17

#endif // BT_MESH_PRFOILE_CONFIG_H_

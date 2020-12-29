/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ALI_VENDORMODEL_PROFILE_CONFIG_H_
#define ALI_VENDORMODEL_PROFILE_CONFIG_H_

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
#define CONFIG_MESH_MODEL_NUM        2

/**
 * @def CONFIG_MESH_VENDOR_MODLE_NUM
 *
 * The specific supported number of vendor models in product.
 *
 */
#define CONFIG_MESH_VENDOR_COMPANY_ID 0x01a8
#define CONFIG_MESH_VENDOR_MODEL_SRV  0x0000
#define CONFIG_MESH_VENDOR_MODEL_CLI  0x0001

#define VENDOR_OPCODE_GET_STATUS 	0xD0
#define VENDOR_OPCODE_SET_ACK 		0xD1
#define VENDOR_OPCODE_SET_UNACK 	0xD2
#define VENDOR_OPCODE_STATUS_ACK 	0xD3
#define VENDOR_OPCODE_INDICATE 		0xD4
#define VENDOR_OPCODE_CONFIME 		0xD5

#define CONFIG_MESH_VENDOR_MODEL_NUM        1

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
#ifndef CONFIG_BT_MESH_CRPL
#define CONFIG_BT_MESH_CRPL          100
#endif
/**
 * @def CONFIG_BT_MESH_LED_PIN
 *
 * The specific LED1 pin number of nrf52832 dev board.
 * You can change this macro with the desired pin number.
 * e.g. LED1(P0.13) for nrf52840 pca10056 dev board.
 *
 */
#define CONFIG_BT_MESH_LED_PIN       17

#endif // ALI_VENDORMODEL_PRFOILE_CONFIG_H_

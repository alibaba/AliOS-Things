/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bt_mesh.h
 * @brief:   vendor bt mesh apis defination
 * @author:  
 * @date:    2019/12/15
 * @version: 1.0
 */


#ifndef _TG_VENDOR_BT_MESH_H_
#define _TG_VENDOR_BT_MESH_H_

#include <stdint.h>
#include <stdbool.h>
#if __cplusplus
extern "C" {
#endif


typedef struct
{
    uint8_t  val[6];
} tg_bt_mesh_addr_t;


/** LE Advertising Parameters. */
struct tg_bt_mesh_le_adv_param
{
    /** Bit-field of advertising options */
    uint8_t  options;

    /** Minimum Advertising Interval (N * 0.625) */
    uint16_t interval_min;

    /** Maximum Advertising Interval (N * 0.625) */
    uint16_t interval_max;

    /** Optional predefined (random) own address. Currently
     *  the only permitted use of this is for NRPA with
     *  non-connectable advertising.
     */
    const tg_bt_mesh_addr_t *own_addr;
};


/** LE scan parameters */
struct tg_bt_mesh_le_scan_param
{
    /** Scan type (BT_MESH_HCI_LE_SCAN_ACTIVE or BT_MESH_HCI_LE_SCAN_PASSIVE) */
    uint8_t  type;

    /** Duplicate filtering (BT_MESH_HCI_LE_SCAN_FILTER_DUP_ENABLE or
     *  BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE)
     */
    uint8_t  filter_dup;

    /** Scan interval (N * 0.625 ms) */
    uint16_t interval;

    /** Scan window (N * 0.625 ms) */
    uint16_t window;
};

struct tg_bt_mesh_data
{
    uint8_t type;
    uint8_t data_len;
    const uint8_t *data;
};

typedef struct
{
    uint8_t      type;  /* 0 - public addr, other - RFU */
    tg_bt_mesh_addr_t a;
} tg_bt_mesh_addr_le_t;

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#define __net_buf_align __aligned(sizeof(uint8_t *))
struct tg_bt_mesh_net_buf_simple
{
    /** Pointer to the start of data in the buffer. */
    uint8_t *data;

    /** Length of the data behind the data pointer. */
    uint16_t len;

    /** Amount of data that this buffer can store. */
    uint16_t size;

    /** Start of the data storage. Not to be accessed directly
     *  (the data pointer should be used instead).
     */
    uint8_t __buf[0] __net_buf_align;
};
/** @def BT_MESH_NET_BUF_SIMPLE
 *  @brief Define a net_buf_simple stack variable and get a pointer to it.
 *
 *  This is a helper macro which is used to define a net_buf_simple object on
 *  the stack and the get a pointer to it as follows:
 *
 *  struct net_buf_simple *my_buf = NET_BUF_SIMPLE(10);
 *
 *  After creating the object it needs to be initialized by calling
 *  net_buf_simple_init().
 *
 *  @param _size Maximum data storage for the buffer.
 *
 *  @return Pointer to stack-allocated net_buf_simple object.
 */
#define BT_MESH_NET_BUF_SIMPLE(_size)                        \
	((struct tg_bt_mesh_net_buf_simple *)(&(struct {        \
		struct tg_bt_mesh_net_buf_simple buf;           \
		u8_t data[_size] __net_buf_align; \
	}) {                                         \
		.buf.size = _size,                   \
	}))

/**
 * @brief Generate random data
 * @param[in] addr:  received advertise address
 * @param[in] rssi:  received advertise packet rssi
 * @param[in] adv_type:  received advertise packet type
 * @param[in] buf:  received advertise packet content
 * @return: None
 */
typedef void tg_bt_mesh_le_scan_cb_t(const tg_bt_mesh_addr_le_t *addr, int8_t rssi,
                                      uint8_t adv_type, struct tg_bt_mesh_net_buf_simple *buf);

 struct tg_bt_mesh_scan_misc {
    /** Scan type (BT_HCI_LE_SCAN_ACTIVE or BT_HCI_LE_SCAN_PASSIVE) */
    uint8_t  type;

    /** Duplicate filtering (BT_HCI_LE_SCAN_FILTER_DUP_ENABLE or
    *	BT_HCI_LE_SCAN_FILTER_DUP_DISABLE)
    */
    uint8_t  filter_dup;

    /** Scan interval (N * 0.625 ms) */
    uint16_t interval;

    /** Scan window (N * 0.625 ms) */
    uint16_t window;

    tg_bt_mesh_le_scan_cb_t *scan_cb;
};

struct tg_bt_mesh_adv_misc {
    uint8_t  isInMeshAdv;
    // le_adv_param
    uint8_t  options;
    uint16_t interval_min;
    uint16_t interval_max;
    uint8_t  own_mesh_addr[6];
    uint8_t adv_type;

    // adv data
    uint8_t adv_data_len;
    uint8_t adv_data[0x1F];
    
    // scan response data	
    uint8_t srd_data_len;
    uint8_t srd_data[0x1F];	
};

/**
 * @brief Generate random data
 * @param[out] buf:  buffer to store random value.
 * @param[in] len:  random length
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_rand(void *buf, int32_t len);

/**
 * @brief mesh start adv broadcast
 * @param[in] param: advertise paramters set
 * @param[in] ad: advertise data
 * @param[in] ad_len: advertise data length
 * @param[in] sd: scan response data, reserved in mesh
 * @param[in] sd_len: advertise data length, reserved in mesh, set to 0
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_adv_start(const struct tg_bt_mesh_le_adv_param *param,
                          const struct tg_bt_mesh_data *ad, int32_t ad_len,
                          const struct tg_bt_mesh_data *sd, int32_t sd_len);

/**
 * @brief mesh stop adv broadcast
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_adv_stop(void);

/**
 * @brief mesh start scan
 * @param[in] param: scan paramters set
 *            cb: callback function when scaned advertising packet
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_scan_start(const struct tg_bt_mesh_le_scan_param *param,
                           tg_bt_mesh_le_scan_cb_t cb);

/**
 * @brief mesh stop scan
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_scan_stop(void);

/**
 * @brief mesh set priority
 * @param[in] enable: 1 means increase mesh priority; 0 means set mesh priority to normal
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_enable_aggressive_setting(bool enable);

/**
 * @brief set oneshot adv broadcast data
 * @param[in] ad: advertise data
 * @param[in] ad_len: advertise data length
 * @param[in] sd: scan response data, reserved in mesh
 * @param[in] sd_len: advertise data length, reserved in mesh, set to 0
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_set_oneshot_data(const struct tg_bt_mesh_data *ad, int32_t ad_len,
                                 const struct tg_bt_mesh_data *sd, int32_t sd_len);

/**
 * @brief send oneshot adv packet
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_send_adv_oneshot(void);


#if __cplusplus
};
#endif

#endif /* _TG_VENDOR_BT_MESH_H_ */

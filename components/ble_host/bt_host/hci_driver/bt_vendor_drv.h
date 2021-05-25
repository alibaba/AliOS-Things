/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __BT_VENDOR_DRV_H_
#define __BT_VENDOR_DRV_H_

#include <stdio.h>
#include <stdint.h>

typedef void (*rx_ind_cb_t)(void);

/**
* 初始化BT驱动、加载patch、调节Coex，其他厂家IC初始化的操作
*
* @return  操作状态，0代表成功，其他值表示失败
*/
int bt_vendor_drv_bring_up(void);

/**
* 设置RX事件回调，BT Driver收到EVENT/ACL数据后，通过此接口通知BT Stack进行读取操作
* @param[in]  ready_to_rx  回调函数
* @return     操作状态，0代表成功，其他值表示失败
*/
int bt_vendor_drv_set_rx_ind(rx_ind_cb_t ind);

/**
* 从BT Controller读取EVENT/ACL数据
* @param[out]  data 数据保存的起始内存地址
* @param[in]   len  期望读取的数据长度
* @return      实际读取的数据长度
*/
size_t bt_vendor_drv_rx(uint8_t *data, size_t len);

/**
* BT Stack通过此接口往Controller发送数据
* @param[in]  data    数据的起始内存地址
* @param[in]  len     期望发送的数据长度
* @param[in]  reserve NULL
* @return             实际发送的数据长度
*/
size_t bt_vendor_drv_tx(uint8_t *data, size_t len, void *reserve);

#endif
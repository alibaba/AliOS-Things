/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __BT_VENDOR_DRV_H_
#define __BT_VENDOR_DRV_H_

#include <stdio.h>
#include <stdint.h>
#include "osif_customer.h"
#include "hci_if.h"
#include "hci_tp.h"

bool bt_vendor_open_flag = false;
void *bt_vendor_open_sem = NULL;
void *bt_vendor_tx_sem = NULL;

static bool bt_vendor_init_callback(T_HCI_IF_EVT evt, bool status, uint8_t *p_buf, uint32_t len)
{
    if (evt == HCI_IF_EVT_OPENED) {
        if (status == true) {
            printf("hci_if_open success\r\n");
        } else {
            printf("hci_if_open fail\r\n");
        }

        bt_vendor_open_flag = status;
        os_sem_give(bt_vendor_open_sem);
    } else {
        printf("Other event enter bt_vendor_init_callback! evt = 0x%x, status = 0x%x\r\n", evt, status);
    }
}

static bool bt_vendor_tx_callback(void)
{
    os_sem_give(bt_vendor_tx_sem);
}

/**
* 初始化BT驱动、加载patch、调节Coex，其他厂家IC初始化的操作
*
* @return  操作状态，0代表成功，其他值表示失败
*/
int bt_vendor_drv_bring_up(void)
{
    int ret = 0;

	if(bt_vendor_open_flag == true) {
    	printf("no need to bt_vendor_drv_bring_up again!\r\n");
        return 1;
	}
	
    if (os_sem_create(&bt_vendor_open_sem, 0, 1) != true) {
        printf("Init bt_vendor_open_sem error!\r\n");
        return 1;
    }

    wifi_on(1);
    sleep(1);

    hci_if_open(bt_vendor_init_callback);

    if (os_sem_take(bt_vendor_open_sem, 0xFFFFFFFF) == true) {
        if (bt_vendor_open_flag == true) {
            if (os_sem_create(&bt_vendor_tx_sem, 1, 1) != true) {
                printf("Init bt_vendor_tx_sem error!\r\n");
				ret = 1;
            }
        } else {
            ret = 1;
        }
    } else {
        printf("Take bt_vendor_open_sem error!\r\n");
		ret = 1;
    }
	
    os_sem_delete(bt_vendor_open_sem);
	hci_if_del_task();
	hci_if_deinit();
    return ret;
}
/**
* 设置RX事件回调，BT Driver收到EVENT/ACL数据后，通过此接口通知BT Stack进行读取操作
* @param[in]  ready_to_rx  回调函数
* @return     操作状态，0代表成功，其他值表示失败
*/
int bt_vendor_drv_set_rx_ind(void (*ready_to_rx)(void))
{
	if (bt_vendor_open_flag == true) {
    	hci_tp_set_rx_ind(ready_to_rx);
    } else {
        printf("must call after bt_vendor_drv_bring_up\r\n");
    }
}

/**
* 从BT Controller读取EVENT/ACL数据
* @param[out]  data 数据保存的起始内存地址
* @param[in]   len  期望读取的数据长度
* @return      实际读取的数据长度
*/
size_t bt_vendor_drv_rx(uint8_t *data, size_t len)
{
	return hci_tp_recv(data, len);
}
/**
* BT Stack通过此接口往Controller发送数据
* @param[in]  data    数据的起始内存地址
* @param[in]  len     期望发送的数据长度
* @param[in]  reserve NULL
* @return             实际发送的数据长度
*/

size_t bt_vendor_drv_tx(uint8_t *data, size_t len, void *reserve)
{
	os_sem_take(bt_vendor_tx_sem, 0xFFFFFFFF);
	hci_tp_send(data, len, bt_vendor_tx_callback);
	return len;
}
#endif
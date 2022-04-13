/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICES_HCI_H_
#define DEVICES_HCI_H_
#include <stdint.h>

//#include <devices/driver.h>

typedef enum {
    HCI_EVENT_READ,
} hci_event_t;

#define hci_open(name) device_open(name)
#define hci_open_id(name, id) device_open_id(name, id)
#define hci_close(dev) device_close(dev)

typedef void (*hci_event_cb_t)(hci_event_t event, uint32_t size, void *priv);

typedef int (*hci_driver_send_cmd_t)(uint16_t opcode, uint8_t* send_data, uint32_t send_len, uint8_t* resp_data, uint32_t *resp_len);

#ifndef YULONG_HCI
#define YULONG_HCI
#endif

#ifndef YULONG_HCI
/**
  \brief       set hci event
  \param[in]   dev      Pointer to device object.
  \param[in]   event    data read event callback.
  \param[in]   priv     event callback userdata arg.
  \return      0 on success, else on fail.
*/
int hci_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv);

/**
  \brief       send hci format data
  \param[in]   dev      Pointer to device object.
  \param[in]   data     data address to send.
  \param[in]   size     data length expected to read.
  \return      send data len.
*/
int hci_send(aos_dev_t *dev, void *data, uint32_t size);

/**
  \brief       recv hci format data
  \param[in]   dev      Pointer to device object.
  \param[in]   data     data address to read.
  \param[in]   size     data length expected to read.
  \return      read data len.
*/
int hci_recv(aos_dev_t *dev, void* data, uint32_t size);

/**
  \brief       start hci driver
  \param[in]   dev      Pointer to device object.
  \param[out]  send_cmd sned hci command callback.
  \return      0 on success, else on fail.
*/
int hci_start(aos_dev_t *dev, hci_driver_send_cmd_t send_cmd);
#endif
#endif

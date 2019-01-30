/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN
#include "adapter.h"
#include "can.h"
#include "canfestival.h"

static can_dev_t can_dev;
static CO_Data *co_data = NULL;

int32_t can_init(uint8_t port, uint32_t baud, CO_Data* d)
{
    int32_t ret = -1;

    co_data = d;
    can_dev.port = port;
    can_dev.config.baud_rate = baud;

    ret = hal_can_init(&can_dev);
    if (ret == -1){
        LOGD("canopen", "can_handle init failed!\n");
    }

    return ret;
}

/* canSend is a protocol library interface implement */
UNS8 canSend(CAN_PORT notused, Message *m)
{
    UNS8 ret = 0;
    uint8_t i;
    uint8_t tx_data[8] = {0};
    can_frameheader_t  tx_message = {0};

    tx_message.id = m->cob_id;
    if(m->rtr != 0){
        tx_message.rtr = CAN_RTR_REMOTE;
    }
    else{
        tx_message.rtr = CAN_RTR_DATA;
    }
    tx_message.dlc = m->len;
    for(i = 0; i < m->len; i++){
        tx_data[i] = m->data[i];
    }

    ret = (UNS8)hal_can_send(&can_dev, &tx_message, tx_data, WAIT_FOREVER);
    if (ret != 0){
        LOGD("canopen", "can send error!\n");
        return 0xFF;
    }

    return ret;
}

void can_dispatch()
{
    int i;
    uint8_t rx_data[8] = {0};
    can_frameheader_t  rx_message = {0};
    Message rxm = {0};

    hal_can_recv(&can_dev, &rx_message, rx_data, WAIT_FOREVER);

    rxm.cob_id = rx_message.id;
    if(rx_message.rtr != 0){
        rxm.rtr = 1;
    }

    rxm.len = rx_message.dlc;
    for(i = 0; i < rxm.len; i++){
        rxm.data[i] = rx_data[i];
    }

    canDispatch(co_data, &rxm);
}
#endif    /* AOS_CANOPEN */


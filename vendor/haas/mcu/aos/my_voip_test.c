#include <stdbool.h>
#include <stdio.h>
#include "tg_speech.h"
#include "hal_location.h"
#include "transq_msg.h"
#include "k_api.h"

static ktask_t *g_tx_rx = NULL;
static void bes_m33_recv_cb(void *msg, int size)
{
    callback_msg_t *_msg = (callback_msg_t *)msg;
    int *arr = (int *)_msg->data;
    printf("bes_m33_recv_cb _msg:%d,%d,%d \n",arr[0],arr[1],arr[2]);
}

static void bes_m33_tx_rx(void *arg)
{
    printf("bes_m33_tx_rx init\n");
    osDelay(15000);
    tg_voip_msg_register(bes_m33_recv_cb);

    callback_msg_t msg;
    int a[]={1,2,3};
    while (1)
    {
        memset(&msg, 0, sizeof(msg));
        msg.evt_id = 1;
        msg.data = a;
        msg.extValue = 1; // start record
        msg.len = sizeof(a);
        tg_voip_msg_send(&msg, sizeof(msg), 1);
        osDelay(3000);
    }
}

void bes_voip_test()
{
    krhino_task_dyn_create(&g_tx_rx, "bes_m33_tx_rx", 0, 32, 0, 0x1000, (task_entry_t)bes_m33_tx_rx, 1);
}
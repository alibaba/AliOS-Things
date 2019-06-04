/**
 ****************************************************************************************
 *
 * @file app.c
 *
 *
 * Copyright (C) Beken Corp 2011-2016
 *
 ****************************************************************************************
 */
#include "include.h"
#include "mem_pub.h"
#include "rwnx_config.h"
#include "app.h"

#if (NX_POWERSAVE)
#include "ps.h"
#endif //(NX_POWERSAVE)

#include "sa_ap.h"
#include "sa_station.h"
#include "main_none.h"
#include "sm.h"
#include "uart_pub.h"

#include "rtos_pub.h"
#include "error.h"
#include "param_config.h"
#include "rxl_cntrl.h"
#include "lwip/pbuf.h"
#include "rw_pub.h"
#include "rw_msg_rx.h"
#include "hostapd_intf_pub.h"

#include "app_music_pub.h"
#include "wlan_ui_pub.h"
#include "ps_debug_pub.h"
#include "power_save_pub.h"
#include "mcu_ps_pub.h"
#if CFG_SUPPORT_ALIOS
#include "rw_msdu.h"
#include "txu_cntrl.h"
#include "ll.h"
#elif (!CFG_SUPPORT_RTT)
#include "wlan_cli_pub.h"
#include "demos_start.h"
#include "demos_start.h"
#endif
#include "app_music_pub.h"

#if (CFG_SUPPORT_ALIOS || CFG_SUPPORT_RTT)
beken_thread_t  init_thread_handle;
beken_thread_t  app_thread_handle;
#else
xTaskHandle  init_thread_handle;
xTaskHandle  app_thread_handle;
#endif
uint32_t  init_stack_size = 2000;
uint32_t  app_stack_size = 2048;

beken_semaphore_t app_sema = NULL;
WIFI_CORE_T g_wifi_core = {0};
volatile int32_t bmsg_rx_count = 0;

extern void net_wlan_initial(void);
extern void wpas_thread_start(void);

void app_init(void)
{
#if (!CFG_SUPPORT_RTT)
    net_wlan_initial();
#endif
    wpas_thread_start();
}

void app_set_sema(void)
{
    OSStatus ret;
    ret = rtos_set_semaphore(&app_sema);

    (void)ret;
}

static void kmsg_bk_thread_main( void *arg )
{
    OSStatus ret;

    mr_kmsg_init();
    while(1)
    {
        ret = rtos_get_semaphore(&app_sema, BEKEN_WAIT_FOREVER);
        ASSERT(kNoErr == ret);

        rwnx_recv_msg();
        ke_evt_none_core_scheduler();
    }
}

static void init_thread_main( void *arg )
{
    GLOBAL_INT_START();

    app_init();
    os_printf("app_init finished\r\n");

    rtos_delete_thread( NULL );
}

/** @brief  When in dtim rf off mode,user can manual wakeup before dtim wakeup time.
 *          this function must be called in "core_thread" context
 */
int bmsg_ps_handler_rf_ps_mode_real_wakeup(void)
{
#if CFG_USE_STA_PS
    power_save_rf_dtim_manual_do_wakeup();
    power_save_rf_ps_wkup_semlist_set();
#endif
    return 0;
}

void bmsg_rx_handler(BUS_MSG_T *msg)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    if(bmsg_rx_count > 0)
    {
        bmsg_rx_count -= 1;
    }
    GLOBAL_INT_RESTORE();

    rxl_cntrl_evt((int)msg->arg);
}

void bmsg_skt_tx_handler(BUS_MSG_T *msg)
{
    hapd_intf_ke_rx_handle(msg->arg);
}

void bmsg_tx_handler(BUS_MSG_T *msg)
{
    OSStatus ret;
    struct pbuf *p = (struct pbuf *)msg->arg;
    struct pbuf *q = p;
    uint8_t vif_idx = (uint8_t)msg->len;

    if(p->next)
    {
        q = pbuf_coalesce(p, PBUF_RAW);
        if(q == p)
        {
            // must be out of memory
            goto tx_handler_exit;
        }
    }

    ps_set_data_prevent();
#if CFG_USE_STA_PS
    bmsg_ps_handler_rf_ps_mode_real_wakeup();
#endif
    rwm_transfer(vif_idx, q->payload, q->len);
tx_handler_exit:

    pbuf_free(q);
}

#if CFG_SUPPORT_ALIOS
void bmsg_tx_raw_handler(BUS_MSG_T *msg)
{
	OSStatus ret;
	uint8_t *pkt = (uint8_t *)msg->arg;
	uint16_t len = msg->len;
	MSDU_NODE_T *node;
	UINT8 *content_ptr;
	UINT32 queue_idx = AC_VI;
	struct txdesc *txdesc_new;
	struct umacdesc *umac;

	node = rwm_tx_node_alloc(len);
	if (node == NULL) {
		goto exit;
	}

	rwm_tx_msdu_renew(pkt, len, node->msdu_ptr);
	content_ptr = rwm_get_msdu_content_ptr(node);

	txdesc_new = tx_txdesc_prepare(queue_idx);
	if(txdesc_new == NULL || TXDESC_STA_USED == txdesc_new->status) {
		rwm_node_free(node);
		goto exit;
	}

	txdesc_new->status = TXDESC_STA_USED;
	txdesc_new->host.flags = TXU_CNTRL_MGMT;
	txdesc_new->host.orig_addr = (UINT32)node->msdu_ptr;
	txdesc_new->host.packet_addr = (UINT32)content_ptr;
	txdesc_new->host.packet_len = len;
	txdesc_new->host.status_desc_addr = (UINT32)content_ptr;
	txdesc_new->host.tid = 0xff;

	umac = &txdesc_new->umac;
	umac->payl_len = len;
	umac->head_len = 0;
	umac->tail_len = 0;
	umac->hdr_len_802_2 = 0;

	umac->buf_control = &txl_buffer_control_24G;

	txdesc_new->lmac.agg_desc = NULL;
	txdesc_new->lmac.hw_desc->cfm.status = 0;

    ps_set_data_prevent();
#if CFG_USE_STA_PS
    bmsg_ps_handler_rf_ps_mode_real_wakeup();
#endif

	rwm_push_tx_list(node);
	txl_cntrl_push(txdesc_new, queue_idx);

exit:
	os_free(pkt);
}

void bmsg_rx_lsig_handler(BUS_MSG_T *msg)
{
	lsig_input((msg->arg&0xFFFF0000)>>16, msg->arg&0xFF, msg->len);
}
#endif

void bmsg_ioctl_handler(BUS_MSG_T *msg)
{
    ke_msg_send((void *)msg->arg);
}

void bmsg_music_handler(BUS_MSG_T *msg)
{
#if (CONFIG_APP_MP3PLAYER == 1)
    media_msg_sender((void *)msg->arg);
#endif
}

void bmsg_skt_tx_sender(void *arg)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_SKT_TX_TYPE;
    msg.arg = (uint32_t)arg;
    msg.len = 0;
    msg.sema = NULL;

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        os_printf("bmsg_rx_sender_failed\r\n");
    }
}

void ps_msg_process(UINT8 ps_msg)
{
    switch(ps_msg)
    {
#if CFG_USE_STA_PS
    case PS_BMSG_IOCTL_RF_ENABLE:
        power_save_dtim_enable();
        break;

    case PS_BMSG_IOCTL_RF_USER_WKUP:
        bmsg_ps_handler_rf_ps_mode_real_wakeup();
        break;

    case PS_BMSG_IOCTL_RF_DISANABLE:
        power_save_dtim_disable();
        break;
#endif
#if CFG_USE_MCU_PS
    case PS_BMSG_IOCTL_MCU_ENABLE:
        mcu_ps_init();
        break;

    case PS_BMSG_IOCTL_MCU_DISANABLE:
        mcu_ps_exit();
        break;
#endif
#if CFG_USE_STA_PS
    case PS_BMSG_IOCTL_RF_TD_SET:
        power_save_td_ck_timer_set();
        break;

        case PS_BMSG_IOCTL_RF_TD_HANDLER:
            power_save_td_ck_timer_real_handler();
            break;

        case PS_BMSG_IOCTL_RF_KP_HANDLER:
            power_save_keep_timer_real_handler();
            break;

        case PS_BMSG_IOCTL_RF_KP_SET:
            power_save_keep_timer_set();
            break;

        case PS_BMSG_IOCTL_RF_KP_STOP:
            power_save_keep_timer_stop();
            break;
#endif

        default:
            break;
    }
}

void bmsg_null_sender(void)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_NULL_TYPE;
    msg.arg = 0;
    msg.len = 0;
    msg.sema = NULL;

    if(!rtos_is_queue_empty(&g_wifi_core.io_queue))
    {
        return;
    }

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        os_printf("bmsg_null_sender_failed\r\n");
    }
}

void bmsg_rx_sender(void *arg)
{
    OSStatus ret;
    BUS_MSG_T msg;
    GLOBAL_INT_DECLARATION();

    msg.type = BMSG_RX_TYPE;
    msg.arg = (uint32_t)arg;
    msg.len = 0;
    msg.sema = NULL;

    GLOBAL_INT_DISABLE();
    if(bmsg_rx_count >= 2)
    {
        GLOBAL_INT_RESTORE();
        return;
    }

    bmsg_rx_count += 1;
    GLOBAL_INT_RESTORE();

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        APP_PRT("bmsg_rx_sender_failed\r\n");
    }
}

int bmsg_tx_sender(struct pbuf *p, uint32_t vif_idx)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_TX_TYPE;
    msg.arg = (uint32_t)p;
    msg.len = vif_idx;
    msg.sema = NULL;

    pbuf_ref(p);
    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, 1 * SECONDS);
    if(kNoErr != ret)
    {
        APP_PRT("bmsg_tx_sender failed\r\n");
        pbuf_free(p);
    }

    return ret;
}

#if CFG_SUPPORT_ALIOS
int bmsg_tx_raw_sender(uint8_t *payload, uint16_t length)
{
	OSStatus ret;
	BUS_MSG_T msg;

	msg.type = BMSG_TX_RAW_TYPE;
	msg.arg = (uint32_t)payload;
	msg.len = length;
	msg.sema = NULL;

	ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, 1*SECONDS);

	if(ret != kNoErr) 
	{
		APP_PRT("bmsg_tx_sender failed\r\n");
		os_free(payload);
	}

	return ret;
}

void bmsg_rx_lsig(uint16_t len, uint8_t rssi)
{
	BUS_MSG_T msg;

	msg.type = BMSG_RX_LSIG;
	msg.arg = (uint32_t)((len << 16) | rssi);
	msg.len = rtos_get_time();
	msg.sema = NULL;
	rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
}
#endif

int bmsg_ioctl_sender(void *arg)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_IOCTL_TYPE;
    msg.arg = (uint32_t)arg;
    msg.len = 0;
    msg.sema = NULL;

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        APP_PRT("bmsg_ioctl_sender_failed\r\n");
    }
    else
    {
        APP_PRT("bmsg_ioctl_sender\r\n");
    }

    return ret;
}

void bmsg_music_sender(void *arg)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_MEDIA_TYPE;
    msg.arg = (uint32_t)arg;
    msg.len = 0;
    msg.sema = NULL;

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        APP_PRT("bmsg_media_sender_failed\r\n");
    }
}

#if CFG_USE_AP_PS
void bmsg_txing_sender(uint8_t sta_idx)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_TXING_TYPE;
    msg.arg = (uint32_t)sta_idx;
    msg.len = 0;
    msg.sema = NULL;

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        APP_PRT("bmsg_txing_sender failed\r\n");
    }
}

void bmsg_txing_handler(BUS_MSG_T *msg)
{
    OSStatus ret;
    UINT8 sta_idx = (UINT8)msg->arg;

    rwm_msdu_send_txing_node(sta_idx);
}
#endif

void bmsg_ps_sender(uint8_t arg)
{
    OSStatus ret;
    BUS_MSG_T msg;

    msg.type = BMSG_STA_PS_TYPE;
    msg.arg = (uint32_t)arg;
    msg.len = 0;
    msg.sema = NULL;

    ret = rtos_push_to_queue(&g_wifi_core.io_queue, &msg, BEKEN_NO_WAIT);
    if(kNoErr != ret)
    {
        os_printf("bmsg_ps_sender failed\r\n");
    }

}
#if CFG_USE_STA_PS

void bmsg_ps_handler(BUS_MSG_T *msg)
{
    UINT8 arg;
    OSStatus ret;

    arg = (UINT8)msg->arg;
    ps_msg_process(arg);
}
#endif
static void core_thread_main( void *arg )
{
    OSStatus ret;
    BUS_MSG_T msg;
    uint8_t ke_skip = 0;
    uint8_t ps_flag = 0;

    while(1)
    {
        ret = rtos_pop_from_queue(&g_wifi_core.io_queue, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == ret)
        {
            switch(msg.type)
            {
#if CFG_USE_STA_PS
            case BMSG_STA_PS_TYPE:
                if(msg.arg == PS_BMSG_IOCTL_RF_DISANABLE)
                {
                    bmsg_ps_handler(&msg);
                }
                else
                {
                    ps_flag = 1;
                }
                break;
#endif

            case BMSG_RX_TYPE:
                APP_PRT("bmsg_rx_handler\r\n");
                bmsg_rx_handler(&msg);
                break;

            case BMSG_TX_TYPE:
                APP_PRT("bmsg_tx_handler\r\n");
                bmsg_tx_handler(&msg);
                break;

            case BMSG_SKT_TX_TYPE:
                APP_PRT("bmsg_skt_tx_handler\r\n");
                bmsg_skt_tx_handler(&msg);
                break;

            case BMSG_IOCTL_TYPE:
                APP_PRT("bmsg_ioctl_handler\r\n");
                bmsg_ioctl_handler(&msg);
                break;
            case BMSG_MEDIA_TYPE:
                ke_skip = 1;
                bmsg_music_handler(&msg);
                break;

#if CFG_USE_AP_PS
            case BMSG_TXING_TYPE:
                bmsg_txing_handler(&msg);
                break;
#endif

#if CFG_SUPPORT_ALIOS
                case BMSG_TX_RAW_TYPE:
                    bmsg_tx_raw_handler(&msg);
                    break;
					
				case BMSG_RX_LSIG:
					bmsg_rx_lsig_handler(&msg);
					break;
					
#endif
            default:
                APP_PRT("unknown_msg\r\n");
                break;
            }

            if (msg.sema != NULL)
            {
                rtos_set_semaphore(&msg.sema);
            }
            if(!ke_skip)
                ke_evt_core_scheduler();
            else
                ke_skip = 0;
        }

#if CFG_USE_STA_PS
        if(ps_flag == 1)
        {
            bmsg_ps_handler(&msg);
            ps_flag = 0;
        }
        power_save_rf_sleep_check();
#endif

    }
}

void core_thread_init(void)
{
    OSStatus ret;

    g_wifi_core.queue_item_count = CORE_QITEM_COUNT;
    g_wifi_core.stack_size = CORE_STACK_SIZE;

    ret = rtos_init_queue(&g_wifi_core.io_queue,
                          "core_queue",
                          sizeof(BUS_MSG_T),
                          g_wifi_core.queue_item_count);
    if (kNoErr != ret)
    {
        os_printf("Create io queue failed\r\n");
        goto fail;
    }

    ret = rtos_create_thread(&g_wifi_core.handle,
                             THD_CORE_PRIORITY,
                             "core_thread",
                             (beken_thread_function_t)core_thread_main,
                             (unsigned short)g_wifi_core.stack_size,
                             (beken_thread_arg_t)0);
    if (kNoErr != ret)
    {
        os_printf("Create core thread failed\r\n");
        goto fail;
    }

    return;

fail:
    core_thread_uninit();

    return;
}

void core_thread_uninit(void)
{
    if(g_wifi_core.handle)
    {
        rtos_delete_thread(&g_wifi_core.handle);
        g_wifi_core.handle = 0;
    }

    if(g_wifi_core.io_queue)
    {
        rtos_deinit_queue(&g_wifi_core.io_queue);
        g_wifi_core.io_queue = 0;
    }

    g_wifi_core.queue_item_count = 0;
    g_wifi_core.stack_size = 0;
}

#if (!CFG_SUPPORT_ALIOS && !CFG_SUPPORT_RTT)
static void init_app_thread( void *arg )
{
    if(application_start)
    {
        application_start();
    }

    rtos_delete_thread( NULL );
}
#endif

void app_pre_start(void)
{
    OSStatus ret;

#if CFG_SUPPORT_ALIOS
    ret = rtos_init_semaphore(&app_sema, 0);
#else
    ret = rtos_init_semaphore(&app_sema, 1);
#endif
    ASSERT(kNoErr == ret);

    ret = rtos_create_thread(&app_thread_handle,
                             THD_APPLICATION_PRIORITY,
                             "kmsgbk",
                             (beken_thread_function_t)kmsg_bk_thread_main,
                             (unsigned short)app_stack_size,
                             (beken_thread_arg_t)0);
    ASSERT(kNoErr == ret);

    ret = rtos_create_thread(&init_thread_handle,
                             THD_INIT_PRIORITY,
                             "init_thread",
                             (beken_thread_function_t)init_thread_main,
                             (unsigned short)init_stack_size,
                             (beken_thread_arg_t)0);
    ASSERT(kNoErr == ret);

    core_thread_init();

#if (CONFIG_APP_MP3PLAYER == 1)
    key_init();
    media_thread_init();
#endif

#if (!CFG_SUPPORT_ALIOS && !CFG_SUPPORT_RTT)
    ret = rtos_create_thread(NULL,
                             THD_INIT_PRIORITY,
                             "app",
                             (beken_thread_function_t)init_app_thread,
                             (unsigned short)1024,
                             (beken_thread_arg_t)0);
#endif
}


void app_start(void)
{
    app_pre_start();

#if (!CFG_SUPPORT_ALIOS && !CFG_SUPPORT_RTT)
    cli_init();
#endif

#if CFG_UDISK_MP3
    um_init();
#endif

#if defined(SUPPORT_MIDEA_BLE)
    if(!get_ate_mode_state())
    {
	    bk_wlan_start_ble();
    }
#endif

#ifdef CFG_ENABLE_USER_APP
    do
    {
        OSStatus ret;
        extern void user_main( beken_thread_arg_t args );
        ret = rtos_create_thread(NULL,
                                 BEKEN_APPLICATION_PRIORITY,
                                 "app",
                                 (beken_thread_function_t)user_main,
                                 app_stack_size,
                                 (beken_thread_arg_t)0);
    }
    while(0);
#endif // CFG_ENABLE_USER_APP
}

int bmsg_is_empty(void)
{
    if(!rtos_is_queue_empty(&g_wifi_core.io_queue))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// eof


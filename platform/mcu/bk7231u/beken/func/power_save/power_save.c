#include "intc_pub.h"
#include "rtos_pub.h"

#include "wdt_pub.h"
#include "gpio_pub.h"
#include "pwm_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"

#include "fake_clock_pub.h"
#include "power_save.h"
#include "target_util_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "arm_arch.h"
#include "rwnx_config.h"
#include "ps.h"
#include "rwnx.h"
#include "uart_pub.h"
#include "mcu_ps_pub.h"
#include "error.h"

volatile static PS_MODE_STATUS    bk_ps_mode = PS_NO_PS_MODE;

#if CFG_USE_STA_PS
static STA_PS_INFO bk_ps_info =
{
    .tm_status = PS_TM_CK_TIMEOUT,
    .ps_dtim_period = 1,
    .ps_dtim_multi = 1,
    .sleep_ms = 300,
    .liston_int = 1,
    .waited_beacon = STA_GET_INIT,
    .ps_bcn_ab_status = PS_BCN_STATUS_INIT,
    .sleep_first = 1,
    .ps_can_sleep = 0,
    .ps_bcn_cal_status = PS_BCN_NO_CAL,
    .ps_real_sleep = 0,
    .pwm0_less_time = 0,
};

#if (CFG_SOC_NAME == SOC_BK7231)
static UINT16 r_wakeup_time = 50;
#else
static UINT16 r_wakeup_time = 32;
#endif

#if PS_DTIM_WITH_NORMAL
volatile static UINT8 ps_dtim_normal_enable = 0;
#endif
static UINT8 ps_lock = 1;
static PS_FORBID_STATUS bk_forbid_code = 0;
static UINT16 bk_forbid_count = 0;
static UINT32 ps_dis_flag = 0;
static UINT16 beacon_len = 0;
static UINT32 ps_delay_wait_time = 0;
static UINT32 ps_delay_rfup_time = 0;
UINT32 ps_next_data_ck_time = 0;
static UINT8 pwm0_use_for_ps = 0;
#if CFG_SUPPORT_ALIOS
static beken_timer_t ps_td_ck_timer = {0};
static beken_timer_t ps_keep_timer = {0};
#else
static beken2_timer_t ps_td_ck_timer = {0};
static beken2_timer_t ps_keep_timer = {0};
#endif
static UINT32 ps_td_ck_timer_status = 0;
static UINT32 ps_keep_timer_status = 0;
static UINT32 ps_td_last_tick = 0;
static UINT32 ps_keep_timer_period = 0;
static UINT32 ps_reseted_moniter_flag = 0;
static UINT32 ps_bcn_loss_max_count = 0;


void power_save_td_ck_timer_handler(void *data);
void power_save_keep_timer_handler(void *data);
void power_save_td_timer_stop(void);

int net_if_is_up(void)
{
    return  (mhdr_get_station_status() == MSG_GOT_IP);
}

void power_save_wakeup_isr(void)
{
}

void power_save_dtim_wake(UINT32 status )
{
    if(bk_ps_mode == PS_DTIM_PS_MODE &&
            bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_NONE)
    {
        UINT32 reg;

        if(status )
        {
            if((status) & MAC_ARM_WAKEUP_EN_BIT)
            {
                bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_UPING;
                reg = REG_READ(ICU_INTERRUPT_ENABLE);
                reg &= ~(CO_BIT(FIQ_MAC_WAKEUP));
                REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
                PS_DEBUG_UP_TRIGER;
#if 1

                if( bk_ps_info.pwm0_less_time != 0)
                {
                    if(bk_ps_info.pwm0_clkmux == PWM_MUX_PCLK)
                    {
                        ps_pwm0_reconfig(bk_ps_info.pwm0_less_time, bk_ps_info.pwm0_clkmux);
                        bk_ps_info.tm_status = TM_LOW_POWER;
                        pwm0_use_for_ps = 1;
                    }
                    else if(bk_ps_info.pwm0_clkmux == PWM_MUX_LPO)
                    {
                        UINT32 cur_tick = fclk_get_tick();
                        ps_delay_rfup_time = cur_tick + bk_ps_info.pwm0_less_time / 2;

                        if(ps_delay_rfup_time < cur_tick)
                        {
                            power_save_ieee_dtim_wakeup();
                            ps_delay_rfup_time = 0;
                        }
                    }
                    else
                    {
                        bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
                        power_save_ieee_dtim_wakeup();
                    }

                    bk_ps_info.pwm0_less_time = 0;
                }
                else
                {
                    bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
                    power_save_ieee_dtim_wakeup();
                }

#else
                power_save_ieee_dtim_wakeup();
#endif
            }
        }
    }
}

void power_save_pwm0_isr(UINT8 param)
{
    if(bk_ps_mode == PS_DTIM_PS_MODE)
    {
        if (bk_ps_info.tm_status == TM_LOW_POWER)
        {
            GLOBAL_INT_DECLARATION();
            GLOBAL_INT_DISABLE();
            bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
            power_save_ieee_dtim_wakeup();
            ps_pwm0_resume_tick();
            pwm0_use_for_ps = 0;
            bk_ps_info.tm_status = PS_TM_CK_TIMEOUT;
            GLOBAL_INT_RESTORE();
        }
    }
}

UINT32 power_save_use_timer0(void)
{
    return (pwm0_use_for_ps == 1);
}

void power_save_td_check(void)
{
    int32 err;
    /*receive UC or manual wakeup send*/
    if((PS_DPSM_STATE_GET(PAUSE)
            || PS_DPSM_STATE_GET(PAUSING) || PS_DPSM_STATE_GET(RESUMING))
            || bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_USER)
    {
        ps_run_td_timer();
    }
}

/*This function will run in mac go to ps fiq,
only an actual emergency can put here,
can't operate wifi tx,rx,modem,rf here*/
void power_save_gops_wait_idle_int_cb(void)
{
    //rf_ps_wakeup_isr_idle_int_cb();
}

bool power_save_sleep(void)
{
    UINT32 ret = false;
    UINT32 reg;
    UINT16        sleep_time;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(1 == bk_ps_info.ps_real_sleep)
    {
        GLOBAL_INT_RESTORE();
        return ret;
    }

    if(! (PS_STA_DTIM_CAN_SLEEP))
    {
        GLOBAL_INT_RESTORE();
        return ret;
    }

    if(rwnxl_get_status_in_doze())
    {
        GLOBAL_INT_RESTORE();
        return ret;
    }

    nxmac_enable_idle_interrupt_setf(1);
    PS_DEBUG_CK_TRIGER;

    if(REG_READ((ICU_BASE + 19 * 4))
            & (CO_BIT(FIQ_MAC_TX_RX_MISC)
               | CO_BIT(FIQ_MAC_TX_RX_TIMER)
               | CO_BIT(FIQ_MAC_RX_TRIGGER)
               | CO_BIT(FIQ_MAC_TX_TRIGGER)
               | CO_BIT(FIQ_MAC_PROT_TRIGGER)
              ))
    {
        GLOBAL_INT_RESTORE();
        return ret;
    }

    reg = REG_READ(ICU_INTERRUPT_ENABLE);
    //reg |= (CO_BIT(FIQ_MAC_WAKEUP));
    reg &= ~(CO_BIT(FIQ_MAC_TX_RX_MISC)
             | CO_BIT(FIQ_MAC_TX_RX_TIMER)
             | CO_BIT(FIQ_MAC_RX_TRIGGER)
             | CO_BIT(FIQ_MAC_TX_TRIGGER)
             | CO_BIT(FIQ_MAC_GENERAL)
             | CO_BIT(FIQ_MAC_PROT_TRIGGER)
             | CO_BIT(IRQ_SARADC)
             | CO_BIT(FIQ_DPLL_UNLOCK));
    REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
#if NX_POWERSAVE
    ret = rwnxl_sleep(power_save_gops_wait_idle_int_cb, power_save_mac_idle_callback);

    if(false == ret)
    {
        PS_PRT("can't ps\r\n");
        reg = REG_READ(ICU_INTERRUPT_ENABLE);
        reg &= ~(CO_BIT(FIQ_MAC_WAKEUP));
        reg |= (CO_BIT(FIQ_MAC_TX_RX_MISC)
                | CO_BIT(FIQ_MAC_TX_RX_TIMER)
                | CO_BIT(FIQ_MAC_RX_TRIGGER)
                | CO_BIT(FIQ_MAC_TX_TRIGGER)
                | CO_BIT(FIQ_MAC_GENERAL)
                | CO_BIT(FIQ_MAC_PROT_TRIGGER)
                | CO_BIT(IRQ_SARADC)
                | CO_BIT(FIQ_DPLL_UNLOCK));
        REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
        GLOBAL_INT_RESTORE();
        return ret;
    }

#endif

    if(ps_lock)
        ps_lock --;
    else
    {
        PS_WPRT("error ps\r\n");
        GLOBAL_INT_RESTORE();
        return ret;
    }

    PS_WPRT("go ps\r\n");
#if CFG_USE_STA_PS
    power_save_sleep_status_set();
    sctrl_sta_rf_sleep();
#if CFG_USE_BLE_PS
    ble_switch_rf_to_ble();
#endif
    reg = REG_READ(ICU_INTERRUPT_ENABLE);
    reg |= (CO_BIT(FIQ_MAC_WAKEUP));
    REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
#endif
    if(1 == ps_keep_timer_status)
    {
        bmsg_ps_sender(PS_BMSG_IOCTL_RF_KP_STOP);
    }

    GLOBAL_INT_RESTORE();
    return true;
}

/*time = BI*1024*LIST*0.016*/
void power_save_wkup_time_cal(UINT8 sleep_int)
{
    UINT64 s_less_time = 0;
    UINT32 tmp_r_wkup = r_wakeup_time + 12;
    UINT32 reg;

#if 0 //ignore,may case bcn period drift
    /*the error_range_time ,unit:ns*/
    s_less_time = sleep_int * bk_ps_info.ps_beacon_int * 15000; //ns36 //16384

    if(sleep_int * bk_ps_info.ps_beacon_int > 100)
    {
        s_less_time = (s_less_time * 990) / 1000;
    }

    //s_less_time -= 120000;//-delay(2)

    /*if error_range_time > radio_wake_up_time ,unit:ns*/
    if((s_less_time) >= tmp_r_wkup * 32000)
    {
        /*merge radio_wake_up_time to error_range_time*/
        s_less_time -= tmp_r_wkup * 32000;

        if(s_less_time > 2000000)//500000
        {
            bk_ps_info.pwm0_less_time = (s_less_time) / 1000000; //(s_less_time)/31250; ms
            bk_ps_info.pwm0_clkmux = PWM_MUX_LPO;
        }
        else
        {
            bk_ps_info.pwm0_less_time = (s_less_time * 100) / 3846;
            bk_ps_info.pwm0_clkmux = PWM_MUX_PCLK;
        }

        tmp_r_wkup = 0;
    }
    else
    {
        /*only have radio_wake_up_time*/
        tmp_r_wkup -= s_less_time / 32000;
        bk_ps_info.pwm0_less_time = 0;
    }
#endif

    nxmac_radio_wake_up_time_setf(tmp_r_wkup);
}

int power_save_get_wkup_less_time()
{
    if(bk_ps_info.liston_mode == PS_LISTEN_MODE_DTIM)
    {
        return bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi \
               *bk_ps_info.ps_beacon_int * 15;
    }
    else
    {
        return bk_ps_info.liston_int * bk_ps_info.ps_beacon_int * 15;
    }
}

void power_save_mac_idle_callback(void)
{
    if(power_save_if_sleep_first())
    {
#if (CFG_SOC_NAME == SOC_BK7231)
        power_save_wkup_time_cal(1);
        nxmac_tsf_mgt_disable_setf(0);
        nxmac_listen_interval_setf(1);
#else
        nxmac_radio_wake_up_time_setf(r_wakeup_time);
        nxmac_dtim_period_setf(bk_ps_info.ps_dtim_period);
        delay(1);
        nxmac_dtim_updated_by_sw_setf(1);
        nxmac_tsf_mgt_disable_setf(0);
        nxmac_wakeup_dtim_setf(1);
#endif

        nxmac_atim_w_setf(512);
        nxmac_wake_up_sw_setf(0);
        /*first clear beacon interval,delay,then set beacon interval,to fix rw sleep wakeup time*/
        nxmac_beacon_int_setf(0);
        delay(1);
        nxmac_beacon_int_setf(bk_ps_info.ps_beacon_int);
        os_printf(" sleep_first %d\r\n", bk_ps_info.liston_mode);
        os_printf(" dtim period:%d multi:%d\r\n", bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_multi);
        bk_ps_info.sleep_first = 0;
    }
#if (CFG_SOC_NAME == SOC_BK7231)
    else
    {
        if(bk_ps_info.liston_mode == PS_LISTEN_MODE_DTIM)
        {
            if(bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_USER
                    && bk_ps_info.waited_beacon == STA_GET_FALSE)
            {
                power_save_wkup_time_cal(1);
                nxmac_listen_interval_setf(1);
            }
            else if(bk_ps_info.ps_dtim_count == 0)
            {
                power_save_wkup_time_cal(bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi);
                nxmac_listen_interval_setf(bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi);
            }
            else
            {
                power_save_wkup_time_cal(bk_ps_info.ps_dtim_period * (bk_ps_info.ps_dtim_multi - 1) + bk_ps_info.ps_dtim_count);
                nxmac_listen_interval_setf(bk_ps_info.ps_dtim_period * (bk_ps_info.ps_dtim_multi - 1) + bk_ps_info.ps_dtim_count);
            }
        }
        else
        {
        }

    }
#endif

    bk_ps_info.sleep_count ++;
}


void power_save_sleep_status_set(void)
{
    bk_ps_info.ps_real_sleep = 1;
    bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_NONE;
}

UINT8 power_save_set_all_vif_prevent_sleep(UINT32 prevent_bit )
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx = 0xff;
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];

        if(vif_entry->active && vif_entry->type == VIF_STA)
        {
            vif_entry->prevent_sleep |= prevent_bit;
        }
    }

    return 0;
}

/*This function will run in mac wakeup fiq,
only an actual emergency can put here,
can't operate wifi tx,rx,modem,rf here*/
void power_save_wkup_wait_idle_int_cb(void)
{
    //rf_ps_wakeup_isr_idle_int_cb();
}

UINT8 power_save_clr_all_vif_prevent_sleep(UINT32 prevent_bit )
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx = 0xff;
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];

        if(vif_entry->active && vif_entry->type == VIF_STA)
        {
            vif_entry->prevent_sleep &= ~ (prevent_bit);
        }
    }

    return 0;
}
void power_save_wakeup(void)
{
    UINT32 reg;
    PS_DEBUG_UP_TRIGER;
    bk_ps_info.waited_beacon = STA_GET_FALSE;

    if(bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_RW)
    {
        power_save_set_all_vif_prevent_sleep((UINT32)(PS_VIF_WAITING_BCN));
    }

#if CFG_USE_STA_PS
    sctrl_sta_rf_wakeup();
#if CFG_USE_BLE_PS
    rf_wifi_used_set();
#endif
    reg = REG_READ(ICU_ARM_WAKEUP_EN); 
    reg &= ~(MAC_ARM_WAKEUP_EN_BIT);
    REG_WRITE(ICU_ARM_WAKEUP_EN, reg);    
#endif
    bk_ps_info.tm_status = PS_TM_CK_TIMEOUT;
#if NX_POWERSAVE
    rwnxl_wakeup(power_save_wkup_wait_idle_int_cb);
#endif
    reg = REG_READ(ICU_INTERRUPT_ENABLE);
    reg &= ~(CO_BIT(FIQ_MAC_WAKEUP));
    reg |= (CO_BIT(FIQ_MAC_TX_RX_MISC)
            | CO_BIT(FIQ_MAC_TX_RX_TIMER)
            | CO_BIT(FIQ_MAC_RX_TRIGGER)
            | CO_BIT(FIQ_MAC_TX_TRIGGER)
            | CO_BIT(FIQ_MAC_GENERAL)
            | CO_BIT(FIQ_MAC_PROT_TRIGGER)
            | CO_BIT(IRQ_SARADC)
            | CO_BIT(FIQ_DPLL_UNLOCK));
    REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
    PS_DEBUG_UP_TRIGER;

    ASSERT(!ps_lock);
    ps_lock ++;
}

void power_save_dtim_exit_check()
{
    if(power_save_wkup_event_get() & NEED_DISABLE_BIT)
    {
        power_save_dtim_rf_ps_disable_send_msg();
        power_save_wkup_event_clear(NEED_DISABLE_BIT);
    }
}

void power_save_ieee_dtim_wakeup(void)
{
    if((bk_ps_info.ps_arm_wakeup_way >  PS_ARM_WAKEUP_NONE
            && bk_ps_info.ps_arm_wakeup_way <= PS_ARM_WAKEUP_USER)
            && bk_ps_info.ps_real_sleep)
    {
        PS_DEBUG_UP_TRIGER;
        power_save_wakeup();

        if(!bk_ps_info.ps_real_sleep)
            os_printf("ps r s not 0\r\n");

        bk_ps_info.ps_real_sleep = 0;
        bk_ps_info.ps_can_sleep = 1;
#if PS_DTIM_PERI_WAKE_DELAY

        if(bk_ps_info.ps_arm_wakeup_way == PS_DTIM_ARM_WAKEUP_PERI)
        {
            os_printf("w:peri wake\r\n");

            if(bk_ps_info.PsPeriWakeupWaitTimeMs != 0)
            {
                ps_delay_wait_time = fclk_get_tick() + bk_ps_info.PsPeriWakeupWaitTimeMs / 2;
                os_printf("uart wake delay %d %d\r\n", bk_ps_info.PsPeriWakeupWaitTimeMs, ps_delay_wait_time);
            }
        }

#endif
        if(!power_save_if_sleep_first() && ps_keep_timer_period)
        {
            bmsg_ps_sender(PS_BMSG_IOCTL_RF_KP_SET);
        }
        power_save_dtim_exit_check();
    }
}


bool power_save_rf_sleep_check( void )
{
#if (NX_POWERSAVE)
#if CFG_USE_STA_PS
#if PS_WAKEUP_MOTHOD_RW
    if(PS_STA_DTIM_SWITCH)
    {
        UINT32 tick;
        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();

        if((ps_delay_rfup_time != 0) && ((INT32)(ps_delay_rfup_time - fclk_get_tick()) <= 0))
        {
            if(bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_UPING)
            {
                bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
                power_save_ieee_dtim_wakeup();
            }

            ps_delay_rfup_time = 0;
        }

        if(ps_td_last_tick &&
                (power_save_beacon_state_get() == STA_GET_TRUE    \
                 || power_save_wkup_way_get() == PS_ARM_WAKEUP_USER))
        {
            {
                power_save_td_check();
                ps_td_last_tick = 0;
            }
        }

        GLOBAL_INT_RESTORE();
    }

    if(PS_STA_DTIM_CAN_SLEEP)
    {
        GLOBAL_INT_DECLARATION();

        if (ke_evt_get() != 0)
        {
            return false;
        }

        if(!bmsg_is_empty())
        {
            return false;
        }

        GLOBAL_INT_DISABLE();
        ps_sleep_check();
        GLOBAL_INT_RESTORE();
    }

#endif
#endif
#endif //(NX_POWERSAVE)
}

void power_save_me_ps_first_set_state(UINT8 state)
{
    int param_len;
    struct ke_msg *kmsg_dst;
    struct me_set_ps_disable_req *me_ps_ptr;
    os_printf("%s:%d \r\n", __FUNCTION__, __LINE__);
    param_len = sizeof(struct me_set_ps_disable_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);

    if(0 == kmsg_dst)
    {
        os_printf("%s:%d malloc fail\r\n", __FUNCTION__, __LINE__);
        return ;
    }

    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));
    kmsg_dst->id = ME_PS_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = TASK_NONE;
    kmsg_dst->param_len = param_len;
    me_ps_ptr = (struct me_set_ps_disable_req *)kmsg_dst->param;
    me_ps_ptr->ps_disable = state;
    ke_msg_send(ke_msg2param(kmsg_dst));
}


void power_save_me_ps_set_state(UINT8 state , UINT8 vif_idx)
{
    int param_len;
    struct ke_msg *kmsg_dst;
    struct me_set_ps_disable_req *me_ps_ptr;
    os_printf("%s:%d \r\n", __FUNCTION__, __LINE__);
    {
        struct me_set_ps_disable_req *me_ps_ptr = KE_MSG_ALLOC(ME_SET_PS_DISABLE_REQ, TASK_ME, TASK_NONE,
                me_set_ps_disable_req);
        me_ps_ptr->ps_disable = state;
        me_ps_ptr->vif_idx = vif_idx;
        ke_msg_send(me_ps_ptr);
    }
}

void power_save_sm_set_bcmc(UINT8 bcmc , UINT8 vif_idx)
{
    struct mm_set_ps_options_req *req;
    // Get a pointer to the kernel message
    req = KE_MSG_ALLOC(MM_SET_PS_OPTIONS_REQ, TASK_MM, TASK_NONE, mm_set_ps_options_req);
    // Fill the message parameters
    req->dont_listen_bc_mc = bcmc;
    req->listen_interval = 0;
    req->vif_index = vif_idx;
    os_printf("%s %d %d %d\r\n", __FUNCTION__, req->dont_listen_bc_mc,
              req->listen_interval, req->vif_index);
    // Set the PS options for this VIF
    ke_msg_send(req);
}

UINT8 power_save_sm_set_all_bcmc(UINT8 bcmc )
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx = 0xff;
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];

        if(vif_entry->active && vif_entry->type != VIF_STA)
        {
            os_printf("%s:%d %d is %d not STA!!!!\r\n", __FUNCTION__, __LINE__, i, vif_entry->type);
            return 0;
        }
    }

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];

        if(vif_entry->active )
        {
            power_save_sm_set_bcmc(bcmc, i);
        }
    }

    return 0;
}



UINT8 power_save_me_ps_set_all_state(UINT8 state )
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx = 0xff;
    UINT32 i;

    if(state == false)
    {
        for(i = 0; i < NX_VIRT_DEV_MAX; i++)
        {
            vif_entry = &vif_info_tab[i];

            if(vif_entry->active && vif_entry->type != VIF_STA)
            {
                os_printf("%s:%d %d is %d not STA!!!!\r\n", __FUNCTION__, __LINE__, i, vif_entry->type);
                return 0;
            }
        }
    }

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];
        if(vif_entry->type == VIF_STA)
        {
            power_save_me_ps_set_state(state, i);
        }
    }

    return 0;
}

void power_save_timer_init(void)
{
    UINT32 reg, err;

#if CFG_SUPPORT_ALIOS
    if(rtos_is_timer_init(&ps_td_ck_timer))
#else
    if(rtos_is_oneshot_timer_init(&ps_td_ck_timer))
#endif
    {
        power_save_td_timer_stop();
#if CFG_SUPPORT_ALIOS
        rtos_deinit_timer(&ps_td_ck_timer);
#else
        rtos_deinit_oneshot_timer(&ps_td_ck_timer);
#endif
    }

    if(bk_ps_info.PsDataWakeupWaitTimeMs > 0)
    {
#if CFG_SUPPORT_ALIOS
        err = rtos_init_timer(&ps_td_ck_timer,
#else
        err = rtos_init_oneshot_timer(&ps_td_ck_timer,
#endif
                              bk_ps_info.PsDataWakeupWaitTimeMs,
#if CFG_SUPPORT_ALIOS
                              (timer_handler_t)power_save_td_ck_timer_handler,
#else
                              (timer_2handler_t)power_save_td_ck_timer_handler,
                              NULL,
#endif
                              NULL);
        ASSERT(kNoErr == err);
    }
}

void power_save_keep_timer_init(void)
{
    UINT32 reg, err;
    
#if CFG_SUPPORT_ALIOS
    if(rtos_is_timer_init(&ps_keep_timer))
#else
    if(rtos_is_oneshot_timer_init(&ps_keep_timer))
#endif
    {
        power_save_keep_timer_stop();
#if CFG_SUPPORT_ALIOS
        rtos_deinit_timer(&ps_keep_timer);
#else
        rtos_deinit_oneshot_timer(&ps_keep_timer);
#endif
    }
    os_printf("ps_keep_timer init\r\n");

    if(ps_keep_timer_period > 0)
    {
#if CFG_SUPPORT_ALIOS
    err = rtos_init_timer(&ps_keep_timer,
#else
    err = rtos_init_oneshot_timer(&ps_keep_timer,
#endif
                          ps_keep_timer_period,
#if CFG_SUPPORT_ALIOS
                          (timer_handler_t)power_save_keep_timer_handler,
#else
                          (timer_2handler_t)power_save_keep_timer_handler,
                          NULL,
#endif
                          NULL);
    ASSERT(kNoErr == err);
    }
}


void power_save_dtim_ps_init(void)
{
    UINT32 reg, err;

    bk_ps_info.sleep_count = 0;
    bk_ps_info.sleep_first = 1;
    os_printf("power_save_dtim_ps_init\r\n");
    bk_ps_info.ps_can_sleep = 1;
    ps_td_last_tick = 0;

}


void power_save_dtim_ps_exit(void)
{
    UINT32 reg;
    OSStatus ret, err;
    bk_ps_info.ps_can_sleep = 0;
    bk_ps_info.waited_beacon = STA_GET_INIT;
    bk_ps_info.PsDataWakeupWaitTimeMs = 0 ;
    bk_ps_info.PsPeriWakeupWaitTimeMs = 0 ;
    nxmac_beacon_int_setf(0);
    delay(1);
    bk_ps_info.ps_real_sleep = 0;
    bk_ps_info.sleep_count = 0;
    bk_ps_info.sleep_first = 1;
}


int power_save_dtim_enable_handler(void)
{
    UINT32 temp;
    UINT32 wakeup_time = 2;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(bk_ps_mode == PS_DTIM_PS_OPENING)
    {
        os_printf("enter %d ps,p:%d m:%d int:%d l:%d!\r\n", bk_ps_info.liston_mode,
                  bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_multi,
                  bk_ps_info.ps_beacon_int, bk_ps_info.liston_int);
        power_save_set_uart_linger_time(0);
        power_save_dtim_ps_init();
        bk_ps_info.if_wait_bcn = 1;
        bk_ps_mode = PS_DTIM_PS_MODE;
    }

    GLOBAL_INT_RESTORE();
}


int power_save_dtim_enable(void)
{
    if( ! net_if_is_up())
    {
        return -1;
    }

    if(g_wlan_general_param->role != CONFIG_ROLE_STA)
    {
        os_printf("can't dtim,role %d not only sta!\r\n", g_wlan_general_param->role);
        return -1;
    }

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(bk_ps_mode != PS_NO_PS_MODE)
    {
        os_printf("can't dtim ps,ps in mode %d!\r\n", bk_ps_mode);
        GLOBAL_INT_RESTORE();
        return -1;
    }

    {
        os_printf("first enable sleep \r\n");
        power_save_me_ps_first_set_state(PS_MODE_ON_DYN);
    }

    GLOBAL_INT_RESTORE();
    return 0;
}

int power_save_dtim_disable_handler(void)
{
    UINT32 wdt_val = 1;
    FUNCPTR reboot = 0;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(bk_ps_mode == PS_DTIM_PS_CLOSING)
    {
        bk_ps_mode = PS_NO_PS_MODE;

        if(bk_ps_info.ps_real_sleep == 1)
        {
            os_printf("%s:%d err----\r\n", __FUNCTION__, __LINE__);
        }

        rwnxl_set_nxmac_timer_value();
        power_save_dtim_ps_exit();

        if(power_save_wkup_event_get() & NEED_REBOOT_BIT)
        {
#if CFG_SUPPORT_BOOTLOADER
            os_printf("wdt reboot\r\n");
            sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &wdt_val);
            sddev_control(WDT_DEV_NAME, WCMD_POWER_UP, NULL);
#else
            os_printf("reboot\r\n");
            (*reboot)();
#endif
        }
    }

    GLOBAL_INT_RESTORE();
    os_printf("exit dtim ps!\r\n");
    return 0;
}

int power_save_dtim_disable(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(bk_ps_mode == PS_DTIM_PS_MODE)
    {
        GLOBAL_INT_RESTORE();
        power_save_me_ps_set_all_state(true);
        os_printf("start exit!\r\n");
        return 0;
    }
    else
    {
        GLOBAL_INT_RESTORE();
    }
}


int power_save_dtim_rf_ps_disable_send_msg(void)
{
    if(bk_ps_mode == PS_DTIM_PS_MODE)
    {
        bmsg_ps_sender(PS_BMSG_IOCTL_RF_DISANABLE);
    }
}

void power_save_rf_dtim_manual_do_wakeup(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    PS_DEBUG_UP_TRIGER;

    if((bk_ps_mode == PS_DTIM_PS_MODE)
            && (bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_NONE
                || bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_UPING)
            && (bk_ps_info.ps_real_sleep == 1))
    {
        delay(1);
        PS_DEBUG_UP_TRIGER;

        if(bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_UPING)
        {
            bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
        }
        else
        {
            bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_USER;
        }

        reg = REG_READ(ICU_INTERRUPT_ENABLE);
        reg &= ~(CO_BIT(FIQ_MAC_WAKEUP));
        REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
        power_save_ieee_dtim_wakeup();
        PS_PRT("m_r_u\r\n");
    }

    GLOBAL_INT_RESTORE();
}

/*@brief set the time how long wait after the last tx/rx.
* @param data_wakeup_time(ms):  if 0 mean wait maximum time,
*                               else wait the set time.
*/
void power_save_set_linger_time(UINT32 data_wakeup_time)
{
    if(data_wakeup_time >= 0 && data_wakeup_time < 100)
    {
        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();
        bk_ps_info.PsDataWakeupWaitTimeMs = data_wakeup_time ;
        power_save_timer_init();
        GLOBAL_INT_RESTORE();
    }
    return;
}

void power_save_set_keep_timer_time(UINT32 time)
{
    if(time >= 0 && time < 100)
    {
        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();
        ps_keep_timer_period = time ;
        power_save_keep_timer_init();
        GLOBAL_INT_RESTORE();
    }
    return;
}

void power_save_set_uart_linger_time(UINT32 uart_wakeup_time)
{
    bk_ps_info.PsPeriWakeupWaitTimeMs = uart_wakeup_time;
    return;
}

void power_save_set_dtim_period(UINT8 period)
{
    if(bk_ps_info.ps_dtim_period != period)
    {
        os_printf("new dtim period:%d\r\n", period);
    }

    bk_ps_info.ps_dtim_period = period;
}

void power_save_set_dtim_count(UINT8 count)
{
    bk_ps_info.ps_dtim_count = count;
}

void power_save_cal_bcn_liston_int(UINT16 bcn_int)
{
    if(bcn_int != 0 )
    {
        bk_ps_info.ps_beacon_int = bcn_int;
#if PS_WAKEUP_MOTHOD_RW

        if(bk_ps_info.ps_beacon_int >= bk_ps_info.sleep_ms)
            bk_ps_info.liston_int = 1;
        else if(bk_ps_info.sleep_ms % bk_ps_info.ps_beacon_int == 0)
            bk_ps_info.liston_int = bk_ps_info.sleep_ms / bk_ps_info.ps_beacon_int;
        else
            bk_ps_info.liston_int = bk_ps_info.sleep_ms / bk_ps_info.ps_beacon_int + 1;

#endif
    }
}

UINT8 power_save_get_liston_int(void)
{
    return bk_ps_info.liston_int;
}

void power_save_td_timer_stop(void)
{
    int32 err;

    GLOBAL_INT_DECLARATION();
#if CFG_SUPPORT_ALIOS
    err = rtos_stop_timer(&ps_td_ck_timer);
#else
    err = rtos_stop_oneshot_timer(&ps_td_ck_timer);
#endif
    ASSERT(kNoErr == err);
    GLOBAL_INT_DISABLE();
    ps_td_ck_timer_status = 0;
    GLOBAL_INT_RESTORE();
}

void power_save_td_ck_timer_real_handler(void *data)
{
    OSStatus err;
    power_save_td_timer_stop();
    ps_td_last_tick = 1;

#if CFG_USE_STA_PS
    extern void bmsg_null_sender(void);
    bmsg_null_sender();
#endif

}

void power_save_td_ck_timer_handler(void *data)
{
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_TD_HANDLER);
}

void power_save_delay_sleep_check(void)
{
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_TD_SET);
}

void power_save_td_ck_timer_set(void)
{
    OSStatus err;

#if CFG_SUPPORT_ALIOS
    if(rtos_is_timer_init(&ps_td_ck_timer) && ps_td_ck_timer_status == 0)
#else
    if(rtos_is_oneshot_timer_init(&ps_td_ck_timer) && ps_td_ck_timer_status == 0)
#endif
    {
        ps_td_ck_timer_status = 1;
#if CFG_SUPPORT_ALIOS
        err = rtos_start_timer(&ps_td_ck_timer);
#else
        err = rtos_start_oneshot_timer(&ps_td_ck_timer);
#endif
        ASSERT(kNoErr == err);
    }
}

void power_save_keep_timer_stop(void)
{
    OSStatus err;
    GLOBAL_INT_DECLARATION();
#if CFG_SUPPORT_ALIOS
    err = rtos_stop_timer(&ps_keep_timer);
#else
    err = rtos_stop_oneshot_timer(&ps_keep_timer);
#endif
    ASSERT(kNoErr == err);
    GLOBAL_INT_DISABLE();
    ps_keep_timer_status = 0;
    GLOBAL_INT_RESTORE();
}

void power_save_keep_timer_real_handler()
{
    OSStatus err;
    GLOBAL_INT_DECLARATION();

    power_save_keep_timer_stop();

    GLOBAL_INT_DISABLE();
    if(bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_RW && 0 == bk_ps_info.ps_real_sleep)
    {
        if(0 == ps_reseted_moniter_flag 
#if CFG_SUPPORT_ALIOS
        && ps_bcn_loss_max_count < PS_BCN_MAX_LOSS_LIMIT
#endif
        )
        {
            bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_USER;
            power_save_clr_all_vif_prevent_sleep((UINT32)(PS_VIF_WAITING_BCN));
            ps_bcn_loss_max_count ++;
#if CFG_USE_BLE_PS
            rf_wifi_used_clr();
#endif
        }
        else
        {
            ps_reseted_moniter_flag = 0;
        }
        GLOBAL_INT_RESTORE();

#if CFG_USE_STA_PS
        extern void bmsg_null_sender(void);
        bmsg_null_sender();
#endif
    }
    else
    {
        GLOBAL_INT_RESTORE();
    }

}

void power_save_keep_timer_handler(void *data)
{
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_KP_HANDLER);
}

void power_save_keep_timer_set(void)
{
    OSStatus err;

#if CFG_SUPPORT_ALIOS
    if(rtos_is_timer_init(&ps_keep_timer) && ps_keep_timer_status == 0)
#else
    if(rtos_is_oneshot_timer_init(&ps_keep_timer) && ps_keep_timer_status == 0)
#endif
    {
        ps_keep_timer_status = 1;
#if CFG_SUPPORT_ALIOS
        err = rtos_start_timer(&ps_keep_timer);
#else
        err = rtos_start_oneshot_timer(&ps_keep_timer);
#endif
        ASSERT(kNoErr == err);
    }
}


void power_save_rf_ps_wkup_semlist_init(void)
{
    co_list_init(&bk_ps_info.wk_list);
}

void power_save_rf_ps_wkup_semlist_wait(void)
{
    UINT32 ret;
    PS_DO_WKUP_SEM *sem_list = (PS_DO_WKUP_SEM *) os_malloc(sizeof(PS_DO_WKUP_SEM));
#if CFG_SUPPORT_ALIOS
    ret = rtos_init_semaphore(&sem_list->wkup_sema, 0);
#else
    ret = rtos_init_semaphore(&sem_list->wkup_sema, 1);
#endif
    ASSERT(0 == ret);
    co_list_push_back(&bk_ps_info.wk_list, &sem_list->list);
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_USER_WKUP);

    if(sem_list)
    {
        rtos_get_semaphore(&sem_list->wkup_sema, BEKEN_NEVER_TIMEOUT);
        ASSERT(0 == ret);
        ret = rtos_deinit_semaphore(&sem_list->wkup_sema);
        ASSERT(0 == ret);
        os_free(sem_list);
        sem_list = NULL;
    }
    else
    {
        os_printf("err ---- NULL\r\n");
    }
}

void power_save_rf_ps_wkup_semlist_set(void)
{
    UINT32 ret;

    while (!co_list_is_empty(&bk_ps_info.wk_list))
    {
        PS_DO_WKUP_SEM *sem_list;
        sem_list = list2sem(co_list_pop_front(&bk_ps_info.wk_list));
        ret = rtos_set_semaphore(&sem_list->wkup_sema);
        ASSERT(0 == ret);
    }
}

void power_save_beacon_state_set(PS_STA_BEACON_STATE state)
{
    bk_ps_info.waited_beacon = state;
}

void power_save_beacon_state_update(void)
{
    PS_DEBUG_RX_TRIGER;
    
#if CFG_USE_BLE_PS
    rf_wifi_used_clr();
#endif
    if(PS_STA_DTIM_SWITCH)
    {
        if(power_save_if_ps_can_sleep()
                && power_save_beacon_state_get() == STA_GET_INIT)
        {
            power_save_beacon_state_set(STA_GET_FALSE);
        }
    }

    if(PS_STA_DTIM_SWITCH
            && (power_save_beacon_state_get() != STA_GET_TRUE)
      )
    {
        power_save_beacon_state_set(STA_GET_TRUE);
        ps_bcn_loss_max_count = 0;
    }
}

UINT8 power_save_if_sleep_first(void)
{
    return bk_ps_info.sleep_first;
}

PS_STA_BEACON_STATE power_save_beacon_state_get(void)
{
    return bk_ps_info.waited_beacon;
}

PS_ARM_WAKEUP_WAY power_save_wkup_way_get(void)
{
    return bk_ps_info.ps_arm_wakeup_way;
}


UINT8 power_save_if_ps_can_sleep(void)
{
    return (bk_ps_info.ps_can_sleep == 1);
}

INT8 power_save_if_sleep_at_first(void)
{
    return (bk_ps_info.sleep_count < 6);
}

UINT32 power_save_get_sleep_count(void)
{
    return bk_ps_info.sleep_count;
}



UINT8 power_save_if_ps_rf_dtim_enabled(void)
{
    return (bk_ps_mode == PS_DTIM_PS_MODE);
}

void power_save_ps_mode_set(PS_MODE_STATUS mode)
{
    bk_ps_mode = mode;
}

UINT16 power_save_radio_wkup_get(void)
{
    return r_wakeup_time;
}

void power_save_radio_wkup_set(UINT16 time)
{
    r_wakeup_time = time;
}

UINT32 power_save_wkup_event_get(void)
{
    return ps_dis_flag;
}

void power_save_wkup_event_set(UINT32 value)
{
    ps_dis_flag |= value;
}

void power_save_wkup_event_clear(UINT32 value)
{
    ps_dis_flag &= ~value;
}

UINT16 power_save_beacon_len_get(void)
{
    return beacon_len;
}

void power_save_beacon_len_set(UINT16 len)
{
    beacon_len = len + 4/*fcs*/ /*+25 radiotap*/;
}

void power_save_set_reseted_flag(void)
{
    ps_reseted_moniter_flag = 1;
}

UINT32 power_save_get_bcn_lost_count(void)
{
    return ps_bcn_loss_max_count;
}

UINT8 power_save_set_dtim_multi(UINT8 multi)
{
    bk_ps_info.ps_dtim_multi = multi;

    if(bk_ps_info.ps_dtim_multi > 0 && bk_ps_info.ps_dtim_multi < 100)
    {
        os_printf("set listen dtim:%d\r\n", bk_ps_info.ps_dtim_multi);
    }
    else
    {
        os_printf("set listen dtim:%d err,use default 1\r\n", bk_ps_info.ps_dtim_multi);
        bk_ps_info.ps_dtim_multi = 1;
    }

    bk_ps_info.liston_mode = PS_LISTEN_MODE_DTIM;
}

UINT16 power_save_forbid_trace(PS_FORBID_STATUS forbid)
{
    bk_forbid_count ++;

    if(bk_forbid_code != forbid || (bk_forbid_count % 100 == 0))
    {
        PS_PRT("front c:%d\r\n\r\n", bk_forbid_count);
        PS_PRT("ps_cd:%d %d\r\n", bk_forbid_code,forbid);
        bk_forbid_count = 0;
    }

    bk_forbid_code = forbid;
    return bk_forbid_count;
}

void power_save_dump(void)
{
    UINT32 i;
    os_printf("rf:%x\r\n", bk_ps_mode);
    os_printf("info dump\r\n");

    for(i = 0; i < sizeof(bk_ps_info); i++)
        os_printf(" %d 0x%x\r\n", i, *((UINT8 *)(&bk_ps_info) + i));

    os_printf("globel dump\r\n");
    os_printf("%d %d %d %d %d %d\r\n",
              bk_ps_mode,
              mhdr_get_station_status(),
              g_wlan_general_param->role,
              bk_ps_info.waited_beacon,
              bk_ps_info.ps_can_sleep,
              ps_lock);
    os_printf("env dump\r\n");
    os_printf("%d %d %d %d\r\n",
              ps_env.ps_on,
              me_env.ps_on,
              beacon_len,
              txl_cntrl_pck_get());
    #if CFG_USE_MCU_PS
    os_printf("mcu dump\r\n");
    os_printf("%d %d\r\n",              
                peri_busy_count_get(),
                mcu_prevent_get());
    #endif
    
    os_printf("%d %d %d %d %d %d\r\n",
              bk_ps_info.pwm0_less_time, bk_ps_info.pwm0_clkmux,
              bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_count,
              bk_ps_info.ps_dtim_multi, bk_forbid_code);
#if CFG_USE_STA_PS
    sctrl_ps_dump();
#endif
#if CFG_USE_BLE_PS
    ble_ps_dump();
#endif
}

#endif

PS_MODE_STATUS power_save_ps_mode_get(void)
{
    return bk_ps_mode;
}
UINT8 power_save_if_rf_sleep(void)
{
    if(bk_wlan_has_role(VIF_STA))
    {
#if CFG_USE_STA_PS
    if(PS_STA_DTIM_SWITCH)
    {
        if(bk_ps_info.ps_real_sleep == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
#endif
        return 0;
    }
    else
    {
        return 1;
    }
}

// eof


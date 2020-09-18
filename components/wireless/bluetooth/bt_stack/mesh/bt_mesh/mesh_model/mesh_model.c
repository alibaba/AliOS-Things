/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#if 0
#include "network_para.h"
#include "light_base.h"

_TMM_STATE_S g_mesh_state[CONFIG_MESH_ELEM_NUM];
_TMM_POWERUP_S g_mesh_powerup = {
    .last_actual = 0xFFFF,
    .last_temp = CTL_TEMP_MAX,
};
_TMM_STATE_WORK_S g_tmm_state_work;

u16_t TRANS_TIMES[] = {1, 10, 100, 6000};

static u8_t transition_time(u32_t time)
{
    MODEL_D("time(%d)", time);
    
    time /= 100;
    
    if(time > TRANS_TIMES[3] * 62) {
        return 0;
    }
    else if(time > TRANS_TIMES[2] * 62) {
        return (time/TRANS_TIMES[3]) | 0xC0;
    }
    else if(time > TRANS_TIMES[1] * 62) {
        return (time/TRANS_TIMES[2]) | 0x80;
    }
    else if(time > TRANS_TIMES[0] * 62) {
        return (time/TRANS_TIMES[1]) | 0x40;
    }
    else
        return (time/TRANS_TIMES[0]);
}

//unit is 1ms
u32_t get_transition_time(u8_t byte)
{
    if((byte & 0x3F) == 0x3F)
    {
        return 0xFFFFFFFF;
    }
    return (byte & 0x3F) * TRANS_TIMES[byte>>6] * 100;
}

u8_t get_remain_byte(bool is_ack)
{
    u8_t remain_byte = g_mesh_state.trans;
    long long l_ticker = aos_now_ms();
    
    if (!is_ack && g_tmm_state_work.trans_start_time < l_ticker) {
        l_ticker -= g_tmm_state_work.trans_start_time;
        u32_t l_trans = get_transition_time(g_mesh_state.trans);
        if(l_trans == 0xFFFFFFFF) {
            remain_byte = 0x3F;
        }
        else if(l_trans > l_ticker) {
            remain_byte = transition_time(l_trans - l_ticker);
        }
        else {
            remain_byte = 0;
        }
    
    }

    MODEL_D("remain_byte(0x%02x)", remain_byte);
    
    return remain_byte;
}

static aos_timer_t TID_timer;
static void gen_delta_clear(void *arg)
{
    aos_timer_free(&TID_timer);
    g_mesh_state.trans_src = 0;
}

_MESH_ERROR_TYPE mesh_check_TID(u16_t src_addr, u8_t TID)
{
    //MODEL_D("src(0x%04X) TID(0x%02X)", src_addr, TID);
    if(g_mesh_state.trans_id == TID && g_mesh_state.trans_src == src_addr) {
        return MESH_TID_REPEAT;
    }
    else {
        g_mesh_state.trans_id = TID;
        g_mesh_state.trans_src = src_addr;
        aos_timer_new(&TID_timer, gen_delta_clear, NULL, 6000, 0);
    }
    
    return MESH_SUCCESS;
}

void light_publication(void)
{
    struct bt_mesh_elem *p_elem = bt_mesh_elem_find(bt_mesh_primary_addr());
    struct bt_mesh_model *ctl_model = bt_mesh_model_find(p_elem, BT_MESH_MODEL_ID_LIGHT_CTL_SRV);

    ctl_publication(ctl_model);
}

#if 1   //light transtion
static struct k_timer timer_delay;
static struct k_timer light_trans;

_LIGHT_CTL light_ctl;

uint16_t duty_list[] =
{
    #include "debang_duty_list.h"
};
//temperature 800~20000
//ligntness 0~65535
//return duty 1-100
static void get_led_duty(uint16_t *p_duty)
{
    uint8_t cold = 0;
    uint8_t warm = 0;
    uint16_t ln = ((g_mesh_state.actual[T_CUR]-1)*99/65534)+1;

    if(g_mesh_state.actual[T_CUR] == 0) {
        cold = 0;
        warm = 0;
        return;
    }

    cold = (g_mesh_state.temp[T_CUR] - CTL_TEMP_MIN) * 100 / (CTL_TEMP_MAX - CTL_TEMP_MIN);
    warm = 100 - cold;

    if(ln == 1) {
        if(cold || warm) {
            if(cold > warm) {
                cold = 1;
                warm = 0;
            }
            else if(cold == warm) {
                cold = 1;
                warm = 1;
            }
            else {
                cold = 0;
                warm = 1;
            }
        }
    }
    else {
        if(cold) {
            cold = ln * cold / 100;
        }
        if(warm) {
            warm = ln * warm / 100;
        }
    }

    //MODEL_D("index [%d %d]\r\n", cold, warm);
    p_duty[0] = duty_list[cold];
    p_duty[1] = duty_list[warm];

    //MODEL_D("duty [%d %d]\r\n", p_duty[0], p_duty[1]);
}

static void light_gpio(void)
{
    uint16_t duty[2];  //0~100

    get_led_duty(duty);
    //MODEL_D("duty(%d %d)", duty[0], duty[1]);
    
    my_hal_pwm_para_chg(1999-duty[0], 1999-duty[1]);
}

//check tmall mesh model state per 10ms
#define TMALL_MESH_STATE_INTERVAL 10
static struct k_timer tmm_state_timer;

void tmm_state_save(void)
{
    MODEL_D("%d %d", g_mesh_state.actual[T_TAR], g_mesh_state.temp[T_TAR]);
    
    //memset(&g_mesh_state, 0, sizeof(g_mesh_state));
    if(g_mesh_state.actual[T_TAR]) {
        g_mesh_powerup.last_actual = g_mesh_state.actual[T_TAR];
    }
    if(g_mesh_state.temp[T_TAR]) {
        g_mesh_powerup.last_temp = g_mesh_state.temp[T_TAR];
    }
    
    ais_set_para(ALI_MESH_LIGHT_STATE, &g_mesh_powerup, 4);
}

void tmm_state_get(void)
{
    uint8_t len = 0;

    MODEL_D("");
    
    memset(&g_mesh_state, 0, sizeof(g_mesh_state));
    g_mesh_state.temp[T_CUR] = CTL_TEMP_MIN;
    ais_get_para(ALI_MESH_LIGHT_STATE, &g_mesh_powerup, &len);

    //read flash
    //get_powerup_state
    
    g_mesh_state.trans = TMM_TRANS_DEFAULT;

    g_mesh_state.actual[T_TAR] = g_mesh_powerup.last_actual?g_mesh_powerup.last_actual:TMM_ACTUAL_DEFAULT;
    g_mesh_state.temp[T_TAR] = g_mesh_powerup.last_temp?g_mesh_powerup.last_temp:TMM_TEMP_DEFAULT;
    
    MODEL_D("cur(0x%04x 0x%04x)", g_mesh_state.actual[T_CUR], g_mesh_state.temp[T_CUR]);
    MODEL_D("tar(0x%04x 0x%04x)", g_mesh_state.actual[T_TAR], g_mesh_state.temp[T_TAR]);

    //light_publication();
}

void genie_mesh_get_states(void)
{
}
#endif

#define USER_TASK_PERIOD 10 //ms
long long g_user_task_tick = 0;
void light_action(struct bt_mesh_model *model)
{
    //MODEL_D("");
    
    if(g_mesh_state.delay || g_mesh_state.trans) {
        g_tmm_state_work.trans_start_time = g_user_task_tick*USER_TASK_PERIOD;
        
        if(g_mesh_state.delay) {
            g_tmm_state_work.trans_start_time += g_mesh_state.delay*5;
        }
        //MODEL_D("%x %d", g_mesh_state.delay, (u32_t)g_tmm_state_work.trans_start_time);
        
        if(g_mesh_state.trans) {
            g_tmm_state_work.trans_end_time = g_tmm_state_work.trans_start_time + get_transition_time(g_mesh_state.trans);
        }
        //MODEL_D("%x %d", g_mesh_state.trans, (u32_t)g_tmm_state_work.trans_end_time);
    }
}

void light_set_target_with_onoff(u8_t onoff)
{
    g_mesh_state.onoff[T_TAR] = onoff;
    
    if(g_mesh_state.onoff[T_TAR] == 1) {
        if(g_mesh_state.actual[T_CUR] == 0) {
            g_mesh_state.actual[T_TAR] = g_mesh_powerup.last_actual;
            g_mesh_state.temp[T_TAR] = g_mesh_powerup.last_temp;
        }
    }
    else {
        g_mesh_state.actual[T_TAR] = 0;
    }
}

void light_set_target_with_ctl(u16_t lihgtness, u16_t temperature)
{
    g_mesh_powerup.last_actual = g_mesh_state.actual[T_TAR] = lihgtness;
    g_mesh_powerup.last_temp = g_mesh_state.temp[T_TAR] = temperature;
    
    g_mesh_state.onoff[T_TAR] = g_mesh_state.actual[T_TAR]?1:0;
}

#if 0
#define USER_HW_TIMER
static void update_light_state(long long cur_time)
{
    int32_t delta_actual = 0;
    int32_t delta_temp = 0;
    long long step;

    //check time
    if(g_tmm_state_work.trans_start_time == 0 && g_tmm_state_work.trans_end_time == 0) {
        return;
    }

    //MODEL_D("10-> %d %d", (u32_t)g_tmm_state_work.trans_start_time, (u32_t)g_tmm_state_work.trans_end_time);
    //wait delay timeout
    if(g_tmm_state_work.trans_start_time != 0) {
        //MODEL_D("1-> %d %d", (u32_t)cur_time, (u32_t)g_tmm_state_work.trans_start_time);
        if(cur_time < g_tmm_state_work.trans_start_time) {
            return;
        }
        else {
            g_tmm_state_work.trans_start_time = 0;
        }
    }

    //MODEL_D("2.1-> %d %d", g_mesh_state.actual[T_CUR], g_mesh_state.actual[T_TAR]);
    //MODEL_D("2.2-> %d %d", g_mesh_state.temp[T_CUR], g_mesh_state.temp[T_TAR]);
    //compare target & current
    if(g_mesh_state.actual[T_CUR] != g_mesh_state.actual[T_TAR] || g_mesh_state.temp[T_CUR] != g_mesh_state.temp[T_TAR]) {
        delta_actual = g_mesh_state.actual[T_TAR] - g_mesh_state.actual[T_CUR];
        delta_temp = g_mesh_state.temp[T_TAR] - g_mesh_state.temp[T_CUR];
        
        step = g_tmm_state_work.trans_end_time - cur_time;
        if(step <= 0) {
            //MODEL_D("0-> %d %d", (u32_t)g_tmm_state_work.trans_start_time, (u32_t)g_tmm_state_work.trans_end_time);
            g_tmm_state_work.trans_end_time = 0;
            g_mesh_state.actual[T_CUR] = g_mesh_state.actual[T_TAR];
            g_mesh_state.temp[T_CUR] = g_mesh_state.temp[T_TAR];
            tmm_state_save();
        }
        else {
            step /= TMALL_MESH_STATE_INTERVAL;
            //MODEL_D("++ %d", delta_actual);
            delta_actual /= step;
            delta_temp /= step;
            //MODEL_D("-- %d", delta_actual);
            g_mesh_state.actual[T_CUR] += delta_actual;
            g_mesh_state.temp[T_CUR] += delta_temp;
        }
        //MODEL_D("3-> %d %d", (u32_t)cur_time, (u32_t)g_tmm_state_work.trans_end_time);
        light_gpio();
    }
    else
    {
        //MODEL_D("1-> %d %d", (u32_t)g_tmm_state_work.trans_start_time, (u32_t)g_tmm_state_work.trans_end_time);
        if(g_tmm_state_work.trans_end_time != 0) {
            g_tmm_state_work.trans_end_time = 0;
        }
        tmm_state_save();
    }
}


#ifdef USER_HW_TIMER

static void tmall_mesh_cycle(void)
{
    g_user_task_tick++;
    
    //MODEL_D("%d", (uint32_t)g_user_task_tick);

    update_light_state(g_user_task_tick * USER_TASK_PERIOD);
}

#else

static void tmall_mesh_cycle(struct k_work *work)
{
    update_light_state(aos_now_ms());
}
#endif

void tmall_mesh_user_task_init(void)
{
    static uint8_t inited = 0;
    timer_dev_t tim;

    //MODEL_D("");

    //init work
    if(!inited) {
#ifdef USER_HW_TIMER
        tim.port = 4;
        tim.config.period = 1000 * USER_TASK_PERIOD;
        tim.config.reload_mode = TIMER_RELOAD_AUTO;
        tim.config.cb = tmall_mesh_cycle;
        tim.config.arg = NULL;
        hal_timer_init(&tim);
#else
        k_timer_init(&tmm_state_timer, tmall_mesh_cycle, NULL);
#endif
        inited = 1;
    }
    
    //run
#ifdef USER_HW_TIMER
    hal_timer_start(&tim);
#else
    k_timer_start(&tmm_state_timer, 0);
#endif
}


void tmm_state_init(void)
{
    //MODEL_D("");

    //init tmm state        
    tmm_state_get();

    //init work
    tmall_mesh_user_task_init();

    light_action(NULL);
}

void tmm_led_flash(uint8_t times, uint16_t ln)
{
    uint8_t i = 0;

    while(i < times)
    {
        i++;
        g_tmm_state_work.trans_start_time = g_user_task_tick*USER_TASK_PERIOD;
        g_tmm_state_work.trans_end_time = g_tmm_state_work.trans_start_time + 500;
        g_mesh_state.actual[T_TAR] = 65535 - ln;
        while(g_tmm_state_work.trans_end_time) {
            aos_msleep(100);
        }

        g_tmm_state_work.trans_start_time = g_user_task_tick*USER_TASK_PERIOD;
        g_tmm_state_work.trans_end_time = g_tmm_state_work.trans_start_time + 500;
        g_mesh_state.actual[T_TAR] = 65535;
        while(g_tmm_state_work.trans_end_time) {
            aos_msleep(100);
        }
    }
}

#endif

void set_pwm(uint16_t ttt){
    g_tmm_state_work.trans_start_time = g_user_task_tick*10;
    
    g_tmm_state_work.trans_end_time = g_tmm_state_work.trans_start_time + 1000;
    g_mesh_state.actual[T_TAR] = ttt;

}
#endif


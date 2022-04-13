#include <stdlib.h>
#include "aos_common.h"
#include "aos_mem.h"
#include "aos_timer.h"
#include "typedef.h"
/*
os_timer_handle_t _os_timer_create( char *timer_name,u32_t period,u32_t auto_reload,void * const pvTimerID,timer_callback_t func)
{
    int ret = 0;
    os_timer__t *timer;

    //timer = _os_malloc(sizeof(os_timer__t*));
    //NULL_PARA_CHECK(timer,NULL);
    if(auto_reload>0)auto_reload=1;
    if(0 == auto_reload)
    {
        ret = krhino_timer_dyn_create(&timer, timer_name,func,period, 0, pvTimerID, 0);
    }
    else
    {
        ret = krhino_timer_dyn_create(&timer, timer_name,func,period, period, pvTimerID, 0);
    }
    if(0 != ret)
    {
        _os_free(timer);
        timer_debug_error("create timer fail,name(%s),reason:%d\r\n",(timer_name == NULL)?"NULL":timer_name,ret);
        return NULL;
    }
    else
    {
        timer_debug_info("create timer ,handle:%x name:%s period:%d\r\n",timer,(timer_name == NULL)?"NULL":timer_name,period);
        return timer;
    }
}
*/

/* timer just run once after starting it */
#define ESPOS_TIMER_NO_AUTO_RUN   0
/* timer run cycle after starting it */
#define ESPOS_TIMER_AUTO_RUN      (1 << 0)


typedef size_t          espos_size_t;
typedef espos_size_t    espos_tick_t;
typedef uint32_t        espos_opt_t;

void espos_timer_adapter_cb(os_timer_handle_t timer, void *arg)
{
    espos_adapter_timer* espos_handle = arg;
    TimerCallbackFunction_t cb;
    if(arg == NULL){
        return;
    }
    cb = espos_handle->cb;
    if(cb == NULL){
        return;
    }
    cb(espos_handle->arg);
}
/*
os_timer_handle_t espos_timer_create_obj (
    const char *name,
    timer_cb_t cb,
    void *arg,
    espos_tick_t period_ticks,
    espos_opt_t opt
)
{
    //kstat_t ret = RHINO_SUCCESS;
    os_timer_handle_t timer = 0;
    ktimer_t **ptimer = (ktimer_t **)(&timer);
    tick_t round;

    if (opt == ESPOS_TIMER_NO_AUTO_RUN)
        round = 0;
    else if (opt == ESPOS_TIMER_AUTO_RUN)
        round = period_ticks;
    else
        return 0;

    krhino_timer_dyn_create(ptimer, name, (timer_cb_t)cb, period_ticks,
                        round, arg, 0);

    return timer;
}
*/

os_timer_handle_t espos_timer_create_static (
    ktimer_t * ptmr,
    const char *name,
    timer_cb_t cb,
    void *arg,
    espos_tick_t period_ticks,
    espos_opt_t opt
)
{
    //kstat_t ret = RHINO_SUCCESS;
    tick_t round;

    if (opt == ESPOS_TIMER_NO_AUTO_RUN)
        round = 0;
    else if (opt == ESPOS_TIMER_AUTO_RUN)
        round = period_ticks;
    else
        return 0;

    krhino_timer_create(ptmr, name, (timer_cb_t)cb, period_ticks,
                        round, arg, 0);

    return ptmr;
}
/*
os_timer_handle_t espos_timer_create_obj_adapter (
    const char *name,
    TimerCallbackFunction_t cb,
    void *arg,
    espos_tick_t period_ticks,
    espos_opt_t opt
)
{
    espos_adapter_timer *espos_handle;
    espos_handle = malloc(sizeof(espos_adapter_timer));
    if(NULL == espos_handle){
        return 0;
    }
    espos_handle->cb = cb;
    espos_handle->arg = arg;

    return espos_timer_create_obj(name,espos_timer_adapter_cb,espos_handle,period_ticks,opt);
}
*/
os_timer_handle_t espos_timer_create_static_adapter (
    ktimer_t * ptmr,
    const char *name,
    TimerCallbackFunction_t cb,
    void *arg,
    espos_tick_t period_ticks,
    espos_opt_t opt
)
{
    espos_adapter_timer *espos_handle;
    espos_handle = (espos_adapter_timer*)malloc(sizeof(espos_adapter_timer));
    if(NULL == espos_handle){
        return 0;
    }
    espos_handle->cb = cb;
    espos_handle->arg = arg;

    espos_timer_create_static(ptmr,name,espos_timer_adapter_cb,espos_handle,period_ticks,opt);
    return arg;    //arg is adapter timer handle
}



int espos_timer_start(os_timer_handle_t timer_handle)
{
    NULL_PARA_CHECK(timer_handle,os_false);
    int ret = 0;
    const char *timer_name = ((ktimer_t*)timer_handle)->name;
    ret = krhino_timer_start((ktimer_t*)timer_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        timer_debug_error("timer start fail reason:%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
        return os_false;
    }
}

int espos_timer_stop(os_timer_handle_t timer_handle)
{
    NULL_PARA_CHECK(timer_handle,os_false);
    int ret = 0;
    const char *timer_name = ((ktimer_t*)timer_handle)->name;
    ret = krhino_timer_stop((ktimer_t*)timer_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        timer_debug_error("timer stop fail reason:%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
        return os_false;
    }
}

int espos_timer_delete(os_timer_handle_t timer_handle)
{
    NULL_PARA_CHECK(timer_handle,os_false);
    int ret = 0;
    const char *timer_name = ((ktimer_t*)timer_handle)->name;
    krhino_timer_stop(timer_handle);

    if(((ktimer_t*)timer_handle)->timer_cb_arg){
        free(((ktimer_t*)timer_handle)->timer_cb_arg);
    }

    ret = krhino_timer_dyn_del(timer_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        timer_debug_error("timer delete fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
        return os_false;
    }
}

int os_timer_period_change(os_timer_handle_t timer_handle,u32_t new_period,u32_t waite_time, u32_t uxAutoReload)
{
    NULL_PARA_CHECK(timer_handle,os_false);
    int ret = 0;
    u32_t period = (uxAutoReload == os_true)?new_period:0;
    const char *timer_name = ((ktimer_t*)timer_handle)->name;
    krhino_timer_stop(timer_handle);
    ret = krhino_timer_change(timer_handle,new_period,period);
    krhino_timer_start(timer_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        timer_debug_error("timer period change fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
        return os_false;
    }
}
int os_timer_reset(os_timer_handle_t timer_handle)
{
    NULL_PARA_CHECK(timer_handle,os_false);
    int ret = 0;
    char *timer_name = (char*)((ktimer_t*)timer_handle)->name;
    ret = krhino_timer_stop(timer_handle);
    ret = krhino_timer_start(timer_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        timer_debug_error("timer reset fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
        return os_false;
    }
}

void _os_delay_tick(u32_t tick)
{
    krhino_task_sleep(tick);
}

void _os_delay_ms(u32_t ms)
{
    krhino_task_sleep(krhino_ms_to_ticks(ms));
}


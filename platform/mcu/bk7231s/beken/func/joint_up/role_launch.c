#include "include.h"
#include "sys_rtos.h"
#include "role_launch.h"
#include "mem_pub.h"
#include "rtos_pub.h"
#include "error.h"
#include "wlan_ui_pub.h"

#if CFG_ROLE_LAUNCH
JL_T g_role_launch = {0};

uint32_t rl_launch_sta(void)
{
    uint32_t next_launch_flag = 0;
    JL_ENTITY_T *entity, *pre_entity;
    uint32_t ret = LAUNCH_STATUS_OVER;
    
    if(NULL == g_role_launch.jl_previous_sta)
    {
        if(g_role_launch.jl_following_sta)
        {
            pre_entity = g_role_launch.jl_following_sta;
            g_role_launch.jl_previous_sta = pre_entity;
            g_role_launch.jl_following_sta = NULL;

			#if 0
            	rl_pre_sta_init();
            	rl_sta_request_start(&pre_entity->rlaunch);
            #else
				rl_sta_request_start(&pre_entity->rlaunch);
			#endif
			
            ret = LAUNCH_STATUS_CONT;
        }
    }
    else
    {
        if(g_role_launch.jl_following_sta)
        {
            entity = g_role_launch.jl_following_sta;
            if(LAUNCH_TYPE_ASAP == entity->launch_type)
            {
                rl_pre_sta_set_cancel();
            }
        }
        
        pre_entity = g_role_launch.jl_previous_sta;
        if(pre_entity->relaunch_limit 
                && (pre_entity->launch_count >= pre_entity->relaunch_limit))
        {
            rl_pre_sta_set_cancel();
        }

        next_launch_flag = rl_sta_may_next_launch();
        if(next_launch_flag)
        {
            rl_pre_sta_stop_launch();
            
            pre_entity = g_role_launch.jl_previous_sta;
            rl_free_entity(pre_entity);
                
            pre_entity = g_role_launch.jl_following_sta;
            g_role_launch.jl_previous_sta = pre_entity;
            g_role_launch.jl_following_sta = NULL;

            if(pre_entity)
            {   
                #if 0
            		rl_pre_sta_init();
            		rl_sta_request_start(&pre_entity->rlaunch);
	            #else
					rl_sta_request_start(&pre_entity->rlaunch);
				#endif
                
                ret = LAUNCH_STATUS_CONT;
            }
        }
        else
        {
            ret = LAUNCH_STATUS_CONT;
        }
    }
    
launch_exit:    
    return ret;
}

uint32_t rl_launch_ap(void)
{
    uint32_t cancel_pre_flag = 0;
    uint32_t next_launch_flag = 0;
    JL_ENTITY_T *entity, *pre_entity;
    uint32_t ret = LAUNCH_STATUS_OVER;
    
    if(NULL == g_role_launch.jl_previous_ap)
    {
        if(g_role_launch.jl_following_ap)
        {
            pre_entity = g_role_launch.jl_following_ap;
            g_role_launch.jl_previous_ap = pre_entity;
            g_role_launch.jl_following_ap = NULL;

            rl_pre_ap_init();
            rl_ap_request_start(&pre_entity->rlaunch);
            
            ret = LAUNCH_STATUS_CONT;
        }
    }
    else
    {
        if(g_role_launch.jl_following_ap)
        {
            entity = g_role_launch.jl_following_ap;
            if(LAUNCH_TYPE_ASAP == entity->launch_type)
            {
                cancel_pre_flag = 1;
                rl_pre_ap_set_cancel();
            }
        }
        
        pre_entity = g_role_launch.jl_previous_ap;
        if(pre_entity->relaunch_limit 
                && (pre_entity->launch_count >= pre_entity->relaunch_limit))
        {
            cancel_pre_flag = 1;
            rl_pre_ap_set_cancel();
        }

        next_launch_flag = rl_ap_may_next_launch(cancel_pre_flag);
        if(next_launch_flag)
        {
            rl_pre_ap_stop_launch();
            
            pre_entity = g_role_launch.jl_previous_ap;
            rl_free_entity(pre_entity);
                
            pre_entity = g_role_launch.jl_following_ap;
            g_role_launch.jl_previous_ap = pre_entity;
            g_role_launch.jl_following_ap = NULL;

            if(pre_entity)
            {
                rl_pre_ap_init();
                rl_ap_request_start(&pre_entity->rlaunch);
                
                ret = LAUNCH_STATUS_CONT;
            }
        }
        else
        {
            ret = LAUNCH_STATUS_CONT;
        }
    }
    
launch_exit:    
    return ret;
}

uint32_t rl_relaunch_chance(void)
{
	ASSERT(rtos_is_oneshot_timer_init(&g_role_launch.rl_timer));
    
	rtos_oneshot_reload_timer(&g_role_launch.rl_timer);
}

void rl_launch_handler(void *left, void *right)
{
    uint32_t ap_ret = LAUNCH_STATUS_OVER;
    uint32_t sta_ret = LAUNCH_STATUS_OVER;
    
    switch(g_role_launch.pre_entity_type)
    {
        case PRE_ENTITY_AP:
            ap_ret = rl_launch_ap();
            if(LAUNCH_STATUS_OVER == ap_ret)
            {
                g_role_launch.pre_entity_type = PRE_ENTITY_STA;
                
                sta_ret = rl_launch_sta();
            }
            break;
            
        case PRE_ENTITY_STA:
            sta_ret = rl_launch_sta();
            if(LAUNCH_STATUS_OVER == sta_ret)
            {
                g_role_launch.pre_entity_type = PRE_ENTITY_AP;
                
                ap_ret = rl_launch_ap();
            }
            break;
            
        case PRE_ENTITY_IDLE:
        default:
            break;
    }

    if((LAUNCH_STATUS_OVER == sta_ret)
        && (LAUNCH_STATUS_OVER == ap_ret))
    {
        rl_stop();
    }
    else
    {
        rl_relaunch_chance();
    }
}
       
void rl_init(void)
{
    OSStatus err = kNoErr;
    
	err = rtos_init_oneshot_timer(&g_role_launch.rl_timer, 
									RL_LAUNCH_PERIOD, 
									(timer_2handler_t)rl_launch_handler, 
									NULL, 
									NULL);
	ASSERT(kNoErr == err); 
    g_role_launch.rl_timer_flag = RL_TIMER_INIT;
}

void rl_uninit(void)
{    
    OSStatus err = kNoErr;

    if(RL_TIMER_START == g_role_launch.rl_timer_flag)
    {
        rl_stop();
    }
    
    err = rtos_deinit_oneshot_timer(&g_role_launch.rl_timer);
    ASSERT(kNoErr == err);  
    g_role_launch.rl_timer_flag = RL_TIMER_UNINIT;   
}

void rl_start(void)
{    
    OSStatus err = kNoErr;

    JL_PRT("rl_start\r\n");
    if(0 == rtos_is_oneshot_timer_running(&g_role_launch.rl_timer))
    {
        err = rtos_start_oneshot_timer(&g_role_launch.rl_timer);
        ASSERT(kNoErr == err);
        JL_PRT("rl_start_timer\r\n");
        
        g_role_launch.rl_timer_flag = RL_TIMER_START;
    }
}

void rl_stop(void)
{    
    OSStatus err = kNoErr;
    err = rtos_stop_oneshot_timer(&g_role_launch.rl_timer);
    ASSERT(kNoErr == err);   
    
    g_role_launch.rl_timer_flag = RL_TIMER_STOP;
}

JL_ENTITY_T *rl_alloc_entity(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion)
{
    JL_ENTITY_T *entity;

    entity = (JL_ENTITY_T *)os_zalloc(sizeof(JL_ENTITY_T));
    ASSERT(entity);

    entity->completion_cb = completion;
    entity->launch_type = LAUNCH_TYPE_ASAP;
    os_memcpy(&entity->rlaunch, param, sizeof(LAUNCH_REQ));
    
    return entity;
}

void rl_free_entity(JL_ENTITY_T *d)
{
    os_free(d);
}

uint32_t rl_pre_sta_stop_launch(void)
{
}

uint32_t rl_pre_ap_stop_launch(void)
{
}

uint32_t rl_sta_may_next_launch(void)
{
    uint32_t yes = 0;

    if((RL_STATUS_STA_CSA_LAUNCHED_UNCERTAINTY != (rl_pre_sta_get_status() & RL_STATUS_OTHER_MASK)) 
            && ((rl_pre_sta_get_status() & RL_STATUS_CANCEL)
        || (RL_STATUS_STA_LAUNCHED == rl_pre_sta_get_status())))
    {
        yes = 1;
    }
	else
	{
		if(RL_STATUS_STA_LAUNCH_FAILED == rl_pre_sta_get_status())
		{
			yes = 2;
		}
		
	}
    return yes;
}

uint32_t rl_ap_may_next_launch(uint32_t pre_cancel)
{
    uint32_t yes = 0;

    if(0 == pre_cancel)
    {
        yes = 1;
        goto exit_check;
    }
    
    if(((RL_STATUS_AP_TRANSMITTED_BCN + RL_STATUS_CANCEL) == rl_pre_ap_get_status())
        || (RL_STATUS_AP_LAUNCHED == (rl_pre_ap_get_status() & (~RL_STATUS_CANCEL))))
    {
        yes = 1;
    }
    
exit_check:    
    return yes;
}

uint32_t rl_pre_sta_get_status(void)
{
    return g_role_launch.pre_sta_status;
}

uint32_t rl_pre_ap_get_status(void)
{
    return g_role_launch.pre_ap_status;
}

uint32_t rl_pre_sta_set_status(uint32_t status)
{
    uint32_t cancel = 0;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    if((RL_STATUS_STA_CHANNEL_SWITCHING == g_role_launch.pre_sta_status) 
            && (RL_STATUS_STA_LAUNCHED == status))
    {
        g_role_launch.pre_sta_status = RL_STATUS_STA_CSA_LAUNCHED_UNCERTAINTY;
    }
    else if((RL_STATUS_STA_CSA_LAUNCHED_UNCERTAINTY == g_role_launch.pre_sta_status) 
            && (RL_STATUS_STA_CHANNEL_SWITCHED== status))
    {
        g_role_launch.pre_sta_status = RL_STATUS_STA_LAUNCHED;
    }
    else
    {
        g_role_launch.pre_sta_status = status;
    }
    
    cancel = g_role_launch.pre_sta_cancel;
    if(cancel)
    {
        g_role_launch.pre_sta_status += RL_STATUS_CANCEL;
    }
    GLOBAL_INT_RESTORE();
    
    return cancel;
}

uint32_t rl_pre_ap_disable_autobcn(void)
{
    if(g_role_launch.pre_ap_cancel)
    {
        mm_hw_ap_disable();
    }
    
    return g_role_launch.pre_ap_cancel;
}

uint32_t rl_pre_ap_set_status(uint32_t status)
{
    uint32_t cancel = 0;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    g_role_launch.pre_ap_status = status;
    cancel = g_role_launch.pre_ap_cancel;
    if(cancel)
    {
        g_role_launch.pre_ap_status += RL_STATUS_CANCEL;
    }

    GLOBAL_INT_RESTORE();
    
    return cancel;
}

void rl_pre_sta_set_cancel(void)
{
    g_role_launch.pre_sta_cancel = 1;
}

void rl_pre_sta_clear_cancel(void)
{
    g_role_launch.pre_sta_cancel = 0;
}

uint32_t fl_get_pre_sta_cancel_status(void)
{
    return g_role_launch.pre_sta_cancel;
}

void rl_pre_ap_set_cancel(void)
{
    g_role_launch.pre_ap_cancel = 1;
}

void rl_pre_ap_clear_cancel(void)
{
    g_role_launch.pre_ap_cancel = 0;
}

uint32_t fl_get_pre_ap_cancel_status(void)
{
    return g_role_launch.pre_ap_cancel;
}

void rl_pre_ap_init(void)
{
    g_role_launch.pre_ap_cancel = 0;
    g_role_launch.pre_ap_status = 0;
}

void rl_pre_sta_init(void)
{
    g_role_launch.pre_sta_cancel = 0;
    g_role_launch.pre_sta_status = 0;
}

void rl_sta_request_start(LAUNCH_REQ *req)
{
    extern void demo_scan_app_init(void);

    ASSERT(req);
    
    switch(req->req_type)
    {
        case LAUNCH_REQ_STA:
            rl_pre_sta_init();
            bk_wlan_start_sta(&req->descr);
            break;
            
        case LAUNCH_REQ_PURE_STA_SCAN:
			rl_pre_sta_init();
            demo_scan_app_init();
            break;
            
        case LAUNCH_REQ_DELIF_STA:
            bk_wlan_stop(STATION);
            break;
            
        default:
            break;
    }
}

void rl_ap_request_start(LAUNCH_REQ *req)
{
    ASSERT(req);
    
    switch(req->req_type)
    {
        case LAUNCH_REQ_AP:
            rl_pre_ap_init();
            bk_wlan_start_ap(&req->descr);
            break;
            
        case LAUNCH_REQ_DELIF_AP:
            bk_wlan_stop(SOFT_AP);
            break;
            
        default:
            break;
    }
}

void rl_sta_request_enter(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion)
{
    JL_ENTITY_T *entity = 0;
    GLOBAL_INT_DECLARATION();
	
//	if(bk_wlan_is_monitor_mode()&&(LAUNCH_REQ_PURE_STA_SCAN == param->req_type))
//	{
//		os_printf("monitor (ie.airkiss) is not finish yet, stop it or waiting it finish!\r\n");
//		return;
//	}

    GLOBAL_INT_DISABLE();
    if((0 == g_role_launch.jl_previous_ap)
        && (0 == g_role_launch.jl_previous_sta))
    {
        entity = rl_alloc_entity(param, completion);

        g_role_launch.jl_previous_sta = entity;
        
        JL_PRT("rl_sta_start\r\n");
        rl_sta_request_start(param);
    }
    else if(0 == g_role_launch.jl_following_sta)
    {
        entity = rl_alloc_entity(param, completion);
        
        g_role_launch.jl_following_sta = entity;
        rl_start();
    }
	else
	{
		os_printf("cmd queue fill!\n");
	}

    if(entity
        && (PRE_ENTITY_IDLE == g_role_launch.pre_entity_type))
    {
        g_role_launch.pre_entity_type = PRE_ENTITY_STA;
    }
    GLOBAL_INT_RESTORE();
}

void rl_ap_request_enter(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion)
{
    JL_ENTITY_T *entity = 0;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    if((0 == g_role_launch.jl_previous_ap)
        && (0 == g_role_launch.jl_previous_sta))
    {
        entity = rl_alloc_entity(param, completion);
        
        g_role_launch.jl_previous_ap = entity;
        
        JL_PRT("rl_ap_start\r\n");
        rl_ap_request_start(param);
    }
    else if(0 == g_role_launch.jl_following_ap)
    {
        entity = rl_alloc_entity(param, completion);
        
        g_role_launch.jl_following_ap = entity;
        rl_start();
    }

    if(entity
        && (PRE_ENTITY_IDLE == g_role_launch.pre_entity_type))
    {
        g_role_launch.pre_entity_type = PRE_ENTITY_AP;
    }
    GLOBAL_INT_RESTORE();
}

#endif // CFG_ROLE_LAUNCH

// eof


#include "include.h"
#include "arm_arch.h"


#include "target_util_pub.h"
#include "mem_pub.h"
#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "saradc_pub.h"
#include "uart_pub.h"

#if (!CFG_SUPPORT_ALIOS)
#include "sys_rtos.h"
#endif
#include "rtos_pub.h"
#include "error.h"
#include "fake_clock_pub.h"

#include "bk7011_cal_pub.h"
#include "temp_detect_pub.h"
#include "temp_detect.h"
#include "power_save_pub.h"

// for single temp dectect
saradc_desc_t tmp_single_desc;
UINT16 tmp_single_buff[ADC_TEMP_BUFFER_SIZE];
volatile DD_HANDLE tmp_single_hdl = DD_HANDLE_UNVALID;
beken_semaphore_t tmp_single_semaphore = NULL;

extern void manual_cal_tmp_pwr_init(UINT16 init_temp, UINT16 init_thre, UINT16 init_dist);
extern void turnoff_PA_in_temp_dect(void);
extern void turnon_PA_in_temp_dect(void);
static void temp_single_get_desc_init(void);

#if CFG_USE_TEMPERATURE_DETECT
volatile DD_HANDLE tmp_detect_hdl = DD_HANDLE_UNVALID;
saradc_desc_t tmp_detect_desc;
UINT16 tmp_detect_buff[ADC_TEMP_BUFFER_SIZE];
TEMP_DETECT_CONFIG_ST g_temp_detect_config;
#if CFG_SUPPORT_ALIOS
beken_thread_t  temp_detct_handle = NULL;
#else
xTaskHandle  temp_detct_handle = NULL;
#endif

enum
{
	TMPD_PAUSE_TIMER          = 0,
    TMPD_RESTART_TIMER,    
    TMPD_CHANGE_PARAM,
    TMPD_TIMER_POLL,    
    TMPD_INT_POLL,
	TMPD_EXIT,
};

typedef struct temp_message 
{
	u32 temp_msg;
}TEMP_MSG_T;

#define TEMP_DET_QITEM_COUNT          (5)

beken_queue_t tempd_msg_que = NULL;


static void temp_detect_handler(void);
static void temp_detect_main( beken_thread_arg_t data );
extern void rwnx_cal_do_temp_detect(UINT16 cur_val, UINT16 thre, UINT16 *last);

static void temp_detect_desc_init(void)
{
    tmp_detect_desc.has_data                = 0;
    tmp_detect_desc.current_sample_data_cnt = 0;
}

static void temp_detect_enable_config_sysctrl(void)
{
    UINT32 param;

    param = BLK_BIT_TEMPRATURE_SENSOR;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);
}

static void temp_detect_disable_config_sysctrl(void)
{
    UINT32 param;
    param = BLK_BIT_TEMPRATURE_SENSOR;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_DISABLE, &param);
}

void temp_detect_send_msg(u32 new_msg)
{
	OSStatus ret;
	TEMP_MSG_T msg;

    if(tempd_msg_que) {
    	msg.temp_msg = new_msg;
    	
    	ret = rtos_push_to_queue(&tempd_msg_que, &msg, BEKEN_NO_WAIT);
    	if(kNoErr != ret)
    	{
    		os_printf("temp_detect_send_msg failed\r\n");
    	}
    }
}


UINT32 temp_detect_init(UINT32 init_val)
{   
    int ret;

    TMP_DETECT_FATAL("temp_detect_init %d\r\n", init_val);
	
    if((!temp_detct_handle) && (!tempd_msg_que))
    {

    	ret = rtos_init_queue(&tempd_msg_que, 
    							"temp_det_queue",
    							sizeof(TEMP_MSG_T),
    							TEMP_DET_QITEM_COUNT);
    	if (kNoErr != ret) 
    	{
    		TMP_DETECT_FATAL("temp detect ceate queue failed\r\n");
            return kGeneralErr;
    	}
        
        ret = rtos_create_thread(&temp_detct_handle,
                                      BEKEN_DEFAULT_WORKER_PRIORITY,
                                      "temp_detct",
                                      (beken_thread_function_t)temp_detect_main,
                                      1024,
                                      (beken_thread_arg_t)init_val);
        if (ret != kNoErr)
        {
            rtos_deinit_queue(&tempd_msg_que);
            tempd_msg_que = NULL;
            TMP_DETECT_FATAL("Error: Failed to create temp_detect: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

UINT32 temp_detect_uninit(void)
{
    int ret;
    
    if((temp_detct_handle) && (tempd_msg_que))
    {
        //temp_detect_send_msg(TMPD_EXIT);
        
        // wait untill task exit
        //while(temp_detct_handle)
        //    rtos_delay_milliseconds(100);    
        
        // set reg mod & pa to initial value, this must be happened in
        // txevm or rxsens to calibration txpwr or rxsens
        manual_cal_temp_pwr_unint();    
    }
    
}

void temp_detect_pause_timer(void)
{
    int ret;
       
    if(g_temp_detect_config.detect_timer.function 
        && rtos_is_timer_running(&g_temp_detect_config.detect_timer)) 
    {
        temp_detect_send_msg(TMPD_PAUSE_TIMER);
    }
}

void temp_detect_restart_detect(void)
{
    int ret;
    
    if(g_temp_detect_config.detect_timer.function && 
        !rtos_is_timer_running(&g_temp_detect_config.detect_timer)) 
    {
        temp_detect_send_msg(TMPD_RESTART_TIMER);
    }
}

UINT32 temp_detect_is_opened_saradc(void) 
{
    // if saradc is opened, idle hook to do sleep may turn off saradc's icu clk and inter enbit
    // so before sleep, should check this bit

    return (DD_HANDLE_UNVALID != tmp_detect_hdl);
}

UINT32 temp_detect_is_init(void)
{
    return ((temp_detct_handle) && (tempd_msg_que));
}

static UINT32 temp_detect_open(void)
{
    UINT32 status;
    GLOBAL_INT_DECLARATION();

#if (CFG_SOC_NAME == SOC_BK7231)    
    turnoff_PA_in_temp_dect();
#endif // (CFG_SOC_NAME == SOC_BK7231)
    GLOBAL_INT_DISABLE();
    tmp_detect_hdl = ddev_open(SARADC_DEV_NAME, &status, (UINT32)&tmp_detect_desc);
    status = status;
    if(DD_HANDLE_UNVALID == tmp_detect_hdl)
    {
        GLOBAL_INT_RESTORE();
        return SARADC_FAILURE;
    }
    
    GLOBAL_INT_RESTORE();

    return SARADC_SUCCESS;
}

static UINT32 temp_detect_close(void)
{
    UINT32 status;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    status = ddev_close(tmp_detect_hdl);
    if(DRV_FAILURE == status )
    {
        GLOBAL_INT_RESTORE();
        return SARADC_FAILURE;
    }
    tmp_detect_hdl = DD_HANDLE_UNVALID;
    GLOBAL_INT_RESTORE();

    return SARADC_SUCCESS;
}

static UINT32 temp_detect_enable(void)
{
    UINT32 err = SARADC_SUCCESS;
    
    if(tmp_detect_hdl != DD_HANDLE_UNVALID) 
	{   
        //aready enable saradc, so return no err
        os_printf("aready open\r\n");
        return SARADC_SUCCESS;
    }

    while(tmp_single_hdl !=  DD_HANDLE_UNVALID)
    {
        rtos_delay_milliseconds(10);;      
    }

    err = rtos_stop_timer(&g_temp_detect_config.detect_timer);
    ASSERT(kNoErr == err);
    TMP_DETECT_PRT("stop detect timer, start ADC\r\n");

    temp_detect_desc_init();
    temp_detect_enable_config_sysctrl();

    err = temp_detect_open();
    if(err == SARADC_FAILURE)
    {
        temp_detect_disable_config_sysctrl();
		
        TMP_DETECT_FATAL("Can't open saradc, have you register this device?\r\n");
        return err;
    }
    TMP_DETECT_PRT("saradc_open is ok \r\n");

    return SARADC_SUCCESS;
}

static void temp_detect_disable(void)
{
    temp_detect_close();
    temp_detect_disable_config_sysctrl();
    TMP_DETECT_PRT("saradc_open is close \r\n");
}

static void temp_detect_timer_handler(void *data)
{
    temp_detect_send_msg(TMPD_TIMER_POLL);
}

static void temp_detect_timer_poll(void)
{   
    OSStatus err;    
	
    if((temp_detect_enable() != SARADC_SUCCESS))
    {
        err = rtos_reload_timer(&g_temp_detect_config.detect_timer);
        TMP_DETECT_PRT("temp_detect_enable failed, restart detect timer, \r\n");  
    } 
}

static void temp_detect_polling_handler(void)
{
    OSStatus err;
    UINT16 cur_val, thre;
    
    #if (CFG_SOC_NAME != SOC_BK7231)
    UINT16 last_detect_val;
    cur_val = tmp_detect_desc.pData[0];
    #else
    cur_val = tmp_detect_desc.pData[ADC_TEMP_BUFFER_SIZE-1];
    #endif // (CFG_SOC_NAME != SOC_BK7231)

    thre = g_temp_detect_config.detect_thre;

    g_temp_detect_config.detect_intval_change++;

    TMP_DETECT_PRT("%d:%d seconds: last:%d, cur:%d, thr:%d\r\n",
                    g_temp_detect_config.detect_intval,
                    ADC_TMEP_DETECT_INTVAL_CHANGE,
                    g_temp_detect_config.last_detect_val,
                    cur_val,
                    g_temp_detect_config.detect_thre);

    rwnx_cal_do_temp_detect(cur_val, thre, &g_temp_detect_config.last_detect_val);

    #if 0 //(CFG_SOC_NAME != SOC_BK7231)
	// bk7231u no need xtal cali
    if(last_detect_val != g_temp_detect_config.last_detect_val)
        manual_cal_do_xtal_cali(cur_val, &g_temp_detect_config.last_xtal_val, 
            g_temp_detect_config.xtal_thre_val, g_temp_detect_config.xtal_init_val);
    #endif // (CFG_SOC_NAME != SOC_BK7231)

    if(g_temp_detect_config.detect_intval_change == ADC_TMEP_DETECT_INTVAL_CHANGE) 
    {
        temp_detect_send_msg(TMPD_CHANGE_PARAM);
    }
    else 
    {
        err = rtos_reload_timer(&g_temp_detect_config.detect_timer);
        ASSERT(kNoErr == err);
    }
}

static void temp_detect_main( beken_thread_arg_t data )
{
    OSStatus err;

    os_memset(&tmp_detect_buff[0], 0, sizeof(UINT16)*ADC_TEMP_BUFFER_SIZE);

    saradc_config_param_init(&tmp_detect_desc);
    tmp_detect_desc.channel = ADC_TEMP_SENSER_CHANNEL;
    tmp_detect_desc.pData = &tmp_detect_buff[0];
    tmp_detect_desc.data_buff_size = ADC_TEMP_BUFFER_SIZE;
    tmp_detect_desc.p_Int_Handler = temp_detect_handler;   
    
    g_temp_detect_config.last_detect_val = (UINT32)(data);
    g_temp_detect_config.inital_data = (UINT32)(data) + ADC_TMEP_DIST_INTIAL_VAL;        
    g_temp_detect_config.detect_thre = ADC_TMEP_LSB_PER_10DEGREE * ADC_TMEP_10DEGREE_PER_DBPWR;
    g_temp_detect_config.detect_intval = ADC_TMEP_DETECT_INTVAL_INIT;
    g_temp_detect_config.detect_intval_change = 0;
    g_temp_detect_config.dist_inital = ADC_TMEP_DIST_INTIAL_VAL;

    g_temp_detect_config.last_xtal_val = (UINT32)(data);
    #if (CFG_SOC_NAME != SOC_BK7231)
    g_temp_detect_config.xtal_thre_val = ADC_XTAL_DIST_INTIAL_VAL;
    g_temp_detect_config.xtal_init_val = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_XTALH_CTUNE, NULL);
    os_printf("xtal inital:%d, %d, %d\r\n", g_temp_detect_config.last_xtal_val,
        g_temp_detect_config.xtal_thre_val, g_temp_detect_config.xtal_init_val);
    #endif // (CFG_SOC_NAME != SOC_BK7231)

	err = rtos_init_timer(&g_temp_detect_config.detect_timer, 
							g_temp_detect_config.detect_intval * 1000, 
							temp_detect_timer_handler, 
							(void *)0);
    ASSERT(kNoErr == err);
	err = rtos_start_timer(&g_temp_detect_config.detect_timer);

	ASSERT(kNoErr == err);

    while(1)
    {
        TEMP_MSG_T msg;
        err = rtos_pop_from_queue(&tempd_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
        	switch(msg.temp_msg) 
            {
                case TMPD_PAUSE_TIMER:
                    {
                        os_printf("pause_detect\r\n");
                        err = rtos_stop_timer(&g_temp_detect_config.detect_timer);
                        ASSERT(kNoErr == err);  
                    }
                    break;
                case TMPD_RESTART_TIMER:
                    {
                        os_printf(" restart detect timer\r\n");
                        err = rtos_reload_timer(&g_temp_detect_config.detect_timer);
                        ASSERT(kNoErr == err);
                    }
                    break;
                case TMPD_CHANGE_PARAM:
                    {
                        temp_detect_change_configuration(ADC_TMEP_DETECT_INTVAL,
                            g_temp_detect_config.detect_thre, ADC_TMEP_DIST_INTIAL_VAL);
                    }
                    break;
                case TMPD_TIMER_POLL:
                    {
                        temp_detect_timer_poll();
                    }
                    break;
                case TMPD_INT_POLL:
                    temp_detect_polling_handler();
                    break;
                case TMPD_EXIT:
                    goto tempd_exit;
                    break;
                default:
                    break;
            }
        }
    }

tempd_exit:
    err = rtos_deinit_timer(&g_temp_detect_config.detect_timer);
    ASSERT(kNoErr == err);
 
    rtos_deinit_queue(&tempd_msg_que);
    tempd_msg_que = NULL;

    temp_detct_handle = NULL;
    rtos_delete_thread(NULL);
}

static void temp_detect_handler(void)
{
    if(tmp_detect_desc.current_sample_data_cnt >= tmp_detect_desc.data_buff_size) 
    {
        #if (CFG_SOC_NAME != SOC_BK7231)
        UINT32 sum = 0, sum1, sum2;
        //turnon_PA_in_temp_dect();
        temp_detect_disable();
        TMP_DETECT_PRT("buff:%p,%d,%d,%d,%d,%d\r\n", tmp_detect_desc.pData,
                       tmp_detect_desc.pData[0], tmp_detect_desc.pData[1],
                       tmp_detect_desc.pData[2], tmp_detect_desc.pData[3], 
                       tmp_detect_desc.pData[4]);
        sum1 = tmp_detect_desc.pData[1] + tmp_detect_desc.pData[2];
        sum2 += tmp_detect_desc.pData[3] + tmp_detect_desc.pData[4];
        sum = sum1 / 2 + sum1 / 2;        
        sum = sum / 2;
        tmp_detect_desc.pData[0] = sum;
        #else
        turnon_PA_in_temp_dect();
        TMP_DETECT_PRT("buff:%p,%d,%d,%d,%d,%d\r\n", tmp_detect_desc.pData,
                       tmp_detect_desc.pData[0], tmp_detect_desc.pData[1],
                       tmp_detect_desc.pData[2], tmp_detect_desc.pData[3], 
                       tmp_detect_desc.pData[4]);
        temp_detect_disable();
        #endif // (CFG_SOC_NAME != SOC_BK7231)
        temp_detect_send_msg(TMPD_INT_POLL);
    }
}

void temp_detect_change_configuration(UINT32 intval, UINT32 thre, UINT32 dist)
{
    UINT32 interval;
    OSStatus err;

    if(intval == 0)
        intval = ADC_TMEP_DETECT_INTVAL;
    if(thre == 0)
        thre = ADC_TMEP_LSB_PER_10DEGREE * ADC_TMEP_10DEGREE_PER_DBPWR;
    if(dist == 0)
        dist = ADC_TMEP_DIST_INTIAL_VAL;

    TMP_DETECT_WARN("config: intval:%d, thre:%d, dist:%d\r\n", intval, thre, dist);

    if((g_temp_detect_config.detect_thre != thre) 
        || (g_temp_detect_config.dist_inital != dist))
    {     
        if(g_temp_detect_config.detect_thre != thre)
            g_temp_detect_config.detect_thre = thre;

        if(g_temp_detect_config.dist_inital != dist)
            g_temp_detect_config.dist_inital = dist;      
        
        manual_cal_tmp_pwr_init(g_temp_detect_config.inital_data, 
            g_temp_detect_config.detect_thre, g_temp_detect_config.dist_inital);
    }
 
    if(g_temp_detect_config.detect_intval != intval) 
    {
        g_temp_detect_config.detect_intval = intval;

        if(g_temp_detect_config.detect_timer.function) {
            err = rtos_deinit_timer(&g_temp_detect_config.detect_timer); 
            ASSERT(kNoErr == err); 
        } 
        
    	err = rtos_init_timer(&g_temp_detect_config.detect_timer, 
    							g_temp_detect_config.detect_intval * 1000, 
    							temp_detect_timer_handler, 
    							(void *)0);
        ASSERT(kNoErr == err);

    	err = rtos_start_timer(&g_temp_detect_config.detect_timer);
    	ASSERT(kNoErr == err); 
    }
}

UINT32 temp_get_detect_time(void)
{
    return rtos_get_timer_expiry_time(&g_temp_detect_config.detect_timer);
}

////////////////////////////////////////////////////////////////////////
#endif  // CFG_USE_TEMPERATURE_DETECT

static UINT32 temp_single_get_enable(void)
{
    UINT32 status;
    GLOBAL_INT_DECLARATION();
    
#if CFG_USE_TEMPERATURE_DETECT
    while(tmp_detect_hdl !=  DD_HANDLE_UNVALID)
    {
        rtos_delay_milliseconds(10);     
    }
#endif
    temp_single_get_desc_init();

    status = BLK_BIT_TEMPRATURE_SENSOR;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &status);

#if (CFG_SOC_NAME == SOC_BK7231)
    turnoff_PA_in_temp_dect();
#endif // (CFG_SOC_NAME == SOC_BK7231)
    GLOBAL_INT_DISABLE();
    tmp_single_hdl = ddev_open(SARADC_DEV_NAME, &status, (UINT32)&tmp_single_desc);
    if(DD_HANDLE_UNVALID == tmp_single_hdl)
    {
        GLOBAL_INT_RESTORE();
        TMP_DETECT_FATAL("Can't open saradc, have you register this device?\r\n");
        return SARADC_FAILURE;
    }   
    GLOBAL_INT_RESTORE();
    
    return SARADC_SUCCESS;
}

static void temp_single_get_disable(void)
{
    UINT32 status;
    
    status = ddev_close(tmp_single_hdl);
    if(DRV_FAILURE == status )
    {
        TMP_DETECT_PRT("saradc disable failed\r\n");
        return;
    }
    tmp_single_hdl = DD_HANDLE_UNVALID;
    
    status = BLK_BIT_TEMPRATURE_SENSOR;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_DISABLE, &status);
    
    TMP_DETECT_PRT("saradc_open is close \r\n");
}

static void temp_single_detect_handler(void)
{
    if(tmp_single_desc.current_sample_data_cnt >= tmp_single_desc.data_buff_size) 
    {
        #if (CFG_SOC_NAME != SOC_BK7231)
        UINT32 sum = 0, sum1, sum2;
        //turnon_PA_in_temp_dect();
        temp_single_get_disable();
        TMP_DETECT_PRT("buff:%p,%d,%d,%d,%d,%d\r\n", tmp_single_desc.pData,
                       tmp_single_desc.pData[0], tmp_single_desc.pData[1],
                       tmp_single_desc.pData[2], tmp_single_desc.pData[3], 
                       tmp_single_desc.pData[4]);

        sum1 = tmp_single_desc.pData[1] + tmp_single_desc.pData[2];
        sum2 += tmp_single_desc.pData[3] + tmp_single_desc.pData[4];
        sum = sum1 / 2 + sum1 / 2;        
        sum = sum / 2;
        tmp_single_desc.pData[0] = sum;
        #else
        turnon_PA_in_temp_dect();
        temp_single_get_disable();
        TMP_DETECT_PRT("buff:%p,%d,%d,%d,%d,%d\r\n", tmp_single_desc.pData,
                       tmp_single_desc.pData[0], tmp_single_desc.pData[1],
                       tmp_single_desc.pData[2], tmp_single_desc.pData[3], 
                       tmp_single_desc.pData[4]);
        #endif // (CFG_SOC_NAME != SOC_BK7231)
        
        rtos_set_semaphore(&tmp_single_semaphore);
    }
}

static void temp_single_get_desc_init(void)
{
    os_memset(&tmp_single_buff[0], 0, sizeof(UINT16)*ADC_TEMP_BUFFER_SIZE);

    saradc_config_param_init(&tmp_single_desc);
    tmp_single_desc.channel = ADC_TEMP_SENSER_CHANNEL;
    tmp_single_desc.pData = &tmp_single_buff[0];
    tmp_single_desc.data_buff_size = ADC_TEMP_BUFFER_SIZE;
    tmp_single_desc.p_Int_Handler = temp_single_detect_handler;
}

UINT32 temp_single_get_current_temperature(UINT32 *temp_value)
{
    UINT32 ret;
    int result;
    *temp_value = 0;

    if(tmp_single_semaphore == NULL) {
#if CFG_SUPPORT_ALIOS
        result = rtos_init_semaphore(&tmp_single_semaphore, 0);
#else
        result = rtos_init_semaphore(&tmp_single_semaphore, 1);
#endif
        ASSERT(kNoErr == result);
    }
    
    temp_single_get_enable();
    
    ret = 1000; // 1s
    result = rtos_get_semaphore(&tmp_single_semaphore, ret);
    if(result == kNoErr) {
        #if (CFG_SOC_NAME != SOC_BK7231)
        *temp_value = tmp_single_desc.pData[0];
        #else
        *temp_value = tmp_single_desc.pData[4];
        #endif
        ret = 0;        
    }else {
        TMP_DETECT_FATAL("temp_single timeout\r\n");
        ret = 1;
    }
        
    return ret;
}

//EOF



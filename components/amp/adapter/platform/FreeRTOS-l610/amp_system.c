/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

#include "fibocom.h"
#include "amp_system.h"

#if defined(CONFIG_FIBOCOM_ALIOS)
#include "osi_api.h"
#include "osi_log.h"

#include "cfw_event.h"
#include "osi_event_hub.h"
#include "at_command.h"
#include "at_cfg.h"
#include "at_response.h"

#include "und_hal.h"
#include "und_config.h"

#include "cfw.h"
#include "nvm.h"
#define PLATFORM_WAIT_INFINITE (~0)
#define DEFAULT_TASK_PRIORITY 24
#define SYSINFO_VERSION "0.0.1"
#define SYSINFO_DEVICE_NAME "Freertos"

#endif


#if defined(CONFIG_FIBOCOM_ALIOS)
osiThread_t *aliAmpThreadID = NULL;

typedef struct
{
	uint8_t cid;
    uint8_t nSim;
    uint16_t uti;
}fibo_ali_callinfo_st;

#define ERR_SUCCESS 0
#define FIBO_ALI_TCPIP_PDP_ACT 1
#define FIBO_ALI_TCPIP_PDP_DEACT 2

fibo_ali_callinfo_st g_fibo_ali_callinfo = {0};
void *g_fibo_alios_jsengine_thread_handle = NULL;
void *g_fibo_alios_network_thread_handle = NULL;
#endif

extern char *g_cgmm;
extern char *g_cgmr;

void *HAL_Malloc(uint32_t size)
{
    return malloc(size);
}

void HAL_Free(void *ptr)
{
    free(ptr);
}

void *HAL_Calloc(uint32_t nitems, uint32_t size)
{
    return calloc((size_t)nitems, (size_t)size);
}

void *HAL_Realloc(void *ptr, uint32_t size)
{
    return realloc(ptr, (size_t)size);
}

uint64_t HAL_UptimeMs(void)
{
	struct timeval timeVal;
	int ret = 0;
	uint64_t timeMs = 0;

	memset(&timeVal, 0, sizeof(struct timeval));

   	ret = gettimeofday(&timeVal, NULL);
	if (ret != 0)
	{
		return 0;
	}
	timeMs = timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;

    return timeMs;
}

void HAL_SleepMs(uint32_t ms)
{
    osiThreadSleep(ms);
}

void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return random();
}

void HAL_Printf(const char *fmt, ...)
{
    va_list args;
	char buffer[256] = {0};
    va_start(args, fmt);
	vsnprintf(buffer, 256, fmt, args);
	OSI_PRINTFI("%s", buffer);
    va_end(args);

}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Reboot(void)
{
    osiShutdown(OSI_SHUTDOWN_RESET);
}

void *HAL_SemaphoreCreate(void)
{
    return (void*)(osiSemaphoreCreate(1,0));
}

void HAL_SemaphoreDestroy(void *sem)
{
    osiSemaphoreDelete(sem);
}

void HAL_SemaphorePost(void *sem)
{
    osiSemaphoreRelease(sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    return (int)(osiSemaphoreTryAcquire(sem, timeout_ms));
}

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            amp_os_thread_param_t *amp_os_thread_param,
            int *stack_used)
{
	osiThread_t * task_handle = NULL;
	if (thread_handle == NULL || amp_os_thread_param == NULL) {
		return -1;
	}
	
	task_handle = osiThreadCreate(amp_os_thread_param->name, (osiCallback_t)work_routine, arg, 
									amp_os_thread_param->priority, amp_os_thread_param->stack_size, 0);

	if (task_handle == NULL) {
		return -1;
	}
	
    if (stack_used) {
        *stack_used = 0;
    }

	*thread_handle = task_handle;
    return 0;
}

int HAL_ThreadDelete(void *thread_handle)
{
	vTaskDelete(thread_handle);
}

int HAL_GetDefaultTaskPriority()
{
    return DEFAULT_TASK_PRIORITY;
}

void *HAL_MutexCreate(void)
{
    return osiMutexCreate();
}

void HAL_MutexDestroy(void *mutex)
{
    osiMutexDelete((osiMutex_t *)mutex);
}

void HAL_MutexLock(void *mutex)
{
    osiMutexLock((osiMutex_t *)mutex);
}

void HAL_MutexUnlock(void *mutex)
{
    osiMutexUnlock((osiMutex_t *)mutex);
}

extern osiThread_t *ali_amp_get_TaskID();

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
	osiTimer_t * timer  = NULL;

    /* check parameter */
    if (func == NULL) {
        return NULL;
    }
    timer = osiTimerCreate(atEngineGetThreadId(), func, user_data);
    if (timer == NULL) 
    {
        return NULL;
    }

    return (void *)timer;
}

void *HAL_GetTimerPrams(void *timerHandle)
{
    return timerHandle;
}

int32_t HAL_Timer_Start(void *timer, int ms, amp_timer_type repeat)
{
    int ret = -1;
    if(repeat == TimerRunPeriodic)
    {
        ret = osiTimerStartPeriodic((osiTimer_t *)timer, ms);
    }
    else
    {
    	  ret = osiTimerStart((osiTimer_t *)timer, ms);
    }
    if(ret)
    {
         ret = 0;
    }
    else
    {
        ret  = -1;
    }
    return ret;
}

int32_t HAL_Timer_Stop(void *timer)
{
	osiTimerStop((osiTimer_t *)timer);
	return 0;
}

int32_t HAL_Timer_Delete(void *timer)
{
    osiTimerDelete((osiTimer_t *)timer);
	return 0;
}

void *HAL_MessageQCreate(int32_t queue_length, int32_t item_size)
{
    return (void*)(osiMessageQueueCreate(queue_length, item_size));
}

int32_t HAL_MessageQPut(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    return (int32_t)(osiMessageQueueTryPut(mq, p_info, millisec));
}

int32_t HAL_MessageQGet(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    bool ret = osiMessageQueueTryGet(mq, p_info, millisec);
    int32_t ret2 = !ret;
    return (int32_t) ret2;
}

int32_t HAL_MessageQDelete(void *mq)
{
    osiMessageQueueDelete(mq);
	return 0;
}

int HAL_GetFirmwareVersion(char *version)
{
    return 0;
}

const char *HAL_Get_System_Version(void)
{
    return SYSINFO_VERSION;
}

const char *HAL_Get_system_platform_type(void)
{
    return SYSINFO_DEVICE_NAME;
}

int32_t HAL_Get_ip(char *ip)
{
    return 0;
}

int32_t HAL_Get_mac(unsigned char mac[8])
{
    memset(mac, 0, 8);
    char default_value = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    memcpy(mac, default_value, 8);
    return 0;
}

int32_t HAL_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    return 0;
}

int32_t HAL_Get_network_status(void)
{
    return 0;
}

int32_t HAL_Sysinit(void)
{
    return 0;
}

#if defined(CONFIG_FIBOCOM_ALIOS)
uint8_t g_fibo_aliyun_sim = 0;
void fibo_get_rssi_per(int* RssiValue, uint8_t* BitErrorValue)
{
#ifndef CONFIG_SOC_6760
    uint8_t nSim = g_fibo_aliyun_sim;
#endif
    uint8_t nSignalLevel = 99;
    uint8_t nBitError = 0;
	int     iRssi = -113;
#ifndef CONFIG_SOC_6760
    uint32_t ret = 0;
    uint8_t rat = 0;

	if (RssiValue == NULL || BitErrorValue == NULL)
	{
		OSI_PRINTFI("[%s]%d param error, Rssi:%p, BitError:%p", __func__, __LINE__, RssiValue, BitErrorValue);
		return;
	}

    CFW_COMM_MODE nFM = CFW_DISABLE_COMM;
    ret = CFW_GetComm(&nFM, nSim);
    if (nFM != CFW_DISABLE_COMM)
    {
        rat = CFW_NWGetStackRat(nSim);
        if (rat == 4)
        {
            CFW_NW_QUAL_INFO iQualReport;
            ret = CFW_NwGetLteSignalQuality(&nSignalLevel, &nBitError, nSim);
            if (ret != 0)
            {
            	OSI_PRINTFI("[%s]%d CFW_NwGetLteSignalQuality fail", __func__, __LINE__);
            }
			OSI_PRINTFI("[%s]%d CFW_NwGetLteSignalQuality ret:%d", __func__, __LINE__, ret);
            ret = CFW_NwGetQualReport(&iQualReport, nSim);
            if (ret != 0)
            {
            	OSI_PRINTFI("[%s]%d CFW_NwGetQualReport fail", __func__, __LINE__);
            }
			iRssi = iQualReport.nRssidBm;
			OSI_PRINTFI("[%s]%d CFW_NwGetQualReport ret:%d, iRssi:%d, nBitError:%d", __func__, __LINE__, ret, iRssi, nBitError);
			
        }
        else
        {
            ret = CFW_NwGetSignalQuality(&nSignalLevel, &nBitError, nSim);
            iRssi = 0 - nSignalLevel;
			OSI_PRINTFI("[%s]%d CFW_NwGetSignalQuality ret:%d, iRssi:%d, nBitError:%d", __func__, __LINE__, ret, iRssi, nBitError);
        }
    }
#else
    extern void nbiot_atGetUeRssiAndBer(uint8_t * rssi, uint8_t * ber);
    nbiot_atGetUeRssiAndBer(&nSignalLevel, &nBitError);
	iRssi = 0 - nSignalLevel;
	OSI_PRINTFI("[%s]%d CFW_NwGetSignalQuality ret:%d, iRssi:%d, nBitError:%d", __func__, __LINE__, ret, iRssi, nBitError);
#endif
	*RssiValue = iRssi;
	*BitErrorValue = nBitError;
	return;
}

int HAL_GetWirelessInfo(struct _amp_wireless_info_t *info)
{
	CFW_TUE_STATUS_INFO ueStatsRpt;
    uint8_t nSim = g_fibo_aliyun_sim;
	int iRssiValue = -113;
	uint8_t BitErrorValue = 0;
	OSI_PRINTFI("[%s]%d", __func__, __LINE__);

	if (info == NULL) 
	{
        return -1;
    }
	memset(&ueStatsRpt, 0, sizeof(CFW_TUE_STATUS_INFO));
    CFW_GetStatusInfo(&ueStatsRpt, nSim);
	OSI_PRINTFI("[%s]%d snr:%d", __func__, __LINE__, ueStatsRpt.sinr);
	
	fibo_get_rssi_per(&iRssiValue, &BitErrorValue);

    /* BLE needs to implement HAL API to get wirelesss information */
	info->rssi = iRssiValue;
	info->snr = ueStatsRpt.sinr;
	info->per = BitErrorValue;

	OSI_PRINTFI("[%s]%d rssi:%d, snr:%d, per:%d", __func__, __LINE__,iRssiValue, ueStatsRpt.sinr, BitErrorValue);

	return 0;
}
#endif
/* get imei */
bool HAL_Get_Module_IMEI(char* Imei)
{
	uint8_t nImei[32] = {0};
    int nImeiLen = 0;
    char *init = "000000000000000";

	if (Imei == NULL)
	{
		return false;
	}

    nImeiLen = nvmReadImei(0, (nvmImei_t *)&nImei[0]);
    if (nImeiLen == -1)
    {
        nImeiLen = 15;
        memcpy(nImei, init, nImeiLen);
    }
    OSI_PRINTFI("HAL_Get_Module_IMEI:%s", nImei);

	memcpy(Imei, nImei, nImeiLen);
    return true;
}
int32_t HAL_Heap_Memory_Info(amp_heap_info_t *heap_info)
{
	uint32_t ret_size1 = 0; 
    uint32_t ret_size2 = 0;
    uint32_t ret_size3 = 0;	
    uint32_t ret_size4=0;
    int ret = -1;
	
	if (heap_info == NULL)
	{
		return -1;
	}

    ret = fibo_get_heapinfo(&ret_size1,&ret_size2,&ret_size3);
    OSI_PRINTFI("the heapinfo is: total size:%ld",ret_size1);
    OSI_PRINTFI("the heapinfo is: avalid size:%ld",ret_size2);
    OSI_PRINTFI("the heapinfo is: block size:%ld",ret_size3);
    ret_size4 = ret_size1 - ret_size2;

	heap_info->heap_total = ret_size1;
	heap_info->heap_used = ret_size4;
	
	return 0;
}


static char g_dev_imei[20];
const char *HAL_Get_Device_Name(void)
{
    memset(g_dev_imei, 0, sizeof(g_dev_imei));
    if (!HAL_Get_Module_IMEI(g_dev_imei)) {
        OSI_PRINTFE("%s: get imei failed !", __func__);
        return NULL;
    }
    return g_dev_imei;
}

const char *HAL_Get_Module_Hardware_Version(void)
{
    return g_cgmm;
}

const char *HAL_Get_Module_Software_Version(void)
{
    return g_cgmr;
}

void fibo_alios_thread_delete(void)
{
	OSI_PRINTFI("[%s] enter! jsengine_handle:%p,network_handle:%p", __func__, g_fibo_alios_jsengine_thread_handle, g_fibo_alios_network_thread_handle);
	
	/* delete jsengine thread */
	if (g_fibo_alios_jsengine_thread_handle != NULL)
	{
        HAL_ThreadDelete(g_fibo_alios_jsengine_thread_handle);
		g_fibo_alios_jsengine_thread_handle = NULL;
	}
	
	/* delete network thread */
	if (g_fibo_alios_network_thread_handle != NULL)
	{
        HAL_ThreadDelete(g_fibo_alios_network_thread_handle);
		g_fibo_alios_network_thread_handle = NULL;
	}
}

osiThread_t *ali_amp_get_TaskID()
{
    return aliAmpThreadID;
}

static void ali_amp_thread(void *arg)
{
    for (;;)
    {
		HAL_Printf("[%s]line:%d", __func__, __LINE__);

		osiEvent_t event = {};
        osiEventWait(osiThreadCurrent(), &event);
	
		HAL_Printf("[%s]:%d", __func__, __LINE__);
        if (event.id == 0)
            continue;
        HAL_Printf("[%s]Netthread get a event: 0x%08x/0x%08x/0x%08x/0x%08x", __func__,(unsigned int)event.id, (unsigned int)event.param1, (unsigned int)event.param2, (unsigned int)event.param3);

        if ((!cfwIsCfwIndicate(event.id)) && (cfwInvokeUtiCallback(&event)))
        {
            ; // handled by UTI
        }
        else
        {

            CFW_EVENT *cfw_event = (CFW_EVENT *)&event;
            switch (event.id)
            {
            case EV_TCPIP_CFW_GPRS_ACT:
            {
                uint8_t nCid, nSimId;
                nCid = event.param1;
                nSimId = cfw_event->nFlag;
                TCPIP_netif_create(nCid, nSimId);
				HAL_Printf("[%s]:%d, EV_TCPIP_CFW_GPRS_ACT", __func__, __LINE__);
				amp_main();
                break;
            }
            case EV_TCPIP_CFW_GPRS_DEACT:
            {
                uint8_t nCid, nSimId;
                nCid = event.param1;
                nSimId = cfw_event->nFlag;
                TCPIP_netif_destory(nCid, nSimId);
				fibo_alios_thread_delete();
                break;
            }
			// case  FIBO_ALIOS_GET_NEIGHBOR_INFO:
    		// 	Fibo_Alios_GetCellInfo();
			// 	break;
            default:
                break;
            }
        }
    }
}

int ali_amp_init(void)
{
	// fibo_device_info_init();
	// fibo_aliyun_mqtt_dev_info_init();
	/* amp thread */
	aliAmpThreadID = osiThreadCreate("amp_network", ali_amp_thread, NULL, 32, 1024*8, 10);
	HAL_Printf("[%s]:%d aliAmpThreadID:%p\r\n", __func__, __LINE__, aliAmpThreadID);

	return 0;
}
static void FIBO_ALI_TCPIP_AsyncEventProcess(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    uint32_t ret = 0xFFFFFFFF;
    uint32_t param1 = pEvent->param1;
    HAL_Printf("[%s]pEvent->nEventId=%d, param1=%d,%d,%d", __func__,pEvent->id, param1, g_fibo_ali_callinfo.cid, g_fibo_ali_callinfo.uti);
    uint8_t actValue = param1;
    switch (pEvent->id)
    {
        case FIBO_ALI_TCPIP_PDP_ACT:
            HAL_Printf("FIBO_ALI_TCPIP_PDP_ACT actValue=%d!", actValue);
            ret = CFW_GprsAct(actValue, g_fibo_ali_callinfo.cid, g_fibo_ali_callinfo.uti, g_fibo_ali_callinfo.nSim);
            if (ERR_SUCCESS != ret)
            {
                HAL_Printf("FIBO_ALI_TCPIP_PDP_ACT FAIL!");
                cfwReleaseUTI(g_fibo_ali_callinfo.uti);
            }
            HAL_Printf("FIBO_ALI_TCPIP_PDP_ACT SUCCESS!");
            break;
        case FIBO_ALI_TCPIP_PDP_DEACT:
            HAL_Printf("FIBO_ALI_TCPIP_PDP_DEACT actValue=%d!", actValue);
            ret = CFW_GprsAct(actValue, g_fibo_ali_callinfo.cid, g_fibo_ali_callinfo.uti, g_fibo_ali_callinfo.nSim);
            if (ERR_SUCCESS != ret)
            {
                HAL_Printf("FIBO_ALI_TCPIP_PDP_DEACT FAIL!");
                cfwReleaseUTI(g_fibo_ali_callinfo.uti);
            }
            HAL_Printf("FIBO_ALI_TCPIP_PDP_DEACT SUCCESS!");
            break;
        default:
            HAL_Printf("Event is undefined");
            break;
    }
    free(pEvent);
    pEvent = NULL;
    RETURN_FOR_ASYNC();
}

static void FIBO_ALI_TCPIPPostEvent(uint32_t nEventId, uint32_t response)
{
    osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
    if (ev != NULL)
    {
        memset(ev, 0, sizeof(osiEvent_t));
        ev->id = nEventId;
        ev->param1 = response;

        osiThreadCallback(atEngineGetThreadId(), FIBO_ALI_TCPIP_AsyncEventProcess, (void *)ev);
    }
    else
    {
        HAL_Printf("malloc ev fail!");
    }
    return;
}

static void FIBO_ALI_GprsActRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    uint8_t nCid, nSimId;
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    HAL_Printf("event->id=%u, cfw_event->nType=%u, event->param1=%u, cfw_event->nFlag=%u", event->id, cfw_event->nType, event->param1, cfw_event->nFlag);

    if (event->id == EV_CFW_GPRS_ACT_RSP)
    {
        if (cfw_event->nType == CFW_GPRS_ACTIVED)
        {
            HAL_Printf("[%s] actived!", __func__);
            osiEvent_t network_event = *event;
            network_event.id = EV_TCPIP_CFW_GPRS_ACT;
            osiEventSend(ali_amp_get_TaskID(), (const osiEvent_t *)&network_event);
			//fibo_alios_bootAutoAct_Ip_Syn(); //Synchronize IP
        }
        else if (cfw_event->nType == CFW_GPRS_DEACTIVED)
        {
            HAL_Printf("deactived!");
            nCid = event->param1;
            nSimId = cfw_event->nFlag;
            pppSessionDeleteByNetifDestoryed(nSimId, nCid);
            osiEvent_t network_event = *event;
            network_event.id = EV_TCPIP_CFW_GPRS_DEACT;
            osiEventSend(ali_amp_get_TaskID(), (const osiEvent_t *)&network_event);
        }
    }
    
    return;
}

void FIBO_ALI_BootAutoACT(CFW_SIM_ID sim)
{
    uint8_t attState = 0;
    uint8_t actState = 0;
    uint32_t ret;
    CFW_SIM_ID nSim = sim;
    AT_Gprs_CidInfo *g_staAtGprsCidInfo_nv = gAtSetting.g_staAtGprsCidInfo[nSim];
    CFW_GPRS_PDPCONT_INFO_V2 pdp_cont = {0};
	uint8_t cid = 1;
    HAL_Printf("nSim = %d", nSim);

    ret = CFW_GetGprsAttState(&attState, nSim);
    if ((ERR_SUCCESS != ret) || (attState == CFW_GPRS_DETACHED))
    {
        HAL_Printf("[%s] attSate:%d", __func__, attState);
        return;
    }
	
    
	//act default cid 1
   	cid = 1;
    ret = CFW_GetGprsActState(cid, &actState, nSim);
    HAL_Printf("[%s]:%d cid=%d, actState=%u,ret=%d", __func__, __LINE__, cid, actState, ret);
    if ((ERR_SUCCESS == ret) && (CFW_GPRS_ACTIVED == actState))
    {
        HAL_Printf("[%s]the cid is actived!", __func__);
        return;
    }
    else
    {
        HAL_Printf("pdptype=%u,apn=%s,userName=%s,userPassword=%s,authPort=%u", g_staAtGprsCidInfo_nv[cid].nPdpType, g_staAtGprsCidInfo_nv[cid].pApn, g_staAtGprsCidInfo_nv[cid].uaUsername, g_staAtGprsCidInfo_nv[cid].uaPassword, g_staAtGprsCidInfo_nv[cid].nAuthProt);
        if ((ret = CFW_GprsGetPdpCxtV2(cid, &pdp_cont, nSim)) != 0)
        {
            HAL_Printf("CFW_GprsGetPdpCxtV2 return = %lu", ret);
            memset(&pdp_cont, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
        }
		HAL_Printf("[%s]:%d ", __func__, __LINE__);
        pdp_cont.nPdpType = g_staAtGprsCidInfo_nv[cid].nPdpType;
        if (0 != strlen((const char*)g_staAtGprsCidInfo_nv[cid].pApn))
        {
            pdp_cont.nApnSize = strlen((const char*)g_staAtGprsCidInfo_nv[cid].pApn);
            memset(pdp_cont.pApn, 0x00, THE_APN_MAX_LEN);
            memcpy(pdp_cont.pApn, g_staAtGprsCidInfo_nv[cid].pApn, pdp_cont.nApnSize);
        }
        if (0 != strlen((const char*)g_staAtGprsCidInfo_nv[cid].uaUsername))
        {
            pdp_cont.nApnUserSize = strlen((const char*)g_staAtGprsCidInfo_nv[cid].uaUsername);
            memset(pdp_cont.pApnUser, 0x00, THE_APN_USER_MAX_LEN);
            memcpy(pdp_cont.pApnUser, g_staAtGprsCidInfo_nv[cid].uaUsername, pdp_cont.nApnUserSize);
        }
        if (0 != strlen((const char*)g_staAtGprsCidInfo_nv[cid].uaPassword))
        {
            pdp_cont.nApnPwdSize = strlen((const char*)g_staAtGprsCidInfo_nv[cid].uaPassword);
            memset(pdp_cont.pApnPwd, 0x00, THE_APN_PWD_MAX_LEN);
            memcpy(pdp_cont.pApnPwd, g_staAtGprsCidInfo_nv[cid].uaPassword, pdp_cont.nApnPwdSize);
        }
        pdp_cont.nAuthProt = g_staAtGprsCidInfo_nv[cid].nAuthProt;
		HAL_Printf("[%s]:%d", __func__, __LINE__);
        if (CFW_GprsSetPdpCxtV2(cid, &pdp_cont, nSim) != 0)
        {
            HAL_Printf("[%s]CFW_GprsSetPdpCxtV2 FAIL!", __func__);
            return;
        }
		HAL_Printf("[%s]:%d", __func__, __LINE__);
        g_fibo_ali_callinfo.cid = cid;
        g_fibo_ali_callinfo.nSim = nSim;
        g_fibo_ali_callinfo.uti = cfwRequestUTI((osiEventCallback_t)FIBO_ALI_GprsActRsp, NULL);
		HAL_Printf("[%s]:%d uti:%d", __func__, __LINE__, g_fibo_ali_callinfo.uti);
		uint32_t pdpActEvent = 1;
        uint32_t pdpActValue = 1;
        FIBO_ALI_TCPIPPostEvent(pdpActEvent, pdpActValue);
        return;
    }
}

void atCmdHandleCLOUDCONN(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDDEVSECRET(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDDISCONN(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDDEVINFO(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDSUB(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDAUTH(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDPUB(atCommand_t *cmd)
{

}

void atCmdHandleCLOUDUNSUB(atCommand_t *cmd)
{
    
}
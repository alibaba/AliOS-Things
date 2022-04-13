/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_network.h"
#include "auto_apn.h"
#include "cfw.h"
#include "nvm.h"
#include "osi_log.h"
#include "at_command.h"
#include "osi_api.h"
#include "cfw_event.h"
#include "osi_event_hub.h"
#include "amp_defines.h"
#include "duk_config.h"


#define ERR_SUCCESS 0

typedef struct _fibo_alios_cellinfo_element_st
{
	uint32_t lac;
	uint32_t cellId;
	char MCC[4];
	char MNC[4];
}fibo_alios_cellinfo_element;

typedef struct _fibo_alios_cellinfo_st
{
	uint8_t NeighborCellNum;
	fibo_alios_cellinfo_element curr_cellinfo;
	fibo_alios_cellinfo_element NeighborCellInfo[7];
}fibo_alios_cellinfo;

uint16_t g_fibo_alios_get_cellinfo_uti = 0;
int g_neighbor_bs_info_js_cb_ref = 0;
fibo_alios_cellinfo g_fibo_alios_cellinfo = {0};


int32_t HAL_Get_SimInfo(sim_info_t *sim_info)
{
    int32_t ret = -1;
	/* imei */
	uint8_t nSim = 0;
    uint8_t nImei[32] = {0};
	int nImeiLen = 0;
	char *init = "000000000000000";
	/* imsi */
	uint8_t IMSI[24]={0};
    uint8_t maxlen = 16;
	uint8_t simId = 0;
    uint8_t nPreIMSI[16] = {0};
	uint32_t nRet = 0;
	/* iccid */
	uint8_t ICCID[ICCID_LEN] = {0};
	CFW_SIM_ID nSimID = CFW_SIM_0;

	if (sim_info == NULL)
	{
		HAL_Printf("[%s]%d parameter NULL !", __func__, __LINE__);
		return -1;
	}
	memset(sim_info, 0x0, sizeof(sim_info_t));
	/* imei */
	nImeiLen = nvmReadImei(nSim, (nvmImei_t *)&nImei[0]);
    if (nImeiLen == -1)
    {
        nImeiLen = 15;
        memcpy(nImei, init, nImeiLen);
    }
	memcpy(sim_info->imei, nImei, nImeiLen);
	HAL_Printf("[%s]%d imei:%s", __func__, __LINE__, sim_info->imei);

	/* imsi */
	memset(IMSI, 0,sizeof(IMSI));
	memset(nPreIMSI, 0, sizeof(nPreIMSI));
    nRet = CFW_CfgGetIMSI(nPreIMSI, simId);
    if (ERR_SUCCESS != nRet){
		HAL_Printf("[%s]%d get imsi fail!", __func__, __LINE__);
		return -1;
    }
	cfwIMSItoASC(nPreIMSI, IMSI, &maxlen);
    memcpy(sim_info->imsi, IMSI, maxlen);
	HAL_Printf("[%s]%d imsi:%s, maxlen:%d", __func__, __LINE__, sim_info->imsi, maxlen);

	/* iccid */
    uint8_t *pICCID = CFW_GetICCID(nSimID);
    memset(ICCID, 0, sizeof(ICCID));
	if (NULL != pICCID)
    {
        sulgsmbcd2asciiEx(pICCID, 10, ICCID);
    }
	memcpy(sim_info->iccid, ICCID, ICCID_LEN);
	HAL_Printf("[%s]%d iccid:%s", __func__, __LINE__, sim_info->iccid);
	
    return 0;
}

int32_t HAL_Get_LocatorInfo(locator_info_t *locator_info)
{
    int32_t ret = -1;
	uint32_t lac = 0;
	uint32_t cellId = 0;
	uint32_t tac = 0;
	uint32_t lac_push = 0;
	char MCC[4] = {0};
	char MNC[4] = {0};
	CFW_NW_STATUS_INFO nStatusInfo;
	CFW_NW_STATUS_INFO sStatus;
    uint8_t status;
	CFW_SIM_ID simId = CFW_SIM_0;
	uint32_t uErrCode = 0;
	uint8_t oper_id[6] = {0};
    uint8_t mode;
	uint8_t ucLoop = 0;
	uint16_t uti;
	uint8_t MNC_len = 1;
    uint8_t nCurrRat = CFW_NWGetStackRat(simId);

	if (locator_info == NULL)
	{
		HAL_Printf("[%s]%d parameter null!", __func__, __LINE__);
		return -1;
	}

	memset(&nStatusInfo, 0x0, sizeof(CFW_NW_STATUS_INFO));
	memset(&sStatus, 0x0, sizeof(CFW_NW_STATUS_INFO));
	HAL_Printf("[%s]%d currRat:%d", __func__, __LINE__, nCurrRat);
	/* cellId & lac/tac */
	if (nCurrRat == 4)  //4G mode
	{
		uErrCode = CFW_GprsGetstatus(&sStatus, simId);
		if (uErrCode != 0)
		{	
			HAL_Printf("[%s]%d gprs get status failed", __func__, __LINE__);
		}
		
		tac = sStatus.nAreaCode[3] << 8 | sStatus.nAreaCode[4];
		cellId = sStatus.nCellId[0] << 24 | sStatus.nCellId[1] << 16 
			| sStatus.nCellId[2] << 8 | sStatus.nCellId[3];
		
		HAL_Printf("[%s]%d tac:%d, cellId:%d", __func__, __LINE__, tac, cellId);
	}
	else //2G mode
	{
		if (CFW_CfgGetNwStatus(&status, simId) != 0 ||
        	CFW_NwGetStatus(&nStatusInfo, simId) != 0)
	    {
	    	HAL_Printf("[%s]%d gprs get status failed", __func__, __LINE__);
	    }
		lac = nStatusInfo.nAreaCode[3] << 8 | nStatusInfo.nAreaCode[4];
		cellId = nStatusInfo.nCellId[0] << 8 | nStatusInfo.nCellId[1];
		HAL_Printf("[%s]%d lac:%04x, cellId:%08x", __func__, __LINE__, lac, cellId);
	}

	/* MCC/MNC */
    if (CFW_NwGetCurrentOperator(oper_id, &mode, simId) != 0)
    {
    	HAL_Printf("[%s]%d MCC/MNC get failed", __func__, __LINE__);
    }
	
	/* MCC */
	memset(MCC, 0x0, sizeof(MCC));
	for (ucLoop = 0; ucLoop < 3; ucLoop++)
	{
		MCC[ucLoop] = '0' + oper_id[ucLoop];
	}
	
	/* MNC */
	memset(MNC, 0x0, sizeof(MNC));
	for (ucLoop = 0; ucLoop < 3; ucLoop++)
	{
		if (oper_id[ucLoop + 3] > 9)
		{
			break;
		}
		MNC[ucLoop] = '0' + oper_id[3 + ucLoop];
		MNC_len++;
	}
	HAL_Printf("[%s]%d MCC:%s, MNC:%s", __func__, __LINE__, MCC, MNC);
	lac_push = (nCurrRat == 4) ? tac: lac;
	
	locator_info->cid = cellId;
	locator_info->lac = lac_push;
	memcpy(locator_info->mcc, MCC, 4);
	memcpy(locator_info->mnc, MNC, 4);

	HAL_Printf("[%s]%d cellId:%08x, lac:%04x, MCC:%s, MNC:%s", __func__, __LINE__, locator_info->cid,
		locator_info->lac, locator_info->mcc, locator_info->mnc);
    return 0;
}

int32_t HAL_Get_SignalInfo(signal_info_t *signal_info)
{
    int32_t ret = -1;

	int RssiValue = 0;
	int BitErrorValue = 0;
	
	fibo_get_rssi_per(&RssiValue, &BitErrorValue);
	signal_info->rssi = RssiValue;

    return 0;
}


int32_t HAL_Get_NeighborInfo(void)
{
	HAL_Printf("[%s] enter!", __func__);
	osiEvent_t network_event;
    // network_event.id = FIBO_ALIOS_GET_NEIGHBOR_INFO;
    osiEventSend(ali_amp_get_TaskID(), (const osiEvent_t *)&network_event);
	HAL_Printf("[%s] %d", __func__, __LINE__);
	return 0;
}

void Fibo_Alios_NeighborCellInfo_notify(void)
{
    int i                      = 0;
	uint8_t neighborcell_num   = 0;
    duk_context *ctx           = be_get_context();
	
	HAL_Printf("[%s] enter!", __func__);
	neighborcell_num = g_fibo_alios_cellinfo.NeighborCellNum;
	be_push_ref(ctx, g_neighbor_bs_info_js_cb_ref);
    duk_push_int(ctx, neighborcell_num);
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < neighborcell_num; i++) {
        duk_push_object(ctx);
		duk_push_number(ctx, g_fibo_alios_cellinfo.NeighborCellInfo[i].cellId);
		duk_put_prop_string(ctx, -2, "cellId");	
		duk_push_number(ctx, g_fibo_alios_cellinfo.NeighborCellInfo[i].lac);
		duk_put_prop_string(ctx, -2, "lac");
		duk_push_string(ctx, g_fibo_alios_cellinfo.NeighborCellInfo[i].MCC);
		duk_put_prop_string(ctx, -2, "MCC");
		duk_push_string(ctx, g_fibo_alios_cellinfo.NeighborCellInfo[i].MNC);
		duk_put_prop_string(ctx, -2, "MNC");	
        duk_put_prop_index(ctx, arr_idx, i);
    }
    duk_pcall(ctx, 2);
    duk_pop(ctx);
    be_unref(ctx, g_neighbor_bs_info_js_cb_ref);
}

static bool Fibo_Alios_GetCellInfo_Lte_ProcessData(CFW_NET_INFO_T *pNetinfo, uint8_t nRequestedDump)
{
	fibo_alios_cellinfo_element* curr_cellinfo_ptr = NULL;
	fibo_alios_cellinfo_element* Neighbor_CellInfo_Ptr = NULL;
	HAL_Printf("[%s] requesterDump:%d", __func__, nRequestedDump);
	memset(&g_fibo_alios_cellinfo, 0x0, sizeof(fibo_alios_cellinfo));
    if (nRequestedDump == 1)
    {
		HAL_Printf("[%s] curr cellinfo", __func__);
		curr_cellinfo_ptr = &g_fibo_alios_cellinfo.curr_cellinfo;
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[0] & 0x0F) != 0x0F)
        {
            curr_cellinfo_ptr->MCC[0] = (pNetinfo->nwCapabilityLte.lteScell.plmn[0] & 0x0F) + '0';
        }
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[0] >> 4) != 0x0F)
        {
            curr_cellinfo_ptr->MCC[1] = (pNetinfo->nwCapabilityLte.lteScell.plmn[0] >> 4) + '0';
        }
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[1] & 0x0F) != 0x0F)
        {
            curr_cellinfo_ptr->MCC[2] = (pNetinfo->nwCapabilityLte.lteScell.plmn[1] & 0x0F) + '0';
        }
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[2] & 0x0F) != 0x0F)
        {
            curr_cellinfo_ptr->MNC[0] = (pNetinfo->nwCapabilityLte.lteScell.plmn[2] & 0x0F) + '0';
        }
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[2] >> 4) != 0x0F)
        {
            curr_cellinfo_ptr->MNC[1] = (pNetinfo->nwCapabilityLte.lteScell.plmn[2] >> 4 & 0x0F) + '0';
        }
        if ((pNetinfo->nwCapabilityLte.lteScell.plmn[1] >> 4) != 0x0F)
        {
            curr_cellinfo_ptr->MNC[2] = (pNetinfo->nwCapabilityLte.lteScell.plmn[1] >> 4 & 0x0F) + '0';
        }
		curr_cellinfo_ptr->lac = pNetinfo->nwCapabilityLte.lteScell.tac;
		curr_cellinfo_ptr->cellId = (pNetinfo->nwCapabilityLte.lteScell.enodeBID << 8) | (pNetinfo->nwCapabilityLte.lteScell.cellId);
        HAL_Printf("[%s] curr cellinfo,MCC:%s, MNC:%s,lac:%u,cellid:%u", curr_cellinfo_ptr->MCC,curr_cellinfo_ptr->MNC, 
			curr_cellinfo_ptr->lac, curr_cellinfo_ptr->cellId);
    }
    else
	{
		HAL_Printf("[%s] neighbour cell! num:%d", __func__, pNetinfo->nwCapabilityLte.lteNcellNum);
		int j = 0;
	    if (pNetinfo->nwCapabilityLte.lteNcellNum > 7)
	    {
	        pNetinfo->nwCapabilityLte.lteNcellNum = 7;
	    }
		g_fibo_alios_cellinfo.NeighborCellNum = pNetinfo->nwCapabilityLte.lteNcellNum;
	    for (j = 0; j < pNetinfo->nwCapabilityLte.lteNcellNum; j++)
	    {
			Neighbor_CellInfo_Ptr = &(g_fibo_alios_cellinfo.NeighborCellInfo[j]);
			/* MCC */
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[0] & 0x0F) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MCC[0] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[0] & 0x0F) + '0';
	        }
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[0] >> 4) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MCC[1] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[0] >> 4) + '0';
	        }
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[1] & 0x0F) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MCC[2] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[1] & 0x0F) + '0';
	        }
			/* MNC */
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[2] & 0x0F) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MNC[0] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[2] & 0x0F) + '0';
	        }
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[2] >> 4) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MNC[1] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[2] >> 4 & 0x0F) + '0';
	        }
	        if ((pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[1] >> 4) != 0x0F)
	        {
	            Neighbor_CellInfo_Ptr->MNC[2] = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.plmn[1] >> 4 & 0x0F) + '0';
	        }
			Neighbor_CellInfo_Ptr->lac = pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.tac;
			Neighbor_CellInfo_Ptr->cellId = (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.enodeBID << 8) | (pNetinfo->nwCapabilityLte.lteNcell[j].cellAccRelInfo.cellId);

			HAL_Printf("[%s] neighbor_cellinfo[%d], MCC:%s, MNC:%s, lac:%u, cellid:%u", __func__, j, Neighbor_CellInfo_Ptr->MCC, 
				Neighbor_CellInfo_Ptr->MNC, Neighbor_CellInfo_Ptr->lac, Neighbor_CellInfo_Ptr->cellId);
		}
		if (g_fibo_alios_cellinfo.NeighborCellNum > 0)
		{
			amp_task_schedule_call(Fibo_Alios_NeighborCellInfo_notify, NULL);
		}		
    }
}

static bool Fibo_Alios_GetCellInfo_GSM_ProcessData(CFW_TSM_CURR_CELL_INFO *pCurrCellInfo,
                              CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo)
{
	fibo_alios_cellinfo_element* Curr_CellInfo_Ptr = NULL;
	fibo_alios_cellinfo_element* Neighbor_CellInfo_Ptr = NULL;
	memset(&g_fibo_alios_cellinfo, 0x0, sizeof(fibo_alios_cellinfo));
	if ((pCurrCellInfo == NULL) && (pNeighborCellInfo == NULL))
	{
		HAL_Printf("[%s] parameter err!", __func__);
		return false;
	}

	if (pCurrCellInfo != NULL)
	{
		HAL_Printf("[%s] current cellinfo", __func__);
		Curr_CellInfo_Ptr = &g_fibo_alios_cellinfo.curr_cellinfo;
		Curr_CellInfo_Ptr->MCC[0] = pCurrCellInfo->nTSM_LAI[0] & 0x0F + '0';
        Curr_CellInfo_Ptr->MCC[1] = pCurrCellInfo->nTSM_LAI[0] >> 4 + '0';
        Curr_CellInfo_Ptr->MCC[2] = pCurrCellInfo->nTSM_LAI[1] & 0x0F + '0';

        Curr_CellInfo_Ptr->MNC[0] = pCurrCellInfo->nTSM_LAI[2] & 0x0F + '0';
        Curr_CellInfo_Ptr->MNC[1] = pCurrCellInfo->nTSM_LAI[2] >> 4 + '0';
		if ((pCurrCellInfo->nTSM_LAI[1] >> 4) <= 9)
		{
			Curr_CellInfo_Ptr->MNC[2] = pCurrCellInfo->nTSM_LAI[1] >> 4 + '0';
		}      

        Curr_CellInfo_Ptr->lac = (pCurrCellInfo->nTSM_LAI[3] << 8) | (pCurrCellInfo->nTSM_LAI[4]);
        Curr_CellInfo_Ptr->cellId = (pCurrCellInfo->nTSM_CellID[0] << 8) | (pCurrCellInfo->nTSM_CellID[1]);
		
		HAL_Printf("[%s] current cellinfo, MCC:%s, MNC:%s, lac:%d, cellid:%d", __func__,
			Curr_CellInfo_Ptr->MCC, Curr_CellInfo_Ptr->MNC, Curr_CellInfo_Ptr->lac, Curr_CellInfo_Ptr->cellId);
	}
	
	if (pNeighborCellInfo != NULL)
    {
        HAL_Printf("[%s] Neighbor cellinfo, cellNum:%d", __func__, pNeighborCellInfo->nTSM_NebCellNUM);
		g_fibo_alios_cellinfo.NeighborCellNum = pNeighborCellInfo->nTSM_NebCellNUM;
        for (uint8_t i = 0; i < pNeighborCellInfo->nTSM_NebCellNUM; i++)
        {
			Neighbor_CellInfo_Ptr = &(g_fibo_alios_cellinfo.NeighborCellInfo[i]);
			Neighbor_CellInfo_Ptr->MCC[0] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] & 0x0F) + '0';
            Neighbor_CellInfo_Ptr->MCC[1] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] >> 4) + '0';
            Neighbor_CellInfo_Ptr->MCC[2] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] & 0x0F) + '0';
			
            Neighbor_CellInfo_Ptr->MNC[0] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] & 0x0F) + '0';
            Neighbor_CellInfo_Ptr->MNC[1] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] >> 4) + '0';
			if ((pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] >> 4) <= 9)
			{
				Neighbor_CellInfo_Ptr->MNC[2] = (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] >> 4) + '0';
			}

            Neighbor_CellInfo_Ptr->lac =
                (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[3] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[4]);
			
            Neighbor_CellInfo_Ptr->cellId =
                (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[0] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[1]);
			
			HAL_Printf("[%s] Neighbor cellinfo[%d], MCC:%s, MNC:%s, lac:%d, cellid:%d", __func__,
			i, Neighbor_CellInfo_Ptr->MCC, Neighbor_CellInfo_Ptr->MNC, Neighbor_CellInfo_Ptr->lac, Neighbor_CellInfo_Ptr->cellId);
        }
		if (g_fibo_alios_cellinfo.NeighborCellNum > 0)
		{
			amp_task_schedule_call(Fibo_Alios_NeighborCellInfo_notify, NULL);
		}
	}
	return true;
}

static void Fibo_Alios_GetCellInfo_RspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    HAL_Printf("[%s]%d enter!!!", __func__, __LINE__);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    CFW_SIM_ID nSim = (CFW_SIM_ID)cfw_event->nFlag;
    uint32_t nRet = 0;
    bool bRet = false;
    CFW_TSM_CURR_CELL_INFO tCurrCellInf;
    CFW_TSM_ALL_NEBCELL_INFO tNeighborCellInfo;
    CFW_NET_INFO_T tNetLTEinfo;

    memset(&tCurrCellInf, 0, sizeof(CFW_TSM_CURR_CELL_INFO));
    memset(&tNeighborCellInfo, 0, sizeof(CFW_TSM_ALL_NEBCELL_INFO));
    memset(&tNetLTEinfo, 0, sizeof(CFW_NET_INFO_T));

    if (cfw_event->nEventId == EV_CFW_TSM_INFO_IND)
    {
        if (cfw_event->nType != 0)
        {
            HAL_Printf("[%s] %d cfw_event type error", __func__, __LINE__);
			return;
        }
        if (cfw_event->nParam2 == CFW_TSM_CURRENT_CELL)
        {
            nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);
            if (nRet != ERR_SUCCESS)
            {
                HAL_Printf("[%s] get currcellinfo fail:0x%x!\n", nRet);
            }
            else
            {
				HAL_Printf("[%s] GSM get currcellinfo", __func__);
				Fibo_Alios_GetCellInfo_GSM_ProcessData(&tCurrCellInf, NULL);
            }
            g_fibo_alios_get_cellinfo_uti = cfwRequestUTI((osiEventCallback_t)Fibo_Alios_GetCellInfo_RspCB, NULL);
            CFW_EmodOutfieldTestEnd(g_fibo_alios_get_cellinfo_uti, nSim);
        }
        else if (cfw_event->nParam2 == CFW_TSM_NEIGHBOR_CELL)
        {
            nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);
            if (nRet != ERR_SUCCESS)
            {
                HAL_Printf("NW_AsyncEvent NEIGHBOR_CELL ERROR:0x!%x\n", nRet);
            }
            else
            {
                HAL_Printf("NW_AsyncEvent nTSM_NebCellNUM :%d!\n", tNeighborCellInfo.nTSM_NebCellNUM);
                bRet = Fibo_Alios_GetCellInfo_GSM_ProcessData(NULL, &tNeighborCellInfo);
            }
            g_fibo_alios_get_cellinfo_uti = cfwRequestUTI((osiEventCallback_t)Fibo_Alios_GetCellInfo_RspCB, NULL);
            CFW_EmodOutfieldTestEnd(g_fibo_alios_get_cellinfo_uti, nSim);
        }
        else if (cfw_event->nParam2 == CFW_TSM_PLOT_LTE)
        {
            nRet = CFW_GetLETRealTimeInfo(&tNetLTEinfo, nSim);
            if (nRet != ERR_SUCCESS)
            {
                HAL_Printf("NW_AsyncEvent CFW_GetNetLETInfo ERROR:0x%x !\n", nRet);           
            }
            else
            {
                Fibo_Alios_GetCellInfo_Lte_ProcessData(&tNetLTEinfo, 8);//get neighbor cellinfo
            }
            g_fibo_alios_get_cellinfo_uti = cfwRequestUTI((osiEventCallback_t)Fibo_Alios_GetCellInfo_RspCB, NULL);
            CFW_EmodOutfieldTestEnd(g_fibo_alios_get_cellinfo_uti, nSim);
        }
    }
    else if (cfw_event->nEventId == EV_CFW_TSM_INFO_END_RSP)
    {
        if (cfw_event->nType != 0)
        {
            HAL_Printf("[%s] nType err,%d",__func__, cfw_event->nType);
        }
        else
        {
            HAL_Printf("[%s] EV_CFW_TSM_INFO_END_RSP",__func__);
        }
		cfwReleaseUTI(g_fibo_alios_get_cellinfo_uti);
		g_fibo_alios_get_cellinfo_uti = 0;
    }
}

void Fibo_Alios_GetCellInfo(void)
{
	uint8_t nRequestedDump = 2;
	CFW_SIM_ID nSim = CFW_SIM_0;
	int32_t iRet = 0;
	uint8_t rat = CFW_NWGetStackRat(nSim);
	CFW_TSM_FUNCTION_SELECT tSelecFUN;
	HAL_Printf("[%s] enter!", __func__);
    memset(&tSelecFUN, 0, sizeof(CFW_TSM_FUNCTION_SELECT));
    if (1 == nRequestedDump)
    {
        tSelecFUN.nServingCell = 1;
    }
    else
    {
        tSelecFUN.nNeighborCell = 1;
    }
    tSelecFUN.nCurrentNetMode = rat;
    g_fibo_alios_get_cellinfo_uti = cfwRequestUTI((osiEventCallback_t)Fibo_Alios_GetCellInfo_RspCB, NULL);

    if (ERR_SUCCESS != (iRet = CFW_EmodOutfieldTestStart(&tSelecFUN, g_fibo_alios_get_cellinfo_uti, nSim)))
    {
        HAL_Printf("[%s]%d iRet:%d", __func__, __LINE__, iRet);
        cfwReleaseUTI(g_fibo_alios_get_cellinfo_uti);
		g_fibo_alios_get_cellinfo_uti = 0;
    }
}

int32_t HAL_Get_neighborLocatorInfo(void (*cb)(locator_info_t*, int))
{
    cb(NULL, 0);
    return 0;
}
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "stm32f4xx.h"
#ifdef HAL_CAN_MODULE_ENABLED
#include "hal_can_stm32f4.h"

/* REMARK: This header file k_api.h specify the Rhino OS, include this will
            make this module is not easily to be port, but I have to do
            this as currently there is no cross-platform *api.h and STMF1
            currently only run on Rhino*/
#include "k_api.h"

typedef struct{
    uint32_t Baudrate;
    uint32_t Prescaler;
    uint32_t SJW;
    uint32_t BS1;
    uint32_t BS2;
}can_parameter_mapping;

/* private parameter of ST, only list common baud. User may modify this table
 * if he wouldn't want any of below parameter
 */
const can_parameter_mapping can_para_baud[] =
{
   { can_baud_1M,      7, CAN_SJW_1TQ, CAN_BS1_2TQ, CAN_BS2_3TQ },
   { can_baud_500K,   14, CAN_SJW_1TQ, CAN_BS1_2TQ, CAN_BS2_3TQ },
   { can_baud_250K,   28, CAN_SJW_1TQ, CAN_BS1_2TQ, CAN_BS2_3TQ },
   { can_baud_125K,   56, CAN_SJW_1TQ, CAN_BS1_2TQ, CAN_BS2_3TQ }
};

typedef struct {
    uint8_t                inited;
    uint8_t                filter_top;
    uint8_t                start_filter;
    CAN_TxHeaderTypeDef    tx_header_def;
    CAN_RxHeaderTypeDef    rx_header_def;
    CAN_HandleTypeDef      hal_can_handle;
}stm32_can_t;

/* will be initialized in hal_can_init */
static stm32_can_t stm32_can[PORT_CAN_SIZE];
static int32_t set_can_filter(const uint8_t can_port, const uint32_t check_id, const uint32_t mask, const uint8_t rtr, const uint8_t filter_num);

/* Get CAN Instanse & attribute from Logical Port */
CAN_MAPPING* get_can_list_logical(const PORT_CAN_TYPE port)
{
    int8_t i = 0;
    CAN_MAPPING* ret = NULL;
    for(i=0; i<PORT_CAN_SIZE; i++)
    {
        if(CAN_MAPPING_TABLE[i].logical_func == port)
        {
            ret = &CAN_MAPPING_TABLE[i];
            break;
        }
    }
    return ret;
}

/* Get CAN Instanse & attribute from instanse */
static CAN_MAPPING* get_can_list_instanse(const CAN_HandleTypeDef * def)
{
    int8_t i = 0;
    CAN_MAPPING* ret = NULL;
    for(i=0; i<PORT_CAN_SIZE; i++)
    {
        if((CAN_HandleTypeDef*)CAN_MAPPING_TABLE[i].physical_port == def)
        {
            ret = &CAN_MAPPING_TABLE[i];
            break;
        }
    }
    return ret;
}

/**
* @brief This function handles USB low priority or CAN RX0 interrupts.
*/
#ifdef CAN1
void CAN1_RX0_IRQHandler(void)
{
    krhino_intrpt_enter();
    if(stm32_can[1].inited){
        HAL_CAN_IRQHandler(&stm32_can[1].hal_can_handle);
    }
    krhino_intrpt_exit();
}
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
    CAN_RxHeaderTypeDef * rx_header;
    CAN_MAPPING* list = get_can_list_instanse(CanHandle->Instance);
    if(list!=NULL && list->isr_rx_handler!=NULL){
        list->isr_rx_handler(rx_header, list->isr_rx_para);
    }

    if (HAL_CAN_ActivateNotification(CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
      /* Reception Error, which will result CAN ISR not work, shall consider restart the CAN bus,
      other than just error_handler */
    }
}

int32_t hal_can_init(can_dev_t *can)
{
    int32_t ret = -1;
    if (can != NULL) {
        /* no found this port in function-physical canIns, no need initialization */
        CAN_MAPPING* canIns = get_can_list_logical(can->port);
        if( NULL != canIns ){
            memset(&stm32_can[can->port],0,sizeof(stm32_can_t));
            CAN_TxHeaderTypeDef * tx_header;
            CAN_RxHeaderTypeDef * rx_header;
            CAN_HandleTypeDef * const pstcanhandle = &stm32_can[can->port].hal_can_handle;
            pstcanhandle->Instance = canIns->physical_port;
            const uint8_t para_table_size = sizeof(can_para_baud)/sizeof(can_para_baud[0]);
            uint8_t i = 0;
            can_parameter_mapping* para = NULL;
            for( ; i<para_table_size; i++ )
            {
                if(can_para_baud[i].Baudrate==can->config.baud_rate){
                    para = &can_para_baud[i];
                    break;
                }
            }
            if(para!=NULL){
                tx_header = &stm32_can[can->port].tx_header_def;
                rx_header = &stm32_can[can->port].rx_header_def;
                pstcanhandle->Init.Prescaler = para->Prescaler;
                pstcanhandle->Init.SyncJumpWidth = para->SJW;
                pstcanhandle->Init.TimeSeg1 = para->BS1;
                pstcanhandle->Init.TimeSeg2 = para->BS2;
                /* maybe we never modify these parameter, so we locate it here,
                 * If we have to modify these parameter, then the can_attr_t shall be updated
                 */
                pstcanhandle->Init.Mode = CAN_MODE_NORMAL;
                pstcanhandle->Init.TimeTriggeredMode    = DISABLE;
                pstcanhandle->Init.AutoBusOff           = can->config.auto_bus_off?ENABLE:DISABLE;
                pstcanhandle->Init.AutoWakeUp           = DISABLE;
                pstcanhandle->Init.AutoRetransmission   = can->config.auto_retry_transmit?ENABLE:DISABLE;
                pstcanhandle->Init.ReceiveFifoLocked    = DISABLE;
                pstcanhandle->Init.TransmitFifoPriority = DISABLE;
                ret = HAL_CAN_Init(pstcanhandle);
                if ( ret == HAL_OK){
                    /* Configure the CAN Filter */
                    tx_header->IDE = can->config.ide?CAN_ID_EXT:CAN_ID_STD;
                    stm32_can[can->port].start_filter = STM32F4_BANK_NUM_HALF*can->port;
                    stm32_can[can->port].filter_top = stm32_can[can->port].start_filter;

                    if(0==set_can_filter(can->port,0,0,0,stm32_can[can->port].filter_top)){
                        stm32_can[can->port].filter_top++;
                    }

                    if(0==set_can_filter(can->port,0,0,1,stm32_can[can->port].filter_top)){
                        stm32_can[can->port].filter_top++;
                    }

                    ret = 0;
                }else{
                    ret = -1;
                }
            }else{
                ret - -1;
            }
            if(0==ret){
                if(HAL_CAN_Start(pstcanhandle) != HAL_OK){
                    ret = -1;
                }
                if (HAL_CAN_ActivateNotification(pstcanhandle, CAN_IT_RX_FIFO0_MSG_PENDING) == HAL_OK){
                    stm32_can[can->port].inited = 1;
                }
            }
        }
    }
    return ret;
}

static int32_t set_can_filter(const uint8_t can_port, const uint32_t check_id, const uint32_t mask, const uint8_t rtr, const uint8_t filter_num)
{
    int32_t ret = -1;
    if(can_port<PORT_CAN_SIZE){
        CAN_FilterTypeDef tmp_filter;
        CAN_TxHeaderTypeDef * tx_header;
        CAN_HandleTypeDef * const pstcanhandle = &stm32_can[can_port].hal_can_handle;
        tx_header = &stm32_can[can_port].tx_header_def;
        if(tx_header->IDE == CAN_ID_EXT){
            tmp_filter.FilterBank = 0;
            tmp_filter.SlaveStartFilterBank = filter_num;
            tmp_filter.FilterMode = CAN_FILTERMODE_IDMASK;
            tmp_filter.FilterScale = CAN_FILTERSCALE_32BIT;
            tmp_filter.FilterIdHigh = ((check_id<<3)&0xFFFF0000)>>16;
            tmp_filter.FilterIdLow = ((check_id<<3)|CAN_ID_EXT|(rtr?CAN_RTR_REMOTE:CAN_RTR_DATA))&0xFFFF;
            tmp_filter.FilterMaskIdHigh = ((mask<<3)&0xFFFF0000)>>16;
            tmp_filter.FilterMaskIdLow = ((mask<<3)|0x0007)&0xFFFF;
            tmp_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
            tmp_filter.FilterActivation = ENABLE;
            ret = HAL_CAN_ConfigFilter(pstcanhandle, &tmp_filter);
            ret = (ret==HAL_OK)?0:(ret|0x80000000);
        }else{
            tmp_filter.FilterBank = 0;
            tmp_filter.SlaveStartFilterBank = filter_num;
            tmp_filter.FilterMode = CAN_FILTERMODE_IDMASK;
            tmp_filter.FilterScale = CAN_FILTERSCALE_32BIT;
            tmp_filter.FilterIdHigh = (check_id<<5)&0xFFFF;
            tmp_filter.FilterIdLow = CAN_ID_STD|(rtr?CAN_RTR_REMOTE:CAN_RTR_DATA);
            tmp_filter.FilterMaskIdHigh = (mask<<5)&0xFFFF;
            tmp_filter.FilterMaskIdLow = mask<<3;
            tmp_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
            tmp_filter.FilterActivation = ENABLE;
            ret = HAL_CAN_ConfigFilter(pstcanhandle, &tmp_filter);
            ret = (ret==HAL_OK)?0:(ret|0x80000000);
        }
    }
    return ret;
}

int32_t hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config)
{
    int32_t ret = -1;
    if(can!=NULL && can->port<PORT_CAN_SIZE && stm32_can[can->port].inited){
        ret = 0;

        /* Step 1. Disable all the current filter */
        CAN_FilterTypeDef tmp_filter;
        uint8_t i = stm32_can[can->port].start_filter;
        for(; i<stm32_can[can->port].filter_top; i++){
            tmp_filter.FilterBank = i;
            tmp_filter.FilterActivation = DISABLE;
            if(HAL_OK!=HAL_CAN_ConfigFilter(&stm32_can[can->port].hal_can_handle, &tmp_filter)){
                ret = -1;
                break;
            }
        }
        if(0==ret){
            stm32_can[can->port].filter_top = stm32_can[can->port].start_filter;

            /* Step 2. Config the filter */
            if(filter_grp_cnt==0){

                /* All CAN-ID will be pass */
                if(0==set_can_filter(can->port,0,0,0,stm32_can[can->port].filter_top)){
                    stm32_can[can->port].filter_top++;
                }

                if(0==set_can_filter(can->port,0,0,1,stm32_can[can->port].filter_top)){
                    stm32_can[can->port].filter_top++;
                }
            }else{
                uint8_t j = 0;
                for(; j<filter_grp_cnt && j<10; j++){
                    if(0==set_can_filter(can->port,filter_config[j].check_id,
                        filter_config[j].filter_mask,
                        filter_config[j].rtr,
                        stm32_can[can->port].filter_top)){
                        stm32_can[can->port].filter_top++;
                    }
                }
            }
        }
    }

    return ret;
}

int32_t hal_can_send(can_dev_t *can, can_frameheader_t *txheader, const void *data, const uint32_t timeout)
{
    int32_t ret = -1;
    uint32_t TxMailbox;
    if(can!=NULL && can->port<PORT_CAN_SIZE && txheader!=NULL){
        if(!stm32_can[can->port].inited){
            ret = -1;
        }else{
            CAN_HandleTypeDef * const pstcanhandle = &stm32_can[can->port].hal_can_handle;
            CAN_TxHeaderTypeDef tx_header;
            tx_header.IDE = CAN_ID_STD;
            if(CAN_ID_STD == tx_header.IDE){
                tx_header.StdId = txheader->id;
            }else{
                tx_header.ExtId = txheader->id;
            }
            tx_header.RTR = txheader->rtr?CAN_RTR_REMOTE:CAN_RTR_DATA;
            tx_header.DLC = txheader->dlc<8?txheader->dlc:8;
            ret = HAL_CAN_AddTxMessage(pstcanhandle, &tx_header, (uint8_t *)data, &TxMailbox);
            if(ret==HAL_OK){
                ret = 0;
            }else{
                ret = -1;
            }
        }
    }
    return ret;
}

int32_t hal_can_recv(can_dev_t *can, can_frameheader_t *rxheader, void *data, const uint32_t timeout)
{
    int32_t ret = -1;
    CAN_MAPPING* can_list = NULL;
    if( (can!=NULL) && ((can_list = get_can_list_logical(can->port))!=NULL) ){
        if(can_list->customer_recv!=NULL){
            ret = can_list->customer_recv(rxheader,data,timeout);
        }else{//polling mode
            CAN_HandleTypeDef * const pstcanhandle = &stm32_can[can->port].hal_can_handle;
            CAN_RxHeaderTypeDef rx_header;
            rx_header.IDE = CAN_ID_STD;
            ret =  HAL_CAN_GetRxMessage(pstcanhandle, CAN_RX_FIFO0, &rx_header, (uint8_t *)data);
            if(HAL_OK==ret){
                rxheader->id = rx_header.IDE==CAN_ID_STD?rx_header.StdId:rx_header.ExtId;
                rxheader->rtr = rx_header.RTR==CAN_RTR_DATA?0:1;
                rxheader->dlc = rx_header.DLC<8?rx_header.DLC:8;
                if(rxheader->rtr==0 || rxheader->dlc==0){
                    ret = 0;
                }else if(data==NULL){
                    ret = -1;
                }
                else{
                ret = -1;
                }
            }
        }
    }
    return ret;
}

int32_t hal_can_finalize(can_dev_t *can)
{
    int32_t ret = -1;

    if ( can != NULL && can->port<PORT_CAN_SIZE && stm32_can[can->port].inited ) {
        HAL_CAN_DeInit(&stm32_can[can->port].hal_can_handle);
        stm32_can[can->port].inited = 0;
        ret = 0;
    }
    return ret;
}
#endif
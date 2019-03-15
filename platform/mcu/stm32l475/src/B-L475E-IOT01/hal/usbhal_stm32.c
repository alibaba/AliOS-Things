/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/usbd.h"
#include "usb_device_os.h"
#include "usbendpoints_stm32.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"
#include "usbhal_stm_target.h"
#include <assert.h>
#include "stm32l4xx_hal_rcc.h"
#include "k_api.h"



PCD_HandleTypeDef hpcd;

/* endpoint definition to hal definition   */
#define EP_ADDR(ep) (((ep) >> 1)|((ep) & 1) << 7)
/* from hal definition to definition */
#define ADDR_EPIN(ep) (((ep) << 1) | 1)
#define ADDR_EPOUT(ep) (((ep) << 1))
/* id to detect if rx buffer is used or not  */


void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Configure USB FS GPIOs */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure DM DP Pins */
	GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

	/* Configure VBUS Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
	/* Configure ID pin */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Enable USB FS Clock */
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();

	/* Enable Power Clock*/
	__HAL_RCC_PWR_CLK_ENABLE();  
	/* Enable USB power on Pwrctrl CR2 register */
	HAL_PWREx_EnableVddUSB();

	/* Set USB FS Interrupt priority */
	HAL_NVIC_SetPriority(OTG_FS_IRQn, 7, 0);

	/* Enable USB FS Interrupt */
	//HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
      
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{  
	/* Disable USB FS Clock */
	__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
	__HAL_RCC_SYSCFG_CLK_DISABLE(); 
}


/*  this call at device reception completion on a Out Enpoint  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{    
    usbd_handle_type_def *pdev = ((usbd_handle_type_def *)(hpcd->pData));
    USBHAL_Private_t *priv = pdev->pHalData;
    uint8_t endpoint = ADDR_EPOUT(epnum);
    priv->epComplete[endpoint] = 1;
    /* -2 endpoint 0 In out are not in call back list */
    if (epnum) {
        if(pdev->pClass->DataOut != NULL) {
            pdev->pClass->DataOut(pdev); 
        }
    } else {
        usbd_ep0_out(pdev);
    }
}

/*  this is call at device transmission completion on In endpoint */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    usbd_handle_type_def *pdev = ((usbd_handle_type_def *)(hpcd->pData));
    USBHAL_Private_t *priv = pdev->pHalData;
    uint8_t endpoint = ADDR_EPIN(epnum);
    priv->epComplete[endpoint] = 1;
    /*  -2 endpoint 0 In out are not in call back list */
    if (epnum) {
        if(pdev->pClass->DataIn != NULL) {
            pdev->pClass->DataIn(pdev); 
        }
    } else {
        usbd_ep0_in(pdev);
    }
}
/*  This is call at device set up reception  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    usbd_handle_type_def *pdev = ((usbd_handle_type_def *)(hpcd->pData));

    usbd_ep0_setup_callback(pdev);
    usbd_hal_ep0_read(pdev);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    usbd_suspend_state_changed(1);
    
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    usbd_suspend_state_changed(0);
    
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    usbd_connect_state_changed(1);

}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{  
    usbd_connect_state_changed(0);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    usbd_handle_type_def *pdev = ((usbd_handle_type_def *)(hpcd->pData));
    USBHAL_Private_t *priv = pdev->pHalData;
    
    unsigned int i;
    for(i = 0; i < hpcd->Init.dev_endpoints; i++) {
        priv->epComplete[2 * i]=0;
        HAL_PCD_EP_Close(hpcd, EP_ADDR(2 * i));
        HAL_PCD_EP_Flush(hpcd, EP_ADDR(2 * i));
        priv->epComplete[2 * i + 1]=0;
        HAL_PCD_EP_Close(hpcd, EP_ADDR(2 * i + 1));
        HAL_PCD_EP_Flush(hpcd, EP_ADDR(2 * i + 1));

    }
    usbd_bus_reset(pdev);
    usbd_hal_realise_endpoint(pdev, EP0IN, MAX_PACKET_SIZE_EP0, 0);
    usbd_hal_realise_endpoint(pdev, EP0OUT, MAX_PACKET_SIZE_EP0,0);
    
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
    usbd_handle_type_def *pdev=((usbd_handle_type_def *)(hpcd->pData));

    if(pdev->pClass->SOF != NULL) {
        pdev->pClass->SOF(pdev);
    }
}

usbd_stat_t usbd_hal_init(void *pdev)
{
    USBHAL_Private_t *HALPriv = usbd_malloc(sizeof(USBHAL_Private_t));
    /*  initialized all field of init including 0 field  */
    /*  constructor does not fill with zero */
    hpcd.Instance = USB_OTG_FS;
    /*  initialized all field of init including 0 field  */
    /*  constructor does not fill with zero */
    memset(&hpcd.Init, 0, sizeof(hpcd.Init));
    hpcd.Init.dev_endpoints = NB_ENDPOINT;
    hpcd.Init.use_dedicated_ep1 = 0;
    hpcd.Init.ep0_mps =   MAX_PACKET_SIZE_EP0;
	//hpcd.Init.ep0_mps = DEP0CTL_MPS_64;
  	hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd.Init.Sof_enable = 0;
    hpcd.Init.speed = PCD_SPEED_FULL;    
    hpcd.Init.vbus_sensing_enable = 1;
    /*  pass instance for usage inside call back */
    hpcd.pData = pdev;

    ((usbd_handle_type_def *)pdev)->pData = &hpcd;
    ((usbd_handle_type_def *)pdev)->pHalData = (void*)HALPriv;


    hpcd.State = HAL_PCD_STATE_RESET;

    HAL_PCD_Init(&hpcd);

	HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);
	HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);
	HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x40);
	HAL_PCDEx_SetTxFiFo(&hpcd, 2, 0x40);
#if 0
	HAL_PCDEx_SetRxFiFo(&hpcd, (MAXTRANSFER_SIZE/4));
    /*  bulk/int 64 bytes in FS  */
    HAL_PCDEx_SetTxFiFo(&hpcd, 0, (MAX_PACKET_SIZE_EP0/4)+1);
    /*  bulk/int bytes in FS */
    HAL_PCDEx_SetTxFiFo(&hpcd, 1, (MAX_PACKET_SIZE_EP1/4)+1);
    HAL_PCDEx_SetTxFiFo(&hpcd, 2, (MAX_PACKET_SIZE_EP2/4));
    /* ISOchronous */
    HAL_PCDEx_SetTxFiFo(&hpcd, 3, (MAX_PACKET_SIZE_EP3/4));
#endif

    return USBD_OK;
}


usbd_stat_t usbd_hal_deinit(void *pdev)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
	HAL_PCD_DeInit(((usbd_handle_type_def *)pdev)->pData);
    usbd_free(priv);

    return USBD_OK;
}

usbd_stat_t usbd_hal_start(void *pdev)
{
    HAL_PCD_Start(((usbd_handle_type_def *)pdev)->pData);

    return USBD_OK;
}

usbd_stat_t usbd_hal_stop(void *pdev)
{
    HAL_PCD_Stop(((usbd_handle_type_def *)pdev)->pData);

    return USBD_OK;
}

void usbd_hal_set_address(void *pdev, uint8_t address)
{
	HAL_PCD_SetAddress(((usbd_handle_type_def *)pdev)->pData, address);
    usbd_hal_ep0_write(pdev, 0, 0);
}

uint32_t HAL_PCDEx_GetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo)
{
    uint32_t len;
    if (fifo == 0) len = hpcd->Instance->DIEPTXF0_HNPTXFSIZ>>16;
    else
        len =  hpcd->Instance->DIEPTXF[fifo - 1] >> 16;
    return len*4;
}

bool usbd_hal_realise_endpoint(void *pdev, uint8_t endpoint, uint32_t maxPacket, uint32_t flags)
{
    uint32_t epIndex = EP_ADDR(endpoint);
    uint32_t type;
    uint32_t len;
    HAL_StatusTypeDef ret;
    switch (endpoint) {
    case EP0IN:
    case EP0OUT:
        type =  0;
        break;
    case EPISO_IN:
    case EPISO_OUT:
        type = 1;
        break;
    case EPBULK_IN:
    case EPBULK_OUT:
        type =  2;
        break;
    case EPINT_IN:
    case EPINT_OUT:
        type =  3;
        break;
    }
    if (maxPacket > MAXTRANSFER_SIZE) return false;
    if (epIndex & 0x80) {
        len = HAL_PCDEx_GetTxFiFo(((usbd_handle_type_def *)pdev)->pData, epIndex & 0x7f);
        assert(len >= maxPacket);
        
    }
    ret = HAL_PCD_EP_Open(((usbd_handle_type_def *)pdev)->pData, epIndex,  maxPacket, type);
    assert(ret!=HAL_BUSY);
    return (ret == HAL_OK) ? true:false;
}

usbd_stat_t usbd_hal_close_endpoint(void *pdev, uint8_t endpoint)
{
    HAL_PCD_EP_Close(((usbd_handle_type_def *)pdev)->pData, EP_ADDR(endpoint));
    return USBD_OK;
}

usbd_stat_t usbd_hal_flush_endpoint(void *pdev, uint8_t endpoint)
{
    HAL_PCD_EP_Flush(((usbd_handle_type_def *)pdev)->pData, EP_ADDR(endpoint));
    return USBD_OK;
}

void usbd_hal_stall_endpoint(void *pdev, uint8_t endpoint)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
	HAL_StatusTypeDef ret;
	priv->epComplete[endpoint] = 0;
	ret = HAL_PCD_EP_SetStall(((usbd_handle_type_def *)pdev)->pData, EP_ADDR(endpoint));
	assert(ret!=HAL_BUSY);
}

void usbd_hal_unstall_endpoint(void *pdev, uint8_t endpoint)
{
	HAL_StatusTypeDef ret;
    ret = HAL_PCD_EP_ClrStall(((usbd_handle_type_def *)pdev)->pData, EP_ADDR(endpoint));
	assert(ret!=HAL_BUSY);

}

bool usbd_hal_get_endpoint_stall_state(void *pdev, uint8_t endpoint)
{
    return false;
}

ep_status usbd_hal_endpoint_write(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t size)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
    uint32_t epIndex = EP_ADDR(endpoint);
    HAL_StatusTypeDef ret;
    // clean transmission end flag before requesting transmission
    priv->epComplete[endpoint] = 2;
    ret = HAL_PCD_EP_Transmit(((usbd_handle_type_def *)pdev)->pData, epIndex, data, size);
	assert(ret!=HAL_BUSY);
    // update the status
    if (ret != HAL_OK) return EP_INVALID; 
    // fix me return is too simple
    return EP_PENDING;
}

ep_status usbd_hal_endpoint_write_result(void *pdev, uint8_t endpoint)
{    
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
    if (priv->epComplete[endpoint] == 1)
        return EP_COMPLETED;
    return EP_PENDING;
}

ep_status usbd_hal_endpoint_read(void *pdev, uint8_t endpoint, uint32_t maximumSize)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
    uint32_t epIndex = EP_ADDR(endpoint);
    uint8_t* pBuf = (uint8_t *)priv->pBufRx;
	HAL_StatusTypeDef ret;
    // clean reception end flag before requesting reception
    priv->epComplete[endpoint] = 2;
    ret = HAL_PCD_EP_Receive(((usbd_handle_type_def *)pdev)->pData, epIndex, pBuf, maximumSize);
	assert(ret!=HAL_BUSY);
    return EP_PENDING;
}

ep_status usbd_hal_endpoint_read_result(void *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t *bytesRead)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
	if (priv->epComplete[endpoint]==0) {
		/*  no reception possible !!! */
		bytesRead = 0;
        return EP_COMPLETED;
    }else if ((priv->epComplete[endpoint]!=1))
	return EP_PENDING;
    uint32_t epIndex = EP_ADDR(endpoint);
    uint8_t  *buff = (uint8_t *)priv->pBufRx;
    uint32_t length = (uint32_t) HAL_PCD_EP_GetRxCount(((usbd_handle_type_def *)pdev)->pData, epIndex);
    memcpy(buffer, buff, length);
    *bytesRead = length;
	priv->epComplete[endpoint]= 0;
    return EP_COMPLETED;
}

uint32_t usbd_hal_endpoint_readcore(uint8_t endpoint, uint8_t *buffer)
{
    return 0;
}

void usbd_hal_ep0_read(void *pdev)
{
     USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
	 uint32_t epIndex = EP_ADDR(EP0OUT);
	 uint8_t *pBuf = (uint8_t *)priv->pBufRx0;
	 HAL_StatusTypeDef ret;
	 priv->epComplete[EP0OUT] = 2;
	 ret = HAL_PCD_EP_Receive(((usbd_handle_type_def *)pdev)->pData, epIndex, pBuf, MAX_PACKET_SIZE_EP0 );
	 assert(ret!=HAL_BUSY);

}

uint32_t usbd_hal_get_ep0_read_result(void *pdev, uint8_t *buffer)
{
    USBHAL_Private_t *priv = ((usbd_handle_type_def *)pdev)->pHalData;
    uint32_t length = (uint32_t) HAL_PCD_EP_GetRxCount(((usbd_handle_type_def *)pdev)->pData, 0);
	priv->epComplete[EP0OUT] = 0;
    if (length) {
        uint8_t  *buff = (uint8_t *)priv->pBufRx0;
        memcpy(buffer, buff, length);
    }
    return length;
}

// read setup packet
void usbd_hal_ep0_setup(uint8_t *buffer)
{
    memcpy(buffer,  hpcd.Setup, MAX_PACKET_SIZE_SETUP);
	memset(hpcd.Setup,0,MAX_PACKET_SIZE_SETUP);
}

void usbd_hal_ep0_read_stage(void)
{
}

void usbd_hal_ep0_write(void *pdev, uint8_t *buffer, uint32_t size)
{
    /*  check that endpoint maximum size is not exceeding TX fifo */
    assert(hpcd.IN_ep[0].maxpacket >= size);
    usbd_hal_endpoint_write(pdev, EP0IN, buffer, size);
}

void usbd_hal_get_ep0_write_result(void)
{

}

void usbd_hal_ep0_stall(void *pdev)
{
    usbd_hal_stall_endpoint(pdev, EP0IN);
}

void usbd_hal_connect(void)
{
    NVIC_EnableIRQ(USBHAL_IRQn);
}

void usbd_hal_disconnect(void)
{
    NVIC_DisableIRQ(USBHAL_IRQn);
}

void usbd_hal_configure_device(void)
{
    // Not needed
}

void usbd_hal_unconfigure_device(void)
{
    // Not needed
}

void usbd_hal_remote_wakeup(void)
{
}

void OTG_FS_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_PCD_IRQHandler(&hpcd);
    krhino_intrpt_exit();
}


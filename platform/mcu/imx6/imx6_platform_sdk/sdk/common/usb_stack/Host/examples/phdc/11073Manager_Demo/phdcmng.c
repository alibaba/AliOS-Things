/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: phdcmng.c$
* $Version : 1.0.0..0$
* $Date    : Jan-29-2010$
*
* Comments:
*
*   This file is an example of device drivers for the PHDC class which intends
*   to demonstrate the IEEE-11073 Manager functionality.
*
*END************************************************************************/

#include "framework.h"
#include "phdcmng.h"
#include "mem_util.h"

/***************************************
**
** Globals
*/

volatile DEVICE_STRUCT phdc_device = { 0 };
_usb_host_handle host_handle;

TR_INIT_PARAM_STRUCT tr;
USB_STATUS status = USB_OK;
USB_PHDC_PARAM phd_call_param;

FW_EVENT_STRUCT USB_Event;


/* Application function prototypes */
void ApplicationInit(void);
void ApplicationTask(void);
void phdc_manager_inuse(void);
boolean phdc_manager_initialize_device(void);
void usb_host_phdc_manager_event( _usb_device_instance_handle dev_handle,
                                  _usb_interface_descriptor_handle intf_handle,
                                   uint_32 event_code);

/* Manager APDU handlers */
void phdc_manager_aarq_handler(uint_8 *buffer);
void phdc_manager_prst_handler(uint_8 *buffer);
void phdc_manager_rlrq_handler(uint_8 *buffer);
void phdc_manager_abrt_handler(uint_8 *buffer);

void phdc_manager_prst_mdc_noti_config(APDU *apdu, DATA_apdu *dataApdu);
void phdc_manager_prst_mdc_noti_scanrep_fixed(APDU *apdu, DATA_apdu *dataApdu);
void phdc_manager_prst_get(APDU *apdu, DATA_apdu *dataApdu);

/* Manager configuration and scan handlers */
void phdc_manager_validate_print_fixedscan(ObservationScanFixedList *scanList);
ConfigObject* phdc_manager_get_configobj_byhandle(ConfigObjectList *confObjList, HANDLE obj_handle);
AVA_Type*     phdc_manager_get_attribute_byid(ConfigObject *confObj, OID_Type attribute_id);

/* Manager presentation services */
void    phdc_manager_handle_metric_nu_fixedscan(ConfigObject *confObj, ObservationScanFixed* observation);
boolean phdc_manager_print_nomenclature(OID_Type type);
boolean phdc_manager_print_partition(NomPartition partition);
void    phdc_manager_print_float_val(intu8 *value);
void    phdc_manager_print_sfloat_val(intu8 *value);
void    phdc_manager_print_abs_timestamp(intu8 *value);


void usb_host_phdc_send_callback(USB_PHDC_PARAM *call_param);
void usb_host_phdc_recv_callback(USB_PHDC_PARAM *call_param);
void usb_host_phdc_ctrl_callback(USB_PHDC_PARAM *call_param);

USB_PHDC_PARAM phd_call_param;

/* Table of driver capabilities this application wants to use */
#if (defined(__IAR_SYSTEMS_ICC__))
static const USB_HOST_DRIVER_INFO DriverInfoTable[] = {
#else
static USB_HOST_DRIVER_INFO DriverInfoTable[] = {
#endif
    {
            {0x00, 0x00},       /* Vendor ID per USB-IF                               */
            {0x00, 0x00},       /* Product ID per manufacturer                        */
            USB_CLASS_PHDC,     /* Class code (PHDC)                				  */
            0, 					/* Sub-Class code (PHDC does not assign a subclass)   */
            0, 					/* Protocol                         */
            0,                  /* Reserved                         */
            usb_host_phdc_manager_event /* Application call back function   */
        },
    /* USB 1.1 hub */
    {
            {0x00, 0x00},           /* Vendor ID per USB-IF             */
            {0x00, 0x00},           /* Product ID per manufacturer      */
            USB_CLASS_HUB,          /* Class code                       */
            USB_SUBCLASS_HUB_NONE,  /* Sub-Class code                   */
            USB_PROTOCOL_HUB_LS,    /* Protocol                         */
            0,                      /* Reserved                         */
            usb_host_hub_device_event /* Application call back function   */
        },
    {
            {0x00, 0x00},       /* All-zero entry terminates        */
            {0x00, 0x00},       /* driver info list.                */
            0,
            0,
            0,
            0,
        NULL},
};


/* Predefined PHDC Manager messages */

/* association response to send */ 
uint_8 PHD_MNG_ASSOC_RES[ASSOC_RES_SIZE] = 
{   
  0xE3, 0x00,								 /* APDU CHOICE Type (AareApdu) */
  0x00, 0x2C, 								 /* CHOICE.length = 44 */
  0x00, 0x03,							     /* result = accepted-unknown-config (UPDATED by manager) */
  0x50, 0x79,								 /* data-proto-id = 20601 */
  0x00, 0x26,								 /* data-proto-info length = 38 */
  0x80, 0x00, 0x00, 0x00,					 /* protocolVersion */
  0x80, 0x00,								 /* encoding rules = MDER */
  0x80, 0x00, 0x00, 0x00,					 /* nomenclatureVersion */
  0x00, 0x00, 0x00, 0x00,					 /* functionalUnits */
  0x80, 0x00, 0x00, 0x00,					 /* systemType = sys-type-manager */
  0x00, 0x08, 							     /* system-id length = 8 and value */
  0x4C, 0x4E, 0x49, 0x41, 0x47, 0x45, 0x4E, 0x54, /* (manufacturer- and device- specific) */
  0x00, 0x00,								 /* manager's response to config-id is always 0 */
  0x00, 0x00, 0x00, 0x00, 					 /* manager's response to data-req-mode-capab is always 0 */
  0x00, 0x00, 0x00, 0x00				     /* optionList.count = 0 | optionList.length = 0 */
};

uint_8 PHD_MNG_CONFIG_RES[CONFIG_RES_SIZE] = 
{	
  0xE7, 0x00, 								 /* APDU CHOICE Type (PrstApdu) */
  0x00, 0x16, 								 /* CHOICE.length = 22 */
  0x00, 0x14, 								 /* OCTET STRING.length = 20 */
  0x00, 0x00,							     /* invoke-id = 0x0000 (updated by mng) */
  0x02, 0x01,								 /* CHOICE (Remote Operation Response | Confirmed Event Report) */
  0x00, 0x0E,				   				 /* CHOICE.length = 14 */
  0x00, 0x00, 								 /* obj-handle = 0 (MDS object) */
  0x00, 0x00, 0x00, 0x00,					 /* currentTime = 0 */
  0x0D, 0x1C,								 /* event-type = MDC_NOTI_CONFIG */
  0x00, 0x04,						         /* event-reply-info.length = 4 */
  0x00, 0x00,							     /* ConfigReportRsp.config-report-id = 0x0000 (updated by mng) */
  0x00, 0x00 								 /* ConfigReportRsp.config-result = accepted-config (updated by mng) */
};

uint_8 PHD_MNG_GET_MDS_ATTR[GET_MDS_ATTR_SIZE] =
{	
  0xE7, 0x00,								/* APDU CHOICE Type (PrstApdu) */
  0x00, 0x0E,								/* CHOICE.length = 14 */
  0x00, 0x0C,								/* OCTET STRING.length = 12 */
  0x34, 0x56,								/* invoke-id = 0x3456 (start of DataApdu. MDER encoded.) */
  0x01, 0x03,								/* CHOICE (Remote Operation Invoke | Get) */
  0x00, 0x06,								/* CHOICE.length = 6 */
  0x00, 0x00,								/* handle = 0 (MDS object) */
  0x00, 0x00,								/* attribute-id-list.count = 0 (all attributes) */
  0x00, 0x00								/* attribute-id-list.length = 0 */
};

uint_8 PHD_MNG_SCAN_RES[SCAN_RES_SIZE] =
{	
  0xE7, 0x00,								/* APDU CHOICE Type (PrstApdu) */
  0x00, 0x12,								/* CHOICE.length = 18 */
  0x00, 0x10,								/* OCTET STRING.length = 16 */
  0x43, 0x21,								/* invoke-id = 0x4321 (start of DataApdu. MDER encoded.) */
  0x02, 0x01,								/* CHOICE(Remote Operation Response | Confirmed Event Report) */
  0x00, 0x0A, 								/* CHOICE.length = 10 */
  0x00, 0x00, 								/* obj-handle = 0 (MDS object) */
  0xFF, 0xFF, 0xFF, 0xFF,					/* currentTime = 0 */
  0x0D, 0x1D,								/* event-type = MDC_NOTI_SCAN_REPORT_FIXED */
  0x00, 0x00								/* event-reply-info.length = 0 */
};

/* release request to send */
uint_8 PHD_MNG_REL_REQ[REL_REQ_SIZE] = {
0xE4, 0x00,                           /* APDU CHOICE Type (RlrqApdu) */
0x00, 0x02,                           /* CHOICE.length = 2 */
0x00, 0x00                            /* reason = normal */
};

/* release response to the device */
uint_8 PHD_WSL_REL_RES[REL_RES_SIZE] = {
0xE5, 0x00,                           /* APDU CHOICE Type (RlrsApdu) */
0x00, 0x02,                           /* CHOICE.length = 2 */
0x00, 0x00                            /* reason = normal */
};

/* abort request to send */
uint_8 PHD_MNG_ABRT_REQ[ABRT_SIZE] = {
0xE6, 0x00,                           /* APDU CHOICE Type (RlrsApdu) */
0x00, 0x02,                           /* CHOICE.length = 2 */
0x00, 0x02                            /* reason = normal */
};
#if (defined LITTLE_ENDIAN)
intu16 SWAPBYTE16(intu16 a) 
 {
    return ((a >> (intu16)0x08) | ((a & 0xFF) << (intu16)0x08));
 }

intu32 SWAPBYTE32(intu32 a) 
 {
    return (intu32)((SWAPBYTE16((uint_32)(a) & (uint_32)0xFFFF) << 0x10) |(SWAPBYTE16((uint_32)((a) >> 0x10))));
 }
#else
#define SWAPBYTE16(a) 			(a)
#define SWAPBYTE32(a) 			(a)
#endif
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ApplicationInit
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void ApplicationInit(void)
{

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init(HOST_CONTROLLER_NUMBER, /* Use value in header file */
        MAX_FRAME_SIZE,         /* Frame size per USB spec  */
        &host_handle);          /* Returned pointer */

    if(status != USB_OK) 
    {
        printf("\n\rUSB Host Initialization failed! STATUS: 0x%x", (unsigned int)status);
        fflush(stdout);
        exit(1);
    }

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
#if (defined(__IAR_SYSTEMS_ICC__))
    status = _usb_host_driver_info_register(host_handle, (void*)DriverInfoTable);
#else
    status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
#endif
    if(status != USB_OK) 
    {
        printf("\n\rDriver Registration failed! STATUS: 0x%x", (unsigned int)status);
        fflush(stdout);
        exit(1);
    }

    printf("\n\r****************************************************************************");
    printf("\n\rUSB PHDC Manager Demo");
    printf("\n\rWaiting for PHDC Agent to be attached...");

}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ApplicationTask
* Returned Value : None
* Comments       :
*     Application function 
*END*--------------------------------------------------------------------*/

void ApplicationTask(void)
{
  switch (phdc_device.DEV_STATE)
  {
    case USB_DEVICE_IDLE:
        break;

    case USB_DEVICE_ATTACHED:
        printf(" PHDC device attached");
        phdc_device.DEV_STATE = USB_DEVICE_SET_INTERFACE_STARTED;

        status = _usb_hostdev_select_interface(phdc_device.DEV_HANDLE, phdc_device.INTF_HANDLE, (pointer)&(phdc_device.CLASS_INTF));
        if(status != USB_OK)
        {
            printf("\n\rError in _usb_hostdev_select_interface: %x",(unsigned int) status);
            fflush(stdout);
            exit(1);
        }
        break;

    case USB_DEVICE_SET_INTERFACE_STARTED:
        break;

    case USB_DEVICE_INTERFACED:
        printf("\n\rPHDC device interfaced and ready"); 
        /* Advance to the INUSE state to process the PHDC messages */
        phdc_device.DEV_STATE = USB_DEVICE_INUSE;
        fw_event_set(&USB_Event, USB_EVENT_CTRL);
        break;

    case USB_DEVICE_INUSE:
        phdc_manager_inuse();
        break;

    case USB_DEVICE_DETACHED:
        printf("\n\rGoing to idle state");
        phdc_device.DEV_STATE = USB_DEVICE_IDLE;
        phdc_device.phd_recv_in_progress = FALSE;
        if(phdc_device.phd_buffer!=NULL)
        {            	
         USB_mem_free(phdc_device.phd_buffer);
         phdc_device.phd_buffer = NULL;
        }
        if(phdc_device.confObjList != NULL)
        {
         USB_mem_free(phdc_device.confObjList);
         phdc_device.confObjList = NULL;
        }
        break;
  }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_inuse_loop
* Returned Value : None
* Comments       :
*     This function handles all the manager events and actions during the usage time
*END*--------------------------------------------------------------------*/

void phdc_manager_inuse(void)
{
 USB_STATUS status; 
 APDU *apdu;
 
 if(phdc_device.phd_recv_in_progress == FALSE)
 {
  if(phdc_device.phd_recv_size)
  {  	
   apdu = (APDU*)(phdc_device.phd_buffer);
   
   /* APDU processing */
   switch(phdc_device.phd_manager_state)
   {
   	case PHD_MNG_UNASSOCIATED:
   		  		/* Expecting only an RxAssocRequest message (AarqApdu) */
   		  		if(apdu->choice == AARQ_CHOSEN)
                {                
   		  		      phdc_manager_aarq_handler(phdc_device.phd_buffer);
                }
   		  		break;
   	case PHD_MNG_WAIT_CONFIG:
   	case PHD_MNG_OPERATING:
   			    /* Handle the messages received in this state */
   			    if(apdu->choice == PRST_CHOSEN)
   			    {
   			      phdc_manager_prst_handler(phdc_device.phd_buffer);
   			    }
   			    else
   			    {
   			      if(apdu->choice == ABRT_CHOSEN)
   			      {
   			       phdc_manager_abrt_handler(phdc_device.phd_buffer);
   			      }
   			      else
   			      {
   			       if(apdu->choice == RLRQ_CHOSEN)
   			       {
   			        phdc_manager_rlrq_handler(phdc_device.phd_buffer);
   			       }
   			      }
   			    }   			    
   			    break;   
   }
  }

  /* Schedule a new RX */
  
  phdc_device.phd_recv_in_progress = TRUE;

  phd_call_param.ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
  phd_call_param.qos = 0xFE;
  phd_call_param.buff_ptr = phdc_device.phd_buffer;
  phd_call_param.buff_size = APDU_MAX_BUFFER_SIZE;
 
  status = usb_class_phdc_recv_data(&phd_call_param);
 }
 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_aarq_handler
* Returned Value : None
* Comments       :
*     This function checks the received association request message
*END*--------------------------------------------------------------------*/
void phdc_manager_aarq_handler(uint_8 *buffer)
{
 APDU *apdu = (APDU *)buffer;
 
 AARE_apdu *pAare;
 USB_PHDC_PARAM *phd_call_param_send;
 
 Associate_result assocResult = REJECTED_UNKNOWN; 
 AARQ_apdu *pAarq = &(apdu->u.aarq);
  
  
 if(pAarq->assoc_version != ASSOC_VERSION1)
 {
   assocResult = REJECTED_UNSUPPORTED_ASSOC_VERSION;
 }
 else
 {
   if(pAarq->data_proto_list.value[0].data_proto_id != DATA_PROTO_ID_20601)
   {
    assocResult = REJECTED_NO_COMMON_PROTOCOL;
   }
   else
   {
    assocResult = ACCEPTED_UNKNOWN_CONFIG;
    /* Advance the manager state */
    printf("\n\r11073 MNG Demo: Received a valid association request.");
   	fflush(stdout);
   	phdc_device.phd_manager_state = PHD_MNG_WAIT_CONFIG;
   }
 }
 
 /* Send back the response */
  
 /* APDU now pointing to the response */
 apdu = (APDU*)&PHD_MNG_ASSOC_RES[0];
  
 /* Update the association result in the message template */
 pAare = &(apdu->u.aare);
 pAare->result = SWAPBYTE16(assocResult);
  
 /* Send data */
 phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
 if(phd_call_param_send != NULL)
 { 	
  phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
  phd_call_param_send->metadata = FALSE;
  phd_call_param_send->buff_ptr = &PHD_MNG_ASSOC_RES[0];
  phd_call_param_send->buff_size = ASSOC_RES_SIZE;
 
  printf("\n\r11073 MNG Demo: Send back association response. Waiting for config...");
  usb_class_phdc_send_data(phd_call_param_send);
 }
 else
 {
  printf("\n\r11073 MNG Demo: Cannot allocate memory for the Association response parameter");
 }
 	
 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_prst_handler
* Returned Value : None
* Comments       :
*     This function handles the PRST APDU message
*END*--------------------------------------------------------------------*/
void phdc_manager_prst_handler(uint_8 *buffer)
{
 APDU *apdu = (APDU *)buffer;  
  
 PRST_apdu *pPrst = (PRST_apdu *)&(apdu->u.prst); 
 DATA_apdu *dataApdu = (DATA_apdu *)&(pPrst->value[0]);
 
  
 /* Check the choice and obj-handle */
 if((dataApdu->choice.choice == ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN) &&
    (dataApdu->choice.u.roiv_cmipConfirmedEventReport.obj_handle == 0)       /* MDS Object */     
   )
 {
   switch(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_type)
   {
   	case MDC_NOTI_CONFIG: 
   	        phdc_manager_prst_mdc_noti_config(apdu, dataApdu);
   	        break;
   	case MDC_NOTI_SCAN_REPORT_FIXED:
   	        phdc_manager_prst_mdc_noti_scanrep_fixed(apdu, dataApdu);
   	        break;
   }      
 }
  
 if((dataApdu->choice.choice == RORS_CMIP_GET_CHOSEN) &&
    (dataApdu->choice.u.rors_cmipGet.obj_handle == 0)       /* MDS Object */     
   )
 {
  phdc_manager_prst_get(apdu, dataApdu);
 }

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_rlrq_handler
* Returned Value : None
* Comments       :
*     This function handles the RLRQ APDU message
*END*--------------------------------------------------------------------*/
void phdc_manager_rlrq_handler(uint_8 *buffer)
{
 APDU *apdu = (APDU *)buffer;
 USB_PHDC_PARAM *phd_call_param_send;
  
 RLRQ_apdu *pRlrq = (RLRQ_apdu *)&(apdu->u.prst); 
 
 printf("\n\r11073 MNG Demo: Received a Release request. Reason = %d", pRlrq->reason);
 if(pRlrq->reason == RELEASE_REQUEST_REASON_NORMAL)
 {
  printf(" (REASON_NORMAL)");
 }
 
 if(phdc_device.confObjList != NULL)
 {
  USB_mem_free(phdc_device.confObjList);
  phdc_device.confObjList = NULL;
  printf("\n\r11073 MNG Demo: Device configuration memory is now deallocated");  
 } 
 
 printf("\n\r11073 MNG Demo: Manager state is now UNASSOCIATED");  
 /* Advance the manager state */
 fflush(stdout);
 phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
 
 time_delay(300);
 
 /* Send data -> Release Response */
 phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
 if(phd_call_param_send != NULL)
 { 	
  phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
  phd_call_param_send->metadata = FALSE;
  phd_call_param_send->buff_ptr = (uint_8*)&PHD_WSL_REL_RES[0];
  phd_call_param_send->buff_size = REL_RES_SIZE;  
  usb_class_phdc_send_data(phd_call_param_send);
 }
 else
 {
  printf("\n\r11073 MNG Demo: Cannot allocate memory for the release response parameter");
 }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_abrt_handler
* Returned Value : None
* Comments       :
*     This function handles the RLRQ APDU message
*END*--------------------------------------------------------------------*/
void phdc_manager_abrt_handler(uint_8 *buffer)
{
 APDU *apdu = (APDU *)buffer;
  
 ABRT_apdu *pAbrt = (ABRT_apdu *)&(apdu->u.prst); 
 
 printf("\n\r11073 MNG Demo:Received an Abort request. Reason = %d", pAbrt->reason);

 if(phdc_device.confObjList != NULL)
 {
  USB_mem_free(phdc_device.confObjList);
  phdc_device.confObjList = NULL;
  printf("\n\r11073 MNG Demo: Device configuration memory is now deallocated");  
 } 
 
 printf("\n\r11073 MNG Demo: Manager state is now UNASSOCIATED");  
 /* Advance the manager state */
 fflush(stdout);
 phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_prst_mdc_noti_config
* Returned Value : None
* Comments       :
*     This function handles the MDC_NOTI_CONFIG event types
*END*--------------------------------------------------------------------*/
void phdc_manager_prst_mdc_noti_config(APDU *apdu, DATA_apdu *dataApdu)
{
   ConfigResult configResult = UNSUPPORTED_CONFIG; 
   InvokeIDType configInvokeID = 0;
   ConfigId configReportId = 0;   
   
   intu16 config_length = dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.length; 
   intu16 i,j,configOffset = 0, attrOffset;
   intu16 configListSize;
      
   ConfigReport *configReport = (ConfigReport *)&(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
   ConfigObjectList *confObjList = &(configReport->config_obj_list);
   
   PRST_apdu *pPrst;
   DATA_apdu *respDataApdu;
   ConfigReportRsp *reportResp;
   
   USB_PHDC_PARAM *phd_call_param_send;

 
   configReport->config_report_id = SWAPBYTE16(configReport->config_report_id);
   confObjList->count = SWAPBYTE16(confObjList->count);
   printf("\n\r11073 MNG Demo: Received a configuration event report.");
   printf("\n\r11073 MNG Demo: -------------------------------------------");
   printf("\n\r11073 MNG Demo: Configuration Report Id: %d.", configReport->config_report_id); 
   printf("\n\r11073 MNG Demo: Number of Agent Measurements Objects: %d.", confObjList->count); 
   
   apdu->length = SWAPBYTE16(apdu->length);
   if( (apdu->length + sizeof(apdu->choice) + sizeof(apdu->length)) < APDU_MAX_BUFFER_SIZE)
   {
    configReportId = configReport->config_report_id;
    configInvokeID = dataApdu->invoke_id;
    confObjList->length = SWAPBYTE16(confObjList->length);
    /* Save the config obj list in the device structure */
    configListSize = (uint_16)(confObjList->length + sizeof(confObjList->count) + sizeof(confObjList->length));
   
    if(phdc_device.confObjList!=NULL)
    {
      USB_mem_free(phdc_device.confObjList);
    }
   
    phdc_device.confObjList = USB_mem_alloc_uncached_zero(configListSize);
   
   
    if(phdc_device.confObjList!=NULL)
    {
     /* Store the Configuration report in the application */
     USB_mem_copy((void*)confObjList, (void*)phdc_device.confObjList, configListSize);
   	
     for(i = 0; i<confObjList->count; i++)
     {
      ConfigObject *confObj = (ConfigObject *)(((uint_8*)&confObjList->value[0]) + configOffset);
      confObj->obj_handle = SWAPBYTE16(confObj->obj_handle);
      confObj->obj_class = SWAPBYTE16(confObj->obj_class);
      confObj->attributes.count = SWAPBYTE16(confObj->attributes.count);
      printf("\n\r11073 MNG Demo:  > Object Handle %d: Class = %d  Num Attributes = %d.", confObj->obj_handle, confObj->obj_class, confObj->attributes.count);
      fflush(stdout);
      attrOffset = 0;
      for(j = 0; j<confObj->attributes.count; j++)
      {
       AVA_Type *attr = (AVA_Type *)((uint_8*)(&confObj->attributes.value[0]) + attrOffset);
       printf("\n\r11073 MNG Demo:  > > Attribute%d: Id = ", j);
       if(!phdc_manager_print_nomenclature(attr->attribute_id))
       {
        printf("%d (unsupported ASCII nomenclature", SWAPBYTE16(attr->attribute_id));
       }
       
       fflush(stdout);
       attr->attribute_value.length = SWAPBYTE16(attr->attribute_value.length); 
       /* Go to the next attribute in the list */
       attrOffset += sizeof(attr->attribute_id) +
                     sizeof(attr->attribute_value.length) +
                     attr->attribute_value.length;
      }
      /* Go to the next config object in the list */
      confObj->attributes.length = SWAPBYTE16( confObj->attributes.length);
      configOffset += sizeof(confObj->obj_class) + 
                      sizeof(confObj->obj_handle) + 
                      sizeof(confObj->attributes.count) + 
                      sizeof(confObj->attributes.length) + 
                      confObj->attributes.length;    
     }    
    
     configResult = ACCEPTED_CONFIG;
    
     /* Advance the manager state */
     phdc_device.phd_manager_state = PHD_MNG_OPERATING;
    }
    else
    {
     printf("\n\r11073 MNG Demo: Cannot allocate memory to store the configuration");
    }
   }
   else
   {
    /* Advance the manager state */
    phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
    printf("\n\r11073 APDU length (%d) exceeds allocated RX buffer length (%d)",apdu->length + sizeof(apdu->choice) + sizeof(apdu->length), APDU_MAX_BUFFER_SIZE);
   }
          
   printf("\n\r11073 MNG Demo: -------------------------------------------");
   if(configResult == ACCEPTED_CONFIG)
   {
     printf("\n\r11073 MNG Demo: Configuration Accepted.");
   }
   else
   {
     printf("\n\r11073 MNG Demo: Configuration NOT Accepted.");
   }
   
   /* Send back the configuration response */
 
   /* Update the predefined config response message fields */
   apdu = (APDU *)&PHD_MNG_CONFIG_RES[0];
   pPrst = (PRST_apdu *)&(apdu->u.prst);
   respDataApdu = (DATA_apdu *)&(pPrst->value[0]);
   
   respDataApdu->invoke_id = configInvokeID;
   reportResp = (ConfigReportRsp *)&(respDataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
   reportResp->config_report_id = SWAPBYTE16(configReportId);
   reportResp->config_result = configResult;
  
   /* Send data -> Configuration Response */
   phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
   if(phd_call_param_send != NULL)
   {   	
    phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
    phd_call_param_send->metadata = FALSE;
    phd_call_param_send->buff_ptr = &PHD_MNG_CONFIG_RES[0];
    phd_call_param_send->buff_size = CONFIG_RES_SIZE;    
    printf("\n\r11073 MNG Demo: Send back configuration response.");
    usb_class_phdc_send_data(phd_call_param_send);
    
    if(phdc_device.phd_manager_state == PHD_MNG_OPERATING)
    {      
     printf("\n\r11073 MNG Demo: The MANAGER is now in the OPERATING state. Ready to receive measurements.");
     /* Send data -> Get MDS Attributes */
     phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
     if(phd_call_param_send != NULL)
     {    	
      phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
      phd_call_param_send->metadata = FALSE;
      phd_call_param_send->buff_ptr = &PHD_MNG_GET_MDS_ATTR[0];
      phd_call_param_send->buff_size = GET_MDS_ATTR_SIZE;
 
      printf("\n\r11073 MNG Demo: Send ROIGET MDS Attributes(all) request.");
      usb_class_phdc_send_data(phd_call_param_send);     
     }
     else
     {
      printf("\n\r11073 MNG Demo: Cannot allocate memory for the Get-MDS request parameter");
     }    
    }
    else
    {
     /* configuration was not accepted. The manager is in the UNASSOCIATED state */
     printf("\n\r11073 MNG Demo: The MANAGER is now in the UNASSOCIATED state.");
     /* Send abort */
     phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
     if(phd_call_param_send != NULL)
     {    	
      phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
      phd_call_param_send->metadata = FALSE;
      phd_call_param_send->buff_ptr = &PHD_MNG_ABRT_REQ[0];
      phd_call_param_send->buff_size = ABRT_SIZE;
 
      printf("\n\r11073 MNG Demo: Send Abort message to the device.");
      usb_class_phdc_send_data(phd_call_param_send);     
     }
     else
     {
      printf("\n\r11073 MNG Demo: Cannot allocate memory for the Abort request parameter");
     }
    }
    
    
   }
   else
   {
    printf("\n\r11073 MNG Demo: Cannot allocate memory for the Configuration response parameter");
   }
   
 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_prst_mdc_noti_scanrep_fixed
* Returned Value : None
* Comments       :
*     This function handles the MDC_NOTI_SCAN_REPORT event types
*END*--------------------------------------------------------------------*/
void phdc_manager_prst_mdc_noti_scanrep_fixed(APDU *apdu, DATA_apdu *dataApdu)
{
 ScanReportInfoFixed *scan_rep;

 PRST_apdu *pPrst;    
 DATA_apdu *respDataApdu;
 
 USB_PHDC_PARAM *phd_call_param_send;
 
 printf("\n\r11073 MNG Demo: Received a Scan Report (Fixed) event.");
 printf("\n\r11073 MNG Demo: -------------------------------------------");
 
 scan_rep = (ScanReportInfoFixed *)&(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
 printf("\n\r11073 MNG Demo: Scan Report Num: %d  Num Observations: %d", scan_rep->scan_report_no, scan_rep->obs_scan_fixed.count); 

 phdc_manager_validate_print_fixedscan(&(scan_rep->obs_scan_fixed));
 
 /* Update the predefined scan response message fields */
 apdu = (APDU *)&PHD_MNG_SCAN_RES[0];
 pPrst = (PRST_apdu *)&(apdu->u.prst);
 respDataApdu = (DATA_apdu *)&(pPrst->value[0]);
 respDataApdu->invoke_id = dataApdu->invoke_id; 
 
 /* Send data -> Scan Response */
 phd_call_param_send = USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_PARAM));
 if(phd_call_param_send != NULL)
 { 	
  phd_call_param_send->ccs_ptr = (CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF;
  phd_call_param_send->metadata = FALSE;
  phd_call_param_send->buff_ptr = &PHD_MNG_SCAN_RES[0];
  phd_call_param_send->buff_size = SCAN_RES_SIZE;
 
  printf("\n\r11073 MNG Demo: -------------------------------------------");
  printf("\n\r11073 MNG Demo: Send back measurements confirmation.");
  usb_class_phdc_send_data(phd_call_param_send);
 }
 else
 {
  printf("\n\r11073 MNG Demo: Cannot allocate memory for the Scan response parameter");
 }
   
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_prst_get
* Returned Value : None
* Comments       :
*     This function handles the GET prst choice
*END*--------------------------------------------------------------------*/
void phdc_manager_prst_get(APDU *apdu, DATA_apdu *dataApdu)
{
 AttributeList *attribute_list = &(dataApdu->choice.u.rors_cmipGet.attribute_list);
 intu16 i,j, length, genOffset, attrOffset = 0;
 TypeVerList *type_ver_list;
 attribute_list->count = SWAPBYTE16(attribute_list->count);
 
 UNUSED(apdu)
   
 printf("\n\r11073 MNG Demo: Received a GET Response.");
 printf("\n\r11073 MNG Demo: -------------------------------------------");
 printf("\n\r11073 MNG Demo: Number of attributes = %d", attribute_list->count);
 
 for(i = 0; i<attribute_list->count; i++)
 {
  AVA_Type *attr = (AVA_Type *)((uint_8*)(&attribute_list->value[0]) + attrOffset);
  attr->attribute_value.length = SWAPBYTE16(attr->attribute_value.length);
  switch(attr->attribute_id)
  {
  	case MDC_ATTR_SYS_TYPE_SPEC_LIST: 
  	  			type_ver_list = (TypeVerList*)&attr->attribute_value.value[0];
  	  			type_ver_list->count = SWAPBYTE16(type_ver_list->count);
  	  			for(j = 0; j<type_ver_list->count; j++)
  	  			{
  	  			 int_8 *typeString = NULL;  	  			 
  	  			 TypeVer *type_ver = (TypeVer *)&type_ver_list->value[j];
  	  			 switch(type_ver->type)
  	  			 {
  	  			 	case MDC_DEV_SPEC_PROFILE_PULS_OXIM:   typeString = (int_8*)"Pulse Oximeter";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_BP:          typeString = (int_8*)"Blood Presure";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_TEMP:        typeString = (int_8*)"Thermometer";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_SCALE:       typeString = (int_8*)"Scale";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_GLUCOSE:     typeString = (int_8*)"Glucose Meter";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_HF_CARDIO:   typeString = (int_8*)"HF Cardio";break;
  	  			 	case MDC_DEV_SPEC_PROFILE_HF_STRENGTH: typeString = (int_8*)"HF Strength";break;
  	  			 	default: break;
  	  			 }
  	  			 if(typeString != NULL)
  	  			 {
  	  				type_ver->type = SWAPBYTE16(type_ver->type);
  	  				type_ver->version = SWAPBYTE16(type_ver->version);
  	  			  printf("\n\r11073 MNG Demo: Type = %d (%s)  Version = %d", type_ver->type, typeString, type_ver->version);
  	  			 }  	  			 
  	  			 else
  	  			 {  	  			 	
  	  			  printf("\n\r11073 MNG Demo: Type = %d  Version = %d", type_ver->type, type_ver->version);
  	  			 }
  	  			 
  	  			}
  	  			break;
  	case MDC_ATTR_ID_MODEL:
  	            length = attr->attribute_value.length;
  	            genOffset = 0;
  	            printf("\n\r11073 MNG Demo: Model: ");
  	            
  	            while(length - genOffset)
  	            {
  	             Any *str = (Any *)((uint_8*)&attr->attribute_value.value[0] + genOffset);
				 str->length = SWAPBYTE16(str->length);  	               	             
  	             for(j=0;j<str->length;j++)
  	             {
  	                char tempString[2];
  	                tempString[0] = (char)str->value[j];
  	                tempString[1] = '\0';
  	                printf("%s",tempString);
  	             }
  	             
  	             printf("  ");
  	             
  	             genOffset += sizeof(str->length) +
  	                          str->length;;
  	            }
  	            break;
  	            
  	
  }
         /* Go to the next attribute in the list */
       attrOffset += sizeof(attr->attribute_id) +
                     sizeof(attr->attribute_value.length) +
                     attr->attribute_value.length; 
 }
	
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_validate_print_scan
* Returned Value : None
* Comments       :
*     This function validates the received measurements 
*     based on the existing device configuration
*END*--------------------------------------------------------------------*/
void phdc_manager_validate_print_fixedscan(ObservationScanFixedList *scanList)
{
 ObservationScanFixed *observation; 
 intu16 i, obsListOffset = 0;
 ConfigObject *confObj;
 
 scanList->count = SWAPBYTE16(scanList->count);
 for(i = 0; i < scanList->count; i++)
 {
  observation = (ObservationScanFixed *)((intu8*)&(scanList->value[0]) + obsListOffset);
  observation->obj_handle = SWAPBYTE16(observation->obj_handle);
  confObj = phdc_manager_get_configobj_byhandle(phdc_device.confObjList, observation->obj_handle);
  
  if(confObj != NULL)
  {
   
   if(confObj->obj_class == MDC_MOC_VMO_METRIC_NU)
   {    
    phdc_manager_handle_metric_nu_fixedscan(confObj, observation);  
   }
   else
   {
    printf("\n\r11073 MNG Demo: Usupported object class to print the observation scan : %d", confObj->obj_class);  
   }
   
   
   
  }
  else
  {
   printf("\n\r11073 MNG Demo: Unknown object handle for observation scan : %d", observation->obj_handle); 
  }
  	
  
  observation->obs_val_data.length = SWAPBYTE16(observation->obs_val_data.length);
  obsListOffset += sizeof(observation->obj_handle) + 
                   sizeof(observation->obs_val_data.length) +
                   observation->obs_val_data.length;
 }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_handle_metric_nu_fixedscan
* Returned Value : None
* Comments       :
*     This function handles the observation data for the METRIC_NU class object
*END*--------------------------------------------------------------------*/
void    phdc_manager_handle_metric_nu_fixedscan(ConfigObject *confObj, ObservationScanFixed* observation)
{
  AVA_Type *attr_val_map, *gen_attr, *unit_code_attr;
  AttrValMap *val_map;
  intu16 i, attrOffset = 0, obs_offset = 0, count, attribute_len;  
  
  /* Search for the MDC_ATTR_ATTRIBUTE_VAL_MAP attribute in the object configuration
   * to see what type of measurements are we expecting. If this attribute does not exists 
   * in the configuration  object, then the measurements cannot be decoded.
   */
  attr_val_map = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_ATTRIBUTE_VAL_MAP);
  if(attr_val_map != NULL)
  {      
   /* Print the Type of the object. Get the MDC_ATTR_ID_TYPE attribute */
   gen_attr = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_ID_TYPE);
   if(gen_attr != NULL)
   {
     /* Decode the attribute value - TYPE */
     TYPE *type = (TYPE *)&(gen_attr->attribute_value.value[0]);
     
     printf("\n\r11073 MNG Demo: Observation for object type: ");
     phdc_manager_print_nomenclature(type->code);
     printf(". Partition: ");
     phdc_manager_print_partition(type->partition);
   }
   
   /* Search for the measurement unit MDC_ATTR_UNIT_CODE */
   unit_code_attr = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_UNIT_CODE);
   
   /* Decode the Value map to see what attributes are getting values from the observation scan */
   val_map = (AttrValMap *)&(attr_val_map->attribute_value.value[0]);
   count = SWAPBYTE16(val_map->count);
   
   for(i = 0; i < count; i++)
   {
     AttrValMapEntry *attr = (AttrValMapEntry *)((uint_8*)(&val_map->value[0]) + attrOffset);  
     
     switch(attr->attribute_id)
     {
      case MDC_ATTR_NU_VAL_OBS_SIMP:
          /* SimpleNuObsValue -> Print the observation based on the FLOAT type */
          printf("\n\r11073 MNG Demo: Observation Value = ");
          phdc_manager_print_float_val(&observation->obs_val_data.value[obs_offset]);
          /* print also the unit code */
          if(unit_code_attr != NULL)
          {
           printf(" ");
           phdc_manager_print_nomenclature(*(OID_Type*)&(unit_code_attr->attribute_value.value[0]));
          }
          break;
      case MDC_ATTR_NU_VAL_OBS_BASIC:
          /* BasicNuObsValue -> Print the observation based on the SFLOAT type */
          printf("\n\r11073 MNG Demo: Observation Value = ");
          phdc_manager_print_sfloat_val(&observation->obs_val_data.value[obs_offset]);
          /* print also the unit code */
          if(unit_code_attr != NULL)
          {
           printf(" ");
           phdc_manager_print_nomenclature(*(OID_Type*)&(unit_code_attr->attribute_value.value[0]));
          }
          break;
      case MDC_ATTR_TIME_STAMP_ABS:
          /* AbsoluteTime */
          printf("\n\r11073 MNG Demo: Absolute Timestamp = ");
          phdc_manager_print_abs_timestamp(&observation->obs_val_data.value[obs_offset]);
          break;
       
     }
     attribute_len = SWAPBYTE16(attr->attribute_len);
     
     obs_offset += attribute_len;
     
     /* Go to the next attribute in the list */
     attrOffset += sizeof(attr->attribute_id) +
                   sizeof(attr->attribute_len);
   }
  }
  else
  {
   printf("\n\r11073 MNG Demo: Missing MDC_ATTR_ATTRIBUTE_VAL_MAP attribute to print the fixed observation"); 
  }
  
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_print_float_val
* Returned Value : None
* Comments       :
*     This function prints an ascii representation of a Floating point number
*      according to the medical device numeric format.
*END*--------------------------------------------------------------------*/
void    phdc_manager_print_float_val(intu8 *value)
{
  /* The FLOAT-Type has 32-bits representation */
  /* The resulted number can be calculated with the following formula: 
   *   F = mantisa * 10^exponent
   */  
  uint_32 rawVal = SWAPBYTE32(*(intu32*)value);  
  int_32 mantisa = (((int_32)(rawVal & 0xFFFFFF) << 8)) >> 8; /* Shifting left and right to propagate the sign bit */
  int_8 exponent = (int_8)((int_32)(rawVal & 0xFF000000) >> 24); /* int32 cast to ensure the sign bit is propagated */
  int_32 wInt;
  uint_16 dInt;
  
  if(exponent < 0)
  {   
   uint_8 div =  (uint_8)(1 +  ((uint_8)0xFF - (uint_8)exponent)); /* exponent in absolute value */   
   wInt = (int_32)((int_32)mantisa / (10*div));
   dInt = (uint_16)(mantisa - (wInt * div));   
  }
  else
  {
   wInt = mantisa * 10 ^ exponent;
   dInt = 0; 
  }
    
  printf("%d.%d", wInt, dInt);

}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_print_sfloat_val
* Returned Value : None
* Comments       :
*     This function prints an ascii representation of a S-Floating point number
*      according to the medical device numeric format.
*END*--------------------------------------------------------------------*/
void    phdc_manager_print_sfloat_val(intu8 *value)
{
  /* The SFLOAT-Type has 16-bits representation */
  /* The resulted number can be calculated with the following formula: 
   *   F = mantisa * 10^exponent
   */  
  uint_16 rawVal = BYTESWAP16(*(intu16*)value);  
  int_16 mantisa = (int_16)((((int_16)(rawVal & 0xFFF) << 4)) >> 4); /* Shifting left and right to propagate the sign bit */
  int_8 exponent = (int_8)((int_16)(rawVal & 0xF000) >> 12); /* int cast to ensure the sign bit is propagated */ 
  int_16 wInt;
  uint_16 dInt;
  
  if(exponent < 0)
  {   
   uint_8 div =  (uint_8)(1 +  ((uint_8)0xFF - (uint_8)exponent)); /* exponent in absolute value */   
   wInt = (int_16)((int_16)mantisa / (10*div));
   dInt = (uint_16)(mantisa - (wInt * div));   
  }
  else
  {
   wInt = (int_16)(mantisa * 10 ^ exponent);
   dInt = 0; 
  }
    
  printf("%d.%d", wInt, dInt);

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_print_abs_timestamp
* Returned Value : None
* Comments       :
*     This function prints an ascii representation of the Absolute Time stamp
*      according to the medical device numeric format.
*END*--------------------------------------------------------------------*/
void    phdc_manager_print_abs_timestamp(intu8 *value)
{
 printf("%02x%02x-%02x-%02x  %02x:%02x", value[0], value[1], value[2], value[3], value[4], value[5]);	 
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_print_nomenclature
* Returned Value : None
* Comments       :
*     This function prints the nomenclature ASCII string based on the OID_Type
*END*--------------------------------------------------------------------*/
boolean phdc_manager_print_nomenclature(OID_Type type)
{
 intu16 i;
 for(i = 0; i < NomAsciiCount; i++)
 {
  if((NomenclatureAsciiTable[i].type == type) && (NomenclatureAsciiTable[i].asciiString != NULL))
  {
   printf("%s", (uint_8*)NomenclatureAsciiTable[i].asciiString);    
   return TRUE;
  }
 }
 
 return FALSE;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_print_partition
* Returned Value : None
* Comments       :
*     This function prints the partition ASCII string based on the OID_Type
*END*--------------------------------------------------------------------*/
boolean phdc_manager_print_partition(NomPartition partition)
{
 intu16 i;
 for(i = 0; i < PartitionAsciiCount; i++)
 {
  if((PartitionAsciiTable[i].partition == partition) && (PartitionAsciiTable[i].asciiString != NULL))
  {
   printf("%s", (uint_8*)PartitionAsciiTable[i].asciiString);
   return TRUE;
  }
 }
 
 return FALSE;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_get_configobj_byhandle
* Returned Value : None
* Comments       :
*     This function returns a configuration object from the provided ConfigObjectList
*     using the requested obj_handle.
*     If the obj_handle is not found in the list, this function returns NULL;
*END*--------------------------------------------------------------------*/
ConfigObject* phdc_manager_get_configobj_byhandle(ConfigObjectList *confObjList, HANDLE obj_handle)
{
 ConfigObject *confObjRet = NULL;
 intu16 i, configOffset = 0, length, obj_handle_tmp;
 
 if(confObjList != NULL)
 {
  for(i = 0; i<confObjList->count; i++)
  {
    ConfigObject *confObj = (ConfigObject *)(((uint_8*)&confObjList->value[0]) + configOffset);
    obj_handle_tmp = SWAPBYTE16(confObj->obj_handle);
    if(obj_handle_tmp == obj_handle)
    {
     confObjRet = confObj;
     break;
    }
    length = SWAPBYTE16(confObj->attributes.length);
    /* Go to the next config object in the list */
    configOffset += sizeof(confObj->obj_class) + 
                    sizeof(confObj->obj_handle) + 
                    sizeof(confObj->attributes.count) + 
                    sizeof(confObj->attributes.length) + 
                    length;    
  }
 }
 
 return confObjRet;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_get_attribute_byid
* Returned Value : None
* Comments       :
*     This function returns the searched attribute from the configuration object
*     If the attribute_id is not found in the config object, this function returns NULL;
*END*--------------------------------------------------------------------*/
AVA_Type*     phdc_manager_get_attribute_byid(ConfigObject *confObj, OID_Type attribute_id)
{
  AVA_Type *attrObjRet = NULL;
  
  intu16 i, attrOffset = 0, length, count;
  
  count = SWAPBYTE16(confObj->attributes.count);
 
  for(i = 0; i < count; i++)
  {
    AVA_Type *attr = (AVA_Type *)((uint_8*)(&confObj->attributes.value[0]) + attrOffset);  
    
    if(attr->attribute_id == attribute_id)
    {
     attrObjRet = attr;
     break; 
    }
    
    length = SWAPBYTE16(attr->attribute_value.length);
    /* Go to the next attribute in the list */
    attrOffset += sizeof(attr->attribute_id) +
                  sizeof(attr->attribute_value.length) +
                  length;
  }
  
  return attrObjRet; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_phdc_manager_event
* Returned Value : None
* Comments       :
*     Called when PHDC device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/

void usb_host_phdc_manager_event(
    /* [IN] pointer to device instance */
    _usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    _usb_interface_descriptor_handle intf_handle,
    /* [IN] code number for event causing callback */
    uint_32 event_code)
{
    INTERFACE_DESCRIPTOR_PTR intf_ptr = (INTERFACE_DESCRIPTOR_PTR) intf_handle;

    fflush(stdout);
    switch (event_code) 
    {
		case USB_ATTACH_EVENT:
			printf("\n\r----- Attach Event -----");
			/* Drop through config event for the same processing */
		case USB_CONFIG_EVENT:
        printf("\n\rState = %d", (int)phdc_device.DEV_STATE);
			printf("\n\r  Class = %d", intf_ptr->bInterfaceClass);
			printf("\n\r  SubClass = %d", intf_ptr->bInterfaceSubClass);
			printf("\n\r  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
			fflush(stdout);
	
			if(phdc_device.DEV_STATE == USB_DEVICE_IDLE) 
			{
				phdc_device.DEV_HANDLE = dev_handle;
				phdc_device.INTF_HANDLE = intf_handle;
				phdc_device.DEV_STATE = USB_DEVICE_ATTACHED;
			}
			else 
			{
				printf("\n\rPHDC device already attached");
				fflush(stdout);
			}
			break;
	
		case USB_INTF_EVENT:
			printf("\n\r----- Interfaced Event -----");
			fflush(stdout);
	
			if(phdc_manager_initialize_device())
			{
			   phdc_device.DEV_STATE = USB_DEVICE_INTERFACED;
			}
			break;
	
		case USB_DETACH_EVENT:
			/* Use only the interface with desired protocol */
			printf("\n\r----- Detach Event -----");
        printf("\n\rState = %d", (int)phdc_device.DEV_STATE);
			printf("\n\r  Class = %d", intf_ptr->bInterfaceClass);
			printf("\n\r  SubClass = %d", intf_ptr->bInterfaceSubClass);
			printf("\n\r  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
			fflush(stdout);
			
			usb_class_phdc_uninit(dev_handle, intf_handle);
			
			phdc_device.DEV_HANDLE = NULL;
			phdc_device.INTF_HANDLE = NULL;
			phdc_device.DEV_STATE = USB_DEVICE_DETACHED;
			break;
    }

    /* notify application that status has changed */
    fw_event_set(&USB_Event, USB_EVENT_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : phdc_manager_initialize_device
* Returned Value : None
* Comments       :
*     Called after a PHDC device was interfaced. Fills the application-specific
*     fields from the phdc_device structure, and sets the PHDC callbacks.
*END*--------------------------------------------------------------------*/
boolean phdc_manager_initialize_device(void)
{
 USB_STATUS status;
 
 status = usb_class_phdc_set_callbacks((CLASS_CALL_STRUCT*)&phdc_device.CLASS_INTF, 
                                     		   usb_host_phdc_send_callback,
                                     		   usb_host_phdc_recv_callback,
                                     		   usb_host_phdc_ctrl_callback
                                             );
 printf("\n\rConfigure PHDC callbacks and initializing attached device: ... ");
 if(status == USB_OK)
 {
   phdc_device.phd_recv_in_progress = FALSE;
   phdc_device.phd_buffer = (uint_8*)USB_mem_alloc_uncached_zero(APDU_MAX_BUFFER_SIZE);
   phdc_device.phd_recv_size = 0; /* Expecting an association request */
   phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
   
   if(phdc_device.phd_buffer != NULL)
   {   	
    printf(" DONE");
    return TRUE;
   }
   else
   {
    printf(" ERROR allocating memory");
    return FALSE;
   }
 }
 else
 {
   printf(" ERROR (returned code = %d)", status);
   return FALSE;
 }

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_phdc_send_callback
* Returned Value : None
* Comments       :
*     Called when PHDC device has completed a send transfer.
*END*--------------------------------------------------------------------*/
void usb_host_phdc_send_callback(USB_PHDC_PARAM *call_param)
{
  /* Free the memory allocated for call_param */
  USB_mem_free(call_param);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_phdc_recv_callback
* Returned Value : None
* Comments       :
*     Called when PHDC device has completed a receive transfer.
*END*--------------------------------------------------------------------*/

void usb_host_phdc_recv_callback(USB_PHDC_PARAM *call_param)
{
  phdc_device.phd_recv_in_progress = FALSE;
  phdc_device.phd_recv_size = (uint_8)call_param->buff_size;
 
  /* notify the application */
  fw_event_set(&USB_Event, USB_EVENT_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_phdc_recv_callback
* Returned Value : None
* Comments       :
*     Called when PHDC device has completed a control transfer.
*END*--------------------------------------------------------------------*/
void usb_host_phdc_ctrl_callback(USB_PHDC_PARAM *call_param)
{
 /* Free the memory allocated for call_param */
 USB_mem_free(call_param);
}

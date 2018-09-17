/**HEADER********************************************************************
 * 
 * Copyright (c) 2008 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: ehci_main.c$
 * $Version : 3.5.23.0$
 * $Date    : Oct-22-2009$
 *
 * Comments:
 *
 *   This file contains the main low-level Host API functions specific to 
 *   the VUSB chip.
 *
 *END************************************************************************/
#include "hostapi.h"
#include "ehci_usbprv_host.h"
#include "ehci_main.h"
#include "ehci_host.h"
#include "usbevent.h"
#include "user_config.h"

#if HIGH_SPEED_DEVICE

USB_EVENT_STRUCT ehci_event;
uint_32 CACHE_FIX = 1;
pointer total_mem_ptr=NULL;

#ifdef __USB_OTG__
#include "otgapi.h"
extern USB_OTG_STATE_STRUCT_PTR usb_otg_state_struct_ptr;
#endif
extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;
void USBHS_ISR_HOST();
void EHCI_Process_ISR(void);

/**************************************************************************
 Local Functions. Note intended to be called from outside this file 
 **************************************************************************/

static void _usb_ehci_process_qh_list_tr_complete(
		_usb_host_handle                 handle,
		ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
);

static void _usb_ehci_process_qh_interrupt_tr_complete(
		_usb_host_handle                 handle,
		ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
);

static void _usb_ehci_process_itd_tr_complete
(
		_usb_host_handle                 handle
);

static void _usb_ehci_process_sitd_tr_complete
(
		_usb_host_handle                 handle
);

const USB_HOST_CALLBACK_FUNCTIONS_STRUCT _bsp_usb_host_callback_table = 
{
		//   0,
		/* The Host/Device init function */
		_usb_hci_vusb20_init,

		/* The function to shutdown the host/device */
		_usb_hci_vusb20_shutdown,

		/* The function to send data */
		_usb_hci_vusb20_send_data,

		/* The function to send setup data */
		_usb_hci_vusb20_send_setup,

		/* The function to receive data */
		_usb_hci_vusb20_recv_data,

		/* The function to get the transfer status */
		//   NULL,

		/* The function to cancel the transfer */
		_usb_hci_vusb20_cancel_transfer,

		/* The function to do USB sleep */
		//   NULL,

		/* The function for USB bus control */
		_usb_hci_vusb20_bus_control,

		_usb_ehci_allocate_bandwidth,

		_usb_ehci_free_resources,

		_usb_ehci_get_frame_number,

		_usb_ehci_get_micro_frame_number,

		/* The function to open a pipe */
		_usb_ehci_open_pipe,

		/* The function to update the maximum packet size */
		NULL,

		/* The function to update the device address */
		_usb_ehci_update_dev_address
};





/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_send_data
 *  Returned Value : None
 *  Comments       :
 *        Send the data
 *END*-----------------------------------------------------------------*/
USB_STATUS  _usb_hci_vusb20_send_data
(
		/* [IN] the USB Host state structure */
		_usb_host_handle              handle,

		/* [IN] The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_STATUS status;

	status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
	return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_send_setup
 *  Returned Value : USB_STATUS
 *  Comments       :
 *        Send Setup packet
 *END*-----------------------------------------------------------------*/
USB_STATUS _usb_hci_vusb20_send_setup
(
		/* [IN] the USB Host state structure */
		_usb_host_handle              handle,

		/* [IN] The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_STATUS status;
	status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
	return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_recv_data
 *  Returned Value : None
 *  Comments       :
 *        Receive data
 *END*-----------------------------------------------------------------*/

USB_STATUS _usb_hci_vusb20_recv_data
(
		/* [IN] the USB Host state structure */
		_usb_host_handle              handle,

		/* [IN] The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_STATUS status;

	status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);

	return status;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_ehci_queue_pkts
 *  Returned Value : status of the transfer queuing.
 *  Comments       :
 *        Queue the packet in the hardware
 *END*-----------------------------------------------------------------*/

USB_STATUS  _usb_hci_ehci_queue_pkts
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_STATUS  status;

	/* Queue the packets */
	switch (pipe_descr_ptr->PIPETYPE) {
	case USB_ISOCHRONOUS_PIPE:
		status = _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list(handle, 
				pipe_descr_ptr, current_pipe_tr_struct_ptr);
		break;
	case USB_INTERRUPT_PIPE:
		status = _usb_ehci_add_interrupt_xfer_to_periodic_list(handle, 
				pipe_descr_ptr, current_pipe_tr_struct_ptr);
		break;
	case USB_CONTROL_PIPE:
	case USB_BULK_PIPE:
		/* Queue it on the asynchronous schedule list */
		status = _usb_ehci_add_xfer_to_asynch_schedule_list(handle, pipe_descr_ptr, 
				current_pipe_tr_struct_ptr);
		break;
	default:
		status = USB_STATUS_ERROR;
		break;
	} /* EndSwitch */

	return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_init_Q_HEAD
 *  Returned Value : None
 *  Comments       :
 *        Initialize the Queue Head. This routine initializes a queue head 
 *  for interrupt/Bulk/Control transfers.
 *END*-----------------------------------------------------------------*/
void _usb_ehci_init_Q_HEAD
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

		/* [IN] the queue head to initialize */
		EHCI_QH_STRUCT_PTR               QH_ptr,

		/* [IN] the previous queue head to link to */
		EHCI_QH_STRUCT_PTR               prev_QH_ptr,

		/* [IN] the first QTD to queue for this Queue head */
		EHCI_QTD_STRUCT_PTR              first_QTD_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR     usb_host_ptr;
	uint_32                       control_ep_flag = 0, split_completion_mask = 0;
	uint_32                       interrupt_schedule_mask = 0; 
	uint_32                       data_toggle_control = 0, item_type = 0;
	uint_32                       time_for_xaction = 0, H_bit = 0;
	uint_8                        mult = 1, period = 0;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/******************************************************************************
 PREPARE THE BASIC FIELDS OF A QUEUE HEAD DEPENDING UPON THE PIPE TYPE
	 *******************************************************************************/
#if PSP_HAS_DATA_CACHE    // TODO change to memzero and flush
	/********************************************************
   USB Memzero does not bypass the cache and hence explicit
   setting to zero is required in software
	 ********************************************************/
	QH_ptr->HORIZ_LINK_PTR    =0;
	QH_ptr->EP_CAPAB_CHARAC1  =0;
	QH_ptr->EP_CAPAB_CHARAC2  =0;
	QH_ptr->CURR_QTD_LINK_PTR =0;
	QH_ptr->NEXT_QTD_LINK_PTR =0;
	QH_ptr->ALT_NEXT_QTD_LINK_PTR =0;
	QH_ptr->STATUS       =0;
	QH_ptr->BUFFER_PTR_0 =0;
	QH_ptr->BUFFER_PTR_1 =0;
	QH_ptr->BUFFER_PTR_2 =0;
	QH_ptr->BUFFER_PTR_3 =0;
	QH_ptr->BUFFER_PTR_4 =0;
#else
	USB_mem_zero((void *)QH_ptr, 12*sizeof(uint_32));
#endif // PSP_HAS_DATA_CACHE

	switch (pipe_descr_ptr->PIPETYPE) 
	{
	case  USB_CONTROL_PIPE:
		if (pipe_descr_ptr->SPEED != USB_SPEED_HIGH) 
		{
			control_ep_flag = 1;
		}
		item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
		H_bit = (uint_32) ((prev_QH_ptr == QH_ptr)? 1 : 0);
		data_toggle_control = 1;
		break;

	case  USB_INTERRUPT_PIPE:
		data_toggle_control = 0;
		break;

	case  USB_BULK_PIPE:
		item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
		H_bit = (uint_32) ((prev_QH_ptr == QH_ptr)? 1 : 0);
		data_toggle_control = 1;
		break;

	}  

	if (pipe_descr_ptr->SPEED == USB_SPEED_HIGH) 
	{
		mult = pipe_descr_ptr->TRS_PER_UFRAME;
	}  

	/******************************************************************************
 PREPARE THE Fields that are same for all pipes.
	 *******************************************************************************/

	QH_ptr->PIPE_DESCR_FOR_THIS_QH = (pointer)pipe_descr_ptr;
	pipe_descr_ptr->QH_FOR_THIS_PIPE = (pointer)QH_ptr;

	QH_ptr->CURR_QTD_LINK_PTR = 0;
	QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
	QH_ptr->STATUS = 0;
	QH_ptr->BUFFER_PTR_0 = 0;
	QH_ptr->BUFFER_PTR_1 = 0;
	QH_ptr->BUFFER_PTR_2 = 0;
	QH_ptr->BUFFER_PTR_3 = 0;
	QH_ptr->BUFFER_PTR_4 = 0;


	/****************************************************************************
 copy the fields inside the QH
	 *****************************************************************************/


	/* Initialize the endpoint capabilities registers */   
	QH_ptr->EP_CAPAB_CHARAC1 = (
			((uint_32)pipe_descr_ptr->NAK_COUNT << EHCI_QH_NAK_COUNT_RL_BITS_POS) | 
			(control_ep_flag << EHCI_QH_EP_CTRL_FLAG_BIT_POS) | 
			(pipe_descr_ptr->MAX_PACKET_SIZE << EHCI_QH_MAX_PKT_SIZE_BITS_POS) |
			(H_bit << EHCI_QH_HEAD_RECLAMATION_BIT_POS) |
			(data_toggle_control << EHCI_QH_DTC_BIT_POS) | 
			(pipe_descr_ptr->SPEED << EHCI_QH_SPEED_BITS_POS) |
			((uint_32)pipe_descr_ptr->ENDPOINT_NUMBER << EHCI_QH_EP_NUM_BITS_POS) | 
			pipe_descr_ptr->DEVICE_ADDRESS);

	QH_ptr->EP_CAPAB_CHARAC2 = (
			(mult << EHCI_QH_HIGH_BW_MULT_BIT_POS) |
			(pipe_descr_ptr->HUB_PORT_NUM << EHCI_QH_HUB_PORT_NUM_BITS_POS) | 
			(pipe_descr_ptr->HUB_DEVICE_ADDR << EHCI_QH_HUB_ADDR_BITS_POS) |
			(split_completion_mask << EHCI_QH_SPLIT_COMPLETION_MASK_BITS_POS) |
			interrupt_schedule_mask);

	/****************************************************************************
 Queue the transfers now by updating the pointers
	 *****************************************************************************/      

	switch (pipe_descr_ptr->PIPETYPE) 
	{
	case  USB_CONTROL_PIPE:
		/* Link the new Queue Head to the previous Queue Head */
		QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
		QH_ptr->HORIZ_LINK_PTR = (uint_32)prev_QH_ptr->HORIZ_LINK_PTR;
		prev_QH_ptr->HORIZ_LINK_PTR = 
				((uint_32)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS));
		break;
	case  USB_BULK_PIPE:
		/* Link the new Queue Head to the previous Queue Head */
		QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
		QH_ptr->HORIZ_LINK_PTR = (uint_32)prev_QH_ptr->HORIZ_LINK_PTR;
		prev_QH_ptr->HORIZ_LINK_PTR = 
				((uint_32)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS));
		break;
	case  USB_INTERRUPT_PIPE:
		QH_ptr->NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
		QH_ptr->HORIZ_LINK_PTR = EHCI_QUEUE_HEAD_POINTER_T_BIT;
		break;
	}

	return;

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_init_QTD
 *  Returned Value : None
 *  Comments       :
 *        Initialize the QTD
 *END*-----------------------------------------------------------------*/
void _usb_ehci_init_QTD
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* [IN] the address of the QTD to initialize */
		EHCI_QTD_STRUCT_PTR              QTD_ptr,


		/* The buffer start address for the QTD */            
		uchar_ptr                        buffer_start_address,

		/* The token value */
		uint_32                          token
)
{ /* Body */

#if PSP_HAS_DATA_CACHE    // TODO change to memzore and flush
	/********************************************************
      USB Memzero does not bypass the cache and hence explicit
      setting to zero is required in software
	 ********************************************************/
	QTD_ptr->NEXT_QTD_PTR        =   0;
	QTD_ptr->ALT_NEXT_QTD_PTR    =   0;
	QTD_ptr->TOKEN               =   0;
	QTD_ptr->BUFFER_PTR_0        =   0;
	QTD_ptr->BUFFER_PTR_1        =   0;
	QTD_ptr->BUFFER_PTR_2        =   0;
	QTD_ptr->BUFFER_PTR_3        =   0;
	QTD_ptr->BUFFER_PTR_4        =   0;
#else
	/* Zero the QTD. Leave the scratch pointer */
	USB_mem_zero((void *)QTD_ptr, 8*sizeof(uint_32));
#endif // PSP_HAS_DATA_CACHE

	/* Initialize the QTD */
	QTD_ptr->SCRATCH_PTR = handle;

	/* Set the Terminate bit */
	QTD_ptr->NEXT_QTD_PTR = EHCI_QTD_T_BIT;

	/* Set the Terminate bit */
	QTD_ptr->ALT_NEXT_QTD_PTR = EHCI_QTD_T_BIT;

	QTD_ptr->BUFFER_PTR_0 = (uint_32)buffer_start_address;

	/* 4K apart buffer page pointers */
	QTD_ptr->BUFFER_PTR_1 = (QTD_ptr->BUFFER_PTR_0 + 4096);
	QTD_ptr->BUFFER_PTR_2 = (QTD_ptr->BUFFER_PTR_1 + 4096);
	QTD_ptr->BUFFER_PTR_3 = (QTD_ptr->BUFFER_PTR_2 + 4096);
	QTD_ptr->BUFFER_PTR_4 = (QTD_ptr->BUFFER_PTR_3 + 4096);

	QTD_ptr->TOKEN = token;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_init_Q_element
 *  Returned Value : None
 *  Comments       :
 *        Initialize the Queue Element descriptor(s)
 *END*-----------------------------------------------------------------*/
uint_32 _usb_ehci_init_Q_element
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR        usb_host_ptr;
	EHCI_QTD_STRUCT_PTR              QTD_ptr, prev_QTD_ptr, first_QTD_ptr;
	uint_32                          pid_code, data_toggle, token, num_of_packets;
	uint_32                          total_length, qtd_length;
	uchar_ptr                        buff_ptr;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/* Get a QTD from the queue */   
	EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

	if (!QTD_ptr) {
		return USB_STATUS_TRANSFER_IN_PROGRESS;
	} /* Endif */

	usb_host_ptr->QTD_ENTRIES--;

	/* The data 0/1 for this pipe */
	data_toggle = (uint_32)((pipe_descr_ptr->NEXTDATA01 << 
			EHCI_QTD_DATA_TOGGLE_BIT_POS));

	/* The very first QTD */   
	prev_QTD_ptr = first_QTD_ptr = QTD_ptr;

	if (pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE) {
		pid_code = EHCI_QTD_PID_SETUP;
		/* Setup packet is always 8 bytes */
		token = (((uint_32)8 << EHCI_QTD_LENGTH_BIT_POS) |
				data_toggle | pid_code | EHCI_QTD_STATUS_ACTIVE);

		/* Initialize a QTD for Setup phase */   
		_usb_ehci_init_QTD(handle, QTD_ptr, 
				(uchar_ptr)&current_pipe_tr_struct_ptr->setup_packet,
				token);
		QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
		QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

		/* Get a QTD from the queue for the data phase or a status phase */
		EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

		if (!QTD_ptr) {
			return USB_STATUS_TRANSFER_IN_PROGRESS;
		} /* Endif */

		usb_host_ptr->QTD_ENTRIES--;

		/* The data phase QTD chained to the Setup QTD */      
		prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
		data_toggle = EHCI_QTD_DATA_TOGGLE;

		if (current_pipe_tr_struct_ptr->SEND_PHASE) {
			/* Host to Device data phase */
			if (current_pipe_tr_struct_ptr->TX_LENGTH) {
				/* Anything to send ? */
				pid_code = EHCI_QTD_PID_OUT;
				token = ((current_pipe_tr_struct_ptr->\
						TX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) | data_toggle | 
						pid_code | EHCI_QTD_STATUS_ACTIVE);

				/* Initialize the QTD for OUT data phase */            
				_usb_ehci_init_QTD(handle, QTD_ptr, 
						current_pipe_tr_struct_ptr->\
						TX_BUFFER, token);

				QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
				QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

				/* Use the QTD to chain the next QTD */         
				prev_QTD_ptr = QTD_ptr;

				/* Get a QTD from the queue for status phase */   
				EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

				if (!QTD_ptr) {
					return USB_STATUS_TRANSFER_IN_PROGRESS;
				} /* Endif */

				usb_host_ptr->QTD_ENTRIES--;

				/* Chain the status phase QTD to the data phase QTD */         
				prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
			} /* Endif */
			/* Zero length IN */
			pid_code = EHCI_QTD_PID_IN;
		} else {

			if (current_pipe_tr_struct_ptr->RX_LENGTH) {
				pid_code = EHCI_QTD_PID_IN;
				token = ((current_pipe_tr_struct_ptr->\
						RX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) | data_toggle | 
						pid_code | EHCI_QTD_STATUS_ACTIVE);

				/* Initialize the QTD for the IN data phase */         
				_usb_ehci_init_QTD(handle, QTD_ptr, 
						current_pipe_tr_struct_ptr->RX_BUFFER, token);
				QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
				QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

				/* Use thie QTD to chain the next QTD */         
				prev_QTD_ptr = QTD_ptr;

				/* Get a QTD from the queue for status phase */   
				EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

				if (!QTD_ptr) {
					return USB_STATUS_TRANSFER_IN_PROGRESS;
				} /* Endif */

				usb_host_ptr->QTD_ENTRIES--;

				/* Chain the status phase QTD to the data phase QTD */         
				prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
			} /* Endif */
			/* Zero-length OUT token */
			pid_code = EHCI_QTD_PID_OUT;
		} /* Endif */

		/* Already chained so add a qTD for status phase */
		/* Initialize the QTD for the status phase -- Zero length opposite 
		 ** direction packet 
		 */
		token = ((0 << EHCI_QTD_LENGTH_BIT_POS) |
				data_toggle | EHCI_QTD_IOC | pid_code | EHCI_QTD_STATUS_ACTIVE);
		_usb_ehci_init_QTD(handle, QTD_ptr, NULL, token);
		QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
		QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;
	} else {

		/* Save the next data toggle based on the number of packets 
		 ** for this transfer 
		 */
		if (pipe_descr_ptr->DIRECTION) {
			total_length = current_pipe_tr_struct_ptr->TX_LENGTH;
			buff_ptr = current_pipe_tr_struct_ptr->TX_BUFFER;
			pid_code = EHCI_QTD_PID_OUT;
		} else {
			total_length = current_pipe_tr_struct_ptr->RX_LENGTH;
			buff_ptr = current_pipe_tr_struct_ptr->RX_BUFFER;
			pid_code = EHCI_QTD_PID_IN;
		} /* Endif */

		do {

			/* The data 0/1 for this pipe */
			data_toggle = (uint_32)((pipe_descr_ptr->NEXTDATA01 << 
					EHCI_QTD_DATA_TOGGLE_BIT_POS));

			if (total_length > VUSB_EP_MAX_LENGTH_TRANSFER) {
				token = ((VUSB_EP_MAX_LENGTH_TRANSFER  << 
						EHCI_QTD_LENGTH_BIT_POS) | data_toggle | 
						pid_code | EHCI_QTD_DEFAULT_CERR_VALUE |
						EHCI_QTD_STATUS_ACTIVE);
				total_length -= VUSB_EP_MAX_LENGTH_TRANSFER;
				qtd_length = VUSB_EP_MAX_LENGTH_TRANSFER;
			} else {
				token = ((total_length << EHCI_QTD_LENGTH_BIT_POS) | 
						data_toggle | EHCI_QTD_IOC | pid_code | 
						EHCI_QTD_DEFAULT_CERR_VALUE | EHCI_QTD_STATUS_ACTIVE);
				qtd_length = total_length;
				total_length = 0;
			} /* Endif */

			if (qtd_length < pipe_descr_ptr->MAX_PACKET_SIZE) {
				num_of_packets = 1;
			} else {
				/* Deliberately using a divide */
				//num_of_packets = 
				//   ((qtd_length / pipe_descr_ptr->MAX_PACKET_SIZE) + 
				//   ((total_length % pipe_descr_ptr->MAX_PACKET_SIZE) ? 1 : 0));
				num_of_packets = ((qtd_length / pipe_descr_ptr->MAX_PACKET_SIZE) + 
						((qtd_length % pipe_descr_ptr->MAX_PACKET_SIZE) ? 1 : 0));




			} /* Endif */

			pipe_descr_ptr->NEXTDATA01 ^= (num_of_packets & 0x1);

			/* Initialize the QTD for the OUT data transaction */
			_usb_ehci_init_QTD(handle, QTD_ptr, buff_ptr, token);
			QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
			QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;
			buff_ptr += qtd_length;

			if (total_length) {

				/* Use the QTD to chain the next QTD */         
				prev_QTD_ptr = QTD_ptr;

				/* Get a QTD from the queue for status phase */   
				EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

				if (!QTD_ptr) {
					return USB_STATUS_TRANSFER_IN_PROGRESS;
				} /* Endif */

				usb_host_ptr->QTD_ENTRIES--;

				/* Chain the status phase QTD to the data phase QTD */         
				prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
			} /* Endif */
		} while (total_length); /* EndWhile */
	} /* Endif */

	return ((uint_32)first_QTD_ptr);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_add_xfer_to_asynch_schedule_list
 *  Returned Value : USB Status
 *  Comments       :
 *        Queue the packet in the EHCI hardware Asynchronous schedule list
 *END*-----------------------------------------------------------------*/

uint_32 _usb_ehci_add_xfer_to_asynch_schedule_list
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr=NULL;
	EHCI_QH_STRUCT_PTR                           prev_QH_ptr=NULL;
	EHCI_QH_STRUCT_PTR                           first_QH_ptr;
	EHCI_QTD_STRUCT_PTR                          first_QTD_ptr, temp_QTD_ptr;
	PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_for_queue = NULL;
	ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr, temp_list_ptr;
	uint_32                                      cmd_val;
	boolean                                      init_async_list = FALSE;
	boolean                                      found_existing_q_head = FALSE;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/* All Control and Bulk Transfers */
	/* First initialize all QTDs and then initialize the QHs */

	/* Initialize the QTDs for the Queue Head */
	first_QTD_ptr = (EHCI_QTD_STRUCT_PTR)_usb_ehci_init_Q_element(handle, 
			pipe_descr_ptr, current_pipe_tr_struct_ptr);

	/* If the Asynch Schedule is disabled then initialize a new list */
	if ((!(USBHS_USBCMD & USBHS_USBCMD_ASE_MASK)) &&
			(!(USBHS_USBSTS & USBHS_USBSTS_AS_MASK)) || (!usb_host_ptr->ACTIVE_ASYNC_LIST_PTR)) 
	{
		init_async_list = TRUE;
	} else {
		first_QH_ptr = QH_ptr = (EHCI_QH_STRUCT_PTR)
    						  ((uint_32)usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR & 
    								  EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

		while (QH_ptr) {
			pipe_for_queue = (PIPE_DESCRIPTOR_STRUCT_PTR)QH_ptr->PIPE_DESCR_FOR_THIS_QH;

			if (pipe_for_queue == pipe_descr_ptr) {
				/* found an existing QHEAD for this endpoint */
				found_existing_q_head = TRUE;
				break;
			} else {
				prev_QH_ptr = QH_ptr;
				QH_ptr = (EHCI_QH_STRUCT_PTR)((uint_32)QH_ptr->HORIZ_LINK_PTR & 
						EHCI_QH_HORIZ_PHY_ADDRESS_MASK);
				if (((uint_32)QH_ptr == (uint_32)first_QH_ptr)) {
					break;
				} /* Endif */
			} /* Endif */
		} /* Endwhile */
	} /* Endif */

	if (!found_existing_q_head) {

		active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR)USB_mem_alloc_zero(sizeof(ACTIVE_QH_MGMT_STRUCT));
		active_list_member_ptr->TIME = EHCI_PIPE_TIMEOUT;

		if (!active_list_member_ptr) {
			return USBERR_ALLOC;
		} /* Endif */

		active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = NULL;

		/* Get new QH from the free QH queue */
		EHCI_QH_QGET(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL, QH_ptr);

		if (!QH_ptr) {
			USB_mem_free(active_list_member_ptr);
			return USBERR_ALLOC;
		} /* Endif */

		active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;

		usb_host_ptr->QH_ENTRIES--;

		//printf("\nQH Get 0x%x, #entries=%d",QH_ptr,usb_host_ptr->QH_ENTRIES);

		/**************Not required, done by init Q head routine *****
      pipe_descr_ptr->QH_FOR_THIS_PIPE = (pointer)QH_ptr;
		 **********************************************************/

		if (init_async_list) {
			prev_QH_ptr = (EHCI_QH_STRUCT_PTR)QH_ptr;
			usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = active_list_member_ptr;
		} else {
			temp_list_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
			while (temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR) {
				temp_list_ptr = temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
			} /* EndWhile */
			temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;
		} /* Endif */


		/* Initialize the Queue head */
		_usb_ehci_init_Q_HEAD(handle,pipe_descr_ptr,(EHCI_QH_STRUCT_PTR)QH_ptr, 
				(EHCI_QH_STRUCT_PTR)prev_QH_ptr, first_QTD_ptr);

	} else {

		/* Get the pointer to the member of the active list */
		active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
		while ((active_list_member_ptr != NULL) && (active_list_member_ptr->QH_PTR != QH_ptr)) {
			active_list_member_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
		} /* EndWhile */

		if (active_list_member_ptr == NULL) /* just for safe, should not occur */
			return USBERR_BAD_STATUS;

		active_list_member_ptr->TIME = EHCI_PIPE_TIMEOUT;

		/* Queue head is already on the active list. Simply queue the transfer */
		/* Queue the transfer onto the relevant queue head */
		if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT) {
			/* No alternate descriptor */
			QH_ptr->ALT_NEXT_QTD_LINK_PTR |= EHCI_QTD_T_BIT;
			QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
		} else {
			temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)(QH_ptr->NEXT_QTD_LINK_PTR & ~EHCI_QTD_T_BIT);
			while (!(temp_QTD_ptr->NEXT_QTD_PTR & EHCI_QTD_T_BIT)) {
				temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)(temp_QTD_ptr->NEXT_QTD_PTR & ~EHCI_QTD_T_BIT);
			} /* EndWhile */
			temp_QTD_ptr->NEXT_QTD_PTR = (uint_32)first_QTD_ptr;
		} /* Endif */

	} /* Endif */   

	if (init_async_list) {
		/* Write the QH address to the Current Async List Address */
		USBHS_ASYNCLISTADDR = (uint_32)QH_ptr;
	} /* Endif */

	if (!((uint_32) active_list_member_ptr->FIRST_QTD_PTR & EHCI_QH_HORIZ_PHY_ADDRESS_MASK))
		active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;

	/* Enable the Asynchronous schedule if:
	 ** if asynchronous_schedule_enable == asynchronous_schedule_status
	 */
	cmd_val = USBHS_USBCMD;
	if ((cmd_val & USBHS_USBCMD_ASE_MASK) == (USBHS_USBSTS & USBHS_USBSTS_AS_MASK)) 
	{
		USBHS_USBCMD = (cmd_val | USBHS_USBCMD_ASE_MASK);
	} /* Endif */

	return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_port_change
 *  Returned Value : None
 *  Comments       :
 *        Process port change
 *END*-----------------------------------------------------------------*/

boolean _usb_ehci_process_port_change
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
	uint_8                              i, total_port_numbers;
	uint_32                             port_control, status;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	// cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)
	//    _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);
	////


	i = 0;

	port_control = USBHS_PORTSC1;

	if (port_control & USBHS_PORTSC1_CSC_MASK) {
		/* Turn on the 125 usec uframe interrupt. This effectively 
		 ** starts the timer to count 125 usecs 
		 */
		USBHS_USBINTR |= USBHS_USBINTR_SRE_MASK;
		do {
			if (port_control & USBHS_PORTSC1_CSC_MASK) {
				usb_host_ptr->UFRAME_COUNT = 0;
				port_control = USBHS_PORTSC1;
				port_control &= ~EHCI_PORTSCX_W1C_BITS;
				port_control |= USBHS_PORTSC1_CSC_MASK;
				USBHS_PORTSC1 = port_control;
			} /* Endif */
			status = (USBHS_USBSTS & USBHS_USBINTR);
			if (status & USBHS_USBSTS_SRI_MASK) {
				/* Increment the 125 usecs count */
				usb_host_ptr->UFRAME_COUNT++;
				USBHS_USBSTS = status;
			} /* Endif */
			port_control = USBHS_PORTSC1;
		} while (usb_host_ptr->UFRAME_COUNT != 2);
		/* Turn off the 125 usec uframe interrupt. This effectively 
		 ** stops the timer to count 125 usecs 
		 */
		USBHS_USBINTR &= ~USBHS_USBSTS_SRI_MASK;

		port_control = USBHS_PORTSC1;

		usb_host_ptr->UFRAME_COUNT = 0;

		/* We waited to check for stable current connect status */         
		if (port_control & USBHS_PORTSC1_CCS_MASK) {
			/* Attach on port i */
			/* send change report to the hub-driver */
			/* The hub driver does GetPortStatus and identifies the new connect */
			/* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */
			/* reset and enable the port */
			_usb_ehci_reset_and_enable_port(handle, i);
		} else {
			/* Detach on port i */
			/* send change report to the hub-driver */
			/* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */

			/* clear the connect status change */
			port_control = USBHS_PORTSC1;
			port_control &= ~EHCI_PORTSCX_W1C_BITS;
			port_control |= USBHS_PORTSC1_CSC_MASK;
			USBHS_PORTSC1 = port_control;

			/* Disable the asynch and periodic schedules */
			USBHS_USBCMD &=	~(USBHS_USBCMD_ASE_MASK|USBHS_USBCMD_PSE_MASK);

			if ((_usb_host_check_service((pointer)usb_host_ptr, 
					USB_SERVICE_DETACH, 0)) == USB_OK) 
			{
				_usb_host_call_service(usb_host_ptr, USB_SERVICE_DETACH, 0);
			} else {
				/* call device detach (host pointer, speed, hub #, port #) */
				usb_dev_list_detach_device((pointer)usb_host_ptr, 0, (uint_8)((i + 1)));
			} /* Endif */ 

			if (!i) {
				return TRUE;
			} /* Endif */
		} /* Endif */
	} /* Endif */

	if (port_control & USBHS_PORTSC1_FPR_MASK) {
		port_control = USBHS_PORTSC1;
		port_control &= ~EHCI_PORTSCX_W1C_BITS;
		port_control &= ~USBHS_PORTSC1_FPR_MASK;
		_usb_host_call_service((pointer)usb_host_ptr, USB_SERVICE_HOST_RESUME, 0);
	} /* Endif */

	if ((usb_host_ptr->IS_RESETTING != FALSE) && 
			(port_control & EHCI_PORTSCX_PORT_ENABLE))
	{
		usb_host_ptr->IS_RESETTING = FALSE;

		/* reset process complete */
		/* Report the change to the hub driver and enumerate */
		usb_host_ptr->SPEED = ((port_control & VUSBHS_SPEED_MASK) >> VUSBHS_SPEED_BIT_POS);
		usb_host_ptr->PORT_NUM = (uint_32)(i+1);

		/* Now wait for reset recovery */
		usb_host_ptr->RESET_RECOVERY_TIMER = (USB_RESET_RECOVERY_DELAY*8);

		/* Turn on the 125 usec uframe interrupt. This effectively 
		 ** starts the timer to count 125 usecs 
		 */
		USBHS_USBINTR |= USBHS_USBINTR_SRE_MASK;
	} /* Endif */


	return FALSE;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_reset_and_enable_port
 *  Returned Value : None
 *  Comments       :
 *        Reset and enabled the port
 *END*-----------------------------------------------------------------*/

void _usb_ehci_reset_and_enable_port
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle,

		/* [IN] the port number */
		uint_8                  port_number
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR               dev_ptr;
	uint_32                             port_status_control;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/* Check the line status bit in the PORTSC register */
	port_status_control = USBHS_PORTSC1;

	port_status_control &= ~EHCI_PORTSCX_W1C_BITS;

#ifdef __USB_OTG__
	/* reset should wait for 100 Ms debouce period before starting*/
	if ((usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != A_SUSPEND) &&  
			(usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != B_WAIT_ACON))
	{
		uint_32 i;

		for (i = 0; i < USB_DEBOUNCE_DELAY; i++) {
			_usb_host_delay(handle, 1);

			/* Check the line status bit in the PORTSC register */
			port_status_control = 
					dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number];

			if (!(port_status_control & EHCI_PORTSCX_CURRENT_CONNECT_STATUS)) {
				return;
			} /* Endif */
		} /* Endfor */

	}

#endif

	usb_host_ptr->IS_RESETTING = TRUE;

	/*
	 ** Start the reset process
	 */
	USBHS_PORTSC1 = (port_status_control | USBHS_PORTSC1_PR_MASK);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_host_process_reset_recovery_done
 *  Returned Value : None
 *  Comments       :
 *        Reset and enabled the port
 *END*-----------------------------------------------------------------*/

void _usb_host_process_reset_recovery_done
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR               dev_ptr;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

	/* call device attach (host pointer, speed, hub #, port #) */
	if ((_usb_host_check_service((pointer)usb_host_ptr, 
			USB_SERVICE_ATTACH, usb_host_ptr->SPEED)) == USB_OK) 
	{
		_usb_host_call_service(usb_host_ptr, USB_SERVICE_ATTACH, 
				usb_host_ptr->SPEED);
	} else {
		usb_dev_list_attach_device((pointer)usb_host_ptr, 
				(uint_8)(usb_host_ptr->SPEED), 0, (uint_8)(usb_host_ptr->PORT_NUM));
	} /* Endif */ 
	/* Turn off the 125 usec uframe interrupt. This effectively 
	 ** stops the timer to count 125 usecs 
	 */
	USBHS_USBINTR &= ~USBHS_USBINTR_SRE_MASK;
} /* EndBody */


// todo: remove this in HIGH SPEED mode(needed by IAR)
void USB_ISR(){
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : USBHS_ISR_HOST
 *  Returned Value : None
 *  Comments       :
 *        Service all the interrupts in the ULPI hardware
 *END*-----------------------------------------------------------------*/
void USBHS_ISR_HOST(void)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR	usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR		dev_ptr;
	uint_32						status;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)usb_host_state_struct_ptr;

	/* We use a while loop to process all the interrupts while we are in the 
	 ** loop so that we don't miss any interrupts 
	 */
	while (TRUE) {
		/* Check if any of the enabled interrupts are asserted */
		status = USBHS_USBSTS & USBHS_USBINTR;

		if (!status) {
			/* No more interrupts pending to be serviced */
			//break;
			return;
		} /* Endif */

		//		/printf("status: %d\n", status);

		/* Clear all enabled interrupts */


		if (status & EHCI_STS_SOF_COUNT) {
			USBHS_USBSTS = status;
			/* Waiting for an interval of 10 ms for reset recovery */
			if (usb_host_ptr->RESET_RECOVERY_TIMER) {
				usb_host_ptr->RESET_RECOVERY_TIMER--;
				if (!usb_host_ptr->RESET_RECOVERY_TIMER) {
					_usb_host_process_reset_recovery_done((pointer)usb_host_ptr);
				} /* Endif */
			} /* Endif */
		} /* Endif */

		if (status & EHCI_STS_ASYNCH_ADVANCE) {
			//printf("EHCI_STS_ASYNCH_ADVANCE\n");
			USBHS_USBSTS = status;
			/* Process the asynch advance */
		} /* Endif */

		if (status & EHCI_STS_HOST_SYS_ERROR) {
			printf("EHCI_STS_HOST_SYS_ERROR\n");
			USBHS_USBSTS = status;
			/* Host system error. Controller halted. Inform the upper layers */
			_usb_host_call_service((pointer)usb_host_ptr, USB_SERVICE_SYSTEM_ERROR, 0);
		} /* Endif */

		if (status & EHCI_STS_FRAME_LIST_ROLLOVER) {
			/* Process frame list rollover */
			printf("EHCI_STS_FRAME_LIST_ROLLOVER\n");
			USBHS_USBSTS = status;
		} /* Endif */

		if (status & EHCI_STS_RECLAIMATION) {
			/* Process reclaimation */
			printf("EHCI_STS_RECLAIMATION\n");
			USBHS_USBSTS = status;
		} /* Endif */

		if (status & EHCI_STS_NAK) {
			printf("EHCI_STS_NAK\n");
			USBHS_USBSTS = status;
			_usb_ehci_process_tr_complete((pointer)usb_host_ptr);
		}

		if (status & EHCI_STS_PORT_CHANGE) {
			printf("EHCI_STS_PORT_CHANGE\n");
			USBHS_USBSTS = status;

			/* Process the port change detect */
			if (_usb_ehci_process_port_change((pointer)usb_host_ptr)) {
				/* There was a detach on port 0 so we should return */
				printf("detach\n");
				return;
			} /* Endif */
			/* Should return if there was a detach on OTG port */
		} /* Endif */

		if (status & (EHCI_STS_USB_INTERRUPT | EHCI_STS_USB_ERROR)) {
			//printf("EHCI_STS_USB_INTERRUPT | EHCI_STS_USB_ERROR\n");
			USBHS_USBSTS = status;
			/* Process the USB transaction completion and transaction error 
			 ** interrupt 
			 */

			_usb_ehci_process_tr_complete((pointer)usb_host_ptr);
#ifdef DEBUG_INFO 
			printf("TR completed\n");
#endif

		} /* Endif */

		if (status & EHCI_STS_TIMER0) {
			USBHS_USBSTS = status;
			_usb_ehci_process_timer((pointer)usb_host_ptr);
		}

	} /* EndWhile */

} /* Endbody */

// todo: check

USB_STATUS  _usb_hci_vusb20_init
(
		/* the USB Host State structure */
		_usb_host_handle     handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr;
	EHCI_QTD_STRUCT_PTR                          QTD_ptr;   
	uint_8                                       vector;
	uint_32                                      i,frame_list_size_bits;
#ifndef __USB_OTG__
	uint_32                                      port_control;
#endif
	uint_32_ptr                                  temp_periodic_list_ptr = NULL;
	uint_32                                      total_non_periodic_memory=0;
	pointer                                      non_periodic_mem_ptr=NULL;
	uint_32                                      total_periodic_memory=0;
	pointer                                      periodic_mem_ptr=NULL;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;


	/*******************************************************************
    Set the size of frame list in CMD register
	 *******************************************************************/
	if(usb_host_ptr->FRAME_LIST_SIZE > 512){
		usb_host_ptr->FRAME_LIST_SIZE = 1024;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_1024;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 256){
		usb_host_ptr->FRAME_LIST_SIZE = 512;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_512;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 128){
		usb_host_ptr->FRAME_LIST_SIZE = 256;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_256;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 64){
		usb_host_ptr->FRAME_LIST_SIZE = 128;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_128;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 32){
		usb_host_ptr->FRAME_LIST_SIZE = 64;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_64;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 16){
		usb_host_ptr->FRAME_LIST_SIZE = 32;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_32;
	}else if(usb_host_ptr->FRAME_LIST_SIZE > 8){
		usb_host_ptr->FRAME_LIST_SIZE = 16;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_16;
	}else{
		usb_host_ptr->FRAME_LIST_SIZE = 8;
		frame_list_size_bits = EHCI_CMD_FRAME_SIZE_8;
	}

	/*
	 **   ALL CONTROLLER DRIVER MEMORY NEEDS are allocated here.
	 */
	total_non_periodic_memory += (MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;
	total_non_periodic_memory += (MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;

	total_periodic_memory += usb_host_ptr->FRAME_LIST_SIZE*sizeof(uint_8_ptr)*8*sizeof(uint_8);
	total_periodic_memory += (sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER)) * usb_host_ptr->FRAME_LIST_SIZE + 4096;

	/*memory required by high-speed Iso transfers */
	total_periodic_memory += (MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;
	total_periodic_memory +=  sizeof(LIST_NODE_STRUCT)*MAX_ITD_DESCRS;

	/*memory required by full-speed Iso transfers */
	total_periodic_memory +=  (MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;
	total_periodic_memory +=  sizeof(LIST_NODE_STRUCT)*MAX_SITD_DESCRS;

	total_mem_ptr = USB_mem_alloc_uncached(total_non_periodic_memory + total_periodic_memory);
	if (!total_mem_ptr) {
#ifdef SERIAL_DEBUG
		printf("error allocating memory\n");
#endif
		return USBERR_ALLOC;
	}
	memset(total_mem_ptr, 0, total_non_periodic_memory + total_periodic_memory);

	// store the memory block pointer in device handle
	usb_host_ptr->CONTROLLER_MEMORY = total_mem_ptr;

	// divide memory between periodic and non periodic memories 
	non_periodic_mem_ptr = total_mem_ptr;
	periodic_mem_ptr = (uint_8_ptr) total_mem_ptr + total_non_periodic_memory;


	/*
	 **   NON-PERIODIC MEMORY DISTRIBUTION STUFF
	 */
	usb_host_ptr->ASYNC_LIST_BASE_ADDR = (pointer) non_periodic_mem_ptr;
	usb_host_ptr->QH_BASE_PTR = (pointer) non_periodic_mem_ptr;

	usb_host_ptr->ALIGNED_ASYNCLIST_ADDR = (pointer)USB_MEM32_ALIGN((uint_32)usb_host_ptr->ASYNC_LIST_BASE_ADDR);
	usb_host_ptr->QH_ALIGNED_BASE_PTR =	(pointer)USB_MEM32_ALIGN((uint_32)usb_host_ptr->ASYNC_LIST_BASE_ADDR);


	non_periodic_mem_ptr =  (uint_8_ptr) non_periodic_mem_ptr + (MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;
	usb_host_ptr->QTD_BASE_PTR = (EHCI_QTD_STRUCT_PTR) non_periodic_mem_ptr;

	// Align the QTD base to 32 byte boundary
	usb_host_ptr->QTD_ALIGNED_BASE_PTR = (EHCI_QTD_STRUCT_PTR) USB_MEM32_ALIGN((uint_32)usb_host_ptr->QTD_BASE_PTR);

	non_periodic_mem_ptr =  (uint_8_ptr) non_periodic_mem_ptr + (MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;

	QH_ptr = usb_host_ptr->QH_ALIGNED_BASE_PTR;

	for (i=0;i<MAX_QH_DESCRS;i++) {
		// Set the dTD to be invalid
		QH_ptr->HORIZ_LINK_PTR = EHCI_QUEUE_HEAD_POINTER_T_BIT;
		_usb_hci_vusb20_free_QH((pointer)usb_host_ptr, (pointer)QH_ptr);
		QH_ptr++;
	}

	QTD_ptr = usb_host_ptr->QTD_ALIGNED_BASE_PTR;

	// Enqueue all the QTDs
	for (i=0;i<MAX_QTD_DESCRS;i++) {
		/* Set the QTD to be invalid */
		QTD_ptr->NEXT_QTD_PTR = EHCI_QTD_T_BIT;
		_usb_hci_vusb20_free_QTD((pointer)usb_host_ptr, (pointer)QTD_ptr);
		QTD_ptr++;
	}

	USBHS_ASYNCLISTADDR = (uint_32)usb_host_ptr->ALIGNED_ASYNCLIST_ADDR;

	/*
	 **   BANDWIDTH MEMORY DISTRIBUTION STUFF
	 */
	/*********************************************************************
	 * Allocating the memory to store periodic bandwidth. A periodic BW list
	 * is a two dimensional array with dimension (frame list size x 8 u frames).
	 * Also note that the following could be a large allocation of memory
	 * The max value stored in a location will be 125 micro seconds and so we
	 * use uint_8
	 *********************************************************************/

	usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR = (uint_8_ptr) periodic_mem_ptr;
	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			usb_host_ptr->FRAME_LIST_SIZE*sizeof(uint_8_ptr)*8*sizeof(uint_8);

	USB_mem_zero(usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR,
			usb_host_ptr->FRAME_LIST_SIZE *  \
			sizeof(uint_8_ptr) * \
			8 *            \
			sizeof(uint_8)  \
	);


	usb_host_ptr->PERIODIC_LIST_BASE_ADDR = periodic_mem_ptr;
	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			(sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER)) * 
			usb_host_ptr->FRAME_LIST_SIZE + 4096;
	usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR = 
			(pointer)USB_MEM4096_ALIGN((uint_32)usb_host_ptr->PERIODIC_LIST_BASE_ADDR);
	USBHS_PERIODICLISTBASE = (uint_32)usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR;

	// make sure that periodic list is uninitialized   
	usb_host_ptr->PERIODIC_LIST_INITIALIZED = FALSE;  

	// Initialize the list of active structures to NULL initially
	usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = NULL;
	usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = NULL;


	/*
	 **  HIGH SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
	 */
	usb_host_ptr->ITD_BASE_PTR = (EHCI_ITD_STRUCT_PTR) periodic_mem_ptr;
	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			(MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;


	// Align the ITD base to 32 byte boundary   
	usb_host_ptr->ITD_ALIGNED_BASE_PTR = (EHCI_ITD_STRUCT_PTR)
    						  USB_MEM32_ALIGN((uint_32)usb_host_ptr->ITD_BASE_PTR);


	usb_host_ptr->ITD_LIST_INITIALIZED = FALSE;

	/*****************************************************************************
     ITD QUEUING PREPARATION 
	 *****************************************************************************/
	/* memory for doubly link list of nodes that keep active ITDs. Head and Tail point to
	 * same place when list is empty */
	usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
	usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;
	USB_mem_zero(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
			sizeof(LIST_NODE_STRUCT)*MAX_ITD_DESCRS);

	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			sizeof(LIST_NODE_STRUCT)*MAX_ITD_DESCRS;


	/*
	 **   FULL SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
	 */
	/* Allocate the Split-transactions Isochronous Transfer Descriptors: 
	 ** 32 bytes aligned 
	 */
	usb_host_ptr->SITD_BASE_PTR = (EHCI_SITD_STRUCT_PTR) periodic_mem_ptr;
	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			(MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;  


	// Align the SITD base to 32 byte boundary   
	usb_host_ptr->SITD_ALIGNED_BASE_PTR = (EHCI_SITD_STRUCT_PTR)
    						  USB_MEM32_ALIGN((uint_32)usb_host_ptr->SITD_BASE_PTR);

	usb_host_ptr->SITD_LIST_INITIALIZED = FALSE;

	/*****************************************************************************
     SITD QUEUING PREPARATION 
	 *****************************************************************************/

	/* memory for doubly link list of nodes that keep active SITDs. Head and Tail point to
   same place when list is empty */
	usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
	usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;

	USB_mem_zero(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
			sizeof(LIST_NODE_STRUCT)*MAX_SITD_DESCRS);

	periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr + 
			sizeof(LIST_NODE_STRUCT)*MAX_SITD_DESCRS;

	/*
	 ** HARDWARE  REGISTERS INITIALIZATION STUFF
	 */

#ifndef __USB_OTG__
#ifndef MCU_MK70F12
	if (USBHS_HCCPARAMS & VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG) 
	{
		port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0];
		port_control &= ~EHCI_PORTSCX_W1C_BITS;
		dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0] = 
				(port_control | EHCI_PORTSCX_PORT_POWER);
	} /* Endif */
#endif	// MCU_MK70F12
#endif

	// Configure the VUSBHS has a host controller
	USBHS_USBMODE = USBHS_USBMODE_CM(3);

	USBHS_USBCMD =	USBHS_USBCMD_ASP(3) |
			USBHS_USBCMD_ITC(0) ;

	// set running state
	USBHS_USBCMD |= USBHS_USBCMD_RS_MASK;

	/* Apply port power. This does not initialize the transceiver.
	 * If ULPI is being used it should be initialized before this point. */
	USBHS_PORTSC1 |= USBHS_PORTSC1_PP_MASK;

	// start 10 msec = 10000 usec infinite repeat timer
	USBHS_GPTIMER0LD = 10000 - 1;

	USBHS_GPTIMER0CTL = USBHS_GPTIMER1CTL_MODE_MASK |
			USBHS_GPTIMER1CTL_RST_MASK |
			USBHS_GPTIMER1CTL_RUN_MASK;

	// Write the appropriate value to the USBINTR register to enable the appropriate interrupts
	USBHS_USBINTR = 
			USBHS_USBINTR_UE_MASK|		// bit 0. USB interrupt Enable
			USBHS_USBINTR_UEE_MASK|		// bit 1. USB Error interrupt Enable
			USBHS_USBINTR_PCE_MASK|		// bit 2. Port Change detect Enable
			USBHS_USBINTR_SEE_MASK|		// bit 4. System Error Enable
			USBHS_USBINTR_AAE_MASK|		// bit 5. Interrupt on Async advance Enable
			USBHS_USBINTR_ULPIE_MASK	// bit 10.ULPI Enable
			;
	USBHS_USBINTR |= USBHS_USBINTR_TIE0_MASK;


	return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_free_QTD
 *  Returned Value : void
 *  Comments       :
 *        Enqueues an QTD onto the free QTD ring.
 *
 *END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_free_QTD
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle,

		/* [IN] the QTD to enqueue */
		pointer                 QTD_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/*
	 ** This function can be called from any context, and it needs mutual
	 ** exclusion with itself.
	 */
	USB_lock();

	((EHCI_QTD_STRUCT_PTR)QTD_ptr)->NEXT_QTD_PTR = EHCI_QTD_T_BIT;

	/*
	 ** Add the QTD to the free QTD queue and increment the tail to the next descriptor
	 */
	EHCI_QTD_QADD(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, 
			(EHCI_QTD_STRUCT_PTR)QTD_ptr);
	usb_host_ptr->QTD_ENTRIES++;

	USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_free_QH
 *  Returned Value : void
 *  Comments       :
 *        Enqueues a QH onto the free QH ring.
 *
 *END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_free_QH
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle,

		/* [IN] the QH to enqueue */
		pointer                 QH_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	EHCI_QH_STRUCT_PTR tmp;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/*
	 ** This function can be called from any context, and it needs mutual
	 ** exclusion with itself.
	 */
	USB_lock();

	/*
	 ** Add the QH to the free QH queue and increment the tail to the next descriptor
	 */
	//EHCI_QH_QADD(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL,
	//	(EHCI_QH_STRUCT_PTR)QH_ptr);

	if ((usb_host_ptr->QH_HEAD) == NULL) {
		(usb_host_ptr->QH_HEAD) = (pointer) ((EHCI_QH_STRUCT_PTR)QH_ptr);
	} else {
		(usb_host_ptr->QH_TAIL)->SCRATCH_PTR = (void *) ((EHCI_QH_STRUCT_PTR)QH_ptr);
	} /* Endif */
	(usb_host_ptr->QH_TAIL) = ((EHCI_QH_STRUCT_PTR)QH_ptr);
	((EHCI_QH_STRUCT_PTR)QH_ptr)->SCRATCH_PTR = NULL;

	usb_host_ptr->QH_ENTRIES++;

	// printf("\nQH Add 0x%x, #entries=%d",QH_ptr,usb_host_ptr->QH_ENTRIES);

	USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_free_resources
 *  Returned Value : none
 *  Comments       :
 *        Frees the controller specific resources for a given pipe
 *END*-----------------------------------------------------------------*/
// EAI - need to return a status result or pipe will not close
USB_STATUS _usb_ehci_free_resources
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR                        dev_ptr;
	ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
	ACTIVE_QH_MGMT_STRUCT_PTR                    prev_ptr = NULL, next_ptr = NULL;
	EHCI_QH_STRUCT_PTR                           QH_ptr, temp_QH_ptr;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;
	QH_ptr = pipe_descr_ptr->QH_FOR_THIS_PIPE;

	if ((pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE) || (pipe_descr_ptr->PIPETYPE == USB_BULK_PIPE))
	{
		USB_lock();

		/************************************************************
      Close the pipe on Async transfers (Bulk and Control)   
		 ************************************************************/
		/* Get the head of the active queue head */
		active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;


		while (active_list_member_ptr) {

			temp_QH_ptr = (EHCI_QH_STRUCT_PTR)
            						((uint_32)active_list_member_ptr->QH_PTR & 
            								EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

			next_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

			if (temp_QH_ptr == QH_ptr) {
				/* Found it */

				if (prev_ptr) {
					/* If this is not the first struct in the queue */
					prev_ptr->QH_PTR->HORIZ_LINK_PTR = QH_ptr->HORIZ_LINK_PTR;

					USB_mem_free(active_list_member_ptr);
					prev_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = next_ptr;
				} else {
					/* If this is the first struct in the queue */
					if (next_ptr == NULL) {
						/* If the head is the only one in the queue, disable asynchronous queue */
						USBHS_USBCMD &= ~USBHS_USBCMD_ASE_MASK;
					}
					else {
						/* we dont have previous pointer, so roll down to the end of list */
						/* get last queue head */   
						while ((temp_QH_ptr->HORIZ_LINK_PTR & EHCI_QH_HORIZ_PHY_ADDRESS_MASK) != (uint_32) QH_ptr)
							temp_QH_ptr = (pointer) (temp_QH_ptr->HORIZ_LINK_PTR & EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

						/* get next queue for last queue head */
						next_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
						/* link queue heads in the controller */
						temp_QH_ptr->HORIZ_LINK_PTR = QH_ptr->HORIZ_LINK_PTR;
					}

					USB_mem_free(active_list_member_ptr);
					usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = next_ptr;
				} /* Endif */

				/* move it to free queue heads */
				_usb_hci_vusb20_free_QH(handle, (pointer) QH_ptr);

				break;
			} /* Endif */

			prev_ptr = active_list_member_ptr;
			active_list_member_ptr = next_ptr;

		} /* Endwhile */

		USB_unlock();

	}
	else if(pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE)
	{
		_usb_ehci_close_interrupt_pipe(handle,pipe_descr_ptr);
	}
	else if(pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE)
	{
		_usb_ehci_close_isochronous_pipe(handle,pipe_descr_ptr);
	}

	return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_tr_complete
 *  Returned Value : None
 *  Comments       :
 *     Process the Transaction Done interrupt on the EHCI hardware. Note that this
 *     routine should be improved later. It is needless to search all the lists
 *     since interrupt will belong to only one of them at one time.
 *
 *END*-----------------------------------------------------------------*/

void _usb_ehci_process_tr_complete
(
		/* [IN] the USB Host state structure */
		_usb_host_handle           handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/***************************************************************************
    SEARCH THE NON PERIODIC LIST FIRST
	 ***************************************************************************/

	/* Get the head of the active queue head */
	active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
	if(active_list_member_ptr) 
	{
		_usb_ehci_process_qh_list_tr_complete(handle,active_list_member_ptr);
	}

	/***************************************************************************
    SEARCH THE INTERRUPT LIST
	 ***************************************************************************/

	/* Get the head of the active structures for periodic list*/
	active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
	if(active_list_member_ptr) 
	{
		_usb_ehci_process_qh_interrupt_tr_complete(handle,active_list_member_ptr);
	}

	/***************************************************************************
    SEARCH THE HIGH SPEED ISOCHRONOUS LIST
	 ***************************************************************************/
	if(usb_host_ptr->HIGH_SPEED_ISO_QUEUE_ACTIVE) 
	{
		_usb_ehci_process_itd_tr_complete(handle);
	}

	/***************************************************************************
    SEARCH THE FULL SPEED ISOCHRONOUS LIST
	 ***************************************************************************/
	if(usb_host_ptr->FULL_SPEED_ISO_QUEUE_ACTIVE) 
	{
		_usb_ehci_process_sitd_tr_complete(handle);
	}


} /* Endbody */

/* EOF */




/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_qh_list_tr_complete
 *  Returned Value : None
 *  Comments       :
 *     Search the asynchronous or interrupt list to see which QTD had finished and 
 *     Process the interrupt.
 *
 *END*-----------------------------------------------------------------*/

static void _usb_ehci_process_qh_list_tr_complete
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR                        dev_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr;
	EHCI_QTD_STRUCT_PTR                          QTD_ptr;
	EHCI_QTD_STRUCT_PTR                          temp_QTD_ptr;
	PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
	PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
	boolean                                      first_phase = FALSE;
	uint_32                                      total_req_bytes = 0;
	uint_32                                      remaining_bytes = 0;
	uint_32                                      errors = 0, status = 0, active = 0, expired;
	uchar_ptr                                    buffer_ptr = NULL;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = usb_host_ptr->DEV_PTR;

	/* Check all transfer descriptors on all active queue heads */
	do {
		/* Get the queue head from the active list */
		QH_ptr = active_list_member_ptr->QH_PTR;
		/* Get the first QTD for this Queue head */
		QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;

#if PSP_HAS_DATA_CACHE
		USB_dcache_invalidate_mlines((void *)QH_ptr,sizeof(EHCI_QH_STRUCT));
		USB_dcache_invalidate_mlines((void *)QTD_ptr,sizeof(EHCI_QTD_STRUCT));
#endif

		if (!(active_list_member_ptr->TIME))
			expired = 1;
		else
			expired = 0;

		while (!(((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)) 
		{
			/* This is a valid qTD */
			if (!(QTD_ptr->TOKEN & EHCI_QTD_STATUS_ACTIVE) || expired) 
			{

#ifdef DEBUG_INFO
				printf("QTD done Token=%x\n"
						"  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
						QTD_ptr->TOKEN,
						((QTD_ptr->TOKEN)&0xFF),
						((QTD_ptr->TOKEN) >> 8)&0x3,
						((QTD_ptr->TOKEN) >> 10) &0x3,
						((QTD_ptr->TOKEN) >> 12)&0x7,
						((QTD_ptr->TOKEN) >> 15)&0x1,
						((QTD_ptr->TOKEN) >> 16)&0x7FFF,
						((QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);
#endif

				/* Get the pipe descriptor for this transfer */
				pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)QTD_ptr->\
						PIPE_DESCR_FOR_THIS_QTD;
				pipe_tr_struct_ptr = (PIPE_TR_STRUCT_PTR)QTD_ptr->\
						TR_FOR_THIS_QTD;

				/* Check for errors */
				if (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK) 
				{
					errors |= (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK);
					QTD_ptr->TOKEN &= ~EHCI_QTD_ERROR_BITS_MASK;
					status = USBERR_TR_FAILED;
				} /* Error */

				/* Check if STALL or endpoint halted because of errors */
				if (QTD_ptr->TOKEN & EHCI_QTD_STATUS_HALTED) 
				{
					errors |= EHCI_QTD_STATUS_HALTED;
					status = USBERR_ENDPOINT_STALLED;
					QTD_ptr->TOKEN &= ~EHCI_QTD_STATUS_HALTED;
					QH_ptr->STATUS = 0;
				} /* Endif */

				if (pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE) 
				{

					if (QTD_ptr->TOKEN & EHCI_QTD_PID_SETUP) 
					{
						first_phase = TRUE;
					} 
					else if (first_phase) 
					{
						first_phase = FALSE;
						if (pipe_tr_struct_ptr->SEND_PHASE) 
						{
							total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
							buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
							pipe_tr_struct_ptr->SEND_PHASE = FALSE;
						} 
						else 
						{
							total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
							buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
						} /* Endif */
						/* Increment the total number of bytes sent/received */
						remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >> 
								EHCI_QTD_LENGTH_BIT_POS);
					} /* Endif */
				} 
				else 
				{

					if (pipe_descr_ptr->DIRECTION) {
						total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
						buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
					} else {
						total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
						buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
					} /* Endif */
					remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >> 
							EHCI_QTD_LENGTH_BIT_POS);

				} /* Endif */

				if (QTD_ptr->TOKEN & EHCI_QTD_IOC) 
				{
					/* total number of bytes sent/received */
					pipe_descr_ptr->SOFAR += (total_req_bytes - remaining_bytes);
					pipe_tr_struct_ptr->status = USB_STATUS_IDLE;
				} /* Endif */

				temp_QTD_ptr = QTD_ptr;

				QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;

				/* Queue the transfer onto the relevant queue head */
				QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;

				/* Dequeue the used QTD */
				_usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);

				/* queue the transfer only if it is a valid transfer */

				if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT) 
				{
					/* No alternate descriptor */
					QH_ptr->ALT_NEXT_QTD_LINK_PTR |= EHCI_QTD_T_BIT;

					/* Clear the status field */
					QH_ptr->STATUS &= EHCI_QH_TR_OVERLAY_DT_BIT;
				} /* Endif */

				/* Update the new first transfer descriptor for the
				 ** processed queue 
				 */
				if (!active)
					active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)QTD_ptr;

				/* Now mark it as unused so that next request can use it */
				pipe_tr_struct_ptr->TR_INDEX = 0;

				if (pipe_tr_struct_ptr->status == USB_STATUS_IDLE) 
				{
					/* If there are remaining bytes, that means device returned less than expected.
					 ** We set USBERR_TR_FAILED, but still buffer_ptr is not NULL.
					 ** Application should check: if buffer_ptr is not NULL and status is USBERR_TR_FAILED,
					 ** then it means the transfer was done although not as expected (perhaps the last packet ?)
					 */
					if ((remaining_bytes > 0) && (status == USB_OK))
						status = USBERR_TR_FAILED;

					/* Transfer done. Call the callback function for this 
					 ** transaction if there is one (usually true). 
					 */
					if (pipe_tr_struct_ptr->CALLBACK != NULL) 
					{
#ifdef DEBUG_INFO
						printf("_usb_ehci_process_qh_list_tr_complete: Callback\n");
#endif
						pipe_tr_struct_ptr->CALLBACK((pointer)pipe_descr_ptr, 
								pipe_tr_struct_ptr->CALLBACK_PARAM,
								buffer_ptr,
								(total_req_bytes - remaining_bytes),
								status);

						/* If the application enqueued another request on this pipe 
						 ** in this callback context then it will be at the end of the list
						 */
					} /* Endif */
					errors = 0;
					status = 0;
				} /* Endif */
				active_list_member_ptr->TIME = EHCI_PIPE_TIMEOUT;
				expired = 0;


			} /* Endif */
			else 
			{
				QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;
				active = 1;
			}

			// EAI
			if (CACHE_FIX) {
				USB_dcache_invalidate_mlines((void *)QTD_ptr,sizeof(EHCI_QTD_STRUCT));
			}

		} /* EndWhile */

#ifdef DEBUG_INFO
		printf("_usb_ehci_process_qh_list_tr_complete: next active QH\n");
#endif

		active_list_member_ptr = 
				active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

		if (expired && active_list_member_ptr)
			active_list_member_ptr->TIME = -1; /* it means that no QTD is in this QH */

	} while (active_list_member_ptr);   /* While there are more queue 
	 ** heads in the active list */ 


} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_qh_interrupt_tr_complete
 *  Returned Value : None
 *  Comments       :
 *     Search the interrupt list to see which QTD had finished and 
 *     Process the interrupt.
 *
 *END*-----------------------------------------------------------------*/

static void _usb_ehci_process_qh_interrupt_tr_complete
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR                        dev_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr;
	EHCI_QTD_STRUCT_PTR                          QTD_ptr;
	EHCI_QTD_STRUCT_PTR                          temp_QTD_ptr;
	PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
	PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
	uint_32                                      total_req_bytes = 0;
	uint_32                                      remaining_bytes = 0;
	uint_32                                      errors = 0, status = 0;
	uchar_ptr                                    buffer_ptr = NULL;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = usb_host_ptr->DEV_PTR;

	/* Check all transfer descriptors on all active queue heads */
	do {
		/* Get the queue head from the active list */
		QH_ptr = active_list_member_ptr->QH_PTR;
		/* Get the first QTD for this Queue head */
		QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;


		while ((!(((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)) && (QTD_ptr != NULL))
		{
			/* This is a valid qTD */


#ifdef DEBUG_INFO
			printf("_usb_ehci_process_qh_interrupt_tr_complete: QTD =%x\n"
					"  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
					QTD_ptr->TOKEN,
					((QTD_ptr->TOKEN)&0xFF),
					((QTD_ptr->TOKEN) >> 8)&0x3,
					((QTD_ptr->TOKEN) >> 10) &0x3,
					((QTD_ptr->TOKEN) >> 12)&0x7,
					((QTD_ptr->TOKEN) >> 15)&0x1,
					((QTD_ptr->TOKEN) >> 16)&0x7FFF,
					((QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);
#endif

			if (!(QTD_ptr->TOKEN & EHCI_QTD_STATUS_ACTIVE)) 
			{

				/* Get the pipe descriptor for this transfer */
				pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)QTD_ptr->\
						PIPE_DESCR_FOR_THIS_QTD;
				pipe_tr_struct_ptr = (PIPE_TR_STRUCT_PTR)QTD_ptr->\
						TR_FOR_THIS_QTD;

				/* Check for errors */
				if (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK) 
				{
					errors |= (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK);
					QTD_ptr->TOKEN &= ~EHCI_QTD_ERROR_BITS_MASK;
					status = USBERR_TR_FAILED;
				} /* Error */

				/* Check if STALL or endpoint halted because of errors */
				if (QTD_ptr->TOKEN & EHCI_QTD_STATUS_HALTED) 
				{
					errors |= EHCI_QTD_STATUS_HALTED;
					status = USBERR_ENDPOINT_STALLED;
					QTD_ptr->TOKEN &= ~EHCI_QTD_STATUS_HALTED;
					QH_ptr->STATUS = 0;
				} /* Endif */

				if (pipe_descr_ptr->DIRECTION) {
					total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
					buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
				} else {
					total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
					buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
				} /* Endif */
				remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >> 
						EHCI_QTD_LENGTH_BIT_POS);

#ifdef DEBUG_INFO
				printf("_usb_ehci_process_qh_interrupt_tr_complete: Requested Bytes = %d\
						,Remaining bytes = %d,",total_req_bytes,remaining_bytes);
#endif

				if (QTD_ptr->TOKEN & EHCI_QTD_IOC) 
				{
					/* total number of bytes sent/received */
					pipe_descr_ptr->SOFAR += (total_req_bytes - remaining_bytes);
					pipe_tr_struct_ptr->status = USB_STATUS_IDLE;
				} /* Endif */

				temp_QTD_ptr = QTD_ptr;

				QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;

				/* Queue the transfer onto the relevant queue head */
				QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;
				if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT) 
				{
					/* No alternate descriptor */
					QH_ptr->ALT_NEXT_QTD_LINK_PTR |= EHCI_QTD_T_BIT;

					/* Clear the status field */
					QH_ptr->STATUS &= EHCI_QH_TR_OVERLAY_DT_BIT;
				} /* Endif */


				active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)QTD_ptr;

				/* Dequeue the used QTD */
				_usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);

				/* Now mark it as unused so that next request can use it */
				pipe_tr_struct_ptr->TR_INDEX = 0;

				if (pipe_tr_struct_ptr->status == USB_STATUS_IDLE) 
				{
					/* Transfer done. Call the callback function for this 
					 ** transaction if there is one (usually true). 
					 */
					if (pipe_tr_struct_ptr->CALLBACK != NULL) 
					{
						pipe_tr_struct_ptr->CALLBACK((pointer)pipe_descr_ptr, 
								pipe_tr_struct_ptr->CALLBACK_PARAM,
								buffer_ptr,
								(total_req_bytes - remaining_bytes),
								status);

						/* If the application enqueued another request on this pipe 
						 ** in this callback context then it will be at the end of the list
						 */
					} /* Endif */
					errors = 0;
					status = 0;
				} /* Endif */


			} /* Endif */
			else 
			{ /* Body */

				break;
			} /* Endbody */


		} /* EndWhile */

		active_list_member_ptr = 
				active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

	} while (active_list_member_ptr);   /* While there are more queue 
	 ** heads in the active list */ 


} /* Endbody */


#ifdef __USB_OTG__

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_host_delay
 *  Returned Value : None
 *  Comments       :
 *        Delay for specified number of milliseconds.
 *END*-----------------------------------------------------------------*/

void _usb_host_delay
(
		/* [IN] the USB Host state structure */
		_usb_host_handle        handle,

		/* [IN] time to wait in ms */
		uint_32 delay
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR              usb_host_ptr;
	VUSB20_REG_STRUCT _PTR_                dev_ptr;
	uint_32                                start_frame=0,curr_frame=0,diff =0,i=0,j=0;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;   
	dev_ptr      = (VUSB20_REG_STRUCT _PTR_)usb_host_ptr->DEV_PTR;

	/* Get the frame number (not the uframe number */
	start_frame = _usb_ehci_get_frame_number(handle);

	/*wait till frame numer exceeds by delay mili seconds.*/
	do 
	{
		curr_frame = _usb_ehci_get_frame_number(handle);
		i++;
		if(curr_frame != start_frame)  
		{
			diff++;
			start_frame =  curr_frame;
			j++;
		}

	}while(diff < delay);   


#if 0      

	volatile uint_32              delay_count;
	OTG_STRUCT_PTR                otg_reg;
	uint_32                       otg_status;
	uint_32                       curr_toggle_bit =0;
	uint_32                       new_toggle_bit =0;
	volatile VUSB20_REG_STRUCT _PTR_       dev_ptr;
	uint_32     
	USB_OTG_STATE_STRUCT_PTR usb_otg_ptr = (USB_OTG_STATE_STRUCT_PTR)usb_otg_state_struct_ptr;
	int test1[101],test2[101];
	int i=0,j=0;

	otg_reg = usb_otg_ptr->OTG_REG_PTR;
	dev_ptr = usb_otg_ptr->USB_REG_PTR;
	delay_count = delay;

	/*On host we should use Frindex to count time.*/


	/* Forced to use OTG timer because no other timer available */
	/*START_TIMER(otg_reg);*/

	/* Get the status of enabled interrupts */
	/*otg_status = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC;*/

	/*
   check the status of the toggle bit.
   curr_toggle_bit = otg_status & VUSBHS_OTGSC_1MST;
	 */
	/*while (delay_count) 
   {
	 */
	/*
      wait till toggle bit changes
      do 
      {
	 */
	/* Get the status of enabled interrupts */
	/*otg_status = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC;*/

	/*check the status of the toggle bit.*/
	/*new_toggle_bit = otg_status & VUSBHS_OTGSC_1MST;*/

	/*j++;*/

	/*}while(new_toggle_bit == curr_toggle_bit);*/

	/* curr_toggle_bit = new_toggle_bit;*/
	/*test1[i] = curr_toggle_bit;*/


	/*delay_count--;*/


	j++;

	if (otg_status & VUSBHS_OTGSC_1MSIS) 
		/*if (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC & VUSBHS_OTGSC_1MSIS) */
	{
		/* Clear the timer*/
		i++;
		test1[i] = otg_status;
		otg_status |=  VUSBHS_OTGSC_1MSIS;
		dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC = otg_status;
		test2[i] = otg_status;

		/*         otg_status &= ~VUSBHS_OTGSC_1MSIS; */
		/*delay_count--; */
		/*}*/


	}
	/*STOP_TIMER(otg_reg);*/

#endif


} /* Endbody */


#endif

/* EOF */




/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_itd_tr_complete
 *  Returned Value : None
 *  Comments       :
 *     Search the ITD list to see which ITD had finished and 
 *     Process the interrupt.
 *
 *END*-----------------------------------------------------------------*/

static void _usb_ehci_process_itd_tr_complete
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT _PTR_                      dev_ptr;
	EHCI_ITD_STRUCT_PTR                          ITD_ptr;
	PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
	PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
	uint_32                                      total_req_bytes = 0, no_of_scheduled_slots;
	uint_32                                      remaining_bytes = 0;
	uint_32                                      errors = 0, status = 0;
	uchar_ptr                                    buffer_ptr = NULL;
	LIST_NODE_STRUCT_PTR                         node_ptr,prev_node_ptr,next_node_ptr;
	uint_8                                       transaction_number,page_number;
	boolean                                      pending_transaction;
	uint_32                                      offset,length_transmitted;
	uint_32_ptr                                  prev_link_ptr=NULL,next_link_ptr=NULL;


	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = usb_host_ptr->DEV_PTR;


	/******************************************************************
   Search the ITD list starting from head till we find inactive nodes.
   Note that for Head there is no previous node so we can disntiguish
   it from rest of the list.
	 ******************************************************************/
	prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR; 

	/* loop till current node is active or node is a head node*/
	while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
			|| ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
	{

		ITD_ptr =  (EHCI_ITD_STRUCT_PTR) node_ptr->member;
#ifdef  __USB_OTG__

#ifdef HOST_TESTING
		/*
         usb_otg_state_struct_ptr->STATUS_STARTS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                         = ITD_ptr->status;
		 */            
#endif
#endif


		pipe_tr_struct_ptr =  (PIPE_TR_STRUCT_PTR) ITD_ptr->PIPE_TR_DESCR_FOR_THIS_ITD;

		pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR) ITD_ptr->PIPE_DESCR_FOR_THIS_ITD;

		/* assume that no transactions are pending on this ITD */
		pending_transaction = FALSE;

		no_of_scheduled_slots = 0;

		/**************************************************************
      Check the status of every transaction inside the ITD.
		 **************************************************************/
		for(transaction_number=0; transaction_number < 8; transaction_number++)
		{

			/**************************************************************
         Note that each iteration of this loop checks the micro frame 
         number on which transaction is scheduled. If a micro frame was
         not allocated for this pipe, we don't need to check it. But
         caution is that, there could be a transaction that was too
         small and even though many bandwidth slots are available 
         but this transaction was finished only in 1 of the slots. Thus
         we also keep a check of how many transactions were allocated for
         this ITD.
			 **************************************************************/

			if ((pipe_descr_ptr->BWIDTH_SLOTS[transaction_number]) &&
					(no_of_scheduled_slots < ITD_ptr->number_of_transactions))
			{


				no_of_scheduled_slots++;

				status = ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] 
				                                     & EHCI_ITD_STATUS;

				/* if transaction is not active and IOC was set we look in to it else we move on */
				if ((!(status & EHCI_ITD_STATUS_ACTIVE)) &&
						(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & EHCI_ITD_IOC_BIT))
				{

					/* send callback to app with the status*/
					if (pipe_tr_struct_ptr->CALLBACK != NULL) 
					{
						offset =  ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & 
								EHCI_ITD_BUFFER_OFFSET;

						page_number =  (uint_8)((ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & 
								EHCI_ITD_PAGE_NUMBER) >> EHCI_ITD_PG_SELECT_BIT_POS);

						length_transmitted = (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] &
								EHCI_ITD_LENGTH_TRANSMITTED) >> EHCI_ITD_LENGTH_BIT_POS;

						buffer_ptr = (uchar_ptr) ((ITD_ptr->BUFFER_PAGE_PTR_LIST[page_number] &
								EHCI_ITD_BUFFER_POINTER) >> EHCI_ITD_PG_SELECT_BIT_POS);


						pipe_tr_struct_ptr->CALLBACK(
								(pointer)pipe_descr_ptr, 
								pipe_tr_struct_ptr->CALLBACK_PARAM,
								(buffer_ptr+offset),
								length_transmitted,
								status);
					}


				}
				/* if IOC is set and status is active, we have a pending transaction */
				else if ((status & EHCI_ITD_STATUS_ACTIVE) &&
						(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & EHCI_ITD_IOC_BIT))
				{
					/* This means that this ITD has a pending transaction */
					pending_transaction = TRUE;
				}


			}

		}



		/* If this ITD is done with all transactions, time to free it */
		if(!pending_transaction)
		{

			/* if we are freeing a head node, we move node_ptr to next head 
            or else we move normally*/

			if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR)
			{
				/*free this node */
				EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
						usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
						node_ptr);

				prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
			}
			else
			{
				/*save next node */
				next_node_ptr = node_ptr->next;

				/*free current node */
				EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
						usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
						node_ptr);

				/*move to next node now */
				node_ptr = next_node_ptr;                   
				prev_node_ptr = node_ptr->prev;

			}

			/*(*ITD_ptr->prev_data_struct_ptr) = ITD_ptr->next_data_struct_value;*/
#ifdef  __USB_OTG__ 
#ifdef HOST_TESTING
			usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
			                                 = status;

			USB_mem_copy((uchar_ptr)ITD_ptr,
					&usb_otg_state_struct_ptr->LOG_INTERRUPT_ITDS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
					                                              ,8);

			usb_otg_state_struct_ptr->LOG_ITD_COUNT++;
			if(usb_otg_state_struct_ptr->LOG_ITD_COUNT > HOST_LOG_MOD)
				usb_otg_state_struct_ptr->LOG_ITD_COUNT = 0;

#endif
#endif

			/*remove the ITD from periodic list */
			prev_link_ptr = next_link_ptr =  ITD_ptr->frame_list_ptr;
			/*iterate the list while we find valid pointers (1 means invalid pointer) */
			while(!((*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
			{
				/*if a pointer matches our ITD we remove it from list*/
				if(((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint_32) ITD_ptr)
				{
					(*prev_link_ptr) = (uint_32)ITD_ptr->NEXT_LINK_PTR;
					break;
				}

				prev_link_ptr = next_link_ptr;
				next_link_ptr = (uint_32_ptr) ((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
			}



			/* free the ITD used */
			_usb_hci_vusb20_free_ITD((pointer)usb_host_ptr, (pointer)ITD_ptr);

			/* subtract on how many ITDs are pending from this transfer */
			pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

			/* if all ITDS are served free the TR INDEX */
			if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
			{
				/* Mark TR as unused so that next request can use it */
				pipe_tr_struct_ptr->TR_INDEX = 0;

			}

		}
		else
		{
			/* move to next ITD in the list */      
			prev_node_ptr = node_ptr;
			node_ptr = node_ptr->next;

		}



	}/* end while loop */


} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_sitd_tr_complete
 *  Returned Value : None
 *  Comments       :
 *     Search the SITD list to see which SITD had finished and 
 *     Process the interrupt.
 *
 *END*-----------------------------------------------------------------*/
static int counter = 0;
static void _usb_ehci_process_sitd_tr_complete
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT _PTR_             dev_ptr;
	EHCI_SITD_STRUCT_PTR                SITD_ptr;
	PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
	PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
	uint_32                                      status = 0;
	uchar_ptr                                    buffer_ptr = NULL;
	LIST_NODE_STRUCT_PTR                         node_ptr,prev_node_ptr,next_node_ptr;
	uint_32                                      length_transmitted;   
	uint_32_ptr                                  prev_link_ptr=NULL,next_link_ptr=NULL;


	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = usb_host_ptr->DEV_PTR;

	/******************************************************************
   Search the SITD list starting from head till we find inactive nodes.
   Note that for Head there is no previous node so we can disntiguish
   it from rest of the list.
	 ******************************************************************/
	prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR; 

	counter = 0;

	/* loop till current node is active or node is a head node*/
	while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
			|| ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
	{

		if(++counter == 9){
			printf("tick1\n");
		}
		SITD_ptr =  (EHCI_SITD_STRUCT_PTR) node_ptr->member;

		pipe_tr_struct_ptr =  (PIPE_TR_STRUCT_PTR) SITD_ptr->PIPE_TR_DESCR_FOR_THIS_SITD;

		pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR) SITD_ptr->PIPE_DESCR_FOR_THIS_SITD;

		/*grab the status and check it */
		status = SITD_ptr->TRANSFER_STATE & EHCI_SITD_STATUS;

		/* if transaction is not active we look in to it else we move on */
		if(!(status & EHCI_SITD_STATUS_ACTIVE))
		{

			/* send callback to app with the status*/
			if (pipe_tr_struct_ptr->CALLBACK != NULL) 
			{

				length_transmitted = SITD_ptr->TRANSFER_STATE & EHCI_SITD_LENGTH_TRANSMITTED;

				buffer_ptr = (uchar_ptr) (SITD_ptr->BUFFER_PTR_0 & EHCI_SITD_BUFFER_POINTER);


				pipe_tr_struct_ptr->CALLBACK(
						(pointer)pipe_descr_ptr, 
						pipe_tr_struct_ptr->CALLBACK_PARAM,
						buffer_ptr,
						length_transmitted,
						status);
			}


			/*********************************************************************
             Since status is Non active for this SITD, time to delete it.           
			 *********************************************************************/

			/* if we are freeing a head node, we move node_ptr to next head 
            or else we move normally */

			if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR)
			{
				/*free this node */
				/*EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
						node_ptr);*/
				{
					printf("tick2\n");
					if(node_ptr->prev != NULL) {
						node_ptr->prev->next = node_ptr->next;
						node_ptr->prev->next_active = node_ptr->next_active;
						node_ptr->next->prev = node_ptr->prev;
					} else  {
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR = node_ptr->next;
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR->prev = NULL;
					}
					node_ptr->next = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next;
					node_ptr->prev = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR;
					node_ptr->next_active = FALSE;
					node_ptr->member = NULL;
					if(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next != NULL)   {
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next->prev = node_ptr;
					}
					usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next = node_ptr;
					usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next_active = FALSE;

					printf("tick3\n");
				}


				prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
			}
			else
			{
				if((uint_32)usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR->next < 0x1fff0000u){
					printf("");
				}
				printf("else\n");
				/*save next node */
				next_node_ptr = node_ptr->next;

				/*free current node */
				EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
						node_ptr);

				/*move to next node now */
				node_ptr = next_node_ptr;                   
				prev_node_ptr = node_ptr->prev;
				printf("else end\n");
			}

#ifdef  __USB_OTG__ 

#ifdef HOST_TESTING
			usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
			                                 = status;

			USB_mem_copy((uchar_ptr)SITD_ptr,
					&usb_otg_state_struct_ptr->LOG_INTERRUPT_SITDS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
					                                               ,44);

			usb_otg_state_struct_ptr->LOG_SITD_COUNT++;
			if(usb_otg_state_struct_ptr->LOG_SITD_COUNT > HOST_LOG_MOD)
				usb_otg_state_struct_ptr->LOG_SITD_COUNT = 0;

#endif
#endif

			/*remove the SITD from periodic list */
			prev_link_ptr = next_link_ptr =  SITD_ptr->frame_list_ptr;
			/*iterate the list while we find valid pointers (1 means invalid pointer) */
			while(!((*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
			{
				/*if a pointer matches our SITD we remove it from list*/
				if(((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint_32) SITD_ptr)
				{
					(*prev_link_ptr) = (uint_32)SITD_ptr->NEXT_LINK_PTR;
					break;
				}

				prev_link_ptr = next_link_ptr;
				next_link_ptr = (uint_32_ptr) ((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
			}



			/* free the SITD used */
			_usb_hci_vusb20_free_SITD((pointer)usb_host_ptr, (pointer)SITD_ptr);

			/* subtract on how many SITDs are pending from this transfer */
			pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

			/* if all SITDS are served free the TR INDEX */
			if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
			{
				/* Mark TR as unused so that next request can use it */
				pipe_tr_struct_ptr->TR_INDEX = 0;

			}



		}
		/* else move on to the next node in the queue */
		else
		{

			prev_node_ptr = node_ptr;
			node_ptr = node_ptr->next;

		}


#if 0            
		/* save the next node */
		next_node_ptr =  

				/*free this node */
				EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
						usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
						node_ptr);

		/* subtract on how many ITDs are pending from this transfer */
		pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

		/* if all ITDS are served free the TR INDEX */
		if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
		{
			/* Mark TR as unused so that next request can use it */
			pipe_tr_struct_ptr->TR_INDEX = 0;

		}

		/* if we just freed a head node, we must update our list head */
		if(node_ptr->prev == NULL)
		{
			prev_node_ptr = node_ptr = next_node_ptr;
		}
		else /* we freed some non head node so we move on normally*/
		{
			/* move to next ITD in the list */      
			node_ptr = next_node_ptr;
			prev_node_ptr = node_ptr->prev;

		}

#endif

	}/* end while loop */


} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_open_pipe
 *  Returned Value : USB Status
 *  Comments       :
 *        When opening a pipe, this callback ensures for iso / int pipes
 *        to allocate bandwidth.
 *END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_open_pipe
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr
)
{ /* Body */
	USB_STATUS error;
	if ((pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE) || ((pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE))) {
		/* Call the low-level routine to send a setup packet */
		error = _usb_host_alloc_bandwidth_call_interface (handle, pipe_descr_ptr);

		if (error != USB_OK){
			return USBERR_BANDWIDTH_ALLOC_FAILED;
		} /* Endif */
	} /* Endif */
	return USB_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_update_dev_address
 *  Returned Value : USB Status
 *  Comments       :
 *        Update the queue in the EHCI hardware Asynchronous schedule list
 *        to reflect new device address.
 *END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_update_dev_address
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr
)
{ /* Body */

	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	//VUSB20_REG_STRUCT_PTR                        dev_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr = NULL, QH_prev_ptr = NULL;
	uint_32                                      saved_link;
	boolean                                      active_async;

	//   return _usb_ehci_free_resources(handle, pipe_descr_ptr);

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
	//dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

	/* Get the pointer to the member of the active list */
	if (usb_host_ptr->ACTIVE_ASYNC_LIST_PTR != NULL)
		QH_prev_ptr = QH_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR;
	else
		return USBERR_BAD_STATUS;

	while ((QH_ptr->HORIZ_LINK_PTR & EHCI_QH_HORIZ_PHY_ADDRESS_MASK) != (uint_32) usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR) {
		QH_prev_ptr = QH_ptr;
		QH_ptr = (pointer) (QH_ptr->HORIZ_LINK_PTR & EHCI_QH_HORIZ_PHY_ADDRESS_MASK);
		if (QH_ptr == pipe_descr_ptr->QH_FOR_THIS_PIPE)
			break;
	} /* EndWhile */

	/* Queue head is already on the active list. Simply update the queue */
	active_async = USBHS_USBCMD & USBHS_USBCMD_ASE_MASK;
	/* stop async schedule */
	USBHS_USBCMD &= ~USBHS_USBCMD_ASE_MASK;
	saved_link = QH_ptr->HORIZ_LINK_PTR;
	//   QH_prev_ptr->HORIZ_LINK_PTR = QH_ptr->HORIZ_LINK_PTR; /* unlink QH */
	_usb_ehci_init_Q_HEAD(handle, pipe_descr_ptr, QH_ptr, QH_prev_ptr, (pointer) QH_ptr->NEXT_QTD_LINK_PTR);
	QH_ptr->HORIZ_LINK_PTR = saved_link;

	if (active_async)
		USBHS_USBCMD |= USBHS_USBCMD_ASE_MASK;

	return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_ehci_process_timer
 *  Returned Value : USB Status
 *  Comments       :
 *        Updates Asynchronous list QTDs timer information. Removes QTDs
 *        with timeout.
 *END*-----------------------------------------------------------------*/
void _usb_ehci_process_timer
(
		/* [IN] the USB Host state structure */
		_usb_host_handle                 handle
)
{
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	EHCI_QH_STRUCT_PTR                           QTD_ptr;
	ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	for (active_list_member = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR; 
			active_list_member != NULL; 
			active_list_member = active_list_member->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR)
	{
		if (active_list_member->TIME > 0)
			if (!--active_list_member->TIME)
				_usb_ehci_process_qh_list_tr_complete(handle, usb_host_ptr->ACTIVE_ASYNC_LIST_PTR);
	}
}
/*
void EHCI_ACTIVE_QUEUE_ADD_NODE(
		LIST_NODE_STRUCT_PTR tail,
		EHCI_ITD_STRUCT_PTR member_ptr
		){
   if (tail->prev != NULL) {
       tail->prev->next_active = TRUE;
   }
   tail->member = (pointer) member_ptr;
   tail->next_active = FALSE;
   tail = tail->next;
}
 */
void EHCI_QUEUE_FREE_NODE(
		LIST_NODE_STRUCT_PTR head,
		LIST_NODE_STRUCT_PTR tail,
		LIST_NODE_STRUCT_PTR node_ptr
){
	if(node_ptr->prev != NULL) {
		node_ptr->prev->next = node_ptr->next;
		node_ptr->prev->next_active = node_ptr->next_active;
		node_ptr->next->prev = node_ptr->prev;
	} else  {
		head = node_ptr->next;
		head->prev = NULL;
	}
	node_ptr->next = tail->next;
	node_ptr->prev = tail;
	node_ptr->next_active = FALSE;
	node_ptr->member = NULL;
	if(tail->next != NULL)   {
		tail->next->prev = node_ptr;
	}
	tail->next = node_ptr;
	tail->next_active = FALSE;
}
/*
void EHCI_SITD_QADD(
		EHCI_SITD_STRUCT_PTR head, 
		EHCI_SITD_STRUCT_PTR tail, 
		EHCI_SITD_STRUCT_PTR SITD
){
	if ((head) == NULL) {
		(head) = (SITD);
	} else {
		(tail)->SCRATCH_PTR = (void *) (SITD);
	}
	(tail) = (pointer) (SITD);
	(SITD)->SCRATCH_PTR = NULL;
}
 */
#endif	// HIGH_SPEED_DEVICE

/* EOF */

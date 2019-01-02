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
 * $FileName: ehci_cncl.c$
 * $Version : 3.4.4.0$
 * $Date    : Sep-10-2009$
 *
 * Comments:
 *
 *   This file contains the low-level Host API function specific to the 
 *   VUSB chip to cancel a transfer
 *
 *END************************************************************************/
#include "hostapi.h"
#include "ehci_usbprv_host.h"
#include "ehci_cncl.h"
#include "ehci_host.h"
#include "user_config.h"

#if HIGH_SPEED_DEVICE

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_hci_vusb20_cancel_transfer
 *  Returned Value : None
 *  Comments       :
 *        Cancel a transfer
 *END*-----------------------------------------------------------------*/

USB_STATUS _usb_hci_vusb20_cancel_transfer
(
		/* [IN] the USB Host state structure */
		_usb_host_handle              handle,

		/* The pipe descriptor to queue */            
		PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

		/* [IN] the transfer parameters struct */
		PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{ /* Body */
	USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
	EHCI_QH_STRUCT_PTR                           QH_ptr;
	EHCI_QTD_STRUCT_PTR                          QTD_ptr, temp_QTD_ptr, prev_QTD_ptr, start_QTD_ptr;
	ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
	uint_32                                      cmd_val, temp=0;

	usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

	/* Cancel the transaction at hardware level if required */
	if ((pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE) || (pipe_descr_ptr->PIPETYPE == USB_BULK_PIPE))
	{
		/* Get the head of the active queue head */
		active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;

		/* Asynchronous */
		while (active_list_member_ptr) {
			/* Get the first QTD for this Queue head */
			QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;

			if (!QTD_ptr) {
				break;
			} /* Endif */

			/**************************************************************************
This code has a shortcoming. All the relevant QTDs should be freed for TR
and not just one QTD. Currently this will work in most cases because 1 QTD
really can handle large transfers (16K recommended though 20K possible,see
ehci specs on QTD structure for details). However, it should be taken care
of in a future release.

S Garg 08/19/2003
			 **************************************************************************/


			if (QTD_ptr->PIPE_DESCR_FOR_THIS_QTD == pipe_descr_ptr) {
				/* Set Asynch_Enable bit = 0 */
				USBHS_USBCMD &= ~USBHS_USBCMD_ASE_MASK;

				temp_QTD_ptr = QTD_ptr;
				QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;

				/* Dequeue the used QTD */
				_usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);

				/* Get the queue head from the active list */
				QH_ptr = active_list_member_ptr->QH_PTR;

				/* Queue the transfer onto the relevant queue head */
				QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;

				/* Clear all error conditions */
				temp = QH_ptr->STATUS;
				QH_ptr->STATUS = (temp & EHCI_QH_TR_OVERLAY_DT_BIT);

				/* Enable the Asynchronous schedule if:
				 ** if asynchronous_schedule_enable == asynchronous_schedule_status
				 */
				cmd_val = USBHS_USBCMD;
				if ((cmd_val & USBHS_USBCMD_ASE_MASK) ==
						(USBHS_USBSTS & USBHS_USBSTS_AS_MASK)) 
				{
					USBHS_USBCMD = (cmd_val | USBHS_USBCMD_ASE_MASK);
				} /* Endif */
				break;
			} /* Endif */

			active_list_member_ptr = 
					active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

		} /* EndWhile */

	} /* Endif */

	/****************************************************************************
   SGARG: Add the ability to cancel the transfers for the interrupt pipe. Note that
   interrupts QHs are in the periodic list and they must be unlinked from all
   the possible frame lists that are linked to them.   
	 *****************************************************************************/

	else if (pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE) 
	{
		/* Get the head of the active interrupt queue heads */
		active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;

		/***********************************************************************
         Loop through the list of all active QHs on Interrupt pipes and cancel the
         transfer which is requested to cancel. Note that this list of QH called,
         usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR is created when a send_data
         is called on interrupt pipe.
		 ************************************************************************/

		while (active_list_member_ptr) 
		{

			/* Get the first QTD for this Queue head */
			QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;

			/* Get the queue head from the active list */
			QH_ptr = active_list_member_ptr->QH_PTR;

			/*******************************************************************
               If this QH belongs to the pipe we are looking for , we can proceed.
			 ********************************************************************/
			if ((QH_ptr->PIPE_DESCR_FOR_THIS_QH == (pointer) pipe_descr_ptr) &&
					(QTD_ptr != NULL))
			{
				/* Now we can disable the QTD list from this QH */
				QH_ptr->NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;

				/*******************************************************************
                     Loop through all QTDs for this QH starting from first one and cancel
                     all of those who belong to this TR. Make sure we relink rest of them
                     so that their transfers can proceed.
				 ********************************************************************/

				/* start pointer points to the first QTD in the final list*/
				start_QTD_ptr = NULL;

				/* previous pointer points to NULL in the beginning */
				prev_QTD_ptr =  NULL;

				do 
				{
					/*******************************************************************
                           If this QTD belongs to the TR that generated it, it must be cancelled.
					 ********************************************************************/

					if (QTD_ptr->TR_FOR_THIS_QTD == (pointer)current_pipe_tr_struct_ptr)

					{
						/* if list already started we connect previous QTD with next one*/
						if(prev_QTD_ptr != NULL) 
						{
							prev_QTD_ptr->NEXT_QTD_PTR = QTD_ptr->NEXT_QTD_PTR;
						}

						/* if list already started we link previous pointer*/
						temp_QTD_ptr = QTD_ptr;

						/* advance the QTD pointer */
						QTD_ptr = (EHCI_QTD_STRUCT_PTR) QTD_ptr->NEXT_QTD_PTR;

						/* Dequeue the used QTD */
						_usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);


					} /* Endif QTD_ptr->TR_FOR_THIS_QTD  */


					/******************************************************************
					 *******************************************************************/

					else 
					{
						/***************************************************************
                              If start pointer is not initialized we should do it once only.
						 ***************************************************************/

						if(start_QTD_ptr == NULL)
						{
							/* Initialize the start pointer */
							start_QTD_ptr =  QTD_ptr;
						}

						/* store the previous qtd pointer */
						prev_QTD_ptr = QTD_ptr;

						/* advance the QTD pointer */
						QTD_ptr = (EHCI_QTD_STRUCT_PTR) QTD_ptr->NEXT_QTD_PTR;

					}


				} while(!((uint_32)QTD_ptr & EHCI_QTD_T_BIT));


				/*******************************************************************
                     Our work on this QH has ended so we must put the new list back to
                     the periodic schedule immidiately.
				 ********************************************************************/

				if(start_QTD_ptr != NULL) 
				{
					/* Queue the transfer onto the relevant queue head */
					QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)start_QTD_ptr;
				}

				active_list_member_ptr->FIRST_QTD_PTR = start_QTD_ptr;



				/* Clear all error conditions */
				temp = QH_ptr->STATUS;
				QH_ptr->STATUS = temp & EHCI_QH_TR_OVERLAY_DT_BIT;


			}/*end if (QTD_ptr->PIPE_DESCR_FOR_THIS_QTD*/

			active_list_member_ptr = 
					active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;


		} /* EndWhile */

	} /*end if (pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE) */
	/* Use interrupt on asynch advance doorbell */

	return USB_OK;
} /* EndBody */

#endif

/* EOF */

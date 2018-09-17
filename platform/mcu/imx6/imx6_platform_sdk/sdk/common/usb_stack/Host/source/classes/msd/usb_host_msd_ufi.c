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
* $FileName: usb_host_msd_ufi.c$
* $Version : 3.0.4.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   This file creates the mass storage class UFI command headers
*
*END************************************************************************/

#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_ufi_generic
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_mass_ufi_generic
   (
      /* [IN] command object allocated by application*/
      COMMAND_OBJECT_PTR         cmd_ptr,
      uint_8                     opcode,
      uint_8                     lun,
      uint_32                    lbaddr,
      uint_32                    blen,

      uint_8                     cbwflags,

      uchar_ptr                  buf,
      uint_32                    buf_len
   )
{ /* Body */
   USB_STATUS                    status = USB_OK;
   UFI_CBWCB_EXTENDED_STRUCT_PTR ufi_ptr = NULL;

   ufi_ptr = (pointer) &(cmd_ptr->CBW_PTR->CMD_BLOCK);

   /* Construct UFI command buffer */
   ufi_ptr->BUFIOPCODE = opcode;
   ufi_ptr->BUFILUN = lun;
   HOST_WRITE_BEOCT_32(ufi_ptr->BUFILOGICALBLOCKADDRESS, lbaddr);
   HOST_WRITE_BEOCT_32(ufi_ptr->BLENGTH, blen);

   /* Construct CBW fields (sig and tag will be filled up by class driver)*/
   htou32(cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH, buf_len);
   cmd_ptr->CBW_PTR->BMCBWFLAGS = cbwflags;
   TRANSFER_LOW_NIBBLE(cmd_ptr->LUN, cmd_ptr->CBW_PTR->BCBWLUN);
   TRANSFER_LOW_NIBBLE(sizeof(UFI_CBWCB_STRUCT),
      cmd_ptr->CBW_PTR->BCBWCBLENGTH);

   /* Construct Command object */
   cmd_ptr->DATA_BUFFER = buf;
   cmd_ptr->BUFFER_LEN = buf_len;

   /* Pass this request to class driver */
   return usb_class_mass_storage_device_command(cmd_ptr->CALL_PTR, cmd_ptr);
} /* Endbody */

boolean usb_mass_ufi_cancel
   (
      COMMAND_OBJECT_PTR         cmd_ptr   )
{ /* Body */
   return (uint_8)usb_class_mass_storage_device_command(cmd_ptr->CALL_PTR, cmd_ptr);
} /* Endbody */
/* EOF */

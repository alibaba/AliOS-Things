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
* $FileName: usb_host_msd_queue.c$
* $Version : 3.0.5.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   Implements the queue system for the mass storage class.
*
*END************************************************************************/
#include "types.h"

#include "usb_host_msd_bo.h"
#define MASSQ_NEXT(index)   (uint_8)( index ==(QUEUE_SIZE - 1)?0:index+1)

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_q_init
* Returned Value : None
* Comments       :
*     This function initializes a mass storage class queue.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_q_init
   (
      /* [IN] interface structure pointer */
      USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr
   )
{ /* Body */
   memset((MASS_QUEUE_STRUCT_PTR)&intf_ptr->QUEUE, 0, sizeof(intf_ptr->QUEUE));
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_q_insert
* Returned Value : Position at which insertion took place in the queue.
* Comments       :
*     This function is called by class driver for inserting a command in the
*     queue.
*END*--------------------------------------------------------------------*/

int_32 usb_class_mass_q_insert
   (
      /* [IN] interface structure pointer */
      USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr,

      /* [IN] command object to be inserted in the queue*/
      COMMAND_OBJECT_PTR pCmd
   )
{ /* Body */
   MASS_QUEUE_STRUCT_PTR   Q =  (MASS_QUEUE_STRUCT_PTR)&intf_ptr->QUEUE;
   int_32                  tmp = -1;

   /*
   ** Insert into queue, update LAST, check if full and return queue position.
   ** If queue is full -1 will be returned
   */
   USB_lock();
   if (Q->COUNT < QUEUE_SIZE) {
      Q->ELEMENTS[Q->LAST] = pCmd;
      tmp = Q->LAST;
      Q->LAST = MASSQ_NEXT(Q->LAST);
      Q->COUNT++;
   } /* Endif */
   USB_unlock();

   return tmp;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_deleteq
* Returned Value : None
* Comments       :
*     This routine deletes the pending request in the queue
*END*--------------------------------------------------------------------*/

void usb_class_mass_deleteq
   (
      /* [IN] interface structure pointer */
      USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr
   )
{ /* Body */
   MASS_QUEUE_STRUCT_PTR   Q =  (MASS_QUEUE_STRUCT_PTR)&intf_ptr->QUEUE;

   /* Remove current command and increment FIRST modulo the Q size */
   USB_lock();
   if (Q->COUNT) {
      Q->ELEMENTS[Q->FIRST] = NULL;
      Q->FIRST = MASSQ_NEXT(Q->FIRST);
      Q->COUNT--;
   }
   USB_unlock();
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_get_pending_request
* Returned Value : None
* Comments       :
*     This routine fetches the pointer to the first (pending) request in
*     the queue, or NULL if there is no pending requests.
*END*--------------------------------------------------------------------*/

void usb_class_mass_get_pending_request
   (
      /* [IN] interface structure pointer */
      USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr,

      /* [OUT] pointer to pointer which will hold the pending request */
      COMMAND_OBJECT_PTR _PTR_         cmd_ptr_ptr
   )
{ /* Body */
   MASS_QUEUE_STRUCT_PTR   Q = (MASS_QUEUE_STRUCT_PTR)&intf_ptr->QUEUE;

   USB_lock();
   if (Q->COUNT) {
      *cmd_ptr_ptr = (COMMAND_OBJECT_PTR)Q->ELEMENTS[Q->FIRST];
   } else {
      *cmd_ptr_ptr = NULL;
   } /* Endif */
   USB_unlock();

   return;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_cancelq
* Returned Value : None
* Comments       :
*     This routine cancels the given request in the queue
*END*--------------------------------------------------------------------*/

boolean usb_class_mass_cancelq
   (
      /* [IN] interface structure pointer */
      USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr,

      /* [IN] command object to be inserted in the queue*/
      COMMAND_OBJECT_PTR pCmd
   )
{ /* Body */
   MASS_QUEUE_STRUCT_PTR   Q =  (MASS_QUEUE_STRUCT_PTR)&intf_ptr->QUEUE;
   uint_32                 i,index;
   boolean                 result = FALSE;
   
   /* Remove given command - leave q size the same*/
   USB_lock();
   if (Q->COUNT) {
      index = Q->FIRST;
      for (i=0;i<Q->COUNT;i++) {
         if (Q->ELEMENTS[index] == pCmd) {
            Q->ELEMENTS[index] = NULL;
            result = TRUE;
            //printf("\nMASS q entry cancelled!!!" );
            break;
         }   
         index = MASSQ_NEXT(index);
      }
   }
   USB_unlock();
 
  
   return result;
} /* Endbody */

/* EOF */

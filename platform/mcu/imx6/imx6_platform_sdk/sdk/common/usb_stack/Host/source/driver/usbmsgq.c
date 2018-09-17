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
* $FileName: usbmsg.c$
* $Version : 0.0.0.1$
* $Date    : Nov-18-2009$
*
* Comments:
*
*   
*
*
*END************************************************************************/

#include "usbmsgq.h"
#include "psptypes.h"
#include "khci.h"

USB_MSGQ_ELEMENT element_array[10];
uint_32 index;

extern USBMSGQ_STRUCT tr_que;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_msgq_init
* Returned Value   :
* Comments         : Initialize message queue. Allocate message pointer. Make queue in QUEUE_EMPTY status.
*    
*
*END*----------------------------------------------------------------------*/
uint_32 _usb_msgq_init
  (
      /* Number of messages created after the header */
      uint_32 num_messages,

      /* The size of the messages in _mqx_max_type's */
      uint_32 msg_size
  )
{
  /* Set head pointer and tail pointer are NULL */
  tr_que.HEAD = NULL;
  tr_que.TAIL = NULL;
  
  /* set information for queue */
  tr_que.QUEUE_STATUS = USB_MSGQ_EMPTY;
  tr_que.SIZE = msg_size;
  tr_que.MAX_SIZE = num_messages;
  tr_que.CURRENT_SIZE = 0;
  index = 0;
  
  return USB_MSGQ_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_msgq_send
* Returned Value   :
* Comments         : Send a message to the message pool.
*    
*
*END*----------------------------------------------------------------------*/
uint_32 _usb_msgq_send
  (
      /* Number of messages created after the header */
      int*   message
  )
{
  USB_MSGQ_ELEMENT_PTR new_element_ptr;
 
  int *to_ptr;
  int *from_ptr;
  int i;
  
   /* check status of queue */
  if (USB_MSGQ_FULL == tr_que.QUEUE_STATUS)
  {
    return USB_MSGQ_SEND_ERROR;
  }
 
  index++;
  if(10 == index) index = 0;
  new_element_ptr = &element_array[index];
  
  
  /* copy data from message to new element */
  to_ptr = (int *)(&new_element_ptr->MESSAGE_DATA);
  from_ptr = message;
  /* copy data */
  i = (int)tr_que.SIZE+1;
  while (--i) 
  {
     *to_ptr++ = *from_ptr++;
  } /* Endwhile */

  /* Link new element to link list */
  /* Check queue status */
  if (USB_MSGQ_EMPTY == tr_que.QUEUE_STATUS){ /* this is the first element */
    /*
        HEAD   new_element
       ___________________
       |NULL|   1   |NULL|
       |____|_______|____|

        TAIL          
     */
    new_element_ptr->NEXT = NULL;
    new_element_ptr->PREV = NULL;
    tr_que.HEAD = new_element_ptr; 
    tr_que.TAIL = new_element_ptr;
  } 
  else 
  { /* this is NOT the first element */
    
    new_element_ptr->NEXT = NULL;
    /*
                                        new_element
       ___________________      ___________________
    -->|n-1 |   n   |NULL|      |  n |  n+1  |NULL|
    <--|____|_______|____|      |____|_______|____|
        TAIL                                          */
        
    new_element_ptr->PREV = tr_que.TAIL;
    /*
                                        new_element
       ___________________      ___________________
    -->|n-1 |   n   |NULL|      |  n |  n+1  |NULL|
    <--|____|_______|____|<-----|____|_______|____|
        TAIL                                          */
                                            
    tr_que.TAIL->NEXT = new_element_ptr;
    /*                                  new_element
       ___________________      ___________________
    -->|n-1 |   n   |n+1 |----->|  n |  n+1  |NULL|
    <--|____|_______|____|<-----|____|_______|____|
        TAIL
                                                  */
    tr_que.TAIL = new_element_ptr;
    /*                                  new_element
       ___________________      ___________________
    -->|n-1 |   n   |n+1 |----->|  n |  n+1  |NULL|
    <--|____|_______|____|<-----|____|_______|____|
                                 TAIL
                                                  */
  }
  
  /* update new current size */
  tr_que.CURRENT_SIZE++;
  
   /* Set status of queue after send an element */
   if (tr_que.CURRENT_SIZE == tr_que.MAX_SIZE)
   {
     tr_que.QUEUE_STATUS = USB_MSGQ_FULL;
   }
   else
   {
     tr_que.QUEUE_STATUS = USB_MSGQ_VALID;
   }

   return USB_MSGQ_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_msgq_receive
* Returned Value   :
* Comments         : Receive a message from the message pool.
*    
*
*END*----------------------------------------------------------------------*/
uint_32 _usb_msgq_receive
  (
      /* Number of messages created after the header */
      int*   message
  )
{
  USB_MSGQ_ELEMENT_PTR receive_element_ptr;
  int *to_ptr;
  int *from_ptr;
  int i;
 
  /* check status of queue */
  if (USB_MSGQ_EMPTY == tr_que.QUEUE_STATUS)
  {
    return USB_MSGQ_RECEIVE_ERROR;
  }
  
  /* Get receive element in the link list (it is the first element) */
  receive_element_ptr = tr_que.HEAD;
  
  /* Copy data from receive element to message */
  /* Set address of source and destination */
  to_ptr = message;
  from_ptr = (int *)(&receive_element_ptr->MESSAGE_DATA);
  /* copy data */
  i = (int)(tr_que.SIZE+1);
  while (--i) 
  {
     *to_ptr++ = *from_ptr++;
  } /* Endwhile */
  
  /* Remove receive element from link list */
  /* Check queue status */
  if (1 == tr_que.CURRENT_SIZE)
  { /* there are only 1 element in queue */
    /*
        HEAD   
       ___________________
       |NULL|   1   |NULL|
       |____|_______|____|

        TAIL          
     */
      tr_que.HEAD = NULL;
      tr_que.TAIL = NULL;
    
  } 
  else 
  {
    /*
        HEAD receive_element            
       ___________________      ___________________
       |NULL|   1   | 2  |----->|  1 |   2   | 3  |--->
       |____|_______|____|<-----|____|_______|____|<---

     */
     tr_que.HEAD = receive_element_ptr->NEXT; 
    /*
       receive_element           HEAD   
       ___________________      ___________________
       |NULL|   1   | 2  |----->|  1 |   2   | 3  |--->
       |____|_______|____|<-----|____|_______|____|<---

     */
     tr_que.HEAD->PREV = NULL;
    /*
       receive_element           HEAD   
       ___________________      ___________________
       |NULL|   1   | 2  |----->|NULL|   2   | 3  |--->
       |____|_______|____|<--x--|____|_______|____|<---

     */
  }
  
  /* Free receive element */
  tr_que.CURRENT_SIZE--;
  /* update index of last element */
  if (0 == tr_que.CURRENT_SIZE)
  {
    tr_que.QUEUE_STATUS = USB_MSGQ_EMPTY;
  }
  else 
  {
    tr_que.QUEUE_STATUS = USB_MSGQ_VALID;
  }
 
  return USB_MSGQ_OK;
}

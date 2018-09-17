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
* $FileName: usbevent.c$
* $Version : 0.0.0.1$
* $Date    : Nov-18-2009$
*
* Comments:
*
*   
*
*
*END************************************************************************/

#include "usbsem.h"
#include "usb.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_sem_create
* Returned Value   :
* Comments         : Create a semaphore pointer, set VALID field to USB_SEM_VALID and VAULE to initial_number
*    
*
*END*----------------------------------------------------------------------*/
uint_16 _usb_sem_create
(
  USB_SEM_STRUCT_PTR sem_ptr,
  int_32               initial_number
)
{
  sem_ptr->VALID = USB_SEM_VALID;
  sem_ptr->VALUE = (uint_32)initial_number;
  return  USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_sem_post
* Returned Value   : 
* Comments         : Increate VALUE field of semaphore pointer.
*    
*
*END*----------------------------------------------------------------------*/
uint_16 _usb_sem_post
(
  USB_SEM_STRUCT_PTR sem_ptr
)
{
    if(sem_ptr->VALID == USB_SEM_VALID) 
    {
      sem_ptr->VALUE++;
      return USB_OK;
    }
    return USB_SEM_INVALID;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_sem_wait
* Returned Value   : 
* Comments         : Wait for a lightweight semaphore.
* 
*
*END*----------------------------------------------------------------------*/
uint_16 _usb_sem_wait
(
  USB_SEM_STRUCT_PTR sem_ptr
)
{
   if(sem_ptr->VALID == USB_SEM_VALID) 
   {
      sem_ptr->VALUE--;
      return USB_OK;
    }
    return USB_SEM_INVALID;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_sem_wait_ticks
* Returned Value   : 
* Comments         :
*    
*
*END*----------------------------------------------------------------------*/
uint_16 _usb_sem_wait_ticks
(
  USB_SEM_STRUCT_PTR sem_ptr,
  uint_32 ticks
)
{
	UNUSED(ticks)
	
   if(sem_ptr->VALID == USB_SEM_VALID) 
   {
      if(sem_ptr->VALUE <= 0)
        return USB_SEM_FREE;
      else 
      {
        sem_ptr->VALUE--;
        return USB_OK;
      }
    }
    return USB_SEM_INVALID;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_sem_wait_ticks
* Returned Value   : 
* Comments         : Set VALID field of semaphore pointer to USB_SEM_INVALID
*    
*
*END*----------------------------------------------------------------------*/
uint_16 _usb_sem_destroy
(
  USB_SEM_STRUCT_PTR sem_ptr
)
{
   if(sem_ptr->VALID == USB_SEM_VALID) 
   {
      sem_ptr->VALUE--;
      return USB_OK;
    }
    return USB_SEM_INVALID;
}


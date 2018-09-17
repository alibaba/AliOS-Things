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
* $FileName: msd_commands.c$
* $Version : 3.4.21.0$
* $Date    : Sep-15-2009$
*
* Comments:
*
*   This file contains device driver for mass storage class. This code tests
*   the UFI set of commands.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include "types.h"
#include "hostapi.h"
#include "derivative.h" /* include peripheral declarations */
#include "host_common.h"
#include "usb.h"
#include "usb_bsp.h"
#include "khci.h"
#include "usbevent.h"
#include "msd_commands.h"
#include "sci.h"
#include "rtc.h"
#include "poll.h"
#include "hidef.h"
#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"
#include "usb_host_hub_sm.h"
#include "mem_util.h"

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif


/**************************************************************************
   Global variables
**************************************************************************/
volatile DEVICE_STRUCT       mass_device = { 0 };   /* mass storage device struct */
volatile boolean bCallBack = FALSE;
volatile USB_STATUS bStatus       = USB_OK;
volatile uint_32  dBuffer_length = 0;
boolean  Attach_Notification = FALSE;
static uint_32 test = 0, ontest;
//_usb_host_handle        host_handle;         /* global handle for calling host   */


/* the following is the mass storage class driver object structure. This is
used to send commands down to  the class driver. See the Class API document
for details */
COMMAND_OBJECT_STRUCT pCmd;

/*some handles for communicating with USB stack */
uchar buff_in[BUFF_IN_SIZE], buff_out[0x0F];
MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO  read_capacity;
static uint_32 start_lba  = 0;
static uint_32 lba_length = 0;

void Main_Task(void);

const USB_HOST_DRIVER_INFO DriverInfoTable[] = {
   /* Floppy drive */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {

      {0x49,0x0D},                  /* Vendor ID per USB-IF             */
      {0x00,0x30},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 1.1 hub */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_LS,          /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },

   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /*    driver info list.             */
      0,
      0,
      0,
      0,
      NULL
   }

};

uint_8 disk_ioctl 
(
  /* [IN] Drive number */
  uint_8 Drive,      
  /* [IN] Control command code */
  uint_8 Command, 
  /* [IN/OUT] Parameter and data buffer */   
  void* Buff
);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
#ifdef __GNUC__
int main(void)
#else
void main(void) 
#endif
{
  USB_STATUS           status = USB_OK;
  _usb_host_handle     host_handle;

  /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
  _bsp_platform_init();
#ifdef MCU_MK70F12
  sci2_init();
#else
  sci1_init();
#endif
    
  TimerInit();
  
  /* Init polling global variable */
  POLL_init(); 

  DisableInterrupts;
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
  usb_int_dis();
  #endif  
  status = _usb_host_init
    (HOST_CONTROLLER_NUMBER,    /* Use value in header file */
     MAX_FRAME_SIZE,            /* Frame size per USB spec  */
     &host_handle);             /* Returned pointer */

  if (status != USB_OK) 
  {
    printf("\nUSB Host Initialization failed. STATUS: 0x%x", (unsigned int) status);    
  } /* Endif */

   /*
   ** Since we are going to act as the host driver, register
   ** the driver information for wanted class/subclass/protocols
   */
  status = _usb_host_driver_info_register(host_handle, (void *)DriverInfoTable);
  if (status != USB_OK) 
  {
     printf("\nDriver Registration failed. STATUS: 0x%x", (unsigned int)status);      
  }
   
  EnableInterrupts;
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
  usb_int_en();
  #endif
   
  pCmd.CBW_PTR = (CBW_STRUCT_PTR)USB_mem_alloc_word_aligned(sizeof(CBW_STRUCT));  
  if (pCmd.CBW_PTR == NULL)
  {
     printf ("\nUnable to allocate Command Block Wrapper!");     
  }
  memset(pCmd.CBW_PTR, 0, sizeof(CBW_STRUCT));
   
  pCmd.CSW_PTR = (CSW_STRUCT_PTR)USB_mem_alloc_word_aligned(sizeof(CSW_STRUCT));   
  if (pCmd.CSW_PTR == NULL)
  {
     printf ("\nUnable to allocate Command Status Wrapper!");     
  }    
  memset(pCmd.CSW_PTR, 0, sizeof(CSW_STRUCT));
  
  printf("\nUSB MSD Command test\nWaiting for USB mass storage to be attached...\n");

  for(;;) 
  {
    Main_Task();
    Poll();
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
#ifdef __GNUC__
   return 0;
#endif
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void Main_Task (void)
{ /* Body */
   USB_STATUS   status = USB_OK;

   /*----------------------------------------------------**
   ** Infinite loop, waiting for events requiring action **
   **----------------------------------------------------*/
   switch (mass_device.dev_state) 
   {
       case USB_DEVICE_IDLE:
           break;
        
       case USB_DEVICE_ATTACHED:
           printf( "Mass Storage Device Attached\n" );
           mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
           status = _usb_hostdev_select_interface(mass_device.dev_handle,
           mass_device.intf_handle, (pointer)&mass_device.class_intf);
           break;
        
       case USB_DEVICE_SET_INTERFACE_STARTED:
           break;
        
       case USB_DEVICE_INTERFACED:
           usb_host_mass_test_storage();
           //mass_device.dev_state = USB_DEVICE_OTHER;
           break;
        
       case USB_DEVICE_DETACHED:
           printf ( "\nMass Storage Device Detached\n" );
           mass_device.dev_state = USB_DEVICE_IDLE;
           break;
        
       case USB_DEVICE_OTHER:
           break;
        
       default:
           printf ( "Unknown Mass Storage Device State = %d\n",\
               (int)mass_device.dev_state );
           break;
   } /* Endswitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       :
*     called when mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32           event_code
   )
{ /* Body */
   INTERFACE_DESCRIPTOR_PTR   intf_ptr =
      (INTERFACE_DESCRIPTOR_PTR)intf_handle;

   switch (event_code) 
   {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT:
         if (mass_device.dev_state == USB_DEVICE_IDLE) 
         {
            mass_device.dev_handle = dev_handle;
            mass_device.intf_handle = intf_handle;
            mass_device.dev_state = USB_DEVICE_ATTACHED;
         } 
         else 
         {
            printf("Mass Storage Device Is Already Attached\n");
         } /* EndIf */
         break;
         
      case USB_INTF_EVENT:
         mass_device.dev_state = USB_DEVICE_INTERFACED;
         break;
         
      case USB_DETACH_EVENT:
         test = 0;
         if (mass_device.dev_state != USB_DEVICE_IDLE) 
         {
            mass_device.dev_handle = NULL;
            mass_device.intf_handle = NULL;
            mass_device.dev_state = USB_DEVICE_DETACHED;
         } 
         else 
         {
            printf("Mass Storage Device Is Not Attached\n");
         } /* EndIf */
         break;
         
      default:
         mass_device.dev_state = USB_DEVICE_IDLE;
         break;
   } /* EndSwitch */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_ctrl_calback
* Returned Value : None
* Comments       :
*     called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/

static void usb_host_mass_ctrl_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      pointer           user_parm,

      /* [IN] buffer address */
      uchar_ptr         buffer,

      /* [IN] length of data transferred */
      uint_32           buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint_32           status
   )
{ /* Body */

    UNUSED(pipe_handle)
    UNUSED(user_parm)
    UNUSED(buffer)
    UNUSED(buflen)
    
   bCallBack = TRUE;
   bStatus = status;

} /* Endbody */


void callback_bulk_pipe
   (
      /* [IN] Status of this command */
      USB_STATUS status,

      /* [IN] pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      pointer p1,

      /* [IN] pointer to the command object*/
      pointer  p2,

      /* [IN] Length of data transmitted */
      uint_32 buffer_length
   )
{ /* Body */

    UNUSED(p1)
    UNUSED(p2)
    
   dBuffer_length = buffer_length;
   bCallBack = TRUE;
   bStatus = status;

} /* Endbody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_test_storage
* Returned Value : None
* Comments       :
*     Calls the UFI command set for testing
*END*--------------------------------------------------------------------*/
/* todo AI: change name and place for following variable */
uint_8 count = 0;
static void usb_host_mass_test_storage
   (
      void
   )
{ /* Body */
   static USB_STATUS                                 status = USB_OK;
   static uint_8                                     bLun = 0;
   static INQUIRY_DATA_FORMAT                        inquiry;
   static MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO read_capacity;
   static CAPACITY_LIST                              capacity_list;
   static REQ_SENSE_DATA_FORMAT                      req_sense;
   static FORMAT_UNIT_PARAMETER_BLOCK                formatunit = { 0};
   static CBW_STRUCT_PTR cbw_ptr; 
   static CSW_STRUCT_PTR csw_ptr;

   uint_8 res;

   switch(test)
   {
    case 0:
      cbw_ptr = pCmd.CBW_PTR;
      csw_ptr = pCmd.CSW_PTR;

      memset(buff_in, 0, BUFF_IN_SIZE);
      memset(buff_out, 0, 0x0F);
      memset(pCmd.CSW_PTR, 0, sizeof(CSW_STRUCT));
      memset(pCmd.CBW_PTR, 0, sizeof(CBW_STRUCT));
      memset(&pCmd, 0, sizeof(COMMAND_OBJECT_STRUCT));
      
      pCmd.CBW_PTR  = cbw_ptr;
      pCmd.CSW_PTR  = csw_ptr;
      pCmd.LUN      = bLun;
      pCmd.CALL_PTR = (pointer)&mass_device.class_intf;
      pCmd.CALLBACK = callback_bulk_pipe;

      printf("\n ================ START OF A NEW SESSION ================");
      test++;
      ontest = 1;
      count = 0;
      break;
      
      
    case 1:
        count++;
        /* Send standard commands once, after each enumeration in order to 
         * communicate well with most usb sticks */
        if(count == 1){
#if 0
            /* Read LUN info (type, and some strings) */
            res = disk_ioctl(0, UFI_INQUIRY, NULL);
            //res = disk_ioctl(0, UFI_READ_FORMAT_CAPACITY, NULL);
            res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
            //res = disk_ioctl(0, UFI_READ_FORMAT_CAPACITY, NULL);
#endif
#if 1
            res = disk_ioctl(0, UFI_INQUIRY, NULL);
            res = disk_ioctl(0, UFI_TEST_UNIT_READY, NULL);
            //res = disk_ioctl(0, UFI_READ_CAPACITY, NULL);
            printf("");
#endif
        }
        
      /* Test the GET MAX LUN command */
      if (1 == ontest)
      {
        printf("\n Testing: GET MAX LUN Command");
        bCallBack = FALSE;

        status = usb_class_mass_getmaxlun_bulkonly(
          (pointer)&mass_device.class_intf, &bLun,
          usb_host_mass_ctrl_callback);
          ontest = 0;
      } 
      else 
      {
        if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
        {
          printf (".......................ERROR");
          return;
        }
        else
        {
          /* Wait till command comes back */
          if (!bCallBack) {break;}
          if (!bStatus) 
          {
             printf("..........................OK\n");
             test++;
             ontest = 1;
          }
          else 
          {
             printf("...Unsupported (bStatus=0x%08x)\n", (unsigned int)bStatus);
             test++;
             ontest = 1;
          }
        }
      }
      break;
      
    case 2:
        if (1 == ontest)
        {
            printf(" Testing: TEST UNIT READY Command");
            bCallBack = FALSE;

            status =  usb_mass_ufi_test_unit_ready(&pCmd);
            ontest = 0;
        } 
        else 
        {
            if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
            {
                printf ("...................ERROR");
                return;
            }
            else
            {
                /* Wait till command comes back */
                if (!bCallBack) {break;}
                if (!bStatus)
                {
                    printf("......................OK\n");
                    test++;
                    ontest = 1;
                }
                else 
                {
                    printf("...Unsupported(bStatus=0x%08x)\n", (unsigned int)bStatus);
                    test++;
                    ontest = 1;
                }
            }
        }
        break;
      
    case 3:
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: INQUIRY Command");
        res = disk_ioctl(0, UFI_INQUIRY, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        time_delay(2000);
        printf(" Testing: READ CAPACITY Command");
        res = disk_ioctl(0, UFI_READ_CAPACITY, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: READ FORMAT CAPACITIES Command");
        res = disk_ioctl(0, UFI_READ_FORMAT_CAPACITY, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: MODE SENSE Command");
        res = disk_ioctl(0, UFI_MODE_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: PREVENT-ALLOW MEDIUM REMOVAL Command");
        res = disk_ioctl(0, UFI_PREVENT_ALLOW_MEDIUM_ROMVAL, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: VERIFY Command");
        res = disk_ioctl(0, UFI_VERIFY, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: WRITE(10) Command");
        res = disk_ioctl(0, UFI_WRITE10, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: REQUEST SENSE Command");
        
        printf(" Testing: REQUEST SENSE Command");
        res = disk_ioctl(0, UFI_REQUEST_SENSE, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: READ(10) Command");
        res = disk_ioctl(0, UFI_READ10, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        
        printf(" Testing: START-STOP UNIT Command");
        res = disk_ioctl(0, UFI_START_STOP, NULL);
        if(!res)
            printf("......................OK\n");
        else
            printf("FAILED(%x)\n\r", res);
        /* ------------------------------------------------------------------ */
        test++;
      break;
      
    case 4:
      printf("\n Test done!\n");
      test++;
      break;
      
    default:
      break;
   } /* End of switch */
   
   /* ------------------------------------------------------------------ */
   if(test == 19)
   {
       
   }

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : disk_ioctl
* Returned Value : 
* Comments       : The disk_ioctl function controls device specified features
*                  and miscellaneous functions other than disk read/write 
*
*END*--------------------------------------------------------------------*/
uint_8 disk_ioctl 
(
  /* [IN] Drive number */
  uint_8 Drive,      
  /* [IN] Control command code */
  uint_8 Command, 
  /* [IN/OUT] Parameter and data buffer */   
  void* Buff
)
{
    uint_32                                     res = USB_OK;
    
    static CAPACITY_LIST                        capacity_list;
    static INQUIRY_DATA_FORMAT                  inquiry;
    static REQ_SENSE_DATA_FORMAT                req_sense;

    UNUSED(Drive)

    if (UFI_TEST_UNIT_READY == Command)
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_test_unit_ready(&pCmd);
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)   
            {
                Poll();
            }

            if (!bStatus) 
            {
                res = USB_OK;
            }
            else 
            {
                res = USBERR_ERROR;
            }         
        }       
    }
    else if(UFI_MODE_SENSE == Command)
    {
        /* Send mode sense SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_mode_sense(&pCmd,
                  2, //PC
                  0x3F, //page code
                  buff_in,
                  (uint_32)0x08);
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else
                res = USBERR_ERROR;      
        }
    }
    else if(UFI_VERIFY == Command)
    {
        /* Send mode sense SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_verify(
                  &pCmd,
                  0x400, // block address
                  1 //length to be verified
                  );
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else
                res = USBERR_ERROR;      
        }
    }
    else if(UFI_WRITE10 == Command)
    {
        /* Send mode sense SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_write_10(&pCmd, start_lba, buff_out, lba_length, 1);
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else
                res = USBERR_ERROR;      
        }
    }
    else if(UFI_READ10 == Command)
    {
        /* Send mode sense SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_read_10(&pCmd, start_lba, buff_in, lba_length, 1);
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else
                res = USBERR_ERROR;      
        }
    }
    else if(UFI_START_STOP == Command)
    {
        /* Send mode sense SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_start_stop(&pCmd, 0, 1);
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else
                res = USBERR_ERROR;      
        }
    }
    else if(UFI_PREVENT_ALLOW_MEDIUM_ROMVAL == Command)
        {
            /* Send mode sense SCSI command */
            bCallBack = FALSE;
            bStatus = usb_mass_ufi_prevent_allow_medium_removal(
                      &pCmd,
                      1 // prevent
                      );
            if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
            {
                res = USBERR_ERROR;
            }
            else
            {
                /* Wait till command comes back */
                while(!bCallBack)
                    Poll();
                if (!bStatus)
                    res = USB_OK;
                else
                    res = USBERR_ERROR;      
            }
        }
    else if (UFI_READ_CAPACITY == Command)
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_read_capacity(&pCmd, (uchar_ptr)&read_capacity,
                sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus){
                start_lba = (HOST_READ_BEOCT_32(read_capacity.BLLBA) + 1) >> 1;
                /* Length of each LBA */
                lba_length = HOST_READ_BEOCT_32(read_capacity.BLENGTH);
                res = USB_OK;
            }
            else{
                res = USBERR_ERROR;
            }
        }       
    }
    else if (UFI_READ_FORMAT_CAPACITY == Command)
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_format_capacity(&pCmd, (uchar_ptr)&read_capacity,\
                sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus) 
                res = USB_OK;
            else 
                res = USBERR_ERROR;      
        }       
    }
    else if (UFI_INQUIRY == Command)
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_inquiry(&pCmd, (uchar_ptr) &inquiry,
                sizeof(INQUIRY_DATA_FORMAT));
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)   
                Poll();
            if (!bStatus) 
                res = USB_OK;
            else
                res = USBERR_ERROR;    
        }       
    }   

    else if (UFI_REQUEST_SENSE == Command)
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_request_sense(&pCmd, &req_sense,
                sizeof(REQ_SENSE_DATA_FORMAT));
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
                Poll();
            if (!bStatus)
                res = USB_OK;
            else 
                res = USBERR_ERROR;
        }
    }

    else if ((GET_SECTOR_COUNT == Command)||(GET_SECTOR_SIZE == Command))
    {
        /* Send read_capacity SCSI command */
        bCallBack = FALSE;
        bStatus = usb_mass_ufi_read_capacity(&pCmd, (uchar_ptr)&read_capacity,\
                sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
        if ((bStatus != USB_OK) && (bStatus != USB_STATUS_TRANSFER_QUEUED))
        {
            res = USBERR_ERROR;
        }
        else
        {
            /* Wait till command comes back */
            while(!bCallBack)
            {
                Poll();
            }
            if (!bStatus)
            {
                res = USB_OK;
            }
            else
            {
                res = USBERR_ERROR;
            }
        }

        if(!Buff)
            return res = USBERR_ERROR;
        
        /* Get number of sectors on the disk (DWORD) */
        if (GET_SECTOR_COUNT == Command)
        {
            *(uint_32 *)Buff = *(uint_32 *)read_capacity.BLLBA +1 ;
        }
        /* Get the sector size in byte */
        else
        {
            *(uint_32 *)Buff = *(uint_32 *)read_capacity.BLENGTH;
        }

    }
    else if (GET_BLOCK_SIZE == Command)
    {
        if(!Buff)
            return res = USBERR_ERROR;
        
        *(uint_32*)Buff = ERASE_BLOCK_SIZE;
        res = USB_OK;
    }
    else if(CTRL_SYNC == Command)
    {
        res = USB_OK;
    }
    else
    {
        res = USBERR_ERROR;
    }

    return res;
}
/* EOF */

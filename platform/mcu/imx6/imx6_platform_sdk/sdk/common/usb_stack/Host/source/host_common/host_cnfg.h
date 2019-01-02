#ifndef __usb_cnfg_h__
#define __usb_cnfg_h__

#ifdef USBCFG_KHCI

    #define __USB_HOST_KHCI__

    /*
    ** device specific configuration
    */
    #ifndef USBCFG_MAX_PIPES
    #define USBCFG_MAX_PIPES                        (8)
    #endif

    #ifndef USBCFG_MAX_INTERFACE
    #define USBCFG_MAX_INTERFACE                    (1)
    #endif

    #ifndef USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS
    #define USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS  (0)
    #endif


    /* max msg count for KHCI */
    #ifndef USBCFG_KHCI_TR_QUE_MSG_CNT
    #define USBCFG_KHCI_TR_QUE_MSG_CNT              (10)
    #endif

    /* max interrupt transaction count */
    #ifndef USBCFG_KHCI_MAX_INT_TR
    #define USBCFG_KHCI_MAX_INT_TR                  (10)
    #endif

#endif /* USBCFG_KHCI */

#ifdef USBCFG_EHCI
    #define __USB_HOST_EHCI__
#endif /* USBCFG_EHCI */

/*
** default configuration
*/

/* maximum pipes */
#ifndef USBCFG_MAX_PIPES
#if (defined MCU_MK21D5) 
#define  USBCFG_MAX_PIPES                       (10) 
#else
#define  USBCFG_MAX_PIPES                       (24)
#endif
#endif

/* maximum number of usb interfaces */
#ifndef USBCFG_MAX_INTERFACE
#define USBCFG_MAX_INTERFACE                    (1)
#endif

#ifndef USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS
#define USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS  (0)
#endif

#ifndef USBCFG_MAX_DRIVERS
#define USBCFG_MAX_DRIVERS                      (1)
#endif

#ifndef USBCFG_DEFAULT_MAX_NAK_COUNT
#define USBCFG_DEFAULT_MAX_NAK_COUNT            (15)
#endif

#ifndef USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY
#define USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY   (_time_get_ticks_per_sec() * 1)
#endif

#ifndef USBCFG_MFS_LWSEM_TIMEOUT
#define USBCFG_MFS_LWSEM_TIMEOUT                    (_time_get_ticks_per_sec() * 20)
#endif

#ifndef USBCFG_MFS_MAX_RETRIES
#define USBCFG_MFS_MAX_RETRIES                      2
#endif

#endif

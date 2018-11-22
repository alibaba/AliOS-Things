/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef USB_DEVICE_MSD_H
#define USB_DEVICE_MSD_H

#ifdef __cplusplus
 extern "C" {
#endif


/* These headers are included for child class. */
#include "usb_device_endpoints.h"
#include "usb_device_descriptor.h"
#include "usb_device_types.h"
#include "usb_device.h"


#define MSD_VENDOR_ID        0x0951
#define MSD_PRODUCT_ID       0x1605
#define MSD_PRODUCT_RELEASE  0x0200


#if defined   (__GNUC__)        /* GNU Compiler */
	#ifndef __ALIGN_ARRAY
    #define __ALIGN_ARRAY __attribute__ ((aligned (4)))
	#endif /* __ALIGN_ARRAY */
#elif defined   (__CC_ARM)      /* ARM Compiler */
    #ifndef __ALIGN_ARRAY
    #define __ALIGN_ARRAY	__align(4)
	#endif /* __ALIGN_ARRAY */
#elif defined (__ICCARM__)    /* IAR Compiler */
	#ifndef __ALIGN_ARRAY
    #define __ALIGN_ARRAY
	#endif /* __ALIGN_ARRAY */
#endif


/**
 * @brief Initialize msd device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return true is ok, false is false
 */
uint8_t usbd_msd_init(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Deinitialize msd device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return true is ok, false is false
 */
uint8_t usbd_msd_deinit(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Connect the usb msd device, establish disk initialization before really connect the device
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	blocking	blocking if not configured
 * 
 * @return true is ok, false is false
 */
bool usbd_msd_connect(usbd_handle_type_def *pdev, bool blocking);

/**
 * @brief Disconnect the usb msd device.
 * 
 * @param[in]	pdev		point to usb device handler
 * 
 * @return none
 */
void usbd_msd_disconnect(usbd_handle_type_def *pdev);

// MSC Bulk-only Stage
enum stage_e {
    READ_CBW,     // wait a CBW
    ERROR_CBW,    // error
    PROCESS_CBW,  // process a CBW request
    SEND_CSW,     // send a CSW
    WAIT_CSW,     // wait that a CSW has been effectively sent
};

// Bulk-only cbw_t
typedef struct {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataLength;
    uint8_t  Flags;
    uint8_t  LUN;
    uint8_t  CBLength;
    uint8_t  CB[16];
} __packed cbw_t;

// Bulk-only CSW
typedef struct {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t  Status;
} __packed csw_t;

typedef struct {
    //state of the bulk-only state machine
    enum stage_e stage;
    // current cbw_t
    cbw_t cbw;
    // CSW which will be sent
    csw_t csw;
    // addr where will be read or written data
    uint32_t addr;
    // length of a reading or writing
    uint32_t length;
    // memory OK (after a memoryVerify)
    bool memOK;
    // cache in RAM before writing in memory. Useful also to read a block.
    uint8_t * page;
    int BlockSize;
    uint64_t MemorySize;
    uint64_t BlockCount;
} usbd_msd_pri_def;

#ifdef __cplusplus
}
#endif


#endif


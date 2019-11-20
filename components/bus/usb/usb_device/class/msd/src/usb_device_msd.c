/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stdint.h"
#include "usb_device_msd.h"
#include "usb_device_os.h"
#include <assert.h>

#define DISK_OK         0x00
#define NO_INIT         0x01
#define NO_DISK         0x02
#define WRITE_PROTECT   0x04

#define CBW_Signature   0x43425355
#define CSW_Signature   0x53425355

// SCSI Commands
#define TEST_UNIT_READY            0x00
#define REQUEST_SENSE              0x03
#define FORMAT_UNIT                0x04
#define INQUIRY                    0x12
#define MODE_SELECT6               0x15
#define MODE_SENSE6                0x1A
#define START_STOP_UNIT            0x1B
#define MEDIA_REMOVAL              0x1E
#define READ_FORMAT_CAPACITIES     0x23
#define READ_CAPACITY              0x25
#define READ10                     0x28
#define WRITE10                    0x2A
#define VERIFY10                   0x2F
#define READ12                     0xA8
#define WRITE12                    0xAA
#define MODE_SELECT10              0x55
#define MODE_SENSE10               0x5A

// MSC class specific requests
#define MSC_REQUEST_RESET          0xFF
#define MSC_REQUEST_GET_MAX_LUN    0xFE

#define DEFAULT_CONFIGURATION (1)

// max packet size
#define MAX_PACKET  MAX_PACKET_SIZE_EPBULK

// CSW Status
enum Status {
    CSW_PASSED,
    CSW_FAILED,
    CSW_ERROR,
};


/*****************************************************************************************************/

__ALIGN_ARRAY uint8_t g_ramBuf[32 * 1024] = {0};
/* function to read a block */
int usbd_msd_disk_read(uint8_t* data, uint64_t block, uint8_t count)
{
	memcpy(data, (uint8_t *)(g_ramBuf + block * 0x200), count* 0x200);
	return 0;
}

/* function to write a block */
int usbd_msd_disk_write(const uint8_t* data, uint64_t block, uint8_t count)
{
	memcpy((uint8_t *)(g_ramBuf + block * 0x200), data, count* 0x200);
	return (USBD_OK);
}

/* function to initialize the memory */
int usbd_msd_disk_initialize()
{
	return 0;
}

/* return the number of blocks */
uint64_t usbd_msd_disk_sectors()
{
	return 64;
}

/* return the memory size */
uint64_t usbd_msd_disk_size()
{
	return 0x200 * 64;
}

/* return the status of the storage chip (0: OK, 1: not initialized, 2: no medium in the drive, 4: write protection) */
int usbd_msd_disk_status()
{
	return 0;
}

/*****************************************************************************************************/

extern uint8_t usbd_msd_setup(usbd_handle_type_def *pdev);
extern bool usbd_msd_ep_bulk_out_callback(usbd_handle_type_def *pdev);
extern bool usbd_msd_ep_bulk_in_callback(usbd_handle_type_def *pdev);
extern void usbd_callback_request_completed(usbd_handle_type_def *pdev, uint8_t * buf, uint32_t length);
extern bool usbd_msd_callback_set_configuration(usbd_handle_type_def *pdev, uint8_t configuration);
extern void usbd_msd_callback_bus_reset(void);
extern bool usbd_msd_write(usbd_handle_type_def *pdev, uint8_t * buf, uint16_t size);
extern bool usbd_msd_info_transfer(usbd_handle_type_def *pdev);
extern uint8_t *usbd_msd_configuration_desc();
extern uint8_t* usbd_msd_device_desc();
extern uint8_t *usbd_msd_string_langid_desc();
extern uint8_t *usbd_msd_string_manufacturer_desc();
extern uint8_t *usbd_msd_string_serial_desc();
extern uint8_t *usbd_msd_string_product_desc();
extern uint8_t *usbd_msd_string_serial_desc();
extern uint8_t *usbd_msd_string_configuration_desc();
extern uint8_t *usbd_msd_string_interface_desc();
extern void usbd_msd_reset(usbd_handle_type_def *pdev);



usbd_class_type_def MSD_CLASS = 
{
  usbd_msd_init,
  usbd_msd_deinit,
  usbd_msd_setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  usbd_msd_ep_bulk_in_callback,
  usbd_msd_ep_bulk_out_callback,
  NULL, /*SOF */ 
  usbd_callback_request_completed,
  usbd_msd_callback_bus_reset,
  usbd_msd_configuration_desc,
  usbd_msd_callback_set_configuration,
  NULL,
  NULL,
  NULL
};

usbd_descriptors_type_def MSD_DESC = {
  usbd_msd_device_desc,
  usbd_msd_string_langid_desc, 
  usbd_msd_string_manufacturer_desc,
  usbd_msd_string_product_desc,
  usbd_msd_string_serial_desc,
  usbd_msd_string_configuration_desc,
  usbd_msd_string_interface_desc,
};

void usbd_callback_request_completed(usbd_handle_type_def *pdev, uint8_t * buf, uint32_t length)
{
}

uint8_t usbd_msd_init(usbd_handle_type_def *pdev, void *data)
{
    int16_t ret = 0;
    pdev->pClassData = (usbd_msd_pri_def *)usbd_malloc(sizeof(usbd_msd_pri_def));
    if(pdev->pClassData == NULL) {
        return -1; 
    }

    ((usbd_msd_pri_def *)pdev->pClassData)->stage = READ_CBW;
    memset((void *)&(((usbd_msd_pri_def *)pdev->pClassData)->cbw), 0, sizeof(cbw_t));
    memset((void *)&(((usbd_msd_pri_def *)pdev->pClassData)->csw), 0, sizeof(csw_t));
   ((usbd_msd_pri_def *)pdev->pClassData)->page = NULL;

    usbd_msd_connect(pdev, true);

    return 0;
}

uint8_t usbd_msd_deinit(usbd_handle_type_def *pdev, void *data)
{
    usbd_msd_disconnect(pdev);
    usbd_free(pdev->pClassData);
    return 0;
}

// Called in ISR context to process a class specific request
uint8_t usbd_msd_setup(usbd_handle_type_def *pdev)
{
    bool success = false;
    control_transfer_t *transfer = &pdev->transfer;
    static uint8_t maxLUN[1] = {0};

    if (transfer->setup.bmRequestType.Type == CLASS_TYPE) {
        switch (transfer->setup.bRequest) {
            case MSC_REQUEST_RESET:
                usbd_msd_reset(pdev);
                success = true;
                break;
            case MSC_REQUEST_GET_MAX_LUN:
                transfer->remaining = 1;
                transfer->ptr = maxLUN;
                transfer->direction = DEVICE_TO_HOST;
                success = true;
                break;
            default:
                break;
        }
    }

    return success;
}

bool usbd_msd_connect(usbd_handle_type_def *pdev, bool blocking)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    /* flash read and write */
    //disk initialization
    if (usbd_msd_disk_status() & NO_INIT) {
        if (usbd_msd_disk_initialize()) {
            return false;
        }
    }

    // get number of blocks
    msd_pri->BlockCount = usbd_msd_disk_sectors();

    // get memory size
    msd_pri->MemorySize = usbd_msd_disk_size();

    if (msd_pri->BlockCount > 0) {
        msd_pri->BlockSize = msd_pri->MemorySize / msd_pri->BlockCount;
        if (msd_pri->BlockSize != 0) {
            usbd_free(msd_pri->page);
            msd_pri->page = (uint8_t *)usbd_malloc(msd_pri->BlockSize * sizeof(uint8_t));
            if (msd_pri->page == NULL)
                return false;
        }
    } else {
        return false;
    }

    //connect the device
    usbd_connect(pdev, blocking);
    return true;
}

void usbd_msd_disconnect(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    usbd_disconnect(pdev);
    //De-allocate MSD page size:
    usbd_free(msd_pri->page);
    msd_pri->page = NULL;
}

void usbd_msd_reset(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    msd_pri->stage = READ_CBW;
    
}

void usbd_msd_callback_bus_reset(void)
{
}

// Called in ISR context called when a data is received
bool usbd_msd_ep_bulk_out_callback(usbd_handle_type_def *pdev)
{
    uint32_t size = 0;
    uint8_t buf[MAX_PACKET_SIZE_EPBULK];
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    usbd_read_ep(pdev, EPBULK_OUT, buf, &size, MAX_PACKET_SIZE_EPBULK);
    switch (msd_pri->stage) {
            // the device has to decode the CBW received
        case READ_CBW:
            usbd_msd_cbw_decode(pdev, buf, size);
            break;

            // the device has to receive data from the host
        case PROCESS_CBW:
            switch (msd_pri->cbw.CB[0]) {
                case WRITE10:
                case WRITE12:
                    usbd_msd_memory_write(pdev, buf, size);
                    break;
                case VERIFY10:
                    usbd_msd_memory_verify(pdev, buf, size);
                    break;
            }
            break;

            // an error has occured: stall endpoint and send CSW
        default:
            usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
            msd_pri->csw.Status = CSW_ERROR;
            usbd_msd_send_csw(pdev);
            break;
    }

    //reactivate readings on the OUT bulk endpoint
    usbd_read_start(pdev, EPBULK_OUT, MAX_PACKET_SIZE_EPBULK);
    return true;
}

// Called in ISR context when a data has been transferred
bool usbd_msd_ep_bulk_in_callback(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    switch (msd_pri->stage) {

            // the device has to send data to the host
        case PROCESS_CBW:
            switch (msd_pri->cbw.CB[0]) {
                case READ10:
                case READ12:
                    usbd_msd_memory_read(pdev);
                    break;
            }
            break;

            //the device has to send a CSW
        case SEND_CSW:
            usbd_msd_send_csw(pdev);
            break;

        // the host has received the CSW -> we wait a CBW
        case WAIT_CSW:
            msd_pri->stage = READ_CBW;
            break;

        // an error has occured
        default:            
            usbd_hal_stall_endpoint(pdev, EPBULK_IN);
            usbd_msd_send_csw(pdev);
            break;
    }
    return true;
}

void usbd_msd_memory_write(usbd_handle_type_def *pdev, uint8_t * buf, uint16_t size)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    if ((msd_pri->addr + size) > msd_pri->MemorySize) {
        size = msd_pri->MemorySize - msd_pri->addr;
        msd_pri->stage = ERROR_CBW;
        usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
    }

    // we fill an array in RAM of 1 block before writing it in memory
    for (int i = 0; i < size; i++)
        msd_pri->page[msd_pri->addr % msd_pri->BlockSize + i] = buf[i];

    // if the array is filled, write it in memory
    if (!((msd_pri->addr + size) % msd_pri->BlockSize)) {
        if (!(usbd_msd_disk_status() & WRITE_PROTECT)) {
            usbd_msd_disk_write(msd_pri->page, msd_pri->addr / msd_pri->BlockSize, 1);
        }
    }

    msd_pri->addr += size;
    msd_pri->length -= size;
    msd_pri->csw.DataResidue -= size;

    if ((!msd_pri->length) || (msd_pri->stage != PROCESS_CBW)) {
        msd_pri->csw.Status = (msd_pri->stage == ERROR_CBW) ? CSW_FAILED : CSW_PASSED;
        usbd_msd_send_csw(pdev);
    }
}

void usbd_msd_memory_verify(usbd_handle_type_def *pdev, uint8_t * buf, uint16_t size)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;
    uint32_t n;

    if ((msd_pri->addr + size) > msd_pri->MemorySize) {
        size = msd_pri->MemorySize - msd_pri->addr;
        msd_pri->stage = ERROR_CBW;
        usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
    }

    // beginning of a new block -> load a whole block in RAM
    if (!(msd_pri->addr % msd_pri->BlockSize))
        usbd_msd_disk_read(msd_pri->page, msd_pri->addr / msd_pri->BlockSize, 1);

    // info are in RAM -> no need to re-read memory
    for (n = 0; n < size; n++) {
        if (msd_pri->page[msd_pri->addr % msd_pri->BlockSize + n] != buf[n]) {
            msd_pri->memOK = false;
            break;
        }
    }

    msd_pri->addr += size;
    msd_pri->length -= size;
    msd_pri->csw.DataResidue -= size;

    if ( !msd_pri->length || (msd_pri->stage != PROCESS_CBW)) {
        msd_pri->csw.Status = (msd_pri->memOK && (msd_pri->stage == PROCESS_CBW)) ? CSW_PASSED : CSW_FAILED;
        usbd_msd_send_csw(pdev);
    }
}

bool usbd_msd_inquiry_request(usbd_handle_type_def *pdev)
{
    static uint8_t inquiry[] = { 0x00, 0x80, 0x00, 0x01,
                                36 - 4, 0x80, 0x00, 0x00,
                                'A', 'B', 'C', ' ', ' ', ' ', ' ', ' ',
                                'A', 'B', 'C', ' ', ' ', 'U', 'S', 'B',
                                ' ', 'D', 'I', 'S', 'K', ' ', ' ', ' ',
                                '1', '.', '0', ' ',
                                };

    if (!usbd_msd_write(pdev, inquiry, sizeof(inquiry))) {
        return false;
    }
    return true;
}

static uint8_t usb_format_capacity[12] = {0};

bool usbd_msd_read_format_capacity(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    usb_format_capacity[0] = 0x00;
    usb_format_capacity[1] = 0x00;
    usb_format_capacity[2] = 0x00;
    usb_format_capacity[3] = 0x08;
    usb_format_capacity[4] = (uint8_t)((msd_pri->BlockCount >> 24) & 0xff);
    usb_format_capacity[5] = (uint8_t)((msd_pri->BlockCount >> 16) & 0xff);
    usb_format_capacity[6] = (uint8_t)((msd_pri->BlockCount >> 8) & 0xff);
    usb_format_capacity[7] = (uint8_t)((msd_pri->BlockCount >> 0) & 0xff);
    usb_format_capacity[8] = 0x02;
    usb_format_capacity[9] = (uint8_t)((msd_pri->BlockSize >> 16) & 0xff);
    usb_format_capacity[10] = (uint8_t)((msd_pri->BlockSize >> 8) & 0xff);
    usb_format_capacity[11] = (uint8_t)((msd_pri->BlockSize >> 0) & 0xff);

    if (!usbd_msd_write(pdev, usb_format_capacity, sizeof(usb_format_capacity))) {
        return false;
    }
    return true;
}

static uint8_t usb_capacity[8] = {0};

bool usbd_msd_read_capacity(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    usb_capacity[0] = (uint8_t)(((msd_pri->BlockCount - 1) >> 24) & 0xff);
    usb_capacity[1] = (uint8_t)(((msd_pri->BlockCount - 1) >> 16) & 0xff);
    usb_capacity[2] = (uint8_t)(((msd_pri->BlockCount - 1) >> 8) & 0xff);
    usb_capacity[3] = (uint8_t)(((msd_pri->BlockCount - 1) >> 0) & 0xff);
    usb_capacity[4] = (uint8_t)((msd_pri->BlockSize >> 24) & 0xff);
    usb_capacity[5] = (uint8_t)((msd_pri->BlockSize >> 16) & 0xff);
    usb_capacity[6] = (uint8_t)((msd_pri->BlockSize >> 8) & 0xff);
    usb_capacity[7] = (uint8_t)((msd_pri->BlockSize >> 0) & 0xff);

    if (!usbd_msd_write(pdev, usb_capacity, sizeof(usb_capacity))) {
        return false;
    }
    return true;
}

bool usbd_msd_write(usbd_handle_type_def *pdev, uint8_t * buf, uint16_t size)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    if (size >= msd_pri->cbw.DataLength) {
        size = msd_pri->cbw.DataLength;
    }
    msd_pri->stage = SEND_CSW;

    if (!usbd_write_nb(pdev, EPBULK_IN, buf, size, MAX_PACKET_SIZE_EPBULK)) {
        return false;
    }

    msd_pri->csw.DataResidue -= size;
    msd_pri->csw.Status = CSW_PASSED;
    return true;
}


bool usbd_msd_mode_sense6(usbd_handle_type_def *pdev)
{
    static uint8_t sense6[] = { 0x03, 0x00, 0x00, 0x00 };
    if (!usbd_msd_write(pdev, sense6, sizeof(sense6))) {
        return false;
    }
    return true;
}

void usbd_msd_send_csw(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    msd_pri->csw.Signature = CSW_Signature;
	usbd_write_nb(pdev, EPBULK_IN, (uint8_t *)&(msd_pri->csw), sizeof(csw_t), MAX_PACKET_SIZE_EPBULK);
    msd_pri->stage = WAIT_CSW;
}

bool usbd_msd_request_sense(usbd_handle_type_def *pdev)
{
    static uint8_t request_sense[] = {
        0x70,
        0x00,
        0x05,   // Sense Key: illegal request
        0x00,
        0x00,
        0x00,
        0x00,
        0x0A,
        0x00,
        0x00,
        0x00,
        0x00,
        0x30,
        0x01,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    if (!usbd_msd_write(pdev, request_sense, sizeof(request_sense))) {
        return false;
    }

    return true;
}

void usbd_msd_fail(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    msd_pri->csw.Status = CSW_FAILED;    
    usbd_msd_send_csw(pdev);
}

void usbd_msd_cbw_decode(usbd_handle_type_def *pdev, uint8_t * buf, uint16_t size)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    if (size == sizeof(msd_pri->cbw)) {
        memcpy((uint8_t *)&(msd_pri->cbw), buf, size);
        if (msd_pri->cbw.Signature == CBW_Signature) {
            msd_pri->csw.Tag = msd_pri->cbw.Tag;
            msd_pri->csw.DataResidue = msd_pri->cbw.DataLength;
            if ((msd_pri->cbw.CBLength <  1) || (msd_pri->cbw.CBLength > 16) ) {
                usbd_msd_fail(pdev);
            } else {
                switch (msd_pri->cbw.CB[0]) {
                    case TEST_UNIT_READY:
                        usbd_msd_test_unit_ready(pdev);
                        break;
                    case REQUEST_SENSE:
                        usbd_msd_request_sense(pdev);
                        break;
                    case INQUIRY:
                        usbd_msd_inquiry_request(pdev);
                        break;
                    case MODE_SENSE6:
                        usbd_msd_mode_sense6(pdev);
                        break;
                    case READ_FORMAT_CAPACITIES:
                        usbd_msd_read_format_capacity(pdev);
                        break;
                    case READ_CAPACITY:
                        usbd_msd_read_capacity(pdev);
                        break;
                    case READ10:
                    case READ12:
                        if (usbd_msd_info_transfer(pdev)) {
                            if ((msd_pri->cbw.Flags & 0x80)) {
                                msd_pri->stage = PROCESS_CBW;
                                usbd_msd_memory_read(pdev);
                            } else {
                                usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
                                msd_pri->csw.Status = CSW_ERROR;
                                usbd_msd_send_csw(pdev);
                            }
                        }
                        break;
                    case WRITE10:
                    case WRITE12:
                        if (usbd_msd_info_transfer(pdev)) {
                            if (!(msd_pri->cbw.Flags & 0x80)) {
                                msd_pri->stage = PROCESS_CBW;
                            } else {
                                usbd_hal_stall_endpoint(pdev, EPBULK_IN);
                                msd_pri->csw.Status = CSW_ERROR;                                
                                usbd_msd_send_csw(pdev);
                            }
                        }
                        break;
                    case VERIFY10:
                        if (!(msd_pri->cbw.CB[1] & 0x02)) {
                            msd_pri->csw.Status = CSW_PASSED;
                            usbd_msd_send_csw(pdev);
                            break;
                        }
                        if (usbd_msd_info_transfer(pdev)) {
                            if (!(msd_pri->cbw.Flags & 0x80)) {
                                msd_pri->stage = PROCESS_CBW;
                                msd_pri->memOK = true;
                            } else {
                                usbd_hal_stall_endpoint(pdev, EPBULK_IN);
                                msd_pri->csw.Status = CSW_ERROR;
                                usbd_msd_send_csw(pdev);
                            }
                        }
                        break;
                    case MEDIA_REMOVAL:
                        msd_pri->csw.Status = CSW_PASSED;
                        usbd_msd_send_csw(pdev);
                        break;
                    default:
                        usbd_msd_fail(pdev);
                        break;
                }
            }
        }
    }
}

void usbd_msd_test_unit_ready(usbd_handle_type_def *pdev)
{
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    if (msd_pri->cbw.DataLength != 0) {
        if ((msd_pri->cbw.Flags & 0x80) != 0) {
            usbd_hal_stall_endpoint(pdev, EPBULK_IN);
        } else {
            usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
        }
    }

    msd_pri->csw.Status = CSW_PASSED;
    usbd_msd_send_csw(pdev);
}


void usbd_msd_memory_read(usbd_handle_type_def *pdev)
{
    uint32_t n;
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    n = (msd_pri->length > MAX_PACKET) ? MAX_PACKET : msd_pri->length;

    if ((msd_pri->addr + n) > msd_pri->MemorySize) {
        n = msd_pri->MemorySize - msd_pri->addr;
        msd_pri->stage = ERROR_CBW;
    }

    // we read an entire block
    if (!(msd_pri->addr % msd_pri->BlockSize)) {
        usbd_msd_disk_read(msd_pri->page, msd_pri->addr / msd_pri->BlockSize, 1);
    }

    // write data which are in RAM
    if (!usbd_write_nb(pdev, EPBULK_IN, &(msd_pri->page[msd_pri->addr % msd_pri->BlockSize]), n, MAX_PACKET_SIZE_EPBULK)) {
    }

    msd_pri->addr += n;
    msd_pri->length -= n;

    msd_pri->csw.DataResidue -= n;

    if ( !msd_pri->length || (msd_pri->stage != PROCESS_CBW)) {
        msd_pri->csw.Status = (msd_pri->stage == PROCESS_CBW) ? CSW_PASSED : CSW_FAILED;
        msd_pri->stage = (msd_pri->stage == PROCESS_CBW) ? SEND_CSW : msd_pri->stage;
    }
}


bool usbd_msd_info_transfer(usbd_handle_type_def *pdev)
{
    uint32_t n;
    usbd_msd_pri_def *msd_pri = (usbd_msd_pri_def *)pdev->pClassData;

    // Logical Block Address of First Block
    n = (msd_pri->cbw.CB[2] << 24) | (msd_pri->cbw.CB[3] << 16)
            | (msd_pri->cbw.CB[4] <<  8) | (msd_pri->cbw.CB[5] <<  0);

    msd_pri->addr = n * msd_pri->BlockSize;

    // Number of Blocks to transfer
    switch (msd_pri->cbw.CB[0]) {
        case READ10:
        case WRITE10:
        case VERIFY10:
            n = (msd_pri->cbw.CB[7] <<  8) | (msd_pri->cbw.CB[8] <<  0);
            break;

        case READ12:
        case WRITE12:
            n = (msd_pri->cbw.CB[6] << 24) | (msd_pri->cbw.CB[7] << 16)
                    | (msd_pri->cbw.CB[8] <<  8) | (msd_pri->cbw.CB[9] <<  0);
            break;
    }

    msd_pri->length = n * msd_pri->BlockSize;

    if (!msd_pri->cbw.DataLength) {              // host requests no data
        msd_pri->csw.Status = CSW_FAILED;        
        usbd_msd_send_csw(pdev);
        return false;
    }

    if (msd_pri->cbw.DataLength != msd_pri->length) {
        if ((msd_pri->cbw.Flags & 0x80) != 0) {
            usbd_hal_stall_endpoint(pdev, EPBULK_IN);
        } else {
            usbd_hal_stall_endpoint(pdev, EPBULK_OUT);
        }

        msd_pri->csw.Status = CSW_FAILED;        
        usbd_msd_send_csw(pdev);
        return false;
    }

    return true;
}

// Called in ISR context
// Set configuration. Return false if the
// configuration is not supported.
bool usbd_msd_callback_set_configuration(usbd_handle_type_def *pdev, uint8_t configuration)
{
    if (configuration != DEFAULT_CONFIGURATION) {
        return false;
    }

    // Configure endpoints > 0
    usbd_add_endpoint(pdev, EPBULK_IN, MAX_PACKET_SIZE_EPBULK);
    usbd_add_endpoint(pdev, EPBULK_OUT, MAX_PACKET_SIZE_EPBULK);

    //activate readings
    usbd_read_start(pdev, EPBULK_OUT, MAX_PACKET_SIZE_EPBULK);
    return true;
}

uint8_t *usbd_msd_string_langid_desc() {
    static const uint8_t stringLangidDescriptor[] = {
        0x04,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        0x09,0x04,          /*bString Lang ID - 0x0409 - English*/
    };
    return (uint8_t *)stringLangidDescriptor;
}

uint8_t *usbd_msd_string_manufacturer_desc() {

    static uint8_t stringImanufacturerDescriptor[] = {
        0x12,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'a',0,'b',0,'c',0,'d',0,'e',0,'f',0,'g',0,'h',0, /*bString iManufacturer*/
    };
    return stringImanufacturerDescriptor;
}

uint8_t *usbd_msd_string_serial_desc() {
    static uint8_t stringIserialDescriptor[] = {
        0x16,                                                           /*bLength*/
        STRING_DESCRIPTOR,                                              /*bDescriptorType 0x03*/
        '0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,    /*bString iSerial - 0123456789*/
    };
    return stringIserialDescriptor;
}

uint8_t *usbd_msd_string_configuration_desc() {
    static uint8_t stringIconfigurationDescriptor[] = {
        0x06,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        '0',0,'1',0,        /*bString iConfiguration - 01*/
    };
    return stringIconfigurationDescriptor;
}

uint8_t *usbd_msd_string_interface_desc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x08,                           //bLength
        STRING_DESCRIPTOR,              //bDescriptorType 0x03
        'M',0,'S',0,'D',0               //bString iInterface - MSD
    };
    return stringIinterfaceDescriptor;
}


uint8_t *usbd_msd_string_product_desc() {
    static uint8_t stringIproductDescriptor[] = {
        0x12,                                           //bLength
        STRING_DESCRIPTOR,                              //bDescriptorType 0x03
        'a',0,'b',0,'c',0,'d',0,' ',0,'M',0,'S',0,'D',0 //bString iProduct
    };
    return stringIproductDescriptor;
}

uint8_t* usbd_msd_device_desc()
{
    static uint8_t deviceDescriptor[] = {
        DEVICE_DESCRIPTOR_LENGTH,       /* bLength */
        DEVICE_DESCRIPTOR,              /* bDescriptorType */
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        (uint8_t)(LSB(MSD_VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(MSD_VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(MSD_PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(MSD_PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(MSD_PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(MSD_PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
    };
    return deviceDescriptor;
}

uint8_t *usbd_msd_configuration_desc() {
    static uint8_t configDescriptor[] = {

        // Configuration 1
        9,      // bLength
        2,      // bDescriptorType
        LSB(9 + 9 + 7 + 7), // wTotalLength
        MSB(9 + 9 + 7 + 7),
        0x01,   // bNumInterfaces
        0x01,   // bConfigurationValue: 0x01 is used to select this configuration
        0x00,   // iConfiguration: no string to describe this configuration
        0xC0,   // bmAttributes
        0x32,    // bMaxPower, device power consumption is 100 mA

        // Interface 0, Alternate Setting 0, MSC Class
        9,      // bLength
        4,      // bDescriptorType
        0x00,   // bInterfaceNumber
        0x00,   // bAlternateSetting
        0x02,   // bNumEndpoints
        0x08,   // bInterfaceClass
        0x06,   // bInterfaceSubClass
        0x50,   // bInterfaceProtocol
        0x04,   // iInterface

        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                          // bLength
        5,                          // bDescriptorType
        PHY_TO_DESC(EPBULK_IN),     // bEndpointAddress
        0x02,                       // bmAttributes (0x02=bulk)
        LSB(MAX_PACKET_SIZE_EPBULK),// wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPBULK),// wMaxPacketSize (MSB)
        0,                          // bInterval

        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                          // bLength
        5,                          // bDescriptorType
        PHY_TO_DESC(EPBULK_OUT),    // bEndpointAddress
        0x02,                       // bmAttributes (0x02=bulk)
        LSB(MAX_PACKET_SIZE_EPBULK),// wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPBULK),// wMaxPacketSize (MSB)
        0                           // bInterval
    };
    return configDescriptor;
}


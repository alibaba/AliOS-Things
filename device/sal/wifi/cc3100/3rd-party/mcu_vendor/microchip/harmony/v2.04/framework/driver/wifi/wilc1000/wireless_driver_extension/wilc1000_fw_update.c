/*******************************************************************************
  WILC1000 firmware update support.

  File Name:
    wilc1000_fw_update.c

  Summary:
    WILC1000 firmware update support.

  Description:
    WILC1000 firmware update support.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
#include "wilc1000_task.h"
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
#include "system/console/src/sys_console_usb_cdc_local.h"
#endif

#ifndef SYS_CONSOLE_BUFFER_DMA_READY
#define SYS_CONSOLE_BUFFER_DMA_READY
#endif

#define READ_REG	0
#define WRITE_REG	1
#define READ_BUFF	2
#define WRITE_BUFF	3
#define RESET		4
#define RECONFIGURE_UART	5

#define USART_CMD_HDR_LENGTH	sizeof(s_cmd_hdr)
#define SPI_TRANSFER_SIZE	512

/** UART COMMAND */
enum nm_usart_event_types {
	USART_PKT_RECEIVED = 0,
	USART_PKT_TRANSMITTED,
	USART_ERROR_ON_RECEPTION,	
};
enum nm_usart_cmd_process_states {
	INIT = 0,
	WAIT_SYNC,
	WAITING,
	COLLECTING_HDR,
	COLLECTING_PAYLOAD,
	PROCESSING,
};

typedef struct
{
    size_t bytesRead;
    volatile bool rdComplete;
	size_t bytesWrite;
    volatile bool wrComplete;
	uint16_t rxRemaining;
	uint16_t rxOffset;
	uint8_t rxBuf[1024];
} consoleData;

static consoleData SYS_CONSOLE_BUFFER_DMA_READY s_consoleData =
{
    .bytesRead = 0,
    .rdComplete = true,
    .bytesWrite = 0,
    .wrComplete = true,
    .rxRemaining = 0,
    .rxOffset = 0
};

typedef struct s_cmd_hdr_t {
	unsigned long cmd;
	unsigned long addr;
	unsigned long val;
}s_cmd_hdr;

#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
static uint16_t s_cmd_recv_buffer[256];
static uint8_t s_tx_buffer[512];
#else /* !SYS_CONSOLE_USB_CDC_INSTANCE */
static uint16_t s_cmd_recv_buffer[8];
static uint8_t s_tx_buffer[16];
#endif /* SYS_CONSOLE_USB_CDC_INSTANCE */
static uint16_t s_payload_buffer[512];
static uint8_t s_command_pending = 0;
static uint8 *s_cmd_buf;
static s_cmd_hdr s_cmd;
static uint16_t s_next_rx_length = 0;
static uint8_t *s_pkt = NULL;
static uint8_t s_prot_status = INIT;
static uint8_t s_new_state = INIT;
static uint8_t s_change_state = 0;
static uint8_t s_reconfig = 0;

extern uint32 nm_read_reg(uint32 u32Addr);
extern sint8 nm_write_reg(uint32 u32Addr, uint32 u32Val);
extern sint8 nm_read_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);
extern sint8 nm_write_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);

#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
/* This external variable is used for closing the preoccupied USB device. */
extern CONS_USB_CDC_DATA consUsbData;

static size_t console_usb_rd_buf_sz;
static size_t console_usb_wr_buf_sz;
static uint8_t *console_usb_rd_buf;
static const uint8_t *console_usb_wr_buf;
static uint8_t SYS_CONSOLE_BUFFER_DMA_READY usb_cdc_rd_buf[SYS_CONSOLE_USB_CDC_READ_BUFFER_SIZE];
static CONS_USB_CDC_DATA s_consoleUsbData =
{
    .deviceHandle = USB_DEVICE_HANDLE_INVALID,
    .cdcInstance = SYS_CONSOLE_USB_CDC_INSTANCE,
    .state = CONSOLE_USB_CDC_STATE_INIT,
    .isConfigured = false,
    .getLineCodingData = {SYS_CONSOLE_USB_CDC_COMM_BAUD_RATE, 0, 0, 8},
    .readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,
    .writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,
    .isReadComplete = true,
    .isWriteComplete = true,
    .rdCallback = NULL,
    .wrCallback = NULL
};

static SYS_CONSOLE_STATUS console_usb_status(void)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;

    if (s_consoleUsbData.isConfigured) {
        if (s_consoleUsbData.overflowFlag)
            status = SYS_CONSOLE_STATUS_ERROR;
        else
            status = (console_usb_rd_buf || console_usb_wr_buf) ? SYS_CONSOLE_STATUS_BUSY : SYS_CONSOLE_STATUS_CONFIGURED;
    }
    return status;
}

static size_t console_usb_read(void *buf, size_t count)
{
    WDRV_ASSERT(count <= sizeof(usb_cdc_rd_buf), "USB console read size too big");
    if (!console_usb_rd_buf) {
        console_usb_rd_buf = buf;
        console_usb_rd_buf_sz = count;
    } else {
        WDRV_ASSERT(false, "USB console read operation error");
    }
    return console_usb_rd_buf_sz;
}

static size_t console_usb_write(const void *buf, size_t count)
{
    if (!console_usb_wr_buf) {
        console_usb_wr_buf = buf;
        console_usb_wr_buf_sz = count;
    } else {
        WDRV_ASSERT(false, "USB console write operation error");
    }
    return console_usb_wr_buf_sz;
}

static void console_usb_register_callback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event)
{
    switch (event) {
    case SYS_CONSOLE_EVENT_READ_COMPLETE:
        s_consoleUsbData.rdCallback = consCallback;
        break;
    case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
        s_consoleUsbData.wrCallback = consCallback;
        break;
    default:
        break;
    }
}

static void console_usb_flush(void)
{
    s_consoleUsbData.overflowFlag = false;
    s_consoleUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    s_consoleUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    s_consoleUsbData.isReadComplete = true;
    s_consoleUsbData.isWriteComplete = true;
}

static void console_usb_reset(void)
{
    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION;
    s_consoleUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    s_consoleUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    s_consoleUsbData.isReadComplete = true;
    s_consoleUsbData.isWriteComplete = true;
}

static USB_DEVICE_CDC_EVENT_RESPONSE usb_device_cdc_event_handler
(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void *pData,
    uintptr_t userData
)
{
    uint16_t *breakData;
    USB_CDC_CONTROL_LINE_STATE *controlLineStateData;
    CONS_USB_CDC_DATA *consUsbDataObject = (CONS_USB_CDC_DATA *)userData;

    switch (event) {
    case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
        /* This means the host wants to set the line coding.
         * This is a control transfer request. Use the
         * USB_DEVICE_ControlReceive() function to receive the
         * data from the host. */
        USB_DEVICE_ControlReceive(consUsbDataObject->deviceHandle,
            &consUsbDataObject->setLineCodingData,
            sizeof(USB_CDC_LINE_CODING));
        break;

    case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
        /* This means the host wants to know the current line
         * coding. This is a control transfer request. Use the
         * USB_DEVICE_ControlSend() function to send the data to
         * host. */
        USB_DEVICE_ControlSend(consUsbDataObject->deviceHandle,
            &consUsbDataObject->getLineCodingData,
            sizeof(USB_CDC_LINE_CODING));
        break;

    case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
        /* This means the host is setting the control line state.
         * Read the control line state. We will accept this request
         * for now. */
        controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
        consUsbDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
        consUsbDataObject->controlLineStateData.carrier = controlLineStateData->carrier;
        USB_DEVICE_ControlStatus(consUsbDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
        break;

    case USB_DEVICE_CDC_EVENT_SEND_BREAK:
        /* This means that the host is requesting that a break of the
         * specified duration be sent. Read the break duration. */
        breakData = (uint16_t *)pData;
        consUsbDataObject->breakData = *breakData;
        break;

    case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
        /* This means that the data write got completed. We can schedule
         * the next read. */
        consUsbDataObject->isWriteComplete = true;
        break;

    case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
        /* This means that the host has sent some data. */
        consUsbDataObject->isReadComplete = true;
		s_consoleData.rxRemaining = ((USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE  *)pData)->length;
		WDRV_ASSERT(sizeof(s_consoleData.rxBuf) >= s_consoleData.rxRemaining, "");
		memcpy(s_consoleData.rxBuf, console_usb_rd_buf, s_consoleData.rxRemaining);
        break;

    case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
        /* This means the GET LINE CODING function data is valid. We don't
         * do much with this data in this demo. */
        break;

    case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
        /* The data stage of the last control transfer is
         * complete. For now we accept all the data. */
        USB_DEVICE_ControlStatus(consUsbDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
        break;

    default:
        break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

static void usb_device_event_handler(USB_DEVICE_EVENT event, void *eventData, uintptr_t context)
{
    uint8_t usbActiveConfiguration;

    switch (event) {
    case USB_DEVICE_EVENT_RESET:
        s_consoleUsbData.isConfigured = false;
        break;

    case USB_DEVICE_EVENT_CONFIGURED:
        usbActiveConfiguration = ((USB_DEVICE_EVENT_DATA_CONFIGURED *)eventData)->configurationValue;
        if (usbActiveConfiguration == 1) {
            /* Register the CDC Device application event handler here. */
            USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0,
                usb_device_cdc_event_handler, (uintptr_t)&s_consoleUsbData);
            s_consoleUsbData.isConfigured = true;
        }
        break;

    default:
        break;
    }
}

static void console_usb_tasks(void)
{
    switch(s_consoleUsbData.state)
    {
        case CONSOLE_USB_CDC_STATE_INIT:
            /* Open the device layer. */
            s_consoleUsbData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0,
                DRV_IO_INTENT_READWRITE );

            /* Keep trying, as lower layers may require more time to get started. */
            if(s_consoleUsbData.deviceHandle == USB_DEVICE_HANDLE_INVALID)
                break;

            /* Register a callback with device layer to get event notification (for end point 0). */
            USB_DEVICE_EventHandlerSet(s_consoleUsbData.deviceHandle, usb_device_event_handler, 0);

            /* Attach the device. */
            USB_DEVICE_Attach (s_consoleUsbData.deviceHandle);

            s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION;
            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION:
            /* Check if the device was configured. */
            if(s_consoleUsbData.isConfigured)
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_READY;
            break;

        case CONSOLE_USB_CDC_STATE_READY:
            if (!s_consoleUsbData.isConfigured) {
                console_usb_reset();
                break;
            }

            if (console_usb_rd_buf && s_consoleUsbData.readTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;
            else if (console_usb_wr_buf && s_consoleUsbData.writeTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
            else if (console_usb_rd_buf && s_consoleUsbData.isReadComplete && s_consoleUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
            else if (console_usb_wr_buf && s_consoleUsbData.isWriteComplete && s_consoleUsbData.writeTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE;
            break;

        case CONSOLE_USB_CDC_STATE_SCHEDULE_READ:
            if (!s_consoleUsbData.isConfigured) {
                console_usb_reset();
                break;
            }

            s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;

            if (console_usb_rd_buf) {
                s_consoleUsbData.isReadComplete = false;
                s_consoleUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                /* Force this read to a multiple of the receive buffer size.
                 * This is a requirement at the USB CDC level. */
                USB_DEVICE_CDC_Read(s_consoleUsbData.cdcInstance, &s_consoleUsbData.readTransferHandle,
                    usb_cdc_rd_buf, SYS_CONSOLE_USB_CDC_READ_BUFFER_SIZE);

                if (s_consoleUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_READY;
            }
            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE:
            if (!s_consoleUsbData.isConfigured) {
                console_usb_reset();
                break;
            }

            if (s_consoleUsbData.isReadComplete) {
                memcpy(console_usb_rd_buf, usb_cdc_rd_buf, console_usb_rd_buf_sz);

                s_consoleUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                if (s_consoleUsbData.rdCallback != NULL)
                    s_consoleUsbData.rdCallback(&console_usb_rd_buf_sz);

                console_usb_rd_buf = NULL;
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_READY;
            } else {
                /* Go to write if there is a write queued. */
                if (console_usb_wr_buf && s_consoleUsbData.writeTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
                else if (console_usb_wr_buf && s_consoleUsbData.isWriteComplete && s_consoleUsbData.writeTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE;
            }
            break;

        case CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE:
            if (!s_consoleUsbData.isConfigured) {
                console_usb_reset();
                break;
            }

            if (console_usb_wr_buf) {
                s_consoleUsbData.isWriteComplete = false;
                s_consoleUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                USB_DEVICE_CDC_RESULT res = USB_DEVICE_CDC_Write(s_consoleUsbData.cdcInstance,
                    &s_consoleUsbData.writeTransferHandle, console_usb_wr_buf, console_usb_wr_buf_sz, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);

                if (res == USB_DEVICE_CDC_RESULT_OK)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
                else
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_OPERATIONAL_ERROR;
            }
            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE:
            if (!s_consoleUsbData.isConfigured) {
                console_usb_reset();
                break;
            }

            /* Check if a character was sent. The isWriteComplete
             * flag gets updated in the CDC event handler. */
            if (s_consoleUsbData.isWriteComplete) {
                s_consoleUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                if (s_consoleUsbData.wrCallback != NULL)
                    s_consoleUsbData.wrCallback((void *)console_usb_wr_buf);

                console_usb_wr_buf = NULL;
                s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_READY;
            } else {
                /* Go to read if there is a read queued. */
                if (console_usb_rd_buf && s_consoleUsbData.readTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;
                else if (console_usb_rd_buf && s_consoleUsbData.isReadComplete && s_consoleUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                    s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
            }
            break;

        case CONSOLE_USB_CDC_STATE_OPERATIONAL_ERROR:
            /* Clear all flags and buffers and attempt to recover. */
            console_usb_flush();
            s_consoleUsbData.state = CONSOLE_USB_CDC_STATE_READY;
            break;

        case CONSOLE_USB_CDC_STATE_CRITICAL_ERROR:
            /* Soft crash. */
            break;

        default:
            break;
    }
}
#endif /* #ifdef SYS_CONSOLE_USB_CDC_INSTANCE */

static void console_task_run(void)
{
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
    console_usb_tasks();
# if DRV_USBFS_DEVICE_SUPPORT
    DRV_USBFS_Tasks(sysObj.drvUSBObject);
# elif DRV_USBHS_DEVICE_SUPPORT
    DRV_USBHS_Tasks(sysObj.drvUSBObject);
# endif
    USB_DEVICE_Tasks(sysObj.usbDevObject0);
#else
    SYS_CONSOLE_Tasks(sysObj.sysConsole0);
#endif
}

static void console_read_cb(void *handle)
{
    size_t *readSize = (size_t *)handle;

    s_consoleData.bytesRead = *readSize;
    s_consoleData.rdComplete = true;
}

static void console_init(void)
{
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
    USB_DEVICE_Detach(consUsbData.deviceHandle);
    USB_DEVICE_Close(consUsbData.deviceHandle);
    SYS_CONSOLE_Deinitialize(sysObj.sysConsole0);

    console_usb_register_callback(console_read_cb, SYS_CONSOLE_EVENT_READ_COMPLETE);
#else
    SYS_CONSOLE_RegisterCallback(SYS_CONSOLE_INDEX_0, console_read_cb, SYS_CONSOLE_EVENT_READ_COMPLETE);
#endif
}

static void wait_till_console_ready(void)
{
	do {
        console_task_run();
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
    } while (console_usb_status() != SYS_CONSOLE_STATUS_CONFIGURED);
#else
    } while (SYS_CONSOLE_Status(sysObj.sysConsole0) != SYS_STATUS_READY);
#endif
}

static void console_read_data(uint16_t size, void *buf)
{
	static uint16_t offset = 0;
	
	wait_till_console_ready();
	s_next_rx_length = size;

	if (buf) {
		s_pkt = buf;
		offset = size;
	} else {
		s_pkt += offset;
		offset += size;
	}

    s_consoleData.rdComplete = false;
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
	if (s_consoleData.rxRemaining >= size) {
		memcpy(s_pkt, s_consoleData.rxBuf + s_consoleData.rxOffset, size);
		s_consoleData.rxOffset += size;
		s_consoleData.rxRemaining -= size;
		s_consoleData.rdComplete = true;
		goto out;
	} else {
		s_consoleData.rxOffset = 0;
		s_consoleData.rxRemaining = 0;
	}
    console_usb_read(s_pkt, size);
#else /* !SYS_CONSOLE_USB_CDC_INSTANCE */
    SYS_CONSOLE_Read(SYS_CONSOLE_INDEX_0, STDIN_FILENO, s_pkt, size);
#endif /* SYS_CONSOLE_USB_CDC_INSTANCE */

    do {
		console_task_run();
	} while (!s_consoleData.rdComplete);
	
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
	if (s_consoleData.rxRemaining >= size) {
		memcpy(s_pkt, s_consoleData.rxBuf + s_consoleData.rxOffset, size);
		s_consoleData.rxOffset += size;
		s_consoleData.rxRemaining -= size;
	} else {
		WDRV_ASSERT(false, "");
	}
out:
#endif /* SYS_CONSOLE_USB_CDC_INSTANCE */

	if (s_change_state) {
		s_prot_status = s_new_state;
		s_change_state = 0;
 	}
}

static void console_write_data(uint16_t size, uint8_t *buf)
{
    wait_till_console_ready();

    s_consoleData.wrComplete = false;
#ifdef SYS_CONSOLE_USB_CDC_INSTANCE
    console_usb_write((const uint8_t *)buf, size);
#else
    SYS_CONSOLE_Write(SYS_CONSOLE_INDEX_0, STDOUT_FILENO, (const char *)buf, size);
#endif

	wait_till_console_ready(); /* wait till write job done */

	if (s_change_state) {
		s_prot_status = s_new_state;
		s_change_state = 0;
 	}

}

static void nm_usart_send_regval(uint8_t *tx_data,uint16_t length)
{
	uint8_t temp,i,j;
	for(i=0,j=(length-1);i<j;i++,j--) {
		temp = tx_data[i];
		tx_data[i] = tx_data[j];
		tx_data[j] = temp;
	}
	console_write_data(length, tx_data);
}

static void nm_usart_protocol_handler(enum nm_usart_event_types event_name)
{
	static uint16_t payload_length = 0;
	uint8 checksum = 0;
	uint8 i;

	switch(s_prot_status) {
	case INIT:
		if((event_name == USART_PKT_RECEIVED) && (s_next_rx_length == 1)) {
			if((s_pkt[0] == 0x12)) {
				s_prot_status = WAIT_SYNC;
				s_cmd_recv_buffer[0] = 0xFF;
				s_tx_buffer[0] = 0x5B;
				console_write_data(1, s_tx_buffer);
				console_read_data(1, s_cmd_recv_buffer);
			}
			else {
				s_tx_buffer[0] = s_pkt[0];
				console_write_data(1, s_tx_buffer);
				console_read_data(1, s_cmd_recv_buffer);
			}
		}
		else {
			s_cmd_recv_buffer[0] = 0xFF;
			s_tx_buffer[0] = 0xEA;
			console_write_data(1, s_tx_buffer);
			console_read_data(1, s_cmd_recv_buffer);
		}
		break;
	case WAIT_SYNC:
		if(event_name == USART_PKT_RECEIVED) {
			if(s_pkt[0] == 0xA5) {
				uint8 * s_cmd_recv_buffer_u8 = (uint8*)&s_cmd_recv_buffer[0];
				s_prot_status = WAITING;
				s_cmd_recv_buffer_u8[4] = 0xFF;
				console_read_data(1, &s_cmd_recv_buffer[2]);
			}
			else if(s_pkt[0] == 0x12) {	//uart identification command
				s_tx_buffer[0] = 0x5B;
				s_cmd_recv_buffer[0] = 0xFF;
				console_write_data(1, s_tx_buffer);
				console_read_data(1, s_cmd_recv_buffer);
			}
			else {
				if(!s_reconfig) {
					s_tx_buffer[0] = 0x5A;
					s_cmd_recv_buffer[0] = 0xFF;
					console_write_data(1, s_tx_buffer);
					console_read_data(1, s_cmd_recv_buffer);
				}
				else {
					console_read_data(1, s_cmd_recv_buffer);
				}
			}
		}
			break;
	case WAITING:
		if(event_name == USART_PKT_RECEIVED) {
			s_prot_status = COLLECTING_HDR;
			s_cmd_buf = s_pkt;
			console_read_data(USART_CMD_HDR_LENGTH - 1, NULL);
		}
		else {
			s_prot_status = WAIT_SYNC;
			s_tx_buffer[0] = 0xEA;
			console_write_data(1, s_tx_buffer);
			console_read_data(1, s_cmd_recv_buffer);
		}
		break;
	case COLLECTING_HDR:
		if(event_name == USART_PKT_RECEIVED) {
			//Verify check sum
			for(i=0;i<(USART_CMD_HDR_LENGTH);i++) {
				checksum ^= *(((uint8_t *)s_cmd_buf)+i);
			}
			if(checksum != 0) {
				s_prot_status = WAIT_SYNC;
				s_cmd_recv_buffer[0] = 0xFF;
				s_tx_buffer[0] = 0x5A;
				console_write_data(1, s_tx_buffer);
				console_read_data(1, s_cmd_recv_buffer);
			}
			else {
				memcpy(&s_cmd, s_cmd_buf, sizeof(s_cmd_hdr));
				//Process the Command.
				if((s_cmd.cmd & 0xFF) == WRITE_BUFF) {
					s_prot_status = COLLECTING_PAYLOAD;
					payload_length = (s_cmd.cmd >> 16) & 0xFFFF;
					s_tx_buffer[0] = 0xAC;
					console_write_data(1, s_tx_buffer);
					console_read_data(payload_length, (uint8_t *)s_payload_buffer);
				} 
				else if((s_cmd.cmd & 0xFF) == WRITE_REG) {
					s_command_pending = 1;
					s_prot_status = PROCESSING;
				}
				else {
					s_command_pending = 1;
					s_change_state = 1;
					s_new_state = PROCESSING;
					s_tx_buffer[0] = 0xAC;
					console_write_data(1, s_tx_buffer);
				}
			}
		}
		else if(event_name == USART_ERROR_ON_RECEPTION) {
			WDRV_ASSERT(false, "Should not come here.");
		}
		break;
	case COLLECTING_PAYLOAD:
		if((event_name == USART_PKT_RECEIVED) && (s_next_rx_length == payload_length)) {
			s_command_pending = 1;
			s_prot_status = PROCESSING;
		}
		else if(event_name == USART_ERROR_ON_RECEPTION) {
			s_prot_status = WAIT_SYNC;
			s_tx_buffer[0] = 0xEA;
			s_cmd_recv_buffer[0] = 0xFF;
			console_write_data(1, s_tx_buffer);
			console_read_data(1, s_cmd_recv_buffer);
		}
		else {
			s_prot_status = WAIT_SYNC;
			s_tx_buffer[0] = 0x5A;
			s_cmd_recv_buffer[0] = 0xFF;
			console_write_data(1, s_tx_buffer);
			console_read_data(1, s_cmd_recv_buffer);
		}
		break;
	default:
		s_prot_status = WAIT_SYNC;		
		break;
	}
}

static sint8 enter_wifi_firmware_download(void)
{
	sint8 ret;
	static uint8_t done_programming = 0;
	
	ret = m2m_wifi_download_mode();
	if(ret != M2M_SUCCESS)
	{
		M2M_ERR("Failed to put the WiFi Chip in download mode\n");
		return M2M_ERR_INIT;
	}

	console_init();
	wait_till_console_ready();
	
	//Program the WiFi chip here

	console_read_data(1, s_cmd_recv_buffer);
	
	while(!done_programming) {

		nm_usart_protocol_handler(USART_PKT_RECEIVED);
		
		if(s_command_pending && (s_prot_status == PROCESSING)) {
			uint32_t temp;
			switch((s_cmd.cmd) & 0xFF) {
				//Forward it to SERCOM0 SPI
				case READ_REG:
					//Transalate it to SPI Read register
					temp = nm_read_reg(s_cmd.addr);
					s_tx_buffer[0] = (uint8)(temp >> 0);
					s_tx_buffer[1] = (uint8)(temp >> 8);
					s_tx_buffer[2] = (uint8)(temp >> 16);
					s_tx_buffer[3] = (uint8)(temp >> 24);
					s_prot_status = WAIT_SYNC;
					nm_usart_send_regval(&s_tx_buffer[0],sizeof(uint32_t));
					console_read_data(1, s_cmd_recv_buffer);
					break;
				case WRITE_REG:
					//Transalate it to SPI Write register
					nm_write_reg(s_cmd.addr,s_cmd.val);
					s_tx_buffer[0] = 0xAC;
					s_prot_status = WAIT_SYNC;
					console_write_data(1, s_tx_buffer);
					console_read_data(1, s_cmd_recv_buffer);
					break;
				case READ_BUFF:
				//Transalate it to SPI Read buffer
					nm_read_block(s_cmd.addr, (uint8 *)&s_payload_buffer[0],((s_cmd.cmd >> 16) & 0xFFFF));
					s_prot_status = WAIT_SYNC;
					console_write_data(((s_cmd.cmd >> 16) & 0xFFFF), (uint8_t *)s_payload_buffer);
					console_read_data(1, s_cmd_recv_buffer);
					break;
				case WRITE_BUFF:
					//Transalate it to SPI Write buffer
					nm_write_block(s_cmd.addr, (uint8 *)&s_payload_buffer[0],((s_cmd.cmd >> 16) & 0xFFFF));
					s_tx_buffer[0] = 0xAC;
					s_prot_status = WAIT_SYNC;
					console_write_data(1, s_tx_buffer);
					console_read_data(1, s_cmd_recv_buffer);
					break;
				case RECONFIGURE_UART:
					// Send the ack back
					s_prot_status = WAIT_SYNC;
					s_reconfig = 1;
					console_read_data(1, s_cmd_recv_buffer);
					break;
				case 10:
					s_prot_status = WAIT_SYNC;
					console_write_data(1, s_tx_buffer);
					console_read_data(1, s_cmd_recv_buffer);
					WDRV_ASSERT(false, "Should not come here.");
					break;
				default:
					break;
			}
			s_command_pending = 0;
		}
	}
	return ret;
}

void wilc1000_fw_update(uint32_t pContext)
{
	wilc1000_init();
	enter_wifi_firmware_download();
}


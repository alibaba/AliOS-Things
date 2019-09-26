/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef USB_DEVICE_H
#define USB_DEVICE_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "usb_device_types.h"


/* usb device status */
typedef enum {
    USBD_OK   = 0,
    USBD_BUSY,
    USBD_FAIL,
} usbd_stat_t;

/* usb deivce class type structure */
typedef struct _usbd_class_type_def
{
	uint8_t  (*Init)             (struct _usbd_handle_type_def *pdev, void *data);
	uint8_t  (*DeInit)           (struct _usbd_handle_type_def *pdev, void *data);
	/* Control Endpoints*/
	uint8_t  (*callback_request) (struct _usbd_handle_type_def *pdev);
	uint8_t  (*EP0_TxSent)       (struct _usbd_handle_type_def *pdev );
	uint8_t  (*EP0_RxReady)      (struct _usbd_handle_type_def *pdev );
	/* Class Specific Endpoints*/
	uint8_t  (*DataIn)           (struct _usbd_handle_type_def *pdev);
	uint8_t  (*DataOut)          (struct _usbd_handle_type_def *pdev);
	uint8_t  (*SOF)              (struct _usbd_handle_type_def *pdev);
	void     (*callback_request_completed)(struct _usbd_handle_type_def *pdev, uint8_t * buf, uint32_t length);
	void     (*usbd_callback_bus_reset)(void);

	uint8_t *(*get_config_descriptor)(void);
	bool     (*set_configuration)(struct _usbd_handle_type_def *pdev, uint8_t configuration);
	bool     (*set_interface)(uint16_t interface, uint8_t alternate);
	uint8_t *(*report_desc)(void);  
	uint16_t (*report_desc_length)(void);
} usbd_class_type_def;

/* usb deivce descriptors structure */
typedef struct
{
	uint8_t *(*get_device_descriptor)(void);
	uint8_t *(*get_langid_str_descriptor)(void);
	uint8_t *(*get_manufacturer_str_descriptor)(void);
	uint8_t *(*get_product_str_descriptor)(void);
	uint8_t *(*get_serial_str_descriptor)(void);
	uint8_t *(*get_configuration_str_descriptor)(void);
	uint8_t *(*get_interface_str_descriptor)(void);
} usbd_descriptors_type_def;

/* usb device handle structure */
typedef struct _usbd_handle_type_def
{
	usbd_info_t                device_info;
	control_transfer_t         transfer;
	usbd_descriptors_type_def *pDesc;
	usbd_class_type_def       *pClass;
	void                      *pClassData;
	void                      *pUserData;
	void                      *pHalData;    
	void                      *pData;    
} usbd_handle_type_def;

/******************************************************************************************/

/**
 * @brief Initialize usb device stack
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]   pdesc	point to usb device descriptor handler
 * 
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_init(usbd_handle_type_def *pdev, usbd_descriptors_type_def *pdesc);

/**
 * @brief Connect a usb device
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]   blocking	block if not configured
 * 
 * @return none
 */
void usbd_connect(usbd_handle_type_def *pdev, bool blocking);

/**
 * @brief Disconnect a usb device
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return none
 */
void usbd_disconnect(usbd_handle_type_def *pdev);

/**
 * @brief Add an endpoint
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint endpoint which will be added
 * @param[in]	maxPacket	maxPacket Maximum size of a packet which can be sent for this endpoint
 * 
 * @return true if successful, false otherwise
 */
bool usbd_add_endpoint(usbd_handle_type_def *pdev, uint8_t endpoint, uint32_t maxPacket);

/**
 * @brief Start a reading on a certain endpoint
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint endpoint which will be read
 * @param[in]	maxSize		maxSize the maximum length that can be read
 * 
 * @return true if successful, false otherwise
 */
bool usbd_read_start(usbd_handle_type_def *pdev, uint8_t endpoint, uint32_t maxSize);

/**
 * @brief Read a certain endpoint. Before calling this function, Usbd_read_start
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint which will be read
 * @param[in]	buffer		buffer will be filled with the data received
 * @param[in]	size		the number of bytes read will be stored in *size
 * @param[in]	maxSize		the maximum length that can be read
 * 
 * @return true if successful, false otherwise
 */
bool usbd_read_ep(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t *buffer, uint32_t *size, uint32_t maxSize);

/**
 * @brief Read a certain endpoint with non blocking
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint which will be read
 * @param[in]	buffer		buffer will be filled with the data received (if data are available)
 * @param[in]	size		the number of bytes read will be stored in *size
 * @param[in]	maxSize		the maximum length that can be read
 * 
 * @return true if successful, false otherwise
 */
bool usbd_read_ep_nb(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t *buffer, uint32_t *size, uint32_t maxSize);

/**
 * @brief Write a certain endpoint
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint endpoint to write
 * @param[in]	buffer		data contained in buffer will be write
 * @param[in]	size		the number of bytes to write
 * @param[in]	maxSize		the maximum length that can be written on this endpoint
 * 
 * @return true if successful, false otherwise
 */
bool usbd_write(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t *buffer, uint32_t size, uint32_t maxSize);

/**
 * @brief Write a certain endpoint with non blocking
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	endpoint	endpoint endpoint to write
 * @param[in]	buffer		data contained in buffer will be write
 * @param[in]	size		the number of bytes to write
 * @param[in]	maxSize		the maximum length that can be written on this endpoint
 * 
 * @return true if successful, false otherwise
 */
bool usbd_write_nb(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t size, uint32_t maxSize);

/**
 * @brief Usb device/class reset
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return none
 */
void usbd_bus_reset(usbd_handle_type_def *pdev);

/**
 * @brief Called by usbdevice on Endpoint0 setup
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return none
 */
void usbd_ep0_setup_callback(usbd_handle_type_def *pdev);

/**
 * @brief Called by usbdevice on Endpoint0 out data event 
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return none
 */
void usbd_ep0_out(usbd_handle_type_def *pdev);

/**
 * @brief Called by usbdevice on Endpoint0 in data event 
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return none
 */
void usbd_ep0_in(usbd_handle_type_def *pdev);

/**
 * @brief Called by usbdevice on usb device connect state changed
 * 
 * @param[in]	connected	1 - connected, 0 - disconnected
 * 
 * @return none
 */
void usbd_connect_state_changed(unsigned int connected);

/**
 * @brief Called by usbdevice on usb device suspended state changed
 * 
 * @param[in]	suspended	1 - suspended, 0 - non suspended
 * 
 * @return none
 */
void usbd_suspend_state_changed(unsigned int suspended);

/**
 * @brief Find a descriptor within the list of descriptors
 * 
 * @param[in]	pdev			point to usb device handler
 * @param[in]	descriptor_type	the descriptor type
 * 
 * @return usb device descriptor
 */
uint8_t *usbd_find_descriptor(usbd_handle_type_def *pdev, uint8_t descriptor_type);

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_H */
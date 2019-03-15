/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stdint.h"

#include "usb_device_endpoints.h"
#include "usb_device.h"
#include "usb_device_descriptor.h"
#include "aos/hal/usbd.h"

static uint16_t current_interface;
static uint8_t  current_alternate;

/* Device status */
#define DEVICE_STATUS_SELF_POWERED  (1U << 0)
#define DEVICE_STATUS_REMOTE_WAKEUP (1U << 1)

/* Endpoint status */
#define ENDPOINT_STATUS_HALT        (1U << 0)

/* Standard feature selectors */
#define DEVICE_REMOTE_WAKEUP        (1)
#define ENDPOINT_HALT               (0)

/* Macro to convert wIndex endpoint number to physical endpoint number */
#define WINDEX_TO_PHYSICAL(endpoint) (((endpoint & 0x0f) << 1) + ((endpoint & 0x80) ? 1 : 0))


usbd_stat_t usbd_init(usbd_handle_type_def *pdev, usbd_descriptors_type_def *pdesc)
{
    if (pdev == NULL) {
#ifdef DEBUG
        printf("Invalid Device handle\r\n");
#endif
        return USBD_FAIL;
    }

    /* Unlink previous class*/
    if (pdev->pClass != NULL) {
        pdev->pClass = NULL;
    }

    /* Assign USBD Descriptors */
    if (pdesc != NULL) {
        pdev->pDesc = pdesc;
    }

    /* Set initial device state */
    pdev->device_info.state         = POWERED;
    pdev->device_info.configuration = 0;
    pdev->device_info.suspended     = false;

    /* Initialize low level driver */
    usbd_hal_init(pdev);

    return USBD_OK;
}


usbd_stat_t usbd_deinit(usbd_handle_type_def *pdev)
{
    /* Free Class Resources */
    pdev->pClass->DeInit(pdev, NULL);  

    /* Stop the low level driver  */
    usbd_hal_stop(pdev);

    /* Initialize low level driver */
    usbd_hal_deinit(pdev);

    return USBD_OK;
}

usbd_stat_t  usbd_register_class(usbd_handle_type_def *pdev, usbd_class_type_def *pclass)
{
    usbd_stat_t status = USBD_OK;

    if (pclass != 0) {
        /* link the class to the USB Device handle */
        pdev->pClass = pclass;
        status = USBD_OK;
    } else {
#ifdef DEBUG
        printf("Invalid Class handle\r\n");
#endif
        status = USBD_FAIL;
    }

    return status;
}

usbd_stat_t usbd_start(usbd_handle_type_def *pdev, void *data)
{
    /* Start the low level driver  */
    usbd_hal_start(pdev);

    pdev->pClass->Init(pdev, data);

    return USBD_OK;  
}

usbd_stat_t  usbd_stop(usbd_handle_type_def *pdev, void *data)
{
    /* Free Class Resources */
    pdev->pClass->DeInit(pdev, data);

    /* Stop the low level driver */
    usbd_hal_stop(pdev);

    return USBD_OK;
}

bool usbd_request_get_descriptor(usbd_handle_type_def *pdev)
{
    bool success = false;

    control_transfer_t *transfer = &pdev->transfer;

#ifdef DEBUG
    printf("get descr: type: %d\r\n", DESCRIPTOR_TYPE(transfer->setup.wValue));
#endif
    switch (DESCRIPTOR_TYPE(transfer->setup.wValue)) {
        case DEVICE_DESCRIPTOR:
            if (pdev->pDesc->get_device_descriptor() != NULL) {
                if ((pdev->pDesc->get_device_descriptor()[0] == DEVICE_DESCRIPTOR_LENGTH) \
                    && (pdev->pDesc->get_device_descriptor()[1] == DEVICE_DESCRIPTOR)) {
#ifdef DEBUG
                    printf("device descr\r\n");
#endif
                    transfer->remaining = DEVICE_DESCRIPTOR_LENGTH;
                    transfer->ptr = (uint8_t*)pdev->pDesc->get_device_descriptor();
                    transfer->direction = DEVICE_TO_HOST;
                    success = true;
                }
            }
            break;
        case CONFIGURATION_DESCRIPTOR:
            if (pdev->pClass->get_config_descriptor() != NULL) {
                if ((pdev->pClass->get_config_descriptor()[0] == CONFIGURATION_DESCRIPTOR_LENGTH) \
                    && (pdev->pClass->get_config_descriptor()[1] == CONFIGURATION_DESCRIPTOR)) {
#ifdef DEBUG
                    printf("conf descr request\r\n");
#endif
                    /* Get wTotalLength */
                    transfer->remaining = pdev->pClass->get_config_descriptor()[2] \
                        | (pdev->pClass->get_config_descriptor()[3] << 8);

                    transfer->ptr = (uint8_t*)pdev->pClass->get_config_descriptor();
                    transfer->direction = DEVICE_TO_HOST;
                    success = true;
                }
            }
            break;
        case STRING_DESCRIPTOR:
#ifdef DEBUG
            printf("str descriptor\r\n");
#endif
            switch (DESCRIPTOR_INDEX(transfer->setup.wValue)) {
                            case STRING_OFFSET_LANGID:
#ifdef DEBUG
                                printf("1\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_langid_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_langid_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
                            case STRING_OFFSET_IMANUFACTURER:
#ifdef DEBUG
                                printf("2\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_manufacturer_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_manufacturer_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
                            case STRING_OFFSET_IPRODUCT:
#ifdef DEBUG
                                printf("3\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_product_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_product_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
                            case STRING_OFFSET_ISERIAL:
#ifdef DEBUG
                                printf("4\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_serial_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_serial_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
                            case STRING_OFFSET_ICONFIGURATION:
#ifdef DEBUG
                                printf("5\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_configuration_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_configuration_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
                            case STRING_OFFSET_IINTERFACE:
#ifdef DEBUG
                                printf("6\r\n");
#endif
                                transfer->remaining = pdev->pDesc->get_interface_str_descriptor()[0];
                                transfer->ptr = (uint8_t*)pdev->pDesc->get_interface_str_descriptor();
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                                break;
            }
            break;
        case INTERFACE_DESCRIPTOR:
#ifdef DEBUG
            printf("interface descr\r\n");
#endif
        case ENDPOINT_DESCRIPTOR:
#ifdef DEBUG
            printf("endpoint descr\r\n");
#endif
            /* TODO: Support is optional, not implemented here */
            break;
        default:
#ifdef DEBUG
            printf("ERROR\r\n");
#endif
            break;
    }

    return success;
}

void usbd_decode_setup_packet(uint8_t *data, setup_packet_t *packet)
{
    /* Fill in the elements of a setup_packet_t structure from raw data */
    packet->bmRequestType.data_transfer_direction = (data[0] & 0x80) >> 7;
    packet->bmRequestType.Type = (data[0] & 0x60) >> 5;
    packet->bmRequestType.Recipient = data[0] & 0x1f;
    packet->bRequest = data[1];
    packet->wValue = (data[2] | (uint16_t)data[3] << 8);
    packet->wIndex = (data[4] | (uint16_t)data[5] << 8);
    packet->wLength = (data[6] | (uint16_t)data[7] << 8);
}

bool usbd_control_out(usbd_handle_type_def *pdev)
{
    /* Control transfer data OUT stage */
    uint8_t  buffer[MAX_PACKET_SIZE_EP0];
    uint32_t packetSize;

    control_transfer_t *transfer = &pdev->transfer;

    /* Check we should be transferring data OUT */
    if (transfer->direction != HOST_TO_DEVICE) {
         /* for other platforms, count on the HAL to handle this case */
         return false;
    }

    /* Read from endpoint */
    packetSize = usbd_hal_get_ep0_read_result((void *)pdev, buffer);

    /* Check if transfer size is valid */
    if (packetSize > transfer->remaining) {
        /* Too big */
        return false;
    }

    /* Update transfer */
    transfer->ptr += packetSize;
    transfer->remaining -= packetSize;

    /* Check if transfer has completed */
    if (transfer->remaining == 0) {
        /* Transfer completed */
        if (transfer->notify) {
            /* Notify class layer. */
            if (pdev->pClass->callback_request_completed != NULL) {
                pdev->pClass->callback_request_completed(pdev, buffer, packetSize);
            }
            transfer->notify = false;
        }
        /* Status stage */
        usbd_hal_ep0_write(pdev, NULL, 0);
    } else {
        usbd_hal_ep0_read(pdev);
    }

    return true;
}

bool usbd_control_in(usbd_handle_type_def *pdev)
{
    /* Control transfer data IN stage */
    uint32_t packetSize;
    control_transfer_t *transfer = &pdev->transfer;

    /* Check if transfer has completed (status stage transactions */
    /* also have transfer.remaining == 0) */
    if (transfer->remaining == 0) {
        if (transfer->zlp) {
            /* Send zero length packet */
            usbd_hal_ep0_write(pdev, NULL, 0);
            transfer->zlp = false;
        }

        /* Transfer completed */
        if (transfer->notify) {
            /* Notify class layer. */
            if (pdev->pClass->callback_request_completed != NULL) {
                pdev->pClass->callback_request_completed(pdev, NULL, 0);
            }
            transfer->notify = false;
        }

        usbd_hal_ep0_read(pdev);
        usbd_hal_ep0_read_stage();

        /* Completed */
        return true;
    }

    /* Check we should be transferring data IN */
    if (transfer->direction != DEVICE_TO_HOST) {
        return false;
    }

    packetSize = transfer->remaining;

    if (packetSize > MAX_PACKET_SIZE_EP0) {
        packetSize = MAX_PACKET_SIZE_EP0;
    }

    /* Write to endpoint */
    usbd_hal_ep0_write(pdev, transfer->ptr, packetSize);

    /* Update transfer */
    transfer->ptr += packetSize;
    transfer->remaining -= packetSize;

    return true;
}

bool usbd_request_set_address(usbd_handle_type_def *pdev)
{
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    /* Set the device address */
    usbd_hal_set_address(pdev, transfer->setup.wValue);

    if (transfer->setup.wValue == 0) {
        device->state = DEFAULT;
    } else {
        device->state = ADDRESS;
    }

    return true;
}

bool usbd_request_set_configuration(usbd_handle_type_def *pdev)
{
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    device->configuration = transfer->setup.wValue;
    /* Set the device configuration */
    if (device->configuration == 0) {
        /* Not configured */
        usbd_hal_unconfigure_device();
        device->state = ADDRESS;
    } else {
        if (pdev->pClass->set_configuration != NULL) {
            if (pdev->pClass->set_configuration(pdev, device->configuration)) {
                /* Valid configuration */
                usbd_hal_configure_device();
                device->state = CONFIGURED;
            } else {
                return false;
            }
        }
    }

    return true;
}

bool usbd_request_get_configuration(usbd_handle_type_def *pdev)
{
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    /* Send the device configuration */
    transfer->ptr = &device->configuration;
    transfer->remaining = sizeof(device->configuration);
    transfer->direction = DEVICE_TO_HOST;
    return true;
}

bool usbd_request_get_interface(usbd_handle_type_def *pdev)
{
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    /* Return the selected alternate setting for an interface */

    if (device->state != CONFIGURED) {
        return false;
    }

    /* Send the alternate setting */
    transfer->setup.wIndex = current_interface;
    transfer->ptr = &current_alternate;
    transfer->remaining = sizeof(current_alternate);
    transfer->direction = DEVICE_TO_HOST;
    return true;
}

bool usbd_request_set_interface(usbd_handle_type_def *pdev)
{
    bool success = false;
    control_transfer_t *transfer = &pdev->transfer;

    if(pdev->pClass->set_interface != NULL) {
        if(pdev->pClass->set_interface(transfer->setup.wIndex, transfer->setup.wValue)) {
            success = true;
            current_interface = transfer->setup.wIndex;
            current_alternate = transfer->setup.wValue;
        }
    }
    return success;
}

bool usbd_request_set_feature(usbd_handle_type_def *pdev)
{
    bool success = false;
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    if (device->state != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->setup.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->setup.bmRequestType.Recipient) {
        case DEVICE_RECIPIENT:
            /* TODO: Remote wakeup feature not supported */
            break;
        case ENDPOINT_RECIPIENT:
            if (transfer->setup.wValue == ENDPOINT_HALT) {
                /* TODO: We should check that the endpoint number is valid */
                usbd_hal_stall_endpoint(pdev, WINDEX_TO_PHYSICAL(transfer->setup.wIndex));
                success = true;
            }
            break;
        default:
            break;
    }

    return success;
}

bool usbd_request_clear_feature(usbd_handle_type_def *pdev)
{
    bool success = false;
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    if (device->state != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->setup.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->setup.bmRequestType.Recipient) {
        case DEVICE_RECIPIENT:
            /* TODO: Remote wakeup feature not supported */
            break;
        case ENDPOINT_RECIPIENT:
            /* TODO: We should check that the endpoint number is valid */
            if (transfer->setup.wValue == ENDPOINT_HALT) {
                usbd_hal_unstall_endpoint(pdev, WINDEX_TO_PHYSICAL(transfer->setup.wIndex));
                success = true;
            }
            break;
        default:
            break;
    }

    return success;
}

bool usbd_request_get_status(usbd_handle_type_def *pdev)
{
    static uint16_t status;
    bool success = false;
    control_transfer_t *transfer = &pdev->transfer;
    usbd_info_t *device = &pdev->device_info;

    if (device->state != CONFIGURED) {
        /* Endpoint or interface must be zero */
        if (transfer->setup.wIndex != 0) {
            return false;
        }
    }

    switch (transfer->setup.bmRequestType.Recipient) {
        case DEVICE_RECIPIENT:
            /* TODO: Currently only supports self powered devices */
            status = DEVICE_STATUS_SELF_POWERED;
            success = true;
            break;
        case INTERFACE_RECIPIENT:
            status = 0;
            success = true;
            break;
        case ENDPOINT_RECIPIENT:
            /* TODO: We should check that the endpoint number is valid */
            if (usbd_hal_get_endpoint_stall_state(pdev, WINDEX_TO_PHYSICAL(transfer->setup.wIndex))) {
                status = ENDPOINT_STATUS_HALT;
            } else {
                status = 0;
            }
            success = true;
            break;
        default:
            break;
    }

    if (success) {
        /* Send the status */
        transfer->ptr = (uint8_t *)&status; /* Assumes little endian */
        transfer->remaining = sizeof(status);
        transfer->direction = DEVICE_TO_HOST;
    }

    return success;
}

bool usbd_request_setup(usbd_handle_type_def *pdev)
{
    bool success = false;

    /* Process standard requests */
    if ((pdev->transfer.setup.bmRequestType.Type == STANDARD_TYPE)) {
        switch (pdev->transfer.setup.bRequest) {
             case GET_STATUS:
                 success = usbd_request_get_status(pdev);
                 break;
             case CLEAR_FEATURE:
                 success = usbd_request_clear_feature(pdev);
                 break;
             case SET_FEATURE:
                 success = usbd_request_set_feature(pdev);
                 break;
             case SET_ADDRESS:
                 success = usbd_request_set_address(pdev);
                 break;
             case GET_DESCRIPTOR:
                 success = usbd_request_get_descriptor(pdev);
                 break;
             case SET_DESCRIPTOR:
                 /* TODO: Support is optional, not implemented here */
                 success = false;
                 break;
             case GET_CONFIGURATION:
                 success = usbd_request_get_configuration(pdev);
                 break;
             case SET_CONFIGURATION:
                 success = usbd_request_set_configuration(pdev);
                 break;
             case GET_INTERFACE:
                 success = usbd_request_get_interface(pdev);
                 break;
             case SET_INTERFACE:
                 success = usbd_request_set_interface(pdev);
                 break;
             default:
                 break;
        }
    }

    return success;
}

bool usbd_control_setup(usbd_handle_type_def *pdev)
{
    bool success = false;

    /* Control transfer setup stage */
    uint8_t buffer[MAX_PACKET_SIZE_EP0];

    usbd_hal_ep0_setup(buffer);

    /* Initialise control transfer state */
    usbd_decode_setup_packet(buffer, &pdev->transfer.setup);
    pdev->transfer.ptr = NULL;
    pdev->transfer.remaining = 0;
    pdev->transfer.direction = 0;
    pdev->transfer.zlp = false;
    pdev->transfer.notify = false;

#ifdef DEBUG
    printf("data_transfer_direction: %d\r\nType: %d\r\nRecipient: %d\r\nbRequest: %d\r\nwValue: %d\r\nwIndex: %d\r\nwLength: %d\r\n",pdev->transfer.setup.bmRequestType.data_transfer_direction,
                                                                                                                                   pdev->transfer.setup.bmRequestType.Type,
                                                                                                                                   pdev->transfer.setup.bmRequestType.Recipient,
                                                                                                                                   pdev->transfer.setup.bRequest,
                                                                                                                                   pdev->transfer.setup.wValue,
                                                                                                                                   pdev->transfer.setup.wIndex,
                                                                                                                                   pdev->transfer.setup.wLength);
#endif

    /* Class / vendor specific */
    if(pdev->pClass->callback_request != NULL) {
        success = pdev->pClass->callback_request(pdev);
    } else {
        #ifdef DEBUG
            printf("callback_request not registed!!!!\r\n");
        #endif
        return false;
    }

    if (!success) {
        /* Standard requests */
        if (!usbd_request_setup(pdev)) {
#ifdef DEBUG
            printf("fail!!!!\r\n");
#endif
            return false;
        }
    }

    /* Check transfer size and direction */
    if (pdev->transfer.setup.wLength>0) {
        if (pdev->transfer.setup.bmRequestType.data_transfer_direction \
            == DEVICE_TO_HOST) {
            /* IN data stage is required */
            if (pdev->transfer.direction != DEVICE_TO_HOST) {
                return false;
            }

            /* Transfer must be less than or equal to the size */
            /* requested by the host */
            if (pdev->transfer.remaining > pdev->transfer.setup.wLength) {
                pdev->transfer.remaining = pdev->transfer.setup.wLength;
            }
        } else {

            /* OUT data stage is required */
            if (pdev->transfer.direction != HOST_TO_DEVICE) {
                return false;
            }

            /* Transfer must be equal to the size requested by the host */
            if (pdev->transfer.remaining != pdev->transfer.setup.wLength) {
                return false;
            }
        }
    } else {
        /* No data stage; transfer size must be zero */
        if (pdev->transfer.remaining != 0) {
            return false;
        }
    }

    /* Data or status stage if applicable */
    if (pdev->transfer.setup.wLength > 0) {
        if (pdev->transfer.setup.bmRequestType.data_transfer_direction \
            == DEVICE_TO_HOST) {
            /* Check if we'll need to send a zero length packet at */
            /* the end of this transfer */
            if (pdev->transfer.setup.wLength > pdev->transfer.remaining) {
                /* Device wishes to transfer less than host requested */
                if ((pdev->transfer.remaining % MAX_PACKET_SIZE_EP0) == 0) {
                    /* Transfer is a multiple of EP0 max packet size */
                    pdev->transfer.zlp = true;
                }
            }

            /* IN stage */
            usbd_control_in(pdev);
        } else {
            /* OUT stage */
            usbd_hal_ep0_read(pdev);
        }
    } else {
        /* Status stage */
        usbd_hal_ep0_write(pdev, NULL, 0);
    }

    return true;
}

void usbd_bus_reset(usbd_handle_type_def *pdev)
{
    usbd_info_t *device = &pdev->device_info;

    device->state = DEFAULT;
    device->configuration = 0;
    device->suspended = false;

    /* Call class / vendor specific busReset function */
    if (pdev->pClass->usbd_callback_bus_reset != NULL) {
        pdev->pClass->usbd_callback_bus_reset();
    }
}

void usbd_ep0_setup_callback(usbd_handle_type_def *pdev)
{
    /* Endpoint 0 setup event */
    if (!usbd_control_setup(pdev)) {
        /* Protocol stall */
        usbd_hal_ep0_stall(pdev);
    }

    /* Return true if an OUT data stage is expected */
}

void usbd_ep0_out(usbd_handle_type_def *pdev)
{
    /* Endpoint 0 OUT data event */
    if (!usbd_control_out(pdev)) {
        /* Protocol stall; this will stall both endpoints */
        usbd_hal_ep0_stall(pdev);
    }
}

void usbd_ep0_in(usbd_handle_type_def *pdev)
{
#ifdef DEBUG
    printf("EP0IN\r\n");
#endif
    /* Endpoint 0 IN data event */
    if (!usbd_control_in(pdev)) {
        /* Protocol stall; this will stall both endpoints */
        usbd_hal_ep0_stall(pdev);
    }
}

bool usbd_configured(usbd_handle_type_def *pdev)
{
    usbd_info_t *device = &pdev->device_info;

    /* Returns true if device is in the CONFIGURED state */
    return (device->state == CONFIGURED);
}

void usbd_connect(usbd_handle_type_def *pdev, bool blocking)
{
    /* Connect device */
    usbd_hal_connect();

    if (blocking) {
        /* Block if not configured */
        while (!usbd_configured(pdev));
    }

}

void usbd_disconnect(usbd_handle_type_def *pdev)
{
    usbd_info_t *device = &pdev->device_info;

    /* Disconnect device */
    usbd_hal_disconnect();
    
    /* Set initial device state */
    device->state = POWERED;
    device->configuration = 0;
    device->suspended = false;
}

bool usbd_add_endpoint(usbd_handle_type_def *pdev, uint8_t endpoint, uint32_t maxPacket)
{
    return usbd_hal_realise_endpoint((void *)pdev, endpoint, maxPacket, 0);
}

bool usbd_add_rate_feedback_endpoint(usbd_handle_type_def *pdev, uint8_t endpoint, uint32_t maxPacket)
{
    /* For interrupt endpoints only */
    return usbd_hal_realise_endpoint((void *)pdev, endpoint, maxPacket, RATE_FEEDBACK_MODE);
}

uint8_t *usbd_find_descriptor(usbd_handle_type_def *pdev, uint8_t descriptor_type)
{
    /* Find a descriptor within the list of descriptors */
    /* following a configuration descriptor. */
    uint16_t wTotalLength;
    uint8_t *ptr;

    if (pdev->pClass->get_config_descriptor() == NULL) {
        return NULL;
    }

    /* Check this is a configuration descriptor */
    if ((pdev->pClass->get_config_descriptor()[0] != CONFIGURATION_DESCRIPTOR_LENGTH) \
            || (pdev->pClass->get_config_descriptor()[1] != CONFIGURATION_DESCRIPTOR)) {
        return NULL;
    }

    wTotalLength = pdev->pClass->get_config_descriptor()[2] | (pdev->pClass->get_config_descriptor()[3] << 8);

    /* Check there are some more descriptors to follow */
    if (wTotalLength <= (CONFIGURATION_DESCRIPTOR_LENGTH+2)) {
    	/* +2 is for bLength and bDescriptorType of next descriptor */
        return NULL;
    }

    /* Start at first descriptor after the configuration descriptor */
    ptr = &(((uint8_t*)pdev->pClass->get_config_descriptor())[CONFIGURATION_DESCRIPTOR_LENGTH]);

    do {
        if (ptr[1] /* bDescriptorType */ == descriptor_type) {
            /* Found */
            return ptr;
        }

        /* Skip to next descriptor */
        ptr += ptr[0]; /* bLength */
    } while (ptr < (pdev->pClass->get_config_descriptor() + wTotalLength));

    /* Reached end of the descriptors - not found */
    return NULL;
}

void usbd_connect_state_changed(unsigned int connected)
{
}

void usbd_suspend_state_changed(unsigned int suspended)
{
}

bool usbd_read_start(usbd_handle_type_def *pdev, uint8_t endpoint, uint32_t maxSize)
{
    return usbd_hal_endpoint_read((void *)pdev, endpoint, maxSize) == EP_PENDING;
}

bool usbd_write(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t size, uint32_t maxSize)
{
    ep_status result;

    if (size > maxSize) {
        return false;
    }


    if(!usbd_configured(pdev)) {
        return false;
    }

    /* Send report */
    result = usbd_hal_endpoint_write(pdev, endpoint, buffer, size);

    if (result != EP_PENDING) {
        return false;
    }

    /* Wait for completion */
    do {
        result = usbd_hal_endpoint_write_result(pdev, endpoint);
    } while ((result == EP_PENDING) && usbd_configured(pdev));

    return (result == EP_COMPLETED);
}

bool usbd_write_nb(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t size, uint32_t maxSize)
{
    ep_status result;

    if (size > maxSize) {
        return false;
    }

    if(!usbd_configured(pdev)) {
        return false;
    }

    /* Send report */
    result = usbd_hal_endpoint_write(pdev, endpoint, buffer, size);

    if (result != EP_PENDING) {
        return false;
    }

    result = usbd_hal_endpoint_write_result(pdev, endpoint);

    return (result == EP_COMPLETED);
}

bool usbd_read_ep(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t * size, uint32_t maxSize)
{
    ep_status result;

    if(!usbd_configured(pdev)) {
        return false;
    }

    /* Wait for completion */
    do {
        result = usbd_hal_endpoint_read_result(pdev, endpoint, buffer, size);
    } while ((result == EP_PENDING) && usbd_configured(pdev));

    return (result == EP_COMPLETED);
}


bool usbd_read_ep_nb(usbd_handle_type_def *pdev, uint8_t endpoint, uint8_t * buffer, uint32_t * size, uint32_t maxSize)
{
    ep_status result;

    if(!usbd_configured(pdev)) {        
        return false;
    }

    result = usbd_hal_endpoint_read_result(pdev, endpoint, buffer, size); 

    return (result == EP_COMPLETED);
}


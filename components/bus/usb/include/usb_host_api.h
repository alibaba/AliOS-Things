/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _USB_HOST_API_H
#define _USB_HOST_API_H

#include "usb_host_types.h"
#include "usb_host_hcd_hal.h"

/**
 * @brief            This function initializes usb host stack        
 * @param            none
 * @return           USB_SUCESS success, other fail. See error code in usb_host_defines.h
 */
int usb_host_init(void);

/**
 * @brief            This function obtains host stack context pointer
 * @param            none
 * @return           None-null success, none fail
 */
usb_host_t * usb_host_get(void);

/**
 * @brief            This function registers usb host class to host stack
 * @param[in]        class name
 * @param[in]        class control entry function pointer
 * @return           USB_SUCESS success, other fail. See error code in usb_host_defines.h
 */
int usb_host_class_register(uint8_t *class_name,
                            int (*class_entry_function)(usb_host_class_cmd_t *));
 

/**
 * @brief            This function registers usb host hcd to host stack
 * @param[in]        hcd name
 * @param[in]        hcd operations pointer
 * @param[in]        hcd extra parameter
 * @return           USB_SUCESS success, other fail. See error code in usb_host_defines.h
 */
int usb_host_hcd_register(uint8_t *hcd_name, usb_host_hcd_op_t *hcd_op, 
                          uint32_t hcd_param);


/**
 * @brief            This function obtains a registered host class by name
 * @param[in]        class name
 * @param[out]       address of pointer to host class
 * @return           USB_SUCESS success, other fail. See error code in usb_host_defines.h
 */
int usb_host_class_get(uint8_t *class_name, usb_host_class_t **host_class);


/**
 * @brief            This function obtain usb host class instnace from stack
 * @param[in]        pointer to host class
 * @param[in]        class instance index
 * @param[out]       address of pointer to class instance
 * @return           USB_SUCESS success, other fail. See error code in usb_host_defines.h
 */
int usb_host_class_instance_get(usb_host_class_t *host_class, uint32_t class_index, void **class_instance);


 #endif /* _USB_HOST_API_H */

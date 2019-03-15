/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef USB_DEVICE_MOUSE_H
#define USB_DEVICE_MOUSE_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "usb_device_hid.h"


#define REPORT_ID_MOUSE   2

/* Common usage */
enum MOUSE_BUTTON
{
    MOUSE_LEFT = 1,
    MOUSE_RIGHT = 2,
    MOUSE_MIDDLE = 4,
};

/* X and Y limits */
/* These values do not directly map to screen pixels */
/* Zero may be interpreted as meaning 'no movement' */
#define X_MIN_ABS    (1)        /*!< Minimum value on x-axis */
#define Y_MIN_ABS    (1)        /*!< Minimum value on y-axis */
#define X_MAX_ABS    (0x7fff)   /*!< Maximum value on x-axis */
#define Y_MAX_ABS    (0x7fff)   /*!< Maximum value on y-axis */

#define X_MIN_REL    (-127)     /*!< The maximum value that we can move to the left on the x-axis */
#define Y_MIN_REL    (-127)     /*!< The maximum value that we can move up on the y-axis */
#define X_MAX_REL    (127)      /*!< The maximum value that we can move to the right on the x-axis */
#define Y_MAX_REL    (127)      /*!< The maximum value that we can move down on the y-axis */

enum MOUSE_TYPE
{
    ABS_MOUSE,
    REL_MOUSE,
};

#define MOUSE_VENDOR_ID        0x1234
#define MOUSE_PRODUCT_ID       0x0001
#define MOUSE_PRODUCT_RELEASE  0x0001

typedef struct _usbd_mouse_data_def
{
    void* data;
} usbd_mouse_data;


/**
 * @brief Initialize hid device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return none
 */
void usbd_mouse_init(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Deinitialize hid device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return true is ok, false is false
 */
uint8_t usbd_mouse_deinit(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Move the cursor to (x, y)
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	x		x-axis position
 * @param[in]	y		y-axis position
 * 
 * @return true is ok, false is false
 */
bool  usbd_mouse_move(usbd_handle_type_def *pdev, int16_t x, int16_t y);

/**
 * @brief Press one or several buttons
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	button		button state (ex: press(MOUSE_LEFT))
 * 
 * @return true is ok, false is false
 */
bool  usbd_mouse_press(usbd_handle_type_def *pdev, uint8_t button);

/**
 * @brief Release one or several buttons
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	button		button state (ex: press(MOUSE_LEFT))
 * 
 * @return true is ok, false is false
 */
bool  usbd_mouse_release(usbd_handle_type_def *pdev, uint8_t button);

/**
 * @brief Double click (MOUSE_LEFT)
 *
 * 
 * @param[in]	pdev		point to usb device handler
 * 
 * @return true is ok, false is false
 */
bool  usbd_mouse_doubleClick(usbd_handle_type_def *pdev);

/**
 * @brief Click
 *
 * 
 * @param[in]	pdev		point to usb device handler
 * @param[in]	button		state of the buttons ( ex: clic(MOUSE_LEFT))
 * 
 * @return true is ok, false is false
 */
bool usbd_mouse_click(usbd_handle_type_def *pdev, uint8_t button);

/**
 * @brief Scrolling
 *
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	z		value of the wheel (>0 to go down, <0 to go up)
 * 
 * @return true is ok, false is false
 */
bool usbd_mouse_scroll(usbd_handle_type_def *pdev, int8_t z);


#ifdef __cplusplus
}
#endif


#endif

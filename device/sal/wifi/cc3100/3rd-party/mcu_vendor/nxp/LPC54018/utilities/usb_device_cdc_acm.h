/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_DEVICE_CDC_ACM_H_
#define _USB_DEVICE_CDC_ACM_H_ 1
/*******************************************************************************
* Definitions
******************************************************************************/

#define USB_DEVICE_CONFIG_CDC_ACM_MAX_INSTANCE (1)

#define USB_DEVICE_CONFIG_CDC_COMM_CLASS_CODE (0x02)
#define USB_DEVICE_CONFIG_CDC_DATA_CLASS_CODE (0x0A)

/* Class specific request Codes */
#define USB_DEVICE_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND (0x00)
#define USB_DEVICE_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE (0x01)
#define USB_DEVICE_CDC_REQUEST_SET_COMM_FEATURE (0x02)
#define USB_DEVICE_CDC_REQUEST_GET_COMM_FEATURE (0x03)
#define USB_DEVICE_CDC_REQUEST_CLEAR_COMM_FEATURE (0x04)
#define USB_DEVICE_CDC_REQUEST_SET_AUX_LINE_STATE (0x10)
#define USB_DEVICE_CDC_REQUEST_SET_HOOK_STATE (0x11)
#define USB_DEVICE_CDC_REQUEST_PULSE_SETUP (0x12)
#define USB_DEVICE_CDC_REQUEST_SEND_PULSE (0x13)
#define USB_DEVICE_CDC_REQUEST_SET_PULSE_TIME (0x14)
#define USB_DEVICE_CDC_REQUEST_RING_AUX_JACK (0x15)
#define USB_DEVICE_CDC_REQUEST_SET_LINE_CODING (0x20)
#define USB_DEVICE_CDC_REQUEST_GET_LINE_CODING (0x21)
#define USB_DEVICE_CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22)
#define USB_DEVICE_CDC_REQUEST_SEND_BREAK (0x23)
#define USB_DEVICE_CDC_REQUEST_SET_RINGER_PARAMS (0x30)
#define USB_DEVICE_CDC_REQUEST_GET_RINGER_PARAMS (0x31)
#define USB_DEVICE_CDC_REQUEST_SET_OPERATION_PARAM (0x32)
#define USB_DEVICE_CDC_REQUEST_GET_OPERATION_PARAM (0x33)
#define USB_DEVICE_CDC_REQUEST_SET_LINE_PARAMS (0x34)
#define USB_DEVICE_CDC_REQUEST_GET_LINE_PARAMS (0x35)
#define USB_DEVICE_CDC_REQUEST_DIAL_DIGITS (0x36)
#define USB_DEVICE_CDC_REQUEST_SET_UNIT_PARAMETER (0x37)
#define USB_DEVICE_CDC_REQUEST_GET_UNIT_PARAMETER (0x38)
#define USB_DEVICE_CDC_REQUEST_CLEAR_UNIT_PARAMETER (0x39)
#define USB_DEVICE_CDC_REQUEST_GET_PROFILE (0x3A)
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTERS (0x40)
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_POW_PATTER_FILTER (0x41)
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_POW_PATTER_FILTER (0x42)
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_PACKET_FILTER (0x43)
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_STATISTIC (0x44)
#define USB_DEVICE_CDC_REQUEST_SET_ATM_DATA_FORMAT (0x50)
#define USB_DEVICE_CDC_REQUEST_GET_ATM_DEVICE_STATISTICS (0x51)
#define USB_DEVICE_CDC_REQUEST_SET_ATM_DEFAULT_VC (0x52)
#define USB_DEVICE_CDC_REQUEST_GET_ATM_VC_STATISTICS (0x53)
#define USB_DEVICE_CDC_REQUEST_MDLM_SPECIFIC_REQUESTS_MASK (0x7F)

#define USB_DEVICE_CDC_REQUEST_GET_ABSTRACT_STATE (0x23)
#define USB_DEVICE_CDC_REQUEST_GET_COUNTRY_SETTING (0x24)
#define USB_DEVICE_CDC_REQUEST_SET_ABSTRACT_STATE (0x25)
#define USB_DEVICE_CDC_REQUEST_SET_COUNTRY_SETTING (0x26)
/* Class Specific Notification Codes */
#define USB_DEVICE_CDC_REQUEST_NETWORK_CONNECTION_NOTIF (0x00)
#define USB_DEVICE_CDC_REQUEST_RESPONSE_AVAIL_NOTIF (0x01)
#define USB_DEVICE_CDC_REQUEST_AUX_JACK_HOOK_STATE_NOTIF (0x08)
#define USB_DEVICE_CDC_REQUEST_RING_DETECT_NOTIF (0x09)
#define USB_DEVICE_CDC_REQUEST_SERIAL_STATE_NOTIF (0x20)
#define USB_DEVICE_CDC_REQUEST_CALL_STATE_CHANGE_NOTIF (0x28)
#define USB_DEVICE_CDC_REQUEST_LINE_STATE_CHANGE_NOTIF (0x29)
#define USB_DEVICE_CDC_REQUEST_CONNECTION_SPEED_CHANGE_NOTIF (0x2A)
/* Communications Feature Selector Codes */
#define USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE (0x01)
#define USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING (0x02)
/* Control Signal Bitmap Values */
#define USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION (0x02)
#define USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE (0x01)
/* UART State Bitmap Values */
#define USB_DEVICE_CDC_UART_STATE_RX_CARRIER (0x01)
#define USB_DEVICE_CDC_UART_STATE_TX_CARRIER (0x02)
#define USB_DEVICE_CDC_UART_STATE_BREAK (0x04)
#define USB_DEVICE_CDC_UART_STATE_RING_SIGNAL (0x08)
#define USB_DEVICE_CDC_UART_STATE_FRAMING (0x10)
#define USB_DEVICE_CDC_UART_STATE_PARITY (0x20)
#define USB_DEVICE_CDC_UART_STATE_OVERRUN (0x40)

#endif /* _USB_DEVICE_CDC_ACM_H_ */

/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_MISC_H__
#define __ESP_MISC_H__

#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Misc_APIs Misc APIs
  * @brief misc APIs
  */

/** @addtogroup Misc_APIs
  * @{
  */

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

/**
  * @brief  Delay function, maximum value: 65535 us.
  *
  * @param  uint16 us : delay time, uint: us, maximum value: 65535 us
  *
  * @return null
  */
void os_delay_us(uint16 us);

/**
  * @brief     Register the print output function.
  *
  * @attention os_install_putc1((void *)uart1_write_char) in uart_init will set
  *            printf to print from UART 1, otherwise, printf will start from
  *            UART 0 by default.
  *
  * @param     void(*p)(char c) - pointer of print function
  *
  * @return    null
  */
void os_install_putc1(void (*p)(char c));

/**
  * @brief  Print a character. Start from from UART0 by default.
  *
  * @param  char c - character to be printed
  *
  * @return null
  */
void os_putc(char c);

enum dhcp_status {
    DHCP_STOPPED,   /**< disable DHCP */
    DHCP_STARTED    /**< enable DHCP */
};

struct dhcps_lease {
    bool enable;                /**< enable DHCP lease or not */
    struct ip_addr start_ip;    /**< start IP of IP range */
    struct ip_addr end_ip;      /**< end IP of IP range */
};

enum dhcps_offer_option {
    OFFER_START = 0x00,         /**< DHCP offer option start */
    OFFER_ROUTER = 0x01,        /**< DHCP offer router, only support this option now */
    OFFER_END                   /**< DHCP offer option start */
};

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

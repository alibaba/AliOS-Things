/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "rndis_host_app.h"
#include "app_os_prio.h"

#if CFG_TUH_CDC && CFG_TUH_CDC_RNDIS

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
void tusbh_cdc_rndis_mounted_cb(uint8_t dev_addr)
{ // application set-up
  uint8_t mac_address[6];

  printf("\nan RNDIS device is mounted\n");
  tusbh_cdc_rndis_get_mac_addr(dev_addr, mac_address);

  printf("MAC Address ");
  for(uint8_t i=0; i<6; i++)  printf("%X ", mac_address[i]);
  printf("\n");
}

void tusbh_cdc_rndis_unmounted_cb(uint8_t dev_addr)
{
  // application tear-down
  printf("\nan RNDIS device is unmounted\n");
}

void rndis_host_app_init(void)
{

}

void rndis_host_app_task(void* param)
{
  OSAL_TASK_BEGIN
  OSAL_TASK_END
}

#endif

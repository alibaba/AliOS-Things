/**
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#include "aos/init.h"
#include "aos/kernel.h"
#include "atmel_start.h"
#include <k_api.h>

#include <hal/wifi.h>

#define AOS_START_STACK 1536

ktask_t *g_aos_init;
static kinit_t kinit;

static void sys_init(void);

extern struct hal_ota_module_s aos_ota_module_atsamd5x_e5x;

static void sys_init(void)
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;

    atmel_start_init();
    board_init();

    aos_components_init(&kinit);

#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
}

int main(void)
{
    aos_heap_set();
    aos_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    aos_start();

    return 0;
}


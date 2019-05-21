/**
 ******************************************************************************
 * @file    BkDriverWdg.h

 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides all the headers of WDG operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */
#include "include.h"
#include "rtos_pub.h"
#include "BkDriverWdg.h"
#include "drv_model_pub.h"
#include "error.h"
#include "wdt_pub.h"

OSStatus bk_wdg_initialize( uint32_t timeout )
{
    UINT32 ret, param;

    ret = sddev_control(WDT_DEV_NAME, WCMD_POWER_UP, 0);
    ASSERT(WDT_SUCCESS == ret);

    param = timeout;
    ret = sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &param);
    ASSERT(WDT_SUCCESS == ret);

    return kNoErr;
}

void bk_wdg_reload( void )
{
    UINT32 ret;

    ret = sddev_control(WDT_DEV_NAME, WCMD_RELOAD_PERIOD, 0);
    ASSERT(WDT_SUCCESS == ret);

    return;
}

OSStatus bk_wdg_finalize( void )
{
    UINT32 ret;

    ret = sddev_control(WDT_DEV_NAME, WCMD_POWER_DOWN, 0);
    ASSERT(WDT_SUCCESS == ret);

    return kNoErr;
}

// eof


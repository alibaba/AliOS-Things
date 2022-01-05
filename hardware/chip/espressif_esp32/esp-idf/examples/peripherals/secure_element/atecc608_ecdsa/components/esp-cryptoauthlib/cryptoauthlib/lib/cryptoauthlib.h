/**
 * \file
 * \brief Single aggregation point for all CryptoAuthLib header files
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef _ATCA_LIB_H
#define _ATCA_LIB_H

#include <stddef.h>
#include <string.h>

#include "hal/atca_hal.h"
#include "atca_status.h"
#include "atca_device.h"
#include "atca_command.h"
#include "atca_cfgs.h"
#include "basic/atca_basic.h"
#include "basic/atca_helpers.h"

#ifdef ATCAPRINTF
    #include <stdio.h>
//#define BREAK(status, message) {printf(__FUNCTION__": "message" -- Status: %02X\r\n", status); break;}
    #define BREAK(status, message) { printf(": "message " -- Status: %02X\r\n", status); break; }
    #define RETURN(status, message) { printf(": "message " -- Status: %02X\r\n", status); return status; }
    #define PRINTSTAT(status, message) { printf(": "message " -- Status: %02X\r\n", status); }
    #define PRINT(message) { printf(": "message "\r\n"); break; }
    #define DBGOUT(message) { printf(": "message "\r\n"); break; }
#else
    #define BREAK(status, message) { break; }
    #define RETURN(status, message) { return status; }
    #define PRINT(message) { break; }
    #define DBGOUT(message) { break; }
#endif

#endif

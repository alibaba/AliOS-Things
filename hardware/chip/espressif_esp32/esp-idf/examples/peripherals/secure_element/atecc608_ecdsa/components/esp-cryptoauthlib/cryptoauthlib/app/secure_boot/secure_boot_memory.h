/**
 * \file
 *
 * \brief Provides interface to memory component for the secure boot.
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

#ifndef SECURE_BOOT_MEMORY_H
#define SECURE_BOOT_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "atca_status.h"
#include "atca_command.h"


/*Blocking last USER_APPLICATION_HEADER_SIZE bytes for Signature and memory/application specific information*/
typedef struct
{
    uint32_t start_address;
    uint32_t memory_size;
    uint32_t version_info;
    uint8_t  reserved[52];                      //Reserving bytes for Application information and aligning to page
    uint8_t  signature[ATCA_SIG_SIZE];
}memory_parameters;

extern ATCA_STATUS secure_boot_init_memory(memory_parameters* memory_params);
extern ATCA_STATUS secure_boot_read_memory(uint8_t* pu8_data, uint32_t* pu32_target_length);
extern ATCA_STATUS secure_boot_write_memory(uint8_t* pu8_data, uint32_t* pu32_target_length);
extern void secure_boot_deinit_memory(memory_parameters* memory_params);
extern ATCA_STATUS secure_boot_mark_full_copy_completion(void);
extern bool secure_boot_check_full_copy_completion(void);

#ifdef __cplusplus
}
#endif

#endif




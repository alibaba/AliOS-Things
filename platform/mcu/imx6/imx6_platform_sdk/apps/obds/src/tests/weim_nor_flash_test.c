/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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

/*!
 * @defgroup diag_eim_nor Driver and test for a NOR flash connected on the EIM.
 */

/*!
 * @file weim_nor_flash.c
 * @brief Driver for a NOR flash connected to the i.MX WEIM.
 *
 * @ingroup diag_weim_nor
 */

#include "obds.h"

const char g_eim_NOR_flash_test_name[] = "EIM NOR FLASH Test";

/*!
 * Add the following defines to support a new NOR:
 * Only one NOR should be defined here, other must be commented !!!!!!!
 * e.g.
 * #define nor_flash_auto_manu_id my_new_nor_here_auto_manu_id
 * #define nor_flash_auto_dev_id my_new_nor_here_auto_dev_id
 */
#define nor_flash_auto_manu_id m29w256gl_auto_manu_id
#define nor_flash_auto_dev_id m29w256gl_auto_dev_id

int nor_flash_auto_manu_id(void);
int nor_flash_auto_dev_id(void);

// Set up the chip select registers for the weim "parallel" nor flash
void weim_nor_flash_cs_setup(void)
{
   //DSIZ=010 (map 16-bit to D[31:16], csen=1
   // writel(0x00020001, WEIM_BASE_ADDR + 0x000);
   writel(0x00620081, WEIM_BASE_ADDR + 0x0000);

   // CS0 Read Config reg1
   // RWCS=11, OEA=2, OEN=1
   //writel(0x0B002100, WEIM_BASE_ADDR + 0x008);
   writel(0x1C022000, WEIM_BASE_ADDR + 0x0008);

   // CS0 Write Config Reg 1
   // WWCS=11, WEA=2, WEN=1
   //writel(0x0B000440, WEIM_BASE_ADDR + 0x010);
   writel(0x0804a240, WEIM_BASE_ADDR + 0x010); 
}

/*!
 * This test tries to read the manufacturer and device ID of the NOR flash.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
test_return_t eim_nor_flash_test(void)
{
    int rc = 0;

    /* init the weim interface for the NOR flash */
    eim_iomux_config();
#if defined(BOARD_SABRE_AI) && !defined(BOARD_REV_A)  // for I2C3 steering
    reg32_write(IOMUXC_SW_MUX_CTL_PAD_DISP0_DAT8, ALT5);
    gpio_set_direction(GPIO_PORT4, 29, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT4, 29, GPIO_HIGH_LEVEL);

    /* Select ALT5 mode of EIM_A24 for GPIO5_4 - EIMD18_I2C3_STEER(EIM_A24) */
    /* low output to select WEIM NOR option */
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_A24);
    gpio_set_direction(GPIO_PORT5, 4, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT5, 4, GPIO_LOW_LEVEL);
#endif

    weim_nor_flash_cs_setup();

    rc |= nor_flash_auto_manu_id();
    rc |= nor_flash_auto_dev_id();

    if (rc == 0) {
        return TEST_PASSED;
    } else {
        return TEST_FAILED;
    }
}

/*! General programmer's note: when writing to the flash, we are left shifting the address by 1 to align
 * with 16-bit addressing.  For example, if we are writing to flash address of 0x555, the
 * internal AHB address needs to be 0xAAA or "0x555<<1"
 */

// CS_BASE informs the program of the base system address of the Flash
#define CS_BASE  0x08000000 // WEIM_CS_BASE_ADDR

//reset & unlock writes
#define reset() writew(0x00F0, CS_BASE)

#define unlock() writew(0x00AA, CS_BASE+(0x555<<1)); writew(0x0055, CS_BASE+(0x2AA<<1))

//manufacturer and device ID values
#define MANU_ID 0x0020
#define MANU_ID_AD readw(CS_BASE)
#define DEV_ID_1 0x227E
#define DEV_ID_AD_1 (CS_BASE + (0x1<<1))
#define DEV_ID_2 0x2222
#define DEV_ID_AD_2 (CS_BASE + (0xE<<1))
#define DEV_ID_3 0x2201
#define DEV_ID_AD_3 (CS_BASE + (0xF<<1))

/*!
 * This function reads and checks the manufacturer ID of the Numonyx M29W256GL NOR flash.
 *
 * @return 0 on success; non-zero otherwise
 */
int m29w256gl_auto_manu_id(void)
{
    short temp;
    const char* indent = menu_get_indent();

    reset();
    unlock();
    writew(0x0090, CS_BASE + (0x555 << 1));
    temp = MANU_ID_AD;

    printf("%sMANU_ID = 0x%04X\n", indent, temp);   // uncomment for debug purposes

    reset();
    if (temp == MANU_ID) {
        printf("%s  WEIM NOR flash Manufacturer ID correct.\n", indent);
        return 0;
    } else {
        printf("%s  **Manufacturer ID is not correct, it read back as 0x%04X\n", indent, temp);
        printf("%s  **But it should be: 0x%04X\n", indent, MANU_ID);
        return 1;
    }
}

/*!
 * This function reads and checks the device ID of the Numonyx M29W256GL NOR flash.
 *
 * @return 0 on success; non-zero otherwise
 */
int m29w256gl_auto_dev_id(void)
{
    short temp[3];
    const char* indent = menu_get_indent();

    reset();
    unlock();
    writew(0x0090, CS_BASE + (0x555 << 1));

    temp[0] = readw(DEV_ID_AD_1);
    temp[1] = readw(DEV_ID_AD_2);
    temp[2] = readw(DEV_ID_AD_3);

    reset();
    printf("%sDEV_ID = 0x%04X, 0x%04X, 0x%04X\n", indent, temp[0], temp[1], temp[2]);   // uncomment for debug purposes

    if ((temp[0] == DEV_ID_1) && (temp[1] == DEV_ID_2) && (temp[2] == DEV_ID_3)) {
        printf("%s  WEIM NOR flash Device ID correct.\n", indent);
        return 0;
    } else {
        printf("%s  **Device ID is not correct, it reads back as: 0x%04X, 0x%04X, 0x%04X\n", indent, temp[0],
               temp[1], temp[2]);
        printf("%s  **But it should be: 0x%04X, 0x%04X, 0x%04X\n", indent, DEV_ID_1, DEV_ID_2, DEV_ID_3);
        return 1;
    }
}

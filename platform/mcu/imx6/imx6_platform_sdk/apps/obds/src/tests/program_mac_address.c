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

#include <stdio.h>
#include "obds.h"
#include "registers.h"
#include "soc_memory_map.h"
#include "system_util.h"
#include "ocotp/ocotp.h"
#include "registers/regsocotp.h"

const char g_program_mac_address_test_name[] = "MAC Address Test";

/*!
 * @brief HW_OCOTP_MAC0 - Value of OTP Bank4 Word2 (MAC Address) (RW)
 *
 * Reset value: 0x00000000
 *
 * Copied from the OTP automatically after reset. Can be re-loaded by setting
 * HW_OCOTP_CTRL[RELOAD_SHADOWS] Shadowed memory mapped access to OTP Bank 4, word 2 (ADDR = 0x22).
 * EXAMPLE Empty Example.
 */
#define HW_OCOTP_MAC0_BANK      4
#define HW_OCOTP_MAC0_ROW       2

/*!
 * @brief HW_OCOTP_MAC1 - Value of OTP Bank4 Word3 (MAC Address) (RW)
 *
 * Reset value: 0x00000000
 *
 * Copied from the OTP automatically after reset. Can be re-loaded by setting
 * HW_OCOTP_CTRL[RELOAD_SHADOWS] Shadowed memory mapped access to OTP Bank 4, word 3 (ADDR = 0x23).
 * EXAMPLE Empty Example.
 */
#define HW_OCOTP_MAC1_BANK      4
#define HW_OCOTP_MAC1_ROW       3

/*! Read the ENET MAC address fuses
 *
 * @param mac_data pointer to an array to store mac address read from the fuses
 *
 * @return  void
 */
void read_mac_otp_registers(char* const mac_data)
{
    // Read out address backwards since printing is done via
	// old mx53 map where MSB is first in fuse map

	reg32_t reg_val = HW_OCOTP_MAC1_RD();
    mac_data[0] = ((reg_val) & 0xFF);
    mac_data[1] = (reg_val & 0xFF);

    reg_val = HW_OCOTP_MAC0_RD();
    mac_data[2] = ((reg_val >> 24) & 0xFF);
    mac_data[3] = ((reg_val >> 16) & 0xFF);
    mac_data[4] = ((reg_val >> 8) & 0xFF);
    mac_data[5] = (reg_val & 0xFF);
}

const char * const mac_address_str(const char* const mac_data)
{
	static char mac_str[18];

	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac_data[0], mac_data[1], mac_data[2], mac_data[3], mac_data[4], mac_data[5]);

	return mac_str;
}

/*! Program the ENET MAC address value to the hardware fuses
 *
 * @param   fuse_data pointer to an array with mac address to program to fuses
 * @param   indent used for formatting error or debug output.
 *
 * @return  0 if successful; non-zero otherwise
 */
test_return_t program_mac_address_fuses(const char* const mac_data, const char* const indent)
{
    /* Create a 32-bit formated MAC address from mac_data */
    reg32_t mac0_reg =
        (((mac_data[2] & 0xFF) << 24) | ((mac_data[3] & 0xFF) << 16) |
         ((mac_data[4] & 0xFF) << 8) | (mac_data[5] & 0xFF));

    reg32_t mac1_reg = (((mac_data[0] & 0xFF) << 8) | (mac_data[1] & 0xFF));

    /* TODO: Is it still needed? => ask design */
    /* First, allow fuse programming in the CCM CGPR register by setting the 'efuse_prog_supply_gate' bit */
    unsigned int temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
    temp |= 0x00000010;         // set bit 4, efuse_prog_supply_gate bit
    writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

    char existing_mac_addr[6];
    read_mac_otp_registers(existing_mac_addr);
    debug_printf("%s  ** MAC Address Fuses before program: 0x%08X, 0x%08X\n",
    		indent, HW_OCOTP_MAC0_RD(), HW_OCOTP_MAC1_RD());

    /* blow the fuses */
    ocotp_fuse_blow_row(HW_OCOTP_MAC0_BANK, HW_OCOTP_MAC0_ROW, mac0_reg);
    ocotp_fuse_blow_row(HW_OCOTP_MAC1_BANK, HW_OCOTP_MAC1_ROW, mac1_reg);

    if ( ocotp_sense_fuse(HW_OCOTP_MAC0_BANK, HW_OCOTP_MAC0_ROW) != mac0_reg ||
    	 ocotp_sense_fuse(HW_OCOTP_MAC1_BANK, HW_OCOTP_MAC1_ROW) != mac1_reg )
    {
        printf("%s  ** Fuse read-back-verify failed.\n", indent);
        printf("%s  ** Read back: 0x%08X, 0x%08X\n", indent,
        		ocotp_sense_fuse(HW_OCOTP_MAC0_BANK, HW_OCOTP_MAC0_ROW),
        		ocotp_sense_fuse(HW_OCOTP_MAC1_BANK, HW_OCOTP_MAC1_ROW));
        printf("%s  ** Should be: 0x%08X, 0x%08X\n", indent, mac0_reg, mac1_reg);

        /* TODO: Is it still needed? => ask design */
        /* Disable fuse programming in the CCM CGPR register by clearing the 'efuse_prog_supply_gate' bit */
        temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
        temp &= ~(0x00000010);  // clear bit 4, efuse_prog_supply_gate bit
        writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

        return TEST_FAILED;
    }

    debug_printf("%s  ** Fuses programmed successfully.\n\n", indent);
    debug_printf("%s  ** MAC Address Fuses after program: 0x%08X, 0x%08X\n",
    		indent, HW_OCOTP_MAC0_RD(), HW_OCOTP_MAC1_RD());

    /* reload the otp shadow registers */
    ocotp_reload_otp_shadow_registers();

    debug_printf("%s  ** MAC Address Fuses after reload: 0x%08X, 0x%08X\n",
    		indent, HW_OCOTP_MAC0_RD(), HW_OCOTP_MAC1_RD());

    /* TODO: Is it still needed? => ask design */
    /* Disable fuse programming in the CCM CGPR register by clearing the 'efuse_prog_supply_gate' bit */
    temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
    temp &= ~(0x00000010);      // clear bit 4, efuse_prog_supply_gate bit
    writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

    return TEST_PASSED;
}

/* Program the Ethernet MAC Address fuses if not already done */
test_return_t program_mac_address(void)
{
	test_return_t result = TEST_NOT_STARTED;
	const char* const indent = menu_get_indent();

    //
    // Get existing MAC Address
    //
    char existing_mac_addr[6];
    read_mac_otp_registers(existing_mac_addr);
    printf("\n");
    printf("%sExisting MAC address: %s\n", indent, mac_address_str(existing_mac_addr));

    if ((existing_mac_addr[0] == 0) && (existing_mac_addr[1] == 0) && (existing_mac_addr[2] == 0) &&
        (existing_mac_addr[3] == 0) && (existing_mac_addr[4] == 0) && (existing_mac_addr[5] == 0))
    {
        printf("\n");
        printf("%sThe MAC address has not been programmed.\n", indent);
        printf("%sDo you want to program the MAC address?\n", indent);
        if (!is_input_char('y', indent))
        {
            /* Do not program the suggested value to Board ID fuse bank */
            printf("%sNOT programming MAC address to fuses.\n", indent);

            return TEST_BYPASSED;
        }

retry_enter_mac_address:
        // now try to read/set mac
		printf("\n");
        printf("%sEnter MAC address \"MOST SIGNIFICANT BYTE FIRST\"\n", indent);
        printf("%sEnter MAC in the form of [010203040506] then hit enter:\n", indent);
        char user_mac_addr[6], key_pressed, tmp1, tmp2, i = 0;
        do
        {
        	while ((key_pressed = getchar()) == NONE_CHAR);

        	if (key_pressed >= '0' && key_pressed <= '9') {
				tmp1 = key_pressed - '0';
			} else if (key_pressed >= 'a' && key_pressed <= 'f') {
				tmp1 = key_pressed - 'a' + 10;
			} else if (key_pressed >= 'A' && key_pressed <= 'F') {
				tmp1 = key_pressed - 'A' + 10;
			} else {
				if ((key_pressed == '\n') || (key_pressed == '\r'))
					break;
				else {
					printf("\n%sNot a valid input, valid inputs are [0-9,a-f,A-F]\n", indent);
					continue;
				}
			}
			fputc(key_pressed, stdout);
			//printf("%c", recv_ch);
			if ((i & 1) == 0) {
				tmp2 = tmp1;    // save MSB byte
			} else {
				if (i < 12) {
					// only store 6 bytes. if user enters more, just ignore.
					user_mac_addr[i / 2] = tmp2 * 16 + tmp1;
				}
				if (i < 10) {
					//send formatting
					fputc(' ', stdout);
					fputc(':', stdout);
					fputc(' ', stdout);
				}
			}
			i++;
        } while (1);

        if ( key_pressed != '\r' && key_pressed != '\n' )
        	printf("\n");

        printf("\n%sMAC address entered: %s\n", indent, mac_address_str(user_mac_addr));

        printf("\n");
        printf_color(g_TextAttributeBold, g_TextColorRed,
        		"%sContinue with irreversible operation to program the MAC address?\n", indent);
        if (!is_input_char('y', indent))
        {
            /* Give user a chance to reenter the MAC address */
        	printf("\n%sDo you want to reenter the MAC address?\n", indent);
            if (is_input_char('y', indent))
            {
                goto retry_enter_mac_address;
            }
            else
            {
                /* Do not program the entered value to MAC address fuse bank */
            	printf("%sNOT programming the MAC address to fuses.\n", indent);

            	return TEST_BYPASSED;
            }
        }

        // DO THE PROGRAMMING
        result = program_mac_address_fuses(user_mac_addr, indent);
    }
    else
    {
        printf("%sIs this the correct MAC address\n", indent);
        if (is_input_char('y', indent))
        	result = TEST_PASSED;
    }

    if (result == TEST_PASSED)
    	return TEST_PASSED;
    else
    {
	    printf("\n%sThe MAC address is incorrect.\n", indent);

	    return TEST_FAILED;
    }
}

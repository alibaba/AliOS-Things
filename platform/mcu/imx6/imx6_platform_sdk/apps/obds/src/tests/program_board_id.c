/*
 * Copyright (c) 2009-2012, Freescale Semiconductor, Inc.
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
 * @defgroup diag_init Base OBDS files
 */

/*!
 * @file  program_board_id.c
 *
 * @ingroup diag_init
 */

#include "sdk.h"
#include "obds.h"
#include "registers.h"
#include "soc_memory_map.h"
#include "system_util.h"
#include "board_id/board_id.h"
#include "ocotp/ocotp.h"

//const char * const program_board_id_test_name = "BOARD ID Test";
const char g_program_board_id_test_name[] = "BOARD ID Test";

/*!
 * Program the board type and revision efuse
 *
 * @return  0 if successful; non-zero otherwise
 */
test_return_t program_board_id_fuses(const uint32_t board_type, const uint32_t board_rev, const char* const indent)
{
    const uint32_t reg_word = ((board_type & 0xf) << 12) | ((board_rev & 0xF) << 8);

    /* TODO: Is it still needed? => ask design */
    /* First, allow fuse programming in the CCM CGPR register by setting the 'efuse_prog_supply_gate' bit */
    unsigned int temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
    temp |= 0x00000010;         // set bit 4, efuse_prog_supply_gate bit
    writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

    debug_printf("%s  ** Board ID Fuses before program 0x%x\n", indent,
            (get_board_id().B.BOARD_TYPE_ID << 4) | get_board_id().B.BOARD_REV);

    /* blow the fuse */
    ocotp_fuse_blow_row(HW_OCOTP_GP1_BANK, HW_OCOTP_GP1_ROW, reg_word);
    if (ocotp_sense_fuse(HW_OCOTP_GP1_BANK, HW_OCOTP_GP1_ROW) != reg_word) {
        printf("%s  ** Fuse read-back-verify failed, read back 0x%x, should be: 0x%x\n", indent,
                ocotp_sense_fuse(HW_OCOTP_GP1_BANK, HW_OCOTP_GP1_ROW), reg_word);

        /* TODO: Is it still needed? => ask design */
        /* Disable fuse programming in the CCM CGPR register by clearing the 'efuse_prog_supply_gate' bit */
        temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
        temp &= ~(0x00000010);  // clear bit 4, efuse_prog_supply_gate bit
        writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

        return TEST_FAILED;
    }

    debug_printf("%s  ** Fuses programmed successfully.\n\n", indent);
    debug_printf("%s  ** Board ID Fuses after program 0x%x\n", indent,
            (get_board_id().B.BOARD_TYPE_ID << 4) | get_board_id().B.BOARD_REV);

    /* reload the otp shadow registers */
    ocotp_reload_otp_shadow_registers();

    debug_printf("%s  ** Board ID Fuses after reload 0x%x\n", indent,
            (get_board_id().B.BOARD_TYPE_ID << 4) | get_board_id().B.BOARD_REV);

    /* TODO: Is it still needed? => ask design */
    /* Disable fuse programming in the CCM CGPR register by clearing the 'efuse_prog_supply_gate' bit */
    temp = readl(CCM_BASE_ADDR + CCM_CGPR_OFFSET);
    temp &= ~(0x00000010);      // clear bit 4, efuse_prog_supply_gate bit
    writel(temp, (CCM_BASE_ADDR + CCM_CGPR_OFFSET));

    return TEST_PASSED;
}

/* Program the board ID fuses if not already done */
test_return_t program_board_id(void)
{
    const char* const indent = menu_get_indent();

    char chip_str[64];
    char chip_str_full[64];
    char chip_rev_str[64];
    char board_str[64];
    char board_rev_str[64];

    fsl_board_id_t board_id = get_board_id();

    chip_name(chip_str, board_id.B.CHIP_TYPE_ID, false);
    chip_name(chip_str_full, board_id.B.CHIP_TYPE_ID, true);
    chip_revision(chip_rev_str, board_id.B.CHIP_REV_MAJOR, board_id.B.CHIP_REV_MINOR);
    board_name(board_str, board_id.B.BOARD_TYPE_ID);
    board_revision(board_rev_str, board_id.B.BOARD_REV);

    uint32_t fw_board_type = BOARD_TYPE;
    uint32_t fw_board_rev = BOARD_REVISION;

    /* Special case for MX6SDL/DQ SMART_DEVICE/SABRE_AI REV_B boards.
     * Allow user to choose to program the board as a REV_C or a REV_BX board */
    if ((CHIP_TYPE == CHIP_TYPE_MX6SDL || CHIP_TYPE == CHIP_TYPE_MX6DQ) &&
        (BOARD_TYPE == BOARD_TYPE_SABRE_AI || BOARD_TYPE == BOARD_TYPE_SMART_DEVICE) &&
         BOARD_REVISION == BOARD_REVISION_B)
    {
        printf("%sThis binary will work for REV_D, REV_C, REV_BX, and REV_B boards.\n", indent);
        printf("%sWould you like to use the REV_D board ID for this board?\n", indent);
        if (is_input_char('y', indent)) {
            printf("%sUsing REV_D board ID instead of REV_B board ID.\n\n", indent);
            fw_board_rev = BOARD_REVISION_D;
        }
        else {
            printf("%sWould you like to use the REV_C board ID for this board?\n", indent);
            if (is_input_char('y', indent)) {
                printf("%sUsing REV_C board ID instead of REV_B board ID.\n\n", indent);
                fw_board_rev = BOARD_REVISION_C;
            }
            else if(BOARD_TYPE == BOARD_TYPE_SABRE_AI){
                printf("%sWould you like to use the REV_BX board ID for this board?\n", indent);
                if (is_input_char('y', indent)) {
                    printf("%sUsing REV_BX board ID instead of REV_B board ID.\n\n", indent);
                    fw_board_rev = BOARD_REVISION_BX;
                }
            }
        }
    }

    /* Special case for MX6SL EVB/EVK boards.
     * Allow user to choose to program the board as a EVK_EPDC or EVK_SPDC board */
    if (CHIP_TYPE == CHIP_TYPE_MX6SL)
    {
        printf("%sThis binary will work for both EVB and EVK boards.\n", indent);
        printf("%sWould you like to use the EVK board ID for this board?\n", indent);
        if (is_input_char('y', indent))
        {
            printf("%sUsing BOARD_TYPE_EVK board ID instead of BOARD_TYPE_EVB board ID.\n\n", indent);
            printf("%sUsing BOARD_REVISION_A board ID instead of BOARD_REVISION_B board ID.\n\n", indent);
            fw_board_type = BOARD_TYPE_EVK;
            fw_board_rev = BOARD_REVISION_A;
        }
    }

    if (board_id.B.BOARD_TYPE_ID == 0 || board_id.B.BOARD_REV == 0)
    {
        // Board is not programmed
        printf("%sNo Board ID found in the fuses.\n", indent);
        board_id.B.BOARD_TYPE_ID = fw_board_type;
        board_id.B.BOARD_REV = fw_board_rev;

        printf("%sThis OBDS binary will program Board ID to 0x%08X as detailed below:\n", indent, board_id.U);
        show_board_id(board_id, indent);

        printf_color(g_TextAttributeBold, g_TextColorRed, "%sContinue with irreversible operation to program Board ID?\n", indent);
        if (!is_input_char('y', indent))
        {
            /* Do not program the suggested value to Board ID fuse bank */
            printf("%sNOT programming board ID to fuses.\n", indent);

            return TEST_BYPASSED;
        }

        // DO THE PROGRAMMING
        test_return_t err = program_board_id_fuses(board_id.B.BOARD_TYPE_ID, board_id.B.BOARD_REV, indent);
        if (err == TEST_PASSED)
        {
            printf("%sFuses programmed successfully.\n\n", indent);
            show_board_id(get_board_id(), indent);
        }

        return err;
    }
    else if ((board_id.B.BOARD_TYPE_ID == BOARD_TYPE_SABRE_AI || board_id.B.BOARD_REV == BOARD_REVISION_B) &&
             (fw_board_rev == BOARD_REVISION_BX) )
    {
        // SABRE AI Board is programmed as REV_B , but we need it to be REV_BX
        printf("\n%sSABRE AI Rev. B board ID found in the fuses.\n", indent);
        board_id.B.BOARD_TYPE_ID = fw_board_type;
        board_id.B.BOARD_REV = fw_board_rev;

        printf("%sThis OBDS binary will program Board ID to 0x%08X as detailed below:\n", indent, board_id.U);
        show_board_id(board_id, indent);
        printf("\n");
        printf_color(g_TextAttributeBold, g_TextColorRed, "%sContinue with irreversible operation to program Board ID?\n", indent);
        if (!is_input_char('y', indent))
        {
            /* Do not program the suggested value to Board ID fuse bank */
            printf("%sNOT programming board ID to fuses.\n", indent);

            return TEST_BYPASSED;
        }

        // DO THE PROGRAMMING
        test_return_t err = program_board_id_fuses(board_id.B.BOARD_TYPE_ID, board_id.B.BOARD_REV, indent);
        if (err == TEST_PASSED)
        {
            printf("%sFuses programmed successfully.\n\n", indent);
            show_board_id(get_board_id(), indent);
        }

        return err;
    }
    else if ( board_id.B.CHIP_TYPE_ID  != CHIP_TYPE     ||
              board_id.B.BOARD_TYPE_ID != fw_board_type ||
              board_id.B.BOARD_REV     != fw_board_rev  )
    {
        // Software is not compiled for this board
        printf("\n%sSoftware is not compiled for this board.\n", indent);

        chip_name(chip_str, CHIP_TYPE, false);
        board_name(board_str, fw_board_type);
        board_revision(board_rev_str, fw_board_rev);
        printf("%sSoftware for %s %s %s.\n", indent, chip_str, board_str, board_rev_str);

        chip_name(chip_str, board_id.B.CHIP_TYPE_ID, false);
        board_name(board_str, board_id.B.BOARD_TYPE_ID);
        board_revision(board_rev_str, board_id.B.BOARD_REV);
        printf("%sBoard is %s %s %s.\n", indent, chip_str, board_str, board_rev_str);

        return TEST_FAILED;
    }
    else if ( board_id.B.CHIP_TYPE_ID  == CHIP_TYPE     &&
              board_id.B.BOARD_TYPE_ID == fw_board_type &&
              board_id.B.BOARD_REV     == fw_board_rev  )
    {
        // Board ID fuses are already correct
        printf("\n%sBoard ID fuses are already programmed.\n\n", indent);
        show_board_id(board_id, indent);

        return TEST_PASSED;
    }
    else
    {
        // Unhandled cases
        printf("\n%sUnknown failure.\n\n", indent);
        show_board_id(board_id, indent);

        return TEST_FAILED;
    }
}

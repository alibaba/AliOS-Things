/*
 * Copyright (C) 2010-2012, Freescale Semiconductor, Inc. All Rights Reserved
 * THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
 * BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc.
 */

/*Test MX53 Kilby Base Board keyboard*/
#include <stdio.h>
#include "obds.h"
#include "utility/menu.h"

const char g_keypad_port_test_name[] = "Keypad Port Test";

/*16-bit register*/
#define KPP_KPCR        KPP_BASE_ADDR
#define KPP_KPSR        KPP_BASE_ADDR + 0x02
#define KPP_KDDR        KPP_BASE_ADDR + 0x04
#define KPP_KPDR        KPP_BASE_ADDR + 0x06

#define COL_NUM     4
#define ROW_NUM     4

#define KPKD        0
#define KPKR        1
#define KDSC        2
#define KRSS        3
#define KDIE        8
#define KRIE        9

uint32_t dc3 = 0;

void kpp_enable_rows(void)
{
    int i;
    for (i = 0; i < ROW_NUM; i++)
        reg16setbit(KPP_KPCR, i);
}

void kpp_set_cols_opendrain(void)
{
    int i;
    for (i = 0; i < COL_NUM; i++)
        reg16setbit(KPP_KPCR, i + 8);
}

void kpp_set_cols_totempole(void)
{
    int i;
    for (i = 0; i < COL_NUM; i++)
        reg16clrbit(KPP_KPCR, i + 8);
}

void kpp_set_data_dir(void)
{
    int i;
    /*columns as output */
    for (i = 0; i < COL_NUM; i++)
        reg16setbit(KPP_KDDR, i + 8);

    /*rows as input */
    for (i = 0; i < ROW_NUM; i++)
        reg16clrbit(KPP_KDDR, i);
}

void kpp_clr_cols_data(void)
{
    int i;
    for (i = 0; i < COL_NUM; i++)
        reg16clrbit(KPP_KPDR, i + 8);
}

void kpp_set_cols_data(void)
{
    int i;
    for (i = 0; i < COL_NUM; i++)
        reg16setbit(KPP_KPDR, i + 8);
}

int kpp_detect_keypad_pressed(void)
{
    return (reg16_read(KPP_KPSR) & (0x1 << KPKD));
}

int kpp_detect_keypad_released(void)
{
    return (reg16_read(KPP_KPSR) & (0x1 << KPKR));
}

void kpp_clr_keypad_int(void)
{
    reg16setbit(KPP_KPSR, KPKD);    /*write 1 to clear */
}

void kpp_scan_matrix(uint16_t * matrix_state)
{
    int col, reg_val;

    /* 1 disable both (depress and release interrupt) */
    reg16clrbit(KPP_KPSR, KDIE);    /*disable keypad depressed interrupt */
    reg16clrbit(KPP_KPSR, KRIE);    /*disable keypad released interrupt */

    for (col = 0; col < COL_NUM; col++) {
        /* 2 setting column data to 1s
         * 3 config columns as totem pole to discharge capcitance 
         * 4 config columns as open drain */
        kpp_set_cols_data();

        kpp_set_cols_totempole();
        hal_delay_us(10);

        kpp_set_cols_opendrain();
        hal_delay_us(10);

        /* 5 write a single column to 0, others to 1 */
        reg16clrbit(KPP_KPDR, col + 8);

        /* Delay added to avoid propagating the 0 from column to row when scanning. */
        hal_delay_us(10);

        /* 6 sample row inputs and save data */
        /* 1s in matrix_state[col] means key pressures (multiple press detection) */
        reg_val = reg16_read(KPP_KPDR);
        matrix_state[col] = (~reg_val) & 0xF;

        /* 7. Repeat steps 2 - 6 for remaining columns. */
    }

    /* 8 return all columns to 0 for standby mode
     * 9 clear KPKD and KPKR by writing to a '1'; 
     *   set KPKR synchronizer chain by writing 1 to KRSS;
     *   clear KPKD synchronizer chain by writing 1 to KDSC
     * 10 re-enable kepay interrupt write 0s to columns*/
    kpp_clr_cols_data();

    reg16setbit(KPP_KPSR, KPKD);
    reg16setbit(KPP_KPSR, KPKR);
    reg16setbit(KPP_KPSR, KRSS);
    reg16setbit(KPP_KPSR, KDSC);

    reg16setbit(KPP_KPSR, KDIE);    /*enable keypad depressed interrupt */
    reg16clrbit(KPP_KPSR, KRIE);    /*enable keypad depressed interrupt */
}

void kpp_print_key_press_on_dc3(int row, int col)
{
    const char* indent = menu_get_indent();

    if (row == 0 && col == 0)
        printf("%sSELECT on DC3\n", indent);
    if (row == 0 && col == 1)
        printf("%sBACK on DC3\n", indent);
    if (row == 0 && col == 2)
        printf("%sF1 on DC3\n", indent);
    if (row == 0 && col == 3)
        printf("%sF2 on DC3\n", indent);

    if (row == 1 && col == 0)
        printf("%sF3 on DC3\n", indent);
    if (row == 1 && col == 1)
        printf("%sF4 on DC3\n", indent);
    if (row == 1 && col == 2)
        printf("%sF5 on DC3\n", indent);
    if (row == 1 && col == 3)
        printf("%sMENU on DC3\n", indent);

    if (row == 2 && col == 0)
        printf("%sPREV on DC3\n", indent);
    if (row == 2 && col == 1)
        printf("%sNXT1 on DC3\n", indent);
    if (row == 2 && col == 2)
        printf("%sHOME on DC3\n", indent);
    if (row == 2 && col == 3)
        printf("%sNXT2 on DC3\n", indent);

    if (row == 3 && col == 0)
        printf("%sUP on DC3\n", indent);
    if (row == 3 && col == 1)
        printf("%sLEFT on DC3\n", indent);
    if (row == 3 && col == 2)
        printf("%sRIGHT on DC3\n", indent);
    if (row == 3 && col == 3)
        printf("%sDOWN on DC3\n", indent);
}

void kpp_print_key_press_on_evk(int row, int col)
{
    const char* indent = menu_get_indent();

    if (row == 0 && col == 0)
        printf("%sSW6 on EVK\n", indent);
    if (row == 0 && col == 1)
        printf("%sSW7 on EVK\n", indent);
    if (row == 0 && col == 2)
        printf("%sSW8 on EVK\n", indent);

    if (row == 1 && col == 0)
        printf("%sSW9 on EVK\n", indent);
    if (row == 1 && col == 1)
        printf("%sSW10 on EVK\n", indent);
    if (row == 1 && col == 2)
        printf("%sSW11 on EVK\n", indent);

    if (row == 2 && col == 0)
        printf("%sSW12 on EVK\n", indent);
    if (row == 2 && col == 1)
        printf("%sSW13 on EVK\n", indent);
}

/* return 1 if "Back" is press
 * otherwise return 0*/
int kpp_trace_keypad_status(uint16_t * matrix_state, uint32_t press_num[][COL_NUM])
{
    int row, col;
    for (col = 0; col < COL_NUM; col++) {
        for (row = 0; row < ROW_NUM; row++) {
            if (matrix_state[col] & (0x1 << row)) {
                press_num[row][col]++;  /*key pressed */
            } else {
                /*key release */
                if (press_num[row][col] > 4) {
                    if (dc3 == 0) {
                        kpp_print_key_press_on_evk(row, col);
                    } else {
                        kpp_print_key_press_on_dc3(row, col);
                    }
                    if (row == 0 && col == 1) {
                        printf("exit\n");
                        return 1;
                    }
                }
                press_num[row][col] = 0;
            }
        }
    }

//    memset((void *)matrix_state, 0x00, 2 * COL_NUM);

    return 0;
}

static void kpp_keypad_init(void)
{
    /* 1 enable number of rows in keypad KPP_KPCR[KRE]
     * 2 write 0s to KPP_KPDR[KCD]
     * 3 config columns as open drain KPP_KPCR[KCO]
     * 4 config columns as output and rows as input (KPP_KDDR)*/
    kpp_enable_rows();
    kpp_set_cols_opendrain();
    hal_delay_us(10);
    kpp_set_data_dir();

    kpp_clr_cols_data();

    /* 5 write 1 to clear KPKD status flags and synchornizer chain */
    reg16setbit(KPP_KPSR, KPKD);    /*write 1 to clear keypad pressed interrupt */
    reg16setbit(KPP_KPSR, KDSC);    /*set 1 to clear keypad depress sync chain */
    /* 6 set KDIE and clear KRIE (avoid false release event) */
    reg16setbit(KPP_KPSR, KRSS);    /*clear release synchornizer sync chain */
    reg16setbit(KPP_KPSR, KDIE);    /*enable keypad depressed interrupt */
    reg16clrbit(KPP_KPSR, KRIE);    /*disable keypad released interrupt */

    /* 7 KPP in now in standby mode */
}

test_return_t test_gpio_keyboard(void)
{
    uint16_t matrix_state[COL_NUM];
    uint32_t press_num[ROW_NUM][COL_NUM];
    const char* indent = menu_get_indent();

    printf("%sDo you plug the EVK board on DC3 (add_on board3)?", indent);
    if (is_input_char('y', indent)) {
        dc3 = 1;
    } else {
        dc3 = 0;
    }

    kpp_iomux_config();

    kpp_keypad_init();

    if (dc3 == 1)
        printf("%sPlease push buttons (BACK to exit)\n", indent);
    else
        printf("%sPlease push buttons (SW7 to exit)\n", indent);
    memset((void *)matrix_state, 0x00, sizeof(matrix_state));
    memset((void *)press_num, 0x00, sizeof(press_num));
    do {
        /*this test is using enrolling style */
        kpp_scan_matrix(matrix_state);
        if (kpp_trace_keypad_status(matrix_state, press_num))
            break;
    } while (1);

    printf("%sDoes keyboard work? [y/n] \n", indent);
    if (!is_input_char('y', indent)) {
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

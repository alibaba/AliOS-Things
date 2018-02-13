/**
 * File: efc_si.c
 * Brief: Implementation of Sanechips efc
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*******************************************************************************
 *                             Include header files                               *
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <soc.h>
#include "zx29_efc.h"
//#include <io.h>
#include <drv_eflash.h>
#include <drv_intc.h>

/*******************************************************************************
 *                               Macro definitions                               *
 ******************************************************************************/
#define THIS_MODULE                MODULE_DEV_EFLASH

//#define EFLASH_DEBUG_DEF
#ifdef EFLASH_DEBUG_DEF
#define EFC_DEBUG_LOG(fmt, ...) EFC_DEBUG_LOG(fmt, ##__VA_ARGS__)
#else
#define EFC_DEBUG_LOG(fmt, ...)
#endif

#define ERR_EFLASH(errno) (CSI_DRV_ERRNO_EFLASH_BASE | errno)
#define EFLASH_NULL_PARAM_CHK(para)                         \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_EFLASH(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

/* Driver Capabilities */
static const eflash_capabilities_t driver_capabilities = {
    .event_ready = 1, /* event_ready */
    .data_width = 2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    .erase_chip = 1  /* erase_chip */
};
extern int32_t target_get_eflash(int32_t idx, uint32_t *base, uint32_t *irq, eflash_info_t *info);
static int32_t efc_si_lock(enum efc_si_dev_num efc_id);
static int32_t efc_si_unlock(enum efc_si_dev_num efc_id);

extern int32_t target_get_eflash(int32_t idx, uint32_t *base, uint32_t *irq, eflash_info_t *info);

/*******************************************************************************
 *                           Local variable definitions                           *
 ******************************************************************************/
static efc_si_dev_data efc_si_dev[MAX_EFLASHDEV_NUM] = {
    {0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {0, 0, 0, 0, 0, 0, 0, NULL, NULL}
};
#if 0
static uint32_t efc_si_otp_addr[MAX_EFLASHDEV_NUM][MAX_INFOPAGE_NUM] = {
    {EFLASH1_PAGE0_OTP, EFLASH1_PAGE1_OTP},
    {EFLASH2_PAGE0_OTP, EFLASH2_PAGE1_OTP}
};
#endif
//extern int is_amt_mode(void);

static eflash_info_t info[MAX_EFLASHDEV_NUM];
static uint8_t eflash1_buffer[EFLASH1_PAGE_SIZE];
static uint8_t eflash2_buffer[EFLASH2_PAGE_SIZE];

static uint8_t datareq_flag = 0;
static uint8_t trans_flag = 0;

/*******************************************************************************
 *                        Local function implementations                           *
 ******************************************************************************/
/**
 *
 * find the first 1 beginning position of value
 *
 *
 * @param[in]    x        the value
 *
 *
 *
 * @return    the first 1 beginning position
 *
 */

static int is_amt_mode(void)
{
    return 1;
}
static inline uint32_t efc_si_ffs(uint32_t x)
{
    uint32_t r = 1;

    if (!x) {
        return 0;
    }

    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }

    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }

    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }

    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }

    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }

    return r;
}

/**
 *
 * wait for eflash finish all operation, go to   idle
 *
 *
 * @param[in]    efc_id    eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_wait_idle(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*wait for done*/
    while (efc_reg->start_status & EFC_START_BUSY_FLAG);
}

/**
 *
 * eflash 1/2 password
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_password(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    efc_si_wait_idle(efc_id);
    /*step1, password*/
    efc_reg->password = OP_EN_PASSWORD;

    /*step2 wait for write enable*/
    while (!(efc_reg->start_status & EFC_WRITE_ENABLE));
}

/**
 *
 * set eflash operation characters
 *
 *
 * @param[in]    efc_id            eflash 1/2
 * @param[in]    opera_code         operation type
 * @param[in]    opera_addr        operation addr,just need for page_erase and program
 * @param[in]    opera_len        just need for program, 0 for single program, >0 for fast program
 *
 * @return    void
 *
 */
static void efc_si_opera_set(enum efc_si_dev_num efc_id, enum efc_si_opera_code opera_code,
                             uint32_t opera_addr, uint32_t opera_len)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*step3, setting program  operation */
    efc_reg->op_code = opera_code;
    /*step4, set program addr*/
    efc_reg->op_addr = opera_addr;
    /*step5, set the program length*/
    efc_reg->op_len = opera_len;
}

/**
 *
 * set the program data
 *
 *
 * @param[in]    efc_id            eflash 1/2
 * @param[in]    data                the pointer to src data
 *
 *
 * @return    void
 *
 */
static void efc_si_opera_data(enum efc_si_dev_num efc_id, uint8_t *data)
{
    efc_si_regs *efc_reg = NULL;
    uint32_t temp;
    uint32_t i = 0;

    efc_reg = efc_si_dev[efc_id].reg;

    /*step6, set the program data*/
    for (i = 0; i < (efc_si_dev[efc_id].write_allign_size >> 2); i ++) {
        temp = 0x0;
        temp |= (*(data)) << 0;
        data++;
        temp |= (*(data)) << 8;
        data++;
        temp |= (*(data)) << 16;
        data++;
        temp |= (*(data)) << 24;
        data++;
        efc_reg->data[i] = temp;
    }
}

/**
 *
 * start eflash operation
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_opera_start(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*step7, wait fot freedom */
    efc_si_wait_idle(efc_id);
    /*step8, start program*/
    efc_reg->start_status |= EFC_START_BUSY_FLAG;
}

/**
 *
 * wait for data is taken in fast program
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_wait_data_req(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*wait for data taken*/
    while (!(efc_reg->start_status & EFC_DATA_REQ));

    while (!(efc_reg->int_status & EFC_INT_DATA_REQ));

    /*clean  the  data req interrupt*/
    efc_reg->int_status |= EFC_INT_DATA_REQ;
}

/**
 *
 * wait for eflash operation is finished
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_wait_trans_done(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /* wait  trans done int */
    while (!(efc_reg->int_status & EFC_INT_TRANS_DONE));

    /* step10 clean trans done int */
    efc_reg->int_status |= EFC_INT_TRANS_DONE;
}

/**
 *
 * send the ack singnal for valid data
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_send_data_ack(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /* send  a acknowlege to data ack*/
    efc_reg->start_status |= EFC_DATA_ACK;
}

/**
 *
 * clean programe len,need for fast programe
 *
 *
 * @param[in]    efc_id            eflash 1/2
 *
 *
 *
 * @return    void
 *
 */
static void efc_si_clean_opera_len(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*clean the operate length*/
    efc_reg->op_len = 0;
}

/**
 *
 *the core have rbuffer between eflash,clear CK802 buffer
 *
 * @param[in]
 * @param[in]
 *
 *
 * @return         void
 *
*/
static void efc_si_clean_buffer(void)
{
    /*the core have buffer between eflash,clear CK802 buffer*/
    efc_si_dev[0].reg->buf_ctrl |= EFC_CLR_CK802_DBUF;
    efc_si_dev[0].reg->buf_ctrl &= ~EFC_CLR_CK802_DBUF;
    efc_si_dev[0].reg->buf_ctrl |= EFC_CLR_CK802_IBUF;
    efc_si_dev[0].reg->buf_ctrl &= ~EFC_CLR_CK802_IBUF;
}

/**
 *
 *the core have rbuffer between eflash,enble/disable prefetch
 *
 * @param[in]    core_type     dsp/ck802/m0
 * @param[in]    is_prefetch   1:enable,0:disable
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
static int efc_si_bufpre_set(enum efc_si_core_type core_type, uint8_t is_prefetch)
{
    efc_si_regs *efc_reg = NULL;

    if (core_type >= MAX_EFC_CORE) {
        EFC_DEBUG_LOG("efc_si_bufpre_set:core_type =  is invalid.\n");
        return -EINVAL;
    }

    efc_reg =  efc_si_dev[EFLASH_DEV1].reg;

    if (is_prefetch) {
        HAL_BIT_SET(efc_reg->buf_ctrl, core_type);
    } else {
        HAL_BIT_CLR(efc_reg->buf_ctrl, core_type);
    }

    return 0;
}

/**
 * the core have rbuffer between eflash,enble/disable prefetch
 *
 *
 * @param[in]    core_type    dsp/ck802/m0
 * @param[in]is_prefetch   1:enable,0:disable
 *
 *
 * @return     0 on success; -1 on failure
 *
*/
int yunos_bsp_eflash_bufpre_set(enum efc_si_core_type core_type, uint8_t is_prefetch)
{
    return efc_si_bufpre_set(core_type, is_prefetch);
}

/**
 *
 *check the erased page,success or failed.
 *
 * @param[in]    efc_id    eflash 1/2
 * @param[in]    addr        the erase address
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_erase_page_check(enum efc_si_dev_num efc_id, uint32_t addr)
{
    uint32_t cnt = 0;
    uint8_t com_val = 0xff;
    uint8_t *value = NULL;
    uint32_t page_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_erase_page_check: efc_id=%d  is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (!IS_EFC_ADDR(addr)) {
        EFC_DEBUG_LOG("efc_si_erase_page_check:addr=0x%x is out eflash range.\n", addr);
        return -EINVAL;
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    for (cnt = 0, value = (uint8_t *)(page_addr);
         cnt < efc_si_dev[efc_id].page_size; cnt++, value++) {
        com_val &= *value;

        if (com_val != 0xff) {
            EFC_DEBUG_LOG("efc_si_erase_page_check addr=0x%x,com_val=0x%x clear failed.\n",
                          (uint32_t)value, com_val);
            return -EPERM;
        }
    }

    return 0;
}

/**
 *
 * erase one page.
 *
 * @param[in]    efc_id    eflash 1/2
 * @param[in]    addr        the erase address
 *
 *
 * @return     0 on success; -1 on failure
 *
*/
static int32_t efc_si_erase_onepage(enum efc_si_dev_num efc_id, uint32_t addr)
{
    int32_t ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_erase_onepage:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (!IS_EFC_ADDR(addr)) {
        EFC_DEBUG_LOG("efc_si_erase_onepage:addr=0x%x is out eflash range.\n", addr);
        return -EINVAL;
    }

    efc_si_password(efc_id);

    efc_si_opera_set(efc_id, OP_CODE_PAGE_ERASE, addr, 0);

    efc_si_opera_start(efc_id);

    if (efc_si_dev[efc_id].poll_mode) {
        efc_si_wait_trans_done(efc_id);
        EFC_DEBUG_LOG("pageerase polling mode.\n");
    } else {
//        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem), YUNOS_WAIT_FOREVER);
        while (trans_flag == 0);

        trans_flag = 0;
        EFC_DEBUG_LOG("pageerase irq mode.\n");
    }

    efc_si_wait_idle(efc_id);

    ret = efc_si_erase_page_check(efc_id, addr);

    return ret;
}

/**
 *
 * erase efalsh1/2 all main falsh memory pages,information block is not affected.
 *
 * @param[in]    efc_id     eflash 1/2
 *
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_erase_macro(enum efc_si_dev_num efc_id)
{
    int ret = 0;
    uint32_t cnt = 0;
    uint32_t page_nums = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_erase_macro:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    efc_si_password(efc_id);

    efc_si_opera_set(efc_id, OP_CODE_MACRO_ERASE, 0, 0);

    efc_si_opera_start(efc_id);

    if (efc_si_dev[efc_id].poll_mode) {
        efc_si_wait_trans_done(efc_id);
        EFC_DEBUG_LOG("macroerase polling mode.\n");
    } else {
//        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem), YUNOS_WAIT_FOREVER);
        while (trans_flag == 0);

        trans_flag = 0;
        EFC_DEBUG_LOG("macroerase irq mode.\n");
    }

    efc_si_wait_idle(efc_id);

    page_nums = (efc_si_dev[efc_id].main_size >> (efc_si_ffs(efc_si_dev[efc_id].page_size) - 1));

    for (cnt = 0; cnt < page_nums; cnt ++) {
        ret = efc_si_erase_page_check(efc_id, cnt * efc_si_dev[efc_id].page_size + efc_si_dev[efc_id].main_base);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_erase_macro:efc_si_erase_page_check failed!.\n");
            return ret;
        }
    }

    return 0;
}

/**
 *
 * erase many pages begain with the address in main eflash block
 *
 * @param[in]    efc_id     eflash 1/2
 * @param[in]    addr        begining address of the erase pages
 * @param[in]    nums     the nums of erase pages
 *
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_erase_pages(enum efc_si_dev_num efc_id, uint32_t addr, uint32_t nums)
{
    int ret = 0;

    while (nums) {
        ret = efc_si_erase_onepage(efc_id, addr);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_erase_pages:efc_si_erase_onepage failed.\n");
            return ret;
        }

        addr += efc_si_dev[efc_id].page_size;
        nums --;
    }

    return 0;
}

/**
 *
 * erase many pages begain with the address in main eflash block
 *
 * @param[in]    addr        begining address of the erase pages
 * @param[in]    nums     the nums of erase pages
 *
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_main_erase(uint32_t addr, uint32_t nums)
{
    int ret = 0;
    uint32_t page_addr = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_main_erase:addr=0x%x is out eflash main range.\n", addr);
        return -EINVAL;
    }

    page_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);

    if ((page_addr + nums * efc_si_dev[efc_id].page_size)
        > (efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size)) {
        EFC_DEBUG_LOG("efc_si_main_erase:erase pages addr=0x%x,nums=%d out of range.\n", addr, nums);
        return -EINVAL;
    }

    if (0 == nums) {
        EFC_DEBUG_LOG("efc_si_main_erase:nums=0.\n");
        return 0;
    }

    ret = efc_si_erase_pages(efc_id, addr, nums);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_main_erase:efc_si_erase_pages falied.\n");
        return ret;
    }

    return 0;
}

/**
 *
 * erase many pages begain with the address in information eflash block
 *
 * @param[in]    addr        begining address of the erase pages
 * @param[in]    nums    the nums of erase pages
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
#if 0
static int32_t efc_si_info_erase(uint32_t addr, uint32_t nums)
{
    int ret = 0;
    uint32_t page_addr = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_info_erase:addr=0x%x is out eflash info range.\n", addr);
        return -EINVAL;
    }

    page_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);

    if ((page_addr + nums * efc_si_dev[efc_id].page_size)
        > (efc_si_dev[efc_id].info_base + efc_si_dev[efc_id].info_size)) {
        EFC_DEBUG_LOG("efc_si_info_erase:erase pages addr=0x%x,nums=%d out of info range.\n", addr, nums);
        return -EINVAL;
    }

    if (0 == nums) {
        EFC_DEBUG_LOG("efc_si_info_erase: nums=0.\n");
        return 0;
    }

    ret = efc_si_erase_pages(efc_id, addr, nums);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_info_erase:efc_si_erase_pages falied.\n");
        return ret;
    }

    return 0;
}
#endif
/**
 *
 * write (elfsh0:16bytes,eflash1:4bytes)data into the flash device at once.
 *
 * @param[in]    efc_id        eflash 1/2
 * @param[in]    add            the dest write address
 * @param[in]    data         the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_single_write(enum efc_si_dev_num efc_id, uint32_t addr, uint8_t *data)
{
    int32_t ret = 0;
    uint8_t *src_data = data;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_single_write:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_single_write:data is NULL.\n");
        return -EINVAL;
    }

    if (!IS_EFC_ADDR(addr)) {
        EFC_DEBUG_LOG("efc_si_single_write:addr=0x%x is out eflash range.\n", addr);
        return -EINVAL;
    }

    /*addr is not allign*/
    if (addr & (efc_si_dev[efc_id].write_allign_size - 1)) {
        EFC_DEBUG_LOG("efc_si_single_write:single write addr=0x%x not alligned.\n", addr);
        return -EINVAL;
    }

    efc_si_password(efc_id);

    efc_si_opera_set(efc_id, OP_CODE_PROGRAM, addr, 0);

    efc_si_opera_data(efc_id, data);

    efc_si_opera_start(efc_id);

    if (efc_si_dev[efc_id].poll_mode) {
        efc_si_wait_trans_done(efc_id);
        EFC_DEBUG_LOG("single polling mode.\n");
    } else {
//        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem), YUNOS_WAIT_FOREVER);
        while (trans_flag == 0);

        trans_flag = 0;
        EFC_DEBUG_LOG("single irq mode.\n");
    }

    efc_si_wait_idle(efc_id);

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    ret = memcmp(src_data, (void *)addr, efc_si_dev[efc_id].write_allign_size);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_single_write:single write data error.\n");
        return -EPERM;
    }

    return 0;
}

/**
 *
 * program the start 16/4bytes into the flash when start addr is not agligned.
 *
 * @param[in]    efc_id         eflash 1/2
 * @param[in]    src_data        the pointer to the source data address
 * @param[in]    allign_addr    the 16/4bytes alligned address
 * @param[in]    pre_len        the not alligned addr(addr - allign_addr) length in byte
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_addr_alligned(enum efc_si_dev_num efc_id, uint8_t *src_data,
                                    uint32_t allign_addr, uint32_t pre_len)
{
    uint8_t tmp_buf[16];
    uint32_t i = 0;
    int32_t ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_addr_alligned:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    memcpy(tmp_buf, (void *)allign_addr, efc_si_dev[efc_id].write_allign_size);

    for (i = 0; i < (efc_si_dev[efc_id].write_allign_size - pre_len); i++) {
        tmp_buf[i + pre_len] = *(src_data++);
    }

    ret = efc_si_single_write(efc_id, allign_addr, tmp_buf);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_addr_alligned:efc_si_single_write failed.\n");
        return ret;
    }

    return 0;
}

/**
 *
 * program the  left 16/4bytes into the flash when len is not agligned.
 *
 * @param[in]    efc_id        eflash 1/2
 * @param[in]    src_data        the pointer to the source data address
 * @param[in]    left_addr        the left dest write address
 * @param[in]    left_len        the not alligned length in byte
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_len_alligned(enum efc_si_dev_num efc_id, uint8_t *src_data,
                                   uint32_t left_addr, uint32_t left_len)
{
    uint32_t i;
    uint8_t tmp_buf[16];
    int32_t ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_len_alligned:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    memcpy(tmp_buf, (void *)left_addr, efc_si_dev[efc_id].write_allign_size);

    for (i = 0; i < left_len; i ++) {
        tmp_buf[i] = *(src_data++);
    }

    ret = efc_si_single_write(efc_id, left_addr, tmp_buf);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_len_alligned:efc_si_single_write failed.\n");
        return ret;
    }

    return 0;
}

/**
 *
 * write data into the flash device in 16bytes/4bytes unit.
 *
 * @param[in]    efc_id         eflash 1/2
 * @param[in]    addr            the dest write address
 * @param[in]    len             the data length in byte
 * @param[in]    data            the pointer to the source data address
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_single_compat_write(enum efc_si_dev_num efc_id, uint32_t addr,
        uint32_t len, uint8_t *data)
{
    uint32_t j = 0;
    int32_t ret = 0;
    uint8_t *src_data = data;
    uint32_t src_addr = addr;
    uint32_t src_len = len;
    uint32_t allign_addr;
    uint32_t pre_len = 0;
    uint32_t left_len = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_single_compat_write:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_single_compat_write: data is NULL.\n");
        return -EINVAL;
    }

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_single_compat_write:len=0.\n");
        return 0;
    }

    allign_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].write_allign_size);
    pre_len = addr - allign_addr;

    /* addr is not alligned */
    if (pre_len > 0) {
        ret = efc_si_addr_alligned(efc_id, data, allign_addr, pre_len);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_single_compat_write:efc_si_addr_alligned failed.\n");
            return ret;
        }

        addr = allign_addr + efc_si_dev[efc_id].write_allign_size;
        len = len - (efc_si_dev[efc_id].write_allign_size - pre_len);
        data += (efc_si_dev[efc_id].write_allign_size - pre_len);
    }

    left_len = len % efc_si_dev[efc_id].write_allign_size;

    for (j = 0; j < (len >> (efc_si_ffs(efc_si_dev[efc_id].write_allign_size) - 1)); j ++) {
        ret = efc_si_single_write(efc_id, addr + (j * efc_si_dev[efc_id].write_allign_size),
                                  data + (j * efc_si_dev[efc_id].write_allign_size));

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_single_compat_write:efc_si_single_write failed.\n");
            return ret;
        }
    }

    /* len is not 16/4 bytes alligned*/
    if (left_len > 0) {
        ret = efc_si_len_alligned(efc_id, data + (len - left_len), addr + (len - left_len), left_len);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_single_compat_write:eflash_alligned failed.\n");
            return ret;
        }
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    ret = memcmp(src_data, (void *)src_addr, src_len);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_single_compat_write:single write data error.\n");
        return -EPERM;
    }

    return 0;
}

/**
 *
 * program 1K data into the flash device in fast write mode
 *
 * @param[in]    efc_id        eflash 1/2
 * @param[in]    addr            the dest write address
 * @param[in]    en            the program length in byte
 * @param[in]    data            the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_fast_write(enum efc_si_dev_num efc_id, uint32_t addr, uint32_t len, uint8_t *data)
{
    int32_t ret = 0;
    uint32_t j = 0;
    uint32_t left_len = 0;
    uint8_t *src_data = data;
    uint32_t src_addr = addr;
    uint32_t src_len = len;
    uint32_t line_addr;
    uint32_t allign_addr;
    uint32_t pre_len = 0;
    uint32_t opera_len = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_fast_write:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_fast_write: data is NULL.\n");
        return -EINVAL;
    }

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_fast_write:len=0.\n");
        return 0;
    }

    line_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].fast_write_size);
    allign_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].write_allign_size);

    /*judge the length is out of half page*/
    if (addr + len > (line_addr + efc_si_dev[efc_id].fast_write_size)) {
        EFC_DEBUG_LOG("efc_si_fast_write:addr=0x%x,len=%d,addr+len is out of 1K range.\n", addr, len);
        return -EINVAL;
    }

    pre_len = addr - allign_addr;

    if (pre_len > 0) {
        ret = efc_si_addr_alligned(efc_id, data, allign_addr, pre_len);

        if (ret != 0) {
            EFC_DEBUG_LOG("eflash:efc_si_addr_alligned failed.\n");
            return ret;
        }

        addr = allign_addr + efc_si_dev[efc_id].write_allign_size;
        len = len - (efc_si_dev[efc_id].write_allign_size - pre_len);
        data += (efc_si_dev[efc_id].write_allign_size - pre_len);
    }

    opera_len = (len >> (efc_si_ffs(efc_si_dev[efc_id].write_allign_size) - 1));
    left_len = len % efc_si_dev[efc_id].write_allign_size;

    if (opera_len > 0) {
        efc_si_password(efc_id);

        efc_si_opera_set(efc_id, OP_CODE_PROGRAM, addr, opera_len - 1);

        /*step6, set program data*/
        for (j = 0; j < opera_len; j ++) {
            efc_si_opera_data(efc_id, data);
            data += efc_si_dev[efc_id].write_allign_size;

            if (j == 0) {
                efc_si_opera_start(efc_id);
            }

            if (j > 0) {
                efc_si_send_data_ack(efc_id);
            }

            if (j == opera_len - 1) {
                break;
            }

            /*wait for data is taken*/
            if (efc_si_dev[efc_id].poll_mode) {
                efc_si_wait_data_req(efc_id);
            } else {
                while (datareq_flag == 0);

                datareq_flag = 0;
//                yunos_sem_take(&(efc_si_dev[efc_id].isr_datareq_sem), YUNOS_WAIT_FOREVER);
            }
        }

        /*wait for transcation done*/
        if (efc_si_dev[efc_id].poll_mode) {
            efc_si_wait_trans_done(efc_id);
            EFC_DEBUG_LOG("fast polling mode.\n");
        } else {
//            yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem), YUNOS_WAIT_FOREVER);
            while (trans_flag == 0);

            trans_flag = 0;
            EFC_DEBUG_LOG("fast irq mode.\n");
        }

        efc_si_password(efc_id);
        efc_si_clean_opera_len(efc_id);
    }

    /* len is not 16/4 bytes alligned*/
    if (left_len > 0) {
        ret = efc_si_len_alligned(efc_id, data, addr + (len - left_len), left_len);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_fast_write:eflash_alligned failed.\n");
            return ret;
        }
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    ret = memcmp(src_data, (void *)src_addr, src_len);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_fast_write:fast write data error.\n");
        return -EPERM;
    }

    return 0;
}

/**
 *
 * program many 1K data into the flash device in fast write mode
 *
 * @param[in]    efc_id    eflash 1/2
 * @param[in]    addr        to the dest write address
 * @param[in]    len        the program length in byte
 * @param[in]    data        the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_compat_write(enum efc_si_dev_num efc_id, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t f_write_cnt = 0;
    uint32_t f_write_len = 0;
    uint32_t f_left_len = 0;
    uint32_t i = 0;
    int32_t  ret = 0;
    uint32_t line_addr;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_compat_write:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_compat_write: data is NULL.\n");
        return -EINVAL;
    }

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_compat_write: len=0.\n");
        return 0;
    }

    line_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].fast_write_size);

    if ((addr > line_addr) && (addr + len > (line_addr + efc_si_dev[efc_id].fast_write_size))) {
        f_write_len = (line_addr + efc_si_dev[efc_id].fast_write_size) - addr;

        ret = efc_si_fast_write(efc_id, addr, f_write_len, data);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }

        len = len - f_write_len;
        addr = addr + f_write_len;
        data = data + f_write_len;
    }

    f_write_cnt = len >> (efc_si_ffs(efc_si_dev[efc_id].fast_write_size) - 1);
    f_left_len = len % efc_si_dev[efc_id].fast_write_size;

    for (i = 0; i < f_write_cnt; i++) {
        ret = efc_si_fast_write(efc_id,
                                addr + i * efc_si_dev[efc_id].fast_write_size,
                                efc_si_dev[efc_id].fast_write_size,
                                (data + i * efc_si_dev[efc_id].fast_write_size));

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }
    }

    if (f_left_len > 0) {
        ret = efc_si_fast_write(efc_id,
                                addr + f_write_cnt * efc_si_dev[efc_id].fast_write_size,
                                f_left_len,
                                (data + f_write_cnt * efc_si_dev[efc_id].fast_write_size));

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }
    }

    return 0;
}

/**
 *
 * program  data into the flash device in fast write/single mode
 *
 * @param[in]    efc_id    eflash 1/2
 * @param[in]    addr        to the dest write address
 * @param[in]    len        the program length in byte
 * @param[in]    data        the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_common_write(enum efc_si_dev_num efc_id, uint32_t addr, uint32_t len, uint8_t *data)
{
    int32_t  ret = 0;
    uint32_t pre_space = 0;
    uint32_t later_space = 0;
    int32_t  nums = 0;
    uint32_t pre_addr = 0;
    uint32_t later_addr = 0;
    uint8_t *tmp_buf;

    /*----------------------------------------------------------------------*/
    /*--align------------align-----------------align---------------align----*/
    /*----|----------------|---------------------|-------------------|------*/
    /*----|----page_size---|---------------------|----page_size------|------*/
    /*----|---------|------|---------------------|-----------|-------|------*/
    /*----|pre_space|------|---------------------|later_space|-------|------*/
    /*----|---------|------|---------------------|-----------|-------|------*/
    /*----|---------|------|---------------------|-----------|-------|------*/
    /*--pre_addr---addr---------------------later_addr---addr+size-------- -*/
    /*----------------------------------------------------------------------*/
    tmp_buf = efc_si_dev[efc_id].page_buffer;
    pre_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);
    pre_space = (addr - pre_addr);
    later_addr = ROUNDDOWN(addr + len, efc_si_dev[efc_id].page_size);
    later_space = (addr + len - later_addr);
    nums = (later_addr - pre_addr) >> (efc_si_ffs(efc_si_dev[efc_id].page_size) - 1);

    if (pre_space >= 0) {
        /*save pre_addr */
        memcpy(tmp_buf, (void *)pre_addr, efc_si_dev[efc_id].page_size);

        if (len <= (efc_si_dev[efc_id].page_size - pre_space)) {
            memcpy(tmp_buf + pre_space, (void *)data, len);
            data += len;
            len = 0;
        } else {
            memcpy(tmp_buf + pre_space, (void *)data, efc_si_dev[efc_id].page_size - pre_space);
            data += (efc_si_dev[efc_id].page_size - pre_space);
            len = len - (efc_si_dev[efc_id].page_size - pre_space);
        }

        ret = efc_si_erase_pages(efc_id, pre_addr, 1);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:efc_si_erase_pages failed.\n");
            return ret;
        }

        if ((efc_si_dev[efc_id].copy_flag) || (efc_id == EFLASH_DEV1)) {
            ret = efc_si_single_compat_write(efc_id, pre_addr, efc_si_dev[efc_id].page_size, tmp_buf);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, pre_addr, tmp_buf, efc_si_dev[efc_id].page_size);
            EFC_DEBUG_LOG("efc_si_compat_write.\n");
        }

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:single/fast write failed.\n");
            return ret;
        }

        addr = pre_addr + efc_si_dev[efc_id].page_size;
        nums --;
        memset(tmp_buf, 0, efc_si_dev[efc_id].page_size);
    }

    if (nums > 0) {
        ret = efc_si_erase_pages(efc_id, addr, nums);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:efc_si_erase_pages failed.\n");
            return ret;
        }

        if ((efc_si_dev[efc_id].copy_flag) || (efc_id == EFLASH_DEV1)) {
            ret = efc_si_single_compat_write(efc_id, addr, nums * (efc_si_dev[efc_id].page_size), data);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, addr, data, nums * (efc_si_dev[efc_id].page_size));
            EFC_DEBUG_LOG("efc_si_compat_write.\n");
        }

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:single/fast write failed.\n");
            return ret;
        }

        data += nums * (efc_si_dev[efc_id].page_size);
        len = len - nums * (efc_si_dev[efc_id].page_size);
        addr +=  nums * efc_si_dev[efc_id].page_size;
    }

    if ((later_space > 0) && (pre_addr != later_addr)) {
        if ((len != later_space) || (addr != later_addr)) {
            EFC_DEBUG_LOG("efc_si_common_write:len=%d,later_space=%d,addr=0x%x,later_addr=0x%x,last page write addr/len is wrong.\n",
                          len, later_space, addr, later_addr);
            return -EPERM;
        }

        /* save later_addr */
        memcpy(tmp_buf, (void *)addr, efc_si_dev[efc_id].page_size);
        /*write the last page data*/
        memcpy(tmp_buf, data, len);

        ret = efc_si_erase_pages(efc_id, later_addr, 1);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:efc_si_erase_pages failed.\n");
            return ret;
        }

        if ((efc_si_dev[efc_id].copy_flag) || (efc_id == EFLASH_DEV1)) {
            ret = efc_si_single_compat_write(efc_id, addr, efc_si_dev[efc_id].page_size, tmp_buf);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, addr, tmp_buf, efc_si_dev[efc_id].page_size);
            EFC_DEBUG_LOG("efc_si_compat_write.\n");
        }

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:single/fast write failed.\n");
            return ret;
        }

        memset(tmp_buf, 0, efc_si_dev[efc_id].page_size);
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    return 0;
}

/**
 *
 * write main flash, compat with not alligned addr,length.
 *
 *
 * @param[in]    addr        the dest write address
 * @param[in]    len        the program length in byte
 * @param[in]    data        the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_main_write(uint32_t addr, uint32_t len, uint8_t *data)
{
    int32_t ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_main_write:len=0.\n");
        return 0;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_main_write:src_data is NULL.\n");
        return -EINVAL;
    }

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_main_write:addr=0x%x is out eflash main range.\n", addr);
        return -EINVAL;
    }

    if (addr + len > efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size) {
        EFC_DEBUG_LOG("efc_si_main_write:addr=0x%x,len=%d,addr+len is out of main range.\n", addr, len);
        return -EINVAL;
    }

    ret = efc_si_common_write(efc_id, addr, len, data);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_common_write:efc_si_common_write failed.\n");
        return ret;
    }

    return 0;
}

#if 0
/**
 *write information flash, compat with not alligned addr,length.
 *
 *
 * @param[in]    addr        the dest write address
 * @param[in]    len        the program length in byte
 * @param[in]    data        the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_info_write(uint32_t addr, uint32_t len, uint8_t *data)
{
    int32_t ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_info_write:len=0.\n");
        return 0;
    }

    if (data == NULL) {
        EFC_DEBUG_LOG("efc_si_info_write:src_data is NULL.\n");
        return -EINVAL;
    }

    if (IS_EFC1_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_info_write:addr is out eflash info range.\n");
        return -EINVAL;
    }

    if (addr + len > efc_si_dev[efc_id].info_base + efc_si_dev[efc_id].info_size) {
        EFC_DEBUG_LOG("efc_si_info_write:addr=0x%x,len=%d,addr+len is out of info range.\n", addr, len);
        return -EINVAL;
    }

    ret = efc_si_common_write(efc_id, addr, len, data);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_common_write:efc_si_common_write failed.\n");
        return ret;
    }

    return 0;
}

/**
 *
 * read information flash, compat with not alligned addr,length.
 *
 *
 * @param[in]    addr        the dest read address
 * @param[in]    len        the read length in byte
 * @param[in]    buf         the pointer to the store data address
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_info_read(uint32_t addr, uint32_t len, uint8_t *buf)
{
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_info_read:len=0.\n");
        return 0;
    }

    if (buf == NULL) {
        EFC_DEBUG_LOG("efc_si_info_read:buf is NULL.\n");
        return -EINVAL;
    }

    if (IS_EFC1_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_INFO_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_info_read:addr=0x%x is out eflash info range.\n", addr);
        return -EINVAL;
    }

    if (addr + len > efc_si_dev[efc_id].info_base + efc_si_dev[efc_id].info_size) {
        EFC_DEBUG_LOG("efc_si_info_read:addr=0x%x,len=%d,addr+len is out of info range.\n", addr, len);
        return -EINVAL;
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    memcpy(buf, (void *)addr, len);

    return 0;
}
#endif
/**
 *
 * read  main flash, compat with not alligned addr,length.
 *
 *
 * @param[in]    addr        the dest read address
 * @param[in]    len         the read length in byte
 * @param[in]    buf        the pointer to the store data address
 *
 * @return     0 on success; -1 on failure
 *
*/
static int32_t efc_si_main_read(uint32_t addr, uint32_t len, uint8_t *buf)
{
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_main_read:len=0.\n");
        return 0;
    }

    if (buf == NULL) {
        EFC_DEBUG_LOG("efc_si_main_read:buf is NULL.\n");
        return -EINVAL;
    }

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_main_read:addr=0x%x is out eflash main range.\n", addr);
        return -EINVAL;
    }

    if (addr + len > efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size) {
        EFC_DEBUG_LOG("efc_si_main_read:addr=0x%x,len=%d,addr+len is out of main range.\n", addr, len);
        return -EINVAL;
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    memcpy(buf, (void *)addr, len);

    return len;
}

/**
 *
 * main flash copy, compat with not alligned addr,length.
 *
 *
 * @param[in]    src_addr            the source address
 * @param[in]    dest_addr        the dest  address
 * @param[in]    len                 he length of copy data
 *
 * @return      0 on success; -1 on failure
 *
 */
#if 0
static int32_t efc_si_main_copy(uint32_t src_addr, uint32_t dest_addr, uint32_t len)
{
    int32_t ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_main_copy:len=0.\n");
        return 0;
    }

    if (IS_EFC1_MAIN_ADDR(src_addr) && IS_EFC1_MAIN_ADDR(dest_addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_MAIN_ADDR(src_addr) && IS_EFC2_MAIN_ADDR(dest_addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_main_copy:src_addr=0x%x is out of eflash main range.\n", src_addr);
        return -EINVAL;
    }

    if ((src_addr + len > efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size)
        || (dest_addr + len > efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size)) {
        EFC_DEBUG_LOG("efc_si_main_copy:src_addr=0x%x,len=%d,addr+len is out of main range.\n", src_addr, len);
        return -EINVAL;
    }

    if (((src_addr + len) < dest_addr) || ((dest_addr + len) < src_addr)) {
        efc_si_dev[efc_id].copy_flag = 1;
        ret = efc_si_common_write(efc_id, dest_addr, len, (uint8_t *)src_addr);
        efc_si_dev[efc_id].copy_flag = 0;

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_main_copy:efc_si_common_write failed.\n");
            return ret;
        }
    } else {
        EFC_DEBUG_LOG("efc_si_main_copy:src_addr=0x%x,dest_addr=0x%x,len=%d is overlay.\n", src_addr, dest_addr, len);
        return -EINVAL;
    }

    return 0;
}

/**
 * main flash copy, compat with not alligned addr,length.
 *
 *
 *
 * @param[in]    src_addr            the source    address
 * @param[in]    dest_addr        the dest  address
 * @param[in]    len                 the length of copy data
 *
 * @return      0 on success; -1 on failure
 *
 */
int yunos_bsp_eflash_cpy(uint32_t src_addr, uint32_t dest_addr, uint32_t len)
{
    int ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(src_addr)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to copy write eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(src_addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_main_copy(src_addr, dest_addr, len);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 *
 * information flash copy, compat with not alligned addr,length.
 *
 *
 * @param[in]    src_addr             the source    address
 * @param[in]    dest_addr        the dest  address
 * @param[in]    len                the length of copy data
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_info_copy(uint32_t src_addr, uint32_t dest_addr, uint32_t len)
{
    int32_t ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (len == 0) {
        EFC_DEBUG_LOG("efc_si_info_copy:len=0.\n");
        return 0;
    }

    if (IS_EFC1_INFO_ADDR(src_addr) && IS_EFC1_INFO_ADDR(dest_addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_INFO_ADDR(src_addr) && IS_EFC2_INFO_ADDR(dest_addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_info_copy:src_addr=0x%x is out of eflash info range.\n", src_addr);
        return -EINVAL;
    }

    if ((src_addr + len > efc_si_dev[efc_id].info_base + efc_si_dev[efc_id].info_size)
        || (dest_addr + len > efc_si_dev[efc_id].info_base + efc_si_dev[efc_id].info_size)) {
        EFC_DEBUG_LOG("efc_si_info_copy:src_addr=0x%x,len=%d,addr+len is out of info range.\n", src_addr, len);
        return -EINVAL;
    }

    if (((src_addr + len) < dest_addr) || ((dest_addr + len) < src_addr)) {
        efc_si_dev[efc_id].copy_flag = 1;
        ret = efc_si_common_write(efc_id, dest_addr, len, (uint8_t *)src_addr);
        efc_si_dev[efc_id].copy_flag = 0;

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_info_copy:efc_si_info_write failed.\n");
            return ret;
        }
    } else {
        EFC_DEBUG_LOG("efc_si_info_copy:src_addr=0x%x,dest_addr=0x%x,len=%d is overlay.\n", src_addr, dest_addr, len);
        return -EINVAL;
    }

    return 0;
}
#endif
/**
 *
 * flash standby mode
 *
 *
 * @param[in]    efc_id        eflash0/eflash1
 *
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_standby(enum efc_si_dev_num efc_id)
{
    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_standby:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    efc_si_password(efc_id);

    efc_si_opera_set(efc_id, OP_CODE_STANDBY, 0, 0);

    efc_si_opera_start(efc_id);

    efc_si_wait_idle(efc_id);

    return 0;
}

/**
 * flash standby mode
 *
 *
 *
 * @param[in]    efc_id         eflash0/eflash1
 *
 *
 * @return      0 on success; -1 on failure
 *
 */
int yunos_bsp_eflash_standby(enum efc_si_dev_num efc_id)
{
    return efc_si_standby(efc_id);
}

#if 0
/**
 *
 * eflash0: set information page 0/1 as otp area
 * eflash1: set information page 0/1 as otp area
 *
 * @param[in]    efc_id            eflash0/eflash1
 * @param[in]    page_num        0/1
 *
 * @return      0 on success; -1 on failure
 *
 */
static int32_t efc_si_otp_write(enum efc_si_dev_num efc_id, enum efc_si_infopage_num page_num)
{
    uint8_t tmp = 0;
    int32_t ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_otp_write:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (page_num >= MAX_INFOPAGE_NUM) {
        EFC_DEBUG_LOG("efc_si_otp_write:page_num=%d is invalid.\n", page_num);
        return -EINVAL;
    }

    tmp = (*(volatile uint8_t *)(efc_si_otp_addr[efc_id][page_num])) & (~(0x3));

    ret = efc_si_info_write(efc_si_otp_addr[efc_id][page_num], 1, &tmp);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_otp_write:efc_si_info_write failed.\n");
        return ret;
    }

    return 0;
}

/**
 *
 * eflash0: read otp area lock flag
 * eflash1: read otp area lock flag
 *
 * @param[in]    efc_id        eflash0/eflash1
 * @param[in]    page_num        0/1
 *
 * @return      0 on success; -1 on failure
 *
*/
static bool efc_si_otp_read(enum efc_si_dev_num efc_id, enum efc_si_infopage_num page_num)
{
    uint8_t tmp = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_otp_read:efc_id=%d is invalid.\n", efc_id);
        return false;
    }

    if (page_num >= MAX_INFOPAGE_NUM) {
        EFC_DEBUG_LOG("efc_si_otp_read:page_num=%d is invalid.\n", page_num);
        return false;
    }

    tmp = (*(volatile uint8_t *)(efc_si_otp_addr[efc_id][page_num])) & (0x3);

    if (tmp != OTP_LOCK_FLAG) {
        return false;
    }

    return true;
}

/**
 * eflash0: set information page 0/1 as otp area
 * eflash1: set information page 0/1 as otp area
 *
 *
 * @param[in]    efc_id            eflash0/eflash1
 * @param[in]    page_num        0/1
 *
 * @return      0 on success; -1 on failure
 *
*/
int yunos_bsp_eflash_otpwrite(enum efc_si_dev_num efc_id, enum efc_si_infopage_num page_num)
{
    int ret = 0;

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_otp_write(efc_id, page_num);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 * eflash0: read otp area lock flag
 * eflash1: read otp area lock flag
 *
 *
 * @param[in]    efc_id            eflash0/eflash1
 * @param[in]    page_num        0/1
 *
 * @return     false:  otp not lock or error; true:  otp lock
 *
*/
bool yunos_bsp_eflash_otpread(enum efc_si_dev_num efc_id, enum efc_si_infopage_num page_num)
{
    return efc_si_otp_read(efc_id, page_num);
}
#endif
void zx29_eflash_irqhandler(uint8_t id)
{
    enum efc_si_dev_num efc_id = 0;

    uint32_t data;

    data = efc_si_dev[efc_id].reg->int_status;

    if (data == 0x02) {
        /*clean  the  data req interrupt*/
        efc_si_dev[efc_id].reg->int_status |= EFC_INT_DATA_REQ;
//        yunos_sem_give(&(efc_si_dev[efc_id].isr_datareq_sem));
        datareq_flag = 1;
    }

    if (data == 0x01) {
        /*  clean trans done int */
        efc_si_dev[efc_id].reg->int_status |= EFC_INT_TRANS_DONE;
//        yunos_sem_give(&(efc_si_dev[efc_id].isr_trans_sem));
        trans_flag = 1;
    }
}

#if 0
/**
*
* Isr of eflash1/2 interrupt
*
*
* @param[in]efc_id         eflash0/eflash1
*
*
* @return     void
*
*/
static void efc_si_isr(enum efc_si_dev_num efc_id)
{
    uint32_t data;

    data = efc_si_dev[efc_id].reg->int_status;

    if (data == 0x02) {
        /*clean  the  data req interrupt*/
        efc_si_dev[efc_id].reg->int_status |= EFC_INT_DATA_REQ;
//        yunos_sem_give(&(efc_si_dev[efc_id].isr_datareq_sem));
        datareq_flag = 1;
    }

    if (data == 0x01) {
        /*  clean trans done int */
        efc_si_dev[efc_id].reg->int_status |= EFC_INT_TRANS_DONE;
//        yunos_sem_give(&(efc_si_dev[efc_id].isr_trans_sem));
        trans_flag = 1;
    }
}

/**
*
* Isr of eflash1 interrupt
*
*
* @param[in]
*
*
* @return     0 on success; -1 on failure
*
*/
static int efc1_si_intisr(void)
{
    efc_si_isr(EFLASH_DEV1);

    return 0;
}

/**
*
* Isr of eflash2 interrupt
*
*
* @param[in]
*
*
* @return      0 on success; -1 on failure
*
*/
static int efc2_si_intisr(void)
{
    efc_si_isr(EFLASH_DEV2);

    return 0;
}

#endif
/**
*
* install eflash irq
*
*
* @param[in]
*
*
* @return      0 on success; -1 on failure
*
*/
static int32_t efc_si_irq_install(void)
{
    int32_t ret = 0;

//    efc_si_dev[EFLASH_DEV1].irq_line = PIC_IRQ_EFC1;
//    efc_si_dev[EFLASH_DEV2].irq_line = PIC_IRQ_EFC2;

    csi_intc_disable_irq(PIC_IRQ_EFC1);

    ret = csi_intc_set_attribute(PIC_IRQ_EFC1, 1, INT_MODE_HIGH_LEVEL);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_irq_install:yunos_bsp_intc_set_attribute failed.\n");
        return -EPERM;
    }

//    ret = yunos_bsp_intc_attach_irq(PIC_IRQ_EFC1,(int)efc1_si_intisr);
//    if (ret != 0) {
//        EFC_DEBUG_LOG("efc_si_irq_install:yunos_bsp_intc_attach_irq failed.\n");
//        return -EPERM;
//    }

    csi_intc_disable_irq(PIC_IRQ_EFC2);

    ret = csi_intc_set_attribute(PIC_IRQ_EFC2, 1, INT_MODE_HIGH_LEVEL);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_irq_install:yunos_bsp_intc_set_attribute failed.\n");
        return -EPERM;
    }

//    ret = yunos_bsp_intc_attach_irq(PIC_IRQ_EFC2,(int)efc2_si_intisr);
//    if (ret != 0) {
//        EFC_DEBUG_LOG("efc_si_irq_install:yunos_bsp_intc_attach_irq failed.\n");
//        return -EPERM;
//    }
    return 0;
}

/**
 *
 * flash clk soft gate ctrl
 *
 *
 * @param[in]    clk_ctrl         1:enable/0:disable
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
#if 0
static void efc_si_clk_ctrl(uint32_t clk_ctrl)
{
    if (clk_ctrl) {
        __set_bit(EFC1_CFG_SW_PCLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
        __set_bit(EFC1_CTRL_SW_CLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
        __set_bit(EFC2_CTRL_SW_CLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
    } else {
        __clear_bit(EFC1_CFG_SW_PCLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
        __clear_bit(EFC1_CTRL_SW_CLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
        __clear_bit(EFC2_CTRL_SW_CLKEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
    }
}

/**
 *
 * flash clk init
 *
 *
 * @param[in]
 *
 *
 * @return       0 on success; -1 on failure
 *
 */
static void efc_si_init_clk(void)
{
    /******* en auto gate reqen,eflash auto is invalid ********/
    __set_bit(EFC2_CLK_REQEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
    __set_bit(EFC1_CTRL_CLK_REQEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);
    __set_bit(EFC1_CFG_CLK_REQEN, (volatile uint32_t *)TOP_CRPM_EFLASH_CLK);

    efc_si_clk_ctrl(1);
}

/**
 *
 * flash start init
 *
 *
 * @param[in]
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
static void efc_si_power_up(void)
{
    efc_si_init_clk();

    /* release eflash 1&2 reset */
    __write_bits(3, EFLASH_RESET_SHIFT, (volatile uint32_t *)EFLASH_RESET_ADDR, 0x7);

    /* umask eflash interrupt*/
    efc_si_dev[EFLASH_DEV1].reg->int_mask |= 0x03;
    efc_si_dev[EFLASH_DEV2].reg->int_mask |= 0x03;
    /* set recall in normal read*/
    efc_si_dev[EFLASH_DEV1].reg->lock &= ~EFC_RECALL_PIN;
    efc_si_dev[EFLASH_DEV2].reg->lock &= ~EFC_RECALL_PIN;
}
#endif
/**
 * flash init
 *
 *
 * @param[in]
 *
 *
 * @return      0 on success; -1 on failure
 *
 */

static int32_t efc_si_probe(void)
{
    int32_t ret = 0;

//    efc_si_dev[EFLASH_DEV1].reg = (volatile struct efc_si_regs *)(EFC1_BASE_ADDR);
//    efc_si_dev[EFLASH_DEV2].reg = (volatile struct efc_si_regs *)(EFC2_BASE_ADDR);
    efc_si_dev[EFLASH_DEV1].page_size = EFLASH1_PAGE_SIZE;
    efc_si_dev[EFLASH_DEV2].page_size = EFLASH2_PAGE_SIZE;
//    efc_si_dev[EFLASH_DEV1].main_base = EFLASH1_MAIN_START;
//    efc_si_dev[EFLASH_DEV2].main_base = EFLASH2_MAIN_START;
    efc_si_dev[EFLASH_DEV1].info_base = EFLASH1_INFO_START;
    efc_si_dev[EFLASH_DEV2].info_base = EFLASH2_INFO_START;
    efc_si_dev[EFLASH_DEV1].main_size = EFLASH1_MAIN_SIZE;
    efc_si_dev[EFLASH_DEV2].main_size = EFLASH2_MAIN_SIZE;
    efc_si_dev[EFLASH_DEV1].info_size = EFLASH1_INFO_SIZE;
    efc_si_dev[EFLASH_DEV2].info_size = EFLASH2_INFO_SIZE;
    efc_si_dev[EFLASH_DEV1].fast_write_size = EFLASH1_FAST_SIZE;
    efc_si_dev[EFLASH_DEV2].fast_write_size = EFLASH2_FAST_SIZE;
    efc_si_dev[EFLASH_DEV1].write_allign_size = EFLASH1_ALLIGN_SIZE;
    efc_si_dev[EFLASH_DEV2].write_allign_size = EFLASH2_ALLIGN_SIZE;

    efc_si_dev[EFLASH_DEV2].page_buffer = eflash2_buffer;
    efc_si_dev[EFLASH_DEV1].page_buffer = eflash1_buffer;

    efc_si_dev[EFLASH_DEV1].copy_flag = 0;
    efc_si_dev[EFLASH_DEV2].copy_flag = 0;
    efc_si_dev[EFLASH_DEV1].poll_mode = 1;
    efc_si_dev[EFLASH_DEV2].poll_mode = 1;

    ret = efc_si_irq_install();

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_probe:efc_si_irq_install failed.\n");
        return -EPERM;
    }

    return 0;
}

/**
 *
 * lock elfash operation
 *
 *
 * @param[in]efc_id         eflash0/eflash1
 *
 *
 * @return     0 on success; -1 on failure
 *
 */
static int32_t efc_si_lock(enum efc_si_dev_num efc_id)
{
//    soft_spin_lock(NAND_SFLOCK);

//    csi_intc_enable_irq(efc_si_dev[efc_id].irq_line);

    return 0;
}

/**
 *
 * unlock efc operation
 *
 *
 * @param[in]efc_id         eflash0/eflash1
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
static int32_t efc_si_unlock(enum efc_si_dev_num efc_id)
{
//    csi_intc_disable_irq(efc_si_dev[efc_id].irq_line);

//    soft_spin_unlock(NAND_SFLOCK);

    return 0;
}

#if 0
/**************************************packing the function interface in  standard  form******************************/

/**
 * write main flash, compat with not alligned addr,length.
 *
 *
 *
 * @param[in]    addroffset        the dest write address
 * @param[in]    bytesize            the program length in byte
 * @param[in]    buff                the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
int yunos_bsp_eflash_write(uint32_t addroffset, uint8_t *buff, uint32_t bytesize)
{
    int ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to write eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_main_write(addroffset, bytesize, buff);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 * write main flash in polling mode, compat with not alligned addr,length.
 *
 *
 *
 * @param[in]    addroffset        the dest write address
 * @param[in]    bytesize            the program length in byte
 * @param[in]    buff                the pointer to the source data address
 *
 * @return      0 on success; -1 on failure
 *
*/
int yunos_bsp_eflash_write_poll(uint32_t addroffset, uint8_t *buff, uint32_t bytesize)
{
    int ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to write eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = yunos_bsp_intc_disable_irq(efc_si_dev[efc_id].irq_line);

    if (ret != 0) {
        EFC_DEBUG_LOG("disable_irq efc_id=%d failed.\n", efc_id);
        return -EPERM;
    }

    efc_si_dev[efc_id].poll_mode = 1;
    ret = efc_si_main_write(addroffset, bytesize, buff);
    efc_si_dev[efc_id].poll_mode = 0;

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 * read  main flash, compat with not alligned addr,length.
 *
 *
 *
 * @param[in]    addroffset        the dest read address
 * @param[in]    bytesize            the read length in byte
 * @param[in]    buff                the pointer to the store data address
 *
 * @return      0 on success; -1 on failure
 *
 */
int yunos_bsp_eflash_read(uint32_t addroffset, uint8_t *buff, uint32_t bytesize)
{
    return efc_si_main_read(addroffset, bytesize, buff);
}

/**
 * erase many pages begain with the address in main eflash block
 *
 *
 * @param[in]    addroffset        begining address of the erase pages
 * @param[in]    nums            the nums of erase pages
 *
 *
 * @return     0 on success; -1 on failure
 *
 */
int yunos_bsp_eflash_erase(uint32_t addroffset, uint32_t nums)
{
    int ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to erase eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(addroffset)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_main_erase(addroffset, nums);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 * erase efalsh1/2 all main falsh memory pages,information block is not affected.
 *
 *
 * @param[in]    efc_id        eflash 1/2
 *
 *
 *
 * @return      0 on success; -1 on failure
 *
*/
int yunos_bsp_eflash_erasemacro(enum efc_si_dev_num efc_id)
{
    int ret = 0;

    if (efc_id == EFLASH_DEV1) {
        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to erase eflash1.\n");
            return -EPERM;
        }
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_erase_macro(efc_id);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
 * flash init
 *
 *
 *
 * @param[in]
 *
 *
 * @return      0 on success; -1 on failure
 *
 */
int yunos_bsp_eflash_init(void)
{
    return efc_si_probe();
}
#endif

/**
  \brief       Initialize EFLASH Interface. 1. Initializes the resources needed for the EFLASH interface 2.registers event callback function
  \param[in]   idx  device id
  \param[in]   cb_event  Pointer to \ref eflash_event_cb_t
  \return      pointer to eflash handle
*/
eflash_handle_t csi_eflash_initialize(int32_t idx, eflash_event_cb_t cb_event)
{
    if (idx < 0 || idx >= MAX_EFLASHDEV_NUM) {
        return NULL;
    }

    /* obtain the eflash information */
    uint32_t base = 0u;
    uint32_t irq = 0u;
    int32_t real_idx = target_get_eflash(idx, &base, &irq, &info[idx]);

    if (real_idx != idx) {
        return NULL;
    }

    efc_si_dev_data *eflash_priv = &efc_si_dev[idx];

    eflash_priv->reg = (efc_si_regs *)base;
    eflash_priv->main_base = info[idx].start;
    eflash_priv->irq_line = irq;

    /* initialize the eflash context */
    eflash_priv->cb = cb_event;
    eflash_priv->idx = idx;

    eflash_priv->status.busy = 0;
    eflash_priv->status.error = 0U;
    efc_si_probe();

    return (eflash_handle_t)eflash_priv;
}

/**
  \brief       De-initialize EFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  eflash handle to operate.
  \return      error code
*/
int32_t csi_eflash_uninitialize(eflash_handle_t handle)
{
    EFLASH_NULL_PARAM_CHK(handle);

    efc_si_dev_data *eflash_priv = handle;
    eflash_priv->cb = NULL;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref eflash_capabilities_t
*/
eflash_capabilities_t csi_eflash_get_capabilities(int32_t idx)
{
    return driver_capabilities;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or error code
*/
int32_t csi_eflash_read(eflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    EFLASH_NULL_PARAM_CHK(handle);
    EFLASH_NULL_PARAM_CHK(data);
    EFLASH_NULL_PARAM_CHK(cnt);
    return efc_si_main_read(addr, cnt, data);
}

/**
  \brief       Program data to Flash.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t csi_eflash_program(eflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    EFLASH_NULL_PARAM_CHK(handle);
    EFLASH_NULL_PARAM_CHK(data);
    EFLASH_NULL_PARAM_CHK(cnt);

    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to write eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    efc_si_main_write(addr, cnt, (uint8_t *)data);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return cnt;
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Sector address
  \return      error code
*/
int32_t csi_eflash_erase_sector(eflash_handle_t handle, uint32_t addr)
{
    EFLASH_NULL_PARAM_CHK(handle);

    int ret = 0;
    enum efc_si_dev_num efc_id = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;

        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to erase eflash1.\n");
            return -EPERM;
        }
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        return -EINVAL;
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_main_erase(addr, 1);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  eflash handle to operate.
  \return      error code
*/
int32_t csi_eflash_erase_chip(eflash_handle_t handle)
{
    EFLASH_NULL_PARAM_CHK(handle);
    efc_si_dev_data *eflash_priv = handle;

    enum efc_si_dev_num efc_id = eflash_priv->idx;
    int ret = 0;

    if (efc_id == EFLASH_DEV1) {
        if (is_amt_mode() == 0) {
            EFC_DEBUG_LOG("have no permission to erase eflash1.\n");
            return -EPERM;
        }
    }

    if (efc_si_lock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc lock failed.\n");
        return -EPERM;
    }

    ret = efc_si_erase_macro(efc_id);

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

/**
  \brief       Get Flash information.
  \param[in]   handle  eflash handle to operate.
  \return      Pointer to Flash information \ref eflash_info_t
*/
eflash_info_t *csi_eflash_get_info(eflash_handle_t handle)
{
    efc_si_dev_data *eflash_priv = handle;

    info[eflash_priv->idx].start = eflash_priv->main_base;
    info[eflash_priv->idx].end = (eflash_priv->main_base) + (eflash_priv->main_size);
    info[eflash_priv->idx].sector_count = (eflash_priv->main_size) / (eflash_priv->page_size);
    info[eflash_priv->idx].sector_size = eflash_priv->page_size;
    info[eflash_priv->idx].erased_value = 0xff;
    info[eflash_priv->idx].page_size = eflash_priv->page_size;
    info[eflash_priv->idx].program_unit = eflash_priv->write_allign_size;

    return &info[eflash_priv->idx];
}

/**
  \brief       Get EFLASH status.
  \param[in]   handle  eflash handle to operate.
  \return      EFLASH status \ref eflash_status_t
*/
eflash_status_t csi_eflash_get_status(eflash_handle_t handle)
{
    efc_si_dev_data *eflash_priv = handle;

    return eflash_priv->status;
}

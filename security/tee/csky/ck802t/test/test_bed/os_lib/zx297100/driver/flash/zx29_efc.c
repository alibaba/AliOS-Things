

#include "zx29_efc.h"
#include "string.h"

#define EFC_DEBUG_LOG(...)

static efc_si_dev_data efc_si_dev[MAX_EFLASHDEV_NUM] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL }
};

// static eflash_info_t info[MAX_EFLASHDEV_NUM];
static uint8_t eflash1_buffer[EFLASH1_PAGE_SIZE];
static uint8_t eflash2_buffer[EFLASH2_PAGE_SIZE];

static uint8_t datareq_flag = 0;
static uint8_t trans_flag   = 0;


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


static void efc_si_wait_idle(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*wait for done*/
    while (efc_reg->start_status & EFC_START_BUSY_FLAG)
        ;
}

static void efc_si_password(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    efc_si_wait_idle(efc_id);
    /*step1, password*/
    efc_reg->password = OP_EN_PASSWORD;

    /*step2 wait for write enable*/
    while (!(efc_reg->start_status & EFC_WRITE_ENABLE))
        ;
}

static void efc_si_opera_set(enum efc_si_dev_num    efc_id,
                             enum efc_si_opera_code opera_code,
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

static void efc_si_opera_data(enum efc_si_dev_num efc_id, uint8_t *data)
{
    efc_si_regs *efc_reg = NULL;
    uint32_t     temp;
    uint32_t     i = 0;

    efc_reg = efc_si_dev[efc_id].reg;

    /*step6, set the program data*/
    for (i = 0; i < (efc_si_dev[efc_id].write_allign_size >> 2); i++) {
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

static void efc_si_opera_start(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*step7, wait fot freedom */
    efc_si_wait_idle(efc_id);
    /*step8, start program*/
    efc_reg->start_status |= EFC_START_BUSY_FLAG;
}

static void efc_si_wait_data_req(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*wait for data taken*/
    while (!(efc_reg->start_status & EFC_DATA_REQ))
        ;

    while (!(efc_reg->int_status & EFC_INT_DATA_REQ))
        ;

    /*clean  the  data req interrupt*/
    efc_reg->int_status |= EFC_INT_DATA_REQ;
}

static void efc_si_wait_trans_done(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /* wait  trans done int */
    while (!(efc_reg->int_status & EFC_INT_TRANS_DONE))
        ;

    /* step10 clean trans done int */
    efc_reg->int_status |= EFC_INT_TRANS_DONE;
}

static void efc_si_send_data_ack(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /* send  a acknowlege to data ack*/
    efc_reg->start_status |= EFC_DATA_ACK;
}

static void efc_si_clean_opera_len(enum efc_si_dev_num efc_id)
{
    efc_si_regs *efc_reg = NULL;

    efc_reg = efc_si_dev[efc_id].reg;

    /*clean the operate length*/
    efc_reg->op_len = 0;
}

static void efc_si_clean_buffer(void)
{
    /*the core have buffer between eflash,clear CK802 buffer*/
    efc_si_dev[0].reg->buf_ctrl |= EFC_CLR_CK802_DBUF;
    efc_si_dev[0].reg->buf_ctrl &= ~EFC_CLR_CK802_DBUF;
    efc_si_dev[0].reg->buf_ctrl |= EFC_CLR_CK802_IBUF;
    efc_si_dev[0].reg->buf_ctrl &= ~EFC_CLR_CK802_IBUF;
}

static int efc_si_bufpre_set(enum efc_si_core_type core_type,
                             uint8_t               is_prefetch)
{
    efc_si_regs *efc_reg = NULL;

    if (core_type >= MAX_EFC_CORE) {
        EFC_DEBUG_LOG("efc_si_bufpre_set:core_type =  is invalid.\n");
        return -EINVAL;
    }

    efc_reg = efc_si_dev[EFLASH_DEV1].reg;

    if (is_prefetch) {
        HAL_BIT_SET(efc_reg->buf_ctrl, core_type);
    } else {
        HAL_BIT_CLR(efc_reg->buf_ctrl, core_type);
    }

    return 0;
}

int yunos_bsp_eflash_bufpre_set(enum efc_si_core_type core_type,
                                uint8_t               is_prefetch)
{
    return efc_si_bufpre_set(core_type, is_prefetch);
}

static int32_t efc_si_erase_page_check(enum efc_si_dev_num efc_id,
                                       uint32_t            addr)
{
    uint32_t cnt       = 0;
    uint8_t  com_val   = 0xff;
    uint8_t *value     = NULL;
    uint32_t page_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_erase_page_check: efc_id=%d  is invalid.\n",
                      efc_id);
        return -EINVAL;
    }

    if (!IS_EFC_ADDR(addr)) {
        EFC_DEBUG_LOG(
          "efc_si_erase_page_check:addr=0x%x is out eflash range.\n", addr);
        return -EINVAL;
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    for (cnt = 0, value = (uint8_t *)(page_addr);
         cnt < efc_si_dev[efc_id].page_size; cnt++, value++) {
        com_val &= *value;

        if (com_val != 0xff) {
            EFC_DEBUG_LOG(
              "efc_si_erase_page_check addr=0x%x,com_val=0x%x clear failed.\n",
              (uint32_t)value, com_val);
            return -EPERM;
        }
    }

    return 0;
}

static int32_t efc_si_erase_onepage(enum efc_si_dev_num efc_id, uint32_t addr)
{
    int32_t ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_erase_onepage:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    if (!IS_EFC_ADDR(addr)) {
        EFC_DEBUG_LOG("efc_si_erase_onepage:addr=0x%x is out eflash range.\n",
                      addr);
        return -EINVAL;
    }

    efc_si_password(efc_id);

    efc_si_opera_set(efc_id, OP_CODE_PAGE_ERASE, addr, 0);

    efc_si_opera_start(efc_id);

    if (efc_si_dev[efc_id].poll_mode) {
        efc_si_wait_trans_done(efc_id);
        EFC_DEBUG_LOG("pageerase polling mode.\n");
    } else {
        //        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem),
        //        YUNOS_WAIT_FOREVER);
        while (trans_flag == 0)
            ;

        trans_flag = 0;
        EFC_DEBUG_LOG("pageerase irq mode.\n");
    }

    efc_si_wait_idle(efc_id);

    ret = efc_si_erase_page_check(efc_id, addr);

    return ret;
}

int32_t efc_si_erase_macro(enum efc_si_dev_num efc_id)
{
    int      ret       = 0;
    uint32_t cnt       = 0;
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
        //        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem),
        //        YUNOS_WAIT_FOREVER);
        while (trans_flag == 0)
            ;

        trans_flag = 0;
        EFC_DEBUG_LOG("macroerase irq mode.\n");
    }

    efc_si_wait_idle(efc_id);

    page_nums = (efc_si_dev[efc_id].main_size >>
                 (efc_si_ffs(efc_si_dev[efc_id].page_size) - 1));

    for (cnt = 0; cnt < page_nums; cnt++) {
        ret =
          efc_si_erase_page_check(efc_id, cnt * efc_si_dev[efc_id].page_size +
                                            efc_si_dev[efc_id].main_base);

        if (ret != 0) {
            EFC_DEBUG_LOG(
              "efc_si_erase_macro:efc_si_erase_page_check failed!.\n");
            return ret;
        }
    }

    return 0;
}

static int32_t efc_si_erase_pages(enum efc_si_dev_num efc_id, uint32_t addr,
                                  uint32_t nums)
{
    int ret = 0;

    while (nums) {
        ret = efc_si_erase_onepage(efc_id, addr);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_erase_pages:efc_si_erase_onepage failed.\n");
            return ret;
        }

        addr += efc_si_dev[efc_id].page_size;
        nums--;
    }

    return 0;
}

static int32_t efc_si_main_erase(uint32_t addr, uint32_t nums)
{
    int                 ret       = 0;
    uint32_t            page_addr = 0;
    enum efc_si_dev_num efc_id    = MAX_EFLASHDEV_NUM;

    if (IS_EFC1_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV1;
    } else if (IS_EFC2_MAIN_ADDR(addr)) {
        efc_id = EFLASH_DEV2;
    } else {
        EFC_DEBUG_LOG("efc_si_main_erase:addr=0x%x is out eflash main range.\n",
                      addr);
        return -EINVAL;
    }

    page_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);

    if ((page_addr + nums * efc_si_dev[efc_id].page_size) >
        (efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size)) {
        EFC_DEBUG_LOG(
          "efc_si_main_erase:erase pages addr=0x%x,nums=%d out of range.\n",
          addr, nums);
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

static int32_t efc_si_single_write(enum efc_si_dev_num efc_id, uint32_t addr,
                                   uint8_t *data)
{
    int32_t  ret      = 0;
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
        EFC_DEBUG_LOG("efc_si_single_write:addr=0x%x is out eflash range.\n",
                      addr);
        return -EINVAL;
    }

    /*addr is not allign*/
    if (addr & (efc_si_dev[efc_id].write_allign_size - 1)) {
        EFC_DEBUG_LOG(
          "efc_si_single_write:single write addr=0x%x not alligned.\n", addr);
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
        //        yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem),
        //        YUNOS_WAIT_FOREVER);
        while (trans_flag == 0)
            ;

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


static int32_t efc_si_addr_alligned(enum efc_si_dev_num efc_id,
                                    uint8_t *src_data, uint32_t allign_addr,
                                    uint32_t pre_len)
{
    uint8_t  tmp_buf[16];
    uint32_t i   = 0;
    int32_t  ret = 0;

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

static int32_t efc_si_len_alligned(enum efc_si_dev_num efc_id,
                                   uint8_t *src_data, uint32_t left_addr,
                                   uint32_t left_len)
{
    uint32_t i;
    uint8_t  tmp_buf[16];
    int32_t  ret = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_len_alligned:efc_id=%d is invalid.\n", efc_id);
        return -EINVAL;
    }

    memcpy(tmp_buf, (void *)left_addr, efc_si_dev[efc_id].write_allign_size);

    for (i = 0; i < left_len; i++) {
        tmp_buf[i] = *(src_data++);
    }

    ret = efc_si_single_write(efc_id, left_addr, tmp_buf);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_len_alligned:efc_si_single_write failed.\n");
        return ret;
    }

    return 0;
}

static int32_t efc_si_single_compat_write(enum efc_si_dev_num efc_id,
                                          uint32_t addr, uint32_t len,
                                          uint8_t *data)
{
    uint32_t j        = 0;
    int32_t  ret      = 0;
    uint8_t *src_data = data;
    uint32_t src_addr = addr;
    uint32_t src_len  = len;
    uint32_t allign_addr;
    uint32_t pre_len  = 0;
    uint32_t left_len = 0;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        EFC_DEBUG_LOG("efc_si_single_compat_write:efc_id=%d is invalid.\n",
                      efc_id);
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
    pre_len     = addr - allign_addr;

    /* addr is not alligned */
    if (pre_len > 0) {
        ret = efc_si_addr_alligned(efc_id, data, allign_addr, pre_len);

        if (ret != 0) {
            EFC_DEBUG_LOG(
              "efc_si_single_compat_write:efc_si_addr_alligned failed.\n");
            return ret;
        }

        addr = allign_addr + efc_si_dev[efc_id].write_allign_size;
        len  = len - (efc_si_dev[efc_id].write_allign_size - pre_len);
        data += (efc_si_dev[efc_id].write_allign_size - pre_len);
    }

    left_len = len % efc_si_dev[efc_id].write_allign_size;

    for (j = 0;
         j < (len >> (efc_si_ffs(efc_si_dev[efc_id].write_allign_size) - 1));
         j++) {
        ret = efc_si_single_write(
          efc_id, addr + (j * efc_si_dev[efc_id].write_allign_size),
          data + (j * efc_si_dev[efc_id].write_allign_size));

        if (ret != 0) {
            EFC_DEBUG_LOG(
              "efc_si_single_compat_write:efc_si_single_write failed.\n");
            return ret;
        }
    }

    /* len is not 16/4 bytes alligned*/
    if (left_len > 0) {
        ret = efc_si_len_alligned(efc_id, data + (len - left_len),
                                  addr + (len - left_len), left_len);

        if (ret != 0) {
            EFC_DEBUG_LOG(
              "efc_si_single_compat_write:eflash_alligned failed.\n");
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

static int32_t efc_si_fast_write(enum efc_si_dev_num efc_id, uint32_t addr,
                                 uint32_t len, uint8_t *data)
{
    int32_t  ret      = 0;
    uint32_t j        = 0;
    uint32_t left_len = 0;
    uint8_t *src_data = data;
    uint32_t src_addr = addr;
    uint32_t src_len  = len;
    uint32_t line_addr;
    uint32_t allign_addr;
    uint32_t pre_len   = 0;
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

    line_addr   = ROUNDDOWN(addr, efc_si_dev[efc_id].fast_write_size);
    allign_addr = ROUNDDOWN(addr, efc_si_dev[efc_id].write_allign_size);

    /*judge the length is out of half page*/
    if (addr + len > (line_addr + efc_si_dev[efc_id].fast_write_size)) {
        EFC_DEBUG_LOG(
          "efc_si_fast_write:addr=0x%x,len=%d,addr+len is out of 1K range.\n",
          addr, len);
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
        len  = len - (efc_si_dev[efc_id].write_allign_size - pre_len);
        data += (efc_si_dev[efc_id].write_allign_size - pre_len);
    }

    opera_len = (len >> (efc_si_ffs(efc_si_dev[efc_id].write_allign_size) - 1));
    left_len  = len % efc_si_dev[efc_id].write_allign_size;

    if (opera_len > 0) {
        efc_si_password(efc_id);

        efc_si_opera_set(efc_id, OP_CODE_PROGRAM, addr, opera_len - 1);

        /*step6, set program data*/
        for (j = 0; j < opera_len; j++) {
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
                while (datareq_flag == 0)
                    ;

                datareq_flag = 0;
                //                yunos_sem_take(&(efc_si_dev[efc_id].isr_datareq_sem),
                //                YUNOS_WAIT_FOREVER);
            }
        }

        /*wait for transcation done*/
        if (efc_si_dev[efc_id].poll_mode) {
            efc_si_wait_trans_done(efc_id);
            EFC_DEBUG_LOG("fast polling mode.\n");
        } else {
            //            yunos_sem_take(&(efc_si_dev[efc_id].isr_trans_sem),
            //            YUNOS_WAIT_FOREVER);
            while (trans_flag == 0)
                ;

            trans_flag = 0;
            EFC_DEBUG_LOG("fast irq mode.\n");
        }

        efc_si_password(efc_id);
        efc_si_clean_opera_len(efc_id);
    }

    /* len is not 16/4 bytes alligned*/
    if (left_len > 0) {
        ret =
          efc_si_len_alligned(efc_id, data, addr + (len - left_len), left_len);

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

static int32_t efc_si_compat_write(enum efc_si_dev_num efc_id, uint32_t addr,
                                   uint8_t *data, uint32_t len)
{
    uint32_t f_write_cnt = 0;
    uint32_t f_write_len = 0;
    uint32_t f_left_len  = 0;
    uint32_t i           = 0;
    int32_t  ret         = 0;
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

    if ((addr > line_addr) &&
        (addr + len > (line_addr + efc_si_dev[efc_id].fast_write_size))) {
        f_write_len = (line_addr + efc_si_dev[efc_id].fast_write_size) - addr;

        ret = efc_si_fast_write(efc_id, addr, f_write_len, data);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }

        len  = len - f_write_len;
        addr = addr + f_write_len;
        data = data + f_write_len;
    }

    f_write_cnt = len >> (efc_si_ffs(efc_si_dev[efc_id].fast_write_size) - 1);
    f_left_len  = len % efc_si_dev[efc_id].fast_write_size;

    for (i = 0; i < f_write_cnt; i++) {
        ret = efc_si_fast_write(
          efc_id, addr + i * efc_si_dev[efc_id].fast_write_size,
          efc_si_dev[efc_id].fast_write_size,
          (data + i * efc_si_dev[efc_id].fast_write_size));

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }
    }

    if (f_left_len > 0) {
        ret = efc_si_fast_write(
          efc_id, addr + f_write_cnt * efc_si_dev[efc_id].fast_write_size,
          f_left_len,
          (data + f_write_cnt * efc_si_dev[efc_id].fast_write_size));

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_compat_write:efc_si_fast_write failed.\n");
            return ret;
        }
    }

    return 0;
}

static int32_t efc_si_lock(enum efc_si_dev_num efc_id)
{

    return 0;
}


static int32_t efc_si_unlock(enum efc_si_dev_num efc_id)
{


    return 0;
}

static int32_t efc_si_common_write(enum efc_si_dev_num efc_id, uint32_t addr,
                                   uint32_t len, uint8_t *data)
{
    int32_t  ret         = 0;
    uint32_t pre_space   = 0;
    uint32_t later_space = 0;
    int32_t  nums        = 0;
    uint32_t pre_addr    = 0;
    uint32_t later_addr  = 0;
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
    tmp_buf     = efc_si_dev[efc_id].page_buffer;
    pre_addr    = ROUNDDOWN(addr, efc_si_dev[efc_id].page_size);
    pre_space   = (addr - pre_addr);
    later_addr  = ROUNDDOWN(addr + len, efc_si_dev[efc_id].page_size);
    later_space = (addr + len - later_addr);
    nums =
      (later_addr - pre_addr) >> (efc_si_ffs(efc_si_dev[efc_id].page_size) - 1);

    if (pre_space >= 0) {
        /*save pre_addr */
        memcpy(tmp_buf, (void *)pre_addr, efc_si_dev[efc_id].page_size);

        if (len <= (efc_si_dev[efc_id].page_size - pre_space)) {
            memcpy(tmp_buf + pre_space, (void *)data, len);
            data += len;
            len = 0;
        } else {
            memcpy(tmp_buf + pre_space, (void *)data,
                   efc_si_dev[efc_id].page_size - pre_space);
            data += (efc_si_dev[efc_id].page_size - pre_space);
            len = len - (efc_si_dev[efc_id].page_size - pre_space);
        }

        ret = efc_si_erase_pages(efc_id, pre_addr, 1);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:efc_si_erase_pages failed.\n");
            return ret;
        }

        if ((efc_si_dev[efc_id].copy_flag) || (efc_id == EFLASH_DEV1)) {
            ret = efc_si_single_compat_write(
              efc_id, pre_addr, efc_si_dev[efc_id].page_size, tmp_buf);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, pre_addr, tmp_buf,
                                      efc_si_dev[efc_id].page_size);
            EFC_DEBUG_LOG("efc_si_compat_write.\n");
        }

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:single/fast write failed.\n");
            return ret;
        }

        addr = pre_addr + efc_si_dev[efc_id].page_size;
        nums--;
        memset(tmp_buf, 0, efc_si_dev[efc_id].page_size);
    }

    if (nums > 0) {
        ret = efc_si_erase_pages(efc_id, addr, nums);

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:efc_si_erase_pages failed.\n");
            return ret;
        }

        if ((efc_si_dev[efc_id].copy_flag) || (efc_id == EFLASH_DEV1)) {
            ret = efc_si_single_compat_write(
              efc_id, addr, nums * (efc_si_dev[efc_id].page_size), data);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, addr, data,
                                      nums * (efc_si_dev[efc_id].page_size));
            EFC_DEBUG_LOG("efc_si_compat_write.\n");
        }

        if (ret != 0) {
            EFC_DEBUG_LOG("efc_si_common_write:single/fast write failed.\n");
            return ret;
        }

        data += nums * (efc_si_dev[efc_id].page_size);
        len = len - nums * (efc_si_dev[efc_id].page_size);
        addr += nums * efc_si_dev[efc_id].page_size;
    }

    if ((later_space > 0) && (pre_addr != later_addr)) {
        if ((len != later_space) || (addr != later_addr)) {
            EFC_DEBUG_LOG(
              "efc_si_common_write:len=%d,later_space=%d,addr=0x%x,later_addr="
              "0x%x,last page write addr/len is wrong.\n",
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
            ret = efc_si_single_compat_write(
              efc_id, addr, efc_si_dev[efc_id].page_size, tmp_buf);
            EFC_DEBUG_LOG("efc_si_single_compat_write.\n");
        } else {
            ret = efc_si_compat_write(efc_id, addr, tmp_buf,
                                      efc_si_dev[efc_id].page_size);
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

static int32_t efc_si_main_write(uint32_t addr, uint32_t len, uint8_t *data)
{
    int32_t             ret    = 0;
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
        EFC_DEBUG_LOG("efc_si_main_write:addr=0x%x is out eflash main range.\n",
                      addr);
        return -EINVAL;
    }

    if (addr + len >
        efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size) {
        EFC_DEBUG_LOG(
          "efc_si_main_write:addr=0x%x,len=%d,addr+len is out of main range.\n",
          addr, len);
        return -EINVAL;
    }

    ret = efc_si_common_write(efc_id, addr, len, data);

    if (ret != 0) {
        EFC_DEBUG_LOG("efc_si_common_write:efc_si_common_write failed.\n");
        return ret;
    }

    return 0;
}

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
        EFC_DEBUG_LOG("efc_si_main_read:addr=0x%x is out eflash main range.\n",
                      addr);
        return -EINVAL;
    }

    if (addr + len >
        efc_si_dev[efc_id].main_base + efc_si_dev[efc_id].main_size) {
        EFC_DEBUG_LOG(
          "efc_si_main_read:addr=0x%x,len=%d,addr+len is out of main range.\n",
          addr, len);
        return -EINVAL;
    }

    if (efc_id == EFLASH_DEV1) {
        efc_si_clean_buffer();
    }

    memcpy(buf, (void *)addr, len);

    return len;
}


static int32_t efc_si_probe(void)
{
    efc_si_dev[EFLASH_DEV1].reg               = (efc_si_regs *)(EFC1_BASE_ADDR);
    efc_si_dev[EFLASH_DEV2].reg               = (efc_si_regs *)(EFC2_BASE_ADDR);
    efc_si_dev[EFLASH_DEV1].page_size         = EFLASH1_PAGE_SIZE;
    efc_si_dev[EFLASH_DEV2].page_size         = EFLASH2_PAGE_SIZE;
    efc_si_dev[EFLASH_DEV1].main_base         = EFLASH1_MAIN_START;
    efc_si_dev[EFLASH_DEV2].main_base         = EFLASH2_MAIN_START;
    efc_si_dev[EFLASH_DEV1].info_base         = EFLASH1_INFO_START;
    efc_si_dev[EFLASH_DEV2].info_base         = EFLASH2_INFO_START;
    efc_si_dev[EFLASH_DEV1].main_size         = EFLASH1_MAIN_SIZE;
    efc_si_dev[EFLASH_DEV2].main_size         = EFLASH2_MAIN_SIZE;
    efc_si_dev[EFLASH_DEV1].info_size         = EFLASH1_INFO_SIZE;
    efc_si_dev[EFLASH_DEV2].info_size         = EFLASH2_INFO_SIZE;
    efc_si_dev[EFLASH_DEV1].fast_write_size   = EFLASH1_FAST_SIZE;
    efc_si_dev[EFLASH_DEV2].fast_write_size   = EFLASH2_FAST_SIZE;
    efc_si_dev[EFLASH_DEV1].write_allign_size = EFLASH1_ALLIGN_SIZE;
    efc_si_dev[EFLASH_DEV2].write_allign_size = EFLASH2_ALLIGN_SIZE;

    efc_si_dev[EFLASH_DEV2].page_buffer = eflash2_buffer;
    efc_si_dev[EFLASH_DEV1].page_buffer = eflash1_buffer;

    efc_si_dev[EFLASH_DEV1].copy_flag = 0;
    efc_si_dev[EFLASH_DEV2].copy_flag = 0;
    efc_si_dev[EFLASH_DEV1].poll_mode = 1;
    efc_si_dev[EFLASH_DEV2].poll_mode = 1;

    return 0;
}


int eflash_write_poll(uint32_t addroffset, uint8_t *buff, uint32_t bytesize)
{
    int                 ret    = 0;
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
    /*
        ret = yunos_bsp_intc_disable_irq(efc_si_dev[efc_id].irq_line);

        if (ret != 0) {
            EFC_DEBUG_LOG("disable_irq efc_id=%d failed.\n", efc_id);
            return -EPERM;
        }
    */
    efc_si_dev[efc_id].poll_mode = 1;
    ret = efc_si_main_write(addroffset, bytesize, buff);
    efc_si_dev[efc_id].poll_mode = 0;

    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

int eflash_read(uint32_t addroffset, uint8_t *buff, uint32_t bytesize)
{
    return efc_si_main_read(addroffset, bytesize, buff);
}

int eflash_erase(uint32_t addroffset, uint32_t nums)
{
    int                 ret    = 0;
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
    efc_si_dev[efc_id].poll_mode = 1;
    ret                          = efc_si_main_erase(addroffset, nums);
    efc_si_dev[efc_id].poll_mode = 0;
    if (efc_si_unlock(efc_id) != 0) {
        EFC_DEBUG_LOG("efc unlock failed.\n");
        return -EPERM;
    }

    return ret;
}

int eflash_init(void)
{
    return efc_si_probe();
}

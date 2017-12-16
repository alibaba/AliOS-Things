/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

/*
 ******************************
 *          HEADERS
 ******************************
 */
#include "ck_pmu.h"
#include "device.h"
#include "tee_addr_map.h"
#include "tee_eflash.h"
#include "tee_dbg.h"
#include "hal_memmap.h"

/*
 ******************************
 *          MACROS
 ******************************
 */

#ifdef CONFIG_CK_PMU_DBG
#define CK_PUM_DBG_E(_f, _a ...)  tee_dbg_print(ERR, _f, ##_a)
#define CK_PUM_DBG_I(_f, _a ...)  tee_dbg_print(INF, _f, ##_a)
#else
#define CK_PUM_DBG_E(_f, _a ...)  TEE_ERROR(-9)
#define CK_PUM_DBG_I(_f, _a ...)
#endif

#define TEE_DRV_EFLASH_NAME     "eflash"
/*(PowerManagement) base Address*/
#define P_CK_PMU                ((ck_pmu_t *)PLATFORM_PMU_ADDRBASE)
#define STORE_REG_SIZE          (0x70 * (sizeof(uint32_t)))

/*
 ******************************
 *          TYPES
 ******************************
 */
/*  CPU work mode */
typedef enum {
    CK_POWM_PREWAIT     = 0 ,
    CK_POWM_PREDOZE     = 1 ,
    CK_POWM_PRESTOP     = 2,
    CK_POWM_PRECLOCK    = 3,
    CK_POWM_PRESTANDBY  = 4,
    CK_POWM_WAIT        = 5 ,
    CK_POWM_DOZE        = 6 ,
    CK_POWM_STOP        = 7,
    CK_POWM_STANDBY     = 8,
    POWM_CLOCK          = 9
} ck_cpu_mode;

typedef struct _ck_pmu_t
{
    volatile uint32_t    LPCR;
    volatile uint32_t    MCLKSEL;
    volatile uint32_t    CRCR;
    volatile uint32_t    CGCR;
    volatile uint32_t    CGSR;
    volatile uint32_t    CLKDSENR;
    volatile uint32_t    CLKSTBR;
    volatile uint32_t    CLKSTBST;
    volatile uint32_t    CLKSTBMK;
    volatile uint32_t    CSSCR;
    volatile uint32_t    DFCC;
    volatile uint32_t    PCR;
    volatile uint32_t    PLTR;
    volatile uint32_t    SWHRC;
    volatile uint32_t    SWHRD;
    volatile uint32_t    SWPRC;
    volatile uint32_t    SWPRD;
    volatile uint32_t    SWRE;
    volatile uint32_t    BOOTSEL;
} ck_pmu_t;

typedef struct _ck_register_t
{
    uint32_t        *base;
    uint32_t        nums;
    uint32_t        *data;
    char            *name;
} ck_register_t;

typedef struct _lpm_args_t
{
    ck_cpu_mode mode;
    uint32_t gate;
    uint32_t irqid;
} lpm_args_t;

/*
 ******************************
 *          VARIABLES
 ******************************
 */
static volatile ck_register_t *_g_pmu_regs = NULL;
static volatile void *_g_cur_reg_strore_addr = NULL;

uint32_t lpm_resume_stack[LPM_RESERVE_STACK_SIZE/sizeof(uint32_t)];
/*
 ******************************
 *          FUNCTIONS
 ******************************
 */
extern uint32_t arch_tw_do_cpu_save(void *addr);
extern uint32_t arch_tw_do_cpu_resume(void *addr);
extern uint32_t resume_from_ram_wrapper(void);
extern uint32_t resume_from_disk_wrapper(void);
extern int32_t hal_uart_init(uint32_t addr, uint32_t baudrate, uint32_t parity,
                            int32_t wordsize, int32_t stopbit);

static int32_t _alloc_for_cur_regs(void)
{
    if (NULL == _g_cur_reg_strore_addr) {
        _g_cur_reg_strore_addr = malloc(STORE_REG_SIZE);
        if (NULL == _g_cur_reg_strore_addr) {
            CK_PUM_DBG_E("out of memory\n");
            return EMEM;
        }
    }

    return SUCCESS;
}

static void _free_for_cur_regs(void)
{
    if (NULL != _g_cur_reg_strore_addr) {
        free((void *)_g_cur_reg_strore_addr);
        _g_cur_reg_strore_addr = NULL;
    }
}

static void _arch_tw_cpu_resume(void)
{
    if (NULL == _g_cur_reg_strore_addr) {
        TEE_ASSERT(0);
    }

    arch_tw_do_cpu_resume((void *)_g_cur_reg_strore_addr);
}

static int32_t _tw_save_pmu_regs(void)
{
    int i = 0;

    _g_pmu_regs = (ck_register_t *)malloc(sizeof(ck_register_t));
    if (NULL == _g_pmu_regs) {
        CK_PUM_DBG_E("out of memory\n");
        return EMEM;
    }
    _g_pmu_regs->name = "pmu";
    _g_pmu_regs->base = (uint32_t *)PLATFORM_PMU_ADDRBASE;
    _g_pmu_regs->nums = 19;
    _g_pmu_regs->data = (uint32_t *)malloc((_g_pmu_regs->nums << 2));
    if (NULL == _g_pmu_regs->data) {
        CK_PUM_DBG_E("out of memory\n");
        return EMEM;
    }

    for (i=0; i<_g_pmu_regs->nums; i++) {
        *(_g_pmu_regs->data + i) = *(_g_pmu_regs->base + i);
    }

    return SUCCESS;
}

static int32_t _tw_restore_pmu_regs(ck_cpu_mode mode)
{
    int i = 0;

    if (mode == CK_POWM_STOP || mode == CK_POWM_STANDBY) {
        for (i=0; i<_g_pmu_regs->nums; i++) {
            *(_g_pmu_regs->base + i) = *(_g_pmu_regs->data + i);
        }

        free(_g_pmu_regs->data);
        free((void *)(_g_pmu_regs));
        _g_pmu_regs = NULL;
    }

    return SUCCESS;
}

static int32_t _eflash_program(uint32_t addr, uint32_t *buf, uint32_t length)
{
    dev_t *ef_dev = NULL;
    tee_eflash_write_args_t write_args;
    int32_t ret = SUCCESS;

    ef_dev = dev_open(TEE_DRV_EFLASH_NAME);
    if (NULL == ef_dev) {
        CK_PUM_DBG_E("dev open elfash drv fail\n");
        return ESYSIO;
    }

#if 0
    int i =0 ;
    for (i=0;i<20;++i) {
        CK_PUM_DBG_I("%08x \n", *(buf + i));
    }
    CK_PUM_DBG_I("\n");
#endif

    write_args.addr = addr;
    write_args.buf = buf;
    write_args.size = length;
    ret = dev_write(ef_dev, write_args.addr, write_args.buf, write_args.size);
    if (length != ret) {
        CK_PUM_DBG_E("eflash write fail 0x%08x\n", ret);
        dev_close(ef_dev);
        return ESYSIO;
    }

    dev_close(ef_dev);

    return SUCCESS;
}

static uint32_t _eflash_read(uint32_t *buf, uint32_t flash_addr, uint32_t length)
{
    uint32_t copied = 0;

    CK_PUM_DBG_I("eflash read 0x%08x buf 0x%08x size 0x%08x\n",
                        flash_addr, (uint32_t)buf, length);

    /* becasue the isram is lost, so we need copy directly */
    if (0 == length) {
        return copied;
    }

#if 0
    int i =0 ;
    for (i=0;i<20;++i) {
        CK_PUM_DBG_I("%08x \n", *((uint32_t *)flash_addr + i));
    }
    CK_PUM_DBG_I("\n");
#endif

    while (copied < length) {
        *((uint8_t *)buf + copied) = *((uint8_t *)flash_addr + copied);
        copied++;
    }

    return copied;
}

/*
 * Function :  resume_from_ram();
 */
void resume_from_ram(void)
{
    int32_t ret = SUCCESS;

    hal_uart_init(PLATFORM_UART_ADDRBASE0, 9600, 0, 8, 1);
    dev_resume();
    CK_PUM_DBG_I("resume from stop\n");
    ret =  _tw_restore_pmu_regs(CK_POWM_STOP);

    _arch_tw_cpu_resume();
}

/*
 * Function : resume_from_disk();
 */
void resume_from_disk(void)
{
    int32_t ret = SUCCESS;

    hal_uart_init(PLATFORM_UART_ADDRBASE0, 9600, 0, 8, 1);
    /* restore isram */
    _eflash_read((uint32_t *)LPM_RW_BASE,
                (uint32_t)LPM_RW_SAVED_ADDR,
                (uint32_t)LPM_RW_SAVED_SIZE);

    dev_resume();
    CK_PUM_DBG_I("resume from standby\n");
    ret = _tw_restore_pmu_regs(CK_POWM_STANDBY);

    _arch_tw_cpu_resume();
}

/*
 * Function :  _resume_addr_save();
 * save the resume function to an address
 * Parameters:
 *     mode -- under the different mode to save the context.
 *     addr -- save the resume function  the addr.
 * Return: 0.
 */
static int32_t _resume_addr_save(ck_cpu_mode mode, uint32_t *addr)
{
    volatile uint32_t resume_addr[2] = {(uint32_t)resume_from_ram_wrapper,
                                        (uint32_t)resume_from_disk_wrapper};
    int32_t ret = SUCCESS;

    if (mode == CK_POWM_STOP) {
        ret = _eflash_program((uint32_t)addr, (uint32_t *)(&resume_addr[0]), 0x4);
    } else if (mode == CK_POWM_STANDBY) {
        ret = _eflash_program((uint32_t)addr, (uint32_t *)(&resume_addr[1]), 0x4);
    }

    return ret;
}

/*
 * Function :  _ck_powm_clockgate();
 */
static int32_t  _ck_powm_clockgate(uint32_t gate)
{
    P_CK_PMU->CGCR = ~gate ;

    return SUCCESS;
}


/*
 * Function :  _save_context();
 * Save the IP Context.
 * Parameters:
 *     mode -- under the different mode to save the context.
 * Return: 0.
 */
static int32_t _save_context(ck_cpu_mode mode)
{
    int32_t ret = SUCCESS;

    ret = _tw_save_pmu_regs();
    if (SUCCESS != ret) {
        return ret;
    }

    if (mode == CK_POWM_STOP) {
        ret = dev_suspend(CK_POWM_STOP);
        if (SUCCESS != ret) {
            CK_PUM_DBG_E("dev suspend fail %x\n", ret);
            goto _err_after_save_pmu;
        }
    } else if (mode == CK_POWM_STANDBY) {
        ret = dev_suspend(CK_POWM_STANDBY);
        if (SUCCESS != ret) {
            CK_PUM_DBG_E("dev suspend fail %x\n", ret);
            goto _err_after_save_pmu;
        }
        ret = _eflash_program((uint32_t)LPM_RW_SAVED_ADDR,
                             (uint32_t *)LPM_RW_BASE,
                             (uint32_t)LPM_RW_SAVED_SIZE);
        if (SUCCESS != ret) {
            CK_PUM_DBG_E("eflash write fail %x\n", ret);
            goto _err_after_suspend;
        }
    }

    return SUCCESS;

_err_after_suspend:
    dev_resume();
_err_after_save_pmu:
    _tw_restore_pmu_regs(mode);

    return ret;
}

/*
 * Function :  _tw_to_wait();
 *  Security, enter Wait mode
 * Parameters:
 *     gate -- choose the ip clock you want to close.
 * Return: 0.
 */
static int32_t _tw_to_wait(uint32_t gate)
{
    P_CK_PMU->LPCR = CK_POWM_LPCR_ENABLE;

    _ck_powm_clockgate(gate);
    __asm__("wait");
    _ck_powm_clockgate(~gate);

    return SUCCESS;
}

/*
 * Function :  _tw_to_doze();
 *  Security, enter Doze mode
 * Return: 0.
 */
static int32_t _tw_to_doze(void)
{
    P_CK_PMU->LPCR = CK_POWM_LPCR_ENABLE;
    __asm__("doze");

    return SUCCESS;
}

#ifdef LPM_BTM_EMU
extern void write_btm_resume_jpm_addr(void);
void write_btm_jmp_addr(void)
{
    write_btm_resume_jpm_addr();
}
#endif

/*
 * Function :  _tw_to_stop();
 *  Security, enter Stop mode
 * Parameters:
 *     mode -- enter the different mode.
 *     irqid -- the wakeup source interrupt num.
 * Return: 0.
 */
static int32_t _tw_to_stop(ck_cpu_mode mode)
{
    P_CK_PMU->LPCR = CK_POWM_LPCR_STOP;
    int32_t ret = SUCCESS;

#ifdef LPM_BTM_EMU
    write_btm_jmp_addr();
#endif
    ret = _alloc_for_cur_regs();
    if (SUCCESS != ret) {
        return ret;
    }
    if (SUCCESS == arch_tw_do_cpu_save((void *)_g_cur_reg_strore_addr)) {
        ret = _save_context(mode);
        if (SUCCESS != ret) {
            goto _finish;
        }
        ret = _resume_addr_save(mode, (uint32_t *)LPM_RESUEM_POINT_SAVED_ADDR);
        if (SUCCESS != ret) {
            goto _finish;
        }
        CK_PUM_DBG_I("enter stop\n");
        __asm__("stop");
    }

_finish:
    _free_for_cur_regs();

    return ret;
}

/*
 * Function :  Security_To_Standby();
 *  Security, enter Standby mode
 * Parameters:
 *     mode -- enter the different mode.
 * Return: 0.
 */
static uint32_t _tw_to_standby(ck_cpu_mode mode)
{
    P_CK_PMU->LPCR = CK_POWM_LPCR_STANDBY ;     /* enter into standby mode */
    int32_t ret = SUCCESS;

#ifdef LPM_BTM_EMU
    write_btm_jmp_addr();
#endif
    ret = _alloc_for_cur_regs();
    if (SUCCESS != ret) {
        return ret;
    }
    if (SUCCESS == arch_tw_do_cpu_save((void *)_g_cur_reg_strore_addr)) {
        ret = _save_context(mode);
        if (SUCCESS != ret) {
            goto _finish;
        }
        ret = _resume_addr_save(mode, (uint32_t *)LPM_RESUEM_POINT_SAVED_ADDR);
        if (SUCCESS != ret) {
            goto _finish;
        }
        CK_PUM_DBG_I("enter standby\n");
        __asm__("stop");
    }

_finish:
    _free_for_cur_regs();

    return ret;
}

/*
 * Function :  sw_to_lpm();
 *  Security, enter the lowpower mode the upper interface.
 * Parameters:
 *     mode -- enter the different mode.
 *     gate -- choose the ip clock you want to close.
 *     irqid -- the wakeup source interrupt num.
 * Return: 0.
 */
int32_t sw_to_lpm(void *msg)
{
    int32_t ret = SUCCESS;
    lpm_args_t *args = (lpm_args_t *)msg;

    CK_PUM_DBG_I("lpm enter mode %d\n", args->mode);
    switch (args->mode) {
        case CK_POWM_WAIT:
            ret = _tw_to_wait(args->gate);
            break;
        case CK_POWM_DOZE:
            ret = _tw_to_doze();
            break;
        case CK_POWM_STOP:
            ret = _tw_to_stop(args->mode);
            break;
        case CK_POWM_STANDBY:
            ret = _tw_to_standby(args->mode);
            break;
        default:
            TEE_ASSERT(0);
    }

    CK_PUM_DBG_I("ret from lpm %d\n", ret);
    return ret;
}

/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */
#include "hal_memmap.h"
#include "tee_asc.h"
#include "tee_dbg.h"
#include "tee_addr_map.h"

#ifdef CONFIG_ASC_DRV_DBG
#define ASC_DRV_DBG_E(str, x...)                                        \
    do {                                                                \
        tee_dbg_print(ERR, "%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)

#define ASC_DRV_DBG_I(str, x...)                                        \
    do {                                                                \
        tee_dbg_print(INF, "%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)
#else
#define ASC_DRV_DBG_E(str, x...) TEE_ERROR(-7)
#define ASC_DRV_DBG_I(str, x...)
#endif


#define IS_ALIGNED(x, n) ((x) % (n) == 0)
#define IS_POWERED(x) !((x) & (x - 1))

#define SASC_REGN_NUM (8)
#define SRAM_GRANULE_BNUM (2)
#define SRAM_REGN_SIZE_NUM (13)
#define FLASH_GRANULE_BNUM (9)
#define FLASH_REGN_SIZE_NUM (10)

typedef struct sram_asc_regs_t
{
    uint32_t car;
    uint32_t cr;
    uint32_t rg[8];
    uint32_t cfg;
} sram_asc_regs_t;
typedef sram_asc_regs_t flash_asc_regs_t;

static int32_t _find_idle_region(bool is_sram, uint32_t *regn_idx)
{
    uint32_t                  i;
    volatile sram_asc_regs_t *asc_regs;

    TEE_ASSERT(regn_idx);

    if (is_sram) {
        asc_regs = (sram_asc_regs_t *)PLATFORM_SRAM_SASC_BADDR;
    } else {
        /* flash asc structure is same as sram's */
        asc_regs = (sram_asc_regs_t *)PLATFORM_FLASH_SASC_BADDR;
    }

    for (i = 0; i < SASC_REGN_NUM; i++) {
        if (asc_regs->cr & (0x1 << i)) {
            continue;
        }

        *regn_idx = i;
        return 0;
    }

    return -1;
}

static int _calc_regn_size(bool is_sram, uint32_t paddr, uint32_t size,
                           uint32_t *regn_size)
{
    uint32_t i              = 0;
    uint32_t regn_alignment = 0;

    TEE_ASSERT(regn_size);

    if (is_sram) {
        for (i = 0; i < SRAM_REGN_SIZE_NUM; i++) {
            if (size == (0x1 << (i + SRAM_GRANULE_BNUM))) {
                *regn_size = i + 5;
                if (i != 0) {
                    regn_alignment = size;
                }

                if (regn_alignment && !IS_ALIGNED(paddr, regn_alignment)) {
                    ASC_DRV_DBG_E("paddr(%08x) does not meet rgn align(%08x)\n",
                                  paddr, regn_alignment);
                    return -1;
                }

                return 0;
            }
        }
    } else {
        for (i = 0; i < FLASH_REGN_SIZE_NUM; i++) {
            if (size == (0x1 << (i + FLASH_GRANULE_BNUM))) {
                *regn_size = i;
                if (i != 0) {
                    regn_alignment = size;
                }

                if (regn_alignment && !IS_ALIGNED(paddr, regn_alignment)) {
                    ASC_DRV_DBG_E(
                      "paddr(%08x) is not meet regn alignment(%08x)\n", paddr,
                      regn_alignment);
                    return -1;
                }

                return 0;
            }
        }
    }

    return -1;
}

static int32_t _asc_set_mem_perm(uint32_t paddr, uint32_t size,
                                 tee_asc_perm_t perm)
{
    int32_t                   ret;
    bool                      is_sram;
    uint32_t                  base_addr;
    uint32_t                  regn_idx, regn_size;
    volatile sram_asc_regs_t *asc_regs;

    TEE_ASSERT(size && IS_POWERED(size));

    if (paddr >= SRAM_START && paddr < SRAM_END) {
        is_sram   = true;
        base_addr = (paddr >> SRAM_GRANULE_BNUM) & 0x7fff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_SRAM_SASC_BADDR;
    } else if (paddr >= FLASH_START && paddr < FLASH_END) {
        is_sram   = false;
        base_addr = (paddr >> FLASH_GRANULE_BNUM) & 0x1ff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_FLASH_SASC_BADDR;
    } else {
        ASC_DRV_DBG_E("set_mem_perm: invalid paddr(%08x)\n", paddr);
        return -1;
    }

    ret = _find_idle_region(is_sram, &regn_idx);
    if (ret < 0) {
        ASC_DRV_DBG_E("_asc_set_mem_perm: no idle region left\n");
        return -1;
    }

    ret = _calc_regn_size(is_sram, paddr, size, &regn_size);
    if (ret < 0) {
        ASC_DRV_DBG_E("_asc_set_mem_perm: calc regn size fail\n");
        return -1;
    }

    if (perm.sec) {
        asc_regs->car |= (0x1 << regn_idx);
    } else {
        asc_regs->car &= ~(0x1 << regn_idx);
    }

    if (is_sram) {
        asc_regs->rg[regn_idx] = (base_addr << 17);
        asc_regs->rg[regn_idx] |= regn_size;
    } else {
        asc_regs->rg[regn_idx] = (base_addr << 23);
        asc_regs->rg[regn_idx] |= regn_size;
    }
    asc_regs->cr |= (0x1 << regn_idx);

    asc_regs->cfg |= ((perm.ap & 0x3) << (regn_idx * 2));
    asc_regs->cfg |= ((perm.cd & 0x3) << (regn_idx * 2 + 16));

#ifdef TEE_ASC_DBG
    ASC_DRV_DBG_I("set_perm -- regn_idx: %d  car: %08x  cr: %08x cfg: %08x  "
                  "regn_rg: %08x\n",
                  regn_idx, asc_regs->car, asc_regs->cr, asc_regs->cfg,
                  asc_regs->rg[regn_idx]);
#endif

    return 0;
}

static int32_t _asc_get_mem_perm(uint32_t paddr, uint32_t size,
                                 tee_asc_perm_t *perm)
{
    int32_t                   ret;
    bool                      is_sram;
    uint32_t                  i = 0;
    uint32_t                  base_addr;
    uint32_t                  regn_size;
    volatile sram_asc_regs_t *asc_regs;

    TEE_ASSERT(perm);
    TEE_ASSERT(size && IS_POWERED(size));

    if (paddr >= SRAM_START && paddr < SRAM_END) {
        is_sram   = true;
        base_addr = (paddr >> SRAM_GRANULE_BNUM) & 0x7fff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_SRAM_SASC_BADDR;
    } else if (paddr >= FLASH_START && paddr < FLASH_END) {
        is_sram   = false;
        base_addr = (paddr >> FLASH_GRANULE_BNUM) & 0x1ff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_FLASH_SASC_BADDR;
    } else {
        ASC_DRV_DBG_E("set_mem_perm: invalid paddr(%08x)\n", paddr);
        return -1;
    }

    ret = _calc_regn_size(is_sram, paddr, size, &regn_size);
    if (ret < 0) {
        ASC_DRV_DBG_E("_asc_set_mem_perm: calc regn size fail\n");
        return -1;
    }

    for (i = 0; i < SASC_REGN_NUM; i++) {
        uint32_t cur_regn_addr, cur_regn_size;

        if (asc_regs->cr & (0x1 << i)) {
            if (is_sram) {
                cur_regn_addr = (asc_regs->rg[i] >> 17) & 0x7fff;
                cur_regn_size = asc_regs->rg[i] & 0x1f;
            } else {
                cur_regn_addr = (asc_regs->rg[i] >> 23) & 0x1ff;
                cur_regn_size = asc_regs->rg[i] & 0xf;
            }

            if (base_addr >= cur_regn_addr &&
                base_addr + regn_size <= cur_regn_addr + cur_regn_size) {
                perm->sec = (asc_regs->car >> i) & 0x1;
                perm->ap  = (asc_regs->cfg >> (2 * i)) & 0x3;
                perm->cd  = (asc_regs->cfg >> (2 * i + 16)) & 0x3;

#ifdef TEE_ASC_DBG
                ASC_DRV_DBG_I(
                  "get_perm -- regn_idx: %d  sec: %d  ap: %d  cd: %d\n", i,
                  perm->sec, perm->ap, perm->cd);
#endif

                return 0;
            }
        }
    }

    return -1;
}

static int32_t _asc_clr_mem_perm(uint32_t paddr, uint32_t size)
{
    int32_t                   ret;
    bool                      is_sram;
    uint32_t                  i = 0;
    uint32_t                  base_addr;
    uint32_t                  regn_size;
    volatile sram_asc_regs_t *asc_regs;

    TEE_ASSERT(size && IS_POWERED(size));

    if (paddr >= SRAM_START && paddr < SRAM_END) {
        is_sram   = true;
        base_addr = (paddr >> SRAM_GRANULE_BNUM) & 0x7fff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_SRAM_SASC_BADDR;
    } else if (paddr >= FLASH_START && paddr < FLASH_END) {
        is_sram   = false;
        base_addr = (paddr >> FLASH_GRANULE_BNUM) & 0x1ff;
        asc_regs  = (sram_asc_regs_t *)PLATFORM_FLASH_SASC_BADDR;
    } else {
        ASC_DRV_DBG_E("set_mem_perm: invalid paddr(%08x)\n", paddr);
        return -1;
    }

    ret = _calc_regn_size(is_sram, paddr, size, &regn_size);
    if (ret < 0) {
        ASC_DRV_DBG_E("_asc_set_mem_perm: calc regn size fail\n");
        return -1;
    }

    for (i = 0; i < SASC_REGN_NUM; i++) {
        uint32_t cur_regn_addr, cur_regn_size;

        if (asc_regs->cr & (0x1 << i)) {
            if (is_sram) {
                cur_regn_addr = (asc_regs->rg[i] >> 17) & 0x7fff;
                cur_regn_size = asc_regs->rg[i] & 0x1f;
            } else {
                cur_regn_addr = (asc_regs->rg[i] >> 23) & 0x1ff;
                cur_regn_size = asc_regs->rg[i] & 0xf;
            }

            if (base_addr >= cur_regn_addr &&
                base_addr + regn_size <= cur_regn_addr + cur_regn_size) {
                asc_regs->car &= ~(0x1 << i);
                asc_regs->cr &= ~(0x1 << i);
                asc_regs->rg[i] = 0x0;
                asc_regs->cfg &= ~(0x1 << 2 * i);
                asc_regs->cfg &= ~(0x1 << (2 * i + 16));

#ifdef TEE_ASC_DBG
                ASC_DRV_DBG_I(
                  "clr_perm -- car: %08x  cr: %08x  rg: %08x  cfg: %08x\n", i,
                  asc_regs->car, asc_regs->cr, asc_regs->rg, asc_regs->cfg);
#endif

                return 0;
            }
        }
    }

    return -1;
}

static int32_t _tee_asc_open(dev_t *dev)
{
    return 0;
}

static int32_t _tee_asc_close(dev_t *dev)
{
    return 0;
}

static int32_t _tee_asc_ioctl(dev_t *dev, int32_t cmd, void *arg)
{
    int            ret;
    tee_asc_arg_t *asc_arg;

    asc_arg = (tee_asc_arg_t *)arg;
    TEE_ASSERT(asc_arg);

    ASC_DRV_DBG_I("_tee_asc_ioctl -- phys: %08x  size: %08x cmd: %d\n",
                  asc_arg->paddr, asc_arg->size, cmd);

    switch (cmd) {
        case TEE_ASC_SET_MEM_PERM: {
            ret =
              _asc_set_mem_perm(asc_arg->paddr, asc_arg->size, asc_arg->perm);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case TEE_ASC_GET_MEM_PERM: {
            ret =
              _asc_get_mem_perm(asc_arg->paddr, asc_arg->size, &asc_arg->perm);
            if (ret < 0) {
                return ret;
            }
        }

        case TEE_ASC_CLR_MEM_PERM: {
            ret = _asc_clr_mem_perm(asc_arg->paddr, asc_arg->size);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        default:
            ASC_DRV_DBG_E("illegal command\n");
            return -1;
    }

    return 0;
}

static dev_ops_t _tee_asc_ops = {
    .open    = _tee_asc_open,
    .close   = _tee_asc_close,
    .ioctl   = _tee_asc_ioctl,
    .read    = NULL,
    .write   = NULL,
    .suspend = NULL,
    .resume  = NULL,
};

static int32_t _tee_asc_probe()
{
    dev_t *dev;

    ASC_DRV_DBG_I("_tee_asc_probe\n");

    dev = dev_register(TEE_DRV_ASC_NAME, &_tee_asc_ops);
    if (dev == NULL) {
        ASC_DRV_DBG_E("_tee_asc_probe: dev regs fail\n");
        return -1;
    }

    return 0;
}

DEV_INIT(_tee_asc_probe, (uint8_t *)TEE_DRV_ASC_NAME);

int asc_set_perm(uint32_t paddr, uint32_t size, bool is_secure, uint8_t ap_perm,
                 uint8_t cd_perm)
{
    int32_t        ret;
    tee_asc_perm_t perm;

    perm.sec =
      ((true == is_secure) ? (REGION_SIZE_SECURITY) : (REGION_SIZE_UNSECURITY));
    perm.ap = ap_perm;
    perm.cd = cd_perm;

    ret = _asc_set_mem_perm(paddr, size, perm);
    if (0 != ret) {
        ASC_DRV_DBG_E(
          "failed to set asc perm: _asc_set_mem_perm return: 0x%x!\n", ret);
        return ret;
    }
    return 0;
}

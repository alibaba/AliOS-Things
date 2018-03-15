/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_srv.h"
#include "tee_asc.h"
#include "tee_dbg.h"
#include "device.h"
#include "asc.h"

#include "tee_tos.h"

#if 0
static TEE_Result test_sram_asc()
{
    int32_t ret;
    dev_t *dev = NULL;
    uint32_t size, paddr;
    tee_asc_arg_t set_arg;
    tee_asc_arg_t get_arg;
    TEE_Result result= TEE_SUCCESS;

    size = ASC_TEST_SRAM_SIZE;
    memset(&set_arg, 0, sizeof(tee_asc_arg_t));
    memset(&get_arg, 0, sizeof(tee_asc_arg_t));

    paddr = (uint32_t)malloc(ASC_TEST_SRAM_SIZE);
    if (0 == paddr) {
        tee_dbg_print(ERR, "out of memory\n");
        return TEE_ERROR_OUT_OF_MEMORY;
    }
    //paddr = ASC_TEST_SRAM_ADDR;

    dev = (dev_t *)dev_open("asc");
    if (dev == NULL) {
        tee_dbg_print(ERR, "fail to open asc drv\n");
        free((void *)paddr);
        return TEE_ERROR_GENERIC;
    }

    set_arg.paddr = paddr;
    set_arg.size = size;
    set_arg.perm.sec = REGION_SIZE_SECURITY;
    set_arg.perm.ap = REGION_WRITE_ONLY;
    set_arg.perm.cd = REGION_BOTH;
    ret = dev_ioctl(dev, TEE_ASC_SET_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

    get_arg.paddr = paddr;
    get_arg.size = size;
    ret = dev_ioctl(dev, TEE_ASC_GET_MEM_PERM, &get_arg);
    if (ret < 0 || memcmp(&set_arg, &get_arg, sizeof(tee_asc_arg_t))) {
        tee_dbg_print(ERR, "fail(%d) to get perm - set perm(%d %d %d) vs get perm(%d %d %d)\n",
                          ret, set_arg.perm.sec, set_arg.perm.ap, set_arg.perm.cd,
                          get_arg.perm.sec, get_arg.perm.ap, get_arg.perm.cd);
        result = TEE_ERROR_GENERIC;
        goto out;
    }

    set_arg.paddr = paddr;
    set_arg.size = size;
    ret = dev_ioctl(dev, TEE_ASC_CLR_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

out:
    if (dev) {
        dev_close(dev);
    }
    if (paddr) {
        free((void *)paddr);
    }
    return result;
}

static TEE_Result test_flash_asc()
{
    int32_t ret;
    dev_t *dev = NULL;
    uint32_t phys;
    uint32_t size = ASC_TEST_FLASH_SIZE;
    tee_asc_arg_t set_arg;
    tee_asc_arg_t get_arg;
    TEE_Result result= TEE_SUCCESS;

    memset(&set_arg, 0, sizeof(tee_asc_arg_t));
    memset(&get_arg, 0, sizeof(tee_asc_arg_t));

    dev = (dev_t *)dev_open("asc");
    if (dev == NULL) {
        tee_dbg_print(ERR, "fail to open asc drv\n");
        return TEE_ERROR_GENERIC;
    }

    phys = ASC_TEST_FALSH_ADDR;
    set_arg.paddr = phys;
    set_arg.size = size;
    set_arg.perm.sec = REGION_SIZE_SECURITY;
    set_arg.perm.ap = REGION_WRITE_ONLY;
    set_arg.perm.cd = REGION_BOTH;
    ret = dev_ioctl(dev, TEE_ASC_SET_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

    get_arg.paddr = phys;
    get_arg.size = size;
    ret = dev_ioctl(dev, TEE_ASC_GET_MEM_PERM, &get_arg);
    if (ret < 0 || memcmp(&set_arg, &get_arg, sizeof(tee_asc_arg_t))) {
        tee_dbg_print(ERR, "fail(%d) to get perm - set perm(%d %d %d) vs get perm(%d %d %d)\n",
                          ret, set_arg.perm.sec, set_arg.perm.ap, set_arg.perm.cd,
                          get_arg.perm.sec, get_arg.perm.ap, get_arg.perm.cd);
        result = TEE_ERROR_GENERIC;
        goto out;
    }

    set_arg.paddr = phys;
    set_arg.size = size;
    ret = dev_ioctl(dev, TEE_ASC_CLR_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

out:
    if (dev) {
        dev_close(dev);
    }

    return result;
}
#endif

static TEE_Result test_set_mem_perm(uint32_t addr, uint32_t size)
{
    int32_t ret;
    dev_t *dev = NULL;
    tee_asc_arg_t set_arg;
    TEE_Result result= TEE_SUCCESS;

    dev = (dev_t *)dev_open("asc");
    if (dev == NULL) {
        tee_dbg_print(ERR, "fail to open asc drv\n");
        return TEE_ERROR_GENERIC;
    }

    set_arg.paddr = addr;
    set_arg.size = size;
    set_arg.perm.sec = REGION_SIZE_SECURITY;
    set_arg.perm.ap = REGION_READ_ONLY;
    set_arg.perm.cd = REGION_BOTH;
    ret = dev_ioctl(dev, TEE_ASC_SET_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

out:
    if (dev) {
        dev_close(dev);
    }

    return result;
}

static TEE_Result test_clr_mem_perm(uint32_t addr, uint32_t size)
{
    int32_t ret;
    dev_t *dev = NULL;
    tee_asc_arg_t set_arg;
    TEE_Result result= TEE_SUCCESS;

    dev = (dev_t *)dev_open("asc");
    if (dev == NULL) {
        tee_dbg_print(ERR, "fail to open asc drv\n");
        return TEE_ERROR_GENERIC;
    }

    set_arg.paddr = addr;
    set_arg.size = size;
    ret = dev_ioctl(dev, TEE_ASC_CLR_MEM_PERM, &set_arg);
    if (ret < 0) {
        result = TEE_ERROR_GENERIC;
        goto out;
    }

out:
    if (dev) {
        dev_close(dev);
    }

    return result;
}

static TEE_Result _asc_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _asc_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _asc_OpenSessionEntryPoint(
        uint32_t paramTypes,
        TEE_Param params[4],
        void **sessionContext)
{
    return TEE_SUCCESS;
}

static void _asc_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _asc_InvokeCommandEntryPoint(
        void *sessionContext,
        uint32_t commandID,
        uint32_t paramTypes,
        TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;

    if (paramTypes != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE) &&
        paramTypes != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_VALUE_INPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

#if 0
    if (commandID == TEE_ASC_SRAM) {
        ret = test_sram_asc();
    } else if (commandID == TEE_ASC_FLASH) {
        ret = test_flash_asc();
    } else
#endif
    if (commandID == TEE_SET_PERM) {
        ret = test_set_mem_perm(params[0].value.a,
                                params[0].value.b);
    } else if (commandID == TEE_CLR_PERM) {
        ret = test_clr_mem_perm(params[0].value.a,
                                params[0].value.b);
    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

TEE_SRV_DATA_START(_asc_CreateEntryPoint,
        _asc_DestroyEntryPoint,
        _asc_OpenSessionEntryPoint,
        _asc_CloseSessionEntryPoint,
        _asc_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", ASC_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END

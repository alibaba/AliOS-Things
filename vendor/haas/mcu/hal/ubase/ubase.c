/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "aos/kernel.h"

#if 1 // use uspace heap
void *rt_malloc(size_t size)
{
	return aos_malloc(size);
}

void *rt_realloc(void *rmem, size_t newsize)
{
	return aos_realloc(rmem, newsize);
}

void *rt_calloc(size_t count, size_t size)
{
	return aos_calloc(count, size);
}

void rt_free(void *rmem)
{
	aos_free(rmem);
}
#endif

// for factory section
#include <sys/types.h>
#include <unistd.h>
#include "aos/hal/flash.h"
#include "norflash_api.h"
#include "factory_section.h"
#include "hal2vfs/io_flash.h"
extern uint32_t *__factory_start;
extern uint32_t *__factory_end;
static int _factory_fd = -1;

enum NORFLASH_API_RET_T norflash_api_register(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                enum HAL_NORFLASH_ID_T dev_id,
                uint32_t mod_base_addr,
                uint32_t mod_len,
                uint32_t mod_block_len,
                uint32_t mod_sector_len,
                uint32_t mod_page_len,
                uint32_t buffer_len,
                NORFLASH_API_OPERA_CB cb_func
                )
{
    __factory_start = malloc(FACTORY_SECTOR_SIZE);
	printf("%s %d __factory_start=%p\n", __FUNCTION__, __LINE__, __factory_start);
	if (!__factory_start) {
		printf("%s %d fail!! __factory_start malloc\n", __FUNCTION__, __LINE__);
		return NORFLASH_API_BUFFER_FULL;
	}
	__factory_end = __factory_start + FACTORY_SECTOR_SIZE;

    char dev_str[16];
    snprintf(dev_str, 16-1, "/dev/flash%d", HAL_PARTITION_ENV);

    _factory_fd = open(dev_str, 0);
    printf("open flash %s %s, _factory_fd:%d, __factory_start=%p\n", dev_str, (_factory_fd > 0 ? "success" : "fail"), _factory_fd, __factory_start);

    if (_factory_fd > 0) {
#if 0
        ret = lseek(fd,0,SEEK_SET);
        printf("lseek of flash return %d\n", ret);
        ret = write(fd, "test", sizeof("test"));
        printf("write to flash return %d\n", ret);
#endif
        lseek(_factory_fd, FACTORY_SECTOR_SIZE, SEEK_SET);
        int ret = read(_factory_fd, __factory_start, FACTORY_SECTOR_SIZE);
		if (ret != FACTORY_SECTOR_SIZE) {
			printf("%s %d fail!! __factory_start read ret=%d[need %d]\n", __FUNCTION__, __LINE__, ret, FACTORY_SECTOR_SIZE);
			return NORFLASH_API_BAD_LEN;
		}
#if 0
		char *_dump = (char *)__factory_start;
		for (int i=0; i<FACTORY_SECTOR_SIZE; i++) {
			printf("%02X ", _dump[i]);
		}
#endif
    }
	return NORFLASH_API_OK;
}

enum NORFLASH_API_RET_T norflash_api_erase(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                uint32_t len,
                bool async
                )
{
    int ret = -1;
    printf("erase _factory_fd:%d, start_addr=%p, len=%d\n", _factory_fd, (void *)start_addr, len);

    if (_factory_fd > 0) {
        lseek(_factory_fd, FACTORY_SECTOR_SIZE, SEEK_SET);
        ret = ioctl(_factory_fd, IOC_FLASH_ERASE_FLASH, len);
		if (0 != ret) {
			printf("%s %d fail!! ioctl IOC_FLASH_ERASE_FLASH ret=%d\n", __FUNCTION__, __LINE__, ret);
			return NORFLASH_API_BAD_LEN;
		}
    }
	return NORFLASH_API_OK;
}

enum NORFLASH_API_RET_T norflash_api_write(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                const uint8_t *buffer,
                uint32_t len,
                bool async
                )
{
    int ret = -1;
    printf("write _factory_fd:%d, start_addr=%p, buffer=%p, len=%d\n", _factory_fd, (void *)start_addr, buffer, len);

    if (_factory_fd > 0) {
        lseek(_factory_fd, FACTORY_SECTOR_SIZE, SEEK_SET);
        ret = write(_factory_fd, buffer, len);
		if (ret != len) {
			printf("%s %d fail!! write ret=%d\n", __FUNCTION__, __LINE__, ret);
			return NORFLASH_API_BAD_LEN;
		}
    }
	return NORFLASH_API_OK;
}


#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
static u8 charTodata(const char ch)
{
    switch(ch)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a':
    case 'A': return 10;
    case 'b':
    case 'B': return 11;
    case 'c':
    case 'C': return 12;
    case 'd':
    case 'D': return 13;
    case 'e':
    case 'E': return 14;
    case 'f':
    case 'F': return 15;
    }
    return 0;
}

static void str2mac(const char * szMac,UINT8 * pMac)
{
    const char * pTemp = szMac;
    for (int i = 0;i < 6;++i)
    {
        pMac[i] = charTodata(*pTemp++) * 16;
        pMac[i] += charTodata(*pTemp++);
        pTemp++;
    }
}

#include "aos/cli.h"
static void __ali_factory_mode(int argc, char **argv)
{
    aos_cli_printf("%s ... %d, %s \n",__func__,argc,argv[1]);
    if(argc < 1)
    {
        return;
    }

    if(argc >= 2)
    {
        int ret = -1;
        if(strncmp(argv[1], "ATE_SET_WMAC", strlen(argv[1])) == 0)
        {
            UINT8 pMac[6];
            str2mac(argv[2], pMac);
            ret = factory_section_set_wifi_address(pMac);
        }
        else if(strncmp(argv[1], "ATE_GET_WMAC", strlen(argv[1])) == 0)
        {
            uint8_t *_mac = factory_section_get_wifi_address();
            if(_mac) {
                ret = 0;
                aos_cli_printf("%s %d mac: " MACSTR "\n", __FUNCTION__, __LINE__, MAC2STR(_mac));
            } else {
                ret = -1;
            }
        }

        if(ret >= 0)
            aos_cli_printf("success \n");
        else
            aos_cli_printf("failed \n");
    }
}

void _ali_factory_mode(char *pbuffer, int outlen, int argc, char **argv)
{
    __ali_factory_mode(argc, argv);
}

struct cli_command ali_factory_cmd[] = {
    {"factory", "ali-factory test", _ali_factory_mode},
};

void vendor_usapce_cli_register_init(void)
{
    aos_cli_register_commands(ali_factory_cmd, 1);
}

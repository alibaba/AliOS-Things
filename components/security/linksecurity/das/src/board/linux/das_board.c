/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/hardware.h>

#ifdef DAS_SERVICE_ROM_ENABLED
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <elf.h>
#endif

size_t das_hal_firmware_version(char *buf, size_t size)
{
    size_t len;
    char *fw_ver = "app-1.0.0-20180101.1000";

    len = strlen(fw_ver);
    if (size <= len) {
        DAS_LOG("short buffer, %d\n", (int)len);
        return 0;
    }

    memcpy(buf, fw_ver, len);
    buf[len] = '\0';

    return len;
}

size_t das_hal_device_id(char *buf, size_t size)
{
    size_t len;
    char *dev_id = "1234567890";

    len = strlen(dev_id);
    if (size <= len) {
        DAS_LOG("short buffer, %d\n", (int)len);
        return 0;
    }

    memcpy(buf, dev_id, len);
    buf[len] = '\0';

    return len;
}

#ifdef DAS_SERVICE_ROM_ENABLED

static void *das_get_text_section(size_t *size)
{
    int i, fd;
    struct stat st;
    uint8_t *buf = NULL;
    void *addr = NULL;

    fd = open("/proc/self/exe", O_RDONLY);
    if (fd < 0) {
        DAS_LOG("open fail, errno: %d\n", errno);
        return NULL;
    }

    if (fstat(fd, &st) < 0) {
        goto _out;
    }

    buf = malloc(st.st_size);
    if (buf == NULL) {
        goto _out;
    }

    if (read(fd, buf, st.st_size) != st.st_size) {
        DAS_LOG("read fail, errno: %d\n", errno);
        goto _out;
    }

    if (buf[4] == 0x01) {
        Elf32_Ehdr *e_head = (Elf32_Ehdr *)buf;
        Elf32_Shdr *s_head;
        unsigned char *s_table;

        s_head = (Elf32_Shdr *)(buf + e_head->e_shoff);
        s_table = buf + s_head[e_head->e_shstrndx].sh_offset;

        for (i = 0; i < e_head->e_shnum; i++) {
            if (!strcmp((char *)s_table + s_head->sh_name, ".text")) {
                 break;
            }

            s_head++;
        }

        addr = (void *)(size_t)s_head->sh_addr;
        *size = s_head->sh_size;
    } else {
        Elf64_Ehdr *e_head = (Elf64_Ehdr *)buf;
        Elf64_Shdr *s_head;
        unsigned char *s_table;

        s_head = (Elf64_Shdr *)(buf + e_head->e_shoff);
        s_table = buf + s_head[e_head->e_shstrndx].sh_offset;

        for (i = 0; i < e_head->e_shnum; i++) {
            if (!strcmp((char *)s_table + s_head->sh_name, ".text")) {
                break;
            }

            s_head++;
         }

         addr = (void *)(size_t)s_head->sh_addr;
         *size = s_head->sh_size;
    }


_out:
    if (fd > 0) {
        close(fd);
    }
    if (buf != NULL) {
        free(buf);
    }

    return addr;
}

int das_hal_rom_info(das_rom_bank_t banks[DAS_ROM_BANK_NUMBER])
{
    void *addr;
    size_t size;

    addr = das_get_text_section(&size);
    if (addr == NULL) {
        DAS_LOG("get text section fail\n");
        return -1;
    }

    banks[0].address = addr;
    banks[0].size = size;

    return DAS_ROM_BANK_NUMBER;
}

#endif /* DAS_SERVICE_ROM_ENABLED */


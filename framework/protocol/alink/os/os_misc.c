/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "os.h"
#include "aos/aos.h"

#ifndef isprint
#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

void os_squeeze(uint8_t s[], int c, int len)
{
    int i, j;
    for (i = 0, j = 0; i < len; i++) {
        if (s[i] != c) {
            s[j++] = s[i];
        }
    }
    if (j < i) {
        s[j] = '\0';
    }
}

/****** Convert values between host and big-/little-endian byte order ******/

//reverse byte order
static inline uint16_t reverse_16bit(uint16_t data)
{
    return (data >> 8) | (data << 8);
}

//host byte order to big endian
uint16_t os_htobe16(uint16_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_16bit(data);
}

//host byte order to little endian
uint16_t os_htole16(uint16_t data)
{
    if (os_is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

//big endian to host byte order
uint16_t os_be16toh(uint16_t data)
{
    return os_htobe16(data);
}

//little endian to host byte order
uint16_t os_le16toh(uint16_t data)
{
    return os_htole16(data);
}

//reverse byte order
static inline uint32_t reverse_32bit(uint32_t data)
{
    data = (data >> 16) | (data << 16);
    return ((data & 0xff00ff00UL) >> 8) | ((data & 0x00ff00ffUL) << 8);
}

//little endian to host byte order
uint32_t os_le32toh(uint32_t data)
{
    return os_htole32(data);
}

//reverse byte order
static inline uint64_t reverse_64bit(uint64_t data)
{
    data = (data >> 32) | (data << 32);
    data = ((data & 0xff00ff00ff00ff00ULL) >> 8) | ((data & 0x00ff00ff00ff00ffULL)
                                                    << 8);

    return ((data & 0xffff0000ffff0000ULL) >> 16) | ((data & 0x0000ffff0000ffffULL)
                                                     << 16);
}

//host to little endian
uint64_t os_htole64(uint64_t data)
{
    if (os_is_big_endian()) {
        return reverse_64bit(data);
    }

    return data;
}

//little endian to host
uint64_t os_le64toh(uint64_t data)
{
    return os_htole64(data);
}

//get unaligned data in big endian.
uint16_t os_get_unaligned_be16(uint8_t *ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_be16toh(res);
}

//get unaligned data in little endian.
uint16_t os_get_unaligned_le16(uint8_t *ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_le16toh(res);

}

uint32_t os_get_unaligned_be32(uint8_t *ptr)
{
    uint32_t res;

    memcpy(&res, ptr, sizeof(uint32_t));

    return os_be32toh(res);
}

//get unaligned data in little endian.
uint32_t os_get_unaligned_le32(uint8_t *ptr)
{
    uint32_t res;

    memcpy(&res, ptr, sizeof(uint32_t));

    return os_le32toh(res);

}

uint16_t os_htons(uint16_t n)
{
    return os_htobe16(n);
}

uint16_t os_ntohs(uint16_t n)
{
    return os_be16toh(n);
}

uint32_t os_htonl(uint32_t n)
{
    return os_htobe32(n);
}

uint32_t os_ntohl(uint32_t n)
{
    return os_be32toh(n);
}

char *os_ntoa(const uint32_t ip, char buf[OS_IP_LEN])
{
    char inv[3];
    char *rp;
    uint8_t *ap;
    uint8_t rem;
    uint8_t n;
    uint8_t i;

    rp = buf;
    ap = (uint8_t *) & ip;

    for (n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (uint8_t) 10;
            *ap /= (uint8_t) 10;
            inv[i++] = '0' + rem;
        } while (*ap);

        while (i--) {
            *rp++ = inv[i];
        }

        *rp++ = '.';
        ap++;
    }

    *--rp = 0;
    return buf;
}

uint32_t os_aton(const char *ip_str)
{
    uint32_t val = 0;
    uint8_t base;
    char c;
    uint32_t parts[4];
    uint32_t *pp = parts;

    c = *ip_str;
    for (;;) {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, 1-9=decimal.
         */
        if (!isdigit(c)) {
            return (0);
        }
        val = 0;
        base = 10;
        if (c == '0') {
            c = *++ip_str;
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++ip_str;
            } else {
                base = 8;
            }
        }
        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (int)(c - '0');
                c = *++ip_str;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++ip_str;
            } else {
                break;
            }
        }
        if (c == '.') {
            /*
             * Internet format:
             *  a.b.c.d
             *  a.b.c   (with c treated as 16 bits)
             *  a.b (with b treated as 24 bits)
             */
            if (pp >= parts + 3) {
                return (0);
            }
            *pp++ = val;
            c = *++ip_str;
        } else {
            break;
        }
    }
    /*
     * Check for trailing characters.
     */
    if (c != '\0' && !isspace(c)) {
        return (0);
    }
    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    switch (pp - parts + 1) {

        case 0:
            return (0);                                 /* initial nondigit */

        case 1:                                         /* a -- 32 bits */
            break;

        case 2:                                         /* a.b -- 8.24 bits */
            if (val > 0xffffffUL) {
                return (0);
            }
            val |= parts[0] << 24;
            break;

        case 3:                                         /* a.b.c -- 8.8.16 bits */
            if (val > 0xffff) {
                return (0);
            }
            val |= (parts[0] << 24) | (parts[1] << 16);
            break;

        case 4:                                         /* a.b.c.d -- 8.8.8.8 bits */
            if (val > 0xff) {
                return (0);
            }
            val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
            break;
        default:
            OS_ASSERT(0, "unhandled");
            break;
    }

    return val;
}

/* format mac string uppercase */
char *os_wifi_get_mac_str(char mac_str[OS_MAC_LEN])
{
    char *str;
    int colon_num = 0, i;

    str = platform_wifi_get_mac(mac_str);

    /* sanity check */
    while (str) {
        str = strchr(str, ':');
        if (str) {
            colon_num++;
            str++; /* eating char ':' */
        }
    }

    OS_ASSERT(colon_num == (OS_ETH_ALEN - 1)
              && strlen(mac_str) == OS_MAC_LEN - 1,
              "invalid mac str format");

    for (i = 0; i < OS_MAC_LEN && mac_str[i]; i++) {
        if ('a' <= mac_str[i] && mac_str[i] <= 'z') {
            mac_str[i] -= 'a' - 'A';
        }
    }

    return mac_str;
}

uint8_t *os_wifi_get_mac(uint8_t mac[OS_ETH_ALEN])
{
    char mac_str[OS_MAC_LEN] = { 0 };
    int i = 0;
    char *ptr = mac_str;

    os_wifi_get_mac_str(mac_str);

    while ('\0' != *ptr) {
        mac[i++] = (uint8_t)strtol(ptr, &ptr, 16);
        ++ptr;
    }

    return mac;
}

#ifdef DEBUG
int memory_current_usage;
int memory_peak_usage;
void *os_malloc_lock;
static dlist_t os_malloc_list = AOS_DLIST_INIT(os_malloc_list);

/*
 * with DEBUG enabled, each memory block was prefixed with os_malloc_node, and
 * suffixed with os_malloc_magic string.
 * os_malloc_node info + memory block + os_malloc_magic_string
 */

#define MALLOC_MAGIC         "OSMA"
#define MALLOC_MAGIC_LEN    (strlen(MALLOC_MAGIC))
#define MALLOC_NODE_LEN     (sizeof(struct os_malloc_node) + MALLOC_MAGIC_LEN)

struct os_malloc_node {
    long size;
    dlist_t list;
    char caller[STR_SHORT_LEN]; /* caller name */
    void *ptr; /* pointer to memory */
};

void os_init(void)
{
    if (!os_malloc_lock) {
        os_malloc_lock = os_mutex_init();
        dlist_init(&os_malloc_list);
        memory_current_usage = 0;
        memory_peak_usage = 0;
    }
}

void os_exit(void)
{
    if (os_malloc_lock) {
        struct os_malloc_node *node;
        int memory_leak = 0;
        dlist_t *tmp = NULL;
        dlist_for_each_entry_safe(&os_malloc_list, tmp, node, struct os_malloc_node,
                                  list) {
            platform_printf("memory leak, ptr:%p, size:%d, caller:%s\r\n",
                            node->ptr, node->size, node->caller);
            memory_leak += node->size;
        }

        if (memory_leak) {
            platform_printf("memory_leak total size:%d\r\n", memory_leak);
        }

        platform_printf("memory max usage: %d\r\n", memory_peak_usage);

        //OS_ASSERT(memory_leak == 0, "memory leak!");

        os_mutex_destroy(os_malloc_lock);
        os_malloc_lock = NULL;
    }
}

void os_malloc_dump_memory_usage(void)
{
    struct os_malloc_node *node;
    dlist_t *tmp = NULL;
    if (!os_malloc_lock) {
        return;
    }

    os_mutex_lock(os_malloc_lock);
    dlist_for_each_entry_safe(&os_malloc_list, tmp, node, struct os_malloc_node,
                              list)
    platform_printf("memory ptr:%p, size:%d, caller:%s\r\n",
                    node->ptr, node->size, node->caller);
    os_mutex_unlock(os_malloc_lock);
}

int os_malloc_get_current_usage(void)
{
    return memory_current_usage;
}
int os_malloc_get_peak_usage(void)
{
    return memory_peak_usage;
}

#else
void os_init(void) {}
void os_exit(void) {}
int os_malloc_current_usage(void) {}
int os_malloc_get_current_usage(void) {}
int os_malloc_get_peak_usage(void) {}
#endif

void *__os_malloc_debug(const char *name, uint32_t size)
{
#ifdef DEBUG
    char *buffer = NULL;
    struct os_malloc_node *node;

    if (!os_malloc_lock) {
        os_init();
    }

    size += sizeof(struct os_malloc_node) + MALLOC_MAGIC_LEN;
    buffer = platform_malloc(size);

    if (buffer) {
        /* suppose os_malloc return address align to struct os_malloc_node boundary */
        node = (struct os_malloc_node *)buffer;
        node->size = size;
        strncpy(node->caller, name, STR_SHORT_LEN);
        node->caller[STR_SHORT_LEN - 1] = '\0';
        node->ptr = buffer;
        dlist_init(&node->list);

        os_mutex_lock(os_malloc_lock);
        dlist_add(&node->list, &os_malloc_list);
        os_mutex_unlock(os_malloc_lock);

        memcpy(buffer + size - MALLOC_MAGIC_LEN, MALLOC_MAGIC, MALLOC_MAGIC_LEN);

        memory_current_usage += size;
        if (memory_peak_usage < memory_current_usage) {
            memory_peak_usage = memory_current_usage;
        }

        //platform_printf("os_malloc:%p, size:%d, caller:%s\r\n",
        //       buffer, size - sizeof(struct os_malloc_node) - MALLOC_MAGIC_LEN, name);
        return buffer + sizeof(struct os_malloc_node);
    } else {
        return NULL;
    }
#else
    return platform_malloc(size);
#endif
}

void __os_free_debug(const char *name, void *ptr)
{
#ifdef DEBUG
    int size = 0, equal = 0;
    char *buffer = ptr, *magic;
    struct os_malloc_node *node;

    OS_ASSERT(os_malloc_lock, "os_malloc mutext lock uninit");
    if (!ptr) {
        platform_printf("#######os_free NULL pointer, caller: %s, pointer: %p\n", name,
                        ptr);
        return;
    }

    buffer -= sizeof(struct os_malloc_node);
    node = (struct os_malloc_node *)buffer;
    size = node->size;

    magic = buffer + size - MALLOC_MAGIC_LEN;

    equal = memcmp(magic, MALLOC_MAGIC, MALLOC_MAGIC_LEN);
    OS_ASSERT(equal == 0, "caller:%s os_malloc buffer:%p, size:%d overflow!",
              node->caller, buffer,
              size - sizeof(struct os_malloc_node) - MALLOC_MAGIC_LEN);

    OS_ASSERT(buffer == node->ptr,
              "wired, invaid os_free pointer real(%p) != expect(%p)",
              buffer, node->ptr);

    os_mutex_lock(os_malloc_lock);
    dlist_del(&node->list);
    os_mutex_unlock(os_malloc_lock);

    memory_current_usage -= size;

    //platform_printf("os_free: %p, size:%d, caller:%s\r\n",
    //           buffer, size - sizeof(struct os_malloc_node) - MALLOC_MAGIC_LEN, name);
    platform_free(buffer);
#else
    platform_free(ptr);
#endif
}

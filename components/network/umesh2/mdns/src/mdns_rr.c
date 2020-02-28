#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils.h"
#include "network/umesh2/mdns/mdns_rr.h"


typedef const uint8_t *(*mdns_reader)(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);
typedef uint32_t (*mdns_writer)(uint8_t *, const struct mdns_entry *);
typedef void (*mdns_printer)(const union mdns_data *);

static const uint8_t *mdns_decode(const uint8_t *ptr, uint32_t *n, const uint8_t *root, char **ss);
static uint8_t *mdns_encode(char *s);

const uint8_t *mdns_read(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry, int8_t ans);
static const uint8_t *mdns_read_SRV(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);
static const uint8_t *mdns_read_PTR(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);
static const uint8_t *mdns_read_TXT(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);
static const uint8_t *mdns_read_AAAA(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);
static const uint8_t *mdns_read_A(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *);

uint32_t mdns_write(uint8_t *ptr, const struct mdns_entry *entry, int8_t ans);
static uint32_t mdns_write_SRV(uint8_t *, const struct mdns_entry *);
static uint32_t mdns_write_PTR(uint8_t *, const struct mdns_entry *);
static uint32_t mdns_write_TXT(uint8_t *, const struct mdns_entry *);
static uint32_t mdns_write_AAAA(uint8_t *, const struct mdns_entry *);
static uint32_t mdns_write_A(uint8_t *, const struct mdns_entry *);

void mdns_print(const struct mdns_entry *entry);
static void mdns_print_SRV(const union mdns_data *);
static void mdns_print_PTR(const union mdns_data *);
static void mdns_print_TXT(const union mdns_data *);
static void mdns_print_AAAA(const union mdns_data *);
static void mdns_print_A(const union mdns_data *);

void mdns_free(struct mdns_entry *entry);

static const char *mdns_type_str(enum mdns_type);
static const char *mdns_class_str(enum mdns_class);

#define advance(x) ptr += x; *n -= x

struct mdns_records {
    enum       mdns_type type;
    const char *name;
    mdns_reader  read;
    mdns_writer  write;
    mdns_printer print;

};

static const struct mdns_records rrs[] = {
    {RR_SRV,  "SRV",  &mdns_read_SRV,  &mdns_write_SRV,  &mdns_print_SRV},
    {RR_PTR,  "PTR",  &mdns_read_PTR,  &mdns_write_PTR,  &mdns_print_PTR},
    {RR_TXT,  "TXT",  &mdns_read_TXT,  &mdns_write_TXT,  &mdns_print_TXT},
    {RR_AAAA, "AAAA", &mdns_read_AAAA, &mdns_write_AAAA, &mdns_print_AAAA},
    {RR_A,    "A",    &mdns_read_A,    &mdns_write_A,    &mdns_print_A},
};

static const uint32_t mdns_num = sizeof(rrs) / sizeof(*rrs);

static const uint8_t *mdns_read_SRV(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry)
{
    union mdns_data *data = &entry->data;

    if (*n <= sizeof(uint16_t) * 3) {
        log_e("too short");
        return (NULL);
    }

    ptr = read_u16(ptr, n, &data->SRV.priority);
    ptr = read_u16(ptr, n, &data->SRV.weight);
    ptr = read_u16(ptr, n, &data->SRV.port);
    if ((ptr = mdns_decode(ptr, n, root, &data->SRV.target)) == NULL) {
        return (NULL);
    }
    return (ptr);
}

static uint32_t mdns_write_SRV(uint8_t *ptr, const struct mdns_entry *entry)
{
    uint8_t *target, *p = ptr;

    if ((target = mdns_encode(entry->data.SRV.target)) == NULL) {
        return (0);
    }

    p = write_u16(p, entry->data.SRV.priority);
    p = write_u16(p, entry->data.SRV.weight);
    p = write_u16(p, entry->data.SRV.port);
    p = write_raw(p, target);
    free(target);
    return (p - ptr);
}

static void mdns_print_SRV(const union mdns_data *data)
{
    log_d("{\"target\":\"%s\","
          "\"port\":%" PRIu16 ","
          "\"priority\":%" PRIu16 ","
          "\"weight\":%" PRIu16
          "}", data->SRV.target, data->SRV.port, data->SRV.priority, data->SRV.weight);
}

static const uint8_t *mdns_read_PTR(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry)
{
    union mdns_data *data = &entry->data;

    if (*n == NULL) {
        log_e("*n == NULL");
        return (NULL);
    }

    if ((ptr = mdns_decode(ptr, n, root, &data->PTR.domain)) == NULL) {
        log_e("mdns_decode failed");
        return (NULL);
    }
    return (ptr);
}

static uint32_t mdns_write_PTR(uint8_t *ptr, const struct mdns_entry *entry)
{
    uint8_t *domain, *p = ptr;
    if ((domain = mdns_encode(entry->data.PTR.domain)) == NULL) {
        return (0);
    }
    p = write_raw(p, domain);
    free(domain);
    return (p - ptr);
}

static void mdns_print_PTR(const union mdns_data *data)
{
    log_d("{\"domain\":\"%s\"}", data->PTR.domain);
}

static const uint8_t *mdns_read_TXT(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry)
{
    union mdns_data *data = &entry->data;
    uint16_t len = entry->data_len;
    uint8_t l;

    if (*n == 0 || *n < len) {
        return (NULL);
    }

    for (; len > 0; len -= l + 1) {
        struct mdns_data_txt *text;

        memcpy(&l, ptr, sizeof(l));
        advance(1);
        if (*n < l) {
            return (NULL);
        }
        text = malloc(sizeof(struct mdns_data_txt));
        if (!text) {
            return (NULL);
        }
        text->next = data->TXT;
        data->TXT = text;
        if (l > 0) {
            memcpy(text->txt, ptr, l);
        }
        text->txt[l] = '\0';
        advance(l);
    }
    return (ptr);
}

static uint32_t mdns_write_TXT(uint8_t *ptr, const struct mdns_entry *entry)
{
    uint8_t *p = ptr;
    uint8_t l;

    struct mdns_data_txt *text = entry->data.TXT;
    while (text) {
        l = strlen(text->txt);
        memcpy(p, &l, 1);
        memcpy(p + 1, text->txt, l);
        p += l + 1;
        text = text->next;
    }
    return (p - ptr);
}

static void mdns_print_TXT(const union mdns_data *data)
{
    struct mdns_data_txt *text = data->TXT;

    log_d("{\"text\":[");
    while (text) {
        log_d("\"%s\"%s", text->txt, text->next ? "," : "");
        text = text->next;
    }
    log_d("]}");
}

static const uint8_t *mdns_read_AAAA(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry)
{
    union mdns_data *data = &entry->data;
    const uint32_t len = sizeof(struct in6_addr);

    if (*n < len || entry->data_len < len) {
        return (NULL);
    }

    memcpy(&data->AAAA.addr, ptr, len);
    advance(len);
    if (!inet_ntop(AF_INET6, &data->AAAA.addr, data->AAAA.addr_str, INET6_ADDRSTRLEN)) {
        return (NULL);
    }
    return (ptr);
}

static uint32_t mdns_write_AAAA(uint8_t *ptr, const struct mdns_entry *entry)
{
    uint32_t len = sizeof(entry->data.AAAA.addr);
    memcpy(ptr, &entry->data.AAAA.addr, len);
    return len;
}

static void mdns_print_AAAA(const union mdns_data *data)
{
    log_d("{\"address\":\"%s\"}", data->AAAA.addr_str);
}

static const uint8_t *mdns_read_A(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry)
{
    union mdns_data *data = &entry->data;
    const uint32_t len = sizeof(struct in_addr);

    if (*n < len) {
        return (NULL);
    }

    memcpy(&data->A.addr, ptr, len);
    advance(len);
    if (!inet_ntop(AF_INET, &data->A.addr, data->A.addr_str, INET_ADDRSTRLEN)) {
        return (NULL);
    }
    return (ptr);
}

static uint32_t mdns_write_A(uint8_t *ptr, const struct mdns_entry *entry)
{
    uint32_t len = sizeof(entry->data.A.addr);
    memcpy(ptr, &entry->data.A.addr, sizeof(entry->data.A.addr));
    return len;
}

static void mdns_print_A(const union mdns_data *data)
{
    log_d("{\"address\":\"%s\"}", data->A.addr_str);
}

/*
 * Decodes a DN compressed format (RFC 1035)
 * e.g "\x03foo\x03bar\x00" gives "foo.bar"
 */
static const uint8_t *mdns_decode(const uint8_t *ptr, uint32_t *n, const uint8_t *root, char **ss)
{
    char *s;

    s = *ss = malloc(MDNS_DN_MAXSZ);
    if (!s) {
        return (NULL);
    }

    if (*ptr == 0) {
        *s = '\0';
        advance(1);
        log_e("mdns_decode *prt = 0");
        return (ptr);
    }
    while (*ptr) {
        uint32_t free_space;
        uint16_t len;

        free_space = *ss + MDNS_DN_MAXSZ - s;
        len = *ptr;
        advance(1);

        /* resolve the offset of the pointer (RFC 1035-4.1.4) */
        if ((len & 0xC0) == 0xC0) {
            const uint8_t *p;
            char *buf;
            uint32_t m;

            if (*n < sizeof(len)) {
                log_e("---*n = %d", *n);
                goto err;
            }
            len &= ~0xC0;
            len = (len << 8) | *ptr;
            advance(1);

            p = root + len;
            m = ptr - p + *n;
            mdns_decode(p, &m, root, &buf);
            if (free_space <= strlen(buf)) {
                free(buf);
                log_e("free_space <= strlen(buf)");
                goto err;
            }
            (void) strcpy(s, buf);
            free(buf);
            return (ptr);
        }
        if (*n <= len || free_space <= len) {
            log_e("*n = %d,len = %d,free_space =%d", *n, len, free_space);
            goto err;
        }
        strncpy(s, (const char *) ptr, len);
        advance(len);
        s += len;
        *s++ = (*ptr) ? '.' : '\0';
    }
    advance(1);
    return (ptr);
err:
    free(*ss);
    return (NULL);
}

/*
 * Encode a DN into its compressed format (RFC 1035)
 * e.g "foo.bar" gives "\x03foo\x03bar\x00"
 */
static uint8_t *mdns_encode(char *s)
{
    uint8_t *buf, *b, l = 0;
    char *p = s;

    buf = malloc(strlen(s) + 2);
    if (!buf) {
        return (NULL);
    }
    for (b = buf, l = strcspn(p, "."); l > 0;
         l = *p ? strcspn(++p, ".") : 0) {
        *b = l;
        memcpy(b + 1, p, l);
        b += l + 1;
        p += l;
    }
    *b = 0;
    return (buf);
}

static const uint8_t *mdns_read_RR(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry,
                                   int8_t ans)
{
    uint16_t tmp;

    ptr = mdns_decode(ptr, n, root, &entry->name);
    if (!ptr || *n < 4) {
        return (NULL);
    }

    ptr = read_u16(ptr, n, &entry->type);

    log_d("mdns_read_RR type = %d", entry->type);
    ptr = read_u16(ptr, n, &tmp);
    entry->class = (tmp & ~0x8000);
    entry->msbit = ((tmp & 0x8000) == 0x8000);
    if (ans) {
        if (*n < 6) {
            return (NULL);
        }
        ptr = read_u32(ptr, n, &entry->ttl);
        ptr = read_u16(ptr, n, &entry->data_len);
    }
    return ptr;
}

static uint32_t mdns_write_RR(uint8_t *ptr, const struct mdns_entry *entry, int8_t ans)
{
    uint8_t *name, *p = ptr;

    if ((name = mdns_encode(entry->name)) == NULL) {
        return (0);
    }

    p = write_raw(p, name);
    p = write_u16(p, entry->type);
    p = write_u16(p, (entry->class & ~0x8000) | (entry->msbit << 15));

    if (ans) {
        p = write_u32(p, entry->ttl);
        p = write_u16(p, entry->data_len);
    }
    free(name);
    return (p - ptr);
}

const uint8_t *mdns_read(const uint8_t *ptr, uint32_t *n, const uint8_t *root, struct mdns_entry *entry, int8_t ans)
{
    uint32_t skip;
    const uint8_t *p;

    p = ptr = mdns_read_RR(ptr, n, root, entry, ans);
    if (ans == 0) {
        return ptr;
    }

    for (uint32_t i = 0; i < mdns_num; ++i) {
        if (rrs[i].type == entry->type) {
            ptr = (*rrs[i].read)(ptr, n, root, entry);
            if (!ptr) {
                return (NULL);
            }
            break;
        }
    }
    // XXX skip unknown records
    skip = entry->data_len - (ptr - p);
    if (*n < skip) {
        return (NULL);
    }
    advance(skip);

    return (ptr);
}

uint32_t mdns_write(uint8_t *ptr, const struct mdns_entry *entry, int8_t ans)
{
    uint8_t *p = ptr;
    uint32_t n = 0;
    uint16_t l = 0;

    l = mdns_write_RR(p, entry, ans);
    n += l;

    if (ans == 0) {
        return n;
    }

    for (uint32_t i = 0; i < mdns_num; ++i) {
        if (rrs[i].type == entry->type) {
            l = (*rrs[i].write)(p + n, entry);
            // fill in data length after its computed
            write_u16(p + n - 2, l);
            n += l;
        }
    }
    return n;
}

static const char *mdns_type_str(enum mdns_type type)
{
    for (uint32_t i = 0; i < mdns_num; ++i) {
        if (rrs[i].type == type) {
            return (rrs[i].name);
        }
    }
    return ("UNKNOWN");
}

static const char *mdns_class_str(enum mdns_class mdns_class)
{
    if (mdns_class == RR_IN) {
        return ("IN");
    }
    return ("UNKNOWN");
}

void mdns_item_print(const struct mdns_entry *entry)
{
    uint32_t i;

    log_d("{\"name\":\"%s\","
          "\"type\":\"%s\","
          "\"class\":\"%s\","
          "\"data\":",
          entry->name == NULL ? "NULL" : entry->name, mdns_type_str(entry->type), mdns_class_str(entry->class));

    for (i = 0; i < mdns_num; ++i) {
        if (rrs[i].type == entry->type) {
            (*rrs[i].print)(&entry->data);
            break;
        }
    }
    if (i == mdns_num) {
        log_d("null");
    }

    log_d("}");
}

void mdns_free(struct mdns_entry *entry)
{
    if (!entry) {
        return;
    }

    switch (entry->type) {
        case RR_SRV:
            if (entry->data.SRV.target) {
                free(entry->data.SRV.target);
            }
            break;
        case RR_PTR:
            if (entry->data.PTR.domain) {
                free(entry->data.PTR.domain);
            }
            break;
        case RR_TXT: {
            struct mdns_data_txt *text, *TXT;

            TXT = entry->data.TXT;
            while ((text = TXT)) {
                TXT = TXT->next;
                if (text) {
                    free(text);
                }
            }
        }
    }
    if (entry->name) {
        free(entry->name);
    }
}

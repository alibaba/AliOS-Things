// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/*
 * config stroge format:
 *  key                                      value
 *  filename                            [section_name1]\n[section_name2]\n[section_name3]\n···
 *  section_name                        [entry_key1]\n[entry_key2]\n[entry_key3]\n···
 *  section_name-entry_key              entry_value
 */

#define LOG_TAG "bt_osi_config"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bt_common.h"
#include "osi/allocator.h"
#include "osi/config.h"
#include "osi/list.h"

#include <aos/kv.h>

#define CONFIG_FILE_MAX_SIZE             (1536)//1.5k
#define CONFIG_FILE_DEFAULE_LENGTH       (2048)
#define CONFIG_KEY                       "bt_cfg_key"
typedef struct {
    char *key;
    char *value;
} entry_t;

typedef struct {
    char *name;
    list_t *entries;
} section_t;

struct config_t {
    list_t *sections;
};

// Empty definition; this type is aliased to list_node_t.
struct config_section_iter_t {};

static void config_parse(config_t *config, const char *filename);

static section_t *section_new(const char *name);
static void section_free(void *ptr);
static section_t *section_find(const config_t *config, const char *section);

static entry_t *entry_new(const char *key, const char *value);
static void entry_free(void *ptr);
static entry_t *entry_find(const config_t *config, const char *section, const char *key);

config_t *config_new_empty(void)
{
    config_t *config = osi_calloc(sizeof(config_t));
    if (!config) {
        OSI_TRACE_ERROR("%s unable to allocate memory for config_t.\n", __func__);
        goto error;
    }

    config->sections = osi_list_new(section_free);
    if (!config->sections) {
        OSI_TRACE_ERROR("%s unable to allocate list for sections.\n", __func__);
        goto error;
    }

    return config;

error:;
    config_free(config);
    return NULL;
}

config_t *config_new(const char *filename)
{
    assert(filename != NULL);

    config_t *config = config_new_empty();
    if (!config) {
        return NULL;
    }

    config_parse(config, filename);

    return config;
}

void config_free(config_t *config)
{
    if (!config) {
        return;
    }

    osi_list_free(config->sections);
    osi_free(config);
}

bool config_has_section(const config_t *config, const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    return (section_find(config, section) != NULL);
}

bool config_has_key(const config_t *config, const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    return (entry_find(config, section, key) != NULL);
}

bool config_has_key_in_section(config_t *config, const char *key, char *key_value)
{
    OSI_TRACE_DEBUG("key = %s, value = %s", key, key_value);
    for (const list_node_t *node = osi_list_begin(config->sections); node != osi_list_end(config->sections); node = osi_list_next(node)) {
        const section_t *section = (const section_t *)osi_list_node(node);

        for (const list_node_t *node = osi_list_begin(section->entries); node != osi_list_end(section->entries); node = osi_list_next(node)) {
            entry_t *entry = osi_list_node(node);
            OSI_TRACE_DEBUG("entry->key = %s, entry->value = %s", entry->key, entry->value);
            if (!strcmp(entry->key, key) && !strcmp(entry->value, key_value)) {
                OSI_TRACE_DEBUG("%s, the irk aready in the flash.", __func__);
                return true;
            }
        }
    }

    return false;
}

int config_get_int(const config_t *config, const char *section, const char *key, int def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    char *endptr;
    int ret = strtol(entry->value, &endptr, 0);
    return (*endptr == '\0') ? ret : def_value;
}

bool config_get_bool(const config_t *config, const char *section, const char *key, bool def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    if (!strcmp(entry->value, "true")) {
        return true;
    }
    if (!strcmp(entry->value, "false")) {
        return false;
    }

    return def_value;
}

const char *config_get_string(const config_t *config, const char *section, const char *key, const char *def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    return entry->value;
}

void config_set_int(config_t *config, const char *section, const char *key, int value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    char value_str[32] = { 0 };
    sprintf(value_str, "%d", value);
    config_set_string(config, section, key, value_str, false);
}

void config_set_bool(config_t *config, const char *section, const char *key, bool value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    config_set_string(config, section, key, value ? "true" : "false", false);
}

void config_set_string(config_t *config, const char *section, const char *key, const char *value, bool insert_back)
{
    section_t *sec = section_find(config, section);
    if (!sec) {
        sec = section_new(section);
        if (insert_back) {
            osi_list_append(config->sections, sec);
        } else {
            osi_list_prepend(config->sections, sec);
        }
    }

    for (const list_node_t *node = osi_list_begin(sec->entries); node != osi_list_end(sec->entries); node = osi_list_next(node)) {
        entry_t *entry = osi_list_node(node);
        if (!strcmp(entry->key, key)) {
            osi_free(entry->value);
            entry->value = osi_strdup(value);
            return;
        }
    }

    entry_t *entry = entry_new(key, value);
    osi_list_append(sec->entries, entry);
}

bool config_remove_section(config_t *config, const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    section_t *sec = section_find(config, section);
    if (!sec) {
        return false;
    }

    return osi_list_remove(config->sections, sec);
}

bool config_remove_key(config_t *config, const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    section_t *sec = section_find(config, section);
    entry_t *entry = entry_find(config, section, key);
    if (!sec || !entry) {
        return false;
    }

    return osi_list_remove(sec->entries, entry);
}

const config_section_node_t *config_section_begin(const config_t *config)
{
    assert(config != NULL);
    return (const config_section_node_t *)osi_list_begin(config->sections);
}

const config_section_node_t *config_section_end(const config_t *config)
{
    assert(config != NULL);
    return (const config_section_node_t *)osi_list_end(config->sections);
}

const config_section_node_t *config_section_next(const config_section_node_t *node)
{
    assert(node != NULL);
    return (const config_section_node_t *)osi_list_next((const list_node_t *)node);
}

const char *config_section_name(const config_section_node_t *node)
{
    assert(node != NULL);
    const list_node_t *lnode = (const list_node_t *)node;
    const section_t *section = (const section_t *)osi_list_node(lnode);
    return section->name;
}

#if 0
static int get_config_size(const config_t *config)
{
    assert(config != NULL);

    int w_len = 0, total_size = 0;

    for (const list_node_t *node = osi_list_begin(config->sections); node != osi_list_end(config->sections); node = osi_list_next(node)) {
        const section_t *section = (const section_t *)osi_list_node(node);
        w_len = strlen(section->name) + strlen("[]\n");// format "[section->name]\n"
        total_size += w_len;

        for (const list_node_t *enode = osi_list_begin(section->entries); enode != osi_list_end(section->entries); enode = osi_list_next(enode)) {
            const entry_t *entry = (const entry_t *)osi_list_node(enode);
            w_len = strlen(entry->key) + strlen(entry->value) + strlen(" = \n");// format "entry->key = entry->value\n"
            total_size += w_len;
        }

        // Only add a separating newline if there are more sections.
        if (osi_list_next(node) != osi_list_end(config->sections)) {
                total_size ++;  //'\n'
        } else {
            break;
        }
    }
    total_size ++; //'\0'
    return total_size;
}


static int get_config_size_from_flash(nvs_handle_t fp)
{
    assert(fp != 0);

    bt_err_t err;
    const size_t keyname_bufsz = sizeof(CONFIG_KEY) + 5 + 1; // including log10(sizeof(i))
    char *keyname = osi_calloc(keyname_bufsz);
    if (!keyname){
        OSI_TRACE_ERROR("%s, malloc error\n", __func__);
        return 0;
    }
    size_t length = CONFIG_FILE_DEFAULE_LENGTH;
    size_t total_length = 0;
    uint16_t i = 0;
    snprintf(keyname, keyname_bufsz, "%s%d", CONFIG_KEY, 0);
    err = nvs_get_blob(fp, keyname, NULL, &length);
    if (err == YOC_ERR_NVS_NOT_FOUND) {
        osi_free(keyname);
        return 0;
    }
    if (err != BT_OK) {
        OSI_TRACE_ERROR("%s, error %d\n", __func__, err);
        osi_free(keyname);
        return 0;
    }
    total_length += length;
    while (length == CONFIG_FILE_MAX_SIZE) {
        length = CONFIG_FILE_DEFAULE_LENGTH;
        snprintf(keyname, keyname_bufsz, "%s%d", CONFIG_KEY, ++i);
        err = nvs_get_blob(fp, keyname, NULL, &length);

        if (err == YOC_ERR_NVS_NOT_FOUND) {
            break;
        }
        if (err != BT_OK) {
            OSI_TRACE_ERROR("%s, error %d\n", __func__, err);
            osi_free(keyname);
            return 0;
        }
        total_length += length;
    }
    osi_free(keyname);
    return total_length;
}
#endif

#if 1
#define SECNAMEMARK      "[SecName] = "
#define KEYMARK          "[Key] = "
#define VALMARK          "[Val] = "

bool config_save(const config_t *config, const char *filename)
{    
    assert(config != NULL);
    assert(filename != NULL);
    assert(*filename != '\0');

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        OSI_TRACE_ERROR("%s: fopen faield %s", __func__, filename);
        return false;
    }
    for (const list_node_t *node = osi_list_begin(config->sections); node != osi_list_end(config->sections); node = osi_list_next(node)) {
        const section_t *section = (const section_t *)osi_list_node(node);

        fprintf(fp, SECNAMEMARK"%s\n", section->name);
        OSI_TRACE_WARNING(SECNAMEMARK"%s\n", section->name);

        for (const list_node_t *enode = osi_list_begin(section->entries); enode != osi_list_end(section->entries); enode = osi_list_next(enode)) {
            const entry_t *entry = (const entry_t *)osi_list_node(enode);

            OSI_TRACE_WARNING(KEYMARK"%s\n"VALMARK"%s\n", entry->key, entry->value);
            fprintf(fp, KEYMARK"%s\n"VALMARK"%s\n", entry->key, entry->value);
        }
    }

    fclose(fp);

#if 1
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        OSI_TRACE_ERROR("%s: check file faield %s", __func__, filename);
        return true;
    }

    fseek(fp, 0, SEEK_END);
    OSI_TRACE_WARNING("file length = %d", ftell(fp));

    fclose(fp);
#endif
    return true;
}

#else
bool config_save(const config_t *config, const char *filename)
{
    assert(config != NULL);
    assert(filename != NULL);
    assert(*filename != '\0');

    int err_code = 0;
    char *line = osi_calloc(1024);

    // int config_size = get_config_size(config);
    char *sec_name = osi_calloc(1024);
    char *entry_name = osi_calloc(1024);
    char *entry_key = osi_calloc(1024);

    if (!line || !sec_name || !entry_name || !entry_key) {
        err_code |= 0x01;
        goto error;
    }

    int w_cnt, w_cnt_total = 0, entry_cnt_total = 0;

    for (const list_node_t *node = osi_list_begin(config->sections); node != osi_list_end(config->sections); node = osi_list_next(node)) {
        const section_t *section = (const section_t *)osi_list_node(node);
        w_cnt = snprintf(line, 1024, "[%s]\n", section->name);
        OSI_TRACE_DEBUG("section name: %s, w_cnt + w_cnt_total = %d\n", section->name, w_cnt + w_cnt_total);
        memcpy(sec_name + w_cnt_total, line, w_cnt + 1);
        w_cnt_total += w_cnt;

        entry_cnt_total = 0;

        for (const list_node_t *enode = osi_list_begin(section->entries); enode != osi_list_end(section->entries); enode = osi_list_next(enode)) {
            const entry_t *entry = (const entry_t *)osi_list_node(enode);
            OSI_TRACE_DEBUG("(key, val): (%s, %s)\n", entry->key, entry->value);
            w_cnt = snprintf(line, 1024, "[%s]\n", entry->key);
            memcpy(entry_name + entry_cnt_total, line, w_cnt + 1);
            entry_cnt_total += w_cnt;

            snprintf(entry_key, 1024, "%s-%s", section->name, entry->key);

            if (aos_kv_set(entry_key, entry->value, strlen(entry->value), 1)) {
                goto error;
            }
        }

        if (aos_kv_set(section->name, entry_name, strlen(entry_name), 1)) {
            goto error;
        }
    }

    if (aos_kv_set(filename, sec_name, strlen(sec_name), 1)) {
        goto error;
    }

    // printf("%s, %s", __func__, sec_name);
    osi_free(entry_key);
    osi_free(line);
    osi_free(sec_name);
    osi_free(entry_name);
    return true;

error:
    if (sec_name) {
        osi_free(sec_name);
    }

    if (line) {
        osi_free(line);
    }

    if (entry_name) {
        osi_free(entry_name);
    }

    if (entry_key) {
        osi_free(entry_key);
    }

    if (err_code) {
        OSI_TRACE_ERROR("%s, err_code: 0x%x\n", __func__, err_code);
    }
    return false;
}
#endif
#if 0
static char *trim(char *str)
{
    while (isspace((unsigned char)(*str))) {
        ++str;
    }

    if (!*str) {
        return str;
    }

    char *end_str = str + strlen(str) - 1;
    while (end_str > str && isspace((unsigned char)(*end_str))) {
        --end_str;
    }

    end_str[1] = '\0';
    return str;
}
#endif

#if 1
#define MAX_LINE_SIZE   1024
static void config_parse(config_t *config, const char *filename)
{
    assert(config != NULL);
    assert(filename != NULL);
    assert(*filename != '\0');
    char *line, *section, *key, *val, *ptr;
    bool section_exist, key_exist, val_exist;
    int32_t err_code = 0;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        OSI_TRACE_ERROR("%s: fopen failed %s", __func__, filename);
        return;
    }

    fseek(fp, 0, SEEK_END);
    OSI_TRACE_WARNING("file length = %d", ftell(fp));
    fseek(fp, 0, SEEK_SET);

    line = osi_malloc(MAX_LINE_SIZE);
    section = osi_malloc(MAX_LINE_SIZE);
    key = osi_malloc(MAX_LINE_SIZE);
    val = osi_malloc(MAX_LINE_SIZE);
    if (!line || !section || !key || !val) {
        err_code |= 0x01;
        goto error;
    }

    section_exist = 0;
    key_exist = 0;
    val_exist = 0;
    while(fgets(line, MAX_LINE_SIZE, fp)) {
        if ((ptr = strstr(line, SECNAMEMARK)) != 0) {
            ptr += strlen(SECNAMEMARK);
            strncpy(section, ptr, MAX_LINE_SIZE);
            if ((strlen(section) > 0) && (section[strlen(section) - 1] == '\n')) {
                section[strlen(section) - 1] = 0;
            }
            section_exist = 1;
        } else if ((ptr = strstr(line, KEYMARK)) != 0) {
            ptr += strlen(KEYMARK);
            strncpy(key, ptr, MAX_LINE_SIZE);
            if ((strlen(key) > 0) && (key[strlen(key) - 1] == '\n')) {
                key[strlen(key) - 1] = 0;
            }
            key_exist = 1;
        } else if ((ptr = strstr(line, VALMARK)) != 0) {
            ptr += strlen(VALMARK);
            strncpy(val, ptr, MAX_LINE_SIZE);
            if ((strlen(val) > 0) && (val[strlen(val) - 1] == '\n')) {
                val[strlen(val) - 1] = 0;
            }
            val_exist = 1;
        }
        if (section_exist&&key_exist&&val_exist) {
            OSI_TRACE_WARNING("%s: section = %s, key = %s, val = %s", __func__, section, key, val);
            config_set_string(config, section, key, val, 1);
            key_exist = 0;
            val_exist = 0;
        }
    }

error:
    if (err_code) {
        OSI_TRACE_ERROR("%s, err code = %d", __func__, err_code);
    }
    if (line)
        osi_free(line);
    if (section)
        osi_free(section);
    if (key)
        osi_free(key);
    if (val)
        osi_free(val);
    
    fclose(fp);
    return;
}
#else
static void config_parse(config_t *config, const char *filename)
{
    assert(config != NULL);

    int err_code = 0;

    char *line = osi_calloc(1024);
    char *section = osi_calloc(1024);
    char *entry = osi_calloc(1024);
    char *entry_value = osi_calloc(1024);
    char *sec_names = osi_calloc(1024);
    char *entry_names = osi_calloc(1024);

    if (!line || !section || !sec_names || !entry_names || !entry || !entry_value) {
        err_code |= 0x01;
        goto error;
    }

    int len = 1024;

    if (aos_kv_get(filename, sec_names, &len) < 0) {
        goto error;
    }

    char *p_bgn = sec_names, *p_end;

    while (strlen(p_bgn)) {

        if (*p_bgn == '[') {
            p_end = strchr(p_bgn, '\n');

            if (p_end == NULL) {
                goto error;
            }

            if (*(p_end - 1) != ']') {
                p_bgn = p_end + 1;
                OSI_TRACE_WARNING("%s unterminated section name on\n", __func__);
                continue;
            }

            memset(section, 0, 1024);

            strncpy(section, p_bgn + 1, p_end - p_bgn - 2);
            int len = 1024;

            if (aos_kv_get(section, entry_names, &len) < 0) {
                goto error;
            }

            char *e_bgn = entry_names, *e_end;

            while (strlen(e_bgn)) {
                if (*e_bgn == '[') {
                    e_end = strchr(e_bgn, '\n');

                    if (e_end == NULL) {
                        goto error;
                    }

                    if (*(e_end - 1) != ']') {
                        e_bgn = e_end + 1;
                        OSI_TRACE_WARNING("%s unterminated section name\n", __func__);
                        continue;
                    }

                    *(e_end - 1) = 0;

                    snprintf(entry, 1024, "%s-%s", section, e_bgn + 1);
                    int len = 1024;

                    if (aos_kv_get(entry, entry_value, &len) < 0) {
                        goto error;
                    }

                    config_set_string(config, section, e_bgn + 1, entry_value, 1);
                } else {
                    goto error;
                }

                e_bgn = e_end + 1;
            }

            p_bgn = p_end + 1;

        } else {
            goto error;
        }
    }

error:
    if (sec_names) {
        osi_free(sec_names);
    }

    if (entry) {
        osi_free(entry);
    }

    if (entry_names) {
        osi_free(entry_names);
    }

    if (line) {
        osi_free(line);
    }

    if (section) {
        osi_free(section);
    }

    if (entry_value) {
        osi_free(entry_value);
    }

    if (err_code) {
        OSI_TRACE_ERROR("%s returned with err code: %d\n", __func__, err_code);
    }
}
#endif

static section_t *section_new(const char *name)
{
    section_t *section = osi_calloc(sizeof(section_t));
    if (!section) {
        return NULL;
    }

    section->name = osi_strdup(name);
    section->entries = osi_list_new(entry_free);
    return section;
}

static void section_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    section_t *section = ptr;
    osi_free(section->name);
    osi_list_free(section->entries);
    osi_free(section);
}

static section_t *section_find(const config_t *config, const char *section)
{
    for (const list_node_t *node = osi_list_begin(config->sections); node != osi_list_end(config->sections); node = osi_list_next(node)) {
        section_t *sec = osi_list_node(node);
        if (!strcmp(sec->name, section)) {
            return sec;
        }
    }

    return NULL;
}

static entry_t *entry_new(const char *key, const char *value)
{
    entry_t *entry = osi_calloc(sizeof(entry_t));
    if (!entry) {
        return NULL;
    }

    entry->key = osi_strdup(key);
    entry->value = osi_strdup(value);
    return entry;
}

static void entry_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    entry_t *entry = ptr;
    osi_free(entry->key);
    osi_free(entry->value);
    osi_free(entry);
}

static entry_t *entry_find(const config_t *config, const char *section, const char *key)
{
    section_t *sec = section_find(config, section);
    if (!sec) {
        return NULL;
    }

    for (const list_node_t *node = osi_list_begin(sec->entries); node != osi_list_end(sec->entries); node = osi_list_next(node)) {
        entry_t *entry = osi_list_node(node);
        if (!strcmp(entry->key, key)) {
            return entry;
        }
    }

    return NULL;
}

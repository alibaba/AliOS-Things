/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __OSI_CONFIG_H__
#define __OSI_CONFIG_H__

// This module implements a configuration parser. Clients can query the
// contents of a configuration file through the interface provided here.
// The current implementation is read-only; mutations are only kept in
// memory. This parser supports the INI file format.

// Implementation notes:
// - Key/value pairs that are not within a section are assumed to be under
//   the |CONFIG_DEFAULT_SECTION| section.
// - Multiple sections with the same name will be merged as if they were in
//   a single section.
// - Empty sections with no key/value pairs will be treated as if they do
//   not exist. In other words, |nvrec_config_has_section| will return false for
//   empty sections.
// - Duplicate keys in a section will overwrite previous values.

#include <stdbool.h>
#include "list_ext.h"
// The default section name to use if a key/value pair is not defined within
// a section.
#define CONFIG_DEFAULT_SECTION "Global"
typedef struct {
  char *key;
  char *value;
} nvrec_entry_t;

typedef struct {
  char *name;
  list_t *entries;
} nvrec_section_t;

typedef struct nvrec_config {
  list_t *sections;
}nvrec_config_t;

// Loads the specified file and returns a handle to the config file. If there
// was a problem loading the file or allocating memory, this function returns
// NULL. Clients must call |nvrec_config_free| on the returned handle when it is no
// longer required. |filename| must not be NULL and must point to a readable
// file on the filesystem.
nvrec_config_t *nvrec_config_new(const char *filename);

// Frees resources associated with the config file. No further operations may
// be performed on the |config| object after calling this function. |config|
// may be NULL.
void nvrec_config_free(nvrec_config_t *config);

// Returns true if the config file contains a section named |section|. If
// the section has no key/value pairs in it, this function will return false.
// |config| and |section| must not be NULL.
bool nvrec_config_has_section(const nvrec_config_t *config, const char *section);

// Returns true if the config file has a key named |key| under |section|.
// Returns false otherwise. |config|, |section|, and |key| must not be NULL.
bool nvrec_config_has_key(const nvrec_config_t *config, const char *section, const char *key);

// Returns the integral value for a given |key| in |section|. If |section|
// or |key| do not exist, or the value cannot be fully converted to an integer,
// this function returns |def_value|. |config|, |section|, and |key| must not
// be NULL.
int nvrec_config_get_int(const nvrec_config_t *config, const char *section, const char *key, int def_value);

// Returns the boolean value for a given |key| in |section|. If |section|
// or |key| do not exist, or the value cannot be converted to a boolean, this
// function returns |def_value|. |config|, |section|, and |key| must not be NULL.
bool nvrec_config_get_bool(const nvrec_config_t *config, const char *section, const char *key, bool def_value);

// Returns the string value for a given |key| in |section|. If |section| or
// |key| do not exist, this function returns |def_value|. The returned string
// is owned by the config module and must not be freed. |config|, |section|,
// and |key| must not be NULL. |def_value| may be NULL.
const char *nvrec_config_get_string(const nvrec_config_t *config, const char *section, const char *key, const char *def_value);

// Sets an integral value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, and |key|
// must not be NULL.
void nvrec_config_set_int(nvrec_config_t *config, const char *section, const char *key, int value);

// Sets a boolean value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, and |key|
// must not be NULL.
void nvrec_config_set_bool(nvrec_config_t *config, const char *section, const char *key, bool value);

// Sets a string value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, |key|, and
// |value| must not be NULL.
void nvrec_config_set_string(nvrec_config_t *config, const char *section, const char *key, const char *value);
nvrec_section_t *nvrec_section_find(const nvrec_config_t *config, const char *section);
void nvrec_entry_free(void *ptr);
#endif


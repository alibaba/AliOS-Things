#!/usr/bin/env bash
#
# Script used in CI to build all configurations of mkspiffs
#

set -e

# Generic (default options)
make dist

# Arduino ESP8266
make clean
make dist BUILD_CONFIG_NAME="-arduino-esp8266" \
    CPPFLAGS="-DSPIFFS_USE_MAGIC_LENGTH=0 -DSPIFFS_ALIGNED_OBJECT_INDEX_TABLES=1"

# Build configuration for arduino-esp32
make clean
make dist BUILD_CONFIG_NAME="-arduino-esp32" \
    CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"

# Build configuration for ESP-IDF (esp32)
make clean
make dist BUILD_CONFIG_NAME="-esp-idf" \
    CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"




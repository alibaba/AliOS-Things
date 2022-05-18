#!/usr/bin/env bash

# 1. get shelldir path
shelldir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )

# 2. copy yaml file
cp -f ${shelldir}/../../chip/espressif_esp32/package-esp32.yaml ${shelldir}/../../chip/espressif_esp32/package.yaml

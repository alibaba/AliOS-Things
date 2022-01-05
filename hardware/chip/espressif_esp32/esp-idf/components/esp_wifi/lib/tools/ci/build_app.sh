#!/bin/bash
# This file is sourced in to the CI environment
# in .gitlab-ci.yml file
#

set -x

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${IDF_PATH} ] && die "IDF_PATH is not set."

# Tool chain
export PATH=/wifi_ci/xtensa-esp32-elf/bin:${PATH}

# Sigma DUT
echo Running make for espsigma dut app...
cd ${CI_PROJECT_DIR}/espsigma/esp_sigma_dut
make ${MAKEFLAGS} 

# Copying binaries to create build tar ball
cd $CI_PROJECT_DIR
rm -rf ${board}-${CI_PIPELINE_ID}.debug
mkdir -p ${board}-${CI_PIPELINE_ID}.debug/

echo Creating ${board}-${VERSION} app binaries debug directory...
cp ${CI_PROJECT_DIR}/espsigma/esp_sigma_dut/build/*.bin $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }
cp ${CI_PROJECT_DIR}/espsigma/esp_sigma_dut/build/*.elf $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }
cp ${CI_PROJECT_DIR}/espsigma/esp_sigma_dut/build/bootloader/bootloader.bin $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }

# Creating tar file (debug)
tar -zcvf $board-${CI_PIPELINE_ID}.debug.tar.gz $board-${CI_PIPELINE_ID}.debug || { echo "Create tar Status: Failure"; exit 1; }

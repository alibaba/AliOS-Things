#!/bin/bash
# This file is sourced in to the CI environment
# in .gitlab-ci.yml file

set -x

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${IDF_PATH} ] && die "IDF_PATH is not set."

# Set Env
cd ${CI_PROJECT_DIR}/esp-idf
./install.sh || { echo "Install Failure"; exit 1; }
. ./export.sh || { echo "Export Failure"; exit 1; }

# Set Env
cd ${CI_PROJECT_DIR}/SSC

# SSC Dut
echo Running cmake for ssc app...
if test -f "gen_misc_ng.sh"; then
    ./gen_misc_ng.sh SSC
fi

if test -f "gen_misc_idf.sh"; then
    ./gen_misc_idf.sh ESP32 SSC
fi

# Copying binaries to create build tar ball
cd $CI_PROJECT_DIR
rm -rf ${board}-${CI_PIPELINE_ID}.debug
mkdir -p ${board}-${CI_PIPELINE_ID}.debug/

echo Creating ${board}-${VERSION} app binaries debug directory...
cp ${CI_PROJECT_DIR}/SSC/build/ssc.bin $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }
cp ${CI_PROJECT_DIR}/SSC/build/ssc.elf $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }
cp ${CI_PROJECT_DIR}/SSC/build/bootloader/bootloader.bin $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }
cp ${CI_PROJECT_DIR}/SSC/build/partition_table/partition-table.bin $board-${CI_PIPELINE_ID}.debug/ || { echo "Copy file Status: Failure"; exit 1; }

# Creating tar file (debug)
tar -zcvf $board-${CI_PIPELINE_ID}.debug.tar.gz $board-${CI_PIPELINE_ID}.debug || { echo "Create tar Status: Failure"; exit 1; }

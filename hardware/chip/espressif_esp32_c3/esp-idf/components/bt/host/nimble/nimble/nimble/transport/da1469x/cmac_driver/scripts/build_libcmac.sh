#!/bin/bash

# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

if [ ! -f ${MYNEWT_VAL_CMAC_IMAGE_FILE_NAME} ]; then
    >&2 echo ERROR: BLE stack library not found. Please check nimble/transport/da1469x/README for details.
    exit 1
fi

OBJCOPY=${MYNEWT_OBJCOPY_PATH}
AR=${MYNEWT_AR_PATH}
LIBBLE_A=$(readlink -e ${MYNEWT_VAL_CMAC_IMAGE_FILE_NAME})
LIBCMAC_A=${MYNEWT_USER_SRC_DIR}/libcmac.a

BASENAME_ROM=cmac.rom
BASENAME_RAM=cmac.ram

cd ${MYNEWT_USER_WORK_DIR}

# Extract firmware binary from .a since we do not need to link all other
# objects with our image.
${AR} x ${LIBBLE_A} cmac_fw.c.obj
${OBJCOPY} -O binary --only-section=.cmi_fw_area cmac_fw.c.obj cmac.bin

# We need separate copies for RAM and ROM image since section names are derived
# from file names. For now files are the same, but it would be possible to
# link ROM image without data and shared sections which contains zeroes anyway.
cp cmac.bin ${BASENAME_ROM}.bin
cp cmac.bin ${BASENAME_RAM}.bin

# Convert both binaries to objects and create archive to link
${OBJCOPY} -I binary -O elf32-littlearm -B armv8-m.main \
    --rename-section .data=.libcmac.rom ${BASENAME_ROM}.bin ${BASENAME_ROM}.o
${OBJCOPY} -I binary -O elf32-littlearm -B armv8-m.main \
    --rename-section .data=.libcmac.ram ${BASENAME_RAM}.bin ${BASENAME_RAM}.o
${AR} -rcs ${LIBCMAC_A} ${BASENAME_ROM}.o ${BASENAME_RAM}.o

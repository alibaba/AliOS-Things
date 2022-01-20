# CMake fragment for MicroPython external component

set(MICROPY_EXTERNAL_DIR "${MICROPY_DIR}/../external")

set(MICROPY_INC_EXTERNAL
    ${MICROPY_EXTERNAL_DIR}/amp_boot
    ${MICROPY_EXTERNAL_DIR}/amp_task
    ${MICROPY_EXTERNAL_DIR}/amp_utils
    ${MICROPY_EXTERNAL_DIR}/app_mgr
    ${MICROPY_EXTERNAL_DIR}/common
)

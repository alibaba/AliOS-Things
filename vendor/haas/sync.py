#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    ['delete', 'mcu/drivers/services/multimedia'],
    ['delete', 'mcu/drivers/rtos/rhino/Makefile'],
    ['delete', 'mcu/drivers/rtos/rhino/agcts'],
    ['delete', 'mcu/drivers/rtos/rhino/arch'],
    ['delete', 'mcu/drivers/rtos/rhino/board.c'],
    ['delete', 'mcu/drivers/rtos/rhino/board.h'],
    ['delete', 'mcu/drivers/rtos/rhino/debug'],
    ['delete', 'mcu/drivers/rtos/rhino/gic'],
    ['delete', 'mcu/drivers/rtos/rhino/include'],
    ['delete', 'mcu/drivers/rtos/rhino/init'],
    ['delete', 'mcu/drivers/rtos/rhino/k_config.c'],
    ['delete', 'mcu/drivers/rtos/rhino/k_config.h'],
    ['delete', 'mcu/drivers/rtos/rhino/kernel'],
    ['delete', 'mcu/drivers/rtos/rhino/Makefile'],
    ['delete', 'mcu/drivers/rtos/rhino/smp'],
    ['delete', 'mcu/drivers/rtos/rhino/source'],
    ['delete', 'mcu/release/write_flash_gui/PyQt4'],
    ['delete', 'mcu/drivers/platform/drivers/codec/codec_tlv32aic32.h'],
    ['delete', 'mcu/drivers/utils/crc16/crc16.h'],
    ['delete', 'mcu/drivers/utils/libc'],
    ['delete', 'mcu/drivers/services/interconnection/umm_malloc'],
    ['delete', 'mcu/drivers/services/a7_dsp/common/decom/lzma_decode.h'],
    ['delete', 'mcu/aos/touch/input-event-codes.h'],
    ['delete', 'mcu/aos/lcd/test.h'],
    ['delete', 'mcu/aos/dsp/best2300wp_a7.lst.tar.gz'],
]

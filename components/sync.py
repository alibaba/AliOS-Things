#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    # deleted by Gu Yin
    ['delete', 'fs/fatfs/'],
    ['delete', 'fs/nfs/'],
    ['delete', 'fs/ramfs/'],
    ['delete', 'fs/uffs/'],
    ['delete', 'fs/yaffs2/'],
    ['delete', 'fs/littlefs/platform_conf/vexpressa9/'],
    # deleted by Zhang Tian
    ['delete', 'autotest/'],
    ['delete', 'gui/'],
    ['delete', 'network/bt_module/'],
    ['delete', 'network/httpdns/'],
    ['delete', 'network/ortp/'],
    ['delete', 'network/websocket/'],
    ['delete', 'oss/'],
    ['delete', 'prebuilts/'],
    ['delete', 'service/udisplay/'],
    ['delete', 'service/uinput/'],
    ['delete', 'service/upower/'],
    ['delete', 'sqlite'],
    ['delete', 'test/'],
    ['delete', 'tools/'],
    ['delete', 'utilities/json-c/'],
    ['delete', 'utilities/libev/'],
    ['delete', 'utilities/logProducer/'],
    ['delete', 'utilities/zlib/'],
    # deleted by Jin Liang
    ['delete', 'musllibc/'],
    # deleted by Chao Zhong
    ['delete', 'drivers/devicevfs/src/'],
    ['delete', 'drivers/audio/'],
    ['delete', 'drivers/backlight/'],
    ['delete', 'drivers/bus/'],
    ['delete', 'drivers/fbdev/'],
    ['delete', 'drivers/include/linux/'],
    ['delete', 'drivers/input'],
    ['delete', 'drivers/ladapter/'],
    ['delete', 'drivers/macro/'],
    ['delete', 'drivers/micro/'],
    ['delete', 'drivers/misc/'],
    ['delete', 'drivers/vfs_sample/'],
    ['delete', 'service/sound/'],
    ['delete', 'utilities/ffmpeg/'],
    ['delete', 'utilities/SDL/'],
    ['delete', 'utilities/tinyalsa/'],
    ['delete', 'network/curl/'],
    # deleted by Zhu Qing
    ['delete', 'drivers/irq/gic/'],
    ['delete', 'drivers/irq/nvic/'],
    ['delete', 'drivers/irq/irqdesc.c'],
]

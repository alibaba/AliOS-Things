#define if this module need to be synchronized
sync = True

#file operations
#['operation', 'source'[, 'destination']]
fileopts = [
    ['delete', 'LoRaModuleSDK/*.c'],
    ['delete', 'LoRaModuleSDK/main.c.example'],
    ['copy', 'publish/lib', 'lib'],
    ['copy', 'publish/aos.mk', 'aos.mk'],
]


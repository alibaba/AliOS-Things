#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    ['copy', 'include', 'include'],
    ['copy', 'libmbmaster.mk', 'mbmaster.mk']
]

#compile lib processings
#['lib compile command', 'OS', 'path to copy module.a to']
makelib = [
    ['aos make modbus_test@developerkit', 'Linux', 'lib/Cortex-M4/gcc/mbmaster.a']
]

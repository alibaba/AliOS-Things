#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    ['copy', 'publish/main.c', 'main.c'],
    ['copy', 'publish/aos.mk', 'aos.mk'],
    ['copy', 'publish/Config.in', 'Config.in'],
    ['copy', 'publish/ucube.py', 'ucube.py'],
]

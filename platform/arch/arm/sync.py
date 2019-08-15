#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    ['delete', 'armv6m-mk/common/svc_gcc.S'],
    ['delete', 'armv6m-mk/common/svc_iccarm.S'],
    ['delete', 'armv6m-mk/common/svc_armcc.S'],
    ['delete', 'armv6m-mk/common/svc_debug.c'],
    ['delete', 'armv6m-mk/common/k_proc.c'],
    ['copy', 'publish/armv6m-mk_aos.mk', 'armv6m-mk/aos.mk'],
    ['copy', 'publish/armv6m-mk_lib', 'armv6m-mk/lib'],
    ['delete', 'armv7m-mk/common/svc_gcc.S'],
    ['delete', 'armv7m-mk/common/svc_iccarm.S'],
    ['delete', 'armv7m-mk/common/svc_debug.c'],
    ['delete', 'armv7m-mk/common/k_proc.c'],
    ['copy', 'publish/armv7m-mk_aos.mk', 'armv7m-mk/aos.mk'],
    ['copy', 'publish/armv7m-mk_lib', 'armv7m-mk/lib'],
    ['delete', 'armv7a-mk/common/svc_gcc.S'],
    ['delete', 'armv7a-mk/common/svc_armcc.S'],
    ['delete', 'armv7a-mk/common/svc_debug.c'],
    ['delete', 'armv7a-mk/common/k_proc.c'],
    ['copy', 'publish/armv7a-mk_aos.mk', 'armv7a-mk/aos.mk'],
    ['copy', 'publish/armv7a-mk_lib', 'armv7a-mk/lib'],
]


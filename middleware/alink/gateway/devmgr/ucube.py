src = Split('''
        devmgr_alink.c
        devmgr_common.c
        devmgr.c
        devmgr_cache.c
        devmgr_router.c
''')

component = aos_component('devmgr', src)

src     = Split('''
        vfs.c
        vfs_file.c
        vfs_inode.c
        vfs_register.c
''')
component = aos_component('vfs', src)

if aos_global_config.ide != 'keil':
    component.add_comp_deps('kernel/fs/vfs/device')

component.add_global_includes('include')

component.add_global_macros('AOS_COMP_VFS')

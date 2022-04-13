#define if this module need to be synchronized
sync = True

#file copy processings
#['source_file', 'target directory to copy source_file to']
fileopts = [
    # deleted by Zhu Qing
    ['delete', 'arch/arm/armv5-mk/'],
    ['delete', 'arch/arm/armv7a/'],
    ['delete', 'arch/arm/armv7a-mk/'],
    ['delete', 'arch/arm/armv7m/'],
    ['delete', 'arch/arm/armv8m-mk/'],
    ['delete', 'core/mk/publish/'],
    ['delete', 'core/mk/syscall/usyscall/svc_asm/armv6m_svc_iar.s'],
    ['delete', 'core/mk/syscall/usyscall/svc_asm/armv7a_svc_armcc.S'],
    ['delete', 'core/mk/syscall/usyscall/svc_asm/armv7a_svc_iar.s'],
    ['delete', 'core/mk/syscall/usyscall/svc_asm/armv7m_svc_gcc.S'],
    ['delete', 'core/libc/'],
    ['delete', 'core/nand_kv/'],
    ['delete', 'core/nftl/sim/'],
    ['delete', 'core/mk/syscall/usyscall/svc_asm/armv7a_svc_gcc.S'],
    ['delete', 'core/mk/uspace_lib/libc/'],
    ['delete', 'include/network/'],
    ['delete', 'core/kv/nand_kv/sim/'],
    ['delete', 'include/cunit/'],
    # deleted by Cai Jun
    ['delete', 'core/mk/uspace_lib/aos/fs_rpc_server.c'],
    ['delete', 'core/mk/uspace_lib/aos/uspace_aos.c'],
    ['delete', 'core/mk/uspace_lib/aos/vfs_state.c'],
    ['delete', 'core/mk/uspace_lib/aos/aos_socket.c'],
    ['delete', 'core/mk/uspace_lib/aos/fs_rpc_task.c'],
    ['delete', 'core/mk/uspace_lib/aos/kv.c'],
    ['delete', 'core/mk/uspace_lib/aos/vfs.c'],
    ['delete', 'core/mk/uspace_lib/aos/usr_vfs.c'],
    ['delete', 'core/mk/uspace_lib/aos/include/aos_socket.h'],
    ['delete', 'core/mk/uspace_lib/aos/include/dev_log.h'],
    ['delete', 'core/mk/uspace_lib/aos/include/fs_rpc.h'],
    ['delete', 'core/mk/uspace_lib/aos/include/usr_vfs.h'],
    ['delete', 'core/mk/uspace_lib/aos/include/vfs_state.h'],
    # deleted by Yin Xin
    #['delete', 'arch/arm/armv7a/debug/k_backtrace.c'],
    #['delete', 'arch/arm/armv7a/debug/panic.c'],
    ['delete', 'arch/arm/armv8m/debug/k_backtrace.c'],
    ['delete', 'arch/arm/armv8m/debug/panic.c'],
    ['delete', 'arch/arm/armv8m/debug/panic_gcc.S'],
    ['delete', 'arch/arm/armv8m/debug/panic_armcc.S'],
    ['delete', 'arch/arm/armv8m/debug/panic_iccarm.S'],
    ['delete', 'core/mk/uspace/src/ubacktrace.c'],
    ['delete', 'core/mk/uspace/src/ulock.c'],
    ['delete', 'core/mk/uspace/include/ulock.h'],
    # deleted by Jin Liang
    ['delete', 'core/mk/kspace/core_src/usignal.c'],
    # deleted by Zhang Tian
    ['delete', 'core/mbins/'],
    ['delete', 'core/kobj_core/class.c'],
    ['delete', 'core/kobj_core/handle_table.c'],
    ['delete', 'core/kobj_core/obj.c'],
    ['delete', 'core/kobj_core/obj_info.c'],
    ['delete', 'core/kobj_core/obj_open.c'],
    ['delete', 'core/kobj_core/obj_owner.c'],
    ['delete', 'core/kobj_core/obj_register.c'],
]

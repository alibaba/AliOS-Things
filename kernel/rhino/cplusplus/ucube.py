src     = Split('''
                   cpp_mem.cpp
                   cpp_init.c
                   Thread.cpp
                   Semaphore.cpp
                   Mutex.cpp
                   Queue.cpp
                   Timer.cpp
                   WorkQueue.cpp
                  ''')
component = aos_component('cplusplus', src)

component.add_global_includes('include', '')

CONFIG_SYSINFO_KERNEL_VERSION = 'AOS-R-2.0.0'
component.add_global_macros({'SYSINFO_KERNEL_VERSION':'\\"AOS-R-2.0.0\\"'})
component.add_global_macros('AOS_CPLUSPLUS')

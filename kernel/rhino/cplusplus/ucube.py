src     = Split('''
                   cpp_mem.cpp
                   cpp_init.c
                   thread.cpp
                   semaphore.cpp
                   mutex.cpp
                   queue.cpp
                   timer.cpp
                   workQueue.cpp
                  ''')
component = aos_component('cplusplus', src)

component.add_global_includes('include', '')

CONFIG_SYSINFO_KERNEL_VERSION = 'AOS-R-2.0.0'
component.add_global_macros({'SYSINFO_KERNEL_VERSION':'\\"AOS-R-2.0.0\\"'})
component.add_global_macros('AOS_CPLUSPLUS')

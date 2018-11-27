src     = Split('''
        cpp_demo.cpp
        thread_test.cpp
        basic_test.cpp
        inheritance.cpp
        mutex_test.cpp
        queue_test.cpp
        semaphore_test.cpp
        timer_test.cpp
        workqueue_test.cpp
''')

component = aos_component('cpp_demo', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli', 'kernel/cplusplus')
component.add_global_macros('AOS_NO_WIFI')

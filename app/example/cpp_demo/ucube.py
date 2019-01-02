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
component.add_comp_deps('kernel/yloop', 'tools/cli', 'kernel/rhino/cplusplus')
component.add_global_macros('AOS_NO_WIFI')

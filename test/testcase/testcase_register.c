/*
 * warning: testcase collection code is auto generate, please don't change!!!
 */

/*
extern void kernel_test(void);
extern void aos_fs_test(void);
extern void test_epoll_test(void);
extern void test_select_test(void);
extern void test_poll_test(void);
extern void realtime_test(void);
extern void test_posix_test(void);
extern void test_cpp_test(void);
extern void clib_test(void);
extern void stress_test(void);
extern void test_socket(void);
extern void test_kv_test(void);
extern void netmgr_test(void);
extern void test_uagent(void);
*/
extern void test_api(void);
extern void aos_fs_test(void);
extern void aos_kv_test(void);

void add_test(void) {
    test_api();
    aos_fs_test();
    aos_kv_test();
    /*
    kernel_test();

    aos_fs_test();

    test_epoll_test();

    test_select_test();

    test_poll_test();

    realtime_test();

    test_posix_test();

    test_cpp_test();

    clib_test();

    stress_test();

    test_socket();

    test_kv_test();

    netmgr_test();

    test_uagent();
    */

}

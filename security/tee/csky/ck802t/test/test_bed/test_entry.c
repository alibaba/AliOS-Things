/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "test_bed.h"
#include "link_test.h"

NEW_TABLE(test_vec_t, testtree);

extern int tee_xor_test();
extern int tee_tmp_mem_test();
extern int tee_regs_shm_test();
extern int tee_alloc_shm_test();
extern int tee_hash_test();
extern int tee_rsa_test();
extern int tee_id2_test();
extern int tee_id2_test2();
extern int tee_id2_cipher_test();
extern int tee_random_test();

/*
TEST_ADD(tee_id2_test, "id2 hal test");
TEST_ADD(tee_id2_test2, "id2 hal test2");
*/
TEST_ADD(tee_id2_cipher_test, "id2 hal test");

/* these test cases are disabled by default
   you can enable them on demand */
TEST_ADD(tee_xor_test, "XOR sample test");

TEST_ADD(tee_tmp_mem_test, "tmp memory test");
TEST_ADD(tee_regs_shm_test, "register shared memory test");
TEST_ADD(tee_alloc_shm_test, "alloc shared memory test");
TEST_ADD(tee_hash_test, "hash test");
//TEST_ADD(tee_rsa_test, "rsa test");
//TEST_ADD(tee_random_test, "rng test");

extern int tee_aes_test();
TEST_ADD(tee_aes_test, "aes test");

#if 1
extern int tee_asc_drv_test();
TEST_ADD(tee_asc_drv_test, "asc driver test");
#endif

#if 0
extern int tee_tst_test();
TEST_ADD(tee_tst_test, "tst test");
#endif

#define TEST_LOOP       1

void test_entry(void)
{
    int32_t ret;
    test_vec_t *vector;
    int i;

    for (i = 0; i < TEST_LOOP; i++) {
        printk("=======================Start loop %d=======================\n", i);

        /* init each driver in .table.devtree.data section */
        TABLE_FOREACH(vector, testtree) {
            if (vector) {
                printk("+++++Test %s Start+++++\n", vector->name);
                ret = (*(vector->test_func))();
                if (ret != 0) {
                    printk("||||||||||FAIL||||||||||\n");
                    return;
                } else {
                    printk("||||||||||PASS||||||||||\n");
                }
                printk("-----Test %s End-----\n", vector->name);
            }
        }
    }

    printk("All Test Finish !\n");

#ifdef CONFIG_MEMORY_PROFILING
    printk("Call to TW to measure memory usage!\n");
extern uint32_t _chan_call_tee(
        uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);
    _chan_call_tee(0x12345678, 0, 0, 0);
#endif
}

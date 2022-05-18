#include <stdio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "test_ccm.h"
#include "test_ecc.h"
#include "tinydtls.h"

int main(void) {
  CU_ErrorCode result;
  CU_BasicRunMode run_mode = CU_BRM_VERBOSE;

  if (CU_initialize_registry() != CUE_SUCCESS) {
    fprintf(stderr, "E: test framework initialization failed\n");
    return -2;
  }

  t_init_ccm_tests();
  t_init_ecc_tests();

  CU_basic_set_mode(run_mode);
  result = CU_basic_run_tests();

  CU_cleanup_registry();

  return result;
}

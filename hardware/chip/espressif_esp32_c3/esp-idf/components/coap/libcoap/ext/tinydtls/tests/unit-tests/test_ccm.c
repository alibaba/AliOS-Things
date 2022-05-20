/*******************************************************************************
 *
 * Copyright (c) 2020 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 */

#include <assert.h>

#include "dtls_config.h"
#include "test_ccm.h"

#include "tinydtls.h"
#include "ccm.h"
#include "crypto.h"
#include "../ccm-testdata.c"

#include <stdio.h>

static uint8_t buf[1024];

static void
t_test_encrypt_message(void) {
  long int len;
  int n;
  rijndael_ctx ctx;

  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {

    CU_ASSERT(rijndael_set_key_enc_only(&ctx, data[n].key, 8*sizeof(data[n].key)) == 0);

    assert(data[n].r_lm - data[n].la <= sizeof(buf));
    memcpy(buf, data[n].msg + data[n].la, data[n].lm - data[n].la);
    len = dtls_ccm_encrypt_message(&ctx, data[n].M, data[n].L, data[n].nonce,
				   buf,
				   data[n].lm - data[n].la,
				   data[n].msg, data[n].la);

    CU_ASSERT(len == data[n].r_lm - data[n].la);
    CU_ASSERT(memcmp(buf, data[n].result + data[n].la, len) == 0);
  }
}

static void
t_test_decrypt_message(void) {
  int n, len;
  rijndael_ctx ctx;

  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {

    CU_ASSERT(rijndael_set_key_enc_only(&ctx, data[n].key, 8*sizeof(data[n].key)) == 0);

    assert(data[n].r_lm - data[n].la <= sizeof(buf));
    memcpy(buf, data[n].result + data[n].la, data[n].r_lm - data[n].la);

    len = dtls_ccm_decrypt_message(&ctx, data[n].M, data[n].L, data[n].nonce, 
				   buf, data[n].r_lm - data[n].la,
				   data[n].result, data[n].la);

    CU_ASSERT(len == data[n].lm - data[n].la);
    CU_ASSERT(memcmp(buf, data[n].msg + data[n].la, len) == 0);
  }
}

static void
t_test_dtls_encrypt_params(void) {
  int n, len;
  
  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {
    dtls_ccm_params_t params =
      { .nonce = data[n].nonce,
        .tag_length = data[n].M,
        .l = data[n].L
      };

    len = dtls_encrypt_params(&params,
                              data[n].msg + data[n].la,
                              data[n].lm - data[n].la,
                              buf,
                              data[n].key,
                              sizeof(data[n].key),
                              data[n].msg,
                              data[n].la);

    CU_ASSERT(len == data[n].r_lm - data[n].la);
    CU_ASSERT(memcmp(data[n].result + data[n].la, buf, len) == 0);
  }
}

static void
t_test_dtls_decrypt_params(void) {
  int n, len;

  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {
    dtls_ccm_params_t params =
      { .nonce = data[n].nonce,
        .tag_length = data[n].M,
        .l = data[n].L
      };

    assert(data[n].r_lm - data[n].la - data[n].M <= sizeof(buf));

    len = dtls_decrypt_params(&params,
                              data[n].result + data[n].la,
                              data[n].r_lm - data[n].la,
                              buf,
                              data[n].key,
                              sizeof(data[n].key),
                              data[n].result,
                              data[n].la);

    CU_ASSERT(len == data[n].lm - data[n].la);
    CU_ASSERT(memcmp(data[n].msg + data[n].la, buf, len) == 0);
  }
}

static void
t_test_dtls_encrypt(void) {
  int n, len;
  
  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {
    /* The backwards-compatible dtls_encrypt() and dtls_decrypt()
     * only handle cipher suites with M=8 and L=3. */
    if ((data[n].M == 8) && ((15 - data[n].L) == DTLS_CCM_NONCE_SIZE)) {
      len = dtls_encrypt(data[n].msg + data[n].la,
                         data[n].lm - data[n].la,
                         buf,
                         data[n].nonce,
                         data[n].key,
                         sizeof(data[n].key),
                         data[n].msg,
                         data[n].la);
      CU_ASSERT(len == data[n].r_lm - data[n].la);
      CU_ASSERT(memcmp(data[n].result + data[n].la, buf, len) == 0);
    }
  }
}

static void
t_test_dtls_decrypt(void) {
  int n, len;
  
  for (n = 0; n < sizeof(data)/sizeof(struct test_vector); ++n) {
    /* The backwards-compatible dtls_encrypt() and dtls_decrypt()
     * only handle cipher suites with M=8 and L=3. */
    if ((data[n].M == 8) && ((15 - data[n].L) == DTLS_CCM_NONCE_SIZE)) {
      assert(data[n].r_lm - data[n].la - data[n].M <= sizeof(buf));

      len = dtls_decrypt(data[n].result + data[n].la, data[n].r_lm - data[n].la,
                         buf,
                         data[n].nonce,
                         data[n].key,
                         sizeof(data[n].key),
                         data[n].msg, data[n].la);
      CU_ASSERT(len == data[n].r_lm - data[n].la - data[n].M);
      CU_ASSERT(memcmp(data[n].msg + data[n].la, buf, len) == 0);
    }
  }
}

CU_pSuite
t_init_ccm_tests(void) {
  CU_pSuite suite;

  suite = CU_add_suite("CCM", NULL, NULL);
  if (!suite) {                        /* signal error */
    fprintf(stderr, "W: cannot add CCM test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

  if (!CU_ADD_TEST(suite,t_test_encrypt_message)) {
    fprintf(stderr, "W: cannot add t_dtls_encrypt_message (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_decrypt_message)) {
    fprintf(stderr, "W: cannot add t_dtls_decrypt_message (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_dtls_encrypt_params)) {
    fprintf(stderr, "W: cannot add t_dtls_encrypt_params (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_dtls_decrypt_params)) {
    fprintf(stderr, "W: cannot add t_dtls_decrypt_params (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_dtls_encrypt)) {
    fprintf(stderr, "W: cannot add t_dtls_encrypt (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_dtls_decrypt)) {
    fprintf(stderr, "W: cannot add t_dtls_decrypt (%s)\n",
            CU_get_error_msg());
  }

  return suite;
}


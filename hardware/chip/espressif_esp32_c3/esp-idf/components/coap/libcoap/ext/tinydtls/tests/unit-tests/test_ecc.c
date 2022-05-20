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
#include "dtls_prng.h"
#include "test_ecc.h"
#include "ecc/ecc.h"

#include "tinydtls.h"
#include "crypto.h"

#include <stdio.h>

//These are testvalues taken from the NIST P-256 definition
//6b17d1f2 e12c4247 f8bce6e5 63a440f2 77037d81 2deb33a0 f4a13945 d898c296
uint32_t BasePointx[8] = {	0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81,
							0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2};

//4fe342e2 fe1a7f9b 8ee7eb4a 7c0f9e16 2bce3357 6b315ece cbb64068 37bf51f5
uint32_t BasePointy[8] = {	0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357,
							0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2};

//de2444be bc8d36e6 82edd27e 0f271508 617519b3 221a8fa0 b77cab39 89da97c9
uint32_t Sx[8] = {	0x89da97c9, 0xb77cab39, 0x221a8fa0, 0x617519b3, 
					0x0f271508, 0x82edd27e, 0xbc8d36e6, 0xde2444be};

//c093ae7f f36e5380 fc01a5aa d1e66659 702de80f 53cec576 b6350b24 3042a256
uint32_t Sy[8] = {	0x3042a256, 0xb6350b24, 0x53cec576, 0x702de80f,
		 			0xd1e66659, 0xfc01a5aa, 0xf36e5380, 0xc093ae7f};

//55a8b00f 8da1d44e 62f6b3b2 5316212e 39540dc8 61c89575 bb8cf92e 35e0986b
uint32_t Tx[8] = {	0x35e0986b, 0xbb8cf92e, 0x61c89575, 0x39540dc8,
					0x5316212e, 0x62f6b3b2, 0x8da1d44e, 0x55a8b00f};

//5421c320 9c2d6c70 4835d82a c4c3dd90 f61a8a52 598b9e7a b656e9d8 c8b24316
uint32_t Ty[8] = {	0xc8b24316, 0xb656e9d8, 0x598b9e7a, 0xf61a8a52,
					0xc4c3dd90, 0x4835d82a, 0x9c2d6c70, 0x5421c320};

//c51e4753 afdec1e6 b6c6a5b9 92f43f8d d0c7a893 3072708b 6522468b 2ffb06fd
uint32_t secret[8] = {	0x2ffb06fd, 0x6522468b, 0x3072708b, 0xd0c7a893,
						0x92f43f8d, 0xb6c6a5b9, 0xafdec1e6, 0xc51e4753};
							
//72b13dd4 354b6b81 745195e9 8cc5ba69 70349191 ac476bd4 553cf35a 545a067e
uint32_t resultAddx[8] = {	0x545a067e, 0x553cf35a, 0xac476bd4, 0x70349191,
							0x8cc5ba69, 0x745195e9, 0x354b6b81, 0x72b13dd4};

//8d585cbb 2e1327d7 5241a8a1 22d7620d c33b1331 5aa5c9d4 6d013011 744ac264
uint32_t resultAddy[8] = {	0x744ac264, 0x6d013011, 0x5aa5c9d4, 0xc33b1331,
							0x22d7620d, 0x5241a8a1, 0x2e1327d7, 0x8d585cbb};

//7669e690 1606ee3b a1a8eef1 e0024c33 df6c22f3 b17481b8 2a860ffc db6127b0
uint32_t resultDoublex[8] = {	0xdb6127b0, 0x2a860ffc, 0xb17481b8, 0xdf6c22f3,
								0xe0024c33, 0xa1a8eef1, 0x1606ee3b, 0x7669e690};

//fa878162 187a54f6 c39f6ee0 072f33de 389ef3ee cd03023d e10ca2c1 db61d0c7
uint32_t resultDoubley[8] = {	0xdb61d0c7, 0xe10ca2c1, 0xcd03023d, 0x389ef3ee,
								0x072f33de, 0xc39f6ee0, 0x187a54f6, 0xfa878162};

//51d08d5f 2d427888 2946d88d 83c97d11 e62becc3 cfc18bed acc89ba3 4eeca03f
uint32_t resultMultx[8] = {	0x4eeca03f, 0xacc89ba3, 0xcfc18bed, 0xe62becc3,
							0x83c97d11, 0x2946d88d, 0x2d427888, 0x51d08d5f};

//75ee68eb 8bf626aa 5b673ab5 1f6e744e 06f8fcf8 a6c0cf30 35beca95 6a7b41d5
uint32_t resultMulty[8] = {	0x6a7b41d5, 0x35beca95, 0xa6c0cf30, 0x06f8fcf8,
							0x1f6e744e, 0x5b673ab5, 0x8bf626aa, 0x75ee68eb};

static const uint32_t ecdsaTestMessage[] = { 0x65637572, 0x20612073, 0x68206F66, 0x20686173, 0x69732061, 0x68697320, 0x6F2C2054, 0x48616C6C};

static const uint32_t ecdsaTestSecret[] = {0x94A949FA, 0x401455A1, 0xAD7294CA, 0x896A33BB, 0x7A80E714, 0x4321435B, 0x51247A14, 0x41C1CB6B};

static const uint32_t ecdsaTestRand1[] = { 0x1D1E1F20, 0x191A1B1C, 0x15161718, 0x11121314, 0x0D0E0F10, 0x090A0B0C, 0x05060708, 0x01020304};
static const uint32_t ecdsaTestresultR1[] = { 0xC3B4035F, 0x515AD0A6, 0xBF375DCA, 0x0CC1E997, 0x7F54FDCD, 0x04D3FECA, 0xB9E396B9, 0x515C3D6E};
static const uint32_t ecdsaTestresultS1[] = { 0x5366B1AB, 0x0F1DBF46, 0xB0C8D3C4, 0xDB755B6F, 0xB9BF9243, 0xE644A8BE, 0x55159A59, 0x6F9E52A6};

static const uint32_t ecdsaTestRand2[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x01FFFFFF};
static const uint32_t ecdsaTestresultR2[] = { 0x14146C91, 0xE878724D, 0xCD4FF928, 0xCC24BC04, 0xAC403390, 0x650C0060, 0x4A30B3F1, 0x9C69B726};
static const uint32_t ecdsaTestresultS2[] = { 0x433AAB6F, 0x808250B1, 0xE46F90F4, 0xB342E972, 0x18B2F7E4, 0x2DB981A2, 0x6A288FA4, 0x41CF59DB};

static void
t_test_ecc_add(void) {
  uint32_t tempx[8];
  uint32_t tempy[8];

  ecc_ec_add(Tx, Ty, Sx, Sy, tempx, tempy);
  CU_ASSERT(ecc_isSame(tempx, resultAddx, arrayLength));
  CU_ASSERT(ecc_isSame(tempy, resultAddy, arrayLength));
}

static void
t_test_ecc_double(void) {
  uint32_t tempx[8];
  uint32_t tempy[8];

  ecc_ec_double(Sx, Sy, tempx, tempy);
  CU_ASSERT(ecc_isSame(tempx, resultDoublex, arrayLength));
  CU_ASSERT(ecc_isSame(tempy, resultDoubley, arrayLength));
}

static void
t_test_ecc_mult(void) {
  uint32_t tempx[8];
  uint32_t tempy[8];

  ecc_ec_mult(Sx, Sy, secret, tempx, tempy);
  CU_ASSERT(ecc_isSame(tempx, resultMultx, arrayLength));
  CU_ASSERT(ecc_isSame(tempy, resultMulty, arrayLength));
}

static void
t_test_ecc_dh(void) {
  uint32_t tempx[8];
  uint32_t tempy[8];
  uint32_t tempAx2[8];
  uint32_t tempAy2[8];
  uint32_t tempBx1[8];
  uint32_t tempBy1[8];
  uint32_t tempBx2[8];
  uint32_t tempBy2[8];
  uint32_t secretA[8];
  uint32_t secretB[8];
  int ret;

  ret = dtls_prng((void *)secretA, sizeof(secretA));
  CU_ASSERT(ret > 1);

  ret = dtls_prng((void *)secretB, sizeof(secretB));
  CU_ASSERT(ret > 1);

  ecc_ec_mult(BasePointx, BasePointy, secretA, tempx, tempy);
  ecc_ec_mult(BasePointx, BasePointy, secretB, tempBx1, tempBy1);
  //public key exchange
  ecc_ec_mult(tempBx1, tempBy1, secretA, tempAx2, tempAy2);
  ecc_ec_mult(tempx, tempy, secretB, tempBx2, tempBy2);
  CU_ASSERT(ecc_isSame(tempAx2, tempBx2, arrayLength));
  CU_ASSERT(ecc_isSame(tempAy2, tempBy2, arrayLength));
}

static void
t_test_ecc_ecdsa(void) {
  int ret;
  uint32_t tempx[9];
  uint32_t tempy[9];
  uint32_t pub_x[8];
  uint32_t pub_y[8];

  ecc_ec_mult(BasePointx, BasePointy, ecdsaTestSecret, pub_x, pub_y);

  ret = ecc_ecdsa_sign(ecdsaTestSecret, ecdsaTestMessage, ecdsaTestRand1, tempx, tempy);
  assert(ecc_isSame(tempx, ecdsaTestresultR1, arrayLength));
  assert(ecc_isSame(tempy, ecdsaTestresultS1, arrayLength));
  assert(ret == 0);

  ret = ecc_ecdsa_validate(pub_x, pub_y, ecdsaTestMessage, tempx, tempy);
  assert(ret == 0);

  ret = ecc_ecdsa_sign(ecdsaTestSecret, ecdsaTestMessage, ecdsaTestRand2, tempx, tempy);
  assert(ecc_isSame(tempx, ecdsaTestresultR2, arrayLength));
  assert(ecc_isSame(tempy, ecdsaTestresultS2, arrayLength));
  assert(ret == 0);

  ret = ecc_ecdsa_validate(pub_x, pub_y, ecdsaTestMessage, tempx, tempy);
  assert(ret == 0);
}

static void
t_test_ecc_ecdsa0(void) {
  int ret;
  uint32_t tempx[9] = {0x433aab6f, 0x36dfe2c6, 0xf9f2ed29, 0xda0a9a8f, 0x62684e91, 0x6375ba10, 0x300c28c5, 0xe47cfbf2, 0x5fa58f52};
  uint32_t tempy[9];
  uint32_t pub_x[8];
  uint32_t pub_y[8];

  ecc_ec_mult(BasePointx, BasePointy, ecdsaTestSecret, pub_x, pub_y);

  ret = ecc_ecdsa_sign(ecdsaTestSecret, ecdsaTestMessage, ecdsaTestRand1, tempx, tempy);
  
  memset(tempy, 0, sizeof(tempy));
  ret = ecc_ecdsa_validate(pub_x, pub_y, ecdsaTestMessage, tempx, tempy);
  CU_ASSERT(ret == -1);
}

CU_pSuite
t_init_ecc_tests(void) {
  CU_pSuite suite;

  suite = CU_add_suite("ECC", NULL, NULL);
  if (!suite) {                        /* signal error */
    fprintf(stderr, "W: cannot add ECC test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

  /* On POSIX system, getrandom() is used where available, ignoring
   * the seed. For other platforms, the random sequence will be the
   * same for each run. */
  dtls_prng_init(0);
  
  if (!CU_ADD_TEST(suite,t_test_ecc_add)) {
    fprintf(stderr, "W: cannot add test for ECC add (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_ecc_double)) {
    fprintf(stderr, "W: cannot add test for ECC double (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_ecc_mult)) {
    fprintf(stderr, "W: cannot add test for ECC mult (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_ecc_dh)) {
    fprintf(stderr, "W: cannot add test for ECC DH (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_ecc_ecdsa)) {
    fprintf(stderr, "W: cannot add test for ECC ECDSA (%s)\n",
            CU_get_error_msg());
  }

  if (!CU_ADD_TEST(suite,t_test_ecc_ecdsa0)) {
    fprintf(stderr, "W: cannot add test for ECC ECDSA with invalid argument (%s)\n",
            CU_get_error_msg());
  }

  return suite;
}


/*-----------------------------------------------------------------------------
 *      Name:         CV_CoreSimd.c
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/**
\brief Test case: TC_CoreSimd_SatAddSub
\details
- Check Saturating addition and subtraction:
  __QADD
  __QSUB
*/
void TC_CoreSimd_SatAddSub (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile int32_t op1_s32, op2_s32;
  volatile int32_t res_s32;

  /* --- __QADD Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80000003;
  op2_s32 = (int32_t)0x00000004;
  res_s32 = __QADD(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80000007);

  op1_s32 = (int32_t)0x80000000;
  op2_s32 = (int32_t)0x80000002;
  res_s32 = __QADD(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80000000);

  /* --- __QSUB Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80000003;
  op2_s32 = (int32_t)0x00000004;
  res_s32 = __QSUB(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80000000);

  op1_s32 = (int32_t)0x80000003;
  op2_s32 = (int32_t)0x00000002;
  res_s32 = __QSUB(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80000001);
#endif
}

/**
\brief Test case: TC_CoreSimd_ParSat16
\details
- Check Parallel 16-bit saturation:
  __SSAT16
  __USAT16
*/
void TC_CoreSimd_ParSat16 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile int32_t op1_s32;
  volatile int32_t res_s32;

  /* --- __SSAT16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80030168;
  res_s32 = __SSAT16(op1_s32, 8);
  ASSERT_TRUE(res_s32 == (int32_t)0xFF80007F);

  /* --- __USAT16 Test ---------------------------------------------- */
  op1_s32 = 0x0030168;
  res_s32 = __USAT16(op1_s32, 8);
  ASSERT_TRUE(res_s32 == 0x000300FF);
#endif
}

/**
\brief Test case: TC_CoreSimd_PackUnpack
\details
- Check Packing and unpacking:
  __SXTB16
  __SXTAB16
  __UXTB16
  __UXTAB16
*/
void TC_CoreSimd_PackUnpack (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile int32_t op1_s32, op2_s32;
  volatile int32_t res_s32;

  /* --- __SXTB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80830168;
  res_s32 = __SXTB16(op1_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFF830068);

  /* --- __SXTAB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x000D0008;
  op2_s32 = (int32_t)0x80830168;
  res_s32 = __SXTAB16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFF900070);

  /* --- __UXTB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80830168;
  res_s32 = __UXTB16(op1_s32);
  ASSERT_TRUE(res_s32 == 0x00830068);

  /* --- __UXTAB16 Test ---------------------------------------------- */
  op1_s32 =          0x000D0008;
  op2_s32 = (int32_t)0x80830168;
  res_s32 = __UXTAB16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == 0x00900070);
#endif
}

/**
\brief Test case: TC_CoreSimd_ParSel
\details
- Check Parallel selection:
  __SEL
*/
void TC_CoreSimd_ParSel (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile uint32_t res_u32;

  volatile int32_t op1_s32, op2_s32;
  volatile int32_t res_s32;

  APSR_Type  apsr;
  xPSR_Type  xpsr;

  /* --- __SEL Test ---------------------------------------------- */
  op1_s32 = 0x33221100;
  op2_s32 = 0x77665544;

  res_s32 = __SADD8(0x80808080, 0x00000000);            /* __sadd8   sets APSR.GE = 0x00 */
  res_u32 = __get_APSR();
  apsr.w = __get_APSR();
  ASSERT_TRUE( (res_u32 == apsr.w) );
  xpsr.w = __get_xPSR();
  ASSERT_TRUE( (((res_u32 >> 16) & 0x0F) == xpsr.b.GE)  );
  res_s32 = __SEL(op1_s32, op2_s32);                      /* __sel          APSR.GE = 0x00 */
  ASSERT_TRUE( res_s32 == 0x77665544);

  res_s32 = __SADD8(0x80808000, 0x00000000);            /* __sadd8   sets APSR.GE = 0x01 */
  res_u32 = __get_APSR();
  apsr.w = __get_APSR();
  ASSERT_TRUE( (res_u32 == apsr.w)  );
  xpsr.w = __get_xPSR();
  ASSERT_TRUE( (((res_u32 >> 16) & 0x0F) == xpsr.b.GE)  );
  res_s32 = __SEL(op1_s32, op2_s32);                      /* __sel          APSR.GE = 0x01 */
  ASSERT_TRUE(res_s32 == 0x77665500);

  res_s32 = __SADD8(0x80800080, 0x00000000);            /* __sadd8   sets APSR.GE = 0x02 */
  res_u32 = __get_APSR();
  apsr.w = __get_APSR();
  ASSERT_TRUE( (res_u32 == apsr.w) );
  xpsr.w = __get_xPSR();
  ASSERT_TRUE( (((res_u32 >> 16) & 0x0F) == xpsr.b.GE)  );
  res_s32 = __SEL(op1_s32, op2_s32);                      /* __sel          APSR.GE = 0x02 */
  ASSERT_TRUE(res_s32 == 0x77661144);
#endif
}

/**
\brief Test case: TC_CoreSimd_ParAddSub8
\details
- Check Parallel 8-bit addition and subtraction:
  __SADD8                                   S  Signed
  __SSUB8                                   Q  Signed Saturating
  __SHADD8                                  SH Signed Halving
  __SHSUB8                                  U  Unsigned
  __QADD8                                   UQ Unsigned Saturating
  __QSUB8                                   UH Unsigned Halving
  __UADD8
  __USUB8
  __UHADD8
  __UHSUB8
  __UQADD8
  __UQSUB8
*/
void TC_CoreSimd_ParAddSub8 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile uint32_t op1_u32, op2_u32;
  volatile uint32_t res_u32;

  volatile int32_t op1_s32, op2_s32;
  volatile int32_t res_s32;

  /* --- __SADD8 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x87858381;
  op2_s32 = (int32_t)0x08060402;
  res_s32 = __SADD8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x8F8B8783);

  /* --- __SSUB8 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x8F8B8783;
  op2_s32 = (int32_t)0x08060402;
  res_s32 = __SSUB8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x87858381);

  /* --- __SHADD8 Test ---------------------------------------------- */
  op1_s32 = 0x07050302;
  op2_s32 = 0x08060402;
  res_s32 = __SHADD8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == 0x07050302);

  /* --- __SHSUB8 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x8F8B8783;
  op2_s32 = 0x08060402;
  res_s32 = __SHSUB8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xC3C2C1C0);

  /* --- __QADD8 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x8085837F;
  op2_s32 = (int32_t)0xFF060402;
  res_s32 = __QADD8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x808B877F);

  /* --- __QSUB8 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x808B8783;
  op2_s32 = (int32_t)0x08060402;
  res_s32 = __QSUB8(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80858381);

  /* --- __UADD8 Test ---------------------------------------------- */
  op1_u32 = 0x07050301;
  op2_u32 = 0x08060402;
  res_u32 = __UADD8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x0F0B0703);

  /* --- __USUB8 Test ---------------------------------------------- */
  op1_u32 = 0x0F0B0703;
  op2_u32 = 0x08060402;
  res_u32 = __USUB8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x07050301);

  /* --- __UHADD8 Test ---------------------------------------------- */
  op1_u32 = 0x07050302;
  op2_u32 = 0x08060402;
  res_u32 = __UHADD8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x07050302);

  /* --- __UHSUB8 Test ---------------------------------------------- */
  op1_u32 = 0x0F0B0703;
  op2_u32 = 0x08060402;
  res_u32 = __UHSUB8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x03020100);

  /* --- __UQADD8 Test ---------------------------------------------- */
  op1_u32 = 0xFF050301;
  op2_u32 = 0x08060402;
  res_u32 = __UQADD8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0xFF0B0703);

  /* --- __UQSUB8 Test ---------------------------------------------- */
  op1_u32 = 0x080B0702;
  op2_u32 = 0x0F060408;
  res_u32 = __UQSUB8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00050300);
#endif
}

/**
\brief Test case: TC_CoreSimd_AbsDif8
\details
- Check Sum of 8-bit absolute differences:
  __USAD8
  __USADA8
*/
void TC_CoreSimd_AbsDif8 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile uint32_t op1_u32, op2_u32, op3_u32;
  volatile uint32_t res_u32;

  /* --- __USAD8 Test ---------------------------------------------- */
  op1_u32 = 0x87858381;
  op2_u32 = 0x08060402;
  res_u32 = __USAD8(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x000001FC);

  /* --- __USADA8 Test ---------------------------------------------- */
  op1_u32 = 0x87858381;
  op2_u32 = 0x08060402;
  op3_u32 = 0x00008000;
  res_u32 = __USADA8(op1_u32, op2_u32, op3_u32);
  ASSERT_TRUE(res_u32 == 0x000081FC);
#endif
}

/**
\brief Test case: TC_CoreSimd_ParAddSub16
\details
- Check Parallel 16-bit addition and subtraction:
  __SADD16
  __SSUB16
  __SASX
  __SSAX
  __SHADD16
  __SHSUB16
  __SHASX
  __SHSAX
  __QADD16
  __QSUB16
  __QASX
  __QSAX
  __UADD16
  __USUB16
  __UASX
  __USAX
  __UHADD16
  __UHSUB16
  __UHASX
  __UHSAX
  __UQSUB16
  __UQADD16
  __UQASX
  __UQSAX
*/
void TC_CoreSimd_ParAddSub16 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile uint32_t op1_u32, op2_u32;
  volatile uint32_t res_u32;

  volatile int32_t op1_s32, op2_s32;
  volatile int32_t res_s32;

  /* --- __SADD16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038001;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SADD16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80078003);

  /* --- __SSUB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80078003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SSUB16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80038001);

  /* --- __SASX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80078003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SASX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80097FFF);

  /* --- __SSAX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038007;
  op2_s32 = (int32_t)0x00020004;
  res_s32 = __SSAX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x7FFF8009);

  /* --- __SHADD16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038001;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SHADD16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xC003C001);

  /* --- __SHSUB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80078003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SHSUB16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xC001C000);

  /* --- __SHASX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80078003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SHASX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xC004BFFF);

  /* --- __SHSAX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038007;
  op2_s32 = (int32_t)0x00020004;
  res_s32 = __SHSAX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xBFFFC004);

  /* --- __QADD16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038000;
  op2_s32 = (int32_t)0x00048002;
  res_s32 = __QADD16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80078000);

  /* --- __QSUB16 Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __QSUB16(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80008001);

  /* --- __QASX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80078003;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __QASX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80098000);

  /* --- __QSAX Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x80038007;
  op2_s32 = (int32_t)0x00020004;
  res_s32 = __QSAX(op1_s32, op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x80008009);

  /* --- __UADD16 Test ---------------------------------------------- */
  op1_u32 = 0x00010002;
  op2_u32 = 0x00020004;
  res_u32 = __UADD16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00030006);

  /* --- __USUB16 Test ---------------------------------------------- */
  op1_u32 = 0x00030006;
  op2_u32 = 0x00020004;
  res_u32 = __USUB16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00010002);

  /* --- __UASX Test ---------------------------------------------- */
  op1_u32 = 0x80078003;
  op2_u32 = 0x00040002;
  res_u32 = __UASX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x80097FFF);

  /* --- __USAX Test ---------------------------------------------- */
  op1_u32 = 0x80038007;
  op2_u32 = 0x00020004;
  res_u32 = __USAX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x7FFF8009);

  /* --- __UHADD16 Test ---------------------------------------------- */
  op1_u32 = 0x00010002;
  op2_u32 = 0x00020004;
  res_u32 = __UHADD16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00010003);

  /* --- __UHSUB16 Test ---------------------------------------------- */
  op1_u32 = 0x00030006;
  op2_u32 = 0x00020004;
  res_u32 = __UHSUB16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00000001);

  /* --- __UHASX Test ---------------------------------------------- */
  op1_u32 = 0x80078003;
  op2_u32 = 0x00040002;
  res_u32 = __UHASX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x40043FFF);

  /* --- __UHSAX Test ---------------------------------------------- */
  op1_u32 = 0x80038007;
  op2_u32 = 0x00020004;
  res_u32 = __UHSAX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x3FFF4004);

  /* --- __UQADD16 Test ---------------------------------------------- */
  op1_u32 = 0xFFFE0002;
  op2_u32 = 0x00020004;
  res_u32 = __UQADD16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0xFFFF0006);

  /* --- __UQSUB16 Test ---------------------------------------------- */
  op1_u32 = 0x00020006;
  op2_u32 = 0x00030004;
  res_u32 = __UQSUB16(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x00000002);

  /* --- __UQASX Test ---------------------------------------------- */
  op1_u32 = 0xFFF80003;
  op2_u32 = 0x00040009;
  res_u32 = __UQASX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0xFFFF0000);

  /* --- __UQSAX Test ---------------------------------------------- */
  op1_u32 = 0x0003FFF8;
  op2_u32 = 0x00090004;
  res_u32 = __UQSAX(op1_u32, op2_u32);
  ASSERT_TRUE(res_u32 == 0x0000FFFF);
#endif
}

/**
\brief Test case: TC_CoreSimd_ParMul16
\details
- Check Parallel 16-bit multiplication:
  __SMLAD
  __SMLADX
  __SMLALD
  __SMLALDX
  __SMLSD
  __SMLSDX
  __SMLSLD
  __SMLSLDX
  __SMUAD
  __SMUADX
  __SMUSD
  __SMUSDX
*/
void TC_CoreSimd_ParMul16 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile int32_t op1_s32, op2_s32, op3_s32;
  volatile int32_t res_s32;

  volatile int64_t op1_s64;
  volatile int64_t res_s64;

  /* --- __SMLAD Test ---------------------------------------------- */
  op1_s32 = 0x00030002;
  op2_s32 = 0x00050004;
  op3_s32 = 0x20000000;
  res_s32 = __SMLAD(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x20000017);

  /* --- __SMLADX Test ---------------------------------------------- */
  op1_s32 = 0x00030002;
  op2_s32 = 0x00050004;
  op3_s32 = 0x00000800;
  res_s32 = __SMLADX(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x00000816);

  /* --- __SMLALD Test ---------------------------------------------- */
  op1_s32 = 0x00030002;
  op2_s32 = 0x00050004;
  op1_s64 = 0x00000000200000000LL;
  res_s64 = __SMLALD(op1_s32, op2_s32, op1_s64);
  ASSERT_TRUE(res_s64 == 0x0000000200000017LL);

  /* --- __SMLALDX Test ---------------------------------------------- */
  op1_s32 = 0x00030002;
  op2_s32 = 0x00050004;
  op1_s64 = 0x00000000200000000LL;
  res_s64 = __SMLALDX(op1_s32, op2_s32, op1_s64);
  ASSERT_TRUE(res_s64 == 0x0000000200000016LL);

  /* --- __SMLSD Test ---------------------------------------------- */
  op1_s32 = 0x00030006;
  op2_s32 = 0x00050004;
  op3_s32 = 0x00000800;
  res_s32 = __SMLSD(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x00000809);

  /* --- __SMLSDX Test ---------------------------------------------- */
  op1_s32 = 0x00030002;
  op2_s32 = 0x00050004;
  op3_s32 = 0x00000800;
  res_s32 = __SMLSDX(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x000007FE);

  /* --- __SMLSLD Test ---------------------------------------------- */
  op1_s32 = 0x00030006;
  op2_s32 = 0x00050004;
  op1_s64 = 0x00000000200000000LL;
  res_s64 = __SMLSLD(op1_s32, op2_s32, op1_s64);
  ASSERT_TRUE(res_s64 == 0x0000000200000009LL);

  /* --- __SMLSLDX Test ---------------------------------------------- */
  op1_s32 = 0x00030006;
  op2_s32 = 0x00050004;
  op1_s64 = 0x00000000200000000LL;
  res_s64 = __SMLSLDX(op1_s32, op2_s32, op1_s64);
  ASSERT_TRUE(res_s64 == 0x0000000200000012LL);

  /* --- __SMUAD Test ---------------------------------------------- */
  op1_s32 = 0x00030001;
  op2_s32 = 0x00040002;
  res_s32 = __SMUAD(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == 0x0000000E);

  op1_s32 = (int32_t)0xFFFDFFFF;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SMUAD(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFFFFFFF2);

  /* --- __SMUADX Test ---------------------------------------------- */
  op1_s32 = 0x00030001;
  op2_s32 = 0x00040002;
  res_s32 = __SMUADX(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == 0x0000000A);

  op1_s32 = (int32_t)0xFFFDFFFF;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SMUADX(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFFFFFFF6);

  /* --- __SMUSD Test ---------------------------------------------- */
  op1_s32 = (int32_t)0x00030001;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SMUSD(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFFFFFFF6);

  op1_s32 = (int32_t)0xFFFDFFFF;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SMUSD(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == 0x0000000A);

  /* --- __SMUSDX Test ---------------------------------------------- */
  op1_s32 = 0x00030001;
  op2_s32 = 0x00040002;
  res_s32 = __SMUSDX(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0xFFFFFFFE);

  op1_s32 = (int32_t)0xFFFDFFFF;
  op2_s32 = (int32_t)0x00040002;
  res_s32 = __SMUSDX(op1_s32,op2_s32);
  ASSERT_TRUE(res_s32 == (int32_t)0x00000002);
#endif
}

/**
\brief Test case: TC_CoreSimd_Part9
\details
- Check Packing Halfword:
  __PKHBT
  __PKHTB
*/
void TC_CoreSimd_Pack16 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile uint32_t op1_u32, op2_u32;
  volatile uint32_t res_u32;

  /* --- __PKHBT Test ---------------------------------------------- */
  op1_u32 = 0x00000111;
  op2_u32 = 0x22200000;
  res_u32 = __PKHBT(op1_u32, op2_u32, 0);
  ASSERT_TRUE(res_u32 == 0x22200111);

  op1_u32 = 0x00000111;
  op2_u32 = 0x22200000;
  res_u32 = __PKHBT(op1_u32, op2_u32, 4);
  ASSERT_TRUE(res_u32 == 0x22000111);

  /* --- __PKHTB Test ---------------------------------------------- */
  op1_u32 = 0x11100000;
  op2_u32 = 0x00000222;
  res_u32 = __PKHTB(op1_u32, op2_u32, 0);
  ASSERT_TRUE(res_u32 == 0x11100222);

  op1_u32 = 0x11100000;
  op2_u32 = 0x00000222;
  res_u32 = __PKHTB(op1_u32, op2_u32, 4);
  ASSERT_TRUE(res_u32 == 0x11100022);
#endif
}

/**
\brief Test case: TC_CoreSimd_MulAcc32
\details
- Check Signed Most Significant Word Multiply Accumulate:
  __SMMLA
*/
void TC_CoreSimd_MulAcc32 (void) {
#if ((defined (__ARM_ARCH_7EM__ ) && (__ARM_ARCH_7EM__  == 1)) || \
     (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))     )
  volatile int32_t op1_s32, op2_s32, op3_s32;
  volatile int32_t res_s32;

  /* --- __SMMLA Test ---------------------------------------------- */
  op1_s32 = 0x00000200;
  op2_s32 = 0x00000004;
  op3_s32 = 0x00000100;
  res_s32 = __SMMLA(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x00000100);

  op1_s32 = 0x40000000;
  op2_s32 = 0x00000010;
  op3_s32 = 0x00000300;
  res_s32 = __SMMLA(op1_s32, op2_s32, op3_s32);
  ASSERT_TRUE(res_s32 == 0x00000304);
#endif
}

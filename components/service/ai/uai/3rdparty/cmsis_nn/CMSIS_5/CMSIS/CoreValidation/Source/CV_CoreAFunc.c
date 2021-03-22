/*-----------------------------------------------------------------------------
 *      Name:         CV_CoreFunc.c 
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 ARM Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_IRQ(void) {
  uint32_t orig = __get_CPSR();
  
  __enable_irq();
  uint32_t cpsr = __get_CPSR();
  ASSERT_TRUE((cpsr & CPSR_I_Msk) == 0U);
  
  __disable_irq();
  cpsr = __get_CPSR();
  ASSERT_TRUE((cpsr & CPSR_I_Msk) == CPSR_I_Msk);
  
  __set_CPSR(orig);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_FPSCR(void) {

  volatile float f1 = 47.11f;
  volatile float f2 = 8.15f;
  volatile float f3 = f1 / f2;

  uint32_t fpscr = __get_FPSCR();
  __set_FPSCR(fpscr);

  ASSERT_TRUE(fpscr == __get_FPSCR());
  ASSERT_TRUE((f3 < 5.781f) && (f3 > 5.780f));
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
#if defined(__CC_ARM)
#define __SUBS(Rd, Rm, Rn) __ASM volatile("SUBS " # Rd ", " # Rm ", " # Rn)
#define __ADDS(Rd, Rm, Rn) __ASM volatile("ADDS " # Rd ", " # Rm ", " # Rn)
#elif defined( __GNUC__ ) && defined(__thumb__)
#define __SUBS(Rd, Rm, Rn) __ASM volatile("SUB %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn))
#define __ADDS(Rd, Rm, Rn) __ASM volatile("ADD %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn))
#else
#define __SUBS(Rd, Rm, Rn) __ASM volatile("SUBS %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn))
#define __ADDS(Rd, Rm, Rn) __ASM volatile("ADDS %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn))
#endif

void TC_CoreAFunc_CPSR(void) {
  uint32_t result;

  uint32_t cpsr = __get_CPSR();
  __set_CPSR(cpsr & CPSR_M_Msk);

  // Check negative flag
  int32_t Rm = 5;
  int32_t Rn = 7;
  __SUBS(Rm, Rm, Rn);
  result  = __get_CPSR();
  ASSERT_TRUE((result & CPSR_N_Msk) == CPSR_N_Msk);

  // Check zero and compare flag
  Rm = 5;
  __SUBS(Rm, Rm, Rm);
  result  = __get_CPSR();
  ASSERT_TRUE((result & CPSR_Z_Msk) == CPSR_Z_Msk);
  ASSERT_TRUE((result & CPSR_C_Msk) == CPSR_C_Msk);

  // Check overflow flag
  Rm = 5;
  Rn = INT32_MAX;
  __ADDS(Rm, Rm, Rn);
  result  = __get_CPSR();
  ASSERT_TRUE((result & CPSR_V_Msk) == CPSR_V_Msk);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_Mode(void) {
  uint32_t mode = __get_mode();
  __set_mode(mode);

  ASSERT_TRUE(mode == __get_mode());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
static uint32_t TC_CoreAFunc_SP_orig;
static uint32_t TC_CoreAFunc_SP_sp;
static uint32_t TC_CoreAFunc_SP_result;

void TC_CoreAFunc_SP(void) {
  TC_CoreAFunc_SP_orig = __get_SP();

  TC_CoreAFunc_SP_sp = TC_CoreAFunc_SP_orig + 0x12345678U;
  __set_SP(TC_CoreAFunc_SP_sp);
  TC_CoreAFunc_SP_result = __get_SP();

  __set_SP(TC_CoreAFunc_SP_orig);

  ASSERT_TRUE(TC_CoreAFunc_SP_result == TC_CoreAFunc_SP_sp);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
static uint32_t TC_CoreAFunc_SP_usr_orig;
static uint32_t TC_CoreAFunc_SP_usr_sp;
static uint32_t TC_CoreAFunc_SP_usr_result;

void TC_CoreAFunc_SP_usr(void) {
  TC_CoreAFunc_SP_usr_orig = __get_SP_usr();

  TC_CoreAFunc_SP_usr_sp = TC_CoreAFunc_SP_usr_orig + 0x12345678U;
  __set_SP(TC_CoreAFunc_SP_usr_sp);
  TC_CoreAFunc_SP_usr_result = __get_SP_usr();

  __set_SP(TC_CoreAFunc_SP_usr_orig);

  ASSERT_TRUE(TC_CoreAFunc_SP_usr_result == TC_CoreAFunc_SP_usr_sp);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_FPEXC(void) {
  uint32_t fpexc = __get_FPEXC();
  __set_FPEXC(fpexc);

  ASSERT_TRUE(fpexc == __get_FPEXC());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_ACTLR(void) {
  uint32_t actlr = __get_ACTLR();
  __set_ACTLR(actlr);

  ASSERT_TRUE(actlr == __get_ACTLR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_CPACR(void) {
  uint32_t cpacr = __get_CPACR();
  __set_CPACR(cpacr);

  ASSERT_TRUE(cpacr == __get_CPACR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_DFSR(void) {
  uint32_t dfsr = __get_DFSR();
  __set_DFSR(dfsr);

  ASSERT_TRUE(dfsr == __get_DFSR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_IFSR(void) {
  uint32_t ifsr = __get_IFSR();
  __set_IFSR(ifsr);

  ASSERT_TRUE(ifsr == __get_IFSR());
}

/*0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_ISR(void) {
  uint32_t isr = __get_ISR();

  ASSERT_TRUE(isr == __get_ISR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_CBAR(void) {
  uint32_t cbar = __get_CBAR();

  ASSERT_TRUE(cbar == __get_CBAR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_TTBR0(void) {
  uint32_t ttbr0 = __get_TTBR0();
  __set_TTBR0(ttbr0);

  ASSERT_TRUE(ttbr0 == __get_TTBR0());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_DACR(void) {
  uint32_t dacr = __get_DACR();
  __set_DACR(dacr);

  ASSERT_TRUE(dacr == __get_DACR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_SCTLR(void) {
  uint32_t sctlr = __get_SCTLR();
  __set_SCTLR(sctlr);

  ASSERT_TRUE(sctlr == __get_SCTLR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_ACTRL(void) {
  uint32_t actrl = __get_ACTRL();
  __set_ACTRL(actrl);

  ASSERT_TRUE(actrl == __get_ACTRL());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_MPIDR(void) {
  uint32_t mpidr = __get_MPIDR();

  ASSERT_TRUE(mpidr == __get_MPIDR());
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
static uint8_t vectorRAM[32U] __attribute__((aligned(32U)));

void TC_CoreAFunc_VBAR(void) {
  uint32_t vbar = __get_VBAR();
  
  memcpy(vectorRAM, (void*)vbar, sizeof(vectorRAM));
  
  __set_VBAR((uint32_t)vectorRAM);
  ASSERT_TRUE(((uint32_t)vectorRAM) == __get_VBAR());
  
  __set_VBAR(vbar);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_CoreAFunc_MVBAR(void) {
  uint32_t mvbar = __get_MVBAR();
  
  memcpy(vectorRAM, (void*)mvbar, sizeof(vectorRAM));
  
  __set_MVBAR((uint32_t)vectorRAM);
  ASSERT_TRUE(((uint32_t)vectorRAM) == __get_MVBAR());
  
  __set_MVBAR(mvbar);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

void TC_CoreAFunc_FPU_Enable(void) {
  uint32_t fpexc = __get_FPEXC();
  __set_FPEXC(fpexc & ~0x40000000u); // disable FPU
  
  uint32_t cp15;
  __get_CP(15, 0, cp15, 1, 0, 2);
  
  cp15 &= ~0x00F00000u;
  __set_CP(15, 0, cp15, 1, 0, 2); // disable FPU access
  
  __FPU_Enable();
    
  __get_CP(15, 0, cp15, 1, 0, 2);
  ASSERT_TRUE((cp15 & 0x00F00000u) == 0x00F00000u);

  fpexc = __get_FPEXC();  
  ASSERT_TRUE((fpexc & 0x40000000u) == 0x40000000u);
}


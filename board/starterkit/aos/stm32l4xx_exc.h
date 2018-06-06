#ifndef __STM32_EXC_H
#define __STM32_EXC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "k_api.h"
#include "soc_init.h"

#define exc_print 1

typedef struct task_context_s 
{
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t xPSR;
#if (FPU_AVL > 0)
    uint32_t S0;
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
    uint32_t S4;
    uint32_t S5;
    uint32_t S6;    
    uint32_t S7;
    uint32_t S8;
    uint32_t S9;
    uint32_t S10;
    uint32_t S11;
    uint32_t S12;
    uint32_t S13;
    uint32_t S14;    
    uint32_t S15;
    uint32_t FSCR;
#endif
} kcontext_t;


#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler */
  #define __STATIC_INLINE  static inline

#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline

#elif defined ( __TMS470__ )
  #define __ASM            __asm                                      /*!< asm keyword for TI CCS Compiler */
  #define __STATIC_INLINE  static inline

#elif defined ( __TASKING__ )
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler */
  #define __STATIC_INLINE  static inline

#elif defined ( __CSMC__ )
  #define __packed
  #define __ASM            _asm                                      /*!< asm keyword for COSMIC Compiler */
  #define __INLINE         inline                                    /*!< inline keyword for COSMIC Compiler. Use -pc99 on compile line */
  #define __STATIC_INLINE  static inline

#else
  #error Unknown compiler
#endif

#if   defined ( __CC_ARM )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name","#reg"\r\n")
#define ASM_MRS(name,reg)         __ASM volatile("MRS "#reg","#name"\r\n")

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name","#reg"\r\n")
#define ASM_MRS(name,reg)         __ASM volatile("MRS "#reg","#name"\r\n")

#elif defined ( __GNUC__ )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name",%0\r\n":"=r"(reg))
#define ASM_MRS(name,reg)         __ASM volatile("MRS %0,"#name"\r\n":"=r"(reg))

#elif defined ( __ICCARM__ )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name",%0":"=r"(reg))
#define ASM_MRS(name,reg)         __ASM volatile("MRS %0,"#name:"=r"(reg))

#endif


extern int g_msp_value;
extern int g_psp_value;
extern int g_hardfault_num;
extern void print_stack_context(kcontext_t *addr,int len);
extern void print_stack_mem(void* addr,int len);
extern void print_sys_reg(void);
extern void print_sys_variable(void);
extern  void cpu_hardfault_handle(void);


__INLINE void cpu_hardfault_handle(void)
{
    /*msp\psp get and print*/
    ASM_MRS(msp,g_msp_value);
    
    ASM_MRS(psp,g_psp_value);

    print_stack_context((kcontext_t *)g_msp_value,sizeof(kcontext_t));
    printf("\r\npsp context:");
    print_stack_mem((void *)g_psp_value,200);

    /*once or mutitimes*/
    g_hardfault_num++;
    printf("cpu hard fault num:%d\r\n",g_hardfault_num);

    /*base sys reg*/
    print_sys_reg();

    /*cur info    task info  sp over*/
    print_sys_variable();

    /*trace info can be get by cli cmd below*/
    
    /*cli*/ 
    //cli_main(NULL);
}


#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXC_H */

#ifndef COMPILER_HAL_H
#define COMPILER_HAL_H

#if   defined ( __CC_ARM )
  #define __ASM            __asm
  #define __INLINE         __inline
  #define __STATIC_INLINE  static __inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm
  #define __INLINE         inline
  #define __STATIC_INLINE  static inline

#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline
#endif


#endif /* end of include guard: COMPILER_HAL_H */

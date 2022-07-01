#ifndef __ARM_COMPAT__H
#define __ARM_COMPAT__H

#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif
// Reverse the bit order in a 32-bit word.
__STATIC_FORCEINLINE uint32_t __RBIT(
  uint32_t i )
{
  i = ( ( i & 0x55555555 ) << 1 ) | ( ( i >> 1 ) & 0x55555555 );
  i = ( ( i & 0x33333333 ) << 2 ) | ( ( i >> 2 ) & 0x33333333 );
  i = ( ( i & 0x0f0f0f0f ) << 4 ) | ( ( i >> 4 ) & 0x0f0f0f0f );
  i = ( i << 24 ) | ( ( i & 0xff00 ) << 8 ) //
    | ( ( i >> 8 ) & 0xff00 ) | ( i >> 24 );
  return i;
}

// Reverse byte order in each halfword independently
// converts 16-bit big-endian data into little-endian data
// or 16-bit little-endian data into big-endian data
__STATIC_FORCEINLINE short __REV16(
  short s )
{
  return ( s << 8 ) | ( s >> 8 );
}

// Reverse byte order in a word
// converts 32-bit big-endian data into little-endian data
// or 32-bit little-endian data into big-endian data.
__STATIC_FORCEINLINE uint32_t __REV32(
  uint32_t i )
{
  return ( i & 0x000000FFU ) << 24 | ( i & 0x0000FF00U ) << 8
    | ( i & 0x00FF0000U ) >> 8 | ( i & 0xFF000000U ) >> 24;
}

__STATIC_FORCEINLINE uint32_t __SSUB16(uint32_t op1, uint32_t op2)
{
  return ((op1 & 0xFFFF0000) - (op2 & 0xFFFF0000)) | ((op1 - op2) & 0xFFFF);
}

__STATIC_FORCEINLINE uint32_t __UXTB_RORn(uint32_t op1, uint32_t rotate)
{
  return (op1 >> rotate) & 0xFF;
}

#define __CLZ             (uint8_t)__builtin_clz

#define __PKHBT(ARG1,ARG2,ARG3)          ( ((((uint32_t)(ARG1))          ) & 0x0000FFFFUL) |  \
                                           ((((uint32_t)(ARG2)) << (ARG3)) & 0xFFFF0000UL)  )

#define __PKHTB(ARG1,ARG2,ARG3)          ( ((((uint32_t)(ARG1))          ) & 0xFFFF0000UL) |  \
                                           ((((uint32_t)(ARG2)) >> (ARG3)) & 0x0000FFFFUL)  )

__STATIC_FORCEINLINE uint32_t __SMLAD (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;
  uint16_t *op1_s = (uint16_t *) &op1, *op2_s = (uint16_t *) &op2;
  
  result = op1_s[0] * op2_s[0];
  result += op1_s[1] * op2_s[1];
  result += op3;
  
  return result;
}

__STATIC_FORCEINLINE uint32_t __SMUAD  (uint32_t op1, uint32_t op2)
{
  uint32_t result;
  uint16_t *op1_s = (uint16_t *) &op1, *op2_s = (uint16_t *) &op2;
  
  result = op1_s[0] * op2_s[0] + op1_s[1] * op2_s[1];

  return result;  
}

__STATIC_FORCEINLINE uint32_t __QADD16(uint32_t op1, uint32_t op2)
{
  uint32_t result;
  uint16_t *op1_s = (uint16_t *) &op1, *op2_s = (uint16_t *) &op2, *result_s = (uint16_t *) &result;
  
  result_s[0] = op1_s[0] + op2_s[0];
  result_s[1] = op1_s[1] + op2_s[1];
  return(result);
}

__STATIC_FORCEINLINE uint32_t __SMLADX (uint32_t op1, uint32_t op2, uint32_t op3)
{
  uint32_t result;
  uint16_t *op1_s = (uint16_t *) &op1, *op2_s = (uint16_t *) &op2;
  
  result = op1_s[0] * op2_s[1];
  result += op1_s[1] * op2_s[0];
  result += op3;
  
  return result;
}

#define __USAT_ASR(ARG1,ARG2,ARG3) \
({                          \
  uint32_t __RES, __ARG1 = (ARG1), __ARG2 = 1<<ARG2; \
  __ARG1 = __ARG1>>ARG3; \
  if(__ARG1 >= __ARG2) __RES = __ARG2; \
  else __RES = __ARG1;  \
  __RES; \
 })

extern float xtensa_cos_f32(float x);
extern float arm_sin_f32(float x);

#endif

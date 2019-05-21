#ifndef _BK_SEC_H_
#define _BK_SEC_H_

#if (CFG_SOC_NAME == SOC_BK7221U)

#define SECURITY_BASE                  (0x00806000UL)

#define SECURITY_AES_KEY0              (SECURITY_BASE + 0x00 * 4)
#define SECURITY_AES_KEY0_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY0                   (*((volatile unsigned long *) SECURITY_AES_KEY0))


#define SECURITY_AES_KEY1              (SECURITY_BASE + 0x01 * 4)
#define SECURITY_AES_KEY1_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY1                   (*((volatile unsigned long *) SECURITY_AES_KEY1))


#define SECURITY_AES_KEY2              (SECURITY_BASE + 0x02 * 4)
#define SECURITY_AES_KEY2_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY2                   (*((volatile unsigned long *) SECURITY_AES_KEY2))


#define SECURITY_AES_KEY3              (SECURITY_BASE + 0x03 * 4)
#define SECURITY_AES_KEY3_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY3                   (*((volatile unsigned long *) SECURITY_AES_KEY3))


#define SECURITY_AES_KEY4              (SECURITY_BASE + 0x04 * 4)
#define SECURITY_AES_KEY4_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY4                   (*((volatile unsigned long *) SECURITY_AES_KEY4))


#define SECURITY_AES_KEY5              (SECURITY_BASE + 0x05 * 4)
#define SECURITY_AES_KEY5_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY5                   (*((volatile unsigned long *) SECURITY_AES_KEY5))


#define SECURITY_AES_KEY6              (SECURITY_BASE + 0x06 * 4)
#define SECURITY_AES_KEY6_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY6                   (*((volatile unsigned long *) SECURITY_AES_KEY6))


#define SECURITY_AES_KEY7              (SECURITY_BASE + 0x07 * 4)
#define SECURITY_AES_KEY7_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_KEY7                   (*((volatile unsigned long *) SECURITY_AES_KEY7))


#define SECURITY_AES_KEY_X(x)          (SECURITY_BASE + (x) * 4)
#define SECURITY_AES_KEY_X_MASK             0xFFFFFFFFUL
//#define SECURITY_AES_KEY_X(x)               (*((volatile unsigned long *) SECURITY_AES_KEY_X(x)))


#define SECURITY_AES_CTRL              (SECURITY_BASE + 0x08 * 4)
#define SECURITY_AES_CTRL_MASK              0xFFFFFFFFUL
//#define SECURITY_AES_CTRL                   (*((volatile unsigned long *) SECURITY_AES_CTRL))


#define SECURITY_AES_STATUS            (SECURITY_BASE + 0x09 * 4)
#define SECURITY_AES_STATUS_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_STATUS                 (*((volatile unsigned long *) SECURITY_AES_STATUS))


#define SECURITY_AES_CONFIG            (SECURITY_BASE + 0x0A * 4)
#define SECURITY_AES_CONFIG_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_CONFIG                 (*((volatile unsigned long *) SECURITY_AES_CONFIG))


#define SECURITY_AES_BLOCK0            (SECURITY_BASE + 0x0C * 4)
#define SECURITY_AES_BLOCK0_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_BLOCK0                 (*((volatile unsigned long *) SECURITY_AES_BLOCK0))


#define SECURITY_AES_BLOCK1            (SECURITY_BASE + 0x0D * 4)
#define SECURITY_AES_BLOCK1_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_BLOCK1                 (*((volatile unsigned long *) SECURITY_AES_BLOCK1))


#define SECURITY_AES_BLOCK2            (SECURITY_BASE + 0x0E * 4)
#define SECURITY_AES_BLOCK2_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_BLOCK2                 (*((volatile unsigned long *) SECURITY_AES_BLOCK2))


#define SECURITY_AES_BLOCK3            (SECURITY_BASE + 0x0F * 4)
#define SECURITY_AES_BLOCK3_MASK            0xFFFFFFFFUL
//#define SECURITY_AES_BLOCK3                 (*((volatile unsigned long *) SECURITY_AES_BLOCK3))


#define SECURITY_AES_BLOCK_X(x)        (SECURITY_BASE + (0x0C + (x)) * 4)
#define SECURITY_AES_BLOCK_X_MASK           0xFFFFFFFFUL
//#define SECURITY_AES_BLOCK_X(x)             (*((volatile unsigned long *) SECURITY_AES_BLOCK_X(x)))


#define SECURITY_AES_RESULT0           (SECURITY_BASE + 0x10 * 4)
#define SECURITY_AES_RESULT0_MASK           0xFFFFFFFFUL
//#define SECURITY_AES_RESULT0                (*((volatile unsigned long *) SECURITY_AES_RESULT0))


#define SECURITY_AES_RESULT1           (SECURITY_BASE + 0x11 * 4)
#define SECURITY_AES_RESULT1_MASK           0xFFFFFFFFUL
//#define SECURITY_AES_RESULT1                (*((volatile unsigned long *) SECURITY_AES_RESULT1))


#define SECURITY_AES_RESULT2           (SECURITY_BASE + 0x12 * 4)
#define SECURITY_AES_RESULT2_MASK           0xFFFFFFFFUL
//#define SECURITY_AES_RESULT2                (*((volatile unsigned long *) SECURITY_AES_RESULT2))


#define SECURITY_AES_RESULT3           (SECURITY_BASE + 0x13 * 4)
#define SECURITY_AES_RESULT3_MASK           0xFFFFFFFFUL
//#define SECURITY_AES_RESULT3                (*((volatile unsigned long *) SECURITY_AES_RESULT3))


#define SECURITY_AES_RESULT_X(x)       (SECURITY_BASE + (0x10 + (x)) * 4)
#define SECURITY_AES_RESULT_X_MASK          0xFFFFFFFFUL
//#define SECURITY_AES_RESULT_X(x)            (*((volatile unsigned long *) SECURITY_AES_RESULT_X(x)))



#define SECURITY_SHA_BLOCK31           (SECURITY_BASE + 0x40 * 4)
#define SECURITY_SHA_BLOCK31_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK31                (*((volatile unsigned long *) SECURITY_SHA_BLOCK31))


#define SECURITY_SHA_BLOCK30           (SECURITY_BASE + 0x41 * 4)
#define SECURITY_SHA_BLOCK30_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK30                (*((volatile unsigned long *) SECURITY_SHA_BLOCK30))


#define SECURITY_SHA_BLOCK29           (SECURITY_BASE + 0x42 * 4)
#define SECURITY_SHA_BLOCK29_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK29                (*((volatile unsigned long *) SECURITY_SHA_BLOCK29))


#define SECURITY_SHA_BLOCK28           (SECURITY_BASE + 0x43 * 4)
#define SECURITY_SHA_BLOCK28_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK28                (*((volatile unsigned long *) SECURITY_SHA_BLOCK28))


#define SECURITY_SHA_BLOCK27           (SECURITY_BASE + 0x44 * 4)
#define SECURITY_SHA_BLOCK27_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK27                (*((volatile unsigned long *) SECURITY_SHA_BLOCK27))


#define SECURITY_SHA_BLOCK0            (SECURITY_BASE + 0x5F * 4)
#define SECURITY_SHA_BLOCK0_MASK            0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK0                 (*((volatile unsigned long *) SECURITY_SHA_BLOCK0))


#define SECURITY_SHA_BLOCK_X(x)        (SECURITY_BASE + (0x5F - (x)) * 4)
#define SECURITY_SHA_BLOCK_X_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_BLOCK_X(x)             (*((volatile unsigned long *) SECURITY_SHA_BLOCK_X(x)))


#define SECURITY_SHA_DIGEST15          (SECURITY_BASE + 0x60 * 4)
#define SECURITY_SHA_DIGEST15_MASK          0xFFFFFFFFUL
//#define SECURITY_SHA_DIGEST15               (*((volatile unsigned long *) SECURITY_SHA_BLOCK15))


#define SECURITY_SHA_DIGEST0           (SECURITY_BASE + 0x6F * 4)
#define SECURITY_SHA_DIGEST0_MASK           0xFFFFFFFFUL
//#define SECURITY_SHA_DIGEST0                (*((volatile unsigned long *) SECURITY_SHA_BLOCK0))


#define SECURITY_SHA_DIGEST_X(x)       (SECURITY_BASE + (0x6F - (x)) * 4)
#define SECURITY_SHA_DIGEST_X_MASK          0xFFFFFFFFUL
//#define SECURITY_SHA_DIGEST_X(x)            (*((volatile unsigned long *) SECURITY_SHA_DIGEST_X(x)))


#define SECURITY_SHA_CTRL              (SECURITY_BASE + 0x70 * 4)
#define SECURITY_SHA_CTRL_MASK              0xFFFFFFFFUL
//#define SECURITY_SHA_CTRL                   (*((volatile unsigned long *) SECURITY_SHA_CTRL))


#define SECURITY_SHA_STATUS            (SECURITY_BASE + 0x71 * 4)
#define SECURITY_SHA_STATUS_MASK            0xFFFFFFFFUL
//#define SECURITY_SHA_STATUS                 (*((volatile unsigned long *) SECURITY_SHA_STATUS))


#define SECURITY_SHA_CONFIG            (SECURITY_BASE + 0x72 * 4)
#define SECURITY_SHA_CONFIG_MASK            0xFFFFFFFFUL
//#define SECURITY_SHA_CONFIG                 (*((volatile unsigned long *) SECURITY_SHA_CONFIG))


#define SECURITY_RSA_CONFIG            (SECURITY_BASE + 0x80 * 4)
#define SECURITY_RSA_CONFIG_MASK            0xFFFFFFFFUL
//#define SECURITY_RSA_CONFIG                 (*((volatile unsigned long *) SECURITY_RSA_CONFIG))


#define SECURITY_RSA_STATE             (SECURITY_BASE + 0x81 * 4)
#define SECURITY_RSA_STATE_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_STATE                  (*((volatile unsigned long *) SECURITY_RSA_STATE))


#define SECURITY_RSA_N0_PRIME_L        (SECURITY_BASE + 0x82 * 4)
#define SECURITY_RSA_N0_PRIME_L_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_N0_PRIME_L             (*((volatile unsigned long *) SECURITY_RSA_N0_PRIME_L))


#define SECURITY_RSA_N0_PRIME_H        (SECURITY_BASE + 0x83 * 4)
#define SECURITY_RSA_N0_PRIME_H_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_N0_PRIME_H             (*((volatile unsigned long *) SECURITY_RSA_N0_PRIME_H))


#define SECURITY_RSA_LENGTH            (SECURITY_BASE + 0x84 * 4)
#define SECURITY_RSA_LENGTH_MASK            0xFFFFFFFFUL
//#define SECURITY_RSA_LENGTH                 (*((volatile unsigned long *) SECURITY_RSA_LENGTH))


#define SECURITY_RSA_N_RST             (SECURITY_BASE + 0x88 * 4)
#define SECURITY_RSA_N_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_N_RST                  (*((volatile unsigned long *) SECURITY_RSA_N_RST))


#define SECURITY_RSA_N_MEM_DATA        (SECURITY_BASE + 0x89 * 4)
#define SECURITY_RSA_N_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_N_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_N_MEM_DATA))


#define SECURITY_RSA_E_RST             (SECURITY_BASE + 0x8A * 4)
#define SECURITY_RSA_E_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_E_RST                  (*((volatile unsigned long *) SECURITY_RSA_E_RST))


#define SECURITY_RSA_E_MEM_DATA        (SECURITY_BASE + 0x8B * 4)
#define SECURITY_RSA_E_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_E_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_E_MEM_DATA))


#define SECURITY_RSA_M_RST             (SECURITY_BASE + 0x8C * 4)
#define SECURITY_RSA_M_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_M_RST                  (*((volatile unsigned long *) SECURITY_RSA_M_RST))


#define SECURITY_RSA_M_MEM_DATA        (SECURITY_BASE + 0x8D * 4)
#define SECURITY_RSA_M_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_M_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_M_MEM_DATA))


#define SECURITY_RSA_R_RST             (SECURITY_BASE + 0x8E * 4)
#define SECURITY_RSA_R_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_R_RST                  (*((volatile unsigned long *) SECURITY_RSA_R_RST))


#define SECURITY_RSA_R_MEM_DATA        (SECURITY_BASE + 0x8F * 4)
#define SECURITY_RSA_R_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_R_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_R_MEM_DATA))


#define SECURITY_RSA_T_RST             (SECURITY_BASE + 0x90 * 4)
#define SECURITY_RSA_T_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_T_RST                  (*((volatile unsigned long *) SECURITY_RSA_T_RST))


#define SECURITY_RSA_T_MEM_DATA        (SECURITY_BASE + 0x91 * 4)
#define SECURITY_RSA_T_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_T_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_T_MEM_DATA))


#define SECURITY_RSA_C_RST             (SECURITY_BASE + 0x92 * 4)
#define SECURITY_RSA_C_RST_MASK             0xFFFFFFFFUL
//#define SECURITY_RSA_C_RST                  (*((volatile unsigned long *) SECURITY_RSA_C_RST))


#define SECURITY_RSA_C_MEM_DATA        (SECURITY_BASE + 0x93 * 4)
#define SECURITY_RSA_C_MEM_DATA_MASK        0xFFFFFFFFUL
//#define SECURITY_RSA_C_MEM_DATA             (*((volatile unsigned long *) SECURITY_RSA_C_MEM_DATA))

#endif

#endif


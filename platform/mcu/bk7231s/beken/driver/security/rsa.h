
#ifndef _BK_RSA_H_
#define _BK_RSA_H_


#if (CFG_SOC_NAME == SOC_BK7221U)

#include "security_reg.h"


#define RSA_512_LEN         16
#define RSA_2048_LEN        64
#define RSA_4096_LEN        128


enum RSA_MODE       // by gwf
{
    RSA512      = 0,
    RSA2048     = 1,
    RSA4096     = 2,
};



//----------------------------------------------
// SECURITY RSA driver description
//----------------------------------------------
typedef struct
{
    unsigned long           rsa_N0_prime_L_data;
    unsigned long           rsa_N0_prime_H_data;
    unsigned long          *p_rsa_R_data;
    unsigned long          *p_rsa_T_data;
    unsigned long          *p_rsa_E_data;
    unsigned long          *p_rsa_N_data;
    unsigned long          *p_rsa_M_data;
    //    unsigned long          *p_rsa_C_data;

    /* mode:     SECURITY RSA mode
        RSA512      = 0,
        RSA2048     = 1,
        RSA4096     = 2,
     */
    unsigned char           mode;

    /* mode:     SECURITY RSA compute
        compute_C   = 0,
        compute_T   = 1,
        compute_R   = 2,
     */
    unsigned char           compute;
} SECURITY_RSA_DRV_DESC;

#endif

#endif


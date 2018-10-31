#ifndef __PCACHE_REGISTERS_H_
#define __PCACHE_REGISTERS_H_

#if defined(__PIC32MX__) 
typedef struct pcache_regs {
    __CHECONbits_t CHECON;
    volatile unsigned int CHECONCLR;
    volatile unsigned int CHECONSET;
    volatile unsigned int CHECONINV;
    __CHEACCbits_t CHEACC;
    volatile unsigned int CHEACCCLR;
    volatile unsigned int CHEACCSET;
    volatile unsigned int CHEACCINV;
    __CHETAGbits_t CHETAG;
    volatile unsigned int CHETAGCLR;
    volatile unsigned int CHETAGSET;
    volatile unsigned int CHETAGINV;
    volatile unsigned int CHEMSK;
    volatile unsigned int DONTUSE1[3];
    volatile unsigned int CHEW0;
    volatile unsigned int DONTUSE2[3];
    volatile unsigned int CHEW1;
    volatile unsigned int DONTUSE3[3];
    volatile unsigned int CHEW2;
    volatile unsigned int DONTUSE4[3];
    volatile unsigned int CHEW3;
    volatile unsigned int DONTUSE5[3];
    volatile unsigned int CHELRU;
    volatile unsigned int DONTUSE6[3];
    volatile unsigned int CHEHIT;
    volatile unsigned int DONTUSE7[3];
    volatile unsigned int CHEMIS;
    volatile unsigned int DONTUSE8[3];
    volatile unsigned int DONTUSE9[4]; /* not used */
    volatile unsigned int CHEPFABT;
    volatile unsigned int DONTUSE10[3];
} pcache_register_t;
#endif

#if defined(__PIC32MK__) 
typedef struct pcache_regs {
    __CHECONbits_t CHECON;
    volatile unsigned int CHECONCLR;
    volatile unsigned int CHECONSET;
    volatile unsigned int CHECONINV;
    volatile unsigned int DONTUSE1[4];
    volatile unsigned int CHEHIT;
    volatile unsigned int DONTUSE2[3];
    volatile unsigned int CHEMIS;
} pcache_register_t;
#endif

#if defined(__PIC32MZ__)
typedef struct pfm_regs {
    __PRECONbits_t PRECON;
    volatile unsigned int PRECONCLR;
    volatile unsigned int PRECONSET;
    volatile unsigned int PRECONINV;
    __PRESTATbits_t PRESTAT;
    volatile unsigned int PRESTATCLR;
    volatile unsigned int PRESTATSET;
    volatile unsigned int PRESTATINV;
} pfm_register_t;
#endif

#endif /* __PCACHE_REGISTERS_H_ */

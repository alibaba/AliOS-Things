#ifndef  BSP_H
#define  BSP_H

#define  CLK_GEN_BASE_ADDR             ((uint32_t )0x000FFF00u)
#define  REG_CLK_GEN_CMC               (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA0u))
#define  REG_CLK_GEN_CKC               (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA4u))
#define  REG_CLK_GEN_CSC               (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA1u))
#define  REG_CLK_GEN_OSTC              (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA2u))
#define  REG_CLK_GEN_OSTS              (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA3u))
#define  REG_CLK_GEN_OSMC              (*(volatile uint8_t __far*)(0xF0000000 + 0xF3u))
#define  REG_CLK_GEN_HOCODIV           (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA8u))
#define  REG_CLK_GEN_HIOTRM            (*(volatile uint8_t __far*)(CLK_GEN_BASE_ADDR + 0xA0u))

#define  REG_PORT_BASE_ADDR_00         ((uint32_t )0x000F0000u)

#define  REG_P(n)                      (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0xFF00 + n ))
#define  REG_PM(n)                     (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0xFF20 + n ))
#define  REG_PU(n)                     (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0x30 + n ))
#define  REG_PIM(n)                    (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0x40 + n ))
#define  REG_POM(n)                    (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0x50 + n ))
#define  REG_PMC(n)                    (*(volatile uint8_t __far*)(REG_PORT_BASE_ADDR_00 + 0x60 + n ))

#define  REG_PER0                      (*(volatile uint8_t __far*)0x000F00F0u)

#define  REG_TPS0                      (*(volatile uint16_t __far*)0x000F01B6u)
#define  REG_TMR00                     (*(volatile uint16_t __far*)0x000F0190u)
#define  REG_TDR00                     (*(volatile uint16_t __far*)0x000FFF18u)
#define  REG_TOE0                      (*(volatile uint16_t __far*)0x000F01BAu)
#define  REG_TO0                       (*(volatile uint16_t __far*)0x000F01B8u)
#define  REG_TOM0                      (*(volatile uint16_t __far*)0x000F01BEu)
#define  REG_TOL0                      (*(volatile uint16_t __far*)0x000F01BCu)	
#define  REG_TS0                       (*(volatile uint16_t __far*)0x000F01B2u)

#define  REG_PR01L                     (*(volatile uint8_t __far*)0x000FFFEAu)
#define  REG_PR11L                     (*(volatile uint8_t __far*)0x000FFFEEu)

#define  REG_IF1L                      (*(volatile uint8_t __far*)0x000FFFE2u)
#define  REG_MK1L                      (*(volatile uint8_t __far*)0x000FFFE6u)

#define  BIT_MASK(bit_mask, bit_shift)         ((bit_mask) << (bit_shift))
#define  BIT_IS_SET(val, mask)                 ((((mask)  !=  0u)  && \
                                                      (((val) & (mask)) == (mask))) ? (1) : (0))

#define  BIT_NONE                               0x00u
#define  BIT_00                                 0x01u
#define  BIT_01                                 0x02u
#define  BIT_02                                 0x04u
#define  BIT_03                                 0x08u
#define  BIT_04                                 0x10u
#define  BIT_05                                 0x20u
#define  BIT_06                                 0x40u
#define  BIT_07                                 0x80u                                                            
#define  BIT_08                                 0x0100u

#define  REG_PIOR                               (*(volatile uint8_t __far*)0x000F0077u)
#define  REG_CRC0CTL                            (*(volatile uint8_t __far*)0x000F02F0u)
#define  REG_IAWCTL                             (*(volatile uint8_t __far*)0x000F0078u)
#define  REG_PMS                                (*(volatile uint8_t __far*)0x000F007Bu)

#define  BIT_CKC_MCM0                           BIT_04
#define  BIT_CKC_MCS                            BIT_05
#define  BIT_CSC_MSTOP                          BIT_06
#define  BIT_CSC_XTSTOP                         BIT_06
#define  BIT_CKC_CSS                            BIT_06
#define  BIT_CSC_HIOSTOP                        BIT_00
#define  BIT_CMC_AMPH                           BIT_00

#define  BIT_MK1L_TMMK00                        BIT_04
#define  BIT_IF1L_TMIF00                        BIT_04
#define  BIT_PRO1L_TMPR000                      BIT_04
#define  BIT_PR11L_TMPR100                      BIT_04
 
#define  OSC_HSCLK_INPUT                        BIT_NONE
#define  OSC_SUBCLK_INPUT                       BIT_NONE
#define  OSC_XT1_DEFAULT                        BIT_NONE
#define  OSC_FRQ_DEFAULT                        BIT_NONE

#define  OSCSTAB_SEL_26_21ms                    BIT_MASK(7, 0)
#define  OSCSTAB_STA                            0xFFu

#define  CLK_SUBINHALT_ON                       BIT_NONE
#define  CLK_FSUB_ON                            BIT_NONE


#define  OSCSUB_WAITTIME                        160

#define  CLKFREQ_DIV                            512

#define  FSUB_CLKRATE                           32768
#define  LOWSPEED_CLKRATE                       15000

#define  FMX_CLKRATE                            12000000

#define  CLKRATE32                              32000000
#define  CLKRATE24                              24000000
#define  CLKRATE16                              16000000
#define  CLKRATE12                              12000000
#define  CLKRATE08                              8000000
#define  CLKRATE04                              4000000
#define  CLKRATE01                              1000000

#define  FRQSEL_1MHz                            0x0D
#define  FRQSEL_4MHz                            0x0B
#define  FRQSEL_8MHz                            0x0A
#define  FRQSEL_12MHz                           0x01
#define  FRQSEL_16MHz                           0x09
#define  FRQSEL_24MHz                           0x00
#define  FRQSEL_32MHz                           0x08

#define  PIOR_REDIRECT_DIS                      0x00
#define  CRC0CTL_FLASH_OFF                      0x00
#define  IWACTL_INVALID_MEM_DET_OFF             0x00
#define  PMS_PORT_MODE_READ                     0x00

#define  PER0_ENABLE_CLOCK                      0x0001
#define  TPS0_SET_CLOCK_FRQ                     0x0000
#define  TMR00_SET_CK00_SRC                     0x0000
#define  TS0_EN_TMR_OUTPUT                      0x0001
#define  OPTBYTE2_VAL                           0xE8u

#endif


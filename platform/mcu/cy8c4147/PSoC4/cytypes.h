/***************************************************************************//**
* \file cytypes.h
* \version 5.70
*
* \brief CyTypes provides register access macros and approved types for use in
* firmware.
*
* \note Due to endiannesses of the hardware and some compilers, the register
* access macros for big endian compilers use some library calls to arrange
* data the correct way.
*
* Register Access macros and functions perform their operations on an
* input of the type pointer to void.  The arguments passed to it should be
* pointers to the type associated with the register size.
* (i.e. a "uint8 *" shouldn't be passed to obtain a 16-bit register value)
*
********************************************************************************
* \copyright
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYTYPES_H)
#define CY_BOOT_CYTYPES_H

#if defined(__C51__)
    #include <intrins.h>
#endif  /* (__C51__) */

/* ARM and C99 or later */
#if defined(__GNUC__) || defined(__ARMCC_VERSION) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #include <stdint.h>
#endif  /* (__GNUC__) || defined(__ARMCC_VERSION) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) */

#include "cyfitter.h"


#if defined( __ICCARM__ )
    /* Suppress warning for multiple volatile variables in an expression. */
    /* This is common in component code and usage is not order dependent. */
    #pragma diag_suppress=Pa082
#endif  /* defined( __ICCARM__ ) */


/***************************************
* Conditional Compilation Parameters
***************************************/


/*******************************************************************************
* FAMILY encodes the overall architectural family
*******************************************************************************/
#define CY_PSOC3 (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3)
#define CY_PSOC4 (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4)
#define CY_PSOC5 (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5)


/*******************************************************************************
* MEMBER encodes both the family and the detailed architecture
*******************************************************************************/
#ifdef CYDEV_CHIP_MEMBER_4D
    #define CY_PSOC4_4000   (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4D)
#else
    #define CY_PSOC4_4000   (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4D */

#define CY_PSOC4_4100       (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4A)
#define CY_PSOC4_4200       (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4A)

#ifdef CYDEV_CHIP_MEMBER_4F
    #define CY_PSOC4_4100BL (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4F)
    #define CY_PSOC4_4200BL (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4F)
#else
    #define CY_PSOC4_4100BL (0u != 0u)
    #define CY_PSOC4_4200BL (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4F */

#ifdef CYDEV_CHIP_MEMBER_4M
    #define CY_PSOC4_4100M (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4M)
    #define CY_PSOC4_4200M (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4M)
#else
    #define CY_PSOC4_4100M (0u != 0u)
    #define CY_PSOC4_4200M (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4M */

#ifdef CYDEV_CHIP_MEMBER_4H
    #define CY_PSOC4_4200D (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4H)
#else
    #define CY_PSOC4_4200D (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4H */

#ifdef CYDEV_CHIP_MEMBER_4L
    #define CY_PSOC4_4200L (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4L)
#else
    #define CY_PSOC4_4200L (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4L */

#ifdef CYDEV_CHIP_MEMBER_4U
    #define CY_PSOC4_4000U (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4U)
#else
    #define CY_PSOC4_4000U (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4U */

#ifdef CYDEV_CHIP_MEMBER_4J
    #define CY_PSOC4_4000S (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4J)
#else
    #define CY_PSOC4_4000S (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4J */

#ifdef CYDEV_CHIP_MEMBER_4K
    #define CY_PSOC4_4100S (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4K)
#else
    #define CY_PSOC4_4100S (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4K */

#ifdef CYDEV_CHIP_MEMBER_4I
    #define CY_PSOC4_4400 (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4I)
#else
    #define CY_PSOC4_4400 (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4I */

#ifdef CYDEV_CHIP_MEMBER_4E
    #define CY_CCG2 (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4E)
#else
    #define CY_CCG2 (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4E */

#ifdef CYDEV_CHIP_MEMBER_4O
    #define CY_CCG3 (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4O)
#else
    #define CY_CCG3 (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4O */

#ifdef CYDEV_CHIP_MEMBER_4R
    #define CY_CCG3PA (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4R)
#else
    #define CY_CCG3PA (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4R */

#ifdef CYDEV_CHIP_MEMBER_4N
    #define CY_CCG4 (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4N)
#else
    #define CY_CCG4 (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4N */

#ifdef CYDEV_CHIP_MEMBER_4S
    #define CY_CCG5 (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4S)
#else
    #define CY_CCG5 (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4S */

#ifdef CYDEV_CHIP_MEMBER_4P
    #define CY_PSOC4_4100BLII (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4P)
    #define CY_PSOC4_4200BLII (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4P)
#else
    #define CY_PSOC4_4100BLII (0u != 0u)
    #define CY_PSOC4_4200BLII (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4P */

#ifdef CYDEV_CHIP_MEMBER_4V
    #define CY_PSOC4_4100MS (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4V)
    #define CY_PSOC4_4100MS (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4V)
#else
    #define CY_PSOC4_4100MS (0u != 0u)
    #define CY_PSOC4_4100MS (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_4V */

#define CY_IP_HOBTO_DEVICE      (!(0 == 1))


/*******************************************************************************
* IP blocks
*******************************************************************************/
#if (CY_PSOC4)

    /* Using SRSSv2 or SRS-Lite */
    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_SRSSV2            (0 != 0)
        #define CY_IP_SRSSLT            (!CY_IP_SRSSV2)
    #else
        #define CY_IP_SRSSV2            (0 == 0)
        #define CY_IP_SRSSLT            (!CY_IP_SRSSV2)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_CPUSSV3           (1 == 1)
        #define CY_IP_CPUSSV2           (0 == 1)
        #define CY_IP_CPUSS             (0 == 1)
    #else
        #define CY_IP_CPUSSV3           (0 != 0)
        #define CY_IP_CPUSSV2           (0 != 0)
        #define CY_IP_CPUSS             (0 == 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    /* CM0 present or CM0+ present (1=CM0, 0=CM0+) */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_CPUSS_CM0         (0 == 0)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_CPUSS_CM0         (0 == 1)
        #endif  /* (CY_IP_CPUSSV2) */
        #define CY_IP_CPUSS_CM0PLUS         (!CY_IP_CPUSS_CM0)
    #else
        #define CY_IP_CPUSS_CM0             (0 == 0)
        #define CY_IP_CPUSS_CM0PLUS         (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Flash memory present or not (1=Flash present, 0=Flash not present) */
    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_CPUSS_FLASHC_PRESENT      (0 == 0)
    #else
        #define CY_IP_CPUSS_FLASHC_PRESENT      (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Product uses FLASH-Lite or regular FLASH */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_FM                (-1 == 0)
            #define CY_IP_FMLT              (-1 == 1)
            #define CY_IP_FS                (-1 == 2)
            #define CY_IP_FSLT              (-1 == 3)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_FM                (3 == 0)
            #define CY_IP_FMLT              (3 == 1)
            #define CY_IP_FS                (3 == 2)
            #define CY_IP_FSLT              (3 == 3)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_FM                (!CY_IP_FMLT)       /* Regular FLASH */
        #define CY_IP_FMLT              (0 != 0)            /* FLASH-Lite */
        #define CY_IP_FS                (0 != 0)            /* FS */
        #define CY_IP_FSLT              (0 != 0)            /* FSLT */
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Enable simultaneous execution/programming in multi-macro devices */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_FLASH_PARALLEL_PGM_EN (-1 == 1)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_FLASH_PARALLEL_PGM_EN (0 == 1)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_FLASH_PARALLEL_PGM_EN     (0u != 0u)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Number of Flash macros used in the device (0, 1 or 2) */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_FLASH_MACROS (-1u)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_FLASH_MACROS (1u)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_FLASH_MACROS     (1u)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    /* Number of interrupt request inputs to CM0 */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_INT_NR            (-1u)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_INT_NR            (28u)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_INT_NR            (32u)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Presence of the BLESS IP block */
    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_BLESS             (0 != 0)
        #define CY_IP_BLESSV3           (CYIPBLOCK_m0s8bless_VERSION == 3)
    #else
        #define CY_IP_BLESS             (0 != 0)
        #define CY_IP_BLESSV3           (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_USBDEV            (0 != 0)
    #else
        #define CY_IP_USBDEV            (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /***************************************************************************
    * Devices with the SPCIF_SYNCHRONOUS parameter set to one will not use
    * the 36MHz Oscillator for Flash operation. Instead, flash write function
    * ensures that the charge pump clock and the higher frequency clock (HFCLK)
    * are set to the IMO at 48MHz prior to writing the flash.
    ***************************************************************************/
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_SPCIF_SYNCHRONOUS     (-1 == 1)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_IP_SPCIF_SYNCHRONOUS     (1 == 1)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_SPCIF_SYNCHRONOUS     (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    /* Watch Crystal Oscillator (WCO) is present (32kHz) */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_BLESS)
            #define CY_IP_WCO_WCO           (0 != 0)
            #define CY_IP_WCO_SRSSV2        (0 != 0)
            #if (CY_IP_BLESSV3)
                #define CY_IP_WCO_WCOV2     (0 == 0)
                #define CY_IP_WCO_BLESS     (0 != 0)                
            #else
                #define CY_IP_WCO_WCOV2     (0 != 0)
                #define CY_IP_WCO_BLESS     (0 == 0)
            #endif
        #else
            #define CY_IP_WCO_BLESS         (0 != 0)
            #define CY_IP_WCO_WCO           (1 == 1)
            #define CY_IP_WCO_WCOV2         (0 != 0)
            #define CY_IP_WCO_SRSSV2        (-1 == 1)
        #endif  /* (CY_IP_BLESS) */
    #else
        #define CY_IP_WCO_BLESS             (0 != 0)
        #define CY_IP_WCO_WCO               (0 != 0)
        #define CY_IP_WCO_WCOV2             (0 != 0)
        #define CY_IP_WCO_SRSSV2            (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    #define CY_IP_WCO   (CY_IP_WCO_BLESS || CY_IP_WCO_WCO || CY_IP_WCO_WCOV2 || CY_IP_WCO_SRSSV2)

    /* External Crystal Oscillator is present (high frequency) */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_BLESS)
            #define CY_IP_ECO_SRSSV2        (0 != 0)
            #define CY_IP_ECO_SRSSLT        (0 != 0)
            
            #if (CY_IP_BLESSV3)
                #define CY_IP_ECO_BLESS     (0 != 0)
                #define CY_IP_ECO_BLESSV3   (0 == 0)
            #else
                #define CY_IP_ECO_BLESS     (0 == 0)
                #define CY_IP_ECO_BLESSV3   (0 != 0)
            #endif
        #else
            #define CY_IP_ECO_BLESS         (0 != 0)
            #define CY_IP_ECO_BLESSV3       (0 != 0)
            #define CY_IP_ECO_SRSSV2        (-1 == 1)
            #define CY_IP_ECO_SRSSLT        ((1 != 0) && (1 != 0))
        #endif  /* (CY_IP_BLESS) */
    #else
        #define CY_IP_ECO_BLESS             (0 != 0)
        #define CY_IP_ECO_BLESSV3           (0 != 0)
        #define CY_IP_ECO_SRSSV2            (0 != 0)
        #define CY_IP_ECO_SRSSLT            (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    #define CY_IP_ECO   (CY_IP_ECO_BLESS || CY_IP_ECO_SRSSV2 || CY_IP_ECO_BLESSV3 || CY_IP_ECO_SRSSLT)

    /* PLL is present */
    #if (CY_IP_HOBTO_DEVICE)
        #if(CY_IP_SRSSV2)
            #define CY_IP_PLL           ((-1 != 0) || \
                                          (-1 != 0)) 

            #define CY_IP_PLL_NR        (-1u + \
                                          -1u)
        
        #elif (CY_IP_SRSSLT)
            #define CY_IP_PLL           (1 == 1)

            #define CY_IP_PLL_NR        (1)
        #else
            #define CY_IP_PLL           (0 != 0)
            #define CY_IP_PLL_NR        (0)
        #endif /* (CY_IP_SRSSV2) */
    #else
        #define CY_IP_PLL           (0 != 0)
        #define CY_IP_PLL_NR        (0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    /* Clock Source clk_lf implemented in SysTick Counter. When 0, not implemented, 1=implemented */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_SYSTICK_LFCLK_SOURCE     (-1 != 0)
        #else   /* CY_IP_CPUSSV3 */
            #define CY_SYSTICK_LFCLK_SOURCE     (1 != 0)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_SYSTICK_LFCLK_SOURCE     (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    /* Flash Macro 0 has extra rows */
    #if (CY_IP_HOBTO_DEVICE)
        #ifdef CYREG_SFLASH_MACRO_0_FREE_SFLASH0
            #define CY_SFLASH_XTRA_ROWS         (0 == 0)
        #else
            #define CY_SFLASH_XTRA_ROWS         (0 != 0)
        #endif /* CYREG_SFLASH_MACRO_0_FREE_SFLASH0 */

    #else
        #define CY_SFLASH_XTRA_ROWS         (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */


    #if (CY_IP_USBDEV)
        #define CY_IP_IMO_TRIMMABLE_BY_USB  (0 == 0)
    #else
        #define CY_IP_IMO_TRIMMABLE_BY_USB  (0 != 0)
    #endif  /* (CY_IP_USBDEV) */


    #if (CY_IP_WCO_WCO || CY_IP_WCO_SRSSV2)
        #define CY_IP_IMO_TRIMMABLE_BY_WCO  (0 == 0)
    #else
        #define CY_IP_IMO_TRIMMABLE_BY_WCO  (0 != 0)
    #endif  /* (CY_IP_WCO_WCO || CY_IP_WCO_SRSSV2) */


    /* DW/DMA Controller present (0=No, 1=Yes) */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_DMAC_PRESENT      (-1 == 1)
        #else
            #define CY_IP_DMAC_PRESENT      (1 == 1)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_DMAC_PRESENT          (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

    #if (CY_IP_HOBTO_DEVICE)
        #define CY_IP_PASS                  (0 == 1)
    #else
        #define CY_IP_PASS                  (0 != 0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */



	/* Number of external slave ports on System Interconnect */
    #if (CY_IP_HOBTO_DEVICE)
        #if (CY_IP_CPUSSV2)
            #define CY_IP_SL_NR             (-1)
        #else
            #define CY_IP_SL_NR             (3)
        #endif  /* (CY_IP_CPUSSV2) */
    #else
        #define CY_IP_SL_NR                 (0)
    #endif  /* (CY_IP_HOBTO_DEVICE) */

#else

    #if (CY_PSOC3)
        #define CY_SYSTICK_LFCLK_SOURCE     (0 != 0)
    #else /* PSoC 5LP */
        #define CY_SYSTICK_LFCLK_SOURCE     (0 == 0)
    #endif /* (CY_PSOC3) */

#endif  /* (CY_PSOC4) */


/*******************************************************************************
* The components version defines. Available started from cy_boot 4.20
* Use the following construction in order to identify cy_boot version:
* (defined(CY_BOOT_VERSION) && CY_BOOT_VERSION >= CY_BOOT_4_20)
*******************************************************************************/
#define CY_BOOT_4_20            (420u)
#define CY_BOOT_5_0             (500u)
#define CY_BOOT_5_10            (510u)
#define CY_BOOT_5_20            (520u)
#define CY_BOOT_5_30            (530u)
#define CY_BOOT_5_40            (540u)
#define CY_BOOT_5_50            (550u)
#define CY_BOOT_5_60            (560u)
#define CY_BOOT_5_70            (570u)
#define CY_BOOT_VERSION         (CY_BOOT_5_70)


/*******************************************************************************
*   Base Types. Acceptable types from MISRA-C specifying signedness and size.
*******************************************************************************/
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef signed   char   int8;
typedef signed   short  int16;
typedef signed   long   int32;
typedef          float  float32;

#if(!CY_PSOC3)

    typedef               double float64;
    typedef          long long   int64;
    typedef unsigned long long   uint64;

#endif  /* (!CY_PSOC3) */

/* Signed or unsigned depending on compiler selection */
typedef          char   char8;


/*******************************************************************************
*   Memory address functions prototypes
*******************************************************************************/
#if(CY_PSOC3)

    /***************************************************************************
    * Prototypes for absolute memory address functions (cymem.a51) with built-in
    * endian conversion. These functions should be called through the
    * CY_GET_XTND_REGxx and CY_SET_XTND_REGxx macros.
    ***************************************************************************/
    extern uint8  cyread8       (const volatile void far *addr);
    extern void   cywrite8      (volatile void far *addr, uint8 value);

    extern uint16 cyread16      (const volatile void far *addr);
    extern uint16 cyread16_nodpx(const volatile void far *addr);

    extern void   cywrite16      (volatile void far *addr, uint16 value);
    extern void   cywrite16_nodpx(volatile void far *addr, uint16 value);

    extern uint32 cyread24      (const volatile void far *addr);
    extern uint32 cyread24_nodpx(const volatile void far *addr);

    extern void   cywrite24      (volatile void far *addr, uint32 value);
    extern void   cywrite24_nodpx(volatile void far *addr, uint32 value);

    extern uint32 cyread32      (const volatile void far *addr);
    extern uint32 cyread32_nodpx(const volatile void far *addr);

    extern void   cywrite32      (volatile void far *addr, uint32 value);
    extern void   cywrite32_nodpx(volatile void far *addr, uint32 value);


    /***************************************************************************
    * Memory access routines from cymem.a51 for the generated device
    * configuration code. These functions may be subject to change in future
    * revisions of the cy_boot component and they are not available for all
    * devices. Most code should use memset or memcpy instead.
    ***************************************************************************/
    void cymemzero(void far *addr, uint16 size);
    void cyconfigcpy(uint16 size, const void far *src, void far *dest) large;
    void cyconfigcpycode(uint16 size, const void code *src, void far *dest);

    #define CYCONFIGCPY_DECLARED    (1)

#else

    /* Prototype for function to set 24-bit register. Located at cyutils.c */
    extern void     CySetReg24(uint32 volatile * addr, uint32 value);

    #if(CY_PSOC4)

        extern uint32 CyGetReg24(uint32 const volatile * addr);

    #endif  /* (CY_PSOC4) */

#endif  /* (CY_PSOC3) */


/*******************************************************************************
*   Memory model definitions. To allow code to be 8051-ARM agnostic.
*******************************************************************************/
#if(CY_PSOC3)

    #define CYBDATA     bdata
    #define CYBIT       bit
    #define CYCODE      code
    #define CYCOMPACT   compact
    #define CYDATA      data
    #define CYFAR       far
    #define CYIDATA     idata
    #define CYLARGE     large
    #define CYPDATA     pdata
    #define CYREENTRANT reentrant
    #define CYSMALL     small
    #define CYXDATA     xdata
    #define XDATA       xdata

    #define CY_NOINIT

#else

    #define CYBDATA
    #define CYBIT      uint8
    #define CYCODE
    #define CYCOMPACT
    #define CYDATA
    #define CYFAR
    #define CYIDATA
    #define CYLARGE
    #define CYPDATA
    #define CYREENTRANT
    #define CYSMALL
    #define CYXDATA
    #define XDATA

    #if defined(__ARMCC_VERSION)

        #define CY_NOINIT           __attribute__ ((section(".noinit"), zero_init))
        #define CY_NORETURN         __attribute__ ((noreturn))
        #define CY_SECTION(name)    __attribute__ ((section(name)))

        /* Specifies a minimum alignment (in bytes) for variables of the
        *  specified type.
        */
        #define CY_ALIGN(align)     __align(align)


        /* Attached to an enum, struct, or union type definition, specified that
        *  the minimum required memory be used to represent the type.
        */
        #define CY_PACKED
        #define CY_PACKED_ATTR      __attribute__ ((packed))
        #define CY_INLINE           __inline
    #elif defined (__GNUC__)

        #define CY_NOINIT           __attribute__ ((section(".noinit")))
        #define CY_NORETURN         __attribute__ ((noreturn))
        #define CY_SECTION(name)    __attribute__ ((section(name)))
        #define CY_ALIGN(align)     __attribute__ ((aligned(align)))
        #define CY_PACKED
        #define CY_PACKED_ATTR      __attribute__ ((packed))
        #define CY_INLINE           inline
    #elif defined (__ICCARM__)

        #define CY_NOINIT           __no_init
        #define CY_NORETURN         __noreturn
        #define CY_PACKED           __packed
        #define CY_PACKED_ATTR
        #define CY_INLINE           inline
    #endif  /* (__ARMCC_VERSION) */

#endif  /* (CY_PSOC3) */


#if(CY_PSOC3)

    /* 8051 naturally returns 8 bit value. */
    typedef unsigned char cystatus;

#else

    /* ARM naturally returns 32 bit value. */
    typedef unsigned long cystatus;

#endif  /* (CY_PSOC3) */


/*******************************************************************************
*  Hardware Register Types.
*******************************************************************************/
typedef volatile uint8  CYXDATA reg8;
typedef volatile uint16 CYXDATA reg16;
typedef volatile uint32 CYXDATA reg32;


/*******************************************************************************
*  Interrupt Types and Macros
*******************************************************************************/
#if(CY_PSOC3)

    #define CY_ISR(FuncName)        void FuncName (void) interrupt 0
    #define CY_ISR_PROTO(FuncName)  void FuncName (void)
    typedef void (CYCODE * cyisraddress)(void);

#else

    #define CY_ISR(FuncName)        void FuncName (void)
    #define CY_ISR_PROTO(FuncName)  void FuncName (void)
    typedef void (* cyisraddress)(void);

    #if defined (__ICCARM__)
        typedef union { cyisraddress __fun; void * __ptr; } intvec_elem;
    #endif  /* defined (__ICCARM__) */

#endif  /* (CY_PSOC3) */


#define CY_M_PI                         (3.14159265358979323846264338327)


/**
* \addtogroup group_register_access
A library of macros provides read and write access to the registers of the device. These macros are used with the
defined values made available in the generated cydevice_trm.h and cyfitter.h files. Access to registers should be made
using these macros and not the functions that are used to implement the macros. This allows for device independent code
generation.

The PSoC 4 processor architecture use little endian ordering.

SRAM and Flash storage in all architectures is done using the endianness of the architecture and compilers. However,
the registers in all these chips are laid out in little endian order. These macros allow register accesses to match this
little endian ordering. If you perform operations on multi-byte registers without using these macros, you must consider
the byte ordering of the specific architecture. Examples include usage of DMA to transfer between memory and registers,
as well as function calls that are passed an array of bytes in memory.

The PSoC 4 requires these accesses to be aligned to the width of the transaction.

The PSoC 4 requires peripheral register accesses to match the hardware register size. Otherwise, the peripheral might
ignore the transfer and Hard Fault exception will be generated.

*/

/** @} group_register_access */


/**
* \addtogroup group_register_access_macros Register Access
* \ingroup group_register_access
* @{
*/

#if(CY_PSOC3)
    /*******************************************************************************
    * Macro Name: CY_GET_REG8(addr)
    ****************************************************************************//**
    *
    * Reads the 8-bit value from the specified register.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_REG8(addr)               (*((const reg8 *)(addr)))


    /*******************************************************************************
    * Macro Name: CY_SET_REG8(addr, value)
    ****************************************************************************//**
    *
    * Writes the 8-bit value to the specified register.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_REG8(addr, value)        (*((reg8 *)(addr))  = (uint8)(value))


    /*******************************************************************************
    * Macro Name: CY_GET_REG16(addr)
    ****************************************************************************//**
    *
    * Reads the 16-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_REG16(addr)              cyread16_nodpx ((const volatile void far *)(const reg16 *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_REG16(addr, value)
    ****************************************************************************//**
    *
    * Writes the 16-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_REG16(addr, value)       cywrite16_nodpx((volatile void far *)(reg16 *)(addr), value)


    /*******************************************************************************
    * Macro Name: CY_GET_REG24(addr)
    ****************************************************************************//**
    *
    * Reads the 24-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_REG24(addr)              cyread24_nodpx ((const volatile void far *)(const reg32 *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_REG24(addr, value)
    ****************************************************************************//**
    *
    * Writes the 24-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_REG24(addr, value)       cywrite24_nodpx((volatile void far *)(reg32 *)(addr),value)


    /*******************************************************************************
    * Macro Name: CY_GET_REG32(addr)
    ****************************************************************************//**
    *
    * Reads the 32-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_REG32(addr)              cyread32_nodpx ((const volatile void far *)(const reg32 *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_REG32(addr, value)
    ****************************************************************************//**
    *
    * Writes the 32-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_REG32(addr, value)       cywrite32_nodpx((volatile void far *)(reg32 *)(addr), value)


    /*******************************************************************************
    * Macro Name: CY_GET_XTND_REG8(addr)
    ****************************************************************************//**
    *
    * Reads the 8-bit value from the specified register.
    * Identical to \ref CY_GET_REG8 for PSoC 4.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_XTND_REG8(addr)          cyread8((const volatile void far *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_XTND_REG8(addr, value)
    ****************************************************************************//**
    *
    * Writes the 8-bit value to the specified register.
    * Identical to \ref CY_SET_REG8 for PSoC 4.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_XTND_REG8(addr, value)   cywrite8((volatile void far *)(addr), value)


    /*******************************************************************************
    * Macro Name: CY_GET_XTND_REG16(addr)
    ****************************************************************************//**
    *
    * Reads the 16-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_GET_REG16
    * for PSoC 4.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_XTND_REG16(addr)         cyread16((const volatile void far *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_XTND_REG16(addr, value)
    ****************************************************************************//**
    *
    * Writes the 16-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_SET_REG16
    * for PSoC 4.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_XTND_REG16(addr, value)  cywrite16((volatile void far *)(addr), value)


    /*******************************************************************************
    * Macro Name: CY_GET_XTND_REG24(addr)
    ****************************************************************************//**
    *
    * Reads the 24-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_GET_REG24
    * for PSoC 4.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_XTND_REG24(addr)         cyread24((const volatile void far *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_XTND_REG24(addr, value)
    ****************************************************************************//**
    *
    * Writes the 24-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_SET_REG24
    * for PSoC 4.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_XTND_REG24(addr, value)  cywrite24((volatile void far *)(addr), value)


    /*******************************************************************************
    * Macro Name: CY_GET_XTND_REG32(addr)
    ****************************************************************************//**
    *
    * Reads the 32-bit value from the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_GET_REG32
    * for PSoC 4.
    *
    * \param reg Register address.
    *
    * \return Read value.
    *
    *******************************************************************************/
    #define CY_GET_XTND_REG32(addr)         cyread32((const volatile void far *)(addr))


    /*******************************************************************************
    * Macro Name: CY_SET_XTND_REG32(addr, value)
    ****************************************************************************//**
    *
    * Writes the 32-bit value to the specified register. This macro implements the
    * byte swapping required for proper operation. Identical to \ref CY_SET_REG32
    * for PSoC 4.
    *
    * \param reg Register address.
    * \param value Value to write.
    *
    *******************************************************************************/
    #define CY_SET_XTND_REG32(addr, value)  cywrite32((volatile void far *)(addr), value)

#else

    #define CY_GET_REG8(addr)               (*((const reg8 *)(addr)))
    #define CY_SET_REG8(addr, value)        (*((reg8 *)(addr))  = (uint8)(value))

    #define CY_GET_REG16(addr)              (*((const reg16 *)(addr)))
    #define CY_SET_REG16(addr, value)       (*((reg16 *)(addr)) = (uint16)(value))


    #define CY_SET_REG24(addr, value)       CySetReg24((reg32 *) (addr), (value))
    #if(CY_PSOC4)
        #define CY_GET_REG24(addr)          CyGetReg24((const reg32 *) (addr))
    #else
        #define CY_GET_REG24(addr)          (*((const reg32 *)(addr)) & 0x00FFFFFFu)
    #endif  /* (CY_PSOC4) */


    #define CY_GET_REG32(addr)              (*((const reg32 *)(addr)))
    #define CY_SET_REG32(addr, value)       (*((reg32 *)(addr)) = (uint32)(value))

    /* To allow code to be 8051-ARM agnostic. */
    #define CY_GET_XTND_REG8(addr)          CY_GET_REG8(addr)
    #define CY_SET_XTND_REG8(addr, value)   CY_SET_REG8(addr, value)

    #define CY_GET_XTND_REG16(addr)         CY_GET_REG16(addr)
    #define CY_SET_XTND_REG16(addr, value)  CY_SET_REG16(addr, value)

    #define CY_GET_XTND_REG24(addr)         CY_GET_REG24(addr)
    #define CY_SET_XTND_REG24(addr, value)  CY_SET_REG24(addr, value)

    #define CY_GET_XTND_REG32(addr)         CY_GET_REG32(addr)
    #define CY_SET_XTND_REG32(addr, value)  CY_SET_REG32(addr, value)

#endif  /* (CY_PSOC3) */
/** @} group_register_access_macros */


/**
* \addtogroup group_register_access_bits Bit Manipulation
* \ingroup group_register_access
* @{
*/

#if(CY_PSOC4)

    /*******************************************************************************
    * Macro Name: CY_GET_FIELD_MASK(regSize, bitFieldName)
    ****************************************************************************//**
    *
    * Returns the bit field mask for the specified register size and bit field
    * name.
    *
    * \param regSize Size of the register in bits.
    * \param bitFieldName Fully qualified name of the bit field. The biFieldName
    *  is automatically appended with __OFFSET and __SIZE by the macro for usage.
    *
    * \return Returns the bit mask.
    *
    *******************************************************************************/
    #define CY_GET_FIELD_MASK(regSize, bitFieldName) \
        ((((uint ## regSize) 0xFFFFFFFFu << ((uint32)(regSize) - bitFieldName ## __SIZE - bitFieldName ## __OFFSET)) >>\
         ((uint32)(regSize) - bitFieldName ## __SIZE)) << bitFieldName ## __OFFSET)


    /*******************************************************************************
    * Macro Name: CY_GET_REG8_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Reads the specified bit field value from the specified 8-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register will remain uncorrupted during simultaneous read-modify-write
    * operation performed by two threads (main and interrupt threads). To
    * guarantee data integrity in such cases, the macro should be invoked while
    * the specific interrupt is disabled or within a critical section (all
    * interrupts are disabled).
    *
    * Using this macro on 32-bit and 16-bit width registers will generate a
    * hard fault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerName: The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName: fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family
    * register TRM.
    *
    * \return Zero if the specified bit field is zero, and a non-zero value,
    * otherwise. The return value is of type uint32.
    *
    *******************************************************************************/
    #define CY_GET_REG8_FIELD(registerName, bitFieldName) \
         ((CY_GET_REG8((registerName)) >> bitFieldName ## __OFFSET) & (~(0xFFu << bitFieldName ## __SIZE)))


    /*******************************************************************************
    * Macro Name: CY_SET_REG8_FIELD(registerName, bitFieldName, value)
    ****************************************************************************//**
    *
    * Sets the specified bit field value of the specified 8-bit register to the
    * required value.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write
    * operation performed by two threads (main and interrupt threads). To
    * guarantee data integrity in such cases, the macro should be invoked while
    * the specific interrupt is disabled or within a critical section (all
    * interrupts are disabled).
    *
    * Using this macro on the 32-bit and 16-bit width registers, generates a
    * hard fault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    * \param value The value that the field must be configured for.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family
    * register TRM.
    *
    *******************************************************************************/
    #define CY_SET_REG8_FIELD(registerName, bitFieldName, value) \
        CY_SET_REG8((registerName), \
        ((CY_GET_REG8((registerName))                 & ~CY_GET_FIELD_MASK(8, bitFieldName)) | \
        (((uint8)(value) << bitFieldName ## __OFFSET) &  CY_GET_FIELD_MASK(8, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_CLEAR_REG8_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Clears the specified bit field of the specified 8-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write
    * operation performed by two threads (main and interrupt threads). To
    * guarantee data integrity in such cases, the macro should be invoked while
    * the specific interrupt is disabled or within a critical section (all
    * interrupts are disabled).
    *
    * Using this macro on the 32-bit and 16-bit width registers generates a
    * hard fault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    *
    * For fully qualified names of the register and bit fields and the
    * possible values the field can take, please, refer to a respective PSoC
    * family register TRM.
    *
    *******************************************************************************/
    #define CY_CLEAR_REG8_FIELD(registerName, bitFieldName) \
        (CY_SET_REG8((registerName), (CY_GET_REG8((registerName)) & ~CY_GET_FIELD_MASK(8, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_GET_REG16_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Reads the specified bit field value from the specified 16-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write
    * operation performed by two threads (main and interrupt threads). To
    * guarantee data integrity in such cases, the macro should be invoked while
    * the specific interrupt is disabled or within a critical section (all
    * interrupts are disabled).
    *
    * Using this macro on the 32-bit and 16-bit width registers generates a
    * hardfault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    *
    * For fully qualified names of the register and bit fields and the
    * possible values the field can take, please, refer to a respective PSoC
    * family register TRM.
    *
    * \return Zero if the specified bit field is zero, and a non-zero value,
    * otherwise. The return value is of type uint32.
    *
    *******************************************************************************/
    #define CY_GET_REG16_FIELD(registerName, bitFieldName) \
         ((CY_GET_REG16((registerName)) >> bitFieldName ## __OFFSET) & (~(0xFFFFu << bitFieldName ## __SIZE)))


    /*******************************************************************************
    * Macro Name: CY_SET_REG16_FIELD(registerName, bitFieldName, value)
    ****************************************************************************//**
    *
    * Sets the specified bit field value of the specified 16-bit register to the
    * required value.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * Using this macro on the 32-bit and 16-bit width registers generates a hard
    * fault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerNam The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName: fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    * \param value The value that the field must be configured for.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family
    * register TRM.
    *
    *******************************************************************************/
    #define CY_SET_REG16_FIELD(registerName, bitFieldName, value) \
        CY_SET_REG16((registerName), \
        ((CY_GET_REG16((registerName))                 & ~CY_GET_FIELD_MASK(16, bitFieldName)) | \
        (((uint16)(value) << bitFieldName ## __OFFSET) &  CY_GET_FIELD_MASK(16, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_CLEAR_REG16_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Clears the specified bit field of the specified 16-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * Using this macro on the 32-bit and 16-bit width registers generates a hard
    * fault exception. Examples of 8-bit registers are the UDB registers.
    *
    * \param registerName: The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName: fully qualified name of the bit field. The biFieldName is
    * automatically appended with __OFFSET and __SIZE by the macro for usage.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family register
    * TRM.
    *
    *******************************************************************************/
    #define CY_CLEAR_REG16_FIELD(registerName, bitFieldName)\
        (CY_SET_REG16((registerName), (CY_GET_REG16((registerName)) & ~CY_GET_FIELD_MASK(16, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_GET_REG32_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Reads the specified bit field value from the specified 32-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * Using this macro on the 16-bit and 8-bit width registers generates a hard
    * fault exception.
    *
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName The Fully qualified name of the bit field. The
    * biFieldName is automatically appended with __OFFSET and __SIZE by the macro
    * for usage.
    *
    * For fully qualified names of the register and bit fields, please, refer to
    * a respective PSoC family register TRM.
    *
    * \return Zero if the specified bit field is zero, and a non-zero value, otherwise.
    * The return value is of type uint32.
    *
    *******************************************************************************/
    #define CY_GET_REG32_FIELD(registerName, bitFieldName) \
          ((CY_GET_REG32((registerName)) >> bitFieldName ## __OFFSET) & (~(0xFFFFFFFFu << bitFieldName ## __SIZE)))


    /*******************************************************************************
    * Macro Name: CY_SET_REG32_FIELD(registerName, bitFieldName, value)
    ****************************************************************************//**
    *
    * Sets the specified bit field value of the specified 32-bit register to the
    * required value.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * Using this macro on the 16-bit and 8-bit width registers generates a hard
    * fault exception.
    *
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName The fully qualified name of the bit field. The
    * biFieldName is automatically appended with __OFFSET and __SIZE by the macro
    * for usage.
    * \param value The value that the field must be configured for.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family register
    * TRM.
    *
    *******************************************************************************/
    #define CY_SET_REG32_FIELD(registerName, bitFieldName, value) \
        CY_SET_REG32((registerName), \
        ((CY_GET_REG32((registerName))                 & ~CY_GET_FIELD_MASK(32, bitFieldName)) | \
        (((uint32)(value) << bitFieldName ## __OFFSET) &  CY_GET_FIELD_MASK(32, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_CLEAR_REG32_FIELD(registerName, bitFieldName)
    ****************************************************************************//**
    *
    * Clears the specified bit field of the specified 32-bit register.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * Using this macro on the 16-bit and 8-bit width registers generates a hard
    * fault exception.
	*
    * \param registerName The fully qualified name of the PSoC 4 device register.
    * \param bitFieldName The fully qualified name of the bit field. The
    * biFieldName is automatically appended with __OFFSET and __SIZE by the macro
    * for usage.
    *
    * For fully qualified names of the register and bit fields and the possible
    * values the field can take, please, refer to a respective PSoC family register
    * TRM.
    *
    *******************************************************************************/
    #define CY_CLEAR_REG32_FIELD(registerName, bitFieldName) \
        (CY_SET_REG32((registerName), (CY_GET_REG32((registerName)) & ~CY_GET_FIELD_MASK(32, bitFieldName))))


    /*******************************************************************************
    * Macro Name: CY_GET_FIELD(regValue, bitFieldName)
    ****************************************************************************//**
    *
    * Reads the specified bit field value from the given 32-bit value.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * This macro has to be used in conjunction with \ref CY_GET_REG32 for atomic
    * reads.
    *
    * \param regValue The value as read by \ref CY_GET_REG32.
    * \param bitFieldName The fully qualified name of the bit field. The
    * biFieldName is automatically appended with __OFFSET and __SIZE by the macro
    * for usage.
    *
    * For fully qualified names of the bit field and the possible values the field
    * can take, please, refer to a respective PSoC family register TRM.
    *
    * \return Zero if the specified bit field is zero, and a non-zero value,
    * otherwise. The return value is of type uint32.
    *
    *******************************************************************************/
    #define CY_GET_FIELD(regValue, bitFieldName) \
        (((regValue) >> bitFieldName ## __OFFSET) & (~(0xFFFFFFFFu << bitFieldName ## __SIZE)))


    /*******************************************************************************
    * Macro Name: CY_SET_FIELD(regValue, bitFieldName, value)
    ****************************************************************************//**
    *
    * Sets the specified bit field value within a given 32-bit value.
    *
    * The macro operation is not atomic. It is not guaranteed that the shared
    * register remains uncorrupted during simultaneous read-modify-write operation
    * performed by two threads (main and interrupt threads). To guarantee data
    * integrity in such cases, the macro should be invoked while the specific
    * interrupt is disabled or within a critical section (all interrupts are
    * disabled).
    *
    * This macro has to be used in conjunction with \ref CY_GET_REG32 for atomic
    * reads and \ref CY_SET_REG32 for atomic writes.
    *
    * \param regValue The value as read by \ref CY_GET_REG32.
    * \param bitFieldName The fully qualified name of the bit field. The
    * biFieldName is automatically appended with __OFFSET and __SIZE by the macro
    * for usage.
    * \param value The value that the field must be configured for.
    *
    * For fully qualified names of the bit field and the possible values the field
    * can take, please, refer to the respective PSoC family register TRM.
    *
    *******************************************************************************/
    #define CY_SET_FIELD(regValue, bitFieldName, value) \
        ((regValue) = \
        ((((uint32)(value) & (~(0xFFFFFFu << bitFieldName ## __SIZE))) << bitFieldName ## __OFFSET)) | \
        ((uint32)(regValue) & (((~(0xFFu << bitFieldName ## __SIZE))) << bitFieldName ## __OFFSET)))

#endif  /* (CY_PSOC4) */

/** @} group_register_access_bits */


/*******************************************************************************
*  Data manipulation defines
*******************************************************************************/

/* Get 8 bits of 16 bit value. */
#define LO8(x)                  ((uint8) ((x) & 0xFFu))
#define HI8(x)                  ((uint8) ((uint16)(x) >> 8))

/* Get 16 bits of 32 bit value. */
#define LO16(x)                 ((uint16) ((x) & 0xFFFFu))
#define HI16(x)                 ((uint16) ((uint32)(x) >> 16))

/* Swap the byte ordering of 32 bit value */
#define CYSWAP_ENDIAN32(x)  \
        ((uint32)((((x) >> 24) & 0x000000FFu) | (((x) & 0x00FF0000u) >> 8) | (((x) & 0x0000FF00u) << 8) | ((x) << 24)))

/* Swap the byte ordering of 16 bit value */
#define CYSWAP_ENDIAN16(x)      ((uint16)(((x) << 8) | (((x) >> 8) & 0x00FFu)))


/*******************************************************************************
* Defines the standard return values used in PSoC content. A function is
* not limited to these return values but can use them when returning standard
* error values. Return values can be overloaded if documented in the function
* header. On the 8051 a function can use a larger return type but still use the
* defined return codes.
*
* Zero is successful, all other values indicate some form of failure. 1 - 0x7F -
* standard defined values; 0x80 - ...  - user or content defined values.
*******************************************************************************/
#define CYRET_SUCCESS           (0x00u)           /* Successful */
#define CYRET_BAD_PARAM         (0x01u)           /* One or more invalid parameters */
#define CYRET_INVALID_OBJECT    (0x02u)           /* Invalid object specified */
#define CYRET_MEMORY            (0x03u)           /* Memory related failure */
#define CYRET_LOCKED            (0x04u)           /* Resource lock failure */
#define CYRET_EMPTY             (0x05u)           /* No more objects available */
#define CYRET_BAD_DATA          (0x06u)           /* Bad data received (CRC or other error check) */
#define CYRET_STARTED           (0x07u)           /* Operation started, but not necessarily completed yet */
#define CYRET_FINISHED          (0x08u)           /* Operation completed */
#define CYRET_CANCELED          (0x09u)           /* Operation canceled */
#define CYRET_TIMEOUT           (0x10u)           /* Operation timed out */
#define CYRET_INVALID_STATE     (0x11u)           /* Operation not setup or is in an improper state */
#define CYRET_UNKNOWN           ((cystatus) 0xFFFFFFFFu)    /* Unknown failure */


/*******************************************************************************
*   Intrinsic Defines: Processor NOP instruction
*******************************************************************************/
#if(CY_PSOC3)

    #define CY_NOP          _nop_()

#else

    #if defined(__ARMCC_VERSION)

        /* RealView */
        #define CY_NOP      __nop()

    #else

        /* GCC */
        #define CY_NOP      __asm("NOP\n")

    #endif  /* defined(__ARMCC_VERSION) */

#endif  /* (CY_PSOC3) */


/*******************************************************************************
* The following code is OBSOLETE and must not be used starting from cy_boot 5.10
*
* If the obsoleted macro definitions intended for use in the application use the
* following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#define CY_IP_S8FS      CY_IP_FS


/*******************************************************************************
* The following code is OBSOLETE and must not be used starting from cy_boot 3.10
*******************************************************************************/
#define CY_UDB_V0           (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_5A)
#define CY_UDB_V1           (!CY_UDB_V0)
#define CY_PSOC4A  (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4A)
#ifdef CYDEV_CHIP_MEMBER_4D
    #define CY_PSOC4D   (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_4D)
    #define CY_PSOC4SF  (CY_PSOC4D)
#else
    #define CY_PSOC4D   (0u != 0u)
    #define CY_PSOC4SF  (CY_PSOC4D)
#endif  /* CYDEV_CHIP_MEMBER_4D */
#define CY_PSOC5A  (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_5A)
#ifdef CYDEV_CHIP_MEMBER_5B
    #define CY_PSOC5LP  (CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_5B)
#else
    #define CY_PSOC5LP  (0u != 0u)
#endif  /* CYDEV_CHIP_MEMBER_5B */

#if (!CY_PSOC4)

    /* Device is PSoC 3 and the revision is ES2 or earlier */
    #define CY_PSOC3_ES2 ((CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_3A) && \
        (CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_3A_ES2))

    /* Device is PSoC 3 and the revision is ES3 or later */
    #define CY_PSOC3_ES3 ((CYDEV_CHIP_MEMBER_USED == CYDEV_CHIP_MEMBER_3A) && \
        (CYDEV_CHIP_REVISION_USED >= CYDEV_CHIP_REVISION_3A_ES3))

    /* Device is PSoC 5 and the revision is ES1 or earlier */
    #define CY_PSOC5_ES1 (CY_PSOC5A && \
        (CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_5A_ES1))

    /* Device is PSoC 5 and the revision is ES2 or later */
    #define CY_PSOC5_ES2 (CY_PSOC5A && \
        (CYDEV_CHIP_REVISION_USED > CYDEV_CHIP_REVISION_5A_ES1))

#endif  /* (!CY_PSOC4) */

#endif  /* CY_BOOT_CYTYPES_H */


/* [] END OF FILE */

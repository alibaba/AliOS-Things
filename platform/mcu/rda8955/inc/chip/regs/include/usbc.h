/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _USBC_H_
#define _USBC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'usbc'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define SPFIFORAM_SIZE                          (264)
#define DIEP_NUM                                (3)
#define DOEP_NUM                                (2)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// USBC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_USBC_BASE              0x01A80000

typedef volatile struct
{
    REG32                          GOTGCTL;                      //0x00000000
    REG32                          GOTGINT;                      //0x00000004
    REG32                          GAHBCFG;                      //0x00000008
    REG32                          GUSBCFG;                      //0x0000000C
    REG32                          GRSTCTL;                      //0x00000010
    REG32                          GINTSTS;                      //0x00000014
    REG32                          GINTMSK;                      //0x00000018
    REG32                          GRXSTSR;                      //0x0000001C
    REG32                          GRXSTSP;                      //0x00000020
    REG32                          GRXFSIZ;                      //0x00000024
    REG32                          GNPTXFSIZ;                    //0x00000028
    REG32                          GNPTXSTS;                     //0x0000002C
    REG32                          GI2CCTL;                      //0x00000030
    REG32                          GPVNDCTL;                     //0x00000034
    REG32                          GGPIO;                        //0x00000038
    REG32                          GUID;                         //0x0000003C
    REG32                          GSNPSID;                      //0x00000040
    REG32                          GHWCFG1;                      //0x00000044
    REG32                          GHWCFG2;                      //0x00000048
    REG32                          GHWCFG3;                      //0x0000004C
    REG32                          GHWCFG4;                      //0x00000050
    REG32 Reserved_00000054[43];                //0x00000054
    REG32                          HPTXFSIZ;                     //0x00000100
    struct
    {
        REG32                      DIEnPTXF;                     //0x00000104
    } DIEPTXF[DIEP_NUM];
    REG32 Reserved_00000110[444];               //0x00000110
    REG32                          DCFG;                         //0x00000800
    REG32                          DCTL;                         //0x00000804
    REG32                          DSTS;                         //0x00000808
    REG32 Reserved_0000080C;                    //0x0000080C
    REG32                          DIEPMSK;                      //0x00000810
    REG32                          DOEPMSK;                      //0x00000814
    REG32                          DAINT;                        //0x00000818
    REG32                          DAINTMSK;                     //0x0000081C
    REG32                          DTKNQR1;                      //0x00000820
    REG32                          DTKNQR2;                      //0x00000824
    REG32                          DVBUSDIS;                     //0x00000828
    REG32                          DVBUSPULSE;                   //0x0000082C
    REG32                          DTHRCTL;                      //0x00000830
    REG32                          DIEPEMPMSK;                   //0x00000834
    REG32 Reserved_00000838[50];                //0x00000838
    REG32                          DIEPCTL0;                     //0x00000900
    REG32 Reserved_00000904;                    //0x00000904
    REG32                          DIEPINT0;                     //0x00000908
    REG32 Reserved_0000090C;                    //0x0000090C
    REG32                          DIEPTSIZ0;                    //0x00000910
    REG32                          DIEPDMA0;                     //0x00000914
    REG32                          DIEPFSTS0;                    //0x00000918
    REG32 Reserved_0000091C;                    //0x0000091C
    struct
    {
        REG32                      DIEPCTL;                      //0x00000920
        REG32 Reserved_00000004;                //0x00000004
        REG32                      DIEPINT;                      //0x00000928
        REG32 Reserved_0000000C;                //0x0000000C
        REG32                      DIEPTSIZ;                     //0x00000930
        REG32                      DIEPDMA;                      //0x00000934
        REG32                      DIEPFSTS;                     //0x00000938
        REG32 Reserved_0000001C;                //0x0000001C
    } DIEPnCONFIG[DIEP_NUM];
    REG32 Reserved_00000980[96];                //0x00000980
    REG32                          DOEPCTL0;                     //0x00000B00
    REG32 Reserved_00000B04;                    //0x00000B04
    REG32                          DOEPINT0;                     //0x00000B08
    REG32 Reserved_00000B0C;                    //0x00000B0C
    REG32                          DOEPTSIZ0;                    //0x00000B10
    REG32                          DOEPDMA0;                     //0x00000B14
    REG32 Reserved_00000B18[2];                 //0x00000B18
    struct
    {
        REG32                      DOEPCTL;                      //0x00000B20
        REG32 Reserved_00000004;                //0x00000004
        REG32                      DOEPINT;                      //0x00000B28
        REG32 Reserved_0000000C;                //0x0000000C
        REG32                      DOEPTSIZ;                     //0x00000B30
        REG32                      DOEPDMA;                      //0x00000B34
        REG32 Reserved_00000018[2];             //0x00000018
    } DOEPnCONFIG[DOEP_NUM];
    REG32 Reserved_00000B60[168];               //0x00000B60
    REG32                          PCGCCTL;                      //0x00000E00
    REG32 Reserved_00000E04[127];               //0x00000E04
    struct
    {
        REG32                      TxRxData;                     //0x00001000
        REG32 Reserved_00000004[1023];          //0x00000004
    } EPnFIFO[DIEP_NUM+1];
    REG32 Reserved_00005000[27648];             //0x00005000
    struct
    {
        REG32                      RAMData;                      //0x00020000
    } SPFIFORAM[SPFIFORAM_SIZE];
} HWP_USBC_T;

#define hwp_usbc                   ((HWP_USBC_T*) KSEG1(REG_USBC_BASE))


//GOTGCTL
#define USBC_SESREQSCS             (1<<0)
#define USBC_SESREQSCS_MASK        (1<<0)
#define USBC_SESREQSCS_SHIFT       (0)
#define USBC_SESREQ                (1<<1)
#define USBC_SESREQ_MASK           (1<<1)
#define USBC_SESREQ_SHIFT          (1)
#define USBC_HSTNEGSCS             (1<<8)
#define USBC_HSTNEGSCS_MASK        (1<<8)
#define USBC_HSTNEGSCS_SHIFT       (8)
#define USBC_HNPREQ                (1<<9)
#define USBC_HNPREQ_MASK           (1<<9)
#define USBC_HNPREQ_SHIFT          (9)
#define USBC_HSTSETHNPEN           (1<<10)
#define USBC_HSTSETHNPEN_MASK      (1<<10)
#define USBC_HSTSETHNPEN_SHIFT     (10)
#define USBC_DEVHNPEN              (1<<11)
#define USBC_DEVHNPEN_MASK         (1<<11)
#define USBC_DEVHNPEN_SHIFT        (11)
#define USBC_CONLDSTS              (1<<16)
#define USBC_CONLDSTS_MASK         (1<<16)
#define USBC_CONLDSTS_SHIFT        (16)
#define USBC_DBNCTIME              (1<<17)
#define USBC_DBNCTIME_MASK         (1<<17)
#define USBC_DBNCTIME_SHIFT        (17)
#define USBC_ASESVLD               (1<<18)
#define USBC_ASESVLD_MASK          (1<<18)
#define USBC_ASESVLD_SHIFT         (18)
#define USBC_BSESVLD               (1<<19)
#define USBC_BSESVLD_MASK          (1<<19)
#define USBC_BSESVLD_SHIFT         (19)

//GOTGINT
#define USBC_SESENDDET             (1<<2)
#define USBC_SESENDDET_MASK        (1<<2)
#define USBC_SESENDDET_SHIFT       (2)
#define USBC_SESREQSUCSTSCHNG      (1<<8)
#define USBC_SESREQSUCSTSCHNG_MASK (1<<8)
#define USBC_SESREQSUCSTSCHNG_SHIFT (8)
#define USBC_HSTNEGSUCSTSCHNG      (1<<9)
#define USBC_HSTNEGSUCSTSCHNG_MASK (1<<9)
#define USBC_HSTNEGSUCSTSCHNG_SHIFT (9)
#define USBC_HSTNEGDET             (1<<17)
#define USBC_HSTNEGDET_MASK        (1<<17)
#define USBC_HSTNEGDET_SHIFT       (17)
#define USBC_ADEVTOUTCHG           (1<<18)
#define USBC_ADEVTOUTCHG_MASK      (1<<18)
#define USBC_ADEVTOUTCHG_SHIFT     (18)
#define USBC_DBNCEDONE             (1<<19)
#define USBC_DBNCEDONE_MASK        (1<<19)
#define USBC_DBNCEDONE_SHIFT       (19)

//GAHBCFG
#define USBC_GLBLINTRMSK           (1<<0)
#define USBC_GLBLINTRMSK_MASK      (1<<0)
#define USBC_GLBLINTRMSK_SHIFT     (0)
#define USBC_HBSTLEN(n)            (((n)&15)<<1)
#define USBC_HBSTLEN_MASK          (15<<1)
#define USBC_HBSTLEN_SHIFT         (1)
#define USBC_DMAEN                 (1<<5)
#define USBC_DMAEN_MASK            (1<<5)
#define USBC_DMAEN_SHIFT           (5)
#define USBC_NPTXFEMPLVL           (1<<7)
#define USBC_NPTXFEMPLVL_MASK      (1<<7)
#define USBC_NPTXFEMPLVL_SHIFT     (7)
#define USBC_PTXFEMPLVL            (1<<8)
#define USBC_PTXFEMPLVL_MASK       (1<<8)
#define USBC_PTXFEMPLVL_SHIFT      (8)

//GUSBCFG
#define USBC_TOUTCAL(n)            (((n)&7)<<0)
#define USBC_TOUTCAL_MASK          (7<<0)
#define USBC_TOUTCAL_SHIFT         (0)
#define USBC_PHYIF                 (1<<3)
#define USBC_PHYIF_MASK            (1<<3)
#define USBC_PHYIF_SHIFT           (3)
#define USBC_ULPI_UTMI_SEL         (1<<4)
#define USBC_ULPI_UTMI_SEL_MASK    (1<<4)
#define USBC_ULPI_UTMI_SEL_SHIFT   (4)
#define USBC_FSINTF                (1<<5)
#define USBC_FSINTF_MASK           (1<<5)
#define USBC_FSINTF_SHIFT          (5)
#define USBC_PHYSEL                (1<<6)
#define USBC_PHYSEL_MASK           (1<<6)
#define USBC_PHYSEL_SHIFT          (6)
#define USBC_DDRSEL                (1<<7)
#define USBC_DDRSEL_MASK           (1<<7)
#define USBC_DDRSEL_SHIFT          (7)
#define USBC_SRPCAP                (1<<8)
#define USBC_SRPCAP_MASK           (1<<8)
#define USBC_SRPCAP_SHIFT          (8)
#define USBC_HNPCAP                (1<<9)
#define USBC_HNPCAP_MASK           (1<<9)
#define USBC_HNPCAP_SHIFT          (9)
#define USBC_USBTRDTIM(n)          (((n)&15)<<10)
#define USBC_USBTRDTIM_MASK        (15<<10)
#define USBC_USBTRDTIM_SHIFT       (10)
#define USBC_PHYPWRCLKSEL          (1<<15)
#define USBC_PHYPWRCLKSEL_MASK     (1<<15)
#define USBC_PHYPWRCLKSEL_SHIFT    (15)
#define USBC_OTGI2CSEL             (1<<16)
#define USBC_OTGI2CSEL_MASK        (1<<16)
#define USBC_OTGI2CSEL_SHIFT       (16)
#define USBC_ULPIFSLS              (1<<17)
#define USBC_ULPIFSLS_MASK         (1<<17)
#define USBC_ULPIFSLS_SHIFT        (17)
#define USBC_ULPIAUTORES           (1<<18)
#define USBC_ULPIAUTORES_MASK      (1<<18)
#define USBC_ULPIAUTORES_SHIFT     (18)
#define USBC_ULPICLKSUSM           (1<<19)
#define USBC_ULPICLKSUSM_MASK      (1<<19)
#define USBC_ULPICLKSUSM_SHIFT     (19)
#define USBC_ULPIEXTVBUSDRV        (1<<20)
#define USBC_ULPIEXTVBUSDRV_MASK   (1<<20)
#define USBC_ULPIEXTVBUSDRV_SHIFT  (20)
#define USBC_ULPIEXTVBUSINDICATOR  (1<<21)
#define USBC_ULPIEXTVBUSINDICATOR_MASK (1<<21)
#define USBC_ULPIEXTVBUSINDICATOR_SHIFT (21)
#define USBC_TERMSELDLPULSE        (1<<22)
#define USBC_TERMSELDLPULSE_MASK   (1<<22)
#define USBC_TERMSELDLPULSE_SHIFT  (22)
#define USBC_FORCEHSTMODE          (1<<29)
#define USBC_FORCEHSTMODE_MASK     (1<<29)
#define USBC_FORCEHSTMODE_SHIFT    (29)
#define USBC_FORCEDEVMODE          (1<<30)
#define USBC_FORCEDEVMODE_MASK     (1<<30)
#define USBC_FORCEDEVMODE_SHIFT    (30)
#define USBC_CORRUPTTXPACKET       (1<<31)
#define USBC_CORRUPTTXPACKET_MASK  (1<<31)
#define USBC_CORRUPTTXPACKET_SHIFT (31)

//GRSTCTL
#define USBC_CSFTRST               (1<<0)
#define USBC_CSFTRST_MASK          (1<<0)
#define USBC_CSFTRST_SHIFT         (0)
#define USBC_HSFTRST               (1<<1)
#define USBC_HSFTRST_MASK          (1<<1)
#define USBC_HSFTRST_SHIFT         (1)
#define USBC_FRMCNTRRST            (1<<2)
#define USBC_FRMCNTRRST_MASK       (1<<2)
#define USBC_FRMCNTRRST_SHIFT      (2)
#define USBC_INTKNQFLSH            (1<<3)
#define USBC_INTKNQFLSH_MASK       (1<<3)
#define USBC_INTKNQFLSH_SHIFT      (3)
#define USBC_RXFFLSH               (1<<4)
#define USBC_RXFFLSH_MASK          (1<<4)
#define USBC_RXFFLSH_SHIFT         (4)
#define USBC_TXFFLSH               (1<<5)
#define USBC_TXFFLSH_MASK          (1<<5)
#define USBC_TXFFLSH_SHIFT         (5)
#define USBC_TXFNUM(n)             (((n)&31)<<6)
#define USBC_TXFNUM_MASK           (31<<6)
#define USBC_TXFNUM_SHIFT          (6)
#define USBC_DMAREQ                (1<<30)
#define USBC_DMAREQ_MASK           (1<<30)
#define USBC_DMAREQ_SHIFT          (30)
#define USBC_AHBIDLE               (1<<31)
#define USBC_AHBIDLE_MASK          (1<<31)
#define USBC_AHBIDLE_SHIFT         (31)

//GINTSTS
#define USBC_CURMOD                (1<<0)
#define USBC_CURMOD_MASK           (1<<0)
#define USBC_CURMOD_SHIFT          (0)
#define USBC_MODEMIS               (1<<1)
#define USBC_MODEMIS_MASK          (1<<1)
#define USBC_MODEMIS_SHIFT         (1)
#define USBC_OTGINT                (1<<2)
#define USBC_OTGINT_MASK           (1<<2)
#define USBC_OTGINT_SHIFT          (2)
#define USBC_SOF                   (1<<3)
#define USBC_SOF_MASK              (1<<3)
#define USBC_SOF_SHIFT             (3)
#define USBC_RXFLVL                (1<<4)
#define USBC_RXFLVL_MASK           (1<<4)
#define USBC_RXFLVL_SHIFT          (4)
#define USBC_NPTXFEMP              (1<<5)
#define USBC_NPTXFEMP_MASK         (1<<5)
#define USBC_NPTXFEMP_SHIFT        (5)
#define USBC_GINNAKEFF             (1<<6)
#define USBC_GINNAKEFF_MASK        (1<<6)
#define USBC_GINNAKEFF_SHIFT       (6)
#define USBC_GOUTNAKEFF            (1<<7)
#define USBC_GOUTNAKEFF_MASK       (1<<7)
#define USBC_GOUTNAKEFF_SHIFT      (7)
#define USBC_ULPICKINT             (1<<8)
#define USBC_ULPICKINT_MASK        (1<<8)
#define USBC_ULPICKINT_SHIFT       (8)
#define USBC_I2CINT                (1<<9)
#define USBC_I2CINT_MASK           (1<<9)
#define USBC_I2CINT_SHIFT          (9)
#define USBC_ERLYSUSP              (1<<10)
#define USBC_ERLYSUSP_MASK         (1<<10)
#define USBC_ERLYSUSP_SHIFT        (10)
#define USBC_USBSUSP               (1<<11)
#define USBC_USBSUSP_MASK          (1<<11)
#define USBC_USBSUSP_SHIFT         (11)
#define USBC_USBRST                (1<<12)
#define USBC_USBRST_MASK           (1<<12)
#define USBC_USBRST_SHIFT          (12)
#define USBC_ENUMDONE              (1<<13)
#define USBC_ENUMDONE_MASK         (1<<13)
#define USBC_ENUMDONE_SHIFT        (13)
#define USBC_ISOOUTDROP            (1<<14)
#define USBC_ISOOUTDROP_MASK       (1<<14)
#define USBC_ISOOUTDROP_SHIFT      (14)
#define USBC_EOPF                  (1<<15)
#define USBC_EOPF_MASK             (1<<15)
#define USBC_EOPF_SHIFT            (15)
#define USBC_EPMIS                 (1<<17)
#define USBC_EPMIS_MASK            (1<<17)
#define USBC_EPMIS_SHIFT           (17)
#define USBC_IEPINT                (1<<18)
#define USBC_IEPINT_MASK           (1<<18)
#define USBC_IEPINT_SHIFT          (18)
#define USBC_OEPINT                (1<<19)
#define USBC_OEPINT_MASK           (1<<19)
#define USBC_OEPINT_SHIFT          (19)
#define USBC_INCOMPLSOIN           (1<<20)
#define USBC_INCOMPLSOIN_MASK      (1<<20)
#define USBC_INCOMPLSOIN_SHIFT     (20)
#define USBC_INCOMPIP              (1<<21)
#define USBC_INCOMPIP_MASK         (1<<21)
#define USBC_INCOMPIP_SHIFT        (21)
#define USBC_FETSUSP               (1<<22)
#define USBC_FETSUSP_MASK          (1<<22)
#define USBC_FETSUSP_SHIFT         (22)
#define USBC_PRTINT                (1<<24)
#define USBC_PRTINT_MASK           (1<<24)
#define USBC_PRTINT_SHIFT          (24)
#define USBC_HCHINT                (1<<25)
#define USBC_HCHINT_MASK           (1<<25)
#define USBC_HCHINT_SHIFT          (25)
#define USBC_PTXFEMP               (1<<26)
#define USBC_PTXFEMP_MASK          (1<<26)
#define USBC_PTXFEMP_SHIFT         (26)
#define USBC_CONLDSTSCHNG          (1<<28)
#define USBC_CONLDSTSCHNG_MASK     (1<<28)
#define USBC_CONLDSTSCHNG_SHIFT    (28)
#define USBC_DISCONNINT            (1<<29)
#define USBC_DISCONNINT_MASK       (1<<29)
#define USBC_DISCONNINT_SHIFT      (29)
#define USBC_SESSREQINT            (1<<30)
#define USBC_SESSREQINT_MASK       (1<<30)
#define USBC_SESSREQINT_SHIFT      (30)
#define USBC_WKUPINT               (1<<31)
#define USBC_WKUPINT_MASK          (1<<31)
#define USBC_WKUPINT_SHIFT         (31)

//GINTMSK
//#define USBC_MODEMIS             (1<<1)
//#define USBC_MODEMIS_MASK        (1<<1)
//#define USBC_MODEMIS_SHIFT       (1)
//#define USBC_OTGINT              (1<<2)
//#define USBC_OTGINT_MASK         (1<<2)
//#define USBC_OTGINT_SHIFT        (2)
//#define USBC_SOF                 (1<<3)
//#define USBC_SOF_MASK            (1<<3)
//#define USBC_SOF_SHIFT           (3)
//#define USBC_RXFLVL              (1<<4)
//#define USBC_RXFLVL_MASK         (1<<4)
//#define USBC_RXFLVL_SHIFT        (4)
//#define USBC_NPTXFEMP            (1<<5)
//#define USBC_NPTXFEMP_MASK       (1<<5)
//#define USBC_NPTXFEMP_SHIFT      (5)
//#define USBC_GINNAKEFF           (1<<6)
//#define USBC_GINNAKEFF_MASK      (1<<6)
//#define USBC_GINNAKEFF_SHIFT     (6)
//#define USBC_GOUTNAKEFF          (1<<7)
//#define USBC_GOUTNAKEFF_MASK     (1<<7)
//#define USBC_GOUTNAKEFF_SHIFT    (7)
//#define USBC_ULPICKINT           (1<<8)
//#define USBC_ULPICKINT_MASK      (1<<8)
//#define USBC_ULPICKINT_SHIFT     (8)
//#define USBC_I2CINT              (1<<9)
//#define USBC_I2CINT_MASK         (1<<9)
//#define USBC_I2CINT_SHIFT        (9)
//#define USBC_ERLYSUSP            (1<<10)
//#define USBC_ERLYSUSP_MASK       (1<<10)
//#define USBC_ERLYSUSP_SHIFT      (10)
//#define USBC_USBSUSP             (1<<11)
//#define USBC_USBSUSP_MASK        (1<<11)
//#define USBC_USBSUSP_SHIFT       (11)
//#define USBC_USBRST              (1<<12)
//#define USBC_USBRST_MASK         (1<<12)
//#define USBC_USBRST_SHIFT        (12)
//#define USBC_ENUMDONE            (1<<13)
//#define USBC_ENUMDONE_MASK       (1<<13)
//#define USBC_ENUMDONE_SHIFT      (13)
//#define USBC_ISOOUTDROP          (1<<14)
//#define USBC_ISOOUTDROP_MASK     (1<<14)
//#define USBC_ISOOUTDROP_SHIFT    (14)
//#define USBC_EOPF                (1<<15)
//#define USBC_EOPF_MASK           (1<<15)
//#define USBC_EOPF_SHIFT          (15)
//#define USBC_EPMIS               (1<<17)
//#define USBC_EPMIS_MASK          (1<<17)
//#define USBC_EPMIS_SHIFT         (17)
//#define USBC_IEPINT              (1<<18)
//#define USBC_IEPINT_MASK         (1<<18)
//#define USBC_IEPINT_SHIFT        (18)
//#define USBC_OEPINT              (1<<19)
//#define USBC_OEPINT_MASK         (1<<19)
//#define USBC_OEPINT_SHIFT        (19)
//#define USBC_INCOMPLSOIN         (1<<20)
//#define USBC_INCOMPLSOIN_MASK    (1<<20)
//#define USBC_INCOMPLSOIN_SHIFT   (20)
//#define USBC_INCOMPIP            (1<<21)
//#define USBC_INCOMPIP_MASK       (1<<21)
//#define USBC_INCOMPIP_SHIFT      (21)
//#define USBC_FETSUSP             (1<<22)
//#define USBC_FETSUSP_MASK        (1<<22)
//#define USBC_FETSUSP_SHIFT       (22)
//#define USBC_PRTINT              (1<<24)
//#define USBC_PRTINT_MASK         (1<<24)
//#define USBC_PRTINT_SHIFT        (24)
//#define USBC_HCHINT              (1<<25)
//#define USBC_HCHINT_MASK         (1<<25)
//#define USBC_HCHINT_SHIFT        (25)
//#define USBC_PTXFEMP             (1<<26)
//#define USBC_PTXFEMP_MASK        (1<<26)
//#define USBC_PTXFEMP_SHIFT       (26)
//#define USBC_CONLDSTSCHNG        (1<<28)
//#define USBC_CONLDSTSCHNG_MASK   (1<<28)
//#define USBC_CONLDSTSCHNG_SHIFT  (28)
//#define USBC_DISCONNINT          (1<<29)
//#define USBC_DISCONNINT_MASK     (1<<29)
//#define USBC_DISCONNINT_SHIFT    (29)
//#define USBC_SESSREQINT          (1<<30)
//#define USBC_SESSREQINT_MASK     (1<<30)
//#define USBC_SESSREQINT_SHIFT    (30)
//#define USBC_WKUPINT             (1<<31)
//#define USBC_WKUPINT_MASK        (1<<31)
//#define USBC_WKUPINT_SHIFT       (31)

//GRXSTSR
#define USBC_EPNUM(n)              (((n)&15)<<0)
#define USBC_EPNUM_MASK            (15<<0)
#define USBC_EPNUM_SHIFT           (0)
#define USBC_BCNT(n)               (((n)&0x7FF)<<4)
#define USBC_BCNT_MASK             (0x7FF<<4)
#define USBC_BCNT_SHIFT            (4)
#define USBC_DPID(n)               (((n)&3)<<15)
#define USBC_DPID_MASK             (3<<15)
#define USBC_DPID_SHIFT            (15)
#define USBC_PKTSTS(n)             (((n)&15)<<17)
#define USBC_PKTSTS_MASK           (15<<17)
#define USBC_PKTSTS_SHIFT          (17)
#define USBC_FN(n)                 (((n)&15)<<21)
#define USBC_FN_MASK               (15<<21)
#define USBC_FN_SHIFT              (21)

//GRXSTSP
//#define USBC_EPNUM(n)            (((n)&15)<<0)
//#define USBC_EPNUM_MASK          (15<<0)
//#define USBC_EPNUM_SHIFT         (0)
//#define USBC_BCNT(n)             (((n)&0x7FF)<<4)
//#define USBC_BCNT_MASK           (0x7FF<<4)
//#define USBC_BCNT_SHIFT          (4)
//#define USBC_DPID(n)             (((n)&3)<<15)
//#define USBC_DPID_MASK           (3<<15)
//#define USBC_DPID_SHIFT          (15)
//#define USBC_PKTSTS(n)           (((n)&15)<<17)
//#define USBC_PKTSTS_MASK         (15<<17)
//#define USBC_PKTSTS_SHIFT        (17)
//#define USBC_FN(n)               (((n)&15)<<21)
//#define USBC_FN_MASK             (15<<21)
//#define USBC_FN_SHIFT            (21)

//GRXFSIZ
#define USBC_RXFDEP(n)             (((n)&0xFFFF)<<0)
#define USBC_RXFDEP_MASK           (0xFFFF<<0)
#define USBC_RXFDEP_SHIFT          (0)

//GNPTXFSIZ
#define USBC_NPTXFSTADDR(n)        (((n)&0xFFFF)<<0)
#define USBC_NPTXFSTADDR_MASK      (0xFFFF<<0)
#define USBC_NPTXFSTADDR_SHIFT     (0)
#define USBC_NPTXFDEPS(n)          (((n)&0xFFFF)<<16)
#define USBC_NPTXFDEPS_MASK        (0xFFFF<<16)
#define USBC_NPTXFDEPS_SHIFT       (16)

//GNPTXSTS
#define USBC_NPTXFSPCAVAIL(n)      (((n)&0xFFFF)<<0)
#define USBC_NPTXFSPCAVAIL_MASK    (0xFFFF<<0)
#define USBC_NPTXFSPCAVAIL_SHIFT   (0)
#define USBC_NPTXQSPCAVAIL(n)      (((n)&0xFF)<<16)
#define USBC_NPTXQSPCAVAIL_MASK    (0xFF<<16)
#define USBC_NPTXQSPCAVAIL_SHIFT   (16)
#define USBC_NPTXQTOP(n)           (((n)&0x7F)<<24)
#define USBC_NPTXQTOP_MASK         (0x7F<<24)
#define USBC_NPTXQTOP_SHIFT        (24)

//GUID
#define USBC_USERID(n)             (((n)&0xFFFFFFFF)<<0)
#define USBC_USERID_MASK           (0xFFFFFFFF<<0)
#define USBC_USERID_SHIFT          (0)

//GSNPSID
#define USBC_SYNOPSYSID(n)         (((n)&0xFFFFFFFF)<<0)
#define USBC_SYNOPSYSID_MASK       (0xFFFFFFFF<<0)
#define USBC_SYNOPSYSID_SHIFT      (0)

//GHWCFG1
#define USBC_EPDIR(n)              (((n)&0xFFFFFFFF)<<0)
#define USBC_EPDIR_MASK            (0xFFFFFFFF<<0)
#define USBC_EPDIR_SHIFT           (0)

//GHWCFG2
#define USBC_OTGMODE(n)            (((n)&7)<<0)
#define USBC_OTGMODE_MASK          (7<<0)
#define USBC_OTGMODE_SHIFT         (0)
#define USBC_OTGARCH(n)            (((n)&3)<<3)
#define USBC_OTGARCH_MASK          (3<<3)
#define USBC_OTGARCH_SHIFT         (3)
#define USBC_SINGPNT               (1<<5)
#define USBC_SINGPNT_MASK          (1<<5)
#define USBC_SINGPNT_SHIFT         (5)
#define USBC_HSPHYTYPE(n)          (((n)&3)<<6)
#define USBC_HSPHYTYPE_MASK        (3<<6)
#define USBC_HSPHYTYPE_SHIFT       (6)
#define USBC_FSPHYTYPE(n)          (((n)&3)<<8)
#define USBC_FSPHYTYPE_MASK        (3<<8)
#define USBC_FSPHYTYPE_SHIFT       (8)
#define USBC_NUMDEVEPS(n)          (((n)&15)<<10)
#define USBC_NUMDEVEPS_MASK        (15<<10)
#define USBC_NUMDEVEPS_SHIFT       (10)
#define USBC_NUMHSTCHNL(n)         (((n)&15)<<14)
#define USBC_NUMHSTCHNL_MASK       (15<<14)
#define USBC_NUMHSTCHNL_SHIFT      (14)
#define USBC_PERIOSUPPORT          (1<<18)
#define USBC_PERIOSUPPORT_MASK     (1<<18)
#define USBC_PERIOSUPPORT_SHIFT    (18)
#define USBC_DYNFIFOSIZING         (1<<19)
#define USBC_DYNFIFOSIZING_MASK    (1<<19)
#define USBC_DYNFIFOSIZING_SHIFT   (19)
#define USBC_NPTXQDEPTH(n)         (((n)&3)<<22)
#define USBC_NPTXQDEPTH_MASK       (3<<22)
#define USBC_NPTXQDEPTH_SHIFT      (22)
#define USBC_PTXQDEPTH(n)          (((n)&3)<<24)
#define USBC_PTXQDEPTH_MASK        (3<<24)
#define USBC_PTXQDEPTH_SHIFT       (24)
#define USBC_TKNQDEPTH(n)          (((n)&31)<<26)
#define USBC_TKNQDEPTH_MASK        (31<<26)
#define USBC_TKNQDEPTH_SHIFT       (26)

//GHWCFG3
#define USBC_XFERSIZEWIDTH(n)      (((n)&15)<<0)
#define USBC_XFERSIZEWIDTH_MASK    (15<<0)
#define USBC_XFERSIZEWIDTH_SHIFT   (0)
#define USBC_PKTSIZEWIDTH(n)       (((n)&7)<<4)
#define USBC_PKTSIZEWIDTH_MASK     (7<<4)
#define USBC_PKTSIZEWIDTH_SHIFT    (4)
#define USBC_OTGEN                 (1<<7)
#define USBC_OTGEN_MASK            (1<<7)
#define USBC_OTGEN_SHIFT           (7)
#define USBC_I2CINTSEL             (1<<8)
#define USBC_I2CINTSEL_MASK        (1<<8)
#define USBC_I2CINTSEL_SHIFT       (8)
#define USBC_VNDCTLSUPT            (1<<9)
#define USBC_VNDCTLSUPT_MASK       (1<<9)
#define USBC_VNDCTLSUPT_SHIFT      (9)
#define USBC_OPTFEATURE            (1<<10)
#define USBC_OPTFEATURE_MASK       (1<<10)
#define USBC_OPTFEATURE_SHIFT      (10)
#define USBC_RSTTYPE               (1<<11)
#define USBC_RSTTYPE_MASK          (1<<11)
#define USBC_RSTTYPE_SHIFT         (11)
#define USBC_DFIFODEPTH(n)         (((n)&0xFFFF)<<16)
#define USBC_DFIFODEPTH_MASK       (0xFFFF<<16)
#define USBC_DFIFODEPTH_SHIFT      (16)

//GHWCFG4
#define USBC_NUMDEVPERIOEPS(n)     (((n)&15)<<0)
#define USBC_NUMDEVPERIOEPS_MASK   (15<<0)
#define USBC_NUMDEVPERIOEPS_SHIFT  (0)
#define USBC_ENABLEPWROPT          (1<<4)
#define USBC_ENABLEPWROPT_MASK     (1<<4)
#define USBC_ENABLEPWROPT_SHIFT    (4)
#define USBC_AHBFREQ               (1<<5)
#define USBC_AHBFREQ_MASK          (1<<5)
#define USBC_AHBFREQ_SHIFT         (5)
#define USBC_PHYDATAWIDTH(n)       (((n)&3)<<14)
#define USBC_PHYDATAWIDTH_MASK     (3<<14)
#define USBC_PHYDATAWIDTH_SHIFT    (14)
#define USBC_NUMCTLEPS(n)          (((n)&15)<<16)
#define USBC_NUMCTLEPS_MASK        (15<<16)
#define USBC_NUMCTLEPS_SHIFT       (16)
#define USBC_IDDIGFLTR             (1<<20)
#define USBC_IDDIGFLTR_MASK        (1<<20)
#define USBC_IDDIGFLTR_SHIFT       (20)
#define USBC_VBUSVALIDFLTR         (1<<21)
#define USBC_VBUSVALIDFLTR_MASK    (1<<21)
#define USBC_VBUSVALIDFLTR_SHIFT   (21)
#define USBC_AVALIDFLTR            (1<<22)
#define USBC_AVALIDFLTR_MASK       (1<<22)
#define USBC_AVALIDFLTR_SHIFT      (22)
#define USBC_BVALIDFLTR            (1<<23)
#define USBC_BVALIDFLTR_MASK       (1<<23)
#define USBC_BVALIDFLTR_SHIFT      (23)
#define USBC_SESSENDFLTR           (1<<24)
#define USBC_SESSENDFLTR_MASK      (1<<24)
#define USBC_SESSENDFLTR_SHIFT     (24)
#define USBC_DEFIFOMODE            (1<<25)
#define USBC_DEFIFOMODE_MASK       (1<<25)
#define USBC_DEFIFOMODE_SHIFT      (25)
#define USBC_INEPS(n)              (((n)&15)<<26)
#define USBC_INEPS_MASK            (15<<26)
#define USBC_INEPS_SHIFT           (26)

//DIEnPTXF
#define USBC_IENPNTXFSTADDR(n)     (((n)&0xFFFF)<<0)
#define USBC_IENPNTXFSTADDR_MASK   (0xFFFF<<0)
#define USBC_IENPNTXFSTADDR_SHIFT  (0)
#define USBC_INEPNTXFDEP(n)        (((n)&0xFFFF)<<16)
#define USBC_INEPNTXFDEP_MASK      (0xFFFF<<16)
#define USBC_INEPNTXFDEP_SHIFT     (16)

//DCFG
#define USBC_DEVSPD(n)             (((n)&3)<<0)
#define USBC_DEVSPD_MASK           (3<<0)
#define USBC_DEVSPD_SHIFT          (0)
#define USBC_NZSTSOUTHSHK          (1<<2)
#define USBC_NZSTSOUTHSHK_MASK     (1<<2)
#define USBC_NZSTSOUTHSHK_SHIFT    (2)
#define USBC_DEVADDR(n)            (((n)&0x7F)<<4)
#define USBC_DEVADDR_MASK          (0x7F<<4)
#define USBC_DEVADDR_SHIFT         (4)
#define USBC_PERFRINT(n)           (((n)&3)<<11)
#define USBC_PERFRINT_MASK         (3<<11)
#define USBC_PERFRINT_SHIFT        (11)
#define USBC_EPMISCNT(n)           (((n)&31)<<18)
#define USBC_EPMISCNT_MASK         (31<<18)
#define USBC_EPMISCNT_SHIFT        (18)

//DCTL
#define USBC_RMTWKUPSIG            (1<<0)
#define USBC_RMTWKUPSIG_MASK       (1<<0)
#define USBC_RMTWKUPSIG_SHIFT      (0)
#define USBC_SFTDISCON             (1<<1)
#define USBC_SFTDISCON_MASK        (1<<1)
#define USBC_SFTDISCON_SHIFT       (1)
#define USBC_GNPINNAKSTS           (1<<2)
#define USBC_GNPINNAKSTS_MASK      (1<<2)
#define USBC_GNPINNAKSTS_SHIFT     (2)
#define USBC_GOUTNAKSTS            (1<<3)
#define USBC_GOUTNAKSTS_MASK       (1<<3)
#define USBC_GOUTNAKSTS_SHIFT      (3)
#define USBC_TSTCTL(n)             (((n)&7)<<4)
#define USBC_TSTCTL_MASK           (7<<4)
#define USBC_TSTCTL_SHIFT          (4)
#define USBC_SGNPINNAK             (1<<7)
#define USBC_SGNPINNAK_MASK        (1<<7)
#define USBC_SGNPINNAK_SHIFT       (7)
#define USBC_CGNPINNAK             (1<<8)
#define USBC_CGNPINNAK_MASK        (1<<8)
#define USBC_CGNPINNAK_SHIFT       (8)
#define USBC_SGOUTNAK              (1<<9)
#define USBC_SGOUTNAK_MASK         (1<<9)
#define USBC_SGOUTNAK_SHIFT        (9)
#define USBC_CGOUTNAK              (1<<10)
#define USBC_CGOUTNAK_MASK         (1<<10)
#define USBC_CGOUTNAK_SHIFT        (10)
#define USBC_PWRONPRGDONE          (1<<11)
#define USBC_PWRONPRGDONE_MASK     (1<<11)
#define USBC_PWRONPRGDONE_SHIFT    (11)

//DSTS
#define USBC_SUSPSTS               (1<<0)
#define USBC_SUSPSTS_MASK          (1<<0)
#define USBC_SUSPSTS_SHIFT         (0)
#define USBC_ENUMSPD(n)            (((n)&3)<<1)
#define USBC_ENUMSPD_MASK          (3<<1)
#define USBC_ENUMSPD_SHIFT         (1)
#define USBC_ERRTICERR             (1<<3)
#define USBC_ERRTICERR_MASK        (1<<3)
#define USBC_ERRTICERR_SHIFT       (3)
#define USBC_SOFFN(n)              (((n)&0x3FFF)<<8)
#define USBC_SOFFN_MASK            (0x3FFF<<8)
#define USBC_SOFFN_SHIFT           (8)

//DIEPMSK
#define USBC_XFERCOMPLMSK          (1<<0)
#define USBC_XFERCOMPLMSK_MASK     (1<<0)
#define USBC_XFERCOMPLMSK_SHIFT    (0)
#define USBC_EPDISBLDMSK           (1<<1)
#define USBC_EPDISBLDMSK_MASK      (1<<1)
#define USBC_EPDISBLDMSK_SHIFT     (1)
#define USBC_AHBERRMSK             (1<<2)
#define USBC_AHBERRMSK_MASK        (1<<2)
#define USBC_AHBERRMSK_SHIFT       (2)
#define USBC_TIMEOUTMSK            (1<<3)
#define USBC_TIMEOUTMSK_MASK       (1<<3)
#define USBC_TIMEOUTMSK_SHIFT      (3)
#define USBC_INTKNTXFEMPMSK        (1<<4)
#define USBC_INTKNTXFEMPMSK_MASK   (1<<4)
#define USBC_INTKNTXFEMPMSK_SHIFT  (4)
#define USBC_INTKNEPMISMSK         (1<<5)
#define USBC_INTKNEPMISMSK_MASK    (1<<5)
#define USBC_INTKNEPMISMSK_SHIFT   (5)
#define USBC_INEPNAKEFFMSK         (1<<6)
#define USBC_INEPNAKEFFMSK_MASK    (1<<6)
#define USBC_INEPNAKEFFMSK_SHIFT   (6)
#define USBC_TXFIFOUNDRNMSK        (1<<8)
#define USBC_TXFIFOUNDRNMSK_MASK   (1<<8)
#define USBC_TXFIFOUNDRNMSK_SHIFT  (8)

//DOEPMSK
//#define USBC_XFERCOMPLMSK        (1<<0)
//#define USBC_XFERCOMPLMSK_MASK   (1<<0)
//#define USBC_XFERCOMPLMSK_SHIFT  (0)
//#define USBC_EPDISBLDMSK         (1<<1)
//#define USBC_EPDISBLDMSK_MASK    (1<<1)
//#define USBC_EPDISBLDMSK_SHIFT   (1)
//#define USBC_AHBERRMSK           (1<<2)
//#define USBC_AHBERRMSK_MASK      (1<<2)
//#define USBC_AHBERRMSK_SHIFT     (2)
#define USBC_SETUPMK               (1<<3)
#define USBC_SETUPMK_MASK          (1<<3)
#define USBC_SETUPMK_SHIFT         (3)
#define USBC_OUTTKNEPDISMSK        (1<<4)
#define USBC_OUTTKNEPDISMSK_MASK   (1<<4)
#define USBC_OUTTKNEPDISMSK_SHIFT  (4)
#define USBC_BACK2BACKSETUP        (1<<6)
#define USBC_BACK2BACKSETUP_MASK   (1<<6)
#define USBC_BACK2BACKSETUP_SHIFT  (6)
#define USBC_OUTPKTERRMSK          (1<<8)
#define USBC_OUTPKTERRMSK_MASK     (1<<8)
#define USBC_OUTPKTERRMSK_SHIFT    (8)

//DAINT
#define USBC_INEPINT(n)            (((n)&0xFFFF)<<0)
#define USBC_INEPINT_MASK          (0xFFFF<<0)
#define USBC_INEPINT_SHIFT         (0)
#define USBC_OUTEPINT(n)           (((n)&0xFFFF)<<16)
#define USBC_OUTEPINT_MASK         (0xFFFF<<16)
#define USBC_OUTEPINT_SHIFT        (16)

//DAINTMSK
#define USBC_INEPMSK(n)            (((n)&0xFFFF)<<0)
#define USBC_INEPMSK_MASK          (0xFFFF<<0)
#define USBC_INEPMSK_SHIFT         (0)
#define USBC_OUTEPMSK(n)           (((n)&0xFFFF)<<16)
#define USBC_OUTEPMSK_MASK         (0xFFFF<<16)
#define USBC_OUTEPMSK_SHIFT        (16)

//DVBUSDIS
#define USBC_DVBUSDIS(n)           (((n)&0xFFFF)<<0)
#define USBC_DVBUSDIS_MASK         (0xFFFF<<0)
#define USBC_DVBUSDIS_SHIFT        (0)

//DVBUSPULSE
#define USBC_DVBUSPULSE(n)         (((n)&0xFFF)<<0)
#define USBC_DVBUSPULSE_MASK       (0xFFF<<0)
#define USBC_DVBUSPULSE_SHIFT      (0)

//DTHRCTL
#define USBC_NONISOTHREN           (1<<0)
#define USBC_NONISOTHREN_MASK      (1<<0)
#define USBC_NONISOTHREN_SHIFT     (0)
#define USBC_ISOTHREN              (1<<1)
#define USBC_ISOTHREN_MASK         (1<<1)
#define USBC_ISOTHREN_SHIFT        (1)
#define USBC_TXTHRLEN(n)           (((n)&0x1FF)<<2)
#define USBC_TXTHRLEN_MASK         (0x1FF<<2)
#define USBC_TXTHRLEN_SHIFT        (2)
#define USBC_RXTHREN               (1<<16)
#define USBC_RXTHREN_MASK          (1<<16)
#define USBC_RXTHREN_SHIFT         (16)
#define USBC_RXTHRLEN(n)           (((n)&0x1FF)<<17)
#define USBC_RXTHRLEN_MASK         (0x1FF<<17)
#define USBC_RXTHRLEN_SHIFT        (17)
#define USBC_ARBPRKEN              (1<<27)
#define USBC_ARBPRKEN_MASK         (1<<27)
#define USBC_ARBPRKEN_SHIFT        (27)

//DIEPEMPMSK
#define USBC_INEPTXFEMPMSK(n)      (((n)&0xFFFF)<<0)
#define USBC_INEPTXFEMPMSK_MASK    (0xFFFF<<0)
#define USBC_INEPTXFEMPMSK_SHIFT   (0)

//DIEPCTL0
#define USBC_EP0_MPS(n)            (((n)&3)<<0)
#define USBC_EP0_MPS_MASK          (3<<0)
#define USBC_EP0_MPS_SHIFT         (0)
#define USBC_NEXTEP(n)             (((n)&15)<<11)
#define USBC_NEXTEP_MASK           (15<<11)
#define USBC_NEXTEP_SHIFT          (11)
#define USBC_USBACTEP              (1<<15)
#define USBC_USBACTEP_MASK         (1<<15)
#define USBC_USBACTEP_SHIFT        (15)
#define USBC_NAKSTS                (1<<17)
#define USBC_NAKSTS_MASK           (1<<17)
#define USBC_NAKSTS_SHIFT          (17)
#define USBC_EPTYPE(n)             (((n)&3)<<18)
#define USBC_EPTYPE_MASK           (3<<18)
#define USBC_EPTYPE_SHIFT          (18)
#define USBC_STALL                 (1<<21)
#define USBC_STALL_MASK            (1<<21)
#define USBC_STALL_SHIFT           (21)
#define USBC_EPTXFNUM(n)           (((n)&15)<<22)
#define USBC_EPTXFNUM_MASK         (15<<22)
#define USBC_EPTXFNUM_SHIFT        (22)
#define USBC_CNAK                  (1<<26)
#define USBC_CNAK_MASK             (1<<26)
#define USBC_CNAK_SHIFT            (26)
#define USBC_SNAK                  (1<<27)
#define USBC_SNAK_MASK             (1<<27)
#define USBC_SNAK_SHIFT            (27)
#define USBC_EPDIS                 (1<<30)
#define USBC_EPDIS_MASK            (1<<30)
#define USBC_EPDIS_SHIFT           (30)
#define USBC_EPENA                 (1<<31)
#define USBC_EPENA_MASK            (1<<31)
#define USBC_EPENA_SHIFT           (31)

//DIEPINT0
#define USBC_XFERCOMPL             (1<<0)
#define USBC_XFERCOMPL_MASK        (1<<0)
#define USBC_XFERCOMPL_SHIFT       (0)
#define USBC_EPDISBLD              (1<<1)
#define USBC_EPDISBLD_MASK         (1<<1)
#define USBC_EPDISBLD_SHIFT        (1)
#define USBC_AHBERR                (1<<2)
#define USBC_AHBERR_MASK           (1<<2)
#define USBC_AHBERR_SHIFT          (2)
#define USBC_TIMEOUT               (1<<3)
#define USBC_TIMEOUT_MASK          (1<<3)
#define USBC_TIMEOUT_SHIFT         (3)
#define USBC_INTKNTXFEMP           (1<<4)
#define USBC_INTKNTXFEMP_MASK      (1<<4)
#define USBC_INTKNTXFEMP_SHIFT     (4)
#define USBC_INTKNEPMIS            (1<<5)
#define USBC_INTKNEPMIS_MASK       (1<<5)
#define USBC_INTKNEPMIS_SHIFT      (5)
#define USBC_INEPNAKEFF            (1<<6)
#define USBC_INEPNAKEFF_MASK       (1<<6)
#define USBC_INEPNAKEFF_SHIFT      (6)
#define USBC_TXFEMP                (1<<7)
#define USBC_TXFEMP_MASK           (1<<7)
#define USBC_TXFEMP_SHIFT          (7)
#define USBC_TXFIFOUNDRN           (1<<8)
#define USBC_TXFIFOUNDRN_MASK      (1<<8)
#define USBC_TXFIFOUNDRN_SHIFT     (8)

//DIEPTSIZ0
#define USBC_IEPXFERSIZE0(n)       (((n)&0x7F)<<0)
#define USBC_IEPXFERSIZE0_MASK     (0x7F<<0)
#define USBC_IEPXFERSIZE0_SHIFT    (0)
#define USBC_IEPPKTCNT0(n)         (((n)&3)<<19)
#define USBC_IEPPKTCNT0_MASK       (3<<19)
#define USBC_IEPPKTCNT0_SHIFT      (19)

//DIEPDMA0
#define USBC_DMAADDR(n)            (((n)&0xFFFFFFFF)<<0)
#define USBC_DMAADDR_MASK          (0xFFFFFFFF<<0)
#define USBC_DMAADDR_SHIFT         (0)

//DIEPFSTS0
#define USBC_INEPTXFSPCAVAIL(n)    (((n)&0xFFFF)<<0)
#define USBC_INEPTXFSPCAVAIL_MASK  (0xFFFF<<0)
#define USBC_INEPTXFSPCAVAIL_SHIFT (0)

//DIEPCTL
#define USBC_EPN_MPS(n)            (((n)&0x7FF)<<0)
#define USBC_EPN_MPS_MASK          (0x7FF<<0)
#define USBC_EPN_MPS_SHIFT         (0)
//#define USBC_NEXTEP(n)           (((n)&15)<<11)
//#define USBC_NEXTEP_MASK         (15<<11)
//#define USBC_NEXTEP_SHIFT        (11)
//#define USBC_USBACTEP            (1<<15)
//#define USBC_USBACTEP_MASK       (1<<15)
//#define USBC_USBACTEP_SHIFT      (15)
#define USBC_IEP_DPID              (1<<16)
#define USBC_IEP_DPID_MASK         (1<<16)
#define USBC_IEP_DPID_SHIFT        (16)
//#define USBC_NAKSTS              (1<<17)
//#define USBC_NAKSTS_MASK         (1<<17)
//#define USBC_NAKSTS_SHIFT        (17)
//#define USBC_EPTYPE(n)           (((n)&3)<<18)
//#define USBC_EPTYPE_MASK         (3<<18)
//#define USBC_EPTYPE_SHIFT        (18)
#define USBC_SNP                   (1<<20)
#define USBC_SNP_MASK              (1<<20)
#define USBC_SNP_SHIFT             (20)
//#define USBC_STALL               (1<<21)
//#define USBC_STALL_MASK          (1<<21)
//#define USBC_STALL_SHIFT         (21)
//#define USBC_EPTXFNUM(n)         (((n)&15)<<22)
//#define USBC_EPTXFNUM_MASK       (15<<22)
//#define USBC_EPTXFNUM_SHIFT      (22)
//#define USBC_CNAK                (1<<26)
//#define USBC_CNAK_MASK           (1<<26)
//#define USBC_CNAK_SHIFT          (26)
//#define USBC_SNAK                (1<<27)
//#define USBC_SNAK_MASK           (1<<27)
//#define USBC_SNAK_SHIFT          (27)
#define USBC_SETD0PID              (1<<28)
#define USBC_SETD0PID_MASK         (1<<28)
#define USBC_SETD0PID_SHIFT        (28)
#define USBC_SETD1PID              (1<<29)
#define USBC_SETD1PID_MASK         (1<<29)
#define USBC_SETD1PID_SHIFT        (29)
//#define USBC_EPDIS               (1<<30)
//#define USBC_EPDIS_MASK          (1<<30)
//#define USBC_EPDIS_SHIFT         (30)
//#define USBC_EPENA               (1<<31)
//#define USBC_EPENA_MASK          (1<<31)
//#define USBC_EPENA_SHIFT         (31)

//DIEPINT
//#define USBC_XFERCOMPL           (1<<0)
//#define USBC_XFERCOMPL_MASK      (1<<0)
//#define USBC_XFERCOMPL_SHIFT     (0)
//#define USBC_EPDISBLD            (1<<1)
//#define USBC_EPDISBLD_MASK       (1<<1)
//#define USBC_EPDISBLD_SHIFT      (1)
//#define USBC_AHBERR              (1<<2)
//#define USBC_AHBERR_MASK         (1<<2)
//#define USBC_AHBERR_SHIFT        (2)
//#define USBC_TIMEOUT             (1<<3)
//#define USBC_TIMEOUT_MASK        (1<<3)
//#define USBC_TIMEOUT_SHIFT       (3)
//#define USBC_INTKNTXFEMP         (1<<4)
//#define USBC_INTKNTXFEMP_MASK    (1<<4)
//#define USBC_INTKNTXFEMP_SHIFT   (4)
//#define USBC_INTKNEPMIS          (1<<5)
//#define USBC_INTKNEPMIS_MASK     (1<<5)
//#define USBC_INTKNEPMIS_SHIFT    (5)
//#define USBC_INEPNAKEFF          (1<<6)
//#define USBC_INEPNAKEFF_MASK     (1<<6)
//#define USBC_INEPNAKEFF_SHIFT    (6)
//#define USBC_TXFEMP              (1<<7)
//#define USBC_TXFEMP_MASK         (1<<7)
//#define USBC_TXFEMP_SHIFT        (7)
//#define USBC_TXFIFOUNDRN         (1<<8)
//#define USBC_TXFIFOUNDRN_MASK    (1<<8)
//#define USBC_TXFIFOUNDRN_SHIFT   (8)

//DIEPTSIZ
#define USBC_IEPXFERSIZE(n)        (((n)&0x7FFFF)<<0)
#define USBC_IEPXFERSIZE_MASK      (0x7FFFF<<0)
#define USBC_IEPXFERSIZE_SHIFT     (0)
#define USBC_IEPPKTCNT(n)          (((n)&0x3FF)<<19)
#define USBC_IEPPKTCNT_MASK        (0x3FF<<19)
#define USBC_IEPPKTCNT_SHIFT       (19)
#define USBC_MC(n)                 (((n)&3)<<29)
#define USBC_MC_MASK               (3<<29)
#define USBC_MC_SHIFT              (29)

//DIEPDMA
//#define USBC_DMAADDR(n)          (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK        (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT       (0)

//DIEPFSTS
//#define USBC_INEPTXFSPCAVAIL(n)  (((n)&0xFFFF)<<0)
//#define USBC_INEPTXFSPCAVAIL_MASK (0xFFFF<<0)
//#define USBC_INEPTXFSPCAVAIL_SHIFT (0)

//DOEPCTL0
//#define USBC_EP0_MPS(n)          (((n)&3)<<0)
//#define USBC_EP0_MPS_MASK        (3<<0)
//#define USBC_EP0_MPS_SHIFT       (0)
//#define USBC_USBACTEP            (1<<15)
//#define USBC_USBACTEP_MASK       (1<<15)
//#define USBC_USBACTEP_SHIFT      (15)
//#define USBC_NAKSTS              (1<<17)
//#define USBC_NAKSTS_MASK         (1<<17)
//#define USBC_NAKSTS_SHIFT        (17)
//#define USBC_EPTYPE(n)           (((n)&3)<<18)
//#define USBC_EPTYPE_MASK         (3<<18)
//#define USBC_EPTYPE_SHIFT        (18)
//#define USBC_SNP                 (1<<20)
//#define USBC_SNP_MASK            (1<<20)
//#define USBC_SNP_SHIFT           (20)
//#define USBC_STALL               (1<<21)
//#define USBC_STALL_MASK          (1<<21)
//#define USBC_STALL_SHIFT         (21)
//#define USBC_CNAK                (1<<26)
//#define USBC_CNAK_MASK           (1<<26)
//#define USBC_CNAK_SHIFT          (26)
//#define USBC_SNAK                (1<<27)
//#define USBC_SNAK_MASK           (1<<27)
//#define USBC_SNAK_SHIFT          (27)
//#define USBC_EPDIS               (1<<30)
//#define USBC_EPDIS_MASK          (1<<30)
//#define USBC_EPDIS_SHIFT         (30)
//#define USBC_EPENA               (1<<31)
//#define USBC_EPENA_MASK          (1<<31)
//#define USBC_EPENA_SHIFT         (31)

//DOEPINT0
//#define USBC_XFERCOMPL           (1<<0)
//#define USBC_XFERCOMPL_MASK      (1<<0)
//#define USBC_XFERCOMPL_SHIFT     (0)
//#define USBC_EPDISBLD            (1<<1)
//#define USBC_EPDISBLD_MASK       (1<<1)
//#define USBC_EPDISBLD_SHIFT      (1)
//#define USBC_AHBERR              (1<<2)
//#define USBC_AHBERR_MASK         (1<<2)
//#define USBC_AHBERR_SHIFT        (2)
#define USBC_SETUP                 (1<<3)
#define USBC_SETUP_MASK            (1<<3)
#define USBC_SETUP_SHIFT           (3)
//#define USBC_INTKNTXFEMP         (1<<4)
//#define USBC_INTKNTXFEMP_MASK    (1<<4)
//#define USBC_INTKNTXFEMP_SHIFT   (4)
//#define USBC_INTKNEPMIS          (1<<5)
//#define USBC_INTKNEPMIS_MASK     (1<<5)
//#define USBC_INTKNEPMIS_SHIFT    (5)
//#define USBC_INEPNAKEFF          (1<<6)
//#define USBC_INEPNAKEFF_MASK     (1<<6)
//#define USBC_INEPNAKEFF_SHIFT    (6)
//#define USBC_TXFEMP              (1<<7)
//#define USBC_TXFEMP_MASK         (1<<7)
//#define USBC_TXFEMP_SHIFT        (7)
//#define USBC_TXFIFOUNDRN         (1<<8)
//#define USBC_TXFIFOUNDRN_MASK    (1<<8)
//#define USBC_TXFIFOUNDRN_SHIFT   (8)

//DOEPTSIZ0
#define USBC_OEPXFERSIZE0(n)       (((n)&0x7F)<<0)
#define USBC_OEPXFERSIZE0_MASK     (0x7F<<0)
#define USBC_OEPXFERSIZE0_SHIFT    (0)
#define USBC_OEPPKTCNT0            (1<<19)
#define USBC_OEPPKTCNT0_MASK       (1<<19)
#define USBC_OEPPKTCNT0_SHIFT      (19)
#define USBC_SUPCNT(n)             (((n)&3)<<29)
#define USBC_SUPCNT_MASK           (3<<29)
#define USBC_SUPCNT_SHIFT          (29)

//DOEPDMA0
//#define USBC_DMAADDR(n)          (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK        (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT       (0)

//DOEPCTL
//#define USBC_EPN_MPS(n)          (((n)&0x7FF)<<0)
//#define USBC_EPN_MPS_MASK        (0x7FF<<0)
//#define USBC_EPN_MPS_SHIFT       (0)
//#define USBC_NEXTEP(n)           (((n)&15)<<11)
//#define USBC_NEXTEP_MASK         (15<<11)
//#define USBC_NEXTEP_SHIFT        (11)
//#define USBC_USBACTEP            (1<<15)
//#define USBC_USBACTEP_MASK       (1<<15)
//#define USBC_USBACTEP_SHIFT      (15)
#define USBC_OEP_DPID              (1<<16)
#define USBC_OEP_DPID_MASK         (1<<16)
#define USBC_OEP_DPID_SHIFT        (16)
//#define USBC_NAKSTS              (1<<17)
//#define USBC_NAKSTS_MASK         (1<<17)
//#define USBC_NAKSTS_SHIFT        (17)
//#define USBC_EPTYPE(n)           (((n)&3)<<18)
//#define USBC_EPTYPE_MASK         (3<<18)
//#define USBC_EPTYPE_SHIFT        (18)
//#define USBC_SNP                 (1<<20)
//#define USBC_SNP_MASK            (1<<20)
//#define USBC_SNP_SHIFT           (20)
//#define USBC_STALL               (1<<21)
//#define USBC_STALL_MASK          (1<<21)
//#define USBC_STALL_SHIFT         (21)
//#define USBC_EPTXFNUM(n)         (((n)&15)<<22)
//#define USBC_EPTXFNUM_MASK       (15<<22)
//#define USBC_EPTXFNUM_SHIFT      (22)
//#define USBC_CNAK                (1<<26)
//#define USBC_CNAK_MASK           (1<<26)
//#define USBC_CNAK_SHIFT          (26)
//#define USBC_SNAK                (1<<27)
//#define USBC_SNAK_MASK           (1<<27)
//#define USBC_SNAK_SHIFT          (27)
//#define USBC_SETD0PID            (1<<28)
//#define USBC_SETD0PID_MASK       (1<<28)
//#define USBC_SETD0PID_SHIFT      (28)
//#define USBC_SETD1PID            (1<<29)
//#define USBC_SETD1PID_MASK       (1<<29)
//#define USBC_SETD1PID_SHIFT      (29)
//#define USBC_EPDIS               (1<<30)
//#define USBC_EPDIS_MASK          (1<<30)
//#define USBC_EPDIS_SHIFT         (30)
//#define USBC_EPENA               (1<<31)
//#define USBC_EPENA_MASK          (1<<31)
//#define USBC_EPENA_SHIFT         (31)

//DOEPINT
//#define USBC_XFERCOMPL           (1<<0)
//#define USBC_XFERCOMPL_MASK      (1<<0)
//#define USBC_XFERCOMPL_SHIFT     (0)
//#define USBC_EPDISBLD            (1<<1)
//#define USBC_EPDISBLD_MASK       (1<<1)
//#define USBC_EPDISBLD_SHIFT      (1)
//#define USBC_AHBERR              (1<<2)
//#define USBC_AHBERR_MASK         (1<<2)
//#define USBC_AHBERR_SHIFT        (2)
//#define USBC_SETUP               (1<<3)
//#define USBC_SETUP_MASK          (1<<3)
//#define USBC_SETUP_SHIFT         (3)
//#define USBC_INTKNTXFEMP         (1<<4)
//#define USBC_INTKNTXFEMP_MASK    (1<<4)
//#define USBC_INTKNTXFEMP_SHIFT   (4)
//#define USBC_INTKNEPMIS          (1<<5)
//#define USBC_INTKNEPMIS_MASK     (1<<5)
//#define USBC_INTKNEPMIS_SHIFT    (5)
//#define USBC_INEPNAKEFF          (1<<6)
//#define USBC_INEPNAKEFF_MASK     (1<<6)
//#define USBC_INEPNAKEFF_SHIFT    (6)
//#define USBC_TXFEMP              (1<<7)
//#define USBC_TXFEMP_MASK         (1<<7)
//#define USBC_TXFEMP_SHIFT        (7)
//#define USBC_TXFIFOUNDRN         (1<<8)
//#define USBC_TXFIFOUNDRN_MASK    (1<<8)
//#define USBC_TXFIFOUNDRN_SHIFT   (8)

//DOEPTSIZ
#define USBC_OEPXFERSIZE(n)        (((n)&0x7FFFF)<<0)
#define USBC_OEPXFERSIZE_MASK      (0x7FFFF<<0)
#define USBC_OEPXFERSIZE_SHIFT     (0)
#define USBC_OEPPKTCNT(n)          (((n)&0x3FF)<<19)
#define USBC_OEPPKTCNT_MASK        (0x3FF<<19)
#define USBC_OEPPKTCNT_SHIFT       (19)
//#define USBC_MC(n)               (((n)&3)<<29)
//#define USBC_MC_MASK             (3<<29)
//#define USBC_MC_SHIFT            (29)

//DOEPDMA
//#define USBC_DMAADDR(n)          (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK        (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT       (0)

//PCGCCTL
#define USBC_STOPPCLK              (1<<0)
#define USBC_STOPPCLK_MASK         (1<<0)
#define USBC_STOPPCLK_SHIFT        (0)
#define USBC_GATEHCLK              (1<<1)
#define USBC_GATEHCLK_MASK         (1<<1)
#define USBC_GATEHCLK_SHIFT        (1)
#define USBC_PWRCLMP               (1<<2)
#define USBC_PWRCLMP_MASK          (1<<2)
#define USBC_PWRCLMP_SHIFT         (2)
#define USBC_RSTPDWNMODULE         (1<<3)
#define USBC_RSTPDWNMODULE_MASK    (1<<3)
#define USBC_RSTPDWNMODULE_SHIFT   (3)
#define USBC_PHYSUSPENDED          (1<<4)
#define USBC_PHYSUSPENDED_MASK     (1<<4)
#define USBC_PHYSUSPENDED_SHIFT    (4)

//TxRxData
#define USBC_TXDATA(n)             (((n)&0xFFFFFFFF)<<0)
#define USBC_TXDATA_MASK           (0xFFFFFFFF<<0)
#define USBC_TXDATA_SHIFT          (0)
#define USBC_RXDATA(n)             (((n)&0xFFFFFFFF)<<0)
#define USBC_RXDATA_MASK           (0xFFFFFFFF<<0)
#define USBC_RXDATA_SHIFT          (0)

//RAMData
#define USBC_DATA(n)               (((n)&0xFFFFFFFF)<<0)
#define USBC_DATA_MASK             (0xFFFFFFFF<<0)
#define USBC_DATA_SHIFT            (0)





#endif


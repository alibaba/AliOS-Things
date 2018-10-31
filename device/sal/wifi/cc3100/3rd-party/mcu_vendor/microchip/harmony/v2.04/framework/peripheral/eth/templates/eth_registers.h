//DOM-IGNORE-END
#include <xc.h>

#ifndef _ETH_STRUCTURE_H
#define _ETH_STRUCTURE_H

typedef struct __attribute__ ((packed , aligned(4)))
{
 __ETHCON1bits_t	ETHCON1;
volatile unsigned int        ETHCON1CLR ;
volatile unsigned int        ETHCON1SET ;
volatile unsigned int        ETHCON1INV ;

 __ETHCON2bits_t	ETHCON2;
 volatile unsigned int        ETHCON2CLR ;
 volatile unsigned int        ETHCON2SET ;
 volatile unsigned int        ETHCON2INV ;

 __ETHTXSTbits_t		ETHTXST ;
 volatile unsigned int        ETHTXSTCLR ;
 volatile unsigned int        ETHTXSTSET ;
 volatile unsigned int        ETHTXSTINV ;
 
 __ETHRXSTbits_t		ETHRXST ;
 volatile unsigned int        ETHRXSTCLR ;
 volatile unsigned int        ETHRXSTSET ;
 volatile unsigned int        ETHRXSTINV ;

 __ETHHT0bits_t        ETHHT0 ;
 volatile unsigned int        ETHHT0CLR ;
 volatile unsigned int        ETHHT0SET ;
 volatile unsigned int        ETHHT0INV ;

 __ETHHT1bits_t        ETHHT1 ;
 volatile unsigned int        ETHHT1CLR ;
 volatile unsigned int        ETHHT1SET ;
 volatile unsigned int        ETHHT1INV ;

 __ETHPMM0bits_t 	  ETHPMM0 ;
 volatile unsigned int        ETHPMM0CLR ;
 volatile unsigned int        ETHPMM0SET ;
 volatile unsigned int        ETHPMM0INV ;

 __ETHPMM1bits_t 	  ETHPMM1 ;
 volatile unsigned int        ETHPMM1CLR ;
 volatile unsigned int        ETHPMM1SET ;
 volatile unsigned int        ETHPMM1INV ;

 __ETHPMCSbits_t      ETHPMCS ;
 volatile unsigned int        ETHPMCSCLR ;
 volatile unsigned int        ETHPMCSSET ;
 volatile unsigned int        ETHPMCSINV ;

 __ETHPMObits_t        ETHPMO ;
 volatile unsigned int        ETHPMOCLR ;
 volatile unsigned int        ETHPMOSET ;
 volatile unsigned int        ETHPMOINV ;
 
 __ETHRXFCbits_t	   ETHRXFC ;
 volatile unsigned int        ETHRXFCCLR ;
 volatile unsigned int        ETHRXFCSET ;
 volatile unsigned int        ETHRXFCINV ;

 __ETHRXWMbits_t         ETHRXWM ;
 volatile unsigned int        ETHRXWMCLR ;
 volatile unsigned int        ETHRXWMSET ;
 volatile unsigned int        ETHRXWMINV ;

 __ETHIENbits_t         ETHIEN ;
 volatile unsigned int        ETHIENCLR ;
 volatile unsigned int        ETHIENSET ;
 volatile unsigned int        ETHIENINV ;

 __ETHIRQbits_t         ETHIRQ ;
 volatile unsigned int        ETHIRQCLR ;
 volatile unsigned int        ETHIRQSET ;
 volatile unsigned int        ETHIRQINV ;

 __ETHSTATbits_t         ETHSTAT ;

 volatile unsigned int   	  addroffset[7];
 
 __ETHRXOVFLOWbits_t		ETHRXOVFLOW ;
 volatile unsigned int        ETHRXOVFLOWCLR ;
 volatile unsigned int        ETHRXOVFLOWSET ;
 volatile unsigned int        ETHRXOVFLOWINV ;

 __ETHFRMTXOKbits_t		ETHFRMTXOK ;
 volatile unsigned int        ETHFRMTXOKCLR ;
 volatile unsigned int        ETHFRMTXOKSET ;
 volatile unsigned int        ETHFRMTXOKINV ;

 __ETHSCOLFRMbits_t		ETHSCOLFRM ;
 volatile unsigned int        ETHSCOLFRMCLR ;
 volatile unsigned int        ETHSCOLFRMSET ;
 volatile unsigned int        ETHSCOLFRMINV ;

 __ETHMCOLFRMbits_t		ETHMCOLFRM ;
 volatile unsigned int        ETHMCOLFRMCLR ;
 volatile unsigned int        ETHMCOLFRMSET ;
 volatile unsigned int        ETHMCOLFRMINV ;

 __ETHFRMRXOKbits_t      ETHFRMRXOK ;
 volatile unsigned int        ETHFRMRXOKCLR ;
 volatile unsigned int        ETHFRMRXOKSET ;
 volatile unsigned int        ETHFRMRXOKINV ;

 __ETHFCSERRbits_t       ETHFCSERR ;
 volatile unsigned int        ETHFCSERRCLR ;
 volatile unsigned int        ETHFCSERRSET ;
 volatile unsigned int        ETHFCSERRINV ;

 __ETHALGNERRbits_t      ETHALGNERR ;
  volatile unsigned int        ETHALGNERRCLR ;
 volatile unsigned int        ETHALGNERRSET ;
 volatile unsigned int        ETHALGNERRINV ;

 volatile unsigned int   	  addroffset1[36];
 
 __EMACxCFG1bits_t       EMACxCFG1 ;
 volatile unsigned int        EMACxCFG1CLR ;
 volatile unsigned int        EMACxCFG1SET ;
 volatile unsigned int        EMACxCFG1INV ;

 __EMACxCFG2bits_t       EMACxCFG2 ;
 volatile unsigned int        EMACxCFG2CLR ;
 volatile unsigned int        EMACxCFG2SET ;
 volatile unsigned int        EMACxCFG2INV ;

 __EMACxIPGTbits_t       EMACxIPGT ;
 volatile unsigned int        EMACxIPGTCLR ;
 volatile unsigned int        EMACxIPGTSET ;
 volatile unsigned int        EMACxIPGTINV ;

 __EMACxIPGRbits_t       EMACxIPGR ;
 volatile unsigned int        EMACxIPGRCLR ;
 volatile unsigned int        EMACxIPGRSET ;
 volatile unsigned int        EMACxIPGRINV ;

 __EMACxCLRTbits_t       EMACxCLRT ;
 volatile unsigned int        EMACxCLRTCLR ;
 volatile unsigned int        EMACxCLRTSET ;
 volatile unsigned int        EMACxCLRTINV ;

 __EMACxMAXFbits_t       EMACxMAXF ;
 volatile unsigned int        EMACxMAXFCLR ;
 volatile unsigned int        EMACxMAXFSET ;
 volatile unsigned int        EMACxMAXFINV ;

 __EMACxSUPPbits_t       EMACxSUPP ;
 volatile unsigned int        EMACxSUPPCLR ;
 volatile unsigned int        EMACxSUPPSET ;
 volatile unsigned int        EMACxSUPPINV ;

 __EMACxTESTbits_t       EMACxTEST ;
 volatile unsigned int        EMACxTESTCLR ;
 volatile unsigned int        EMACxTESTSET ;
 volatile unsigned int        EMACxTESTINV ;

 __EMACxMCFGbits_t       EMACxMCFG ;
 volatile unsigned int        EMACxMCFGCLR ;
 volatile unsigned int        EMACxMCFGSET ;
 volatile unsigned int        EMACxMCFGINV ;

 __EMACxMCMDbits_t       EMACxMCMD ;
 volatile unsigned int        EMACxMCMDCLR ;
 volatile unsigned int        EMACxMCMDSET ;
 volatile unsigned int        EMACxMCMDINV ;

 __EMACxMADRbits_t       EMACxMADR ;
 volatile unsigned int        EMACxMADRCLR ;
 volatile unsigned int        EMACxMADRSET ;
 volatile unsigned int        EMACxMADRINV ;


 __EMACxMWTDbits_t  	EMACxMWTD ;
 volatile unsigned int        EMACxMWTDCLR ;
 volatile unsigned int        EMACxMWTDSET ;
 volatile unsigned int        EMACxMWTDINV ;

 __EMACxMRDDbits_t       EMACxMRDD ;
 volatile unsigned int        EMACxMRDDCLR ;
 volatile unsigned int        EMACxMRDDSET ;
 volatile unsigned int        EMACxMRDDINV ;

 __EMACxMINDbits_t       EMACxMIND ;
 volatile unsigned int        EMACxMINDCLR ;
 volatile unsigned int        EMACxMINDSET ;
 volatile unsigned int        EMACxMINDINV ;
 
 volatile unsigned int  addroffset2[8];	

 __EMACxSA0bits_t        EMACxSA0 ;
 volatile unsigned int        EMACxSA0CLR ;
 volatile unsigned int        EMACxSA0SET ;
 volatile unsigned int        EMACxSA0INV ;

 __EMACxSA1bits_t        EMACxSA1 ;
 volatile unsigned int        EMACxSA1CLR ;
 volatile unsigned int        EMACxSA1SET ;
 volatile unsigned int        EMACxSA1INV ;

 __EMACxSA2bits_t        EMACxSA2 ;
 volatile unsigned int        EMACxSA2CLR ;
 volatile unsigned int        EMACxSA2SET ;
 volatile unsigned int        EMACxSA2INV ;
 
} eth_registers_t;

#endif /*_ETH_STRUCTURE_H*/

/******************************************************************************
 End of File
*/


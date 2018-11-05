#ifndef __PERPHERAL_DMA_TEMPLATE_REGISTERS_H
#define __PERPHERAL_DMA_TEMPLATE_REGISTERS_H

/* DMA Core Registers */
typedef struct dma_regs {
    /* Control */
    __DMACONbits_t DMACON;
    uint32_t DMACONCLR;
    uint32_t DMACONSET;
    uint32_t DMACONINV;
    /* DMA status */
    __DMASTATbits_t DMASTAT;
    uint32_t DMASTATCLR;
    uint32_t DMASTATSET;
    uint32_t DMASTATINV;
    /* DMA address */
    uint32_t DMAADDR;
    uint32_t DONTUSE1[3];
    /* CRC Control */
    __DCRCCONbits_t DCRCCON;
    uint32_t DCRCCONCLR;
    uint32_t DCRCCONSET;
    uint32_t DCRCCONINV;
   /* CRC Data */
    uint32_t DCRCDATA;
    uint32_t DONTUSE2[3];
    /* CRC XOR */
    uint32_t DCRCXOR;
    uint32_t DONTUSE3[3];
} dma_register_t;

/* DMA Channel Register */
typedef struct dma_ch_regs {
    /* DMA Channel Control */
    __DCH0CONbits_t DCHxCON;
    uint32_t DCHxCONCLR;
    uint32_t DCHxCONSET;
    uint32_t DCHxCONINV;
    /* DMA Channel Event */
    __DCH0ECONbits_t DCHxECON;
    uint32_t DCHxECONCLR;
    uint32_t DCHxECONSET;
    uint32_t DCHxECONINV;
    /* DMA Channel Interrupt */
    __DCH0INTbits_t DCHxINT;
    uint32_t DCHxINTCLR;
    uint32_t DCHxINTSET;
    uint32_t DCHxINTINV;
    /* DMA Source Start Address */
    uint32_t DCHxSSA;
    uint32_t DONTUSE4[3];
    /* DMA Destination Start Address */
    uint32_t DCHxDSA;
    uint32_t DONTUSE5[3];
    /* DMA Source Size */
    uint32_t DCHxSSIZ;
    uint32_t DONTUSE6[3];
    /* DMA Destination Size */
    uint32_t DCHxDSIZ;
    uint32_t DONTUSE7[3];
    /* DMA Source Pointer */
    uint32_t DCHxSPTR;
    uint32_t DONTUSE8[3];
    /* DMA Destination Pointer */
    uint32_t DCHxDPTR;
    uint32_t DONTUSE9[3];
    /* DMA Cell Size */
    uint32_t DCHxCSIZ;
    uint32_t DONTUSE10[3];
    /* DMA Cell Pointer */
    uint32_t DCHxCPTR;
    uint32_t DONTUSE11[3];
    /* DMA Data Pattern */
    uint32_t DCHxDAT;
    uint32_t DONTUSE12[3];
} dma_ch_register_t;

#endif /* __PERPHERAL_DMA_TEMPLATE_REGISTERS_H */

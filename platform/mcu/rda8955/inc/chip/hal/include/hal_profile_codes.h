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

#ifndef COOL_PROFILE_HAL_PROFILE_CODES_H
#define COOL_PROFILE_HAL_PROFILE_CODES_H


#define CP_hal_SetResourceActivity 0x0032
#define CP_hal_Test 0x0033
#define CP_hal_HostUsbDecode 0x0039
#define CP_hal_HostUsbGenerateReadPacket 0x003A
#define CP_hal_HostUsbSendReadPacket 0x003B
#define CP_hal_HostUsbWriteInternal 0x003C
#define CP_hal_HostUsbInit 0x003D
#define CP_hal_RfspiFillFifo 0x0100
#define CP_hal_HstTraceSendData 0x0101
#define CP_hal_HstTraceGetData 0x0102
#define CP_hal_HstTraceTxDmaDone 0x0103
#define CP_hal_TimSetExpirationTime 0x0104
#define CP_hal_TimSetReminderTime 0x0105
#define CP_hal_TimTickOpen 0x0106
#define CP_hal_SpeechPushRxPopTx 0x0107
#define CP_hal_SpeechPopRxPushTx 0x0108
#define CP_hal_SpeechReset 0x0109
#define CP_hal_UsbIrqHandler 0x010A
#define CP_hal_UsbIrqHandlerI 0x010B
#define CP_hal_UsbIrqHandlerO 0x010C
#define CP_hal_UsbIrqHandlerIcp 0x010D
#define CP_hal_UsbIrqHandlerIto 0x010E
#define CP_hal_UsbIrqHandlerOsp 0x010F
#define CP_hal_UsbIrqHandlerOcp 0x0110
#define CP_hal_TimGetReminderTime 0x0111
#define CP_hal_UsbIrqHandlerReset 0x0113
#define CP_hal_UsbIrqHandlerEnum 0x0114
#define CP_hal_UsbFlushTxFifo 0x0115
#define CP_hal_UsbFlushAllTxFifos 0x0116
#define CP_hal_UsbResetTransfert 0x0117
#define CP_hal_UsbCancelTransfert 0x0118
#define CP_hal_UsbContinueTransfert 0x0119
#define CP_hal_UsbDisableEp 0x0121
#define CP_hal_UsbEnableEp 0x0122
#define CP_hal_UsbStatusIn 0x0123
#define CP_hal_UsbStatusOut 0x0124
#define CP_hal_UsbEpStall 0x0125
#define CP_hal_UsbRecv 0x0126
#define CP_hal_UsbSend 0x0127
#define CP_hal_UsbClose 0x0128
#define CP_hal_UsbReset 0x0129
#define CP_hal_UsbFlushRxFifo 0x012A
#define CP_hal_UsbStartTransfert 0x012B
#define CP_hal_UsbOpen 0x012C
#define CP_hal_UsbNewTransfert 0x012E
#define CP_hal_HostApDecode 0x0160
#define CP_hal_HostApSendReadPacket 0x0161
#define CP_hal_SimTxByteRxByte 0x1150
#define CP_hal_SimTxByteRxByte_nRxBytebl4 0x1151
#define CP_hal_SimTxByteRxByte_nRxByteab4 0x1152
#define CP_hal_SimIrqHandler_argEnd 0x1153
#define CP_hal_SimIrqHandler_rxDone 0x1154
#define CP_hal_SimIrqHandler_wwtTimeout 0x1155
#define CP_hal_SimIrqHandler_resendOvfl 0x1156
#define CP_hal_SimIrqHandler_extraRx 0x1157
#define CP_hal_AnaGpadcGetRaw 0x1158
#define CP_hal_IfcTransferStart 0x1159
#define CP_hal_IfcTransferStart_busy 0x115A
#define CP_XCPU_ERROR 0x2F02
#define CP_hal_UsbEndInTransfert 0x2F51
#define CP_hal_UsbEndOutTransfert 0x2F52
#define CP_hal_UsbAhbError 0x2F53
#define CP_hal_UsbTimeOutIn 0x2F54
#define CP_hal_TimWatchDogKeepAlive 0x2F55
#define CP_hal_LpsInvalidateCalib 0x2F56
#define CP_hal_SysRequestFreq 0x2F57
#define CP_hal_Gdb 0x2F58
#define CP_HAL_USB_MONITOR_EVENT_TIMEOUT 0x2F59
#define CP_HAL_LCDC_START_BLOCK_PULSE 0x2F5A
#define CP_CALIB_RUNNING 0x3700
#define CP_CALIB_VALID 0x3701
#define CP_DEEP_SLEEP 0x3702
#define CP_LIGHT_SLEEP 0x3703
#define CP_XCV_WAKEUP 0x3704
#define CP_RES_RF 0x3710
#define CP_RES_UART0 0x3711
#define CP_RES_UART1 0x3712
#define CP_RES_UART2 0x3713
#define CP_RES_SPI 0x3714
#define CP_RES_AUDIO 0x3715
#define CP_RES_SIM 0x3716
#define CP_RES_DMA 0x3717
#define CP_RES_GPADC 0x3718
#define CP_RES_JDS 0x3719
#define CP_RES_CLKOUT 0x371A
#define CP_RES_KEY 0x371B
#define CP_RES_USER0 0x371C
#define CP_RES_USER1 0x371D
#define CP_RES_USER2 0x371E
#define CP_RES_USER3 0x371F
#define CP_ACTIVATE_SPI1_CS0 0x3720
#define CP_ACTIVATE_SPI1_CS1 0x3721
#define CP_ACTIVATE_SPI1_CS2 0x3722
#define CP_ACTIVATE_SPI1_CS3 0x3723
#define CP_ACTIVATE_SPI2_CS0 0x3724
#define CP_ACTIVATE_SPI2_CS1 0x3725
#define CP_ACTIVATE_SPI2_CS2 0x3726
#define CP_ACTIVATE_SPI2_CS3 0x3727
#define CP_hal_LcdcWriteDataBlockBenchCpu 0x3728
#define CP_hal_LcdcWriteDataBlockBenchDma 0x3729
#define CP_PLL_CLK_FAST_EN 0x372A
#define CP_SLOW_SEL_32K 0x372B
#define CP_XTAL_SEL_52M 0x372C
#define CP_SYS_CLK_IS_26M 0x3740
#define CP_SYS_CLK_IS_39M 0x3741
#define CP_SYS_CLK_IS_52M 0x3742
#define CP_SYS_CLK_IS_78M 0x3743
#define CP_SYS_CLK_IS_89M 0x3744
#define CP_SYS_CLK_IS_104M 0x3745
#define CP_SYS_CLK_IS_113M 0x3746
#define CP_SYS_CLK_IS_125M 0x3747
#define CP_SYS_CLK_IS_139M 0x3748
#define CP_SYS_CLK_IS_156M 0x3749
#define CP_SYS_CLK_IS_178M 0x374A
#define CP_SYS_CLK_IS_208M 0x374B
#define CP_SYS_CLK_IS_250M 0x374C
#define CP_SYS_CLK_IS_312M 0x374D
#define CP_SYS_CLK_IS_OTHERS 0x374E
#define CP_MEM_CLK_IS_26M 0x374F
#define CP_MEM_CLK_IS_39M 0x3750
#define CP_MEM_CLK_IS_52M 0x3751
#define CP_MEM_CLK_IS_78M 0x3752
#define CP_MEM_CLK_IS_89M 0x3753
#define CP_MEM_CLK_IS_104M 0x3754
#define CP_MEM_CLK_IS_113M 0x3755
#define CP_MEM_CLK_IS_125M 0x3756
#define CP_MEM_CLK_IS_139M 0x3757
#define CP_MEM_CLK_IS_156M 0x3758
#define CP_MEM_CLK_IS_178M 0x3759
#define CP_MEM_CLK_IS_208M 0x375A
#define CP_MEM_CLK_IS_250M 0x375B
#define CP_MEM_CLK_IS_312M 0x375C
#define CP_MEM_CLK_IS_OTHERS 0x375D
#define CP_VOC_CLK_IS_26M 0x375E
#define CP_VOC_CLK_IS_39M 0x375F
#define CP_VOC_CLK_IS_52M 0x3760
#define CP_VOC_CLK_IS_78M 0x3761
#define CP_VOC_CLK_IS_89M 0x3762
#define CP_VOC_CLK_IS_104M 0x3763
#define CP_VOC_CLK_IS_113M 0x3764
#define CP_VOC_CLK_IS_125M 0x3765
#define CP_VOC_CLK_IS_139M 0x3766
#define CP_VOC_CLK_IS_156M 0x3767
#define CP_VOC_CLK_IS_178M 0x3768
#define CP_VOC_CLK_IS_208M 0x3769
#define CP_VOC_CLK_IS_250M 0x376A
#define CP_VOC_CLK_IS_312M 0x376B
#define CP_VOC_CLK_IS_OTHERS 0x376C
#define CP_BB_CLK_IS_26M 0x376D
#define CP_BB_CLK_IS_39M 0x376E
#define CP_BB_CLK_IS_52M 0x376F
#define CP_BB_CLK_IS_78M 0x3770
#define CP_BB_CLK_IS_89M 0x3771
#define CP_BB_CLK_IS_104M 0x3772
#define CP_BB_CLK_IS_113M 0x3773
#define CP_BB_CLK_IS_125M 0x3774
#define CP_BB_CLK_IS_139M 0x3775
#define CP_BB_CLK_IS_156M 0x3776
#define CP_BB_CLK_IS_178M 0x3777
#define CP_BB_CLK_IS_208M 0x3778
#define CP_BB_CLK_IS_250M 0x3779
#define CP_BB_CLK_IS_312M 0x377A
#define CP_BB_CLK_IS_OTHERS 0x377B
#define CP_SYS_SEL_FAST 0x377C
#define CP_MEM_SEL_FAST 0x377D
#define CP_VOC_SEL_FAST 0x377E
#define CP_BB_SEL_FAST 0x377F



#endif

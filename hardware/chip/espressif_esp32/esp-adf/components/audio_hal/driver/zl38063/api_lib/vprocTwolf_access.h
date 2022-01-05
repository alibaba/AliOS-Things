/****************************************************************************
* Microsemi Semiconductor, Kanata, ON
****************************************************************************
*
* Description: Voice Processor devices high level access module function
*                definitions
*
* NOTE: The registers of the device are 16-bit wide. A 32-bit access
*       is not required. However, the 32-bit access functions are provided
*       only if the host wants to access two consecutives 16-bit registers
*       in one single access.
*  Author: Jean Bony
****************************************************************************
* Copyright Microsemi Semiconductor Ltd., 2013. All rights reserved. This
* copyrighted work constitutes an unpublished work created in 2013. The use
* of the copyright notice is intended to provide notice that Microsemi
* Semiconductor Ltd. owns a copyright in this unpublished work; the main
* copyright notice is not an admission that publication has occurred. This
* work contains confidential, proprietary information and trade secrets of
* Microsemi Semiconductor Ltd.; it may not be used, reproduced or transmitted,
* in whole or in part, in any form or by any means without the prior
* written permission of Microsemi Semiconductor Ltd. This work is provided on
* a right to use basis subject to additional restrictions set out in the
* applicable license or other agreement.
*
***************************************************************************/

#ifndef VPROC_TWOLFACCESS_H
#define VPROC_TWOLFACCESS_H

#include "vproc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TWOLF_MAILBOX_SPINWAIT  1000  /*at least a 1000 to avoid mailbox busy */


/*device HBI command structure*/
typedef struct hbiCmdInfo {
    unsigned char page;
    unsigned char offset;
    unsigned char numwords;
} hbiCmdInfo;

/* external function prototypes */

VprocStatusType VprocTwolfHbiInit(void); /*Use this function to initialize the HBI bus*/

VprocStatusType VprocTwolfHbiRead(
    unsigned short cmd,       /*the 16-bit register to read from*/
    unsigned char numwords,   /* The number of 16-bit words to read*/
    unsigned short *pData);   /* Pointer to the read data buffer*/

VprocStatusType VprocTwolfHbiWrite(
    unsigned short cmd,     /*the 16-bit register to write to*/
    unsigned char numwords, /* The number of 16-bit words to write*/
    unsigned short *pData); /*the words (0-255) to write*/

VprocStatusType TwolfHbiNoOp( /*send no-op command to the device*/
    unsigned char numWords);  /* The number of no-op (0-255) to write*/

/*An alternative method to loading the firmware into the device
* USe this method if you have used the provided tool to convert the *.s3 into
* c code that can be compiled with the application
*/
VprocStatusType VprocTwolfHbiBoot_alt( /*use this function to boot load the firmware (*.c) from the host to the device RAM*/
    twFirmware *st_firmware); /*Pointer to the firmware image in host RAM*/


VprocStatusType VprocTwolfLoadConfig(dataArr *pCr2Buf, unsigned short numElements);

VprocStatusType VprocTwolfHbiCleanup(void);
VprocStatusType VprocTwolfHbiBootPrepare(void);
VprocStatusType VprocTwolfHbiBootMoreData(char *dataBlock);
VprocStatusType VprocTwolfHbiBootConclude(void);
VprocStatusType VprocTwolfFirmwareStop(void);   /*Use this function to halt the currently running firmware*/
VprocStatusType VprocTwolfFirmwareStart(void);  /*Use this function to start/restart the firmware currently in RAM*/
VprocStatusType VprocTwolfSaveImgToFlash(void); /*Save current loaded firmware from device RAM to FLASH*/
VprocStatusType VprocTwolfSaveCfgToFlash(void); /*Save current device config from device RAM to FLASH*/
VprocStatusType VprocTwolfReset(VprocResetMode mode);
VprocStatusType VprocTwolfEraseFlash(void);
VprocStatusType VprocTwolfLoadFwrCfgFromFlash(uint16 image_number);
VprocStatusType VprocTwolfSetVolume(uint8 vol);
VprocStatusType VprocTwolfGetVolume(int8_t *vol);
VprocStatusType VprocTwolfGetAppStatus(uint16 *status);

#ifdef __cplusplus
}
#endif
#endif /* VPROCTWOLFACCESS_H */

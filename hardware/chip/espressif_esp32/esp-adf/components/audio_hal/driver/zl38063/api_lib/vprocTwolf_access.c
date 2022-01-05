/*
 * User Space API wrapper for the "/dev/microsemi_spis_tw" linux kernel driver
 * if USING_MICROSEMI_LINUX_KERNEL_DRIVER is defined
 * or else this code is Operating System independent.
 * A host can use these functions to access the the microsemi Z
 * L38040/050/051/060/080 Voice Processing devices over a spi interface.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * Microsemi Inc. 2014, Jean Bony
 */

#include "vprocTwolf_access.h"

/*------------------------------------------------------*/
/*TWOLF MACROS-------------------------------*/
#define HBI_PAGED_READ(offset,length) \
    ((uint16)(((uint16)(offset) << 8) | (length)))
#define HBI_DIRECT_READ(offset,length) \
    ((uint16)(0x8000 | ((uint16)(offset) << 8) | (length)))
#define HBI_PAGED_WRITE(offset,length) \
    ((uint16)(HBI_PAGED_READ(offset,length) | 0x0080))
#define HBI_DIRECT_WRITE(offset,length) \
    ((uint16)(HBI_DIRECT_READ(offset,length) | 0x0080))
#define HBI_GLOBAL_DIRECT_WRITE(offset,length) \
    ((uint16)(0xFC00 | ((offset) << 4) | (length)))
#define HBI_CONFIGURE(pinConfig) \
    ((uint16)(0xFD00 | (pinConfig)))
#define HBI_SELECT_PAGE(page) \
    ((uint16)(0xFE00 | (page)))
#define HBI_SELECT_CL_PAGE() \
    ((uint16)(0xFE80)
#define HBI_CL_BROADCAST \
    ((uint16)0xFEFF)
#define HBI_NO_OP \
    ((uint16)0xFFFF)

#ifndef USING_MICROSEMI_LINUX_KERNEL_DRIVER
/*TWOLF REGisters*/
#define HOST_CMD_REG             0x0032  /*Host Command register*/
#define HOST_CMD_IDLE            0x0000  /*idle/ operation complete*/
#define HOST_CMD_NO_OP           0x0001  /*no-op*/
#define HOST_CMD_IMG_CFG_LOAD    0x0002  /*load firmware and CR from flash*/
#define HOST_CMD_IMG_LOAD        0x0003  /*load firmware only from flash*/
#define HOST_CMD_IMG_CFG_SAVE    0x0004  /*save a firmware and CR to flash*/
#define HOST_CMD_IMG_CFG_ERASE   0x0005  /*erase a firmware and CR in flash*/
#define HOST_CMD_CFG_LOAD        0x0006  /*Load CR from flash*/
#define HOST_CMD_CFG_SAVE        0x0007  /*save CR to flash*/
#define HOST_CMD_FWR_GO          0x0008  /*start/restart firmware (GO)*/
#define HOST_CMD_HOST_LOAD_CMP   0x000D  /*Host Application Load Complete*/
#define HOST_CMD_HOST_FLASH_INIT 0x000B  /*Host Application flash discovery*/
#define HOST_CMD_FWR_STOP        0x8000  /*stop firmware */
#define HOST_CMD_CMD_IN_PROGRESS 0xFFFF  /*wait command is in progress */


#define PAGE_255_CHKSUM_LO_REG  0x000A
#define PAGE_255_CHKSUM_HI_REG  0x0008
#define CLK_STATUS_REG          0x014   /*Clock status register*/
#define PAGE_255_BASE_LO_REG  0x000E
#define PAGE_255_BASE_HI_REG  0x000C
#define HOST_SW_FLAGS_REG     0x0006
#define HOST_SW_FLAGS_CMD     0x0001
#define HOST_SW_FLAGS_CMD_NORST     0x0004


#define TWOLF_CLK_STATUS_HBI_BOOT       0x0001

#define HBI_CONFIG_REG          0xFD00
#define HBI_CONFIG_ENDIANNESS   0x0000
#define HBI_CONFIG_DIVEMODE     0x0000
#define HBI_CONFIG_WAKE         0x0080
#define HBI_CONFIG_VAL (HBI_CONFIG_ENDIANNESS | \
                        HBI_CONFIG_DIVEMODE | HBI_CONFIG_WAKE)

#define HOST_CMD_PARAM_RESULT_REG   0x034 /*Host Command Param/Result register*/
#endif /*USING_MICROSEMI_LINUX_KERNEL_DRIVER*/

#define TOTAL_FWR_DATA_WORD_PER_LINE 24
#define TOTAL_FWR_DATA_BYTE_PER_LINE 128
#define TWOLF_STATUS_NEED_MORE_DATA 22
#define TWOLF_STATUS_BOOT_COMPLETE 23

#define TWOLF_MBCMDREG_SPINWAIT  10000
#define TWOLF_MAILBOX_SPINWAIT  1000

/*--------------------------------------------------------------------*/
/* VprocTwolfMailboxAcquire(): use this function to
*   check for the availability of the mailbox
*
* Input Argument: None
* Return: (VprocStatusType) type error code (0 = success, else= fail)
*/
static VprocStatusType VprocTwolfMailboxAcquire(uint16 flag, uint16 timeout)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    /*Check whether the host owns the command register*/
    uint16 i = 0, temp = 0x0BAD;
    for (i = 0; i < timeout; i++) {
        status = VprocTwolfHbiRead(HOST_SW_FLAGS_REG, 1, &temp);
        if ((status != VPROC_STATUS_SUCCESS)) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
        if (!(temp & flag)) {break;}
        Vproc_msDelay(10); /*wait for reset to complete*/
    }
    DEBUG_LOGD(TAG_SPI, "timeout count = %d: \n", i);
    if ((i >= timeout) && (temp & flag)) {
        return VPROC_STATUS_MAILBOX_BUSY;
    }
    /*read the Host Command register*/
    return VPROC_STATUS_SUCCESS;
}


/* VprocTwolfcmdRegAcquire(): use this function to
 *   check whether the last command completed sucsesfully
 *
 * Input Argument: None
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 */
static VprocStatusType VprocTwolfcmdRegAcquire(uint16 flag, uint16 timeout)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    /*Check whether the host owns the command register*/
    uint16 i = 0, temp = 0x0BAD;
    for (i = 0; i < timeout; i++) {
        status = VprocTwolfHbiRead(HOST_CMD_REG, 1, &temp);
        if ((status != VPROC_STATUS_SUCCESS)) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
        if (temp == flag) {break;}
        Vproc_msDelay(10); /*wait*/
    }
    DEBUG_LOGD(TAG_SPI, "timeout count = %d, cmdReg = 0x%04x: \n", i, temp);
    if ((i >= timeout) && (temp != flag)) {
        return VPROC_STATUS_CMDREG_BUSY;
    }
    /*read the Host Command register*/
    return VPROC_STATUS_SUCCESS;
}

/* VprocTwolfcmdRegWr(): use this function to
 *   access the host command register
 *
 * Input Argument: cmd - the command to send
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 */
static VprocStatusType VprocTwolfcmdRegWr(unsigned short cmd)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short flag = 0x0BAD;
    /*Check whether the host owns the command register*/

    status = VprocTwolfMailboxAcquire(HOST_SW_FLAGS_CMD, TWOLF_MAILBOX_SPINWAIT);
    if ((status != VPROC_STATUS_SUCCESS)) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    /*write the command into the Host Command register*/
    status = VprocTwolfHbiWrite(HOST_CMD_REG, 1, &cmd);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    /*Release the command reg*/
    /*read the Host Command register*/
    flag = HOST_SW_FLAGS_CMD;
    status = VprocTwolfHbiWrite(HOST_SW_FLAGS_REG, 1, &flag);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    /*Wait for the command to complete*/
    status = VprocTwolfcmdRegAcquire(HOST_CMD_IDLE, TWOLF_MAILBOX_SPINWAIT);
    if ((status != VPROC_STATUS_SUCCESS)) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: CMD_REG - Operation is not complete\n", status);
        return status;
    }
    return VPROC_STATUS_SUCCESS;
}

static VprocStatusType VprocTwolfCheckCmdResult(void)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf;
    status = VprocTwolfHbiRead(HOST_CMD_PARAM_RESULT_REG, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    if (buf != 0) {
        DEBUG_LOGE(TAG_SPI, "Command failed...Resultcode = 0x%04x\n", buf);
        return VPROC_STATUS_ERR_VERIFY;
    }
    return VPROC_STATUS_SUCCESS;
}

/* spis_tw_hbi_rd16()- Decode the 16-bit T-WOLF Regs Host address into
 * page, offset and build the 16-bit command acordingly to the access type.
 * then read the 16-bit word data and store it in pdata
 *  \param[in]
 *  .addr     the 16-bit HBI address
 *
 *  return ::status
 */
static int spis_tw_hbi_rd16(uint16* pdata)
{
    /*perform the HBI access*/
    if (VprocHALRead(pdata) != 0) {
        return VPROC_STATUS_ERR_HBI;
    }
    return 0;
}

/* spis_tw_hbi_wr16_cmd()- this function is used for single word access
 * It decodes the 16-bit T-WOLF Regs Host address into
 * page, offset and build the 16-bit command acordingly to the access type.
 * then write the command to the device
 *  \param[in]
 *                .addr      the 16-bit HBI address
 *
 *  return ::status
 */
static int spis_tw_hbi_wr16_cmd(uint16 addr, uint8 numwords)
{
    uint16 cmd;
    int status = 0;
    uint8 page;
    uint8 offset;

    page = addr >> 8;
    offset = (addr & 0xFF) / 2;

    if (page == 0) { /*Direct page access*/
        cmd = HBI_DIRECT_WRITE(offset, numwords - 1); /*build the cmd*/
    } else {
        /*indirect page access*/
        if (page != 0xFF) {
            page  -=  1;
        }
        cmd = HBI_SELECT_PAGE(page);
        /*select the page*/
        if (VprocHALWrite(cmd) != 0) {
            return status;
        }
        cmd = HBI_PAGED_WRITE(offset, numwords - 1); /*build the cmd*/
    }
    /*perform the HBI access*/
    if (VprocHALWrite(cmd) != 0) { /*write the register address*/
        return status;
    }
    return 0;
}

/* spis_tw_hbi_rd16()- Decode the 16-bit T-WOLF Regs Host address into
 * page, offset and build the 16-bit command acordingly to the access type.
 * then read the 16-bit word data and store it in pdata
 *  \param[in]
 *                .addr     the 16-bit HBI address
 *
 *  return ::status
 */
static int spis_tw_hbi_rd16_cmd(uint16 addr, uint8 numwords)
{
    uint16 cmd;
    int status = 0;
    uint8 page;
    uint8 offset;

    page = addr >> 8;
    offset = (addr & 0xFF) / 2;

    if (page == 0) {
        /*Direct page access*/
        cmd = HBI_DIRECT_READ(offset, numwords - 1); /*build the cmd*/
    } else {
        /*Indirect page access*/
        if (page != 0xFF) {
            page  -=  1;
        }
        cmd = HBI_SELECT_PAGE(page);
        /*select the page*/
        if (VprocHALWrite(cmd) != 0) {
            return status;
        }
        cmd = HBI_PAGED_READ(offset, numwords - 1); /*build the cmd*/
    }

    /*perform the HBI access*/
    if (VprocHALWrite(cmd) != 0) { /*write the register address*/
        return status;
    }
    return 0;
}

/* spis_tw_hbi_wr16()- this function is used for single word access by the
 * ioctl read. It decodes the 16-bit T-WOLF Regs Host address into
 * page, offset and build the 16-bit command acordingly to the access type.
 * then write the command and data to the device
 *  \param[in]
 *                .addr      the 16-bit HBI address
 *
 *  return ::status
 */
static int spis_tw_hbi_wr16_data(uint16 data)
{
    if (VprocHALWrite(data) != 0) {
        return VPROC_STATUS_ERR_HBI;
    }
    return 0;
}

/******************************************************************************
 * TwolfPagedWrite()
 * This function selects the specified page, writes the number of specified
 * words, starting at the specified offset from a source buffer.
 *
 * \param[in] page Page to select
 * \param[in] offset Offset of the requested Page to read from
 * \param[in] numWords Number of words to read starting from the offset
 * \param[in] pSrc Pointer to the date to write
 *
 * \retval ::VP_STATUS_SUCCESS
 * \retval ::VP_STATUS_ERR_HBI
 ******************************************************************************/
static VprocStatusType
TwolfHbiPage255Write(
    unsigned char page,
    unsigned char offset,
    unsigned char numWords,
    unsigned short* pDdata)
{
    uint16 cmdWrd = (uint16)(page << 8) | (uint16)offset;
    if (VprocTwolfHbiWrite(cmdWrd, numWords, pDdata) != VPROC_STATUS_SUCCESS) {
        return VPROC_STATUS_ERR_HBI;
    }
    return VPROC_STATUS_SUCCESS;
} /* TwolfHbiPagedWrite() */

/*------------------------------------------------------
 * Higher level functions - Can be called by a host application
 *------------------------------------------------------*/
/*VprocTwolfHbiInit - use this function to initialize the device HBI
 *   This function can be called at startup during the system init
 *   Configure the HBI_CONFIG_VAL as per the host system. But default
 *   config is good for most cases. See HBI section in device datasheet for
 *   details
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */

VprocStatusType VprocTwolfHbiInit(void)
{
    unsigned short buf = HBI_CONFIG_REG | HBI_CONFIG_VAL;
    if (VprocHALInit() != 0) {
        return VPROC_STATUS_INIT_FAILED;
    }
    return VprocHALWrite(buf);
}

/*VprocTwolfHbiCleanup - To close any open communication path to
 * to the device
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfHbiCleanup(void)
{
    VprocHALcleanup();
    return VPROC_STATUS_SUCCESS;
}
/*VprocTwolfHbiRead - use this function to read up to 254 words from the device
 * \param[in] cmd of the requested device register to read from
 * \param[in] numWords Number of words to read starting from the offset
 * \param[in] pData Pointer to the data read
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfHbiRead(
    unsigned short cmd,   /*register to read from*/
    unsigned char numwords,
    unsigned short* pData)
{
    VprocStatusType  status = VPROC_STATUS_SUCCESS;
    unsigned short tempBuf = 0x0BAD;
    unsigned char i = 0;

    // DEBUG_LOGE(TAG_SPI, "cmd = 0x%04x\n", cmd);
    status = spis_tw_hbi_rd16_cmd(cmd, numwords);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR: VPROC_STATUS_RD_FAILED,CMD:0x%04x\n", cmd);
        return VPROC_STATUS_WR_FAILED;
    }

    for (i = 0; i < numwords; i++) {
        status = spis_tw_hbi_rd16(&tempBuf);
        pData[i] = tempBuf;
        // DEBUG_LOGE(TAG_SPI, "pData[%d] = 0x%04x\n", i, pData[i]);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR: VPROC_STATUS_RD_FAILED,CMD:0x%04x\n", cmd);
            return VPROC_STATUS_RD_FAILED;
        }
    }
    return status;
}

/*VprocTwolfHbiWrite - use this function to write up to 252 words to the device
 * \param[in] cmd of the requested device register to write to
 * \param[in] numWords Number of words to write starting from the offset
 * \param[in] pData Pointer to the data to write
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfHbiWrite(
    unsigned short cmd,   /*register to read from*/
    unsigned char numwords,
    unsigned short* pData)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned char i = 0;

    if ((numwords == 0) || (numwords > 126)) {
        DEBUG_LOGE(TAG_SPI, "number of words is out of range. Maximum is 126\n");
        return VPROC_STATUS_INVALID_ARG;
    }
    /*16-bit SPI access mode - Send only 1 word within the same CS*/
    status = spis_tw_hbi_wr16_cmd(cmd, numwords);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR: VPROC_STATUS_WR_FAILED\n");
        return VPROC_STATUS_WR_FAILED;
    }
    for (i = 0; i < numwords; i++) {
        status = spis_tw_hbi_wr16_data(pData[i]);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR: VPROC_STATUS_WR_FAILED\n");
            return VPROC_STATUS_WR_FAILED;
        }
    }
    return VPROC_STATUS_SUCCESS;
}

/*VprocTwolfLoadConfig() - use this function to load a custom or new config
 * record into the device RAM to override the default config
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfLoadConfig(dataArr* pCr2Buf, unsigned short numElements)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short i, buf;
    /*stop the current firmware but do not reset the device and do not go to boot mode*/

    /*send the config to the device RAM*/
    for (i = 0; i < numElements; i++) {
        buf = pCr2Buf[i].value;
        status = VprocTwolfHbiWrite(pCr2Buf[i].reg, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            return VPROC_STATUS_ERR_HBI;
        }

    }

    return status;
}

/* HbiSrecBoot_alt() Use this alternate method to load the st_twFirmware.c
 *(converted *.s3 to c code) to the device
 */
static VprocStatusType HbiSrecBoot_alt(twFirmware* st_firmware)
{
    uint16 index = 0;
    uint16 gTargetAddr[2] = {0, 0};
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    while (index < st_firmware->twFirmwareStreamLen) {

        /* put the address into our global target addr */
        gTargetAddr[0] = (uint16)((st_firmware->st_Fwr[index].targetAddr &
                                   0xFFFF0000) >> 16);
        gTargetAddr[1] = (uint16)(st_firmware->st_Fwr[index].targetAddr &
                                  0x0000FFFF);

        //DEBUG_LOGE(TAG_SPI, "gTargetAddr[0] = 0x%04x, gTargetAddr[1] = 0x%04x: \n",
        //                                   gTargetAddr[0], gTargetAddr[1]);

        //DEBUG_LOGE(TAG_SPI, "numWords = %d: \n",
        //                             st_firmware->st_Fwr[index].numWords);

        /* write the data to the device */
        if (st_firmware->st_Fwr[index].numWords != 0) {
            if (st_firmware->st_Fwr[index].useTargetAddr) {
                status = VprocTwolfHbiWrite(PAGE_255_BASE_HI_REG, 2, gTargetAddr);
                if (status != VPROC_STATUS_SUCCESS) {
                    DEBUG_LOGE(TAG_SPI, "Unable to set gTargetAddr[0] = 0x%04x,"
                               " gTargetAddr[1] = 0x%04x: \n", gTargetAddr[0], gTargetAddr[1]);
                    return VPROC_STATUS_ERR_HBI;
                }
            }
            status = TwolfHbiPage255Write(0xFF, (uint8)((gTargetAddr[1] & 0x00FF)),
                                          st_firmware->st_Fwr[index].numWords, st_firmware->st_Fwr[index].buf);
            if (status != VPROC_STATUS_SUCCESS) {
                DEBUG_LOGE(TAG_SPI, "status = %d, numWords = %d: \n",
                           status, st_firmware->st_Fwr[index].numWords);
                return status;
            }

        }
        // DEBUG_LOGE(TAG_SPI, "index %d\n", index);
        if (index == 9) {
            // break;
        }
        index++;
    }

    /*
     * convert the number of bytes to two 16 bit
     * values and write them to the requested page register
     */
    /* even number of bytes required */

    /* program the program's execution start register */
    gTargetAddr[0] = (uint16)((st_firmware->execAddr & 0xFFFF0000) >> 16);
    gTargetAddr[1] = (uint16)(st_firmware->execAddr & 0x0000FFFF);
    status = VprocTwolfHbiWrite(0x12C, 2, gTargetAddr);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, " unable to program page 1 execution address\n");
        return status;
    }

    /* print out the srecord program info */
    DEBUG_LOGI(TAG_SPI, "prgmBase 0x%08x\n", st_firmware->prgmBase);
    DEBUG_LOGI(TAG_SPI, "execAddr 0x%08x\n", st_firmware->execAddr);
    DEBUG_LOGI(TAG_SPI, "DONE\n");
    return VPROC_STATUS_SUCCESS;
}

/*VprocTwolfHbiBoot_alt - use this function to bootload the firmware
 * into the device
 * \param[in] pointer to image data structure
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 * \retval ::VPROC_STATUS_MAILBOX_BUSY
*/
VprocStatusType VprocTwolfHbiBoot_alt(twFirmware* st_firmware)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf[2] = {0, 0};

    /* write a value of 1 to address 0x14 (direct page offset 0x0A).
     * to stop current firmware, reset the device into the Boot Rom mode.
     */
    buf[0]  = 1;//TWOLF_CLK_STATUS_HBI_BOOT;
    status = VprocTwolfHbiWrite(CLK_STATUS_REG, 1, buf);
    if (status != VPROC_STATUS_SUCCESS) {
        return VPROC_STATUS_ERR_HBI;
    }
    Vproc_msDelay(300); /*wait for reset to complete*/

    buf[0] = buf[1] = 0;
    status = VprocTwolfHbiRead(HOST_CMD_PARAM_RESULT_REG, 1, buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    /*Check if the device is accessible for command*/
    if ((buf[0] != 0xD3D3)) {
        DEBUG_LOGE(TAG_SPI, "ERROR: HBI is not accessible\n");
        return VPROC_STATUS_ERR_HBI;
    }
    /*Transfer the image*/
    status =  HbiSrecBoot_alt(st_firmware);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    /*tell Twolf that the firmware loading is complete*/
    buf[0] = HOST_CMD_HOST_LOAD_CMP;
    status =  VprocTwolfcmdRegWr(buf[0]);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    /*Verify whether the boot loading is successful*/
    if (VprocTwolfCheckCmdResult() != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR: Failed to load the Firmware...\n");
        return VPROC_STATUS_FW_LOAD_FAILED;
    }

    return VPROC_STATUS_SUCCESS;
}

/*The following 3 functions provide a mean to loading the *.s3 firmare into
 * the device
 * - Call sequence:
 * 1- Call VprocTwolfHbiBootPrepare()
 * 2- Call VprocTwolfHbiBootMoreData() in a loop by passing one line at a time
 *                from the *.S3 image file until the whole file is transferred
 *                to the device. or until a TWOLF_STATUS_BOOT_COMPLETE status
 *                is reported
 * 3- Call VprocTwolfHbiBootConclude() once the whole data is transferred suc-
 * cessfully
 */
VprocStatusType VprocTwolfHbiBootPrepare(void)
{
    unsigned short buf[2] = {0, 0};
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    /* write a value of 1 to address 0x14 (direct page offset 0x0A).
     * to stop current firmware, reset the device into the Boot Rom mode.
     */
    buf[0]  = 1;//TWOLF_CLK_STATUS_HBI_BOOT;
    status = VprocTwolfHbiWrite(CLK_STATUS_REG, 1, buf);
    if (status != VPROC_STATUS_SUCCESS) {
        return VPROC_STATUS_ERR_HBI;
    }
    Vproc_msDelay(300); /*wait for reset to complete*/


    buf[0] = buf[1] = 0;
    status = VprocTwolfHbiRead(HOST_CMD_PARAM_RESULT_REG, 1, buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    /*Check if the device is accessible for command*/
    if ((buf[0] != 0xD3D3)) {
        DEBUG_LOGE(TAG_SPI, "ERROR: HBI is not accessible\n");
        return VPROC_STATUS_ERR_HBI;
    }

    return status;
}
/*AsciiHexToHex() - to convert ascii char hex to integer hex
 * pram[in] - str - pointer to the char to convert.
 * pram[in] - len - the number of character to convert (2:u8, 4:u16, 8:u32).

 */
static unsigned int AsciiHexToHex(const char* str, unsigned char len)
{
    unsigned int val = 0;
    char c;
    unsigned char i = 0;
    for (i = 0; i < len; i++) {
        c = *str++;
        val <<= 4;

        if (c >= '0' && c <= '9') {
            val += c & 0x0F;
            continue;
        }

        c &= 0xDF;
        if (c >= 'A' && c <= 'F') {
            val += (c & 0x07) + 9;
            continue;
        }
        return 0;
    }
    return val;
}
static int spisTwBootWrite(char* blockOfFwrData) /*0: HBI; 1:FLASH*/
{
    /*Use this method to load the actual *.s3 file line by line*/
    int status = 0;
    int rec_type, i = 0, j = 0;
    uint8 numbytesPerLine = 0;
    uint8 numdataWordPerLine = 0;
    uint16 dataBuf[64];
    unsigned long address = 0;
    uint8 page255Offset = 0x00;
    uint16 cmd = 0;
    uint16 gTargetAddr[2] = {0, 0};
    DEBUG_LOGI(TAG_SPI, "blockOfFwrData = %s\n", blockOfFwrData);

    if (blockOfFwrData == NULL) {
        DEBUG_LOGE(TAG_SPI, "blockOfFwrData[0] = %c\n", blockOfFwrData[0]);
        return VPROC_STATUS_INVALID_ARG;
    }
    /* if this line is not an srecord skip it */
    if (blockOfFwrData[0] != 'S') {
        DEBUG_LOGE(TAG_SPI, "blockOfFwrData[0] = %c\n", blockOfFwrData[0]);
        return VPROC_STATUS_INVALID_ARG;
    }
    numbytesPerLine = AsciiHexToHex(&blockOfFwrData[2], 2);
    numdataWordPerLine = (numbytesPerLine - 5) / 2;
    DEBUG_LOGI(TAG_SPI, "numbytesPerLine = %d\n", numbytesPerLine);
    if (numbytesPerLine == 0) {
        DEBUG_LOGE(TAG_SPI, "blockOfFwrData[3] = %c\n", blockOfFwrData[3]);
        return VPROC_STATUS_INVALID_ARG;
    }
    /* get the srecord type */
    rec_type = blockOfFwrData[1] - '0';

    /* skip non-existent srecord types and block header */
    if ((rec_type == 4) || (rec_type == 5) || (rec_type == 6) || (rec_type == 0)) {
        return TWOLF_STATUS_NEED_MORE_DATA;
    }

    /* get the info based on srecord type (skip checksum) */
    address = AsciiHexToHex(&blockOfFwrData[4], 8);

    page255Offset = (uint8)(address & 0xFF);

    gTargetAddr[0] = (uint16)((address & 0xFFFF0000) >> 16);
    gTargetAddr[1] = (uint16)(address & 0x0000FFFF);

    /* store the execution address */
    if ((rec_type == 7) || (rec_type == 8) || (rec_type == 9)) {
        /* the address is the execution address for the program */
        DEBUG_LOGI(TAG_SPI, "execAddr = 0x%08lx\n", address);
        /* program the program's execution start register */
        //status = spis_tw_hbi_multi_wr8(0x012C, 4, buf);
        status = VprocTwolfHbiWrite(0x12C, 2, gTargetAddr);

        if (status != 0) {
            DEBUG_LOGE(TAG_SPI, " unable to program page 1 execution address\n");
            return status;
        }
        DEBUG_LOGI(TAG_SPI, "Loading firmware data complete...\n");
        return TWOLF_STATUS_BOOT_COMPLETE;  /*BOOT_COMPLETE Sucessfully*/
    }

    /* put the address into our global target addr */

    //DEBUG_LOGE(TAG_SPI, "gTargetAddr = 0x%08lx: \n", address);
    status = VprocTwolfHbiWrite(PAGE_255_BASE_HI_REG, 2, gTargetAddr);
    //status = spis_tw_hbi_multi_wr8(PAGE_255_BASE_HI_REG, 4, buf);
    if (status != 0) {
        DEBUG_LOGE(TAG_SPI, "gTargetAddr = 0x%08lx: \n", address);
        return VPROC_STATUS_FW_LOAD_FAILED;
    }

    /* get the data bytes */
    j = 12;
    DEBUG_LOGI(TAG_SPI, "dataBuf[]=\n");
    for (i = 0; i < numdataWordPerLine; i++) {
        dataBuf[i] = AsciiHexToHex(&blockOfFwrData[j], 4);
        j += 4;
        DEBUG_LOGI(TAG_SPI, "0x%04x, ", dataBuf[i]);
    }
    /* write the data to the device */
    cmd = (uint16)(0xFF << 8) | (uint16)page255Offset;
    status = VprocTwolfHbiWrite(cmd, numdataWordPerLine, dataBuf);
    if (status != 0) {
        return status;
    }

    DEBUG_LOGI(TAG_SPI, "Provide next block of data...\n");
    return TWOLF_STATUS_NEED_MORE_DATA; /*REQUEST STATUS_MORE_DATA*/
}

VprocStatusType VprocTwolfHbiBootMoreData(char* dataBlock)
{

    // return ioctl(twolf_fd, TWOLF_BOOT_SEND_MORE_DATA, dataBlock);
    return spisTwBootWrite(dataBlock);
}

VprocStatusType VprocTwolfHbiBootConclude(void)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf;

    /*tell Twolf that the firmware loading is complete*/
    buf = HOST_CMD_HOST_LOAD_CMP;
    status =  VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    /*Verify whether the boot loading is successful*/
    if (VprocTwolfCheckCmdResult() != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR: Failed to load the Firmware...\n");
        return VPROC_STATUS_FW_LOAD_FAILED;
    }
    return status;
    //return status;
}

/*USe this function to erase a slave flash device controlled by the Twolf*/
VprocStatusType VprocTwolfEraseFlash(void)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf;
    /*Save firmware to flash*/

    status = VprocTwolfReset(VPROC_RST_HARDWARE_RAM);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    buf = HOST_CMD_HOST_FLASH_INIT;
    /*if there is a flash on board initialize it*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    buf = 0xAA55;
    status = VprocTwolfHbiWrite(HOST_CMD_PARAM_RESULT_REG, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "Unable to set command reg = 0x%04x: \n", buf);
        return VPROC_STATUS_ERR_HBI;
    }

    buf = 0x0009;
    /*delete all applications on flash*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    return VPROC_STATUS_SUCCESS;
}


/* VprocTwolfReset(): use this function to reset the device.
 *
 *
 * Input Argument: mode  - the reset mode (VPROC_RST_HARDWARE_ROM,
 *         VPROC_RST_HARDWARE_ROM, VPROC_RST_SOFT, VPROC_RST_AEC)
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 */
VprocStatusType VprocTwolfReset(VprocResetMode mode)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf;
    /*PLATFORM SPECIFIC code*/
    if (mode  == VPROC_RST_HARDWARE_RAM) {       /*hard reset*/
        /*hard reset*/
        buf  = 0x05;
        status = VprocTwolfHbiWrite(0x014, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
    } else if (mode == VPROC_RST_HARDWARE_ROM) {  /*power on reset*/
        /*hard reset*/
        buf  = 0x09;
        status = VprocTwolfHbiWrite(0x014, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
    } else if (mode == VPROC_RST_AEC) { /*AEC method*/
        buf  = 0x01;
        status = VprocTwolfHbiWrite(0x0300, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
    } else if (mode == VPROC_RST_SOFTWARE) { /*soft reset*/
        buf  = 0x02;
        status = VprocTwolfHbiWrite(0x006, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }
    } else {
        DEBUG_LOGE(TAG_SPI, "Invalid reset type\n");
        return VPROC_STATUS_INVALID_ARG;
    }
    Vproc_msDelay(200);
    return VPROC_STATUS_SUCCESS;
}

VprocStatusType VprocTwolfSetVolume(uint8 vol)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf = vol; // Gain A
    buf += (unsigned short )vol << 8; // Gain B
    VprocHALInit();
    status = VprocTwolfHbiWrite(0x238, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    status = VprocTwolfHbiWrite(0x23A, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    return status;
}

VprocStatusType VprocTwolfGetVolume(int8_t* vol)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    unsigned short buf = 0;
    VprocHALInit();
    status = VprocTwolfHbiRead(0x238, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    *vol  = (int8_t)buf;
    return status;
}

VprocStatusType VprocTwolfGetAppStatus(uint16 *status)
{
    VprocStatusType ret = VPROC_STATUS_SUCCESS;
    unsigned short buf = 0;
    VprocHALInit();
    ret = VprocTwolfHbiRead(0x030, 1, &buf);
    if (ret != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", ret);
        return ret;
    }
    *status  = buf & 0x0F;
    return ret;
}
/* VprocTwolfSaveImgToFlash(): use this function to
 *     save both the config record and the firmware to flash. It Sets the bit
 *     which initiates a firmware save to flash when the device
 *     moves from a STOPPED state to a RUNNING state (by using the GO bit)
 *
 * Input Argument: None
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 */

/* VprocTwolfSaveImgToFlash(): use this function to
 *     save both the config record and the firmware to flash. It Sets the bit
 *     which initiates a firmware save to flash when the device
 *     moves from a STOPPED state to a RUNNING state (by using the GO bit)
 *
 * Input Argument: None
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 */
VprocStatusType VprocTwolfSaveImgToFlash(void)
{
    unsigned short buf;
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    /*Save firmware to flash*/

    buf = HOST_CMD_HOST_FLASH_INIT;
    /*if there is a flash on board initialize it*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    buf = 0xAA55;
    status = VprocTwolfHbiWrite(HOST_CMD_PARAM_RESULT_REG, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "Unable to set command reg = 0x%04x: \n", buf);
        return VPROC_STATUS_ERR_HBI;
    }

    buf = 0x0009;
    /*delete all applications on flash*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    buf = HOST_CMD_IMG_CFG_SAVE;
    /*save the   image to flash*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "Save firmware to flash failed... reg = 0x%04x: \n", buf);
        return VPROC_STATUS_ERR_HBI;
    }
    /*check whethe the actions above were performed successfully*/
    return VprocTwolfCheckCmdResult();
}


/* VprocTwolfSaveCfgToFlash(): use this function to
 *     save the config record to flash. It Sets the bit
 *     which initiates a config save to flash when the device
 *     moves from a STOPPED state to a RUNNING state (by using the GO bit)
 *
 * Input Argument: None
 * Return: (VprocStatusType) type error code (0 = success, else= fail)
 * The firmware must be stopped first with VprocTwolfFirmwareStop()
 */

VprocStatusType VprocTwolfSaveCfgToFlash(void)
{
    unsigned short buf;
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    /*Save firmware to flash*/
    /*if there is a flash on board initialize it*/

    /* Clear the checksum register so that the fimrware can calculate a new chk value
     * If this register is not cleared first, then an invalid checksum will be calculated
     * which will cause this procedure to fail
     */

    buf = HOST_CMD_HOST_FLASH_INIT;

    /*if there is a flash on the board initialize it*/
    status = VprocTwolfcmdRegWr(buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }

    /*Check if there is a flash device and an image already saved to it - load it to RAM*/
    status = VprocTwolfHbiRead(0x026, 1, &buf);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
        return status;
    }
    if ((buf > 0)) {

        /*load the corresponding image/cr from flash*/
        buf = 0x0001;
        status = VprocTwolfHbiWrite(HOST_CMD_PARAM_RESULT_REG, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Unable to set command reg = 0x%04x: \n", buf);
            return VPROC_STATUS_ERR_HBI;
        }
        buf = 0x8002;
        status = VprocTwolfHbiWrite(HOST_CMD_REG, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Unable to set command reg = 0x%04x: \n", buf);
            return VPROC_STATUS_ERR_HBI;
        }

        /*Release the command reg*/
        /*read the Host Command register*/
        buf = 0x0004;
        status = VprocTwolfHbiWrite(HOST_SW_FLAGS_REG, 1, &buf);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: \n", status);
            return status;
        }

        status = VprocTwolfcmdRegAcquire(HOST_CMD_IDLE, TWOLF_MAILBOX_SPINWAIT);
        if ((status != VPROC_STATUS_SUCCESS)) {
            DEBUG_LOGE(TAG_SPI, "ERROR %d: CMD_REG - Operation is not complete\n",
                       status);
            return status;
        }

        /*Verify wheter the operation completed sucessfully*/
        status = VprocTwolfCheckCmdResult();
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d: Unable to verify result-param: \n",
                       status);
            return status;
        }

    }

    return VPROC_STATUS_SUCCESS;
}

/*VprocTwolfFirmwareStart - use this function to start/restart the firmware
 * previously stopped with VprocTwolfFirmwareStop()
 * \param[in] none
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfFirmwareStart(void)
{

    unsigned short buf;

    /*Start firmware*/
    buf = HOST_CMD_FWR_GO;
    return VprocTwolfcmdRegWr(buf);

}

/*VprocTwolfFirmwareStop - use this function to stop the firmware currently running
 * And set the device in boot mode
 * \param[in] none
 *
 * \retval ::VPROC_STATUS_SUCCESS
 * \retval ::VPROC_STATUS_ERR_HBI
 */
VprocStatusType VprocTwolfFirmwareStop(void)
{

    unsigned short buf;

    /*Stop firmware*/
    buf = HOST_CMD_FWR_STOP;
    return VprocTwolfcmdRegWr(buf);

}



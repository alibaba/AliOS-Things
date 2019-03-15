/**
 *******************************************************************************
 * @file    upgrade_detect.c
 * @author  Robert
 * @version V1.0.0
 * @date    4-Dec-2013
 * @brief	Upgrade detection when boot up
 * @maintainer lujiangang
 * Changelog:
 *			2013-12-04	Create by Robert
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "nvm.h"
#include "fsinfo.h"
#include "fs_config.h"
#include "fs_mount.h"
#include "fs_sync.h"
#include "app_config.h"
#include "presearch.h"



//keep consist with miscfg.h definition
#define UPGRADE_NVM_ADDR        (176)//boot upgrade information at NVRAM address
#define UPGRADE_ERRNO_NOERR		(-1) //just initialization after boot up
#define UPGRADE_ERRNO_ENOENT	(-2) //no such file open by number
#define UPGRADE_ERRNO_EIO		(-5) //read/write error
#define UPGRADE_ERRNO_E2BIG		(-7) //too big than flash capacity
#define UPGRADE_ERRNO_EBADF		(-9) //no need to upgrade
#define UPGRADE_ERRNO_EFAULT	(-14) //address fault
#define UPGRADE_ERRNO_EBUSY		(-16) //flash lock fail
#define UPGRADE_ERRNO_ENODEV	(-19) //no upgrade device found
#define UPGRADE_ERRNO_ENODATA	(-61) //flash is empty
#define UPGRADE_ERRNO_EPROTOTYPE (-91) //bad head format("MVO\x12")
#define UPGRADE_ERRNO_ELIBBAD	(-80) //CRC error
#define UPGRADE_ERRNO_USBDEV	(-81) //no upgrade USB device
#define UPGRADE_ERRNO_SDDEV		(-82) //no upgrade SD device
#define UPGRADE_ERRNO_USBFILE	(-83) //no upgrade file found in USB device
#define UPGRADE_ERRNO_SDFILE	(-84) //no upgrade file found in SD device
#define UPGRADE_ERRNO_NOBALL	(-85) //no upgrade ball in USB & SD device
#define UPGRADE_ERRNO_CODEMGC	(-86) //wrong code magic number
#define UPGRADE_ERRNO_CODBUFDAT (-87) //code successful but data fail,because of constant data offset setting

#define UPGRADE_SUCC_MAGIC		(0x57F9B3C8) //just a successful magic
#define UPGRADE_REQT_MAGIC		(0x9ab4d18e) //just a request magic
#define UPGARDE_REQT_SPEC_DEV_TAG	(0x9ab4d1c0) //just for specific device upgarde request
#define UPGARDE_REQT_SPEC_DEV_MASK	(0x3F)
#define UPGRADE_ERRNO_LASTCLR	(0x581f9831) //just a clear magic


#ifdef FUNC_SPI_UPDATE_EN
//extern uint8_t UpgradeFileFound;
void BootUpgradeChk(void)
{
	uint32_t BootNvmInfo;

	NvmRead(UPGRADE_NVM_ADDR, (uint8_t*)&BootNvmInfo, 4);
	if(0xFF == UpgradeFileFound)
	{
		if(BootNvmInfo == UPGRADE_SUCC_MAGIC)
		{
			/*
			 * boot up check for the last time
			 */
			APP_DBG("[UPGRADE]:<boot> upgrade successful completely\n");
		}
		else if(BootNvmInfo == (uint32_t)UPGRADE_ERRNO_NOERR)
		{
			APP_DBG("[UPGRADE]:<boot> no upgrade, boot normallly\n");
		}
		else if(BootNvmInfo == (uint32_t)UPGRADE_ERRNO_CODBUFDAT)
		{
			APP_DBG("[UPGRADE]:<boot> upgrade successful partly, data fail\n");
		}
		else
		{
			APP_DBG("[UPGRADE]:<boot> upgrade error, errno = %d\n", (int32_t)BootNvmInfo);
		}
	}
	else
	{
		if(BootNvmInfo == (uint32_t)UPGRADE_ERRNO_NOERR)
		{
			FS_CONTEXT* CurFsContext = FsGetCurContext();
			FsLock(CurFsContext);
			APP_DBG("[UPGRADE]:found upgrade ball(*.MVA), prepare to boot upgrade\n");
			BootNvmInfo = UPGARDE_REQT_SPEC_DEV_TAG | 0x21;
			NvmWrite(UPGRADE_NVM_ADDR, (uint8_t*)&BootNvmInfo, 4);
            //if you want PORRESET to reset GPIO only,uncomment it
            //GpioPorSysReset(GPIO_RSTSRC_PORREST);
			NVIC_SystemReset();
			FsUnLock(CurFsContext);
			while(1);;;
		}
		else if(BootNvmInfo == UPGRADE_SUCC_MAGIC)
		{
			BootNvmInfo = (uint32_t)UPGRADE_ERRNO_NOERR;
			NvmWrite(UPGRADE_NVM_ADDR, (uint8_t*)&BootNvmInfo, 4);
			APP_DBG("[UPGRADE]:found upgrade ball(*.MVA) file for the last time, re-plugin/out, if you want to upgrade again\n");
		}
		else
		{
			APP_DBG("[UPGRADE]:upgrade error, errno = %d\n", (int32_t)BootNvmInfo);
			BootNvmInfo = (uint32_t)UPGRADE_ERRNO_NOERR;
			NvmWrite(UPGRADE_NVM_ADDR, (uint8_t*)&BootNvmInfo, 4);
		}
	}
	UpgradeFileFound = 0;
}
#endif


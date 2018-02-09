/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
#include "cmsis.h"
#include "sys_api.h"
#include "flash_api.h"
#include "osdep_api.h"
#include "device_lock.h"

#define OTA_Signature			"81958711"
#define OTA_Clear				"00000000"
#define OTA_Signature_len		8
#define OTA_Signature_offset	8
#define OTA_valid_offset		0x100000
#undef printf
#define printf					DiagPrintf

#if !defined(__ICCARM__)
#define memcmp(dst, src, sz)      _memcmp(dst, src, sz)
#define memset(dst, val, sz)      _memset(dst, val, sz)
#define memcpy(dst, src, sz)      _memcpy(dst, src, sz)
#endif  // #if !defined(__ICCARM__)

extern VOID HalJtagPinOff(VOID);

extern void HalInitLogUart(void);
extern void HalDeinitLogUart(void);

#if defined ( __ICCARM__ )
extern u8 IsSdrPowerOn();
#endif

/**
 *  @brief   Turn off the JTAG function
 *
 *  @return  None
 *         
 */
void sys_jtag_off(void)
{
	HalJtagPinOff();
}

void sys_clear_ota_signature(void)
{
    flash_t		flash;
    u32			ota_offset=0xFFFFFFFF, part1_offset, part2_offset;
    u8			signature[OTA_Signature_len+1];

	device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_stream_read(&flash, 0x18, 4, (u8*)&part1_offset);
    part1_offset = (part1_offset&0xFFFF) * 1024;
    flash_stream_read(&flash, part1_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    if(!memcmp((char const*)signature, OTA_Signature, OTA_Signature_len)){
    	ota_offset = part1_offset;
    }

    flash_stream_read(&flash, FLASH_SYSTEM_DATA_ADDR, 4, (u8*)&part2_offset);
    flash_stream_read(&flash, part2_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    if(!memcmp((char const*)signature, OTA_Signature, OTA_Signature_len)){
    	ota_offset = part2_offset;
    }
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
    printf("\n\rOTA offset = 0x%08X", ota_offset);

    if(ota_offset < OTA_valid_offset){
		device_mutex_lock(RT_DEV_LOCK_FLASH);
    	flash_stream_read(&flash, ota_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    	signature[OTA_Signature_len] = '\0';
    	printf("\n\rSignature = %s", signature);
    	if(!memcmp((char const*)signature, OTA_Signature, OTA_Signature_len)){
    		memcpy((char*)signature, OTA_Clear, OTA_Signature_len);
    		flash_stream_write(&flash, ota_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    		flash_stream_read(&flash, ota_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    		signature[OTA_Signature_len] = '\0';
    		printf("\n\rSignature = %s", signature);
    		printf("\n\rClear OTA signature success.");
    	}
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
    }

}

void sys_recover_ota_signature(void)
{
    flash_t		flash;
    u32			ota_offset=0xFFFFFFFF, part1_offset, part2_offset;
    u8			signature[OTA_Signature_len+1];
    u8*			pbuf;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_stream_read(&flash, 0x18, 4, (u8*)&part1_offset);
    part1_offset = (part1_offset&0xFFFF) * 1024;
    flash_stream_read(&flash, part1_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    if(!memcmp((char const*)signature, OTA_Clear, OTA_Signature_len)){
    	ota_offset = part1_offset;
    }

    flash_stream_read(&flash, FLASH_SYSTEM_DATA_ADDR, 4, (u8*)&part2_offset);
    flash_stream_read(&flash, part2_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    if(!memcmp((char const*)signature, OTA_Clear, OTA_Signature_len)){
    	ota_offset = part2_offset;
    }
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
    printf("\n\rOTA offset = 0x%08X", ota_offset);

    if(ota_offset < OTA_valid_offset){
		device_mutex_lock(RT_DEV_LOCK_FLASH);
    	flash_stream_read(&flash, ota_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    	signature[OTA_Signature_len] = '\0';
    	printf("\n\rSignature = %s", signature);
    	if(!memcmp((char const*)signature, OTA_Clear, OTA_Signature_len)){
    		// backup
    		pbuf = RtlMalloc(FLASH_SECTOR_SIZE);
    		if(!pbuf) return;
    		flash_stream_read(&flash, ota_offset, FLASH_SECTOR_SIZE, pbuf);
    		memcpy((char*)pbuf+OTA_Signature_offset, OTA_Signature, OTA_Signature_len);
    		flash_erase_sector(&flash, FLASH_RESERVED_DATA_BASE);
    		flash_stream_write(&flash, FLASH_RESERVED_DATA_BASE, FLASH_SECTOR_SIZE, pbuf);
    		// Write
    		flash_stream_read(&flash, FLASH_RESERVED_DATA_BASE, FLASH_SECTOR_SIZE, pbuf);
    		flash_erase_sector(&flash, ota_offset);
    		flash_stream_write(&flash, ota_offset, FLASH_SECTOR_SIZE, pbuf);
    		flash_stream_read(&flash, ota_offset+OTA_Signature_offset, OTA_Signature_len, signature);
    		signature[OTA_Signature_len] = '\0';
    		printf("\n\rSignature = %s", signature);
    		RtlMfree(pbuf, FLASH_SECTOR_SIZE);
    		printf("\n\rRecover OTA signature success.");
    	}
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
    }

}

void sys_log_uart_on(void)
{
    HalInitLogUart();
}

void sys_log_uart_off(void)
{
    HalDeinitLogUart();
}

void sys_adc_calibration(u8 write, u16 *offset, u16 *gain)
{
    flash_t		flash;
    u8*			pbuf;

    if(write){
    	// backup
    	pbuf = RtlMalloc(FLASH_SECTOR_SIZE);
    	if(!pbuf) return;
		device_mutex_lock(RT_DEV_LOCK_FLASH);
    	flash_stream_read(&flash, FLASH_SYSTEM_DATA_ADDR, FLASH_SECTOR_SIZE, pbuf);
    	memcpy((char*)pbuf+FLASH_ADC_PARA_OFFSET, offset, 2);
    	memcpy((char*)pbuf+FLASH_ADC_PARA_OFFSET+2, gain, 2);
    	flash_erase_sector(&flash, FLASH_RESERVED_DATA_BASE);
    	flash_stream_write(&flash, FLASH_RESERVED_DATA_BASE, FLASH_SECTOR_SIZE, pbuf);
    	// Write
    	flash_stream_read(&flash, FLASH_RESERVED_DATA_BASE, FLASH_SECTOR_SIZE, pbuf);
    	flash_erase_sector(&flash, FLASH_SYSTEM_DATA_ADDR);
    	flash_stream_write(&flash, FLASH_SYSTEM_DATA_ADDR, FLASH_SECTOR_SIZE, pbuf);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
    	RtlMfree(pbuf, FLASH_SECTOR_SIZE);
    	printf("\n\rStore ADC calibration success.");
    }
	device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_stream_read(&flash, FLASH_ADC_PARA_BASE, 2, (u8*)offset);
    flash_stream_read(&flash, FLASH_ADC_PARA_BASE+2, 2, (u8*)gain);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

    printf("\n\rADC offset = 0x%04X, gain = 0x%04X.\n\r", *offset, *gain);
}

/**
 *  @brief   system software reset
 *
 *  @return  None
 *
 */
void sys_reset(void)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	HalDelayUs(100*1000);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
}

u8 sys_is_sdram_power_on(void)
{
	u8 ison = 0;

#if defined ( __ICCARM__ )
	ison = IsSdrPowerOn();
#endif

	return ison;
}

void sys_sdram_off(void)
{
#if defined ( __ICCARM__ )
	if (sys_is_sdram_power_on()) {
		SdrPowerOff();
	}
#endif
}
/******************************************************************************
 * @file    chip_info.h
 * @author
 * @version V1.0.0
 * @date    19-07-2013
 * @maintainer Halley
 * @brief   define all chip info
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __CHIP_INFO_H__
#define __CHIP_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//system stack depth in bytes
#define HZ				                (100)
#define CFG_SYS_STACK_SIZE				(1024)
/**
 * system memory addr definition
 */
#define	XMEM_ADDR			0x20000000
#define	VMEM_ADDR			0x20010000
#define PMEM_ADDR           0x20018000
#define XMEM_SIZE           (VMEM_ADDR-XMEM_ADDR)
#define VMEM_SIZE           (PMEM_ADDR-VMEM_ADDR)
#define PMEM_SIZE           (1024*32)
#define	HW_INBUF_SIZE		2560

#ifdef __cplusplus
}
#endif//__cplusplus

#endif /*__CHIP_INFO_H__*/


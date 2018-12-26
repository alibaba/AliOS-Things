/** *******************************************************************************
 * @file    cache.h
 * @author  Robert
 * @version V1.0.0
 * @date    24-July-2013
  *@maintainer lujiangang
 * @brief
 * Changelog:
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

#ifndef __CACHE_H__
#define __CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

enum
{
    CACHE_IOCTL_ENABLE_CACHE = 0x1,
    CACHE_IOCTL_DISABLE_CACHE,
    CACHE_IOCTL_4KBMEM_LOCK,
    CACHE_IOCTL_4KBMEM_UNLOCK,
};

/**
 * @brief
 *   The CacheInit() function initialize the cache
 *
 * @param	None
 *
 * @return	0 for ever
 * @note
 */
extern int32_t CacheInit(void);

/**
 * @brief
 *   The CacheDeInit() function disable the cache
 *
 * @param	None
 *
 * @return	None
 * @note
 */
extern void CacheDeInit(void);

/**
 * @brief
 *   The CacheIOctl() function control CACHE action including but not limited to configration,
 * cache enable,cache disable,inner 4KB resist or not,settings by the command of Cmd with parameter
 * referred by Arg.
 *
 * @param	Cmd	I/O control command
 * @param	Arg	the control command's argument if the command require
 *
 * @return
 *   Upon successful completion,CacheIOctl() function shall return the result like
 * CACHE_IOCTL_XXXX_YYYY(such as CACHE_IOCTL_ENABLE_CACHE) or return zero like
 * CACHE_IOCTL_ZZZZ_VVVV(such as CACHE_IOCTL_DISABLE_CACHE).
 *	Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ENOSYS The command is not available currently.
 * @note
 */
extern int32_t CacheIOctl(int32_t Cmd, uint32_t Opt);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //_CACHE_H_

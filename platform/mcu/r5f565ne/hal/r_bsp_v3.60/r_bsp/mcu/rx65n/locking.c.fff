/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : locking.c
* Description  : This implements a locking mechanism that can be used by all code. The locking is done atomically so
*                common resources can be accessed safely.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.10.2016 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Has intrinsic support. Includes xchg() which is used in this code. */
#include <machine.h>
/* Platform configuration. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_BSP_SoftwareLock
* Description  : Attempt to acquire the lock that has been sent in.
* Arguments    : plock -
*                    Pointer to lock structure with lock to try and acquire.
* Return Value : true -
*                    Lock was acquired.
*                false -
*                    Lock was not acquired.
***********************************************************************************************************************/
bool R_BSP_SoftwareLock (BSP_CFG_USER_LOCKING_TYPE * const plock)
{
#if BSP_CFG_USER_LOCKING_ENABLED == 0
    bool ret = false;

    /* Variable used in trying to acquire lock. Using the xchg instruction makes this atomic */
    int32_t is_locked = true;
    
    /* This example uses the RX MCU's atomic xchg() instruction. plock->lock is the lock we are trying to reserve. 
       The way this works is that 'is_locked' gets the value of the plock->lock and plock->lock gets the value of 
       'is_locked' which we just set to 'true'. Basically this is an atomic 'swap' command. If the lock had not yet been
       reserved then its value would be 'false' and after the xchg() instruction finished 'is_locked' would have 
       'false'. If it had already been reserved then 'is_locked' would have 'true' after the xchg() instruction. Since 
       plock->lock was already 'true' and we just set it back to 'true' everything is ok. To see if we reserved the lock
       we just need to check the value of 'is_locked' after this instruction finishes. */

    /* Try to acquire semaphore to obtain lock */
    xchg(&is_locked, &plock->lock);
    
    /* Check to see if semaphore was successfully taken */
    if (is_locked == false)
    {        
        /* Lock obtained, return success. */
        ret = true;
    }
    else
    {
        /* Lock was not obtained, another task already has it. */
    }

    return ret;   
#else
    /* User is going to handle the locking themselves. */
    return BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION(plock);
#endif
} /* End of function R_BSP_SoftwareLock() */

/***********************************************************************************************************************
* Function Name: R_BSP_SoftwareUnlock
* Description  : Release hold on lock.
* Arguments    : plock -
*                    Pointer to lock structure with lock to release.
* Return Value : true -
*                    Lock was released.
*                false -
*                    Lock was not released.
***********************************************************************************************************************/
bool R_BSP_SoftwareUnlock (BSP_CFG_USER_LOCKING_TYPE * const plock)
{
#if BSP_CFG_USER_LOCKING_ENABLED == 0
    /* Set lock back to unlocked. */
    plock->lock = false;

    return true;
#else
    /* User is going to handle the locking themselves. */
    return BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION(plock);
#endif
} /* End of function R_BSP_SoftwareUnlock() */


/***********************************************************************************************************************
* Function Name: R_BSP_HardwareLock
* Description  : Attempt to acquire the lock that has been sent in. This function takes in a peripheral index into the
*                array that holds hardware locks.
* Arguments    : hw_index -
*                    Index in locks array to the hardware resource to lock.
* Return Value : true -
*                    Lock was acquired.
*                false -
*                    Lock was not acquired.
***********************************************************************************************************************/
bool R_BSP_HardwareLock (mcu_lock_t const hw_index)
{
#if BSP_CFG_USER_LOCKING_ENABLED == 0
    /* Pass actual lock to software lock function. */
    return R_BSP_SoftwareLock(&g_bsp_Locks[hw_index]);
#else
    /* User is going to handle the locking themselves. */
    return BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION(hw_index);
#endif
} /* End of function R_BSP_HardwareLock() */

/***********************************************************************************************************************
* Function Name: R_BSP_HardwareUnlock
* Description  : Release hold on lock.
* Arguments    : hw_index -
*                    Index in locks array to the hardware resource to unlock.
* Return Value : true -
*                    Lock was released.
*                false -
*                    Lock was not released.
***********************************************************************************************************************/
bool R_BSP_HardwareUnlock (mcu_lock_t const hw_index)
{
#if BSP_CFG_USER_LOCKING_ENABLED == 0
    /* Pass actual lock to software unlock function. */
    return R_BSP_SoftwareUnlock(&g_bsp_Locks[hw_index]);
#else
    /* User is going to handle the locking themselves. */
    return BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION(hw_index);
#endif
} /* End of function R_BSP_HardwareUnlock() */

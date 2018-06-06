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



#ifndef __SXR_MUTX_H__
#define __SXR_MUTX_H__

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// sxr_MutexInit
// -----------------------------------------------------------------------------
/// Mutual exclusion data context initialization.
// =============================================================================
void  sxr_MutexInit (void);


// =============================================================================
// sxr_NewMutex
// -----------------------------------------------------------------------------
/// provide a free mutex.
/// @return mutex Id.
// =============================================================================
u8    sxr_NewMutex (void);
// =============================================================================
// sxr_FreeMutex
// -----------------------------------------------------------------------------
/// Free a previously allocated mutex.
/// @param Id mutex Id.
// =============================================================================
void  sxr_FreeMutex (u8 Id);

// =============================================================================
// sxr_TakeMutex
// -----------------------------------------------------------------------------
/// Take the mutex.
/// @param Id mutex Id.
/// @return UserId to give to sxr_ReleaseMutex()
// =============================================================================
u8    sxr_TakeMutex (u8 Id);

// =============================================================================
// sxr_ReleaseMutex
// -----------------------------------------------------------------------------
/// Release a mutex.
/// @param Id mutex Id.
/// @param UserId from sxr_TakeMutex()
// =============================================================================
void  sxr_ReleaseMutex (u8 Id, u8 UserId);

// =============================================================================
// sxr_MutexHot
// -----------------------------------------------------------------------------
/// Check if a mutex is free.
/// @param Id mutex Id.
/// @return \c TRUE when the mutex is free, FALSE when it is hot.
// =============================================================================
u8    sxr_MutexHot (u8 Id);

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutexes
// =============================================================================
void  sxr_CheckMutex (void);

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutex
/// @param Id mutex Id.
// =============================================================================
void  sxr_ChkMutex (u8 Id);


#ifdef __cplusplus
}
#endif

#endif
/// @} // <-- End of sx_mutex group


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


#ifndef _MCI_SOUND_RECORDER_H_
#define _MCI_SOUND_RECORDER_H_

#include "cs_types.h"


// =============================================================================
// mci_SndRecRecord
// -----------------------------------------------------------------------------
/// Start the recording of an audio file, at the specified format.
/// If the file already exists, it is appended. Otherwise, it is created.
/// If the format is not supported, an error is returned.
///
/// @return #MCI_ERR_NO when everything is fine,
/// #MCI_ERR_UNKNOWN_FORMAT when the format is neither recognized nor supported,
/// #MCI_ERR_CANNOT_OPEN_FILE when the file cannot be accessed and
/// #MCI_ERR_ACTION_NOT_ALLOWED when the record cannot be performed.
// =============================================================================
PUBLIC INT32 mci_SndRecRecord(CHAR* fileName, INT16 format);



// =============================================================================
// mci_SndRecStop
// -----------------------------------------------------------------------------
/// Stop a record in progress, or does nothing if no recording is occuring.
///
/// @return #MCI_ERR_NO is a recording was properly stopped, or no action
/// was needed. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecStop(VOID);



// =============================================================================
// mci_SndRecPause
// -----------------------------------------------------------------------------
/// Pause a recording in progress, or does nothing if no recording is currently
/// underway.
///
/// @return #MCI_ERR_NO is a recording was properly paused, or no action
/// was needed. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecPause(VOID);



// =============================================================================
// mci_SndRecResume
// -----------------------------------------------------------------------------
/// If a recording is in pause, resumes it.
///
/// @return #MCI_ERR_NO if the operation occured normaly, or if there was no
/// recording to resume. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecResume(VOID);




/// @}

#endif //_MCI_SOUND_RECORDER_H_


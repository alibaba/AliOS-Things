/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *   This file includes the OpenThread API for Dataset Updater module.
 */

#ifndef OPENTHREAD_DATASET_UPDATER_H_
#define OPENTHREAD_DATASET_UPDATER_H_

#include <openthread/dataset.h>
#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-operational-dataset
 *
 * @brief
 *   This module includes functions for Dataset Updater.
 *
 *   The functions in this module are available when Dataset Updater feature is enabled (i.e.
 *   `OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE` is set to 1). Further this feature is available only on an FTD build.
 *
 * @{
 *
 */

/**
 * This callback function pointer is called when a Dataset update request finishes, reporting success or failure status
 * of the Dataset update request.
 *
 * @param[in] aError   The error status.
 *                     OT_ERROR_NONE            indicates successful Dataset update.
 *                     OT_ERROR_INVALID_STATE   indicates failure due invalid state (MLE being disabled).
 *                     OT_ERROR_ALREADY         indicates failure due to another device within network requesting
 *                                              a conflicting Dataset update.
 *
 * @param[in] aContext A pointer to the arbitrary context (provided by user in `otDatasetUpdaterRequestUpdate()`).
 *
 */
typedef void (*otDatasetUpdaterCallback)(otError aError, void *aContext);

/**
 * This function requests an update to Operational Dataset.
 *
 * @p aDataset should contain the fields to be updated and their new value. It must not contain Active or Pending
 * Timestamp fields. The Delay field is optional, if not provided a default value (1000 ms) would be used.
 *
 * @param[in]  aInstance               A pointer to an OpenThread instance.
 * @param[in]  aDataset                A pointer to the Dataset containing the fields to change.
 * @param[in]  aCallback               A callback to indicate when Dataset update request finishes.
 * @param[in]  aContext                An arbitrary context passed to callback.
 *
 * @retval OT_ERROR_NONE           Dataset update started successfully (@p aCallback will be invoked on completion).
 * @retval OT_ERROR_INVALID_STATE  Device is disabled (MLE is disabled).
 * @retval OT_ERROR_INVALID_ARGS   The @p aDataset is not valid (contains Active or Pending Timestamp).
 * @retval OT_ERROR_BUSY           Cannot start update, a previous one is ongoing.
 * @retval OT_ERROR_NO_BUFS        Could not allocated buffer to save Dataset.
 *
 */
otError otDatasetUpdaterRequestUpdate(otInstance *                aInstance,
                                      const otOperationalDataset *aDataset,
                                      otDatasetUpdaterCallback    aCallback,
                                      void *                      aContext);

/**
 * This function cancels an ongoing (if any) Operational Dataset update request.
 *
 * @param[in]  aInstance         A pointer to an OpenThread instance.
 *
 */
void otDatasetUpdaterCancelUpdate(otInstance *aInstance);

/**
 * This function indicates whether there is an ongoing Operation Dataset update request.
 *
 * @param[in]  aInstance         A pointer to an OpenThread instance.
 *
 * @retval TRUE    There is an ongoing update.
 * @retval FALSE   There is no ongoing update.
 *
 */
bool otDatasetUpdaterIsUpdateOngoing(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_DATASET_UPDATER_H_

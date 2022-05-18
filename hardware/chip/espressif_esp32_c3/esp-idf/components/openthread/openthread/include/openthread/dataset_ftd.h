/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
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
 *  This file defines the OpenThread Operational Dataset API (FTD only).
 */

#ifndef OPENTHREAD_DATASET_FTD_H_
#define OPENTHREAD_DATASET_FTD_H_

#include <openthread/dataset.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-operational-dataset
 *
 * @{
 *
 */

/**
 * This method creates a new Operational Dataset to use when forming a new network.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[out] aDataset   The Operational Dataset.
 *
 * @retval OT_ERROR_NONE    Successfully created a new Operational Dataset.
 * @retval OT_ERROR_FAILED  Failed to generate random values for new parameters.
 *
 */
otError otDatasetCreateNewNetwork(otInstance *aInstance, otOperationalDataset *aDataset);

/**
 * Get minimal delay timer.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval the value of minimal delay timer (in ms).
 *
 */
uint32_t otDatasetGetDelayTimerMinimal(otInstance *aInstance);

/**
 * Set minimal delay timer.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance           A pointer to an OpenThread instance.
 * @param[in]  aDelayTimerMinimal  The value of minimal delay timer (in ms).
 *
 * @retval  OT_ERROR_NONE          Successfully set minimal delay timer.
 * @retval  OT_ERROR_INVALID_ARGS  If @p aDelayTimerMinimal is not valid.
 *
 */
otError otDatasetSetDelayTimerMinimal(otInstance *aInstance, uint32_t aDelayTimerMinimal);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_DATASET_FTD_H_

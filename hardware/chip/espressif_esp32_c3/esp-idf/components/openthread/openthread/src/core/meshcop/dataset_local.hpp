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
 *   This file includes definitions for managing MeshCoP Datasets.
 *
 */

#ifndef MESHCOP_DATASET_LOCAL_HPP_
#define MESHCOP_DATASET_LOCAL_HPP_

#include "openthread-core-config.h"

#include "common/locator.hpp"
#include "meshcop/dataset.hpp"
#include "meshcop/meshcop_tlvs.hpp"

namespace ot {
namespace MeshCoP {

class DatasetLocal : public InstanceLocator
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance  A reference to an OpenThread instance.
     * @param[in]  aType      The type of the dataset, active or pending.
     *
     */
    DatasetLocal(Instance &aInstance, Dataset::Type aType);

    /**
     * This method indicates whether this is an Active or Pending Dataset.
     *
     * @returns The Dataset type.
     *
     */
    Dataset::Type GetType(void) const { return mType; }

    /**
     * This method clears the Dataset.
     *
     */
    void Clear(void);

    /**
     * This method indicates whether an Active or Pending Dataset is saved in non-volatile memory.
     *
     * @retval TRUE  if an Active or Pending Dataset is saved in non-volatile memory.
     * @retval FALSE if an Active or Pending Dataset is not saved in non-volatile memory.
     *
     */
    bool IsSaved(void) const { return mSaved; }

    /**
     * This method indicates whether an Active (Pending) Timestamp is present in the Active (Pending) Dataset.
     *
     * @retval TRUE  if an Active/Pending Timestamp is present.
     * @retval FALSE if an Active/Pending Timestamp is not present.
     *
     */
    bool IsTimestampPresent(void) const { return mTimestampPresent; }

    /**
     * This method restores and retrieves the dataset from non-volatile memory.
     *
     * This method also sets the memory-cached timestamp for subsequent calls to `Compare()`.
     *
     * @param[out]  aDataset  Where to place the dataset.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Restore(Dataset &aDataset);

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDataset  Where to place the dataset.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(Dataset &aDataset) const;

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDatasetInfo  Where to place the dataset as `Dataset::Info`.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(Dataset::Info &aDatasetInfo) const;

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDataset  Where to place the dataset.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(otOperationalDatasetTlvs &aDataset) const;

    /**
     * This method returns the local time this dataset was last updated or restored.
     *
     * @returns The local time this dataset was last updated or restored.
     *
     */
    TimeMilli GetUpdateTime(void) const { return mUpdateTime; }

    /**
     * This method stores the dataset into non-volatile memory.
     *
     * @param[in] aDatasetInfo     The Dataset to save as `Dataset::Info`.
     *
     * @retval kErrorNone             Successfully saved the dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error Save(const Dataset::Info &aDatasetInfo);

    /**
     * This method stores the dataset into non-volatile memory.
     *
     * @param[in]  aDataset  The Dataset to save as `otOperationalDatasetTlvs`.
     *
     * @retval kErrorNone             Successfully saved the dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error Save(const otOperationalDatasetTlvs &aDataset);

    /**
     * This method stores the dataset into non-volatile memory.
     *
     * @param[in]  aDataset  The Dataset to save.
     *
     * @retval kErrorNone             Successfully saved the dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error Save(const Dataset &aDataset);

    /**
     * This method compares this dataset to another based on the timestamp.
     *
     * @param[in]  aCompare  A reference to the timestamp to compare.
     *
     * @retval -1  if @p aCompare is older than this dataset.
     * @retval  0  if @p aCompare is equal to this dataset.
     * @retval  1  if @p aCompare is newer than this dataset.
     *
     */
    int Compare(const Timestamp *aCompare);

private:
    bool IsActive(void) const { return (mType == Dataset::kActive); }
    void SetTimestamp(const Dataset &aDataset);

    Timestamp     mTimestamp;            ///< Active or Pending Timestamp
    TimeMilli     mUpdateTime;           ///< Local time last updated
    Dataset::Type mType;                 ///< Active or Pending
    bool          mTimestampPresent : 1; ///< Whether a timestamp is present
    bool          mSaved : 1;            ///< Whether a dataset is saved in non-volatile
};

} // namespace MeshCoP
} // namespace ot

#endif // MESHCOP_DATASET_LOCAL_HPP_

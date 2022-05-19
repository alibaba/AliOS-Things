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
 *   This file implements common methods for manipulating MeshCoP Datasets.
 *
 */

#include "dataset_local.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/settings.hpp"
#include "meshcop/dataset.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/mle_tlvs.hpp"

namespace ot {
namespace MeshCoP {

DatasetLocal::DatasetLocal(Instance &aInstance, Dataset::Type aType)
    : InstanceLocator(aInstance)
    , mUpdateTime(0)
    , mType(aType)
    , mTimestampPresent(false)
    , mSaved(false)
{
    mTimestamp.Clear();
}

void DatasetLocal::Clear(void)
{
    IgnoreError(Get<Settings>().DeleteOperationalDataset(IsActive()));
    mTimestamp.Clear();
    mTimestampPresent = false;
    mSaved            = false;
}

Error DatasetLocal::Restore(Dataset &aDataset)
{
    Error error;

    mTimestampPresent = false;

    error = Read(aDataset);
    SuccessOrExit(error);

    mSaved            = true;
    mTimestampPresent = (aDataset.GetTimestamp(mType, mTimestamp) == kErrorNone);

exit:
    return error;
}

Error DatasetLocal::Read(Dataset &aDataset) const
{
    DelayTimerTlv *delayTimer;
    uint32_t       elapsed;
    Error          error;

    error = Get<Settings>().ReadOperationalDataset(IsActive(), aDataset);
    VerifyOrExit(error == kErrorNone, aDataset.mLength = 0);

    if (mType == Dataset::kActive)
    {
        aDataset.RemoveTlv(Tlv::kPendingTimestamp);
        aDataset.RemoveTlv(Tlv::kDelayTimer);
    }
    else
    {
        delayTimer = aDataset.GetTlv<DelayTimerTlv>();
        VerifyOrExit(delayTimer);

        elapsed = TimerMilli::GetNow() - mUpdateTime;

        if (delayTimer->GetDelayTimer() > elapsed)
        {
            delayTimer->SetDelayTimer(delayTimer->GetDelayTimer() - elapsed);
        }
        else
        {
            delayTimer->SetDelayTimer(0);
        }
    }

    aDataset.mUpdateTime = TimerMilli::GetNow();

exit:
    return error;
}

Error DatasetLocal::Read(Dataset::Info &aDatasetInfo) const
{
    Dataset dataset;
    Error   error;

    aDatasetInfo.Clear();

    SuccessOrExit(error = Read(dataset));
    dataset.ConvertTo(aDatasetInfo);

exit:
    return error;
}

Error DatasetLocal::Read(otOperationalDatasetTlvs &aDataset) const
{
    Dataset dataset;
    Error   error;

    memset(&aDataset, 0, sizeof(aDataset));

    SuccessOrExit(error = Read(dataset));
    dataset.ConvertTo(aDataset);

exit:
    return error;
}

Error DatasetLocal::Save(const Dataset::Info &aDatasetInfo)
{
    Error   error;
    Dataset dataset;

    SuccessOrExit(error = dataset.SetFrom(aDatasetInfo));
    SuccessOrExit(error = Save(dataset));

exit:
    return error;
}

Error DatasetLocal::Save(const otOperationalDatasetTlvs &aDataset)
{
    Dataset dataset;

    dataset.SetFrom(aDataset);

    return Save(dataset);
}

Error DatasetLocal::Save(const Dataset &aDataset)
{
    Error error = kErrorNone;

    if (aDataset.GetSize() == 0)
    {
        // do not propagate error back
        IgnoreError(Get<Settings>().DeleteOperationalDataset(IsActive()));
        mSaved = false;
        otLogInfoMeshCoP("%s dataset deleted", Dataset::TypeToString(mType));
    }
    else
    {
        SuccessOrExit(error = Get<Settings>().SaveOperationalDataset(IsActive(), aDataset));
        mSaved = true;
        otLogInfoMeshCoP("%s dataset set", Dataset::TypeToString(mType));
    }

    mTimestampPresent = (aDataset.GetTimestamp(mType, mTimestamp) == kErrorNone);
    mUpdateTime       = TimerMilli::GetNow();

exit:
    return error;
}

int DatasetLocal::Compare(const Timestamp *aCompare)
{
    return (aCompare == nullptr) ? (!mTimestampPresent ? 0 : -1)
                                 : (!mTimestampPresent ? 1 : mTimestamp.Compare(*aCompare));
}

} // namespace MeshCoP
} // namespace ot

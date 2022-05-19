/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file implements a simple CLI for the Joiner role.
 */

#include "cli_joiner.hpp"

#include <inttypes.h>

#include "cli/cli.hpp"

#if OPENTHREAD_CONFIG_JOINER_ENABLE

namespace ot {
namespace Cli {

constexpr Joiner::Command Joiner::sCommands[];

otError Joiner::ProcessDiscerner(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;

    if (aArgs[0].IsEmpty())
    {
        const otJoinerDiscerner *discerner = otJoinerGetDiscerner(GetInstancePtr());

        VerifyOrExit(discerner != nullptr, error = OT_ERROR_NOT_FOUND);

        OutputLine("0x%llx/%u", static_cast<unsigned long long>(discerner->mValue), discerner->mLength);
        error = OT_ERROR_NONE;
    }
    else
    {
        otJoinerDiscerner discerner;

        memset(&discerner, 0, sizeof(discerner));

        if (aArgs[0] == "clear")
        {
            error = otJoinerSetDiscerner(GetInstancePtr(), nullptr);
        }
        else
        {
            VerifyOrExit(aArgs[1].IsEmpty());
            SuccessOrExit(Interpreter::ParseJoinerDiscerner(aArgs[0], discerner));
            error = otJoinerSetDiscerner(GetInstancePtr(), &discerner);
        }
    }

exit:
    return error;
}

otError Joiner::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError Joiner::ProcessId(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    OutputExtAddressLine(*otJoinerGetId(GetInstancePtr()));

    return OT_ERROR_NONE;
}

otError Joiner::ProcessStart(Arg aArgs[])
{
    otError error;

    VerifyOrExit(!aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    error = otJoinerStart(GetInstancePtr(),
                          aArgs[0].GetCString(),           // aPskd
                          aArgs[1].GetCString(),           // aProvisioningUrl (nullptr if aArgs[1] is empty)
                          PACKAGE_NAME,                    // aVendorName
                          OPENTHREAD_CONFIG_PLATFORM_INFO, // aVendorModel
                          PACKAGE_VERSION,                 // aVendorSwVersion
                          nullptr,                         // aVendorData
                          &Joiner::HandleCallback, this);

exit:
    return error;
}

otError Joiner::ProcessStop(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otJoinerStop(GetInstancePtr());

    return OT_ERROR_NONE;
}

otError Joiner::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_COMMAND;
    const Command *command;

    if (aArgs[0].IsEmpty())
    {
        IgnoreError(ProcessHelp(aArgs));
        ExitNow();
    }

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

void Joiner::HandleCallback(otError aError, void *aContext)
{
    static_cast<Joiner *>(aContext)->HandleCallback(aError);
}

void Joiner::HandleCallback(otError aError)
{
    switch (aError)
    {
    case OT_ERROR_NONE:
        OutputLine("Join success");
        break;

    default:
        OutputLine("Join failed [%s]", otThreadErrorToString(aError));
        break;
    }
}

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_JOINER_ENABLE

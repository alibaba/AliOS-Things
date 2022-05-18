/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file includes definitions for source address match controller.
 */

#ifndef SOURCE_MATCH_CONTROLLER_HPP_
#define SOURCE_MATCH_CONTROLLER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include "common/error.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"

namespace ot {

class Child;

/**
 * @addtogroup core-source-match-controller
 *
 * @brief
 *   This module includes definition for source address match controller.
 *
 * @{
 */

/**
 * This class implements the "source address match" controller.
 *
 * The source address match feature controls how the radio layer decides the "frame pending" bit for acks sent in
 * response to data request commands from sleepy children.
 *
 * This class updates the source match table and also controls when to enable or disable the source matching
 * feature.
 *
 * The source address match table provides the list of children for which there is a pending frame. Either a short
 * address or an extended/long address can be added to the source address match table.
 *
 */
class SourceMatchController : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance    A reference to the OpenThread instance
     *
     */
    explicit SourceMatchController(Instance &aInstance);

    /**
     * This method returns the current state of source address matching.
     *
     * @returns `true` if source address matching is enabled, `false` otherwise.
     *
     */
    bool IsEnabled(void) const { return mEnabled; }

    /**
     * This method increments the message count for a child and updates the source match table.
     *
     * @param[in] aChild    A reference to the child.
     *
     */
    void IncrementMessageCount(Child &aChild);

    /**
     * This method decrements the message count for a child and updates the source match table.
     *
     * @param[in] aChild    A reference to the child.
     *
     */
    void DecrementMessageCount(Child &aChild);

    /**
     * This method resets the message count for a child to zero and updates the source match table.
     *
     * @param[in] aChild    A reference to the child.
     *
     */
    void ResetMessageCount(Child &aChild);

    /**
     * This method sets whether or not to perform source address matching on the extended or short address for
     * a child.
     *
     * @param[in] aChild            A reference to the child.
     * @param[in] aUseShortAddress  `true` to match on short source address, `false` otherwise.
     *
     */
    void SetSrcMatchAsShort(Child &aChild, bool aUseShortAddress);

private:
    /**
     * This method clears the source match table.
     *
     */
    void ClearTable(void);

    /**
     * This method enables or disables the source matching.
     *
     * If enabled, the radio uses the source match table to determine whether to set or clear the "frame pending" bit
     * in an acknowledgment to a MAC Data Request command. If disabled, the radio layer sets the "frame pending" on all
     * acknowledgment frames in response to MAC Data Request commands.
     *
     * @param[in] aEnable   `true` to enable, `false` to disable.
     *
     */
    void Enable(bool aEnable);

    /**
     * This method adds an entry to source match table for a given child and updates the state of source matching
     * feature accordingly.
     *
     * If the entry is added successfully, source matching feature is enabled (if not already enabled) after ensuring
     * that there are no remaining pending entries. If the entry cannot be added (no space in source match table),
     * the child is marked to remember the pending entry and source matching is disabled.
     *
     * @param[in] aChild    A reference to the child.
     *
     */
    void AddEntry(Child &aChild);

    /**
     * This method clears an entry in source match table for a given child and updates the state of source matching
     * feature accordingly.
     *
     * If the entry is removed successfully and frees up space in the source match table, any remaining pending
     * entries are added. If all pending entries are successfully added, source matching is enabled.
     *
     * @param[in] aChild    A reference to the child.
     *
     */
    void ClearEntry(Child &aChild);

    /**
     * This method adds a given child's address (short or extended address depending on child's setting) to the source
     * source match table (@sa SetSrcMatchAsShort.
     *
     * @param[in] aChild            A reference to the child
     *
     * @retval kErrorNone     Child's address was added successfully to the source match table.
     * @retval kErrorNoBufs   No available space in the source match table.
     *
     */
    Error AddAddress(const Child &aChild);

    /**
     * This method adds all pending entries to the source match table.
     *
     * @retval kErrorNone     All pending entries were successfully added.
     * @retval kErrorNoBufs   No available space in the source match table.
     *
     */
    Error AddPendingEntries(void);

    bool mEnabled;
};

/**
 * @}
 *
 */

} // namespace ot

#endif // OPENTHREAD_FTD

#endif // SOURCE_MATCH_CONTROLLER_HPP_

/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for the SPI interface to radio (RCP).
 */

#ifndef OT_POSIX_PLATFORM_MAINLOOP_HPP_
#define OT_POSIX_PLATFORM_MAINLOOP_HPP_

#include <openthread/openthread-system.h>

namespace ot {
namespace Posix {
namespace Mainloop {

/**
 * This class is the base for all mainloop event sources.
 *
 */
class Source
{
    friend class Manager;

public:
    /**
     * This method registers events in the mainloop.
     *
     * @param[inout]    aContext    A reference to the mainloop context.
     *
     */
    virtual void Update(otSysMainloopContext &aContext) = 0;

    /**
     * This method processes the mainloop events.
     *
     * @param[in]   aContext    A reference to the mainloop context.
     *
     */
    virtual void Process(const otSysMainloopContext &aContext) = 0;

private:
    Source *mNext = nullptr;
};

/**
 * This class manages mainloop.
 *
 */
class Manager
{
public:
    /**
     * This method updates event polls in the mainloop context.
     *
     * @param[inout]    aContext    A reference to the mainloop context.
     *
     */
    void Update(otSysMainloopContext &aContext);

    /**
     * This method processes events in the mainloop context.
     *
     * @param[in]   aContext    A reference to the mainloop context.
     *
     */
    void Process(const otSysMainloopContext &aContext);

    /**
     * This method adds a new event source into the mainloop.
     *
     * @param[in]   aSource     A reference to the event source.
     *
     */
    void Add(Source &aSource);

    /**
     * This method removes an event source from the mainloop.
     *
     * @param[in]   aSource     A reference to the event source.
     *
     */
    void Remove(Source &aSource);

    /**
     * This function returns the Mainloop singleton.
     *
     * @returns A refernce to the Mainloop singleton.
     *
     */
    static Manager &Get(void);

private:
    Source *mSources = nullptr;
};

} // namespace Mainloop
} // namespace Posix
} // namespace ot
#endif // OT_POSIX_PLATFORM_MAINLOOP_HPP_

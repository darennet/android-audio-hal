/*
 * Copyright (C) 2013-2015 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "RouteManager/Route"

#include "AudioPort.hpp"
#include "AudioRoute.hpp"
#include <cutils/bitops.h>
#include <AudioCommsAssert.hpp>
#include <utilities/Log.hpp>

using std::string;
using audio_comms::utilities::Log;

namespace intel_audio
{

static uint32_t count[Direction::gNbDirections] = {
    0, 0
};

AudioRoute::AudioRoute(const string &name, bool isOut)
    : mName(name),
      mMask(1 << count[isOut]++),
      mIsOut(isOut),
      mIsUsed(false),
      mPreviouslyUsed(false),
      mIsApplicable(false)
{
    mRoutingStageRequested.reset();
    mPort[EPortSource] = NULL;
    mPort[EPortDest] = NULL;
}

AudioRoute::~AudioRoute()
{
}


void AudioRoute::addPort(AudioPort &port)
{
    Log::Verbose() << __FUNCTION__ << ": " << port.getName() << " to route " << getName();

    port.addRouteToPortUsers(*this);
    if (!mPort[EPortSource]) {

        mPort[EPortSource] = &port;
    } else {
        mPort[EPortDest] = &port;
    }
}

void AudioRoute::resetAvailability()
{
    mBlocked = false;
    mPreviouslyUsed = mIsUsed;
    mIsUsed = false;
}

void AudioRoute::prepare()
{
    if (isApplicable()) {
        setUsed();
    }
}

bool AudioRoute::isApplicable() const
{
    Log::Verbose() << __FUNCTION__ << ": " << getName()
                   << "!isBlocked()=" << !isBlocked() << " && _isApplicable=" << mIsApplicable;
    return !isBlocked() && !isUsed() && mIsApplicable;
}

void AudioRoute::setUsed()
{
    Log::Verbose() << __FUNCTION__ << ": route " << getName() << " is now in use in "
                   << (mIsOut ? "PLAYBACK" : "CAPTURE");
    mIsUsed = true;

    // Propagate the in use attribute to the ports
    // used by this route
    for (int i = 0; i < ENbPorts; i++) {

        if (mPort[i]) {

            mPort[i]->setUsed(*this);
        }
    }
}

void AudioRoute::setBlocked()
{
    if (!mBlocked) {
        Log::Verbose() << __FUNCTION__ << ": route " << getName() << " is now blocked";
        mBlocked = true;
    }
}

} // namespace intel_audio

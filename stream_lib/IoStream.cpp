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
#include "IoStream.hpp"
#include "AudioDevice.hpp"
#include <IStreamRoute.hpp>
#include <AudioCommsAssert.hpp>
#include <SampleSpec.hpp>
#include <utils/RWLock.h>
#include <utilities/Log.hpp>

using audio_comms::utilities::Log;
using std::string;

namespace intel_audio
{

bool IoStream::isRouted() const
{
    AutoR lock(mStreamLock);
    return isRoutedL();
}

bool IoStream::isRoutedL() const
{
    return mCurrentStreamRoute != NULL;
}

bool IoStream::isNewRouteAvailable() const
{
    AutoR lock(mStreamLock);
    return mNewStreamRoute != NULL;
}

android::status_t IoStream::attachRoute()
{
    AutoW lock(mStreamLock);
    return attachRouteL();
}


android::status_t IoStream::detachRoute()
{
    AutoW lock(mStreamLock);
    return detachRouteL();
}

android::status_t IoStream::attachRouteL()
{
    if (mNewStreamRoute == NULL) {
        Log::Error() << __FUNCTION__ << ": Invalid new stream route to attach";
        return android::BAD_VALUE;
    }
    setCurrentStreamRouteL(mNewStreamRoute);
    setRouteSampleSpecL(mCurrentStreamRoute->getSampleSpec());
    return android::OK;
}


android::status_t IoStream::detachRouteL()
{
    mCurrentStreamRoute = NULL;
    return android::OK;
}

void IoStream::addRequestedEffect(uint32_t effectId)
{
    mEffectsRequestedMask |= effectId;
}

void IoStream::removeRequestedEffect(uint32_t effectId)
{
    mEffectsRequestedMask &= ~effectId;
}

uint32_t IoStream::getOutputSilencePrologMs() const
{
    if (mCurrentStreamRoute == NULL) {
        Log::Warning() << __FUNCTION__ << ": called from invalid context(No route), returning 0 ms";
        return 0;
    }
    return mCurrentStreamRoute->getOutputSilencePrologMs();
}

android::status_t IoStream::setDevices(audio_devices_t devices)
{
    AutoW lock(mStreamLock);
    mDevices = devices;
    return android::OK;
}

void IoStream::resetNewStreamRoute()
{
    mNewStreamRoute = NULL;
}

void IoStream::setNewStreamRoute(IStreamRoute *newStreamRoute)
{
    mNewStreamRoute = newStreamRoute;
}

void IoStream::setCurrentStreamRouteL(IStreamRoute *currentStreamRoute)
{
    mCurrentStreamRoute = currentStreamRoute;
}

void IoStream::setRouteSampleSpecL(SampleSpec sampleSpec)
{
    mRouteSampleSpec = sampleSpec;
}

} // namespace intel_audio

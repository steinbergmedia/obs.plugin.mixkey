// This file is part of OBS MixKey Plug-in. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/obs.mixkey.plugin/LICENSE

#pragma once

#include <string>

namespace MixKey {

//------------------------------------------------------------------------
std::string findStreamDeviceId ();

// https://github.com/obsproject/obs-studio/blob/master/libobs/obs-audio.c#L52
const char* findAudioInputCaptureSourceId ();

//------------------------------------------------------------------------
} // namespace MixKey

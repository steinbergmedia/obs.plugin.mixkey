// This file is part of OBS MixKey Plug-in. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/obs.mixkey.plugin/LICENSE

#include "platform.h"
#include <string>
#include <string_view>

namespace MixKey {

//---------------------------------------------------------------
const char* kSteinbergMixKeyStreamDeviceId = "SteinbergMixKey:Stream";
const char* kAudioInputCaptureSourceId = "coreaudio_input_capture";

//---------------------------------------------------------------
std::string findStreamDeviceId ()
{
	return kSteinbergMixKeyStreamDeviceId;
}

//------------------------------------------------------------------------
const char* findAudioInputCaptureSourceId ()
{
	return kAudioInputCaptureSourceId;
}

//------------------------------------------------------------------------
} // namespace MixKey

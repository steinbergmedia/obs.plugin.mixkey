//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling_win.cpp
// Created by  : rhansen, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

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

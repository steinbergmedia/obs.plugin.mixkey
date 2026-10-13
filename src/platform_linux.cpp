//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling_win.cpp
// Created by  : rhansen, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "sourcehandling.h"
#include <string>

namespace MixKey {

//---------------------------------------------------------------
const char* kSteinbergMixKeyStreamDeviceId = "<no device yet>";
const char* kAudioInputCaptureSourceId = "pulse_output_capture";

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

//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : platform.h
// Created by  : rhansen, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

#include <string>

namespace MixKey {

//------------------------------------------------------------------------
std::string findStreamDeviceId ();

// https://github.com/obsproject/obs-studio/blob/master/libobs/obs-audio.c#L52
const char* findAudioInputCaptureSourceId ();

//------------------------------------------------------------------------
} // namespace MixKey

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

//---------------------------------------------------------------
std::string findStreamDeviceId ()
{
	return kSteinbergMixKeyStreamDeviceId;
}

//------------------------------------------------------------------------
} // namespace MixKey

//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

#include "obs.h"
#include <string>

namespace MixKey {

//------------------------------------------------------------------------
enum SetupStatus
{
	kOk,
	kUnclear,
	kNotOk
};

bool streamChannelSourceExists ();
void createStreamChannelSource ();
bool isStreamChannelSource (obs_source_t* source);
void removeBrokenStreamChannels ();

SetupStatus getCurrentSetupState ();

//------------------------------------------------------------------------
} // namespace MixKey

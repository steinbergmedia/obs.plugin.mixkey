//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

#include "obs.h"

namespace MixKey {

//------------------------------------------------------------------------
enum SetupStatus
{
	kOk,
	kUnclear,
	kNotOk
};

bool streamChannelExists ();
void createStreamChannelSource ();
SetupStatus getCurrentSetupState ();
bool isMixKeySource (obs_source_t* source);

//------------------------------------------------------------------------
} // namespace MixKey

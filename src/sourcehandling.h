// This file is part of OBS MixKey Plug-in. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/obs.mixkey.plugin/LICENSE

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

//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling.cpp
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "platform.h"
#include "sourcehandling.h"
#include "version.h"

#include <obs-module.h>

#include <string_view>

namespace MixKey {

//------------------------------------------------------------------------
const char* kSourceName = PLUGIN_NAME;
const char* kDeviceIdString = "device_id";

//------------------------------------------------------------------------
// https://github.com/obsproject/obs-studio/blob/master/libobs/obs-audio.c#L52
/**
    See also OBS source code
    UpdateAudioDevice(false, ui->desktopAudioDevice1, "Basic.DesktopDevice1", 1);
    UpdateAudioDevice(false, ui->desktopAudioDevice2, "Basic.DesktopDevice2", 2);
    UpdateAudioDevice(true, ui->auxAudioDevice1, "Basic.AuxDevice1", 3);
    UpdateAudioDevice(true, ui->auxAudioDevice2, "Basic.AuxDevice2", 4);
    UpdateAudioDevice(true, ui->auxAudioDevice3, "Basic.AuxDevice3", 5);
    UpdateAudioDevice(true, ui->auxAudioDevice4, "Basic.AuxDevice4", 6);
 */
enum BasicAudioDevice
{
	kNone = -1,
	kDesktopDevice1 = 1,
	kDesktopDevice2 = 2,
	kAuxDevice1 = 3,
	kAuxDevice2 = 4,
	kAuxDevice3 = 5,
	kAuxDevice4 = 6,

};

//------------------------------------------------------------------------
static int32_t getFirstFreeAuxDeviceSlot ()
{
	int32_t index = kNone;
	for (int32_t i = kAuxDevice1; i <= kAuxDevice4; ++i)
	{
		obs_source_t* source = obs_get_output_source (i);
		if (source)
		{
			obs_source_release (source);
			continue;
		}

		index = i;
		break;
	}

	return index;
}

//------------------------------------------------------------------------
static int32_t addToFirstFreeAuxChannel (obs_source_t* source)
{
	if (source == nullptr)
		return kNone;

	const int32_t index = getFirstFreeAuxDeviceSlot ();
	if (index == kNone)
	{
		blog (LOG_WARNING, "[MixKey] No free global Mic/Aux channel available");
		return kNone;
	}

	obs_set_output_source (index, source);
	blog (LOG_INFO, "[MixKey] Add source '%s' to global Mic/Aux channel %u",
	      obs_source_get_name (source), index);

	return index;
}

//------------------------------------------------------------------------
static obs_data_t* createSourceSettings ()
{
	obs_data_t* settings = obs_data_create ();
	obs_data_set_string (settings, kDeviceIdString, findStreamDeviceId ().c_str ());

	return settings;
}

//------------------------------------------------------------------------
bool streamChannelSourceExists ()
{
	bool found = false;

	for (int32_t i = kAuxDevice1; i <= kAuxDevice4; ++i)
	{
		obs_source_t* source = obs_get_output_source (i);
		if (source == nullptr)
			continue;

		obs_data_t* settings = obs_source_get_settings (source);
		if (settings == nullptr)
			continue;

		const std::string_view deviceId = obs_data_get_string (settings, kDeviceIdString);
		found = (deviceId == findStreamDeviceId ());

		obs_data_release (settings);
		obs_source_release (source);

		if (found)
			break;
	}

	return found;
}
//------------------------------------------------------------------------
SetupStatus getCurrentSetupState ()
{
	if (streamChannelSourceExists ())
		return kOk;
	else
		return kNotOk;
}

//------------------------------------------------------------------------
void createStreamChannelSource ()
{
	obs_data_t* settings = createSourceSettings ();
	obs_source_t* source =
	    obs_source_create (findAudioInputCaptureSourceId (), kSourceName, settings, nullptr);
	obs_data_release (settings);
	if (source == nullptr)
		return;

	addToFirstFreeAuxChannel (source);

	obs_source_release (source);
}

//------------------------------------------------------------------------
bool isStreamChannelSource (obs_source_t* source)
{
	if (!source)
		return false;

	obs_source_type type = obs_source_get_type (source);
	if (type != OBS_SOURCE_TYPE_INPUT)
		return false;

	obs_data_t* settings = obs_source_get_settings (source);
	if (settings == nullptr)
		return false;

	const std::string_view deviceId = obs_data_get_string (settings, kDeviceIdString);
	return deviceId == findStreamDeviceId ();
}

//------------------------------------------------------------------------
void removeBrokenStreamChannels ()
{
	for (uint32_t channel = kAuxDevice1; channel <= kAuxDevice4; ++channel)
	{
		obs_source_t* existingSource = obs_get_output_source (channel);
		if (existingSource)
		{
			const std::string_view name = obs_source_get_name (existingSource);
			obs_data_t* settings = obs_source_get_settings (existingSource);

			if (settings == nullptr)
				continue;

			const std::string_view deviceId = obs_data_get_string (settings, kDeviceIdString);

			if (deviceId == findStreamDeviceId () || name == kSourceName)
			{
				blog (LOG_INFO, "[MixKey] removing broken channel at index %u", channel);
				obs_set_output_source (channel, nullptr);
			}

			obs_data_release (settings);
			obs_source_release (existingSource);
		}
	}
}

//------------------------------------------------------------------------
}

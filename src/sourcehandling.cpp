//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling.cpp
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "sourcehandling.h"

#include <obs-module.h>

#include <string_view>

namespace MixKey {

//------------------------------------------------------------------------
const char* kSourceName = "MixKey";
const char* kDeviceIdString = "device_id";

//------------------------------------------------------------------------
// https://github.com/obsproject/obs-studio/blob/master/libobs/obs-audio.c#L52
#if defined(_WIN32)
const char* kAudioInputCaptureSourceId = "wasapi_input_capture";
#elif __APPLE__
const char* kAudioInputCaptureSourceId = "coreaudio_input_capture";
#elif (defined(__gnu_linux__) && __gnu_linux__) || (defined(__linux__) && __linux__)
const char* kAudioInputCaptureSourceId = "pulse_output_capture";
#endif

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
static void addStreamDeviceToSourceProperties (obs_source_t* source)
{
	obs_properties_t* props = obs_source_properties (source);
	if (props == nullptr)
		return;

	obs_property_t* prop = obs_properties_get (props, kDeviceIdString);
	if (prop == nullptr)
		return;

	if (obs_property_get_type (prop) == OBS_PROPERTY_LIST)
	{
		const size_t count = obs_property_list_item_count (prop);
		for (size_t i = 0; i < count; i++)
		{
			const char* name = obs_property_list_item_name (prop, i);
			const char* value = obs_property_list_item_string (prop, i);
			blog (LOG_INFO, "[SMTG] Source: name='%s'  value='%s'", name, value);

			if (std::string_view (value) != findStreamDeviceId ())
				continue;

			obs_data_t* settings = obs_source_get_settings (source);
			if (!settings)
				return;

			obs_data_set_string (settings, kDeviceIdString, value);
			obs_source_update (source, settings);
			obs_data_release (settings);
			break;
		}
	}

	obs_properties_destroy (props);
}
//------------------------------------------------------------------------
static bool enumSourcesCallback (void* /*data*/, obs_source_t* source)
{
	const char* name = obs_source_get_name (source);
	const char* id = obs_source_get_id (source);

	blog (LOG_INFO, "[SMTG] Source: name='%s'  id='%s'", name, id);
	return true;
}

//------------------------------------------------------------------------
static void enumSources ()
{
	obs_enum_sources (enumSourcesCallback, nullptr);
}

//------------------------------------------------------------------------
static void enumOutputSources ()
{
	for (int32_t channel = 0; channel < MAX_CHANNELS; ++channel)
	{
		obs_source_t* source = obs_get_output_source (channel);
		if (source == nullptr)
			continue;

		const char* name = obs_source_get_name (source);
		const char* id = obs_source_get_id (source);
		const char* uuid = obs_source_get_uuid (source);
		obs_source_type type = obs_source_get_type (source);
		if (type != OBS_SOURCE_TYPE_INPUT)
			continue;

		blog (LOG_INFO, "[SMTG] Source: name='%s'  id='%s' uuid='%s' type='%i'", name, id, uuid,
		      type);

		obs_source_release (source);
	}
}
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
	    obs_source_create (kAudioInputCaptureSourceId, kSourceName, settings, nullptr);
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

void removeBrokenStreamChannelCandidates ()
{
	for (uint32_t channel = kAuxDevice1; channel <= kAuxDevice4; ++channel)
	{
		obs_source_t* existingSource = obs_get_output_source (channel);
		if (existingSource)
		{
			const char* name = obs_source_get_name (existingSource);
			obs_data_t* settings = obs_source_get_settings (existingSource);

			const char* deviceId = obs_data_get_string (settings, "device_id");

			if (deviceId == findStreamDeviceId () || strcmp (name, "MixKey") == 0)
			{
				blog (LOG_INFO, "[MixKey] removing broken channel at index %u", channel);
				obs_set_output_source (channel, nullptr);
			}

			obs_data_release (settings);
		}
		obs_source_release (existingSource);
	}
}

//------------------------------------------------------------------------
}

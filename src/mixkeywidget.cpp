//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.cpp
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------
#include "mixkeywidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QProcess>
#include <QPushButton>
#include <QWidget>
#include <obs-defs.h>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>
#include <string_view>

namespace MixKey {

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
const char* kSourceName = "MixKey";
const char* kDeviceIdString = "device_id";

// https://github.com/obsproject/obs-studio/blob/master/libobs/obs-audio.c#L52
#if defined(_WIN32)
const char* kAudioInputCaptureSourceId = "wasapi_input_capture";
const char* kSteinbergMixKeyStreamDeviceId =
    "{0.0.1.00000000}.{0d18d47c-45ed-4ad2-ab39-7432ca979209}";
#elif __APPLE__
const char* kAudioInputCaptureSourceId = "coreaudio_input_capture";
const char* kSteinbergMixKeyStreamDeviceId = "SteinbergMixKey:Stream";
#elif (defined(__gnu_linux__) && __gnu_linux__) || (defined(__linux__) && __linux__)
const char* kAudioInputCaptureSourceId = "pulse_output_capture";
#endif

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

		obs_log (LOG_INFO, "[SMTG] Source: name='%s'  id='%s' uuid='%s' type='%i'", name, id, uuid,
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
		obs_source_t* existingSource = obs_get_output_source (i);
		if (existingSource)
		{
			obs_source_release (existingSource);
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
		obs_log (LOG_WARNING, "[SMTG] MixKey: No free global Mic/Aux channel available");
		return kNone;
	}

	obs_set_output_source (index, source);
	obs_log (LOG_INFO, "[SMTG] MixKey: Added source '%s' to global Mic/Aux channel %u",
	         obs_source_get_name (source), index);

	return index;
}

//------------------------------------------------------------------------
static bool streamChannelExists ()
{
	bool found = false;

	for (int32_t i = kAuxDevice1; i <= kAuxDevice4; ++i)
	{
		obs_source_t* existingSource = obs_get_output_source (i);
		if (existingSource == nullptr)
			continue;

		const std::string_view name = obs_source_get_name (existingSource);
		obs_data_t* settings = obs_source_get_settings (existingSource);
		if (settings == nullptr)
			continue;

		const std::string_view deviceId = obs_data_get_string (settings, kDeviceIdString);
		found = (deviceId == kSteinbergMixKeyStreamDeviceId) && (name == kSourceName);

		obs_data_release (settings);
		obs_source_release (existingSource);

		if (found)
			break;
	}

	return found;
}

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
			obs_log (LOG_INFO, "[SMTG] Source: name='%s'  value='%s'", name, value);

			if (std::string_view (value) != kSteinbergMixKeyStreamDeviceId)
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
static void addSource ()
{
	if (streamChannelExists ())
		return;

	obs_source_t* source = obs_source_create (kAudioInputCaptureSourceId, kSourceName, 0, nullptr);
	if (source == nullptr)
		return;

	addStreamDeviceToSourceProperties (source);
	addToFirstFreeAuxChannel (source);

	obs_source_release (source);
}

//------------------------------------------------------------------------
void* createDocWidget ()
{
	QWidget* dock = new QWidget ();
	QVBoxLayout* layout = new QVBoxLayout (dock);
	layout->setContentsMargins (6, 6, 6, 6);
	layout->setSpacing (0);
	layout->setAlignment (Qt::AlignTop);
	dock->setLayout (layout);

	QLabel* box = new QLabel ();
	box->setText ("SOURCE REQUIRED");
	box->setAlignment (Qt::AlignCenter);

	QPushButton* addButton = new QPushButton ();
	QPushButton::connect (addButton, &QPushButton::pressed, [] () {
		//
		addSource ();
	});

	addButton->setText ("Add MixKey Source");
	QIcon icon;
	icon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\Vector.svg"), QSize (10, 10));
	addButton->setIcon (icon);
	addButton->setIconSize (QSize (10, 10));

	QPushButton* startButton = new QPushButton ();
	QPushButton::connect (startButton, &QPushButton::pressed, [] () {});

	startButton->setText ("Start MixKey");
	QIcon startIcon;
	startIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\start.svg"), QSize (10, 10));
	startButton->setIcon (startIcon);
	startButton->setIconSize (QSize (10, 10));

	QPushButton* repairButton = new QPushButton ();
	QPushButton::connect (repairButton, &QPushButton::pressed, [] () {});
	repairButton->setText ("Repair MixKey Source");
	QIcon repairIcon;
	repairIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\repair.svg"), QSize (10, 10));
	repairButton->setIcon (repairIcon);
	repairButton->setIconSize (QSize (10, 10));

	QLabel* label =
	    new QLabel ("Click \"Add MixKey Source\" to add the MixKey your in Sources list.");
	label->setAlignment (Qt::AlignCenter);
	label->setWordWrap (true);

	layout->addWidget (box);
	layout->addWidget (label);
	layout->addWidget (addButton);
	layout->addWidget (startButton);
	layout->addWidget (repairButton);

	return (void*)dock;
}

//------------------------------------------------------------------------
} // namespace MixKey

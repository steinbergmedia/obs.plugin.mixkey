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
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>

namespace MixKey {

//------------------------------------------------------------------------
const char* kSteinbergMixKeyStreamDeviceId =
    "{0.0.1.00000000}.{0d18d47c-45ed-4ad2-ab39-7432ca979209}";

#if defined (_WIN32)
const char* kAudioInputCaptureSourceId = "wasapi_input_capture";
#elif __APPLE__
const char* kAudioInputCaptureSourceId = "coreaudio_input_capture";
#elif (defined (__gnu_linux__) && __gnu_linux__) || (defined (__linux__) && __linux__)
#error "Implement Me!"
#endif

static int32_t mixKeyChannelIndex = -1;

constexpr uint32_t firstMicAuxChannelIndex =
    3; // Obs' global Audio channels for Mic/Aux start at channelindex 3 and end at 6
constexpr uint32_t lastMicAuxChannelIndex = 6;

//------------------------------------------------------------------------
static int32_t addToFirstFreeMicAuxChannel (obs_source_t* source)
{
	if (!source)
		return -1;

	for (uint32_t channel = firstMicAuxChannelIndex; channel <= lastMicAuxChannelIndex; ++channel)
	{
		obs_source_t* existingSource = obs_get_output_source (channel);

		if (!existingSource)
		{
			obs_set_output_source (channel, source);

			obs_log (LOG_INFO, "MixKey: Added source '%s' to global Mic/Aux channel %u",
			         obs_source_get_name (source), channel);

			return channel;
		}
		obs_source_release (existingSource);
	}

	obs_log (LOG_WARNING, "MixKey: No free global Mic/Aux channel available");

	return -1;
}

//------------------------------------------------------------------------
static int32_t streamChannelExists ()
{
	bool found = false;

	for (uint32_t channel = firstMicAuxChannelIndex; channel <= lastMicAuxChannelIndex; ++channel)
	{
		obs_source_t* existingSource = obs_get_output_source (channel);
		if (existingSource)
		{
			const char* name = obs_source_get_name (existingSource);
			obs_data_t* settings = obs_source_get_settings (existingSource);

			const char* deviceId = obs_data_get_string (settings, "device_id");

			if (strcmp (deviceId, kSteinbergMixKeyStreamDeviceId) == 0 &&
			    strcmp (name, "MixKey") == 0)
			{
				found = true;
				mixKeyChannelIndex = channel;
			}

			obs_data_release (settings);
		}
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
	if (!props)
		return;
	obs_property_t* prop = obs_properties_get (props, "device_id");

	if (!prop)
		return;

	if (obs_property_get_type (prop) == OBS_PROPERTY_LIST)
	{
		size_t count = obs_property_list_item_count (prop);

		for (size_t i = 0; i < count; i++)
		{
			const char* name = obs_property_list_item_name (prop, i);
			const char* value = obs_property_list_item_string (prop, i);
			blog (LOG_INFO, "Source: name='%s'  value='%s'", name, value);

			if (QString (value) != kSteinbergMixKeyStreamDeviceId)
				continue;
			obs_data_t* settings = obs_source_get_settings (source);
			obs_data_set_string (settings, "device_id", value);
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

	obs_source_t* source = obs_source_create (kAudioInputCaptureSourceId, "MixKey", 0, nullptr);
	if (!source)
		return;

	addStreamDeviceToSourceProperties (source);
	mixKeyChannelIndex = addToFirstFreeMicAuxChannel (source);
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

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

namespace MixKey {

const char* kSteinbergMixKeyStreamDeviceId =
    "{0.0.1.00000000}.{0d18d47c-45ed-4ad2-ab39-7432ca979209}";
const char* kAudioInputCaptureSourceId = "wasapi_input_capture";
// const char* kAudioInputCaptureSourceId = "coreaudio_input_capture";

static bool enumSourcesCallback (void* data, obs_source_t* source)
{
	const char* name = obs_source_get_name (source);
	const char* id = obs_source_get_id (source);
	blog (LOG_INFO, "Source: name='%s'  id='%s'", name, id);
	return true;
}

static void inspectSourceProperties (obs_source_t* source)
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
static void addSource ()
{
	obs_source_t* source = obs_source_create (kAudioInputCaptureSourceId, "Mixkey", 0, nullptr);
	if (!source)
		return;

	inspectSourceProperties (source);
	obs_source_t* scene_source = obs_frontend_get_current_scene ();
	if (!scene_source)
		return;
	obs_scene_t* scene = obs_scene_from_source (scene_source);
	obs_scene_add (scene, source);
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
		obs_enum_sources (enumSourcesCallback, nullptr);
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
} // namespace MixKey

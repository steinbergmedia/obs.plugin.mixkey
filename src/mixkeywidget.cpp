//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.cpp
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------
#include "mixkeywidget.h"

#include "mixkeyipc.h"
#include "sourcehandling.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "obs.h"

namespace MixKey {

//------------------------------------------------------------------------
struct Context
{
	Client* client {nullptr};
	signal_handler_t* handler {nullptr};
};

static Context context;

//------------------------------------------------------------------------
static void addSource ()
{
	if (streamChannelExists ())
		return;

	createStreamChannelSource ();
	return;
}

//------------------------------------------------------------------------
static void addIcon (const QString& iconPath, QPushButton* button)
{
	if (iconPath.isEmpty () || button == nullptr)
		return;

	static const QSize kIconSize = QSize (10, 10);

	QIcon icon;
	icon.addFile (iconPath, kIconSize);
	button->setIcon (icon);
	button->setIconSize (kIconSize);
}

//------------------------------------------------------------------------
static QPushButton* createButton (const QString& text, const QString& iconPath,
                                  const QString& objectName)
{
	QPushButton* button = new QPushButton ();
	button->setText (text);
	button->setObjectName (objectName);

	addIcon (iconPath, button);

	return button;
}

//------------------------------------------------------------------------
static void onSourceCreate (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));

	if (!source)
		return;

	obs_source_type type = obs_source_get_type (source);
	if (type != OBS_SOURCE_TYPE_INPUT)
		return;

	const char* name = obs_source_get_name (source);
	const char* id = obs_source_get_id (source);
	const bool mixKeySourceFound = isMixKeySource (source);

	blog (LOG_INFO, "[SMTG] Source created: name='%s', id='%s' mixkey='%s' ", name ? name : "",
	      id ? id : "", mixKeySourceFound ? "yes" : "no");
}

//------------------------------------------------------------------------
static void onSourceUpdate (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!source)
		return;

	const obs_source_type type = obs_source_get_type (source);
	if (type != OBS_SOURCE_TYPE_INPUT)
		return;

	const char* name = obs_source_get_name (source);
	const char* id = obs_source_get_id (source);
	const bool mixKeySourceFound = isMixKeySource (source);

	blog (LOG_INFO, "[SMTG] Source updated: name='%s', id='%s' mixkey='%s' ", name ? name : "",
	      id ? id : "", mixKeySourceFound ? "yes" : "no");
}

//------------------------------------------------------------------------
static void onSourceDestroy (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!source)
		return;

	const obs_source_type type = obs_source_get_type (source);
	if (type != OBS_SOURCE_TYPE_INPUT)
		return;

	const char* name = obs_source_get_name (source);
	const char* id = obs_source_get_id (source);
	const bool mixKeySourceFound = isMixKeySource (source);

	blog (LOG_INFO, "[SMTG] Source destroyed: name='%s', id='%s' mixkey='%s' ", name ? name : "",
	      id ? id : "", mixKeySourceFound ? "yes" : "no");
}

//------------------------------------------------------------------------
static void onSourceRename (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!source)
		return;

	const obs_source_type type = obs_source_get_type (source);
	if (type != OBS_SOURCE_TYPE_INPUT)
		return;

	const char* new_name = calldata_string (cd, "new_name");
	const char* prev_name = calldata_string (cd, "prev_name");

	if (!new_name || !prev_name)
		return;

	const bool mixKeySourceFound = isMixKeySource (source);

	blog (LOG_INFO, "[SMTG] Source renamed: '%s' -> '%s' mixkey='%s'", prev_name, new_name,
	      mixKeySourceFound ? "yes" : "no");
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

	context.client = new Client (dock);

	QLabel* status = new QLabel ();
	status->setText ("Offline");
	status->setAlignment (Qt::AlignCenter);
	status->setStyleSheet ("color: red;");
	status->setFixedHeight (38);

	QPushButton* addButton = createButton (
	    "Add MixKey Source", "C:\\Users\\ABruehl\\Downloads\\Vector.svg", "addbutton");
	QPushButton::connect (addButton, &QPushButton::pressed, [] () { addSource (); });
	addButton->setDisabled (streamChannelExists ());

	QPushButton* startButton =
	    createButton ("Start MixKey", "C:\\Users\\ABruehl\\Downloads\\start.svg", "startbutton");

	QPushButton* repairButton = createButton (
	    "Repair MixKey Source", "C:\\Users\\ABruehl\\Downloads\\repair.svg", "repairbutton");

	layout->addWidget (status);
	layout->addWidget (addButton);
	layout->addWidget (startButton);
	layout->addWidget (repairButton);

	context.client->onConnectionStateChanged = [status] (const Client::State state) {
		switch (state)
		{
			case Client::State::kConnected:
			{
				status->setStyleSheet ("color: green;");
				status->setText ("Online");
				break;
			}
			case Client::State::kDisconnected:
			{
				status->setStyleSheet ("color: red;");
				status->setText ("Offline");
				break;
			}
		}
	};

	signal_handler_t* handler = obs_get_signal_handler ();
	if (handler)
	{
		context.handler = handler;

		signal_handler_connect (handler, "source_create", onSourceCreate, nullptr);
		signal_handler_connect (handler, "source_update", onSourceUpdate, nullptr);
		signal_handler_connect (handler, "source_destroy", onSourceDestroy, nullptr);
		signal_handler_connect (handler, "source_rename", onSourceRename, nullptr);
	}

	return (void*)dock;
}

//------------------------------------------------------------------------
void destroyDocWidget ()
{
	if (context.handler)
	{
		signal_handler_t* handler = context.handler;

		signal_handler_disconnect (handler, "source_create", onSourceCreate, nullptr);
		signal_handler_disconnect (handler, "source_update", onSourceUpdate, nullptr);
		signal_handler_disconnect (handler, "source_destroy", onSourceDestroy, nullptr);
		signal_handler_disconnect (handler, "source_rename", onSourceRename, nullptr);

		context.handler = nullptr;
	}

	context.client->onConnectionStateChanged = nullptr;
}

//------------------------------------------------------------------------
} // namespace MixKey

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
	if (streamChannelSourceExists ())
		return;

	createStreamChannelSource ();
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
	if (!isStreamChannelSource (source))
		return;

	blog (LOG_INFO, "[MixKey] Create source '%s'", obs_source_get_name (source));
}

//------------------------------------------------------------------------
static void onSourceUpdate (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!isStreamChannelSource (source))
		return;

	blog (LOG_INFO, "[MixKey] Update source '%s'", obs_source_get_name (source));
}

//------------------------------------------------------------------------
static void onSourceDestroy (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!isStreamChannelSource (source))
		return;

	blog (LOG_INFO, "[MixKey] Destroy source '%s'", obs_source_get_name (source));
}

//------------------------------------------------------------------------
static void onSourceRename (void* /*param*/, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!isStreamChannelSource (source))
		return;

	const char* newName = calldata_string (cd, "new_name");
	const char* oldName = calldata_string (cd, "prev_name");

	if (!newName || !oldName)
		return;

	blog (LOG_INFO, "[MixKey] Rename source: '%s' -> '%s'", oldName, newName);
}

//------------------------------------------------------------------------
static void connectSourceCallbacks (Context& context)
{
	signal_handler_t* handler = obs_get_signal_handler ();
	if (handler)
	{
		context.handler = handler;

		signal_handler_connect (handler, "source_create", onSourceCreate, &context);
		signal_handler_connect (handler, "source_update", onSourceUpdate, &context);
		signal_handler_connect (handler, "source_destroy", onSourceDestroy, &context);
		signal_handler_connect (handler, "source_rename", onSourceRename, &context);
	}
}

//------------------------------------------------------------------------
static void disconnectSourceCallbacks (Context& context)
{
	if (context.handler)
	{
		signal_handler_t* handler = context.handler;

		signal_handler_disconnect (handler, "source_create", onSourceCreate, &context);
		signal_handler_disconnect (handler, "source_update", onSourceUpdate, &context);
		signal_handler_disconnect (handler, "source_destroy", onSourceDestroy, &context);
		signal_handler_disconnect (handler, "source_rename", onSourceRename, &context);

		context.handler = nullptr;
	}
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

	QPushButton* addButton =
	    createButton ("Add MixKey Source", ":/mixkey/images/add.svg", "addbutton");
	QPushButton::connect (addButton, &QPushButton::pressed, [] () { addSource (); });

	QPushButton* startButton =
	    createButton ("Start MixKey", ":/mixkey/images/start.svg", "startbutton");

	QPushButton* repairButton =
	    createButton ("Repair MixKey Source", ":/mixkey/images/repair.svg", "repairbutton");

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

	connectSourceCallbacks (context);

	return (void*)dock;
}

//------------------------------------------------------------------------
void destroyDocWidget ()
{
	disconnectSourceCallbacks (context);

	context.client->onConnectionStateChanged = nullptr;
}

//------------------------------------------------------------------------
} // namespace MixKey

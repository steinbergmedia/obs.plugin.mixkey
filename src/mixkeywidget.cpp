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

#include "version.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QWidget>

#include "obs-frontend-api.h"
#include "obs.h"
#include <obs-module.h>

#include "plugin-support.h"

namespace MixKey {

//------------------------------------------------------------------------
static const char* kMixKeyAppName = PLUGIN_NAME;

//------------------------------------------------------------------------
struct Context
{
	Client* client {nullptr};
	signal_handler_t* handler {nullptr};
	obs_source_t* source {nullptr};

	QPointer<QPushButton> addBut;
	QPointer<QPushButton> startBut;
	QPointer<QPushButton> repairBut;
};

static Context context;

//------------------------------------------------------------------------
static void startMixKeyApp ()
{
#if defined(_WIN32)
	const QSettings settings (
	    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\MixKey.exe",
	    QSettings::NativeFormat);
	const QString location = settings.value ("Path").toString ();
	QProcess::startDetached (location);
#elif __APPLE__
	QProcess::startDetached ("open", {"-a", kMixKeyAppName});
#elif (defined(__gnu_linux__) && __gnu_linux__) || (defined(__linux__) && __linux__)
	QProcess::startDetached ("open", {kMixKeyAppName});
#endif
}

//------------------------------------------------------------------------
static void repairMixKeyConnection ()
{
	removeBrokenStreamChannels ();
	createStreamChannelSource ();
}

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
static QPushButton* createButton (const QString& textId, const QString& /*iconPath*/,
                                  const QString& objectName)
{
	QPushButton* button = new QPushButton ();
	button->setText (QString::fromUtf8 (obs_module_text (textId.toStdString ().c_str ())));
	button->setObjectName (objectName);

	// addIcon (iconPath, button);

	return button;
}

//------------------------------------------------------------------------
static QLabel* createStatus ()
{
	QLabel* status = new QLabel ();
	status->setText ("Offline");
	status->setAlignment (Qt::AlignCenter);
	status->setStyleSheet ("color: red;");
	status->setFixedHeight (38);

	return status;
}

//------------------------------------------------------------------------
static QVBoxLayout* createBoxLayout (QWidget* parent)
{
	auto layout = new QVBoxLayout (parent);
	layout->setContentsMargins (6, 6, 6, 6);
	layout->setSpacing (0);
	layout->setAlignment (Qt::AlignTop);

	if (parent)
		parent->setLayout (layout);

	return layout;
}

//------------------------------------------------------------------------
static void onSourceCreate (void* param, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!isStreamChannelSource (source))
		return;

	blog (LOG_INFO, "[MixKey] Create source '%s'", obs_source_get_name (source));

	auto* context = static_cast<Context*> (param);
	if (!context || !context->addBut)
		return;

	context->source = source;
	context->addBut->setDisabled (true);
	context->repairBut->setEnabled (true);
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
static void onSourceDestroy (void* param, calldata_t* cd)
{
	obs_source_t* source = static_cast<obs_source_t*> (calldata_ptr (cd, "source"));
	if (!isStreamChannelSource (source))
		return;

	blog (LOG_INFO, "[MixKey] Destroy source '%s'", obs_source_get_name (source));

	auto* context = static_cast<Context*> (param);
	if (!context || !context->addBut)
		return;

	if (context->source != source)
		return;

	context->source = nullptr;
	context->addBut->setEnabled (true);
	context->repairBut->setDisabled (true);
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
static void onFrontendEvent (obs_frontend_event event, void* param)
{
	if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING)
	{
		auto* context = static_cast<Context*> (param);
		if (!context)
			return;

		blog (LOG_INFO, "[MixKey] OBS Frontend finished loading");
	}
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
QWidget* createDocWidget ()
{
	QWidget* dock = new QWidget ();
	QVBoxLayout* layout = createBoxLayout (dock);
	obs_frontend_add_event_callback (onFrontendEvent, &context);
	context.client = new Client (dock);

	context.addBut = createButton ("MixKey.AddButtonText", ":/mixkey/images/add.svg", "addbutton");
	QPushButton::connect (context.addBut, &QPushButton::pressed, [] () { addSource (); });

	context.startBut =
	    createButton ("MixKey.StartButton", ":/mixkey/images/start.svg", "startbutton");
	QPushButton::connect (context.startBut, &QPushButton::pressed, [] () { startMixKeyApp (); });

	context.repairBut =
	    createButton ("MixKey.RepairButton", ":/mixkey/images/repair.svg", "repairbutton");
	QPushButton::connect (context.repairBut, &QPushButton::pressed,
	                      [] () { repairMixKeyConnection (); });
	context.repairBut->setDisabled (true);

	QPointer<QPushButton> startBut = context.startBut;
	QLabel* status = createStatus ();
	context.client->onConnectionStateChanged = [status, startBut] (const Client::State state) {
		switch (state)
		{
			case Client::State::kConnected:
			{
				status->setStyleSheet ("color: green;");
				status->setText ("Online");
				if (startBut)
					startBut->setDisabled (true);
				break;
			}
			case Client::State::kDisconnected:
			{
				status->setStyleSheet ("color: red;");
				status->setText ("Offline");
				if (startBut)
					startBut->setEnabled (true);
				break;
			}
		}
	};

	layout->addWidget (status);
	layout->addWidget (context.addBut);
	layout->addWidget (context.startBut);
	layout->addWidget (context.repairBut);

	connectSourceCallbacks (context);

	return dock;
}

//------------------------------------------------------------------------
void destroyDocWidget ()
{
	disconnectSourceCallbacks (context);
	obs_frontend_remove_event_callback (onFrontendEvent, &context);
	context.client->onConnectionStateChanged = nullptr;
}

//------------------------------------------------------------------------
} // namespace MixKey

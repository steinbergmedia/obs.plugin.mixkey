//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeyipc.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

#include "mixkeyipc.h"
#include "sourcehandling.h"

//------------------------------------------------------------------------
#include <QDockWidget>
#include <QTimer>
#include <QWidget>

namespace MixKey {

//------------------------------------------------------------------------
#define kMixKeyOBSIpcId "MixKeyOBSIpc"
#define kMKDockId "8B9019FE-1225-4D12-9BA8-91F2FC9FFB6C"

constexpr int32_t kReconnectInterval = 2000;
constexpr int32_t kMaxMessageLength = 10;

static void showDock ()
{
	QWidget* mainWindow = static_cast<QWidget*> (obs_frontend_get_main_window ());
	if (!mainWindow)
		return;

	QMetaObject::invokeMethod (mainWindow,
	                           [mainWindow] {
		                           // show window if needed
		                           auto* dock = mainWindow->findChild<QDockWidget*> (kMKDockId);
		                           if (dock && !dock->isVisible ())
		                           {
			                           QAction* action = dock->toggleViewAction ();
			                           if (action)
				                           action->trigger ();
		                           }
	                           },
	                           Qt::QueuedConnection);
}

//------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------
Client::Client (QObject* object) : QObject (object)
{
	initSocket ();
	initConnectionTimer ();
}

//------------------------------------------------------------------------
Client::~Client ()
{
	delete socket;
	delete timer;
}

//------------------------------------------------------------------------
void Client::initSocket ()
{
	socket = new QLocalSocket (this);

	connect (socket, &QLocalSocket::connected, this, [this] () {
		qDebug ("[MixKey] Connected to MixKey.");

		if (onConnectionStateChanged)
			onConnectionStateChanged (State::kConnected);

		socket->write (QString ("Hey, OBS here!").toUtf8 ());
		socket->flush ();
	});

	connect (socket, &QLocalSocket::disconnected, this, [this] () {
		qDebug ("[MixKey] Disconnected from MixKey.");

		if (onConnectionStateChanged)
			onConnectionStateChanged (State::kDisconnected);

		timer->start ();
	});

	connect (socket, &QLocalSocket::errorOccurred, this, [this] (QLocalSocket::LocalSocketError) {
		// qDebug ("[MixKey] Socket error occurred. Try again to connect to MixKey.");
		timer->start ();
	});

	connect (socket, &QLocalSocket::readyRead, this, [this] () {
		QString msg = QString::fromUtf8 (socket->readAll ());

		if (msg == QString ("status"))
		{
			if (getCurrentSetupState () == kOk)
			{
				socket->write ("ok", kMaxMessageLength);
				socket->flush ();
			}
			else
			{
				socket->write ("notok", kMaxMessageLength);
				socket->flush ();
			}
		}
		else if (msg == QString ("showdock"))
		{
			showDock ();
		}

	});
}

//------------------------------------------------------------------------
void Client::initConnectionTimer ()
{
	timer = new QTimer (this);
	timer->setInterval (kReconnectInterval);
	timer->setSingleShot (true);

	connect (timer, &QTimer::timeout, this,
	         [this] () { //
		         socket->connectToServer (kMixKeyOBSIpcId);
	         });

	timer->start ();
}

//------------------------------------------------------------------------
} // namespace MixKey

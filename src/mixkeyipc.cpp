//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeyipc.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#include "mixkeyipc.h"
#include "defines.h"

#include "mixkeywidget.h"
#include "sourcehandling.h"

//------------------------------------------------------------------------
#include <QDockWidget>
#include <QTimer>
#include <QWidget>

namespace MixKey {

//------------------------------------------------------------------------
#define kMixKeyOBSIpcId "MixKeyOBSIpc"

constexpr int32_t kReconnectInterval = 2000;
constexpr int32_t kMaxMessageLength = 10;

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

		if (msg == QString (kIPCMessageStatus))
		{
			if (getCurrentSetupState () == kOk)
				socket->write (kIPCMessageStatusOk, kMaxMessageLength);
			else
				socket->write (kIPCMessageStatusNotOk, kMaxMessageLength);

			socket->flush ();
		}
		else if (msg == QString (kIPCMessageShowDock))
			showDockWidget ();
		else if (msg == QString (kIPCMessageRaiseMainWindow))
			showMainWindow ();
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

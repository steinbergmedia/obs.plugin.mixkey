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
constexpr int32_t kMaxMessageLength = 30;

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
			updateStatus ();
		}
		else if (msg == QString (kIPCMessageShowDock))
			showDockWidget ();
		else if (msg == QString (kIPCMessageRaiseMainWindow))
			showMainWindow ();
		}
		else if (msg == kIPCMessageRepair)
		{
			removeBrokenStreamChannels ();
			createStreamChannelSource ();
			QTimer::singleShot (1000, [this] () { updateStatus (); });
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
void Client::updateStatus ()
{
	if (getCurrentSetupState () == kOk)
		sendMessage (kIPCMessageStatusOk);
	else
		sendMessage (kIPCMessageStatusNotOk);
}

//------------------------------------------------------------------------
void Client::sendMessage (const QByteArray& data)
{
	QMetaObject::invokeMethod (socket,
	                           [this, data] () {
		                           if (socket->state () == QLocalSocket::ConnectedState)
		                           {
			                           socket->write (data, kMaxMessageLength);
		                           }
	                           },
	                           Qt::QueuedConnection);
}

//------------------------------------------------------------------------
} // namespace MixKey

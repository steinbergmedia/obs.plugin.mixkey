//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeyipc.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "mixkeyipc.h"
#include "sourcehandling.h"

#include <QTimer>

namespace MixKey {

#define kMixKeyOBSIpcId "MixKeyOBSIpc"
constexpr int32_t kReconnectingInterval = 2000;
constexpr int32_t kMessageLengthMaximum = 10;

Client::Client (QObject* object) : QObject (object)
{
	socket = new QLocalSocket (this);

	connect (socket, &QLocalSocket::connected, this, [this] () {
		qDebug ("Connected to MixKey.");
		timer->stop ();
	});

	connect (socket, &QLocalSocket::disconnected, this, [this] () { timer->start (); });

	connect (socket, &QLocalSocket::errorOccurred, this,
	         [this] (QLocalSocket::LocalSocketError) { timer->stop (); });

	connect (socket, &QLocalSocket::readyRead, this, [this] () {
		QString msg = QString::fromUtf8 (socket->readAll ());

		QStringList parts = msg.split ("|");
		if (parts.size () == 2)
		{
			QString msgContent = parts.last ();

			if (msgContent == QString ("status"))
			{
				if (getCurrentSetupState () == kOk)
				{
					socket->write ("ok", kMessageLengthMaximum);
					socket->flush ();
				}
				else
				{
					socket->write ("notok", kMessageLengthMaximum);
					socket->flush ();
				}
			}
		}
	});

	timer = new QTimer (this);
	timer->setInterval (kReconnectingInterval);

	connect (timer, &QTimer::timeout, this,
	         [this] () { //
		         this->socket->connectToServer (kMixKeyOBSIpcId);
	         });
	timer->start ();
}

// -----------------------------------------------------------------------------
Client::~Client ()
{
	delete socket;
	delete timer;
}
}

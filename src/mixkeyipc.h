//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeyipc.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

#include <QLocalSocket>
#include <QTimer>
#include <functional>

namespace MixKey {

//------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------
class Client : public QObject
{
public:
//------------------------------------------------------------------------
	enum class State
	{
		kDisconnected,
		kConnected
	};

	using FnOnConnectionState = std::function<void (State)>;
	void updateStatus ();

	Client (QObject* object);
	~Client () override;

	FnOnConnectionState onConnectionStateChanged;

//------------------------------------------------------------------------
protected:
	void initSocket ();
	void initConnectionTimer ();
	void sendMessage (const QByteArray& data);

	QLocalSocket* socket {nullptr};
	QTimer* timer {nullptr};
};

//------------------------------------------------------------------------
} // namespace MixKey

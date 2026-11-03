// This file is part of OBS MixKey Plug-in. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/obs.mixkey.plugin/LICENSE

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

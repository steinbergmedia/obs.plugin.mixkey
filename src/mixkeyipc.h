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

namespace MixKey {

//------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------
class Client : public QObject
{
public:
//------------------------------------------------------------------------
	Client (QObject* object);
	~Client () override;

//------------------------------------------------------------------------
protected:
	void initSocket ();
	void initConnectionTimer ();

	QLocalSocket* socket {nullptr};
	QTimer* timer {nullptr};
};

//------------------------------------------------------------------------
} // namespace MixKey

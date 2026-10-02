//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeyipc.h
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

#include <QLocalSocket>

namespace MixKey {

// -----------------------------------------------------------------------------
class Client : public QObject
{
public:
	Client (QObject* object);
	~Client ();

protected:
	QLocalSocket* socket;
	QTimer* timer;
};
}

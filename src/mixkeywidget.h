//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.h
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------

#pragma once

class QWidget;

namespace MixKey {

//------------------------------------------------------------------------
QWidget* createDockWidget ();
void destroyDockWidget ();
void showDockWidget ();
//------------------------------------------------------------------------
} // namespace MixKey

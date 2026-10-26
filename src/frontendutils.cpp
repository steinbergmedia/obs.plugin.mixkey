//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling.cpp
// Created by  : abruehl, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "frontendutils.h"
#include "defines.h"

#include <obs-frontend-api.h>
#include <obs-module.h>

#include <QDockWidget>
#include <QWidget>

namespace MixKey {

void showDock ()
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
}

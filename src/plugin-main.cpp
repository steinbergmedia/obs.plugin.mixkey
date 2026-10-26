/*
MixKey
Copyright (C) 2026-present Steinberg Media Technologies GmbH

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>

//------------------------------------------------------------------------
#include "mixkeywidget.h"
#include "version.h"
#include "defines.h"

//------------------------------------------------------------------------
#include <QWidget>
#include <QDockWidget>

//------------------------------------------------------------------------
#define kMKFirstShown "MKFirstShown"

//------------------------------------------------------------------------
OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

//------------------------------------------------------------------------
void frontendCallback (obs_frontend_event event, void *)
{
	if (event != OBS_FRONTEND_EVENT_FINISHED_LOADING)
		return;

	// remove callback
	obs_frontend_remove_event_callback (frontendCallback, nullptr);

	// save info that it was shown
	config_t* config = obs_frontend_get_user_config ();
	if (config)
		config_set_bool (config, PLUGIN_NAME, kMKFirstShown, true);

	MixKey::showDockWidget ();
}

//------------------------------------------------------------------------
bool obs_module_load (void)
{
	QWidget* dock = MixKey::createDockWidget ();
	if (!dock)
		return false;

	static const QString kMixKeyDockTitle = QString::fromUtf8 (obs_module_text ("MixKey.SetupText"));
		
	if (!obs_frontend_add_dock_by_id (kMKDockId, kMixKeyDockTitle.toUtf8 (), dock))
	{
		MixKey::destroyDockWidget ();
		delete dock;
		return false;
	}

	bool addFirstShow = true;
	config_t* config = obs_frontend_get_user_config ();
	if (config)
		addFirstShow = !config_get_bool (config, PLUGIN_NAME, kMKFirstShown);
	if (addFirstShow)
		obs_frontend_add_event_callback (frontendCallback, nullptr);
	
	blog (LOG_INFO, "[MixKey] Plugin loaded successfully (version %s)", PLUGIN_VERSION);

	return true;
}

//------------------------------------------------------------------------
void obs_module_unload (void)
{
	MixKey::destroyDockWidget ();
	
	blog (LOG_INFO, "[MixKey] Plugin unloaded");
}

//------------------------------------------------------------------------
MODULE_EXPORT const char* obs_module_name(void)
{
	return PLUGIN_NAME " (" PLUGIN_VERSION ")";
}

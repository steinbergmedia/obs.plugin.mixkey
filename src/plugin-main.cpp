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

#include <obs-module.h>
#include <plugin-support.h>
#include "mixkeywidget.h"
#include "version.h"
#include <string>

#include <QWidget>

#include <obs-frontend-api.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	blog (LOG_INFO, "[MixKey] Plugin loaded successfully (version %s)", PLUGIN_VERSION);

	const std::string kMixKeyDockTitle = std::string(PLUGIN_NAME) + " Setup";
	static const char* kMixKeyDockId = "MK2347865342876432";
	
	void* dock = MixKey::createDocWidget ();
	obs_frontend_add_dock_by_id (kMixKeyDockId, kMixKeyDockTitle.c_str (), dock);

	return true;
}

void obs_module_unload(void)
{
	MixKey::destroyDocWidget ();
	
	blog (LOG_INFO, "[MixKey] Plugin unloaded");
}

MODULE_EXPORT const char *obs_module_name(void)
{
	static const std::string name {PLUGIN_NAME};
	static const std::string vers {PLUGIN_VERSION};
	static const std::string concat = name + " (" + vers + ")";
	
	return concat.c_str ();
}

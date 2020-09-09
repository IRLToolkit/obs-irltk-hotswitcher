#pragma once

#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#include "plugin-main.h"

class Config {
	public:
		Config();
		~Config();
		void Load();
		void Save();
		void SetDefaults();
		config_t* GetConfigStore();

        QString APIKey;

		bool SettingsLoaded;

	private:
		static void OnFrontendEvent(enum obs_frontend_event event, void* param);
};
